# Documentación del Solver Greedy

## Estructura general

- `app.cpp`: punto de entrada. Carga brushes, instancia y ejecuta la construcción Greedy antes de guardar el resultado.
- `greedy_solver.{h,cpp}`: agrupa las utilidades (`RNG`, `Solution`, `PaintingProblem`, `Evaluator`) y la construcción greedy basada en candidatos aleatorios.

El código original (`stroke.*`, `testCall.cpp`) se mantiene sin cambios y se reutiliza mediante las nuevas capas.

## Compilación sugerida

```bash
g++ -std=c++17 -O2 app.cpp greedy_solver.cpp stroke.cpp -o painter
```

(Ajustar según compilador/sistema). Asegurarse de compilar también `stb_image*.h` al incluir `stroke.cpp`.

## Uso del ejecutable

```bash
./painter --instance instancias/mona.png \
          --strokes 300 \
          --greedy-candidates 60 \
          --seed 42 \
          --output resultados/mona_300.png
```

### Parámetros

- `--instance`, `-i`: ruta a la imagen objetivo (obligatorio).
- `--strokes`, `-n`: número total de pinceladas a agregar (default 300).
- `--seed`: semilla RNG (default 12345).
- `--greedy-candidates`: cantidad de candidatos evaluados por pincelada en la etapa greedy (default 50).
- `--output`: ruta del PNG con el resultado (default `output.png`).

Todos los parámetros pueden ajustarse para experimentos y análisis comparativo.

## Flujo interno

1. **Carga de brushes** (`ensureBrushesLoaded`) lee automáticamente `brushes/1.jpg` a `4.jpg`.
2. **Carga de instancia** (`loadProblemFromImage`) obtiene el lienzo objetivo.
3. **Greedy** (`buildGreedySolution`) agrega trazos uno a uno: para cada posición encadena `candidatesPerStroke` strokes aleatorios, evalúa el MSE completo y conserva el que más lo reduce.
4. **Salida**: se renderiza la solución greedy, se guarda el PNG y se reporta el MSE obtenido.

## Sugerencias para experimentación

- Crear un directorio `resultados/` para almacenar distintas ejecuciones.
- Variar `--strokes`, `--greedy-candidates` y la semilla RNG para comparar el impacto en la calidad del MSE.
- Registrar MSE junto con el tiempo de ejecución para cada instancia.

## Próximos pasos posibles

- Ajustar la generación de candidatos (por ejemplo, acotar rangos de tamaños o colores en base al target).
- Incorporar metaheurísticas adicionales para refinar la solución a partir del resultado greedy.
- Añadir registro a archivo (CSV/JSON) con el historial de MSE por iteración para análisis posterior.


