// Copyright 2024 Alexseev Danila
#include "tbb/alexseev_d_hoare_sort_w_batcher_merge/include/hoare_sort_w_batcher_merge_tbb.hpp"

#include <oneapi/tbb.h>

#include <algorithm>
#include <random>
#include <thread>

using namespace std::chrono_literals;

// Sequential implementation of sorting
bool HoareSortWBatcherMergeSequential::pre_processing() {
  try {
    internal_order_test();
    array.clear();
    for (size_t i = 0; i < taskData->inputs_count[0]; ++i) {
      int *currentElementPtr = reinterpret_cast<int *>(taskData->inputs[0] + i * sizeof(int));
      array.push_back(*currentElementPtr);
    }
  } catch (...) {
    return false;
  }
  return true;
}

bool HoareSortWBatcherMergeSequential::validation() {
  try {
    internal_order_test();
  } catch (...) {
    return false;
  }
  return taskData->inputs_count[0] == taskData->outputs_count[0];
}

bool HoareSortWBatcherMergeSequential::run() {
  try {
    internal_order_test();
    HoareSortWBatcherMergeSeq(array, 0, array.size() - 1);
  } catch (...) {
    return false;
  }
  return true;
}

bool HoareSortWBatcherMergeSequential::post_processing() {
  try {
    internal_order_test();
    for (size_t i = 0; i < array.size(); ++i) {
      int *currentElementPtr = reinterpret_cast<int *>(taskData->outputs[0] + i * sizeof(int));
      *currentElementPtr = array[i];
    }
  } catch (...) {
    return false;
  }
  return true;
}

void HoareSortWBatcherMergeSequential::HoareSortWBatcherMergeSeq(std::vector<int> &arr, size_t l, size_t r) {
  if (arr.size() <= 1) return;
  int n = r - l + 1;
  for (int p = 1; p < n; p += p)
    for (int k = p; k > 0; k /= 2)
      for (int j = k % p; j + k < n; j += (k + k))
        for (int i = 0; i < n - j - k; ++i)
          if ((j + i) / (p + p) == (j + i + k) / (p + p)) CompExch(arr[l + j + i], arr[l + j + i + k]);
}

// TBB implementation of sorting
bool HoareSortWBatcherMergeTBB::pre_processing() {
  try {
    internal_order_test();
    array.clear();
    for (size_t i = 0; i < taskData->inputs_count[0]; ++i) {
      int *currentElementPtr = reinterpret_cast<int *>(taskData->inputs[0] + i * sizeof(int));
      array.push_back(*currentElementPtr);
    }
  } catch (...) {
    return false;
  }
  return true;
}

bool HoareSortWBatcherMergeTBB::validation() {
  try {
    internal_order_test();
  } catch (...) {
    return false;
  }
  return taskData->inputs_count[0] == taskData->outputs_count[0];
}

bool HoareSortWBatcherMergeTBB::run() {
  try {
    internal_order_test();
    HoareSortWBatcherMergeParallel(array, 0, array.size() - 1);
  } catch (...) {
    return false;
  }
  return true;
}

bool HoareSortWBatcherMergeTBB::post_processing() {
  try {
    internal_order_test();
    for (size_t i = 0; i < array.size(); ++i) {
      int *currentElementPtr = reinterpret_cast<int *>(taskData->outputs[0] + i * sizeof(int));
      *currentElementPtr = array[i];
    }
  } catch (...) {
    return false;
  }
  return true;
}

void HoareSortWBatcherMergeTBB::HoareSortWBatcherMergeParallel(std::vector<int> &arr, size_t l, size_t r) {
  if (arr.size() <= 1) return;
  int n = r - l + 1;

  for (int p = 1; p < n; p += p) {
    for (int k = p; k > 0; k /= 2) {
      for (int j = k % p; j + k < n; j += (k + k)) {
        auto work = [&](int start, int end) {
          for (int i = start; i < end; ++i) {
            if ((j + i) / (p + p) == (j + i + k) / (p + p)) {
              CompExch(arr[l + j + i], arr[l + j + i + k]);
            }
          }
        };
        tbb::parallel_invoke([&]() { work(0, (n - j - k) / 2); }, [&]() { work((n - j - k) / 2, n - j - k); });
      }
    }
  }
}

// Additional functions
void CompExch(int &a, int &b) {
  if (a > b) std::swap(a, b);
}

std::vector<int> generateRandomVector(int size, int minVal, int maxVal) {
  std::vector<int> res(size);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> distrib(minVal, maxVal);
  for (int i = 0; i < size; ++i) {
    res[i] = distrib(gen);
  }
  return res;
}