#ifndef ENCODING_POOL_H
#define ENCODING_POOL_H

#include <pthread.h>
#include <queue>
#include <string>
#include "tools/global.h"


class EncodingPool
{
public:
    class Mutex
    {
    public:
        Mutex(const Mutex &) = delete;
        Mutex(Mutex &&) = delete;
        Mutex &operator = (const Mutex &) = delete;
        Mutex &operator = (Mutex &&) = delete;
        Mutex() noexcept;
        ~Mutex();

        void lock() noexcept;
        void unlock() noexcept;

    private:
        pthread_mutex_t mutex;
    };

    EncodingPool() MAYTHROW;
    void run() noexcept;
    void add_file(const std::string &filepath) MAYTHROW;
    void add_file(std::string &&filepath) MAYTHROW;
    bool get_file(std::string &dst) noexcept;
    auto get_guard() noexcept;

    static void *run_thread(void *context) noexcept;

private:
    uint32_t cores;
    std::unique_ptr<pthread_t[]> thread_ids;
    std::queue<std::string> queue;
    Mutex mutex;
};

#endif // ENCODING_POOL_H
