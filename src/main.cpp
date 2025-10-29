#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "SharedLibrary.hpp"

#ifndef SHARED_EXTENSION_STR
#error "Shared lib extension not specified"
#endif

#ifndef EPSILON
#define EPSILON 1e-9
#endif

using f_TSP = void(float *, float *, size_t, size_t);
using Clock = std::chrono::high_resolution_clock;

const char SHARED_EXTENSION[] = SHARED_EXTENSION_STR;

struct AdjacencyMatrix {
  std::filesystem::path origin;

  std::vector<float> data;
  size_t cols = 0;
  size_t row = 0;

  float expected = 0.0;
};

bool endsWith(const std::filesystem::directory_entry &dir_entry,
              const char *neddle, size_t n) {
  auto str = dir_entry.path().string();
  auto it = std::prev(str.end(), n);
  for (size_t i = 0; i < n; i++, it++) {
    if (neddle[i] != *it)
      return false;
  }

  return true;
}

std::vector<AdjacencyMatrix> loadExamples(const std::filesystem::path &root) {
  std::vector<AdjacencyMatrix> samples;

  for (auto const &dir_entry :
       std::filesystem::directory_iterator{root / "samples"}) {

    if (!endsWith(dir_entry, ".txt", 4))
      continue;

    std::ifstream file(dir_entry.path());
    AdjacencyMatrix &m = samples.emplace_back();
    m.origin = dir_entry.path();

    int val;
    auto str = dir_entry.path().filename().string();
    sscanf(str.c_str(), "tsp%*d_%d.txt", &val);
    m.expected = val;

    std::string buff;
    while (std::getline(file, buff)) {

      while (true) {
        size_t j = buff.find(' ');

        if (m.row == 0)
          m.cols += 1;
        m.data.push_back(std::stof(buff.substr(0, j)));

        if (j == buff.npos)
          break;
        while (j++ < buff.length() && !isdigit(buff[j]))
          ;

        if (j >= buff.length())
          break;
        buff = buff.substr(j);
      }
      m.row += 1;
    }
  }

  return samples;
}

int main(int argc, const char **argv) {
  int n = 1 << 20;                   // 1,048,576 elements
  std::vector<float> h_a(n, 1.0f);   // Host vector a
  std::vector<float> h_b(n, 2.0f);   // Host vector b
  std::vector<float> h_c(n);         // Host vector c (for the result)
  std::vector<float> h_exp(n, 3.0f); // Host vector expected

  std::filesystem::path root = argv[0];
  root = root.parent_path();

  auto samples = loadExamples(root);

  for (auto const &dir_entry : std::filesystem::directory_iterator{root}) {
    if (!endsWith(dir_entry, SHARED_EXTENSION, sizeof(SHARED_EXTENSION) - 1))
      continue;

    std::cout << dir_entry.path() << '\n';

    SharedLibrary dll(dir_entry.path().string());

    auto tsp = dll.getFunction<f_TSP>("TSP");

    std::cout << dir_entry.path().filename() << ": " << '\n';
    for (auto &sample : samples) {
      float out;
      Clock::time_point start = Clock::now();
      tsp(&out, sample.data.data(), sample.row, sample.cols);
      std::chrono::duration<double> dur = Clock::now() - start;
      std::cout << '\t' << sample.origin.filename() << ": " << dur.count()
                << "s\n";

      float delta = out - sample.expected;
      if (std::abs(delta) > EPSILON) {
        std::cout << "\tWrong by: " << delta << " (out - expected)\n\n";
      } else {
        std::cout << "\tCorrect using EPSILON=" << EPSILON << "\n\n";
      }
    }

    std::cout << '\n';
  }

  return 0;
}