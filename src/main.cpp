#include <iostream>

#include <paradiseo/eo/eo>

#include "FSPProblemFactory.hpp"
#include "heuristics.hpp"
#include "heuristics/all.hpp"
#include "MHParamsSpecsFactory.hpp"
#include "MHParamsSpecs.hpp"

auto split(const std::string& val) -> std::vector<std::string> {
  std::vector<std::string> res;
  std::stringstream ss(val);
  std::string token;
  while (std::getline(ss, token, ',')) {
    res.push_back(token);
  }
  return res;
}

auto main(int argc, char* argv[]) -> int {
  eoParser parser(argc, argv);

  std::string data_folder;
  std::string mh;
  std::vector<std::string> problem_names;
  std::vector<std::string> problem_values;
  long seed = 123;

  data_folder =
      parser.createParam(data_folder, "data_folder", "specs and instances path")
          .value();
  mh = parser.createParam(mh, "mh", "metaheuristic").value();
  seed = parser.createParam(seed, "seed", "rng seed").value();

  MHParamsSpecsFactory::init(data_folder + "/specs");
  FSPProblemFactory::init(data_folder);
  RNG::seed(seed);

  MHParamsSpecs specs = MHParamsSpecsFactory::get(mh);
  std::unordered_map<std::string, std::string> params;
  for (const auto& param : specs) {
    auto argParam = parser.createParam(std::string(), param->name, param->name);
    if (parser.isItThere(argParam))
      params[param->name] = argParam.value();
  }

  std::unordered_map<std::string, std::string> problem;
  for (const auto& pname : FSPProblemFactory::names()) {
    problem[pname] = parser.createParam(data_folder, pname, pname)
                .value();
  }

  bool printConfig =
      parser
          .createParam(false, "--print-config", "print configuration and exit")
          .value();
  if (printConfig) {
    std::cout << "{" << '\n' << "seed: " << seed << '\n' << "problem: {\n";
    for (const auto& kv : problem)
      std::cout << kv.first << ": " << kv.second << '\n';
    std::cout << "mh: " << mh << '\n';
  }

  RunOptions options(parser);

  solveWith(mh, problem, params, options);

  return 0;
}