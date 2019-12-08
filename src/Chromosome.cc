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


uint32_t Chromosome::Hash() const {
  // Use an implementation from Boost
  // [1] https://stackoverflow.com/a/27216842
  uint32_t seed = Chromosome::num_families;
  for (auto const &day : assignment)
    seed ^= day + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  return seed;
}


void Chromosome::Save(std::string const &path) const {
  std::ofstream file{path};
  file << "family_id,assigned_day\n";
  for (int i = 0; i < num_families; ++i)
    file << i << ',' << assignment[i] << '\n';
  file.close();
}


std::ostream &operator<<(std::ostream &stream, Chromosome const &chromosome) {
  stream << chromosome.assignment[0];
  for (int i = 1; i < Chromosome::num_families; ++i)
    stream << " " << chromosome.assignment[i];
  return stream;
}
