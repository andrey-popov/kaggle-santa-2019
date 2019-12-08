#include <Pool.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <set>
#include <sstream>
#include <stdexcept>


Pool::Pool(int capacity, int tournament_size, double crossover_prob,
           double mutation_prob, double survivor_rank_scale)
    : capacity_{capacity}, tournament_size_{tournament_size},
      crossover_prob_{crossover_prob}, mutation_prob_{mutation_prob},
      survivor_rank_scale_{survivor_rank_scale},
      loss_{"family_data.csv"}, rng_engine_{717} {
  PreselectSurvivors();
}


double Pool::GetLoss(double quantile) const {
  int const i = std::lround(quantile * population_.size());
  return population_.at(i).loss;
}


void Pool::Evolve() {
  std::vector<Chromosome> new_population;
  new_population.reserve(2 * capacity_);

  std::set<uint32_t> known_hashes;
  for (auto const &c : population_)
    known_hashes.emplace(c.Hash());

  // Generate children
  while (int(new_population.size()) < capacity_) {
    Chromosome const *parent1 = SelectParent();
    Chromosome const *parent2 = SelectParent(parent1);
    auto [child1, child2] = CrossOver(*parent1, *parent2);

    for (auto const &child : {child1, child2}) {
      Chromosome mutant{Mutate(child)};
      mutant.loss = loss_(mutant);
      if (std::isfinite(mutant.loss)) {
        auto const hash = mutant.Hash();
        if (known_hashes.count(hash) == 0) {
          known_hashes.emplace(hash);
          new_population.emplace_back(mutant);
        }
      }
    }
  }

  // Choose phenotypes out of the union of the previous generation and the
  // children
  new_population.insert(new_population.end(),
                        population_.begin(), population_.end());
  std::sort(new_population.begin(), new_population.end(),
            [](auto const &c1, auto const &c2){return c1.loss < c2.loss;});
  for (int i = 0; i < capacity_; ++i)
    population_[i] = new_population[survivor_ranks_[i]];
}


void Pool::Load(std::string const &path) {
  std::ifstream file{path};
  std::string line;
  while (std::getline(file, line)) {
    Chromosome phenotype;
    int f = 0;
    std::string word;
    std::istringstream line_stream{line};
    while (std::getline(line_stream, word, ',')) {
      phenotype.assignment[f] = std::stoi(word);
      ++f;
    }
    phenotype.loss = loss_(phenotype);
    population_.emplace_back(phenotype);
  }
  file.close();

  if (int(population_.size()) != capacity_)
    throw std::runtime_error{"Read an unexpected number of phenotypes."};
  std::sort(population_.begin(), population_.end(),
            [](auto const &c1, auto const &c2){return c1.loss < c2.loss;});
}


void Pool::Populate() {
  std::uniform_int_distribution<> day_distr{1, Chromosome::num_days};
  for (int n = 0; n < capacity_; ++n) {
    // Generate a random chromosome as an initial guess
    Chromosome candidate;
    while (true) {
      for (int ifamily = 0; ifamily < Chromosome::num_families; ++ifamily)
        candidate.assignment[ifamily] = day_distr(rng_engine_);
      
      candidate.loss = loss_(candidate);
      if (std::isfinite(candidate.loss))
        break;
    }

    // For each family, try the preferred days and see if this improves the
    // loss. Iterate over families in a random order.
    std::array<int, Chromosome::num_families> family_indices;
    for (int i = 0; i < Chromosome::num_families; ++i)
      family_indices[i] = i;
    std::shuffle(family_indices.begin(), family_indices.end(), rng_engine_);

    for (auto const &f : family_indices) {
      int best_day = candidate.assignment[f];
      double min_loss = candidate.loss;
      auto const &preferences = loss_.GetPreferences(f);

      for (auto const &day : preferences) {
        candidate.assignment[f] = day;
        double const loss = loss_(candidate);
        if (loss < min_loss) {
          best_day = day;
          min_loss = loss;
        }
      }

      candidate.assignment[f] = best_day;
      candidate.loss = min_loss;
    }
    
    population_.emplace_back(candidate);
  }

  std::sort(population_.begin(), population_.end(),
            [](auto const &c1, auto const &c2){return c1.loss < c2.loss;});
}


void Pool::Save(std::string const &path) const {
  std::ofstream file{path};
  for (auto const &phenotype : population_) {
    file << phenotype.assignment[0];
    for (int f = 1; f < Chromosome::num_families; ++f)
      file << "," << phenotype.assignment[f];
    file << '\n';
  }

  file.close();
}


