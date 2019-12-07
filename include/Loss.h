#ifndef SANTA_WORKSHOP_LOSS_H_
#define SANTA_WORKSHOP_LOSS_H_

#include <array>
#include <string>
#include <vector>

#include <Chromosome.h>


class Loss {
 public:
  Loss(std::string const &path);

  std::array<int, 10> const &GetPreferences(int family) const {
    return families_[family].preferences;
  }

  double operator()(Chromosome const &chormosome) const;
  std::array<double, Chromosome::num_days> ScanDays(
      Chromosome const &chromosome, int family_id) const;

 private:
  struct Family {
    int64_t PreferenceLoss(int day) const;

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
