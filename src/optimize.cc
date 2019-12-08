#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <Chromosome.h>
#include <Loss.h>
#include <Pool.h>


int main(int argc, char const **argv) {
  Pool pool{50};

  if (argc > 1) {
    std::string const path{argv[1]};
    std::cout << "Loading population from file \"" << path << "\".\n";
    pool.Load(path);
  } else {
    std::cout << "Generating initial population...\n";
    pool.Populate();
    std::string const path{"snapshots/start.csv"};
    pool.Save(path);
    std::cout << "Initial population saved to file \"" << path << "\".\n";
  }

  int64_t const timestamp = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch()).count()
      - 1575800000;
  for (int generation = 0; generation < 5'000; ++generation) {
    pool.Evolve();
    if (generation % 100 == 0) {
      double const best_loss = pool.GetLoss(0.);
      std::cout << "Losses after generation " << generation << ":\n  "
          << std::lround(best_loss) << " (best), "
          << "+" << std::lround(pool.GetLoss(0.25) - best_loss) << " (25%), "
          << "+" << std::lround(pool.GetLoss(0.5) - best_loss) << " (median), "
          << "+" << std::lround(pool.GetLoss(0.75) - best_loss) << " (75%)\n";
      std::ostringstream path;
      path << "snapshots/" << timestamp << "_"
          << std::setfill('0') << std::setw(6) << generation << ".csv";
      pool.Save(path.str());
    }
  }

  auto const &result = pool.GetPopulation()[0];
  std::cout << "Best loss achieved: " << std::lround(result.loss) << '\n';
  result.Save("best.csv");

  return EXIT_SUCCESS;
}
