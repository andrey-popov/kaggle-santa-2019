#ifndef SANTA_WORKSHOP_POOL_H_
#define SANTA_WORKSHOP_POOL_H_

#include <random>
#include <string>
#include <vector>

#include <Chromosome.h>
#include <Loss.h>


class Pool {
 public:
  Pool(int capacity, int tournament_size, double crossover_prob,
       double mutation_prob, double survivor_rank_scale);

  /// Performs one generation
  void Evolve();

  /// Returns loss at the given quantile of the population
  double GetLoss(double quantile) const;

  std::vector<Chromosome> const &GetPopulation() const {
    return population_;
  }

  /// Rule-based improvement of the best solution
  void Improve(int num_top_cost);
  
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
  /**
   * \brief Performs a cross-over between the given chromosomes
   *
   * The probability of cross-over is controlled by dedicated parameter. Values
   * of the loss function for the children are not computed.
   */
  std::tuple<Chromosome, Chromosome> CrossOver(
      Chromosome const &parent1, Chromosome const &parent2,
      int strategy = 1) const;


  Chromosome Mutate(Chromosome const &source) const;

  Chromosome const *SelectParent(Chromosome const *skip = nullptr);

  /**
   * \brief Precompute ranks of surviving phenotypes
   *
   * Will compute these ranks only once and then use them for all generations.
   * Randomness in construction of the new generation is provided by the ranking
   * of phenotypes, and there is no need to choose the surviving ranks randomly
   * each time.
   */
  void PreselectSurvivors();

  /// Target size of the popolation
  int capacity_;

  int tournament_size_;
  double crossover_prob_;
  double mutation_prob_;

  /**
   * \brief Scale for exponential distribution of surviving ranks
   *
   * Defined as a fraction of the capacity.
   */
  double survivor_rank_scale_;

  Loss const loss_;
  mutable std::mt19937 rng_engine_;

  /**
   * \brief Population
   * 
   * In between generations it is sorted by loss.
   */
  std::vector<Chromosome> population_;

  /**
   * \brief Precomputed ranks of surviving phenotypes
   *
   * Correspond to indices of sorted union of the current population and
   * generated children.
   */
  std::vector<int> survivor_ranks_;
};


#endif  // SANTA_WORKSHOP_POOL_H_
