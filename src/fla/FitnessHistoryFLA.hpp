#pragma once

#include "fla/FitnessHistory.hpp"
#include "fla/FitnessLandscapeMetric.hpp"

template <class EOT>
class FitnessHistoryFLA : public FitnessLandscapeMetric, public eoFunctorBase {
  const FitnessHistory<EOT>& fitnessHistory;

 public:
  FitnessHistoryFLA(const FitnessHistory<EOT>& fitnessHistory)
      : fitnessHistory{fitnessHistory} {}

  double compute() override {
    return compute(fitnessHistory.cbegin(), fitnessHistory.cend());
  }

 protected:
  using citerator = typename FitnessHistory<EOT>::citerator;

  virtual double compute(citerator begin, citerator end) = 0;
};