std::tuple<Chromosome, Chromosome> Pool::CrossOver(
    Chromosome const &parent1, Chromosome const &parent2,
    int strategy) const {
  std::uniform_real_distribution<> unit_distr;
  if (unit_distr(rng_engine_) > crossover_prob_) {
    // Return clones of the parents
    return {parent1, parent2};
  }

  Chromosome child1, child2;
  switch (strategy) {
    case 0: {
      // One-point cross-over
      std::uniform_int_distribution<> index_distr{
          0, Chromosome::num_families - 1};
      int const crossover_index = index_distr(rng_engine_);
      for (int i = 0; i < crossover_index; ++i) {
        child1.assignment[i] = parent1.assignment[i];
        child2.assignment[i] = parent2.assignment[i];
      }
      for (int i = crossover_index; i < Chromosome::num_families; ++i) {
        child1.assignment[i] = parent2.assignment[i];
        child2.assignment[i] = parent1.assignment[i];
      }
      break;
    }
    case 1: {
      // Universal cross-over
      for (int i = 0; i < Chromosome::num_families; ++i) {
        if (unit_distr(rng_engine_) < 0.5) {
          child1.assignment[i] = parent1.assignment[i];
          child2.assignment[i] = parent2.assignment[i];
        } else {
          child1.assignment[i] = parent2.assignment[i];
          child2.assignment[i] = parent1.assignment[i];
        }
      }
    }
  }
  return {child1, child2};
}


Chromosome Pool::Mutate(Chromosome const &source) const {
  std::uniform_real_distribution unit_distr;
  if (unit_distr(rng_engine_) > mutation_prob_)
    return source;

  Chromosome mutated{source};
  int const num_strategies = 4;
  std::uniform_int_distribution<> strategy_distr{0, num_strategies - 1};
  std::uniform_int_distribution<> day_distr{1, Chromosome::num_days};
  std::uniform_int_distribution<> family_distr{0, Chromosome::num_families - 1};

  switch (strategy_distr(rng_engine_)) {
    case 0: {
      // For one family, set the assigned day to one of its preference days
      int const f = family_distr(rng_engine_);
      std::uniform_int_distribution<> pref_distr{0, 9};
      mutated.assignment[f] = loss_.GetPreferences(f)[pref_distr(rng_engine_)];
      break;
    }
    case 1:
      // Randomly change the day assigned to one family
      mutated.assignment[family_distr(rng_engine_)] = day_distr(rng_engine_);
      break;
    case 2: {
      // Swap days assigned to two families
      int const f1 = family_distr(rng_engine_);
      int const f2 = family_distr(rng_engine_);
      std::swap(mutated.assignment[f1], mutated.assignment[f2]);
      break;
    }
    case 3: {
      // Shuffle days assigned to a triplet of families
      int const f1 = family_distr(rng_engine_);
      int const f2 = family_distr(rng_engine_);
      int const f3 = family_distr(rng_engine_);
      int const day = mutated.assignment[f1];
      mutated.assignment[f1] = mutated.assignment[f2];
      mutated.assignment[f2] = mutated.assignment[f3];
      mutated.assignment[f3] = day;
      break;
    }
  }
  
  return mutated;
}


Chromosome const *Pool::SelectParent(Chromosome const *skip) {
  std::uniform_int_distribution<> index_distr{0, capacity_ - 1};
  std::set<Chromosome const *> candidates;
  while (int(candidates.size()) < tournament_size_) {
    Chromosome const *c = &population_[index_distr(rng_engine_)];
    if (c != skip)
      candidates.insert(c);
  }

  return *std::min_element(
      candidates.begin(), candidates.end(),
      [](auto const &c1, auto const c2){return c1->loss < c2->loss;});
}


void Pool::PreselectSurvivors() {
  if (survivor_rank_scale_ <= 0.1) {
    // Threshold selection
    for (int r = 0; r < capacity_; ++r)
      survivor_ranks_.emplace_back(r);
  } else {
    std::set<int> ranks;
    ranks.insert(0);  // Make sure the best phenotype is always kept
    
    std::vector<double> weights;
    for (int r = 0; r < 2 * capacity_; ++r)
      weights.emplace_back(std::exp(-r / (survivor_rank_scale_ * capacity_)));
    std::discrete_distribution<> rank_distr{weights.begin(), weights.end()};
    while (int(ranks.size()) < capacity_)
      ranks.insert(rank_distr(rng_engine_));

    for (auto const &r : ranks)
      survivor_ranks_.emplace_back(r);
    std::sort(survivor_ranks_.begin(), survivor_ranks_.end());
  }
}
