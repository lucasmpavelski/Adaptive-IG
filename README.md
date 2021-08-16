## Adaptive Iterated Greedy

This repository contains the implementation and experiments data for the paper "Dynamic Learning in Hyper-Heuristics to Solve Flowshop Problems", to appear in BRACIS 2021 proceedings (http://c4ai.inova.usp.br/bracis/).

The source code is available as an Rcpp package:
- The algorithms are implemented in C++ using the Paradiseo library (https://github.com/nojhan/paradiseo)
- The experiments and plots are done using R packages like tidiverse and ggplot.

All instances are available at data/instances/flowshop folder.

The experiments are reproduced by running:

	Rscript reports/aos/00_generate_train_test_problems.R
	Rscript reports/aos/01_adaptive_destruction_size.R
	Rscript reports/aos/02_adaptive_best_insertion.R
	Rscript reports/aos/03_adaptive_destruction_position.R
	Rscript reports/aos/04_adaptive_local_search.R
	Rscript reports/aos/05_adaptive_perturb.R
	Rscript reports/aos/06_adaptive_neighborhood_size.R
	Rscript reports/aos/09_final_comparison.R

The plots and analysis are available in the file reports/aos/analysis.rmd.

## Dependencies

- Paradiseo >= 2.0
- Eigen >= 3 (used in LinUCB strategy)
- cmake >= 3.15
- clang or gcc
- R >= 4.0
- R dependencies (included in DESCRIPTION)

or 

- docker (using the Dockerfile)

## TODO

- Refactor C++ folders and unused components
- Make analysis and supplementary materials available online