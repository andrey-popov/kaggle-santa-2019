#include <cmath>
#include <iostream>

#include <Chromosome.h>
#include <Loss.h>
#include <Pool.h>


int main() {
  Pool pool{50};
  for (int generation = 0; generation < 10000; ++generation) {
    pool.Evolve();
    if (generation % 100 == 0)
      std::cout << "Best loss after generation " << generation << ": "
          << std::lround(pool.BestLoss()) << '\n';
  }

  auto const &result = pool.GetPopulation()[0];
  std::cout << "Best loss achieved: " << result.loss << '\n';
  result.Save("best.csv");

  return EXIT_SUCCESS;
}
