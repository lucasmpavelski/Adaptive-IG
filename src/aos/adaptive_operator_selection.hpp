#pragma once

#include <iostream>
#include <vector>

#include <paradiseo/eo/eo>

#include "continuators/myTimeStat.hpp"
#include "global.hpp"
#include "heuristics/falseContinuator.hpp"

template <typename OpT>
class OperatorSelection : public eoFunctorBase {
public:
  struct DummyNgh { using EOT = int; };
  using Continuator = moContinuator<DummyNgh>;
private:

  std::vector<OpT> operators;
  enum class WarmUpStrategy {
    RANDOM,
    FIXED
  } warmUpStrategy = WarmUpStrategy::FIXED;
  Continuator*  warmupContinuator;
  int fixedWarmUpParameter = 0;
  falseContinuator<DummyNgh> noWarmUp;
  bool warmingUp = true;

 protected:
  virtual auto selectOperatorIdx() -> int = 0;

 public:
  // lifecycle
  OperatorSelection(std::vector<OpT> operators)
      : operators{std::move(operators)}, warmupContinuator{&noWarmUp} {};

  // accessors
  [[nodiscard]] auto noOperators() const -> int { return operators.size(); };
  [[nodiscard]] auto doAdapt() const -> bool { return noOperators() > 1; };

  void setWarmUp(Continuator& warmup,
                 std::string        strategyStr,
                 int                fixedWarmUpParameter) {
    std::transform(begin(strategyStr), end(strategyStr), begin(strategyStr),
                   tolower);
    WarmUpStrategy strategy = WarmUpStrategy::FIXED;
    if (strategyStr == "random")
      strategy = WarmUpStrategy::RANDOM;
    setWarmUp(warmup, strategy, fixedWarmUpParameter);
  }

  void setWarmUp(Continuator& warmup,
                 WarmUpStrategy     strategy,
                 int                fixedWarmUpParameter) {
    this->warmupContinuator    = &warmup;
    this->warmUpStrategy       = strategy;
    this->fixedWarmUpParameter = fixedWarmUpParameter;
    typename DummyNgh::EOT dummy;
    warmup.init(dummy);
  }

  // main interface
  virtual void reset(double){};  // on init algorithm
  virtual void doFeedback(double){};
  virtual void update(){};  // on finish generation
  virtual auto printOn(std::ostream& os) -> std::ostream& = 0;

  void feedback(const double reward) {
    if (warmingUp) 
      return;
    doFeedback(reward);
  } 

  auto selectOperator() -> OpT& {
    int dummy;
    if ((*warmupContinuator)(dummy)) {
      switch (warmUpStrategy) {
        case WarmUpStrategy::FIXED:
          return operators[fixedWarmUpParameter];
        case WarmUpStrategy::RANDOM:
          return rng.choice(operators);
      }
    } else {
      warmingUp = false;
    }
    auto op_idx = selectOperatorIdx();
    return operators[op_idx];
  };

  auto operatorsRef() const -> const std::vector<OpT>& {
    return operators;
  }

  // misc
  template <typename OpT2>
  friend auto operator<<(std::ostream& os, OperatorSelection<OpT2> const& aos)
      -> std::ostream& {
    os << "AdaptiveOperatorSelection:\n";
    if (aos.doAdapt()) {
      os << "  operators:";
      for (const OpT& op : aos.operators)
        os << "\n" << op;
    } else {
      os << "  constant_operator:\n" << aos.operators[0];
    }
    return aos.printOn(os);
  }
};
