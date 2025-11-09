#include "greedy_solver.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

struct AppConfig {
    std::string instancePath;
    int numStrokes = 300;
    uint64_t seed = 12345;
    GreedyConfig greedy;
    std::string outputImage = "output.png";
};

// Carga los 4 brushes por defecto solo una vez.
void ensureBrushesLoaded() {
    if (!gBrushes.empty()) return;

    const std::vector<std::string> brushFiles = {
        "brushes/1.jpg",
        "brushes/2.jpg",
        "brushes/3.jpg",
        "brushes/4.jpg"
    };

    for (const auto& path : brushFiles) {
        ImageGray brush;
        if (!loadImageGray(path, brush)) {
            throw std::runtime_error("No se pudo cargar el brush: " + path);
        }
        gBrushes.push_back(std::move(brush));
    }
}

// CLI muy simple para configurar instancia, #strokes, candidatos y salida.
AppConfig parseArgs(int argc, char** argv) {
    AppConfig config;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        auto requiresValue = [&](const std::string& name) -> std::string {
            if (i + 1 >= argc) {
                throw std::runtime_error("Falta valor para " + name);
            }
            return argv[++i];
        };

        if (arg == "--instance" || arg == "-i") {
            config.instancePath = requiresValue(arg);
        } else if (arg == "--strokes" || arg == "-n") {
            config.numStrokes = std::stoi(requiresValue(arg));
        } else if (arg == "--seed") {
            config.seed = static_cast<uint64_t>(std::stoll(requiresValue(arg)));
        } else if (arg == "--greedy-candidates") {
            config.greedy.candidatesPerStroke = std::stoi(requiresValue(arg));
        } else if (arg == "--output") {
            config.outputImage = requiresValue(arg);
        } else {
            throw std::runtime_error("Argumento desconocido: " + arg);
        }
    }

    config.greedy.numStrokes = config.numStrokes;

    if (config.instancePath.empty()) {
        throw std::runtime_error("Debe especificarse --instance <ruta>");
    }
    return config;
}

int main(int argc, char** argv) {
    try {
        AppConfig config = parseArgs(argc, argv);
        ensureBrushesLoaded();

        // Cargar target y preparar utilidades de evaluación + RNG.
        PaintingProblem problem = loadProblemFromImage(config.instancePath);
        Evaluator evaluator(problem);
        RNG rng(config.seed);

        std::cout << "Instancia: " << config.instancePath << "\n";
        std::cout << "Lienzo: " << problem.width() << "x" << problem.height() << "\n";
        std::cout << "Greedy -> strokes: " << config.numStrokes
                  << ", candidatos: " << config.greedy.candidatesPerStroke << "\n";

        // Construir la solución greedy y reportar su MSE final.
        Solution greedySolution = buildGreedySolution(problem, evaluator, rng, config.greedy);
        std::cout << "MSE Greedy: " << greedySolution.mse << "\n";

        // Renderizar la solución elegida y guardarla a disco.
        Canvas output(problem.width(), problem.height());
        render(greedySolution.strokes, output);
        if (!savePNG(output, config.outputImage)) {
            throw std::runtime_error("No se pudo guardar la imagen de salida.");
        }
        std::cout << "Guardado resultado en: " << config.outputImage << "\n";

    } catch (const std::exception& ex) {
        std::cerr << "[Error] " << ex.what() << "\n";
        return 1;
    }
    return 0;
}


