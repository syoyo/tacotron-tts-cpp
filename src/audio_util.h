#ifndef AUDIO_UTIL_H_
#define AUDIO_UTIL_H_

#include <cstdlib>
#include <vector>

namespace tts {

std::vector<float> inv_preemphasis(const float *x, const size_t len,
                                   const float scale);

//
// Find end point of audio by detecting silence duration.
// @return End frame index.
//
size_t find_end_point(const float *wav, const size_t wav_len,
                      const size_t sample_rate,
                      const float threshold_db = -40.0f,
                      const float min_silence_sec = 0.8f);

}  // namespace tts

#endif  // AUDIO_UTIL_H_
