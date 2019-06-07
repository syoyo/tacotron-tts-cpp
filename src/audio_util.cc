#include "audio_util.h"

#include <algorithm>
#include <cmath>

namespace tts {

namespace {

float db_to_amp(const float x) { return std::pow(10.0f, x * 0.05f); }

}  // namespace

std::vector<float> inv_preemphasis(const float *x, size_t len,
                                   const float scale) {
  // scipy.signal.lfilter([1], [1, -hparams.preemphasis], x)
  // =>
  // y[0] = x[0]
  // y[1] = -y[0] * (-hparams.preemphasis) + x[1]
  // ...
  // y[n] = -y[n-1] * (-hparams.preemphasis) + x[n]
  //

  std::vector<float> y;
  y.push_back(x[0]);

  for (size_t i = 1; i < len; i++) {
    y.emplace_back(x[i] + y[i - 1] * scale);
  }

  return y;
}

size_t find_end_point(const float *wav, const size_t wav_len,
                      const size_t sample_rate, const float threshold_db,
                      const float min_silence_sec) {
  const size_t window_length = size_t(sample_rate * min_silence_sec);
  const size_t hop_length = window_length / 4;

  const float threshold = db_to_amp(threshold_db);

  if (window_length > wav_len) {
    return wav_len;
  }

  for (size_t x = hop_length; x < (wav_len - window_length); x += hop_length) {
    // find maximum for range [x, x + window_length]
    float m = *(std::max_element(wav + x, wav + (x + window_length)));
    if (m < threshold) {
      return x + hop_length;
    }
  }

  // No silence duration found.
  return wav_len;
}

}  // namespace tts
