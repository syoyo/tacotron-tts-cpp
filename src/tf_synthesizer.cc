#include <chrono>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "tf_synthesizer.h"

namespace tts {

void free_buffer(void *data, size_t length) {
  (void)length;
  free(data);
}

void free_dealloc_tensor(void *data, size_t length, void *arg) {
  free(data);
}

void nonfree_dealloc_tensor(void *data, size_t length, void *arg) {
  // No need to free memory
  (void)data;
  (void)length;
  (void)arg;
}

TF_Buffer *read_file(const std::string &filename) {

  FILE *f = fopen(filename.c_str(), "rb");
  if (!f) {
    std::cerr << "Failed to open file : " << filename << std::endl;
    return nullptr;
  }

  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  if (fsize < 16) {
    std::cerr << "Invalid data size : " << fsize << std::endl;
    return nullptr;
  }

  void *data = malloc(size_t(fsize));
  size_t n = fread(data, size_t(fsize), 1, f);
  fclose(f);

  if (n != 1) {
    std::cerr << "Fread error" << std::endl;
    return nullptr;
  }

  TF_Buffer *buf = TF_NewBuffer();
  buf->data = data;
  buf->length = size_t(fsize);
  buf->data_deallocator = free_buffer;

  return buf;
}

TensorflowSynthesizer::TensorflowSynthesizer() {
  status = TF_NewStatus();
}

TensorflowSynthesizer::~TensorflowSynthesizer() {
    if (session != nullptr) {
      TF_CloseSession(session, status);
      TF_DeleteSession(session, status);
      session = nullptr;
    }
    if (graph != nullptr) {
      TF_DeleteGraph(graph);
      graph = nullptr;
    }
    if (status != nullptr) {
      TF_DeleteStatus(status);
      status = nullptr;
    }
}

void TensorflowSynthesizer::init(int argc, char* argv[]) {
}

bool TensorflowSynthesizer::load(
  const std::string& graph_filename, const std::string& inp_layer, const std::string& out_layer) {
  input_layer = inp_layer;
  output_layer = out_layer;

  // First we load and initialize the model.
  TF_Buffer *graph_def = read_file(graph_filename);
  if (graph_def == nullptr) {
    std::cerr << "Failed to read graph file." << std::endl;
    return false;
  }

  graph = TF_NewGraph();
  TF_ImportGraphDefOptions *graph_opts = TF_NewImportGraphDefOptions();
  TF_GraphImportGraphDef(graph, graph_def, graph_opts, status);

  bool ret = false;
  TF_SessionOptions *sess_opts = nullptr;

  if (TF_GetCode(status) != TF_OK) {
    std::cerr << "ERROR: Unable to import graph : " << TF_Message(status) << std::endl;
    goto release;
  }

  std::cout << "Loaded graph file : " << graph_filename << std::endl;

  sess_opts = TF_NewSessionOptions();
  session = TF_NewSession(graph, sess_opts, status);
  if (TF_GetCode(status) != TF_OK) {
    std::cerr << "Failed to create Session : " << TF_Message(status) << std::endl;
    goto release;
  }

  ret = true;

release:  

  TF_DeleteSessionOptions(sess_opts);
  TF_DeleteBuffer(graph_def);
  TF_DeleteImportGraphDefOptions(graph_opts);

  return ret;
}

bool TensorflowSynthesizer::synthesize(const std::vector<int32_t>& input_sequence, const std::vector<int32_t>& input_lengths, std::vector<float> *output) {

  // Batch size = 1 for a while
  int N = 1;

  int input_length = int(input_sequence.size()); 
  int64_t input_dims[2] = {N, input_length};  
  std::vector<TF_Output> inputs;
  std::vector<TF_Tensor *> input_values;

  TF_Output input_opout = {TF_GraphOperationByName(graph, input_layer.c_str()), 0};
  inputs.push_back(input_opout);
  TF_Tensor *input_tensor = TF_NewTensor(
      TF_INT32, input_dims, 2,
      reinterpret_cast<void *>(const_cast<int32_t*>(input_sequence.data())), sizeof(int32_t) * input_sequence.size(),
      nonfree_dealloc_tensor, /* dealloc_arg */ nullptr);
  input_values.push_back(input_tensor);

  TF_Output input_lengths_opout = {TF_GraphOperationByName(graph, "input_lengths"), 0};
  int32_t input_lengths_sequence[1] = {input_length};  

  inputs.push_back(input_lengths_opout);
  TF_Tensor *input_lengths_tensor = TF_NewTensor(
      TF_INT32, input_dims, 1,
      reinterpret_cast<void *>(input_lengths_sequence), sizeof(int32_t),
      nonfree_dealloc_tensor, /* dealloc_arg */ nullptr);
  input_values.push_back(input_lengths_tensor);

  std::vector<TF_Output> outputs;
  TF_Operation *output_op =
      TF_GraphOperationByName(graph, output_layer.c_str());
  TF_Output output_opout = {output_op, 0};
  outputs.push_back(output_opout);

  std::vector<TF_Tensor *> output_values(outputs.size(), nullptr);
  //output_values.push_back(nullptr);

  auto startT = std::chrono::system_clock::now();

  // Run
  TF_SessionRun(session,
                /* run_options */ nullptr,
                /* const TF_Output* inputs */ &inputs[0],
                /* TF_Tensor* const* input_values */ &input_values[0],
                /* int ninputs */ int(inputs.size()),
                /* const TF_Output* outputs */ &outputs[0],
                /* TF_Tensor** output_values */ &output_values[0],
                /* int noutputs */ int(outputs.size()),
                /* target_opers */ nullptr,
                /* int ntargets */ 0,
                /* run_metadata */ nullptr,
                /* status */ status);

  std::cout << "got it\n";

  if (TF_GetCode(status) != TF_OK) {
    std::cerr << "Failed to run session : " << TF_Message(status) << std::endl;

    TF_DeleteTensor(input_tensor);
    TF_DeleteTensor(input_lengths_tensor);
    TF_DeleteTensor(output_values[0]);

    return false;
  }

  auto endT = std::chrono::system_clock::now();
  std::chrono::duration<double, std::milli> ms = endT - startT;

  std::cout << "Synth time : " << ms.count() << " [ms]" << std::endl;

  std::size_t out_bytes = TF_TensorByteSize(output_values[0]);
  std::cout << "out_bytes = " << out_bytes << std::endl;
  const float *result =
      static_cast<const float *>(TF_TensorData(output_values[0]));

  output->assign(result, result + out_bytes / sizeof(float));

  TF_DeleteTensor(input_tensor);
  TF_DeleteTensor(input_lengths_tensor);
  TF_DeleteTensor(output_values[0]);

  return true;
}

} // namespace tts
