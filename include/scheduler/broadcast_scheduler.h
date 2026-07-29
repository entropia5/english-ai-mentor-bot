#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

class TelegramClient;

class BroadcastScheduler {
  public:
    explicit BroadcastScheduler(TelegramClient& bot);
    ~BroadcastScheduler();

    BroadcastScheduler(const BroadcastScheduler&) = delete;
    BroadcastScheduler& operator=(const BroadcastScheduler&) = delete;

    void start();
    void stop();

  private:
    void run();
    bool wait_for_stop(std::chrono::seconds duration);

    TelegramClient& bot_;
    std::atomic<bool> stopping_{false};
    std::mutex wait_mutex_;
    std::condition_variable wake_up_;
    std::thread worker_;
};
