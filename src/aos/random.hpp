#pragma once

#include <random>
#include <vector>

#include "aos/adaptive_operator_selection.hpp"
#include "global.hpp"

template <class OpT>
class Random : public OperatorSelection<OpT> {
  std::uniform_int_distribution<int> dist;

 protected:
  auto selectOperatorIdx() -> int override { return dist(RNG::engine); }

 public:
  Random(const std::vector<OpT>& strategies)
      : OperatorSelection<OpT>{strategies}, dist(0, strategies.size() - 1) {}

  void update() final{};

  void doFeedback(double) final {}

  auto printOn(std::ostream& os) -> std::ostream& final {
    os << "  strategy: Random\n";
    return os;
  }
};