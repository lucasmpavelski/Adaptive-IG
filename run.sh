#!/bin/sh

Rscript reports/aos/00_generate_train_test_problems.R
Rscript reports/aos/01_adaptive_destruction_size.R
Rscript reports/aos/02_adaptive_best_insertion.R
Rscript reports/aos/03_adaptive_destruction_position.R
Rscript reports/aos/04_adaptive_local_search.R
Rscript reports/aos/05_adaptive_perturb.R
Rscript reports/aos/06_adaptive_neighborhood_size.R
Rscript reports/aos/09_final_comparison.R