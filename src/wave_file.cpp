#include "wave_file.h"
#include <stdexcept>
#include <cstring>


WaveFile::WaveFile(const char *path) MAYTHROW :
    file(fopen(path, "rb"))
{
    if (file == nullptr) {
        throw std::runtime_error("Input wav file opening failed");
    }
    if (fread(&header, sizeof header, 1, file) != 1) {
        throw std::runtime_error("Error reading wav file");
    }
    const auto fmt_size_delta = header.format.size - sizeof header.fmt;
    if (   std::strncmp(header.chunk.id, "RIFF", 4) != 0
        or std::strncmp(header.wave_id, "WAVE", 4) != 0
        or std::strncmp(header.format.id, "fmt ", 4) != 0
        or fmt_size_delta < 0
        or not (   header.fmt.tag == static_cast<uint16_t>(Format::PCM)
                or header.fmt.tag == static_cast<uint16_t>(Format::IEEE_FLOAT)
                or header.fmt.tag == static_cast<uint16_t>(Format::ALAW)
                or header.fmt.tag == static_cast<uint16_t>(Format::MULAW)
                or header.fmt.tag == static_cast<uint16_t>(Format::EXTENSIBLE))
        or header.fmt.block_align != sample_size()
        ) {
        throw std::runtime_error("Wave file reading error");
    }
    if (   header.fmt.tag != static_cast<uint16_t>(Format::PCM)
        or header.fmt.n_channels > 2) {
        throw std::runtime_error("Wave file unsupported format");
    }
    if (fmt_size_delta > 0) {
        if (   fseek(file, fmt_size_delta - sizeof header.data, SEEK_CUR) != 0
            or fread(&header.data, sizeof header.data, 1, file) != 1) {
            throw std::runtime_error("Error reading wav file");
        }
    }
    if (std::strncmp(header.data.id, "data", 4) != 0) {
        throw std::runtime_error("Wave file reading error");
    }
}

size_t WaveFile::read_samples(uint8_t *buffer, size_t size) MAYTHROW
{
    switch (bits_per_sample()) {
    case 16:
    case 32:
        return fread(buffer, sample_size(), size / sample_size(), file);
    case 8:
    {
        const size_t target_sample_size = sizeof(int16_t) * num_channels();
        const size_t num_samples = fread(buffer, sample_size(), size / target_sample_size, file);
        if (num_samples > 0) {
            auto ptr8 = buffer + num_samples * num_channels();
            auto ptr16 = reinterpret_cast<int16_t *>(buffer) + num_samples * num_channels();
            while (ptr8 > buffer) {
                *--ptr16 = (*--ptr8 - 0x80) << 8;
            }
        }
        return num_samples;
    }
    case 24:
    {
        const size_t target_sample_size = sizeof(int32_t) * num_channels();
        const size_t num_samples = fread(buffer, sample_size(), size / target_sample_size, file);
        if (num_samples > 0) {
            auto ptr8 = buffer + num_samples * num_channels() * 3;
            auto ptr32 = reinterpret_cast<int32_t *>(buffer) + num_samples * num_channels();
            while (ptr8 > buffer) {
                ptr8 -= 3;
                *--ptr32 = (ptr8[0] << 8) | (ptr8[1] << 16) | (ptr8[2] << 24);
            }
        }
        return num_samples;
    }
    default:
        throw std::runtime_error("Wave file unsupported format");
    }
}

uint16_t WaveFile::num_channels() const noexcept
{
    return header.fmt.n_channels;
}

size_t WaveFile::sample_size() const noexcept
{
    return header.fmt.n_channels * header.fmt.bits_per_sample / 8;
}

uint16_t WaveFile::bits_per_sample() const noexcept
{
    return header.fmt.bits_per_sample;
}

uint32_t WaveFile::sample_rate() const noexcept
{
    return header.fmt.sample_rate;
}
