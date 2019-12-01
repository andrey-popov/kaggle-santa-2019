#ifndef SANTA_WORKSHOP_CHROMOSOME_H_
#define SANTA_WORKSHOP_CHROMOSOME_H_

#include <array>
#include <string>


struct Chromosome {
  /// Construct from a CSV file
  Chromosome(std::string const &path);

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
};

#endif  // SANTA_WORKSHOP_CHROMOSOME_H_
