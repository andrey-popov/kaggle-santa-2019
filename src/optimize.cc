#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>

#include <Chromosome.h>
#include <Loss.h>
#include <Pool.h>

namespace po = boost::program_options;


int main(int argc, char const **argv) {
  po::options_description options{"Supported options"};
  options.add_options()
      ("help,h", "Prints help message.")
      ("population,p", po::value<std::string>()->default_value(""),
       "File with initial population.")
      ("num,n", po::value<int>()->default_value(5000), "Number of generations.")
      ("size", po::value<int>()->default_value(50), "Size of the population.")
      ("tournament,t", po::value<int>()->default_value(2), "Tournament size.")
      ("crossover,c", po::value<double>()->default_value(0.5),
       "Cross-over probability.")
      ("mutation,m", po::value<double>()->default_value(1.),
       "Mutation probability.")
      ("survival,s", po::value<double>()->default_value(0.5),
       "Relative exponential for surviving ranks.");
  po::variables_map options_map;
  po::store(po::command_line_parser(argc, argv).options(options).run(),
            options_map);
  if (options_map.count("help")) {
    std::cerr << "Usage: optimize [options]\n";
    std::cerr << options << std::endl;
    return EXIT_FAILURE;
  }

  Pool pool{
      options_map["size"].as<int>(), options_map["tournament"].as<int>(),
      options_map["crossover"].as<double>(),
      options_map["mutation"].as<double>(),
      options_map["survival"].as<double>()};

  if (auto const path = options_map["population"].as<std::string>();
      not path.empty()) {
    std::cout << "Loading population from file \"" << path << "\".\n";
    pool.Load(path);
  } else {
    std::cout << "Generating initial population...\n";
    pool.Populate();
    std::string const default_path{"snapshots/start.csv"};
    pool.Save(default_path);
    std::cout << "Initial population saved to file \"" << default_path
        << "\".\n";
  }

  int64_t const timestamp = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch()).count()
      - 1575800000;
  std::cout << "Timestamp of this run: " << timestamp << std::endl;
  int const num_generations = options_map["num"].as<int>();
  for (int generation = 0; generation <= num_generations; ++generation) {
    pool.Evolve();
    if (generation > 0 and generation % 1000 == 0) {
      double best_loss = pool.GetLoss(0.);
      std::cout << "Losses after generation " << generation << ":\n  "
          << std::lround(best_loss) << " (best), "
          << "+" << std::lround(pool.GetLoss(0.25) - best_loss) << " (25%), "
          << "+" << std::lround(pool.GetLoss(0.5) - best_loss) << " (median), "
          << "+" << std::lround(pool.GetLoss(0.75) - best_loss) << " (75%)\n";
      
      pool.Improve(500);
      best_loss = pool.GetLoss(0.);
      std::cout << "Best loss after improvement: "
         << std::lround(best_loss) << '\n';
      if (generation % 5000 == 0) {
        pool.Improve(Chromosome::num_families);
        best_loss = pool.GetLoss(0.);
        std::cout << "Best loss after improvement for all families: "
            << std::lround(best_loss) << '\n';
      }

      std::ostringstream path;
      path << "snapshots/" << timestamp << "_"
          << std::setfill('0') << std::setw(6) << generation << "_"
          << std::lround(best_loss) << ".csv";
      pool.Save(path.str());
    }
  }

  auto const &result = pool.GetPopulation()[0];
  std::cout << "Best loss achieved: " << std::lround(result.loss) << '\n';
  result.Save("best.csv");

  return EXIT_SUCCESS;
}
