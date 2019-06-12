#include "mp3_file.h"
#include <iostream>
#include <dirent.h>
#include <string_view>
#include <algorithm>


constexpr auto path_separator =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::cout << "usage: mp3encoder dir_with_wav_files" << std::endl;
        return 0;
    }
    try {
        if (auto dir_obj = opendir(argv[1]); dir_obj != nullptr) {
            std::string path(argv[1]);
            if (path.back() != path_separator) {
                path += path_separator;
            }
            std::string wav_filepath;
            std::string mp3_filepath;
            struct dirent *ent;
            while ((ent = readdir(dir_obj)) != nullptr) {
                std::string_view filename(ent->d_name);
                if (filename.size() > 3) {
                    const auto ext = filename.substr(filename.size() - 4, 4);
                    if (std::equal(ext.begin(), ext.end(), ".wav", [](char a, char b) { return tolower(a) == b; })) {
                        wav_filepath = path;
                        wav_filepath.append(filename);

                        mp3_filepath = path;
                        mp3_filepath.append(filename.substr(0, filename.size() - 3));
                        mp3_filepath.append("mp3");

                        std::cout << "Processing file " << mp3_filepath << std::endl;

                        WaveFile wave(wav_filepath.c_str());
                        Mp3File mp3(mp3_filepath.c_str(), wave);
                        mp3.encode();
                    }
                }
            }
            closedir(dir_obj);
        }
        else {
            throw std::runtime_error("could not open directory");
        }
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}
