#include "tf_synthesizer.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#endif

#include "tensorflow/cc/ops/array_ops.h"
#include "tensorflow/cc/ops/const_op.h"
#include "tensorflow/cc/ops/image_ops.h"
#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/graph/default_device.h"
#include "tensorflow/core/graph/graph_def_builder.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/stringpiece.h"
#include "tensorflow/core/lib/core/threadpool.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/lib/strings/stringprintf.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/platform/init_main.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/util/command_line_flags.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <chrono>

using namespace tensorflow;
using namespace tensorflow::ops;

namespace tts {

namespace {

// Reads a model graph definition from disk, and creates a session object you
// can use to run it.
Status LoadGraph(const string& graph_file_name,
                 std::unique_ptr<tensorflow::Session>* session) {
  tensorflow::GraphDef graph_def;
  Status load_graph_status =
      ReadBinaryProto(tensorflow::Env::Default(), graph_file_name, &graph_def);
  if (!load_graph_status.ok()) {
    return tensorflow::errors::NotFound("Failed to load compute graph at '",
                                        graph_file_name, "'");
  }
  session->reset(tensorflow::NewSession(tensorflow::SessionOptions()));
  Status session_create_status = (*session)->Create(graph_def);
  if (!session_create_status.ok()) {
    return session_create_status;
  }
  return Status::OK();
}

} // anonymous namespace

class TensorflowSynthesizer::Impl {
public:
  void init(int argc, char* argv[]) {
    // We need to call this to set up global state for TensorFlow.
    tensorflow::port::InitMain(argv[0], &argc, &argv);
  }

  bool load(const std::string& graph_filename, const std::string& inp_layer,
            const std::string& out_layer) {
    // First we load and initialize the model.
    Status load_graph_status = LoadGraph(graph_filename, &session);
    if (!load_graph_status.ok()) {
      std::cerr << load_graph_status;
      return false;
    }

    input_layer = inp_layer;
    output_layer = out_layer;

    return true;
  }

  bool synthesize(const std::vector<int32_t>& input_sequence, const std::vector<int32_t>& input_lengths) {

    // Batch size = 1 for a while
    int N = 1;
    

    int input_length = int(input_sequence.size()); 
    Tensor input_tensor(DT_INT32, {N, input_length});

    std::copy_n(input_sequence.data(), input_sequence.size(),
                input_tensor.flat<int32_t>().data());

    Tensor input_lengths_tensor(DT_INT32, {N});

    *(input_lengths_tensor.flat<int32_t>().data()) = input_length;

     auto startT = std::chrono::system_clock::now();


    // Run
    std::vector<Tensor> output_tensors;
    Status run_status = session->Run({{input_layer, input_tensor}, {"input_lengths", input_lengths_tensor}},
                                     {output_layer}, {}, &output_tensors);
    if (!run_status.ok()) {
      std::cerr << "Running model failed: " << run_status;
      return false;
    }

    auto endT = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> ms = endT - startT;

    std::cout << "Synth time : " << ms.count() << " [ms]" << std::endl;

    const Tensor& output_tensor = output_tensors[0];
    std::cout << "output dim " << output_tensor.dims() << std::endl;

    TTypes<float, 1>::ConstTensor tensor = output_tensor.tensor<float, 1>();
    std::cout << "len = " << tensor.dimension(0) << std::endl;

#if 0
    // Copy to output image
    assert(tensor.dimension(0) == 1);
    size_t out_height = static_cast<size_t>(tensor.dimension(1));
    size_t out_width = static_cast<size_t>(tensor.dimension(2));
    size_t out_channels = static_cast<size_t>(tensor.dimension(3));
    out_img.create(out_width, out_height, out_channels);
    out_img.foreach([&](int x, int y, int c, float& v) {
      v = tensor(0, y, x, c);
    });
#endif

    return true;
  }

private:
  std::unique_ptr<tensorflow::Session> session;
  std::string input_layer, output_layer;
};

// PImpl pattern
TensorflowSynthesizer::TensorflowSynthesizer() : impl(new Impl()) {}
TensorflowSynthesizer::~TensorflowSynthesizer() {}
void TensorflowSynthesizer::init(int argc, char* argv[]) {
  impl->init(argc, argv);
}
bool TensorflowSynthesizer::load(const std::string& graph_filename,
                               const std::string& inp_layer,
                               const std::string& out_layer) {
  return impl->load(graph_filename, inp_layer, out_layer);
}

bool TensorflowSynthesizer::synthesize(const std::vector<int32_t> &input_sequence, const std::vector<int32_t> &input_lengths) {
  return impl->synthesize(input_sequence, input_lengths);
}



} // namespace tts
