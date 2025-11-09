import csv
import itertools
import os
import subprocess
import time

INSTANCES_DIR = "instancias"
OUTPUT_DIR = "resultados"
BINARY = os.path.join(os.getcwd(), "painter.exe")  # asumimos que ya está compilado

STROKE_VALUES = [200, 400, 600]
CANDIDATE_VALUES = [50, 100, 150]
# se puede añadir más semillas para promediar
SEEDS = [42]

CSV_PATH = os.path.join(OUTPUT_DIR, "experimentos.csv")


def ensure_directories() -> None:
    os.makedirs(OUTPUT_DIR, exist_ok=True)


def collect_instances() -> list[str]:
    files = []
    for entry in os.listdir(INSTANCES_DIR):
        if entry.lower().endswith((".png", ".jpg", ".jpeg")):
            files.append(entry)
    files.sort()
    return files


def run_case(instance: str, strokes: int, candidates: int, seed: int) -> tuple[float, float]:
    output_name = f"{os.path.splitext(instance)[0]}_s{strokes}_c{candidates}_seed{seed}.png"
    output_path = os.path.join(OUTPUT_DIR, output_name)

    cmd = [
        BINARY,
        "--instance", os.path.join(INSTANCES_DIR, instance),
        "--strokes", str(strokes),
        "--greedy-candidates", str(candidates),
        "--seed", str(seed),
        "--output", output_path,
    ]

    start = time.perf_counter()
    completed = subprocess.run(cmd, capture_output=True, text=True)
    elapsed = time.perf_counter() - start

    if completed.returncode != 0:
        raise RuntimeError(f"Error ejecutando {cmd}:\n{completed.stderr}")

    mse_value = None
    for line in completed.stdout.splitlines():
        if "MSE Greedy" in line:
            parts = line.strip().split(":")
            if len(parts) == 2:
                try:
                    mse_value = float(parts[1])
                except ValueError:
                    mse_value = None
            break

    if mse_value is None:
        raise RuntimeError(f"No se pudo extraer el MSE de la salida:\n{completed.stdout}")

    return mse_value, elapsed


def main() -> None:
    ensure_directories()
    instances = collect_instances()
    if not instances:
        raise RuntimeError("No se encontraron instancias en la carpeta 'instancias'.")

    with open(CSV_PATH, "w", newline="", encoding="utf-8") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["instancia", "strokes", "candidatos", "seed", "tiempo_seg", "mse"])

        for instance, strokes, candidates, seed in itertools.product(instances, STROKE_VALUES, CANDIDATE_VALUES, SEEDS):
            print(f"Ejecutando {instance} | strokes={strokes} | cand={candidates} | seed={seed} ...")
            mse_value, elapsed = run_case(instance, strokes, candidates, seed)
            writer.writerow([instance, strokes, candidates, seed, elapsed, mse_value])
            csvfile.flush()

    print(f"Experimentos finalizados. Resultados en {CSV_PATH}")


if __name__ == "__main__":
    main()
