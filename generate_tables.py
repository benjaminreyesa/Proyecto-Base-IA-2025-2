import os
from pathlib import Path
import pandas as pd

CSV_PATH = Path("resultados") / "experimentos.csv"
OUTPUT_PATH = Path("resultados") / "tables.md"


def df_to_markdown(df: pd.DataFrame) -> str:
    if isinstance(df.index, pd.MultiIndex):
        table = df.reset_index()
    else:
        table = df.reset_index()

    columns = list(table.columns)
    header = "| " + " | ".join(str(col) for col in columns) + " |"
    separator = "|" + "|".join([" --- " for _ in columns]) + "|"
    rows = []
    for _, row in table.iterrows():
        formatted = [f"{row[col]:.4f}" if isinstance(row[col], float) else str(row[col]) for col in columns]
        rows.append("| " + " | ".join(formatted) + " |")
    return "\n".join([header, separator, *rows])


def main() -> None:
    if not CSV_PATH.exists():
        raise FileNotFoundError(f"No se encuentra {CSV_PATH}")

    df = pd.read_csv(CSV_PATH)

    pivot_mse = df.pivot_table(index=["instancia", "strokes"], columns="candidatos", values="mse")
    pivot_time = df.pivot_table(index=["instancia", "strokes"], columns="candidatos", values="tiempo_seg")

    OUTPUT_PATH.parent.mkdir(exist_ok=True)

    with OUTPUT_PATH.open("w", encoding="utf-8") as f:
        f.write("# Resultados de Experimentos (MSE y Tiempo)\n\n")
        for instancia in df["instancia"].unique():
            f.write(f"## {instancia}\n\n")

            f.write("### MSE\n\n")
            f.write(df_to_markdown(pivot_mse.loc[instancia]))
            f.write("\n\n")

            f.write("### Tiempo (s)\n\n")
            f.write(df_to_markdown(pivot_time.loc[instancia]))
            f.write("\n\n")

        summary = df.groupby(["strokes", "candidatos"]).agg(
            mse_promedio=("mse", "mean"),
            mse_std=("mse", "std"),
            tiempo_promedio=("tiempo_seg", "mean"),
            tiempo_std=("tiempo_seg", "std"),
        ).reset_index()
        f.write("## Resumen global\n\n")
        f.write(df_to_markdown(summary))
        f.write("\n")

    print(f"Tablas Markdown generadas en {OUTPUT_PATH}")


if __name__ == "__main__":
    main()
