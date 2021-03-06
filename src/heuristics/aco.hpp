#pragma once

#include <paradiseo/eo/eo>
#include <paradiseo/mo/mo>

#include <algorithm>
#include <unordered_map>

#include "MHParamsValues.hpp"
#include "heuristics/IGexplorer.hpp"
#include "heuristics/falseContinuator.hpp"
#include "heuristics.hpp"
#include "FSPProblemFactory.hpp"
#include "MHParamsSpecsFactory.hpp"

template <class Ngh>
class MinMaxAntSystem : public moPerturbation<Ngh> {
 public:
  using EOT = typename Ngh::EOT;
  using dvec = std::vector<double>;
  using dmat = std::vector<dvec>;

  MinMaxAntSystem(const int N,
                  const double t_min_f,
                  const double rho,
                  const double p0)
      : moPerturbation<Ngh>{}, N(N), t_min_f(t_min_f), rho(rho), p0(p0) {}

  /**
   * Init the memory
   * @param _sol the current solution
   */
  void init(EOT& _sol) final {
    // Set parameters, initialize pheromone trails
    t_max = 1.0 / ((1 - rho) * _sol.fitness());
    t_min = t_min_f * t_max;
    pheromones.assign(N, dvec(N, t_max));
    Ngh dummy;
    // add(_sol, dummy);
  }

  auto operator()(EOT& sol) -> bool final {
    // ConstructSolutions
    sol.invalidate();
    sol.resize(0);
    sol.reserve(N);
    dmat probs = pheromones;
    int chosen_job = -1;
    for (int j = 0; j < N; j++) {
      if (rng.uniform() < p0) {
        // find the job that has the higher desire to be in j th position
        chosen_job = maxCoeffCol(probs, j);
      } else {
        double sum = sumCol(probs, j);
        if (sum <= 1e-6) {
          // choose randomly if there are not enough values (or all ties at
          // zero)
          do {
            chosen_job = std::floor(rng.uniform(N));
          } while (std::find(sol.begin(), sol.end(), chosen_job) != sol.end());
        } else {
          // use the probabilities as a distribution
          double r = rng.uniform(sum);
          double cum_sum = 0.0;
          chosen_job = -1;
          do {
            chosen_job++;
            cum_sum += probs[j][chosen_job];
          } while (cum_sum < r);
        }
      }
      sol.push_back(chosen_job);

      for (int row = 0; row < N; row++) {
        probs[row][chosen_job] = 0.0;
      }
    }
    return true;
  }

  /**
   * Add data to the memory
   * @param _sol the current solution
   * @param _neighbor the current neighbor
   */
  void add(EOT&, Ngh&) final{};

  /**
   * update the memory
   * @param _sol the current solution
   * @param _neighbor the current neighbor
   */
  void update(EOT& _sol, Ngh&) final {
    // UpdateTrails
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        double val = rho * pheromones[i][j];
        if (_sol[i] == j)
          val += 1.0 / _sol.fitness();
        pheromones[i][j] = clamp(val, t_min, t_max);
      }
    }
  }

  /**
   * clear the memory
   */
  void clearMemory() final{};

 private:
  // parameters
  const int N;
  const double t_min_f, rho, p0;

  // aux
  double t_min, t_max;
  dmat pheromones, prob;

  auto maxCoeffCol(const dmat& matrix, int row) -> int {
    double max = matrix[row][0];
    int ret = 0;
    for (unsigned col = 1; col < matrix.size(); col++) {
      if (max < matrix[row][col]) {
        max = matrix[row][col];
        ret = static_cast<int>(col);
      }
    }
    return ret;
  }

  auto sumCol(const dmat& matrix, int row) -> double {
    double s = 0.0;
    for (unsigned col = 1; col < matrix.size(); col++) {
      s += matrix[row][col];
    }
    return s;
  }
};

