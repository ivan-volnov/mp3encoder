#include <iostream>
#include <exception>
#include <lame.h>
#include "tools/global.h"

constexpr size_t PCM_BUFSIZE = 4096;
constexpr size_t MP3_BUFSIZE = static_cast<size_t>(1.25 * PCM_BUFSIZE) + 7200;
#define SAMPLE_RATE_HZ 16000

class LameEncoder
{
public:
    LameEncoder() MAYTHROW
    {
        if ((gfp = lame_init()) == nullptr) {
            throw std::runtime_error("lame_init failed");
        }

        lame_set_mode(gfp, JOINT_STEREO);
        lame_set_num_channels(gfp, 2);

        lame_set_VBR(gfp, vbr_mtrh);
        lame_set_VBR_mean_bitrate_kbps(gfp, 256);

        if (lame_init_params(gfp) < 0) {
            throw std::runtime_error("lame_init_params failed");
        }
    }

    ~LameEncoder()
    {
        lame_close(gfp);
    }

    LameEncoder(const LameEncoder &) = delete;
    LameEncoder(LameEncoder &&) = delete;
    LameEncoder &operator = (const LameEncoder &) = delete;
    LameEncoder &operator = (LameEncoder &&) = delete;

    bool encode(char *inPath, char *outPath) MAYTHROW
    {
//        lame_set_in_samplerate(gfp, SAMPLE_RATE_HZ);

        auto infp = fopen(inPath, "rb");
        auto outfp = fopen(outPath, "wb");

        short pcm_buffer[PCM_BUFSIZE * 2];
        unsigned char mp3_buffer[MP3_BUFSIZE];

        int input_samples;
        int mp3_bytes;
        do {
            input_samples = fread(pcm_buffer, 2, PCM_BUFSIZE, infp);
            mp3_bytes = lame_encode_buffer(gfp, pcm_buffer, nullptr, input_samples, mp3_buffer, MP3_BUFSIZE);
            if (mp3_bytes < 0) {
                fclose(outfp);
                fclose(infp);
                return false;
            }
            fwrite(mp3_buffer, 1, mp3_bytes, outfp);
        }
        while (input_samples == PCM_BUFSIZE);

        mp3_bytes = lame_encode_flush(gfp, mp3_buffer, MP3_BUFSIZE);
        if (mp3_bytes > 0) {
            fwrite(mp3_buffer, 1, mp3_bytes, outfp);
        }

        lame_mp3_tags_fid(gfp, outfp);

        fclose(outfp);
        fclose(infp);
        return true;
    }

private:
    lame_global_flags *gfp;
};


int main(int argc, char **argv)
{
    if (argc < 3) {
        std::cout << "usage: mp3encoder raw-infile mp3-outfile" << std::endl;
    }
    else {
        try {
            LameEncoder encoder;
            encoder.encode(argv[1], argv[2]);
        } catch (std::runtime_error &e) {
            std::cout << e.what() << std::endl;
        }
    }
}
