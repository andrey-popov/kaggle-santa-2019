#include <iomanip>
#include <iostream>

#include <Chromosome.h>
#include <Loss.h>


int main() {
  Loss loss{"family_data.csv"};
  Chromosome sample_submission{"sample_submission.csv"};
  std::cout << "Loss on sample submission: "
      << std::fixed << std::setprecision(3) << loss(sample_submission)
      << " (references is " << 10641498.403135022 << ")\n";
  return EXIT_SUCCESS;
}
