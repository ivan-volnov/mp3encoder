#include <iostream>
#include <algorithm>
#include <dirent.h>
#include "encoding_pool.h"


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
    EncodingPool pool;
    try {
        auto dir_obj = opendir(argv[1]);
        if (dir_obj == nullptr) {
            std::cout << "could not open directory" << std::endl;
            return 1;
        }
        std::string path(argv[1]);
        if (path.back() != path_separator) {
            path += path_separator;
        }
        struct dirent *ent;
        while ((ent = readdir(dir_obj)) != nullptr) {
            std::string_view filename(ent->d_name, ent->d_namlen);
            if (filename.size() > 3) {
                const auto ext = filename.substr(filename.size() - 4, 4);
                if (std::equal(ext.begin(), ext.end(), ".wav", [](char a, char b) { return tolower(a) == b; })) {
                    auto filepath = path;
                    filepath.append(filename.substr(0, filename.size() - 3));
                    pool.add_file(std::move(filepath));
                }
            }
        }
        closedir(dir_obj);
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    pool.run();
}
