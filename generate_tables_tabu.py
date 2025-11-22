import csv
import os
from tabulate import tabulate

CSV_PATH = os.path.join("resultados_tabu", "experimentos_tabu.csv")
OUTPUT_MD = os.path.join("resultados_tabu", "tablas.md")

# Mapeo de nombres de escenarios a títulos
ESCENARIO_TITULOS = {
    "mala_solucion_inicial": "Escenario 1: Mala Solución Inicial",
    "solucion_inicial_moderada": "Escenario 2: Solución Inicial Moderada",
    "buena_solucion_inicial": "Escenario 3: Buena Solución Inicial",
}

# Mapeo de configuraciones Tabu a títulos
CONFIG_TABU_TITULOS = {
    "moderado": "Moderado",
    "intensivo": "Intensivo",
}


def load_data() -> list[dict]:
    data = []
    with open(CSV_PATH, "r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            data.append(row)
    return data


def format_float(value: float, decimals: int = 2) -> str:
    if value is None:
        return "N/A"
    return f"{value:.{decimals}f}"


def format_percentage(value: float, decimals: int = 2) -> str:
    if value is None:
        return "N/A"
    return f"{value:.{decimals}f}%"


def generate_table_for_scenario(data: list[dict], escenario: str, config_tabu: str) -> str:
    # Filtrar datos para este escenario y configuración Tabu
    filtered = [
        row for row in data
        if row["escenario"] == escenario and row["config_tabu"] == config_tabu
    ]
    
    if not filtered:
        return f"# {ESCENARIO_TITULOS[escenario]} - Tabu {CONFIG_TABU_TITULOS[config_tabu]}\n\n*No hay datos disponibles.*\n\n"
    
    # Ordenar por instancia
    filtered.sort(key=lambda x: x["instancia"])
    
    # Construir tabla
    table_data = []
    for row in filtered:
        instancia = row["instancia"]
        mse_greedy = format_float(float(row["mse_greedy"]))
        mse_tabu = format_float(float(row["mse_tabu"]))
        mejora_pct = format_percentage(float(row["mejora_pct"]))
        tiempo_greedy = format_float(float(row["tiempo_greedy"]), 3)
        tiempo_tabu = format_float(float(row["tiempo_tabu"]), 3)
        tiempo_total = format_float(float(row["tiempo_total"]), 3)
        
        table_data.append([
            instancia,
            mse_greedy,
            mse_tabu,
            mejora_pct,
            tiempo_greedy,
            tiempo_tabu,
            tiempo_total,
        ])
    
    headers = [
        "instancia",
        "mse_greedy",
        f"mse_greedy+tabu_{config_tabu}",
        f"mejora_%_{config_tabu}",
        "tiempo_greedy (s)",
        f"tiempo_tabu_{config_tabu} (s)",
        f"tiempo_total_{config_tabu} (s)",
    ]
    
    table_markdown = tabulate(table_data, headers=headers, tablefmt="github")
    
    # Generar título
    escenario_titulo = ESCENARIO_TITULOS.get(escenario, escenario)
    config_titulo = CONFIG_TABU_TITULOS.get(config_tabu, config_tabu)
    
    return f"# {escenario_titulo} - Tabu {config_titulo}\n\n{table_markdown}\n\n"


def main() -> None:
    if not os.path.exists(CSV_PATH):
        print(f"Error: No se encontró el archivo {CSV_PATH}")
        print("Ejecuta primero run_experiments_tabu.py para generar los datos.")
        return
    
    data = load_data()
    
    if not data:
        print(f"Error: El archivo {CSV_PATH} está vacío.")
        return
    
    # Obtener escenarios y configuraciones únicas
    escenarios = sorted(set(row["escenario"] for row in data))
    configuraciones = sorted(set(row["config_tabu"] for row in data))
    
    # Generar todas las tablas
    output_lines = [
        "# Tablas de Resultados - Greedy + Tabu Search\n\n",
        "Este documento contiene las tablas de resultados para los diferentes escenarios y configuraciones de Tabu Search.\n\n",
    ]
    
    for escenario in escenarios:
        for config_tabu in configuraciones:
            table_content = generate_table_for_scenario(data, escenario, config_tabu)
            output_lines.append(table_content)
            output_lines.append("---\n\n")
    
    # Escribir archivo Markdown
    with open(OUTPUT_MD, "w", encoding="utf-8") as f:
        f.writelines(output_lines)
    
    print(f"Tablas generadas exitosamente en {OUTPUT_MD}")


if __name__ == "__main__":
    try:
        from tabulate import tabulate
    except ImportError:
        print("Instalando dependencia 'tabulate'...")
        import subprocess
        import sys
        subprocess.check_call([sys.executable, "-m", "pip", "install", "tabulate"])
        from tabulate import tabulate
    
    main()

