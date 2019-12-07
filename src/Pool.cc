#include <Pool.h>

#include <algorithm>
#include <cmath>
#include <set>


Pool::Pool(int capacity, Loss const &loss)
    : capacity_{capacity}, num_breeding_(capacity * 0.7),
      tournament_size_{2}, num_elites_{1},
      mutation_probability_{1e-3},
      loss_{loss}, rng_engine_{717} {
  Populate(capacity);
}


void Pool::Evolve() {
  std::vector<Chromosome> new_population;
  new_population.reserve(capacity_);

  // Copy elites directly
  for (int i = 0; i < num_elites_; ++i)
    new_population.emplace_back(population_[i]);

  auto const breeding_pool = SelectParents();
  for (auto &child : GenerateChildren(breeding_pool))
    new_population.emplace_back(child);

  std::sort(new_population.begin(), new_population.end(),
            [](auto const &c1, auto const &c2){return c1.loss < c2.loss;});
  population_ = new_population;
}


Chromosome Pool::CrossOver(Chromosome const &parent1,
                           Chromosome const &parent2) const {
  Chromosome child;
  std::uniform_int_distribution<> index_distr{0, Chromosome::num_families - 1};
  int const cross_over_index = index_distr(rng_engine_);
  for (int i = 0; i < cross_over_index; ++i)
    child.assignment[i] = parent1.assignment[i];
  for (int i = cross_over_index; i < Chromosome::num_families; ++i)
    child.assignment[i] = parent2.assignment[i];
  return child;
}


std::vector<Chromosome> Pool::GenerateChildren(
    std::vector<Chromosome const *> const &breeding_pool) const {
  std::vector<Chromosome> children;
  children.reserve(capacity_ - num_elites_);
  std::uniform_int_distribution<> index_distr{0, num_breeding_ - 1};

  while (int(children.size()) < capacity_ - num_elites_) {
    Chromosome const *parent1 = breeding_pool[index_distr(rng_engine_)];
    Chromosome const *parent2 = parent1;
    while (parent2 == parent1)
      parent2 = breeding_pool[index_distr(rng_engine_)];

    Chromosome child = Mutate(CrossOver(*parent1, *parent2));
    child.loss = loss_(child);
    if (std::isfinite(child.loss))
      children.emplace_back(child);
  }

  return children;
}


Chromosome Pool::Mutate(Chromosome const &source) const {
  Chromosome mutated{source};
  std::uniform_real_distribution<> unit_distr;
  std::uniform_int_distribution<> day_distr{1, Chromosome::num_days};
  
  for (int igene = 0; igene < Chromosome::num_families; ++igene) {
    if (unit_distr(rng_engine_) > mutation_probability_)
      continue;

    mutated.assignment[igene] = day_distr(rng_engine_);
  }

  return mutated;
}


void Pool::Populate(int population_size) {
  std::uniform_int_distribution<> day_distr{1, Chromosome::num_days};
  for (int n = 0; n < population_size; ++n) {
    while (true) {
      Chromosome candidate;
      for (int ifamily = 0; ifamily < Chromosome::num_families; ++ifamily)
        candidate.assignment[ifamily] = day_distr(rng_engine_);
      
      candidate.loss = loss_(candidate);
      if (std::isfinite(candidate.loss)) {
        population_.emplace_back(candidate);
        break;
      }
    }
  }

  std::sort(population_.begin(), population_.end(),
            [](auto const &c1, auto const &c2){return c1.loss < c2.loss;});
}


std::vector<Chromosome const *> Pool::SelectParents() {
  std::vector<Chromosome const *> breeding_pool;
  breeding_pool.reserve(num_breeding_);
  std::uniform_int_distribution<> index_distr{0, capacity_ - 1};

  while (int(breeding_pool.size()) < num_breeding_) {
    std::set<int> candidate_indices;
    while (int(candidate_indices.size()) < tournament_size_)
      candidate_indices.insert(index_distr(rng_engine_));

    int const winner_index = *std::min_element(
        candidate_indices.begin(), candidate_indices.end(),
        [&](int i, int j){return population_[i].loss < population_[j].loss;});
    breeding_pool.emplace_back(&population_[winner_index]);
  }

  return breeding_pool;
}
