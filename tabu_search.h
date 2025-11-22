#ifndef TABU_SEARCH_H
#define TABU_SEARCH_H

#include "greedy_solver.h"

struct TabuConfig {
    int maxIterations = 1000;
    int tabuTenure = 20;
    int neighborhoodSize = 30;
    int maxNoImprove = 200;
    int iterationsToReduceStep = 25;
};

Solution runTabuSearch(const PaintingProblem& problem,
                      Evaluator& evaluator,
                      RNG& rng,
                      const Solution& initial,
                      const TabuConfig& config);

#endif

