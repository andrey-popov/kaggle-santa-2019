#include <Loss.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>


int64_t Loss::Family::PreferenceLoss(int day) const {
  auto const choice = std::find(
      preferences.begin(), preferences.end(), day);
  int const choice_rank = choice - preferences.begin();
  switch (choice_rank) {
    case 0:
      return 0;
    case 1:
      return 50;
    case 2:
      return 50 + 9 * size;
    case 3:
      return 100 + 9 * size;
    case 4:
      return 200 + 9 * size;
    case 5:
      return 200 + 18 * size;
    case 6:
      return 300 + 18 * size;
    case 7:
      return 300 + 36 * size;
    case 8:
      return 400 + 36 * size;
    case 9:
      return 500 + (36 + 199) * size;
    default:
      return 500 + (36 + 398) * size;
  }
}


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
  for (auto const &family : families_)
    preference_loss += family.PreferenceLoss(chromosome.assignment[family.id]);

  return accounting_loss + preference_loss;
}


std::array<double, Chromosome::num_days> Loss::ScanDays(
    Chromosome const &chromosome, int family_id) const {
  std::array<double, Chromosome::num_days> losses;

  // Compute baseline without the specified family
  std::array<int, Chromosome::num_days> base_occupancy;
  std::fill(base_occupancy.begin(), base_occupancy.end(), 0);
  for (auto const &family : families_) {
    if (family.id == family_id)
      continue;
    base_occupancy[chromosome.assignment[family.id] - 1] += family.size;
  }

  int num_underpopulated_days = 0;
  int underpopulated_day = -1;
  bool overpopulated_day_found = false;
  for (int day = 1; day <= Chromosome::num_days; ++day) {
    if (base_occupancy[day - 1] < 125) {
      ++num_underpopulated_days;
      underpopulated_day = day;
    }
    else if (base_occupancy[day - 1] > 300) {
      overpopulated_day_found = true;
      break;
    }
  }
  if (num_underpopulated_days > 1 or overpopulated_day_found) {
    // Whichever day the family is assigned, the occupancy constraints cannot be
    // satisfied
    std::fill(losses.begin(), losses.end(),
              std::numeric_limits<double>::infinity());
    return losses;
  }

  double base_accounting_loss = 0.;
  int n_prev = base_occupancy.back();
  for (int i = base_occupancy.size() - 1; i >= 0; --i) {
    int const n = base_occupancy[i];
    base_accounting_loss += (n - 125) / 400.
        * std::pow(n, 0.5 + std::abs(n - n_prev) / 50.);
    n_prev = n;
  }

  int64_t base_preference_loss = 0;
  for (auto const &family : families_) {
    if (family.id == family_id)
      continue;
    base_preference_loss += family.PreferenceLoss(
      chromosome.assignment[family.id]);
  }

  for (int day = 1; day <= Chromosome::num_days; ++day) {
    if (num_underpopulated_days > 0 and day != underpopulated_day) {
      // If there is an underpopulated day, there is no point in checking others
      losses[day - 1] = std::numeric_limits<double>::infinity();
      continue;
    }
    int const n = base_occupancy[day - 1] + families_[family_id].size;
    if (n < 125 or n > 300) {
      losses[day - 1] = std::numeric_limits<double>::infinity();
      continue;
    }

    // In accounting loss, remove the previously included parts for this day
    // computed with the baseline occupancy and replace them with the parts
    // computed wih the updated occupancy
    double accounting_loss = base_accounting_loss;
    //...

    int const preference_loss = base_preference_loss
        + families_[family_id].PreferenceLoss(day);
    losses[day - 1] = accounting_loss + preference_loss;
  }

  return losses;
}
