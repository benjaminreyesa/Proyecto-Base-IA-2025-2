#include "tabu_search.h"
#include <algorithm>
#include <cmath>
#include <deque>
#include <limits>
#include <stdexcept>
#include <unordered_set>

namespace {

enum class AttributeType {
    POSITION,
    SIZE,
    ROTATION,
    COLOR,
    TYPE
};

struct TabuEntry {
    int strokeIndex;
    AttributeType attribute;
    int expiration;
};

inline float clampFloat(float v, float lo, float hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

inline int clampInt(int v, int lo, int hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

// Selecciona qué atributo modificar según las probabilidades.
AttributeType selectAttribute(RNG& rng) {
    const float p = rng.uniformFloat(0.0f, 1.0f);
    if (p < 0.40f) return AttributeType::POSITION;      // 40%
    if (p < 0.65f) return AttributeType::SIZE;          // 25%
    if (p < 0.85f) return AttributeType::ROTATION;      // 20%
    if (p < 0.95f) return AttributeType::COLOR;         // 10%
    return AttributeType::TYPE;                         // 5%
}

// Modifica un stroke según el atributo seleccionado y step_scale.
Stroke perturbStroke(const Stroke& base, AttributeType attr, RNG& rng, float stepScale) {
    Stroke s = base;

    switch (attr) {
        case AttributeType::POSITION: {
            const float delta = 0.10f * stepScale;
            s.x_rel = clampFloat(base.x_rel + rng.uniformFloat(-delta, delta), 0.0f, 1.0f);
            s.y_rel = clampFloat(base.y_rel + rng.uniformFloat(-delta, delta), 0.0f, 1.0f);
            break;
        }
        case AttributeType::SIZE: {
            const float delta = 0.30f * stepScale;
            const float factor = 1.0f + rng.uniformFloat(-delta, delta);
            s.size_rel = clampFloat(base.size_rel * factor, 0.01f, 1.5f);
            break;
        }
        case AttributeType::ROTATION: {
            const float delta = 30.0f * stepScale;
            float rot = base.rotation_deg + rng.uniformFloat(-delta, delta);
            while (rot < 0.0f) rot += 360.0f;
            while (rot >= 360.0f) rot -= 360.0f;
            s.rotation_deg = rot;
            break;
        }
        case AttributeType::COLOR: {
            const int delta = static_cast<int>(255.0f * 0.30f * stepScale);
            s.r = static_cast<uint8_t>(clampInt(base.r + rng.uniformInt(-delta, delta), 0, 255));
            s.g = static_cast<uint8_t>(clampInt(base.g + rng.uniformInt(-delta, delta), 0, 255));
            s.b = static_cast<uint8_t>(clampInt(base.b + rng.uniformInt(-delta, delta), 0, 255));
            break;
        }
        case AttributeType::TYPE: {
            if (gBrushes.empty()) break;
            const int maxType = static_cast<int>(gBrushes.size()) - 1;
            int newType = base.type + rng.uniformInt(-1, 1);
            s.type = clampInt(newType, 0, maxType);
            break;
        }
    }
    return s;
}

bool isTabu(const std::deque<TabuEntry>& tabuList, int strokeIndex, AttributeType attr, int currentIteration) {
    for (const auto& entry : tabuList) {
        if (entry.strokeIndex == strokeIndex && entry.attribute == attr && entry.expiration > currentIteration) {
            return true;
        }
    }
    return false;
}

void addTabu(std::deque<TabuEntry>& tabuList, int strokeIndex, AttributeType attr, int currentIteration, int tenure) {
    tabuList.push_back({strokeIndex, attr, currentIteration + tenure});
    while (!tabuList.empty() && tabuList.front().expiration <= currentIteration) {
        tabuList.pop_front();
    }
}

} // namespace

Solution runTabuSearch(const PaintingProblem& problem,
                      Evaluator& evaluator,
                      RNG& rng,
                      const Solution& initial,
                      const TabuConfig& config) {
    if (!problem.isValid()) {
        throw std::runtime_error("Tabu Search: problema inválido.");
    }
    if (initial.strokes.empty()) {
        throw std::runtime_error("Tabu Search: solución inicial vacía.");
    }

    Solution current = initial;
    Solution best = current;
    std::deque<TabuEntry> tabuList;

    float stepScale = 1.0f;
    const float stepScaleMin = 0.1f;
    const float stepScaleMax = 2.0f;
    int iterationsWithoutImprovement = 0;
    int iterationsSinceLastImprovement = 0;

    for (int iteration = 0; iteration < config.maxIterations; ++iteration) {
        double bestCandidateMSE = std::numeric_limits<double>::infinity();
        int bestCandidateIndex = -1;
        Stroke bestCandidateStroke;
        AttributeType bestCandidateAttr = AttributeType::POSITION;
        bool foundCandidate = false;

        // Generar vecindario de tamaño neighborhoodSize.
        for (int n = 0; n < config.neighborhoodSize; ++n) {
            const int strokeIdx = rng.uniformInt(0, static_cast<int>(current.strokes.size()) - 1);
            const AttributeType attr = selectAttribute(rng);

            // Verificar si está tabu (con regla de aspiración).
            const bool isTabuMove = isTabu(tabuList, strokeIdx, attr, iteration);
            if (isTabuMove && current.mse >= best.mse) {
                continue; // Rechazar tabu si no mejora el mejor global.
            }

            Stroke perturbed = perturbStroke(current.strokes[strokeIdx], attr, rng, stepScale);
            Solution candidate = current;
            candidate.strokes[strokeIdx] = perturbed;
            const double mse = evaluator.compute(candidate.strokes);

            if (mse < bestCandidateMSE) {
                bestCandidateMSE = mse;
                bestCandidateIndex = strokeIdx;
                bestCandidateStroke = perturbed;
                bestCandidateAttr = attr;
                foundCandidate = true;
            }
        }

        if (!foundCandidate || bestCandidateIndex == -1) {
            break;
        }

        // Aplicar el mejor movimiento.
        current.strokes[bestCandidateIndex] = bestCandidateStroke;
        current.mse = evaluator.compute(current.strokes);
        addTabu(tabuList, bestCandidateIndex, bestCandidateAttr, iteration, config.tabuTenure);

        // Actualizar mejor solución global.
        if (current.mse < best.mse) {
            best = current;
            iterationsWithoutImprovement = 0;
            iterationsSinceLastImprovement = 0;

            // Aumentar step_scale al mejorar.
            stepScale = std::min(stepScale * 1.1f, stepScaleMax);
        } else {
            ++iterationsWithoutImprovement;
            ++iterationsSinceLastImprovement;

            // Reducir step_scale si no mejora durante X iteraciones.
            if (iterationsSinceLastImprovement >= config.iterationsToReduceStep) {
                stepScale = std::max(stepScale * 0.8f, stepScaleMin);
                iterationsSinceLastImprovement = 0;
            }
        }

        // Criterio de parada: sin mejora durante maxNoImprove iteraciones.
        if (iterationsWithoutImprovement >= config.maxNoImprove) {
            break;
        }
    }

    return best;
}

