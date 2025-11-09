#include "greedy_solver.h"

#include <cmath>
#include <limits>
#include <stdexcept>

namespace {

// Calcula el MSE completo entre dos lienzos renderizados
// (lo usamos al evaluar candidatos y la solución final).
double computeMSE(const Canvas& rendered, const Canvas& target) {
    if (rendered.width != target.width || rendered.height != target.height) {
        throw std::runtime_error("computeMSE: dimensiones incompatibles entre lienzos.");
    }
    const std::size_t total_pixels = static_cast<std::size_t>(rendered.width) *
                                     static_cast<std::size_t>(rendered.height);
    if (total_pixels == 0) return 0.0;

    const std::size_t total_components = total_pixels * 3;
    double acc = 0.0;
    for (std::size_t i = 0; i < total_components; ++i) {
        const int diff = static_cast<int>(rendered.rgb[i]) -
                         static_cast<int>(target.rgb[i]);
        acc += static_cast<double>(diff * diff);
    }
    return acc / static_cast<double>(total_components);
}

// Genera un stroke aleatorio dentro de los rangos válidos.
// Este es el candidato base que evaluamos en cada iteración.
Stroke randomStroke(RNG& rng) {
    if (gBrushes.empty()) {
        throw std::runtime_error("randomStroke: no hay brushes cargados.");
    }
    Stroke s;
    s.x_rel = rng.uniformFloat(0.0f, 1.0f);
    s.y_rel = rng.uniformFloat(0.0f, 1.0f);
    s.size_rel = rng.uniformFloat(0.05f, 0.8f);
    s.rotation_deg = rng.uniformFloat(0.0f, 360.0f);
    s.type = rng.uniformInt(0, static_cast<int>(gBrushes.size()) - 1);
    s.r = static_cast<uint8_t>(rng.uniformInt(0, 255));
    s.g = static_cast<uint8_t>(rng.uniformInt(0, 255));
    s.b = static_cast<uint8_t>(rng.uniformInt(0, 255));
    return s;
}

} // namespace

RNG::RNG(uint64_t seed) : engine_(seed) {}

int RNG::uniformInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(engine_);
}

float RNG::uniformFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(engine_);
}

// Guarda el canvas objetivo para poder evaluarlo rápido más adelante.
PaintingProblem::PaintingProblem(const Canvas& target)
    : target_(target.width, target.height) {
    target_.rgb = target.rgb;
}

PaintingProblem loadProblemFromImage(const std::string& filepath) {
    Canvas target(1, 1);
    if (!loadImageRGB_asCanvas(filepath, target)) {
        throw std::runtime_error("No se pudo cargar la instancia: " + filepath);
    }
    return PaintingProblem(target);
}

// Mantiene un canvas de trabajo reutilizable para no alocar cada vez.
Evaluator::Evaluator(const PaintingProblem& problem)
    : problem_(problem),
      workingCanvas_(problem.width(), problem.height()) {
    if (!problem_.isValid()) {
        throw std::runtime_error("Evaluator: problema no válido (canvas objetivo vacío).");
    }
}

// Renderiza el vector de strokes sobre workingCanvas_ y devuelve el MSE.
double Evaluator::compute(const std::vector<Stroke>& strokes) {
    render(strokes, workingCanvas_);
    return computeMSE(workingCanvas_, problem_.targetCanvas());
}

// Guarda el MSE dentro de Solution para comodidad (logging, etc.).
double Evaluator::compute(Solution& solution) {
    solution.mse = compute(solution.strokes);
    return solution.mse;
}

// Greedy clásico: agrega strokes uno a uno, eligiendo el mejor candidato aleatorio.
Solution buildGreedySolution(const PaintingProblem& problem,
                             Evaluator& evaluator,
                             RNG& rng,
                             const GreedyConfig& config) {
    if (config.numStrokes <= 0) {
        throw std::runtime_error("Greedy: numStrokes debe ser positivo.");
    }
    if (config.candidatesPerStroke <= 0) {
        throw std::runtime_error("Greedy: candidatesPerStroke debe ser positivo.");
    }
    if (gBrushes.empty()) {
        throw std::runtime_error("Greedy: no hay brushes cargados.");
    }

    Solution solution;
    solution.strokes.reserve(config.numStrokes);

    for (int i = 0; i < config.numStrokes; ++i) {
        double bestMSE = std::numeric_limits<double>::infinity();
        Stroke bestStroke;
        bool found = false;

        Solution candidate = solution;      // copia parcial (strokes ya elegidos)
        candidate.strokes.emplace_back();   // espacio para el candidato en curso

        for (int c = 0; c < config.candidatesPerStroke; ++c) {
            candidate.strokes.back() = randomStroke(rng);      // candidato aleatorio
            const double mse = evaluator.compute(candidate.strokes); // evaluar MSE completo
            if (mse < bestMSE) {
                bestMSE = mse;
                bestStroke = candidate.strokes.back();
                found = true;
            }
        }

        if (!found) {
            throw std::runtime_error("Greedy: no se generó un stroke candidato válido.");
        }

        solution.strokes.push_back(bestStroke);               // fijar elegido
        solution.mse = evaluator.compute(solution.strokes);   // actualizar MSE acumulado
    }

    return solution;
}

