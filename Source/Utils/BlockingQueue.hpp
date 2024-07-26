/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file BlockingQueue.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>

namespace ra::utils {

template <typename T>
class BlockingQueue {
 public:
  void Push(T value);

  std::optional<T> Pop();

  void Close();

 private:
  std::mutex              mutex_;
  std::condition_variable cv_not_empty_;

  bool                    closed_{false};
  std::deque<T>           data_;
};

template <typename T>
void BlockingQueue<T>::Push(T value) {
  std::lock_guard lock(mutex_);

  if (!closed_) {
    data_.push_back(std::move(value));
  }

  cv_not_empty_.notify_one();
}

template <typename T>
std::optional<T> BlockingQueue<T>::Pop() {
  std::unique_lock lock(mutex_);

  while (!closed_ && data_.empty()) {
    cv_not_empty_.wait(lock);
  }

  if (closed_ && data_.empty()) {
    return std::nullopt;
  }

  T front{std::move(data_.front())};
  data_.pop_front();

  return front;
}

template <typename T>
void BlockingQueue<T>::Close() {
  std::lock_guard lock(mutex_);

  closed_ = true;
  cv_not_empty_.notify_all();
}

}  // namespace ra::utils
