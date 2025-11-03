#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
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

struct Sample {
  std::filesystem::path fileOrigin;

  std::vector<float> adjMatrix;
  size_t vertsAmount = 0;

  float expectedCost = 0.0;
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
  CSVWriter(std::vector<std::string> headers, std::filesystem::path root)
      : csv(root / "log.csv") {
    for (size_t i = 0; i < headers.size(); i++) {
      if (i != headers.size() - 1)
        csv << headers[i] << ',';
      else
        csv << headers[i] << '\n';
    }
  }

  void begin() { began = true; }

  template <typename T> void write(T val) {
    if (!began)
      return;
    csv << val;
    csv << ',';
  }

  void end() {
    csv << '\n';
    csv.flush();
    began = false;
  }

private:
  std::ofstream csv;
  bool began = false;
};

std::vector<Sample> loadExamples(const std::filesystem::path &root) {
  std::vector<Sample> samples;

  for (auto const &dir_entry :
       std::filesystem::directory_iterator{root / "samples"}) {

    if (!endsWith(dir_entry, ".txt", 4))
      continue;

    std::ifstream file(dir_entry.path());
    Sample &m = samples.emplace_back();
    m.fileOrigin = dir_entry.path();

    int val;
    auto str = dir_entry.path().filename().string();
    sscanf(str.c_str(), "tsp%*d_%d.txt", &val);
    m.expectedCost = val;

    std::string buff;
    bool first = true;
    m.vertsAmount = 0;
    while (std::getline(file, buff)) {
      std::string aux;
      m.vertsAmount += 1;
      for (size_t index = 0; index < buff.length(); index++) {
        if (isgraph(buff.at(index))) {
          aux.push_back(buff.at(index));
        } else {
          if (aux.length() > 0) {
            m.adjMatrix.push_back((float)std::stoi(aux));
          }
          aux.clear();
        }
      }
      if (aux.length() > 0) {
        m.adjMatrix.push_back((float)std::stoi(aux));
      }
      aux.clear();
    }
  }

  std::sort(samples.begin(), samples.end(), [](auto &a, auto &b) {
    return a.adjMatrix.size() < b.adjMatrix.size();
  });

  return samples;
}

std::filesystem::path getRoot(const char **argv) {
  std::filesystem::path root = argv[0];
  return root.parent_path();
}

size_t getSampleSize(int argc, const char **argv) {
  size_t samplesAmount = 0;
  if (argc > 1) {
    samplesAmount = atoll(argv[1]);
    if (samplesAmount != 0)
      return samplesAmount;
  }
  return 1;
}

std::vector<std::filesystem::path>
fetchAlgorithms(const std::filesystem::path root) {
  std::vector<std::filesystem::path> algos;
  algos.reserve(10);

  // Filter non shared libraries off
  for (auto const &dir_entry : std::filesystem::directory_iterator{root}) {
    if (!endsWith(dir_entry, SHARED_EXTENSION, sizeof(SHARED_EXTENSION) - 1))
      continue;

    algos.push_back(dir_entry.path());
  }

  const auto alphabeticalOrderBruteLastPred =
      [](const std::filesystem::path &a, const std::filesystem::path &b) {
        if (a.filename().string().find("Brute") != std::string::npos)
          return false;
        else if (b.filename().string().find("Brute") != std::string::npos)
          return true;
        else
          return a.filename().string() < b.filename().string();
      };

  std::sort(algos.begin(), algos.end(), alphabeticalOrderBruteLastPred);

  return algos;
}


int main(int argc, const char **argv) {
  const std::filesystem::path root = getRoot(argv);
  const size_t samplesAmount = getSampleSize(argc, argv);
  const std::vector<Sample> samples = loadExamples(root);

  CSVWriter csv({"Sample", "Algorithm", "VertsAmount", "ExecutionTime", "Path",
                 "Cost", "Expected", "Delta"},
                root);

  const std::vector<std::filesystem::path> algos = fetchAlgorithms(root);
  for (const auto &pa : algos) {
    SharedLibrary dll(pa.string());

    auto tsp = dll.getFunction<f_TSP>("TSP");

    std::cout << pa.filename() << ": " << '\n';
    for (const Sample &sample : samples) {
      for (size_t _ = 0; _ < samplesAmount; _++) {
        std::vector<int> out(sample.vertsAmount);

        Clock::time_point start = Clock::now();
        size_t n = tsp(out.data(), sample.adjMatrix.data(), sample.vertsAmount);
        std::chrono::duration<double> dur = Clock::now() - start;

        std::cout << '\t' << sample.fileOrigin.filename() << ": " << dur.count()
                  << "s\n";

        std::stringstream pathStr;
        float cost = 0;
        pathStr << out[0];
        for (size_t i = 0; i < n; i++) {
          size_t ni = (i + 1) % n;

          pathStr << " -> " << out[ni];

          cost += sample.adjMatrix[out[i] * sample.vertsAmount + out[ni]];
        }

        float delta = cost - sample.expectedCost;
        float p = cost / sample.expectedCost;
        if (std::abs(delta) > EPSILON) {
          std::cout << "\tWrong by: " << delta << " (" << p
                    << ") (out - expected)\n\n";
        } else {
          std::cout << "\tRight Cost/Path" << "\n";
        }

        // "Sample", "Algorithm", "VertsAmount", "ExecutionTime", "Path",
        // "Cost", "Expected", "Delta";
        csv.begin();
        csv.write(sample.fileOrigin.filename());
        csv.write(pa.filename());
        csv.write(sample.vertsAmount);
        csv.write(dur.count());
        csv.write(pathStr.str());
        csv.write(cost);
        csv.write(sample.expectedCost);
        csv.write(delta);
        csv.end();
      }
    }

    std::cout << '\n';
  }

  return 0;
}