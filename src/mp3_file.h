#ifndef MP3_FILE_H
#define MP3_FILE_H

#include "wave_file.h"

struct lame_global_struct;
using lame_global_flags = struct lame_global_struct;

class Mp3File
{
public:
    Mp3File(const char *path, WaveFile &wave) MAYTHROW;
    ~Mp3File();

    Mp3File(const Mp3File &) = delete;
    Mp3File(Mp3File &&) = delete;
    Mp3File &operator = (const Mp3File &) = delete;
    Mp3File &operator = (Mp3File &&) = delete;

public:
    bool encode() MAYTHROW;

private:
    lame_global_flags *gfp;
    FILE *file;
    WaveFile &wave;
};

#endif // MP3_FILE_H
