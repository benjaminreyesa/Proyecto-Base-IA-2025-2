# Tablas de Resultados - Greedy + Tabu Search

Este documento contiene las tablas de resultados para los diferentes escenarios y configuraciones de Tabu Search.

# Escenario 3: Buena Solución Inicial - Tabu Intensivo

| instancia       |   mse_greedy |   mse_greedy+tabu_intensivo | mejora_%_intensivo   |   tiempo_greedy (s) |   tiempo_tabu_intensivo (s) |   tiempo_total_intensivo (s) |
|-----------------|--------------|-----------------------------|----------------------|---------------------|-----------------------------|------------------------------|
| bach.png        |       779.99 |                      193.36 | 75.21%               |             275.568 |                     219.771 |                      495.338 |
| dali.png        |       837.73 |                      358.45 | 57.21%               |             219.866 |                     159.933 |                      379.8   |
| klimt.png       |       874.42 |                      476.79 | 45.47%               |             257.22  |                     198.808 |                      456.029 |
| mona.png        |       486.62 |                      202.48 | 58.39%               |             275.364 |                     210.131 |                      485.495 |
| mondriaan.png   |       604.46 |                      164.27 | 72.82%               |             256.853 |                     217.281 |                      474.134 |
| pollock.png     |      2424.94 |                     1563.05 | 35.54%               |             221.585 |                     152.577 |                      374.162 |
| starrynight.png |       597.94 |                      326.75 | 45.35%               |             242.981 |                     180.585 |                      423.566 |

---

# Escenario 3: Buena Solución Inicial - Tabu Moderado

| instancia       |   mse_greedy |   mse_greedy+tabu_moderado | mejora_%_moderado   |   tiempo_greedy (s) |   tiempo_tabu_moderado (s) |   tiempo_total_moderado (s) |
|-----------------|--------------|----------------------------|---------------------|---------------------|----------------------------|-----------------------------|
| bach.png        |       779.99 |                     460.29 | 40.99%              |             275.466 |                     33.398 |                     308.864 |
| dali.png        |       837.73 |                     624.45 | 25.46%              |             215.801 |                     24.498 |                     240.299 |
| klimt.png       |       874.42 |                     718.35 | 17.85%              |             256.233 |                     29.914 |                     286.146 |
| mona.png        |       486.62 |                     359.21 | 26.18%              |             275.625 |                     32.495 |                     308.12  |
| mondriaan.png   |       604.46 |                     402.1  | 33.48%              |             259.175 |                     31.857 |                     291.032 |
| pollock.png     |      2424.94 |                    2124.86 | 12.37%              |             220.611 |                     24.763 |                     245.374 |
| starrynight.png |       597.94 |                     482.43 | 19.32%              |             243.372 |                     27.874 |                     271.245 |

---

# Escenario 1: Mala Solución Inicial - Tabu Intensivo

| instancia       |   mse_greedy |   mse_greedy+tabu_intensivo | mejora_%_intensivo   |   tiempo_greedy (s) |   tiempo_tabu_intensivo (s) |   tiempo_total_intensivo (s) |
|-----------------|--------------|-----------------------------|----------------------|---------------------|-----------------------------|------------------------------|
| bach.png        |      1911.69 |                      291.79 | 84.74%               |              20.394 |                     158.074 |                      178.467 |
| dali.png        |      1588.46 |                      590.49 | 62.83%               |              16.503 |                     112.97  |                      129.473 |
| klimt.png       |      1354.41 |                      665.69 | 50.85%               |              18.754 |                     142.447 |                      161.201 |
| mona.png        |       988.83 |                      327.78 | 66.85%               |              21.425 |                     155.791 |                      177.215 |
| mondriaan.png   |      1351.89 |                      226.36 | 83.26%               |              16.391 |                     125.849 |                      142.241 |
| pollock.png     |      3531.67 |                     2246.76 | 36.38%               |              17.032 |                    2245.75  |                     2262.78  |
| starrynight.png |       998.72 |                      460.93 | 53.85%               |              19.608 |                     137.279 |                      156.887 |

