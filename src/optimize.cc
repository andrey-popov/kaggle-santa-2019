#include <cmath>
#include <iostream>

#include <Chromosome.h>
#include <Loss.h>
#include <Pool.h>


int main() {
  Pool pool{50};
  for (int generation = 0; generation < 10000; ++generation) {
    pool.Evolve();
    if (generation % 100 == 0) {
      double const best_loss = pool.GetLoss(0.);
      std::cout << "Losses after generation " << generation << ":\n  "
          << std::lround(best_loss) << " (best), "
          << "+" << std::lround(pool.GetLoss(0.25) - best_loss) << " (25%), "
          << "+" << std::lround(pool.GetLoss(0.5) - best_loss) << " (median), "
          << "+" << std::lround(pool.GetLoss(0.75) - best_loss) << " (75%)\n";
    }
  }

  auto const &result = pool.GetPopulation()[0];
  std::cout << "Best loss achieved: " << result.loss << '\n';
  result.Save("best.csv");

  return EXIT_SUCCESS;
}
