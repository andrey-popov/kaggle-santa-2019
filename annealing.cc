#include <cmath>
#include <iostream>
#include <random>

#include <Chromosome.h>
#include <Loss.h>


int main() {
  Loss loss_calc{"family_data.csv"};
  std::mt19937 rng_engine{8855};
  std::uniform_int_distribution<> day_distr{1, Chromosome::num_days};
  std::uniform_int_distribution<> family_distr{0, Chromosome::num_families - 1};
  std::uniform_real_distribution<> unit_distr;

  Chromosome chromosome;
  for (int i = 0; i < Chromosome::num_families; ++i)
    chromosome.assignment[i] = day_distr(rng_engine);
  double prev_loss = loss_calc(chromosome);

  for (int64_t iteration = 0; iteration < 10'000'000; ++iteration) {
    double const temperature = 1000. * std::exp(-iteration / 1e6);
    int const family_id = family_distr(rng_engine);
    int const original_day = chromosome.assignment[family_id];
    chromosome.assignment[family_id] = day_distr(rng_engine);

    double const loss = loss_calc(chromosome);
    if (loss > prev_loss and
        unit_distr(rng_engine) > std::exp((prev_loss - loss) / temperature)) {
      // Reject the proposal
      chromosome.assignment[family_id] = original_day;
    } else
      prev_loss = loss;

    if (iteration % 100'000 == 0)
      std::cout << "Loss at iteraction " << iteration << ": "
          << std::lround(prev_loss) << std::endl;
  }

  std::cout << "Best loss achieved: " << prev_loss << '\n';
  chromosome.Save("best.csv");

  return EXIT_SUCCESS;
}