---

# Escenario 1: Mala Solución Inicial - Tabu Moderado

| instancia       |   mse_greedy |   mse_greedy+tabu_moderado | mejora_%_moderado   |   tiempo_greedy (s) |   tiempo_tabu_moderado (s) |   tiempo_total_moderado (s) |
|-----------------|--------------|----------------------------|---------------------|---------------------|----------------------------|-----------------------------|
| bach.png        |      1911.69 |                     909.29 | 52.44%              |              20.519 |                     25.097 |                      45.615 |
| dali.png        |      1588.46 |                    1061.49 | 33.17%              |              16.498 |                     18.446 |                      34.944 |
| klimt.png       |      1354.41 |                     950.74 | 29.80%              |              18.704 |                     20.934 |                      39.638 |
| mona.png        |       988.83 |                     580.15 | 41.33%              |              21.475 |                     24.228 |                      45.703 |
| mondriaan.png   |      1351.89 |                     673.76 | 50.16%              |              15.85  |                     17.804 |                      33.654 |
| pollock.png     |      3531.67 |                    3012.35 | 14.70%              |              16.989 |                     18.195 |                      35.184 |
| starrynight.png |       998.72 |                     672.13 | 32.70%              |              19.56  |                     22.733 |                      42.292 |

---

# Escenario 2: Solución Inicial Moderada - Tabu Intensivo

| instancia       |   mse_greedy |   mse_greedy+tabu_intensivo | mejora_%_intensivo   |   tiempo_greedy (s) |   tiempo_tabu_intensivo (s) |   tiempo_total_intensivo (s) |
|-----------------|--------------|-----------------------------|----------------------|---------------------|-----------------------------|------------------------------|
| bach.png        |      1042.53 |                      208.19 | 80.03%               |             104.515 |                     193.582 |                      298.097 |
| dali.png        |      1115.17 |                      427.14 | 61.70%               |              92.317 |                     237.653 |                      329.97  |
| klimt.png       |      1059.64 |                      527.33 | 50.24%               |             104.241 |                     178.337 |                      282.578 |
| mona.png        |       650.8  |                      251.56 | 61.35%               |             108.629 |                     195.429 |                      304.058 |
| mondriaan.png   |       891.62 |                      172.89 | 80.61%               |              94.811 |                     178.707 |                      273.518 |
| pollock.png     |      2878.39 |                     1739.36 | 39.57%               |              94.254 |                     142.23  |                      236.484 |
| starrynight.png |       760.27 |                      394.53 | 48.11%               |              97.317 |                     163.833 |                      261.149 |

---

# Escenario 2: Solución Inicial Moderada - Tabu Moderado

| instancia       |   mse_greedy |   mse_greedy+tabu_moderado | mejora_%_moderado   |   tiempo_greedy (s) |   tiempo_tabu_moderado (s) |   tiempo_total_moderado (s) |
|-----------------|--------------|----------------------------|---------------------|---------------------|----------------------------|-----------------------------|
| bach.png        |      1042.53 |                     570.36 | 45.29%              |             104.545 |                     30.081 |                     134.625 |
| dali.png        |      1115.17 |                     732.68 | 34.30%              |              92.075 |                     24.096 |                     116.171 |
| klimt.png       |      1059.64 |                     816.26 | 22.97%              |             103.158 |                     27.349 |                     130.507 |
| mona.png        |       650.8  |                     435.16 | 33.13%              |             108.648 |                     29.099 |                     137.747 |
| mondriaan.png   |       891.62 |                     534.57 | 40.05%              |              95.047 |                     27.489 |                     122.536 |
| pollock.png     |      2878.39 |                    2477.03 | 13.94%              |              93.89  |                     22.735 |                     116.626 |
| starrynight.png |       760.27 |                     580.3  | 23.67%              |              97.538 |                     25.129 |                     122.667 |

---

