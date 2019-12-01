#include <Loss.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>


Loss::Loss(std::string const &path) {
  std::ifstream file{path};
  std::string line;
  std::getline(file, line);  // Skip CSV header
  
  while (std::getline(file, line)) {
    std::istringstream line_stream{line};
    std::string word;
    std::array<int, 12> line_parsed;
    
    int i = 0;
    while (std::getline(line_stream, word, ',')) {
      if (i >= int(line_parsed.size()))
        throw std::runtime_error(
            "Unexpected number of elements on a line in families file.");
      line_parsed[i] = std::stoi(word);
      ++i;
    }

    Family family;
    family.id = line_parsed[0];
    family.size = line_parsed[11];
    for (int i = 1; i < 11; ++i)
      family.preferences[i - 1] = line_parsed[i];
    families_.emplace_back(family);
  }

  file.close();

  if (families_.size() != Chromosome::num_families)
    throw std::runtime_error("Unexpected number of families found.");
}


double Loss::operator()(Chromosome const &chromosome) const {
  std::array<int, Chromosome::num_days> occupancy;
  std::fill(occupancy.begin(), occupancy.end(), 0);
  for (auto const &family : families_)
    occupancy[chromosome.assignment[family.id] - 1] += family.size;

  for (auto const &n : occupancy)
    if (n < 125 or n > 300)
      return std::numeric_limits<double>::infinity();

  double accounting_loss = 0.;
  int n_prev = occupancy.back();
  for (int i = occupancy.size() - 1; i >= 0; --i) {
    int const n = occupancy[i];
    accounting_loss += (n - 125) / 400.
        * std::pow(n, 0.5 + std::abs(n - n_prev) / 50.);
    n_prev = n;
  }

  int64_t preference_loss = 0;
  for (auto const &family : families_) {
    auto const choice = std::find(
        family.preferences.begin(), family.preferences.end(),
        chromosome.assignment[family.id]);
    int const choice_rank = choice - family.preferences.begin();
    switch (choice_rank) {
      case 0:
        break;
      case 1:
        preference_loss += 50;
        break;
      case 2:
        preference_loss += 50 + 9 * family.size;
        break;
      case 3:
        preference_loss += 100 + 9 * family.size;
        break;
      case 4:
        preference_loss += 200 + 9 * family.size;
        break;
      case 5:
        preference_loss += 200 + 18 * family.size;
        break;
      case 6:
        preference_loss += 300 + 18 * family.size;
        break;
      case 7:
        preference_loss += 300 + 36 * family.size;
        break;
      case 8:
        preference_loss += 400 + 36 * family.size;
        break;
      case 9:
        preference_loss += 500 + (36 + 199) * family.size;
        break;
      default:
        preference_loss += 500 + (36 + 398) * family.size;
    }
  }

  return accounting_loss + preference_loss;
}
