/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file Executor.cpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#include <JobSystem/Executor.hpp>

#include <Utils/Assert.hpp>

namespace ra::job {

static thread_local Executor* current_job_system;

Executor::Executor(size_t thread_count) {
  for (size_t i = 0; i < thread_count; ++i) {
    threads_.emplace_back([this]() {
      current_job_system = this;

      while (stopped_.load() == 0) {
        auto job = jobs_.Pop();

        if (!job.has_value()) {
          break;
        }

        job.value()();

        std::lock_guard lock(mutex_);
        jobs_left_.fetch_sub(1);
        cv_jobs_left_.notify_all();
      }

      current_job_system = nullptr;
    });
  }
}

Executor::~Executor() {
  RA_ASSERT(stopped_.load() == 1, "Executor must be stopped before destructor!");
}

void Executor::Submit(Job job) {
  jobs_left_.fetch_add(1);
  jobs_.Push(std::move(job));
}

void Executor::WaitIdle() {
  std::unique_lock lock(mutex_);

  while (jobs_left_.load() > 0) {
    cv_jobs_left_.wait(lock);
  }
}

void Executor::Stop() {
  stopped_.store(1);
  jobs_left_.store(0);
  jobs_.Close();

  for (auto& thread : threads_) {
    thread.join();
  }
}

Executor* Executor::Current() {
  return current_job_system;
}

}//namespace ra::job
