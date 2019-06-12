#include "mp3_file.h"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc < 3) {
        std::cout << "usage: mp3encoder raw-infile mp3-outfile" << std::endl;
    }
    else {
        try {
            WaveFile wave(argv[1]);
            Mp3File mp3(argv[2], wave);
            mp3.encode();
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }
}
