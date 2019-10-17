#ifndef TF_SYNTHESIZER_H_
#define TF_SYNTHESIZER_H_

#include <memory>
#include <string>
#include <vector>

#include <tensorflow/c/c_api.h>

namespace tts {

class TensorflowSynthesizer {
 public:
  TensorflowSynthesizer();
  ~TensorflowSynthesizer();

  void init(int argc, char* argv[]);

  ///
  /// Load's pretrained TF model.
  ///
  bool load(const std::string& graph_filename, const std::string& inp_layer,
            const std::string& out_layer);

  ///
  /// Synthesize speech.
  ///
  /// @param[in] input_sequence Input sequence. Shape = [N, T_in].
  /// @param[in] input_lengths Tensor with shape = [N],  where N is batch size
  /// and values are the lengths
  ///      of each sequence in inputs.
  /// @param[out] output Output audio data(floating point)
  ///
  bool synthesize(const std::vector<int32_t>& input_sequence, const std::vector<int32_t> &input_lengths, std::vector<float> *output);

private:
  TF_Session *session = nullptr;
  TF_Status *status = nullptr;
  TF_Graph *graph = nullptr;
  std::string input_layer, output_layer;
};

}  // namespace tts

#endif  // TF_SYNTHESIZER_H_
