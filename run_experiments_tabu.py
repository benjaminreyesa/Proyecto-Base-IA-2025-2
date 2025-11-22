import csv
import itertools
import os
import subprocess
import time
import re

INSTANCES_DIR = "instancias"
OUTPUT_DIR = "resultados_tabu"
BINARY = os.path.join(os.getcwd(), "painter.exe")

# 3 escenarios: (strokes, candidatos_greedy, nombre_escenario)
ESCENARIOS = [
    (200, 50, "mala_solucion_inicial"),
    (400, 100, "solucion_inicial_moderada"),
    (600, 150, "buena_solucion_inicial"),
]

# 2 configuraciones Tabu: (iteraciones, tenure, vecindario, nombre)
CONFIGURACIONES_TABU = [
    (300, 15, 25, "moderado"),    # Tabu Moderado
    (1000, 30, 50, "intensivo"),  # Tabu Intensivo
]

SEEDS = [42]  # se puede añadir más semillas para promediar
CSV_PATH = os.path.join(OUTPUT_DIR, "experimentos_tabu.csv")


def ensure_directories() -> None:
    os.makedirs(OUTPUT_DIR, exist_ok=True)


def collect_instances() -> list[str]:
    files = []
    for entry in os.listdir(INSTANCES_DIR):
        if entry.lower().endswith((".png", ".jpg", ".jpeg")):
            files.append(entry)
    files.sort()
    return files


def parse_output(output: str) -> dict:
    result = {
        "mse_greedy": None,
        "mse_tabu": None,
        "tiempo_greedy": None,
        "tiempo_tabu": None,
        "tiempo_total": None,
    }
    
    lines = output.splitlines()
    for line in lines:
        if "MSE Greedy:" in line:
            match = re.search(r"MSE Greedy:\s*([\d.]+)", line)
            if match:
                result["mse_greedy"] = float(match.group(1))
        elif "MSE Tabu final:" in line:
            match = re.search(r"MSE Tabu final:\s*([\d.]+)", line)
            if match:
                result["mse_tabu"] = float(match.group(1))
        elif "Tiempo Greedy (s):" in line:
            match = re.search(r"Tiempo Greedy \(s\):\s*([\d.]+)", line)
            if match:
                result["tiempo_greedy"] = float(match.group(1))
        elif "Tiempo Tabu (s):" in line:
            match = re.search(r"Tiempo Tabu \(s\):\s*([\d.]+)", line)
            if match:
                result["tiempo_tabu"] = float(match.group(1))
        elif "Tiempo Total (s):" in line:
            match = re.search(r"Tiempo Total \(s\):\s*([\d.]+)", line)
            if match:
                result["tiempo_total"] = float(match.group(1))
    
    return result


def run_case(instance: str, strokes: int, candidates: int, seed: int,
             tabu_iters: int, tabu_tenure: int, tabu_neigh: int,
             escenario: str, config_tabu: str) -> dict:
    instance_base = os.path.splitext(instance)[0]
    output_name = f"{instance_base}_s{strokes}_c{candidates}_tabu_{config_tabu}_seed{seed}.png"
    output_path = os.path.join(OUTPUT_DIR, output_name)

    cmd = [
        BINARY,
        "--instance", os.path.join(INSTANCES_DIR, instance),
        "--strokes", str(strokes),
        "--greedy-candidates", str(candidates),
        "--seed", str(seed),
        "--tabu-iters", str(tabu_iters),
        "--tabu-tenure", str(tabu_tenure),
        "--tabu-neigh", str(tabu_neigh),
        "--output", output_path,
    ]

    completed = subprocess.run(cmd, capture_output=True, text=True)

    if completed.returncode != 0:
        raise RuntimeError(f"Error ejecutando {cmd}:\n{completed.stderr}")

    parsed = parse_output(completed.stdout)
    
    # Validar que se obtuvieron todos los valores necesarios
    if any(v is None for v in parsed.values()):
        raise RuntimeError(f"No se pudieron extraer todos los valores de la salida:\n{completed.stdout}")
    
    # Calcular mejora porcentual
    mejora_pct = None
    if parsed["mse_greedy"] is not None and parsed["mse_tabu"] is not None:
        if parsed["mse_greedy"] > 0:
            mejora_pct = ((parsed["mse_greedy"] - parsed["mse_tabu"]) / parsed["mse_greedy"]) * 100.0
        else:
            mejora_pct = 0.0
    
    result = {
        "instancia": instance,
        "escenario": escenario,
        "config_tabu": config_tabu,
        "strokes": strokes,
        "candidatos": candidates,
        "seed": seed,
        "tabu_iters": tabu_iters,
        "tabu_tenure": tabu_tenure,
        "tabu_neigh": tabu_neigh,
        "mse_greedy": parsed["mse_greedy"],
        "mse_tabu": parsed["mse_tabu"],
        "mejora_pct": mejora_pct,
        "tiempo_greedy": parsed["tiempo_greedy"],
        "tiempo_tabu": parsed["tiempo_tabu"],
        "tiempo_total": parsed["tiempo_total"],
        "output_file": output_name,
    }
    
    return result


def main() -> None:
    ensure_directories()
    instances = collect_instances()
    if not instances:
        raise RuntimeError("No se encontraron instancias en la carpeta 'instancias'.")

    with open(CSV_PATH, "w", newline="", encoding="utf-8") as csvfile:
        fieldnames = [
            "instancia", "escenario", "config_tabu", "strokes", "candidatos", "seed",
            "tabu_iters", "tabu_tenure", "tabu_neigh",
            "mse_greedy", "mse_tabu", "mejora_pct",
            "tiempo_greedy", "tiempo_tabu", "tiempo_total", "output_file"
        ]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()

        total_cases = len(instances) * len(ESCENARIOS) * len(CONFIGURACIONES_TABU) * len(SEEDS)
        current_case = 0

        for instance in instances:
            for strokes, candidates, escenario_nombre in ESCENARIOS:
                for tabu_iters, tabu_tenure, tabu_neigh, config_tabu_nombre in CONFIGURACIONES_TABU:
                    for seed in SEEDS:
                        current_case += 1
                        print(f"[{current_case}/{total_cases}] Ejecutando {instance} | "
                              f"Escenario: {escenario_nombre} | Tabu: {config_tabu_nombre} | "
                              f"seed={seed} ...")
                        
                        result = run_case(
                            instance, strokes, candidates, seed,
                            tabu_iters, tabu_tenure, tabu_neigh,
                            escenario_nombre, config_tabu_nombre
                        )
                        writer.writerow(result)
                        csvfile.flush()

    print(f"\nExperimentos finalizados. Resultados en {CSV_PATH}")


if __name__ == "__main__":
    main()

