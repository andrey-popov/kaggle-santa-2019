#include <Chromosome.h>

#include <fstream>
#include <sstream>


Chromosome::Chromosome(std::string const &path) {
  std::ifstream file{path};
  std::string line;
  std::getline(file, line);  // Skip CSV header

  while (std::getline(file, line)) {
    std::istringstream line_stream{line};
    std::string word;

    std::getline(line_stream, word, ',');
    int const family_id = std::stoi(word);
    std::getline(line_stream, word, ',');
    int const day = std::stoi(word);
    assignment[family_id] = day;
  }
}


void Chromosome::Save(std::string const &path) const {
  std::ofstream file{path};
  file << "family_id,assigned_day\n";
  for (int i = 0; i < num_families; ++i)
    file << i << ',' << assignment[i] << '\n';
  file.close();
}
