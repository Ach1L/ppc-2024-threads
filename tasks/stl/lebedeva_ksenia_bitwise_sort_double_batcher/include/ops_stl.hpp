// Copyright 2024 Lebedeva Ksenia

#pragma once

#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>
#include <thread>
#include <future>
#include <cstdint>
#include <execution>

#include "core/task/include/task.hpp"

namespace LebedevaKStl {

std::vector<double> batchersMergeSeq(std::vector<std::vector<double>>& subVec);
std::vector<double> batchersMergeStl(std::vector<std::vector<double>>& subVec);

void sortPartSeq(std::vector<std::vector<double>>& parts, std::vector<double>& vec);
void sortPartStl(std::vector<std::vector<double>>& parts, std::vector<double>& vec);

std::vector<double> radixSortWithBatcherSeq(std::vector<double> vec);
std::vector<double> radixSortWithBatcherStl(std::vector<double> vec);

std::vector<double> randomVector(int size, double min, double max);

class RadixSortDoubleWithBatcherSequential : public ppc::core::Task {
 public:
  explicit RadixSortDoubleWithBatcherSequential(std::shared_ptr<ppc::core::TaskData> taskData_)
      : Task(std::move(taskData_)) {}

  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<double> array;
  std::vector<double> result;
};

class RadixSortDoubleWithBatcherStlParallel : public ppc::core::Task {
 public:
  explicit RadixSortDoubleWithBatcherStlParallel(std::shared_ptr<ppc::core::TaskData> taskData_)
      : Task(std::move(taskData_)) {}

  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<double> array;
  std::vector<double> result;
};
}  // namespace LebedevaKStl
