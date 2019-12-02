#ifndef SANTA_WORKSHOP_CHROMOSOME_H_
#define SANTA_WORKSHOP_CHROMOSOME_H_

#include <array>
#include <string>


struct Chromosome {
  Chromosome() = default;

  /// Construct from a CSV file
  Chromosome(std::string const &path);

  /// Save chromosome in a CSV file
  void Save(std::string const &path) const;

  /// Number of days for visits
  static constexpr int num_days = 100;

  /// Number of families
  static constexpr int num_families = 5000;

  /**
   * \brief Day assigned for each family
   *
   * Days are indexed starting from 1.
   */
  std::array<int, num_families> assignment;

  /// Loss associated with this chromosome
  double loss;
};

#endif  // SANTA_WORKSHOP_CHROMOSOME_H_
