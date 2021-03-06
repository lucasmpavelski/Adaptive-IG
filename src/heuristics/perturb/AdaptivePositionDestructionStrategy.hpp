#pragma once

#include <paradiseo/eo/eo>
#include <paradiseo/mo/mo>

#include "position-selector/PositionSelector.hpp"
#include "continuators/myTimeStat.hpp"
#include "heuristics/FitnessReward.hpp"
#include "heuristics/perturb/DestructionConstruction.hpp"
#include "heuristics/perturb/DestructionStrategy.hpp"

template <class EOT>
class AdaptivePositionDestructionStrategy : public DestructionStrategy<EOT> {
  DestructionSize& destructionSize;
  FitnessRewards<EOT>& rewards;
  PositionSelector<EOT>& positionSelector;
  int rewardType;
  bool printRewards;
  bool printChoices;
  int iteration = 0;
  myTimeStat<EOT> time;

  auto choosePosition(EOT& sol) -> int {
    if (iteration > 9) {
      if (printRewards) {
        EOT sol;
        time(sol);
        std::cout << time.value() << ',' << rewards.initialGlobal() << ','
                  << rewards.lastGlobal() << ',' << rewards.initialLocal()
                  << ',' << rewards.lastLocal() << '\n';
      }
      positionSelector.feedback(reward());
    }
    iteration++;
    return positionSelector.select(sol);
  }

  [[nodiscard]] auto reward() -> double {
    double pf, cf;
    switch (rewardType) {
      case 0:
        pf = rewards.initialGlobal();
        cf = rewards.lastGlobal();
        break;
      case 1:
        pf = rewards.initialGlobal();
        cf = rewards.lastLocal();
        break;
      case 2:
        pf = rewards.initialLocal();
        cf = rewards.lastGlobal();
        break;
      case 3:
        pf = rewards.initialLocal();
        cf = rewards.lastLocal();
        break;
      default:
        throw std::runtime_error{"invalid rewardType"};
    }
    return (pf - cf) / pf;
  }
  
 public:
  AdaptivePositionDestructionStrategy(DestructionSize& destructionSize,
                                      PositionSelector<EOT>& positionSelector,
                                      FitnessRewards<EOT>& rewards,
                                      int rewardType,
                                      bool printRewards = false,
                                      bool printChoices = false)
      : destructionSize(destructionSize),
        rewards(rewards),
        positionSelector(positionSelector),
        rewardType(rewardType),
        printRewards(printRewards),
        printChoices(printChoices) {}

  auto operator()(EOT& sol) -> EOT override {
    int ds = std::min(destructionSize.value(), static_cast<int>(sol.size()));
    EOT removed;
    positionSelector.init(sol);
    for (int k = 0; k < ds; k++) {
      unsigned int index = choosePosition(sol);
      removed.push_back(sol[index]);
      sol.erase(sol.begin() + index);
    }
    return removed;
  }

 private:
};
