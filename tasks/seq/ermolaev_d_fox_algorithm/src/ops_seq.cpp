#include "seq/ermolaev_d_fox_algorithm/include/ops_seq.hpp"

#include <cstring>
#include <iostream>
#include <thread>
#include <cmath>

using namespace std::chrono_literals;
namespace {
std::vector<std::vector<double>> multiply_block(const std::vector<std::vector<double>>& block_A,
                                                const std::vector<std::vector<double>>& block_B) {
  size_t blockSize = block_A.size();
  std::vector<std::vector<double>> block_C(blockSize, std::vector<double>(blockSize, 0.0));
  for (size_t i = 0; i < blockSize; ++i) {
    for (size_t j = 0; j < blockSize; ++j) {
      for (size_t k = 0; k < blockSize; ++k) {
        block_C[i][j] += block_A[i][k] * block_B[k][j];
      }
    }
  }
  return block_C;
}

}  // namespace

bool FoxAlgorithm::validation() {
  internal_order_test();
  return !taskData->inputs_count.empty() && taskData->inputs_count.size() == 2 &&
         taskData->inputs_count[0] == taskData->inputs_count[1] && taskData->outputs_count.size() == 1 &&
         taskData->outputs_count[0] == taskData->inputs_count[0] && taskData->inputs[0] != nullptr &&
         taskData->inputs[1] != nullptr && taskData->outputs[0] != nullptr;
}

bool FoxAlgorithm::pre_processing() {
  try {
    internal_order_test();
    data_size = static_cast<size_t>(sqrt(taskData->inputs_count[0]));
    if (data_size * data_size != taskData->inputs_count[0]) {
      std::cerr << "Data size error: input count does not match expected size of square matrix." << std::endl;
      return false;
    }
    matrix_A = reinterpret_cast<double*>(taskData->inputs[0]);
    matrix_B = reinterpret_cast<double*>(taskData->inputs[1]);
    matrix_C = reinterpret_cast<double*>(taskData->outputs[0]);
    for (size_t i = 0; i < data_size; ++i) {
      for (size_t j = 0; j < data_size; ++j) {
        matrix_C[i * data_size + j] = 0;
      }
    }
  } catch (...) {
    return false;
  }
  return matrix_A != nullptr && matrix_B != nullptr;
}

bool FoxAlgorithm::run() {
  internal_order_test();
  try {
    size_t blockSize = 1;
    size_t numBlocks = data_size / blockSize;

    for (size_t stage = 0; stage < numBlocks; ++stage) {
      for (size_t i = 0; i < numBlocks; ++i) {
        for (size_t j = 0; j < numBlocks; ++j) {
          size_t k = (i + stage) % numBlocks;
          std::vector<std::vector<double>> block_A(blockSize, std::vector<double>(blockSize));
          std::vector<std::vector<double>> block_B(blockSize, std::vector<double>(blockSize));
          for (size_t row = 0; row < blockSize; ++row) {
            for (size_t col = 0; col < blockSize; ++col) {
              block_A[row][col] = matrix_A[((i * blockSize) + row) * data_size + (k * blockSize) + col];
              block_B[row][col] = matrix_B[((k * blockSize) + row) * data_size + (j * blockSize) + col];
            }
          }
          std::vector<std::vector<double>> block_C = multiply_block(block_A, block_B);
          for (size_t row = 0; row < blockSize; ++row) {
            for (size_t col = 0; col < blockSize; ++col) {
              matrix_C[((i * blockSize) + row) * data_size + (j * blockSize) + col] += block_C[row][col];
            }
          }
        }
      }
    }
  } catch (...) {
    return false;
  }
  return true;
}

bool FoxAlgorithm::post_processing() {
  internal_order_test();
  try {
    for (size_t i = 0; i < data_size; ++i) {
      for (size_t j = 0; j < data_size; ++j) {
        reinterpret_cast<double*>(taskData->outputs[0])[i * data_size + j] = matrix_C[i * data_size + j];
      }
    }
  } catch (...) {
    return false;
  }
  return true;
}
