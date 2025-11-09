#ifndef GREEDY_SOLVER_H
#define GREEDY_SOLVER_H

#include "stroke.h"

#include <cstdint>
#include <limits>
#include <random>
#include <string>
#include <vector>

struct GreedyConfig {
    int numStrokes = 0;
    int candidatesPerStroke = 50;
};

class RNG {
public:
    explicit RNG(uint64_t seed);
    int uniformInt(int min, int max);
    float uniformFloat(float min, float max);
private:
    std::mt19937_64 engine_;
};

struct Solution {
    std::vector<Stroke> strokes;
    double mse = std::numeric_limits<double>::infinity();
};

class PaintingProblem {
public:
    PaintingProblem() = default;
    explicit PaintingProblem(const Canvas& target);

    const Canvas& targetCanvas() const { return target_; }
    int width() const { return target_.width; }
    int height() const { return target_.height; }
    bool isValid() const { return target_.width > 0 && target_.height > 0; }

private:
    Canvas target_{0, 0};
};

PaintingProblem loadProblemFromImage(const std::string& filepath);

class Evaluator {
public:
    explicit Evaluator(const PaintingProblem& problem);

    double compute(const std::vector<Stroke>& strokes);
    double compute(Solution& solution);
    const Canvas& renderedCanvas() const { return workingCanvas_; }

private:
    const PaintingProblem& problem_;
    Canvas workingCanvas_;
};

Solution buildGreedySolution(const PaintingProblem& problem,
                             Evaluator& evaluator,
                             RNG& rng,
                             const GreedyConfig& config);

#endif


