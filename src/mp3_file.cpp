#include "mp3_file.h"
#include <stdexcept>
#include <lame.h>


Mp3File::Mp3File(const char *path, WaveFile &wave) MAYTHROW :
    gfp(lame_init()), file(fopen(path, "wb")), wave(wave)
{
    if (gfp == nullptr) {
        throw std::runtime_error("lame_init failed");
    }
    if (file == nullptr) {
        throw std::runtime_error("Output mp3 file opening failed");
    }

    lame_set_mode(gfp, wave.num_channels() > 1 ? JOINT_STEREO : MONO);
    lame_set_num_channels(gfp, wave.num_channels());

    lame_set_VBR(gfp, vbr_mtrh);
    lame_set_VBR_mean_bitrate_kbps(gfp, 256);

    lame_set_in_samplerate(gfp, wave.sample_rate());

    if (lame_init_params(gfp) < 0) {
        throw std::runtime_error("lame_init_params failed");
    }
}

Mp3File::~Mp3File()
{
    lame_close(gfp);
    if (file != nullptr) {
        fclose(file);
    }
}

bool Mp3File::encode() MAYTHROW
{
    uint8_t pcm_buffer[4096];
    uint8_t mp3_buffer[static_cast<size_t>(1.25 * sizeof pcm_buffer) + 7200];
    int bytes;
    size_t num_samples;
    while ((num_samples = wave.read_samples(pcm_buffer, sizeof pcm_buffer))) {
        if (wave.num_channels() == 1) {
            bytes = wave.bits_per_sample() <= 16
                    ? lame_encode_buffer(gfp, reinterpret_cast<int16_t *>(pcm_buffer), nullptr, num_samples, mp3_buffer, sizeof mp3_buffer)
                    : lame_encode_buffer_int(gfp, reinterpret_cast<int32_t *>(pcm_buffer), nullptr, num_samples, mp3_buffer, sizeof mp3_buffer);
        }
        else {
            bytes = wave.bits_per_sample() <= 16
                    ? lame_encode_buffer_interleaved(gfp, reinterpret_cast<int16_t *>(pcm_buffer), num_samples, mp3_buffer, sizeof mp3_buffer)
                    : lame_encode_buffer_interleaved_int(gfp, reinterpret_cast<int32_t *>(pcm_buffer), num_samples, mp3_buffer, sizeof mp3_buffer);
        }
        if (bytes < 0) {
            throw std::runtime_error("lame encode failed");
        }
        fwrite(mp3_buffer, 1, bytes, file);
    }
    if ((bytes = lame_encode_flush(gfp, mp3_buffer, sizeof mp3_buffer)) > 0) {
        fwrite(mp3_buffer, 1, bytes, file);
    }

    lame_mp3_tags_fid(gfp, file);
    return true;
}
