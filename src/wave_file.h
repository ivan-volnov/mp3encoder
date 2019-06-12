#ifndef WAVE_FILE_H
#define WAVE_FILE_H

#include <cstdio>
#include <cstdint>
#include "tools/global.h"

class WaveFile
{
    enum class Format: uint16_t {
        PCM = 0x0001,
        IEEE_FLOAT = 0x0003,
        ALAW = 0x0006,
        MULAW = 0x0007,
        EXTENSIBLE = 0xfffe
    };
public:
    WaveFile(const char *path) MAYTHROW;

public:
    size_t read_samples(uint8_t *buffer, size_t size) MAYTHROW;
    uint16_t num_channels() const noexcept;
    size_t sample_size() const noexcept;
    uint16_t bits_per_sample() const noexcept;
    uint32_t sample_rate() const noexcept;

private:
    FILE *file;

    struct __attribute__((packed)) {
        struct Chunk {
            char id[4];
            uint32_t size;
        } chunk;
        char wave_id[4];

        Chunk format;
        struct F {
            uint16_t tag;
            uint16_t n_channels;
            uint32_t sample_rate;
            uint32_t avg_bytes_per_sec;
            uint16_t block_align;
            uint16_t bits_per_sample;
        } fmt;

        Chunk data;
    } header;
};

#endif // WAVE_FILE_H