template <class Ngh, class EOT = typename Problem<Ngh>::EOT>
auto solveWithACO(Problem<Ngh>& prob, const MHParamsValues& params) -> Result {
  const int N = prob.size(0);
  const int max_nh_size = pow(N - 1, 2);

  // continuator
  eoEvalFunc<EOT>& fullEval = prob.eval();
  moEval<Ngh>& evalN = prob.neighborEval();

  moCheckpoint<Ngh>& checkpoint = prob.checkpoint();
  moCheckpoint<Ngh>& checkpointGlobal = prob.checkpointGlobal();

  // comparator strategy
  moSolComparator<EOT> compSS0;               // comp sol/sol strict
  moSolNeighborComparator<Ngh> compSN0;       // comp sol/Ngh strict
  moNeighborComparator<Ngh> compNN0;          // comp Ngh/Ngh strict
  moEqualSolComparator<EOT> compSS1;          // comp sol/sol with equal
  moEqualSolNeighborComparator<Ngh> compSN1;  // comp sol/Ngh with equal
  moEqualNeighborComparator<Ngh> compNN1;     // comp Ngh/Ngh with equal
  moSolComparator<EOT>* compSS = nullptr;
  moSolNeighborComparator<Ngh>* compSN = nullptr;
  moNeighborComparator<Ngh>* compNN = nullptr;
  switch (params.categorical("ACO.Comp.Strat")) {
    case 0:
      compSS = &compSS0;
      compSN = &compSN0;
      compNN = &compNN0;
      break;
    case 1:
      compSS = &compSS1;
      compSN = &compSN1;
      compNN = &compNN1;
      break;
    default:
      assert(false);
      break;
  }

  // initialization
  eoInit<EOT>* init = nullptr;

  // neighborhood size
  const int min_nh_size = (N >= 20) ? 11 : 2;
  const int nh_interval = (N >= 20) ? 10 : 1;
  const int no_nh_sizes = (max_nh_size - min_nh_size) / nh_interval + 1;
  const int scale = int(static_cast<float>(no_nh_sizes) *
                        params.real("ACO.Neighborhood.Size") / 10.0);
  const int nh_size = std::min(max_nh_size, min_nh_size + scale * nh_interval);

  moOrderNeighborhood<Ngh> neighborhood0(nh_size);
  moRndWithoutReplNeighborhood<Ngh> neighborhood1(nh_size);
  moNeighborhood<Ngh>* neighborhood = nullptr;
  switch (params.categorical("ACO.Neighborhood.Strat")) {
    case 0:
      neighborhood = &neighborhood0;
      break;
    case 1:
      neighborhood = &neighborhood1;
      break;
    default:
      assert(false);
      break;
  }

  // algos xxHC
  moFirstImprHC<Ngh> algo0(*neighborhood, fullEval, evalN, checkpoint, *compNN,
                           *compSN);  // FIHC
  moSimpleHC<Ngh> algo1(*neighborhood, fullEval, evalN, checkpoint, *compNN,
                        *compSN);  // BestHC
  moRandomBestHC<Ngh> algo2(*neighborhood, fullEval, evalN, checkpoint, *compNN,
                            *compSN);  // rndBestHC

  // IG (Ruiz+Stuetzle)
  // iterative greedy improvement without replacement (IG)
  // FastIGexplorer igexplorer(evalN, *compNN, *compSN);
  IGexplorer<Ngh> igexplorer(fullEval, N, *compSS);
  moLocalSearch<Ngh> algo3(igexplorer, checkpoint, fullEval);
  moLocalSearch<Ngh>* algo;
  switch (params.categorical("ACO.Local.Search")) {
    case 0:
      algo = &algo0;
      break;
    case 1:
      algo = &algo1;
      break;
    case 2:
      algo = &algo2;
      break;
    case 3:
     algo = &algo3;
     break;
    default:
      assert(false);
      break;
  }

  moCombinedContinuator<Ngh> singleStepContinuator(checkpoint);
  falseContinuator<Ngh> falseCont;
  singleStepContinuator.add(falseCont);
  if (params.categorical("ACO.LS.Single.Step")) {
    algo->setContinuator(singleStepContinuator);
  }

  moAlwaysAcceptCrit<Ngh> accept;

  /****
  *** Perturb
  ****/

  MinMaxAntSystem<Ngh> perturb(N, params.real("ACO.T.Min.Factor"),
                               params.real("ACO.Rho"), params.real("ACO.P0"));

  /****
  *** ILS
  ****/
  moILS<Ngh, Ngh> ils(*algo, fullEval, checkpointGlobal, perturb, accept);

  return runExperiment(*init, ils, prob);
}
