#include <iomanip>
#include <iostream>

#include <Chromosome.h>
#include <Loss.h>
#include <Pool.h>


int main() {
  Loss loss{"family_data.csv"};
  Chromosome sample_submission{"sample_submission.csv"};
  std::cout << "Loss on sample submission: "
      << std::fixed << std::setprecision(3) << loss(sample_submission)
      << " (references is " << 10641498.403135022 << ")\n";

  Pool pool{50, loss};
  for (int generation = 0; generation < 10000; ++generation) {
    pool.Evolve();
    if (generation % 100 == 0)
      std::cout << "Best loss after generation " << generation << ": "
          << pool.BestLoss() << '\n';
  }

  auto const &result = pool.GetPopulation()[0];
  std::cout << "Best loss achieved: " << result.loss << '\n';
  result.Save("best.csv");

  return EXIT_SUCCESS;
}
