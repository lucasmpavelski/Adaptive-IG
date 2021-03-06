#pragma once

#include <paradiseo/mo/mo>
#include <unordered_map>

#include "continuators/myTimeStat.hpp"
#include "heuristics.hpp"
#include "heuristics/aco.hpp"
#include "heuristics/hc.hpp"
#include "heuristics/ig.hpp"
#include "heuristics/ihc.hpp"
#include "heuristics/ils.hpp"
#include "heuristics/isa.hpp"
#include "heuristics/sa.hpp"
#include "heuristics/ts.hpp"
#include "heuristics/IGBP.hpp"
#include "heuristics/neh.hpp"

#include "RunOptions.hpp"
#include "eoFSPFactory.hpp"
#include "heuristics/FitnessReward.hpp"

template <class EOT>
class RewardPrinter : public moStatBase<EOT> {
  FitnessRewards<EOT>& rewards;
  myTimeStat<EOT> timer;

 public:
  RewardPrinter(FitnessRewards<EOT>& rewards) : rewards{rewards} {
  }

  auto header() -> std::string {
    return "runtime,initial_local,last_local,initial_global,last_global\n";
  }

  void operator()(EOT&) override {}

  void lastCall(EOT& sol) override {
    timer(sol);
    std::cout << rewards.initialLocal() << ',' << rewards.lastLocal() << ','
              << rewards.initialGlobal() << ',' << rewards.lastGlobal() << '\n';
  }
};

inline auto solveWith(
    std::string mh,
    const std::unordered_map<std::string, std::string>& problem_specs,
    const std::unordered_map<std::string, std::string>& params_values,
    const RunOptions runOptions = RunOptions()) -> Result {
  FSPProblem prob = FSPProblemFactory::get(problem_specs);
  MHParamsSpecs specs = MHParamsSpecsFactory::get(mh);
  MHParamsValues params(&specs);
  params.readValues(params_values);

  eoFSPFactory factory{params, prob};

  if (mh == "all") {
    mh = params.categoricalName("MH");
  }

  FitnessRewards<FSP> rewards;
  RewardPrinter<FSP> rewardPrinter{rewards};
  if (runOptions.printFitnessReward) {
    std::cout << rewardPrinter.header();
    prob.checkpoint().add(rewards.localStat());
    prob.checkpointGlobal().add(rewards.globalStat());
    prob.checkpointGlobal().add(rewardPrinter);
  }

  if (mh == "NEH")
    return solveWithNEH(prob, factory, runOptions);
  else if (mh == "HC")
    return solveWithHC(prob, factory, runOptions);
  else if (mh == "SA")
    return solveWithSA(prob, params);
  else if (mh == "IHC")
    return solveWithIHC(prob, params);
  else if (mh == "ISA")
    return solveWithISA(prob, params);
  else if (mh == "TS")
    return solveWithTS(prob, params);
  else if (mh == "IG")
    return solveWithIG(prob, factory, runOptions);
  else if (mh == "ILS")
    return solveWithILS(prob, params);
  else if (mh == "ACO")
    return solveWithACO(prob, params);
  else if (mh == "IGBP")
    return solveWithIGBP(prob, params, runOptions);
  else
    throw std::runtime_error("Unknown MH: " + mh);
  return {};
}