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

using f_TSP = size_t(int *, const float *, size_t);
using Clock = std::chrono::high_resolution_clock;

const char SHARED_EXTENSION[] = SHARED_EXTENSION_STR;

struct AdjacencyMatrix {
  std::filesystem::path origin;

  std::vector<float> data;
  size_t cols = 0;

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

struct CSVWriter {
  CSVWriter(const char headers[][20]) : csv("log.csv") {
    for (size_t i = 0; i < sizeof(headers) / sizeof(*headers); i++) {
    }
  }

  void start() { csv << '\n'; }
  template <typename T> void write(T &val) { csv << val; }

private:
  std::ofstream csv;
};

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
    bool first = true;
    while (std::getline(file, buff)) {

      while (true) {
        size_t j = buff.find(' ');

        if (first)
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
      first = false;
    }
  }

  return samples;
}

int main(int argc, const char **argv) {
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
      int *out = (int *)malloc(sizeof(int) * sample.cols);
      Clock::time_point start = Clock::now();
      size_t n = tsp(out, sample.data.data(), sample.cols);
      std::chrono::duration<double> dur = Clock::now() - start;
      std::cout << '\t' << sample.origin.filename() << ": " << dur.count()
                << "s\n";

      float cost = 0;
      for (size_t i = 0; i < n; i++) {
        size_t ni = (i + 1) % n;

        cost += sample.data[out[i] * sample.cols + out[ni]];
      }

      float delta = cost - sample.expected;
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