/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Executor.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Utils/BlockingQueue.hpp>

#include <functional>
#include <thread>
#include <vector>

namespace ra::job {

class Executor {
 public:
  using Job = std::function<void()>;

  explicit Executor(size_t thread_count = std::thread::hardware_concurrency());
  ~Executor();

  Executor(const Executor&) = delete;
  Executor& operator=(const Executor&) = delete;

  Executor(Executor&&) = delete;
  Executor& operator=(Executor&&) = delete;

  void Submit(Job job);

  void WaitIdle();

  void Stop();

  static Executor* Current();

 private:
  void WorkerRoutine();

  std::vector<std::thread> threads_;

  std::mutex              mutex_;
  std::atomic<int32_t>    jobs_left_{0};
  std::condition_variable cv_jobs_left_;
  std::atomic<uint32_t>   stopped_{0U};

  utils::BlockingQueue<Job> jobs_;
};

}  // namespace ra::job