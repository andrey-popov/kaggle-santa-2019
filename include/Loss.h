#ifndef SANTA_WORKSHOP_LOSS_H_
#define SANTA_WORKSHOP_LOSS_H_

#include <array>
#include <string>
#include <vector>

#include <Chromosome.h>


class Loss {
 public:
  Loss(std::string const &path);

  std::vector<int> const &GetFamiliesForDay(int day, int preference) const {
    if (preference == -1)
      return day_to_families_[day - 1];
    else
      return day_pref_to_families_[day - 1][preference];
  }

  std::array<int, 10> const &GetPreferences(int family) const {
    return families_[family].preferences;
  }

  double operator()(Chromosome const &chormosome) const;

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

  void BuildInverseMap();

  std::vector<Family> families_;

  /**
   * \brief Mapping from days to families that can be assigned to those days
   *
   * First index is day - 1, second index is the preference order.
   */
  std::array<std::array<std::vector<int>, 10>, Chromosome::num_days>
  day_pref_to_families_;

  /**
   * \brief Same as \ref day_pref_to_families_ but without distribution between
   * different preferences
   */
  std::array<std::vector<int>, Chromosome::num_days> day_to_families_;
};

#endif  // SANTA_WORKSHOP_LOSS_H_
