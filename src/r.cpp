#include <Rcpp.h>
using namespace Rcpp;

#include "MHParamsSpecsFactory.hpp"
#include "FSPProblemFactory.hpp"
#include "heuristics/all.hpp"
#include "heuristics.hpp"

// [[Rcpp::export]]
void initFactories(std::string data_folder)
{
  MHParamsSpecsFactory::init(data_folder + "/specs");
  FSPProblemFactory::init(data_folder);
}

auto rcharVec2map(Rcpp::CharacterVector charvec) -> std::unordered_map<std::string, std::string> {
  std::unordered_map<std::string, std::string> ret;
  for (const auto& name : Rcpp::as<std::vector<std::string>>(charvec.names())) {
    ret[name] = charvec[name];
  }
  return ret;
}


// [[Rcpp::export]]
List solveFSP(std::string mh, Rcpp::CharacterVector rproblem, long seed,
              Rcpp::CharacterVector rparams, bool verbose = false) 
try {
  using namespace Rcpp;
  
  RNG::seed(seed);
  std::unordered_map<std::string, std::string> params = rcharVec2map(rparams);
  std::unordered_map<std::string, std::string> problem = rcharVec2map(rproblem);
  
  if (verbose) {
    for (const auto& kv : params)
      Rcerr << kv.first << ": " << kv.second << '\n';
    for (const auto& kv : problem)
      Rcerr << kv.first << ": " << kv.second << '\n';
  }
  auto result = solveWith(mh, problem, params);
  return List::create(
    Named("fitness") = result.fitness,
    Named("time") = result.time,
    Named("no_evals") = result.no_evals);
} catch (std::exception &ex) {
  std::cerr << ex.what();
  throw Rcpp::exception(ex.what());
}
