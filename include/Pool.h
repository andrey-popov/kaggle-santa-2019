#ifndef SANTA_WORKSHOP_POOL_H_
#define SANTA_WORKSHOP_POOL_H_

#include <random>
#include <string>
#include <vector>

#include <Chromosome.h>
#include <Loss.h>


class Pool {
 public:
  Pool(int capacity);

  /// Returns loss at the given quantile of the population
  double GetLoss(double quantile) const;

  std::vector<Chromosome> const &GetPopulation() const {
    return population_;
  }

  /// Performs one generation
  void Evolve();

  /// Loads population from a CSV file
  void Load(std::string const &path);

  /// Generate initial popolation
  void Populate();

  /**
   * \brief Saves the population in a CSV file
   *
   * Each line describes one phenotype from the population.
   */
  void Save(std::string const &path) const;

 private:
  Chromosome CrossOver(Chromosome const &parent1,
                       Chromosome const &parent2) const;
  std::vector<Chromosome> GenerateChildren(
      std::vector<Chromosome const *> const &breeding_pool) const;
  Chromosome Mutate(Chromosome const &source) const;

  std::vector<Chromosome const *> SelectParents();

  /// Target size of the popolation
  int capacity_;

  /// Size of breeding pool
  int num_breeding_;

  int tournament_size_;
  int num_elites_;

  Loss const loss_;
  mutable std::mt19937 rng_engine_;

  /**
   * \brief Population
   * 
   * In between generations it is sorted by loss.
   */
  std::vector<Chromosome> population_;
};


#endif  // SANTA_WORKSHOP_POOL_H_
