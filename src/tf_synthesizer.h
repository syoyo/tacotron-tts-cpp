#ifndef TF_SYNTHESIZER_H_
#define TF_SYNTHESIZER_H_

#include <string>
#include <memory>

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
  /// (Including Griffin-Lim) 
  ///
  bool synthesize();

private:
  class Impl;
  std::unique_ptr<Impl> impl;
};

} // namespace prnet


#endif // TF_SYNTHESIZER_H_
