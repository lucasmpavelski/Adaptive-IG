#pragma once

#include <iostream>
#include <string>

#include <paradiseo/eo/es.h>
#include <paradiseo/eo/eo>

#include "MHParamsSpecs.hpp"

class MHParamsValues : public eoReal<eoMaximizingFitness> {
 public:
  using value_type = eoReal<eoMaximizingFitness>::value_type;
  using eoReal<eoMaximizingFitness>::operator[];
  using eoReal<eoMaximizingFitness>::begin;
  using eoReal<eoMaximizingFitness>::end;

  const MHParamsSpecs* specs{nullptr};

  MHParamsValues() : eoReal<eoMaximizingFitness>(unsigned(0)) {}
  MHParamsValues(const MHParamsSpecs* specs)
      : eoReal<eoMaximizingFitness>(unsigned(specs->noParams())),
        specs(specs) {}

  auto operator[](const std::string& s) -> double& {
    return (*this)[specs->getIdx(s)];
  }
  auto operator[](const std::string& s) const -> double {
    return (*this)[specs->getIdx(s)];
  }

  [[nodiscard]] auto categorical(const std::string& s) const -> int {
    if (!specs->isCategoric(s)) {
      throw std::runtime_error("Parameter " + s + " is not categoric");
    }
    return static_cast<int>((*this)[s]);
  }

  [[nodiscard]] auto categoricalName(const std::string& s) const
      -> std::string {
    int index = specs->getIdx(s);
    auto paramSpec = specs->getParam(index);
    return paramSpec->toStrValue(static_cast<float>(categorical(s)));
  }

  [[nodiscard]] auto integer(const std::string& s) const -> int {
    if (!specs->isInteger(s))
      throw std::runtime_error("Parameter " + s + " is not integer");
    return static_cast<int>((*this)[s]);
  }

  [[nodiscard]] auto real(const std::string& s) const -> double {
    if (!specs->isReal(s))
      throw std::runtime_error("Parameter " + s + " is not real");
    return (*this)[s];
  }

  [[nodiscard]] auto mhName() const -> std::string { return specs->mhName(); }

  template <class RNG>
  void randomizeValues(RNG& rng) {
    for (unsigned i = 0; i < size(); i++) {
      float lb = (*specs)[i]->lowerBound();
      float ub = (*specs)[i]->upperBound();
      std::uniform_real_distribution<float> dist(lb, ub);
      this->at(i) = dist(rng);
    }
  }

  auto printValues(std::ostream& out) const -> std::ostream& {
    for (unsigned i = 0; i < size(); i++) {
      auto spec = (*specs)[i];
      spec->strValue(out, operator[](i)) << '\t';
    }
    return out;
  }

  void readValues(std::unordered_map<std::string, std::string> values) {
    for (const auto& ps : *specs) {
      if (values.count(ps->name))
        this->operator[](ps->name) = ps->fromStrValue(values[ps->name]);
      else
        this->operator[](ps->name) = -1.0;
    }
  }

  void readValues(std::unordered_map<std::string, double> values) {
    for (const auto& ps : *specs) {
      if (values.find(ps->name) == values.end())
        throw std::runtime_error("Parameter " + ps->name + " needs a value!");
      this->operator[](ps->name) = values.at(ps->name);
    }
  }

  [[nodiscard]] auto toMap() const -> std::unordered_map<std::string, std::string> {
    std::unordered_map<std::string, std::string> values;
    // TODO: use categorical value string name
    for (const auto& ps : *specs)
      values[ps->name] = std::to_string(this->operator[](ps->name));
    return values;
  }
};
