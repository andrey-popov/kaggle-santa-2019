#ifndef SANTA_WORKSHOP_LOSS_H_
#define SANTA_WORKSHOP_LOSS_H_

#include <array>
#include <string>
#include <vector>

#include <Chromosome.h>


class Loss {
 public:
  Loss(std::string const &path);
  double operator()(Chromosome const &chormosome) const;

 private:
  struct Family {
    /// ID of this family
    int id;
    
    /// Number of family members
    int size;

    /// Preferred days, in the decreasing order of preference
    std::array<int, 10> preferences;
  };

  std::vector<Family> families_;
};

#endif  // SANTA_WORKSHOP_LOSS_H_
