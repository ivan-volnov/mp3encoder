#include "encoding_pool.h"
#include <iostream>
#include <thread>
#include "mp3_file.h"


EncodingPool::Mutex::Mutex() noexcept
{
    pthread_mutex_init(&mutex, nullptr);
}

EncodingPool::Mutex::~Mutex()
{
    pthread_mutex_destroy(&mutex);
}

void EncodingPool::Mutex::lock() noexcept
{
    pthread_mutex_lock(&mutex);
}

void EncodingPool::Mutex::unlock() noexcept
{
    pthread_mutex_unlock(&mutex);
}


EncodingPool::EncodingPool() MAYTHROW :
    cores(std::thread::hardware_concurrency()),
    thread_ids(std::make_unique<pthread_t[]>(cores))
{
    std::cout << "Initialize encoding pool with " << cores << " threads" << std::endl;
}

void EncodingPool::run() noexcept
{
    for (uint32_t i = 0; i < cores; ++i) {
        pthread_create(&thread_ids[i], nullptr, run_thread, this);
    }
    for (uint32_t i = 0; i < cores; ++i) {
        pthread_join(thread_ids[i], nullptr);
    }
}

void EncodingPool::add_file(const std::string &filepath) MAYTHROW
{
    std::lock_guard guard(mutex);
    queue.push(filepath);
}

void EncodingPool::add_file(std::string &&filepath) MAYTHROW
{
    std::lock_guard guard(mutex);
    queue.push(std::move(filepath));
}

bool EncodingPool::get_file(std::string &dst) noexcept
{
    std::lock_guard guard(mutex);
    if (not queue.empty()) {
        dst = std::move(queue.front());
        queue.pop();
        return true;
    }
    return false;
}

auto EncodingPool::get_guard() noexcept
{
    return std::lock_guard(mutex);
}

void *EncodingPool::run_thread(void *context) noexcept
{
    auto pool = reinterpret_cast<EncodingPool *>(context);
    std::string wav_filepath;
    std::string mp3_filepath;
    try {
        while (pool->get_file(wav_filepath)) {
            mp3_filepath = wav_filepath + "mp3";
            wav_filepath.append("wav");
            {
                auto guard = pool->get_guard();
                std::cout << "Processing file " << mp3_filepath << std::endl;
            }
            WaveFile wave(wav_filepath.c_str());
            Mp3File mp3(mp3_filepath.c_str(), wave);
            mp3.encode();
        }
    } catch (const std::exception &e) {
        auto guard = pool->get_guard();
        std::cout << e.what() << std::endl;
    }
    pthread_exit(nullptr);
}
