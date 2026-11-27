
# Análisis de Conectividad, Rutas Óptimas y Resiliencia en la Red Aérea Mundial (OpenFlights)

Proyecto de investigación experimental para la asignatura **Algoritmos y Estructuras de Datos I (AED1)** — Grado en Ingeniería Informática (Universidad de Murcia, Curso 26/27).

## Descripción del problema
Modelado de la red mundial de transporte aéreo comercial (**OpenFlights**) como un grafo dirigido y ponderado $G = (V, E, W)$ con **6.072 aeropuertos** y **36.707 rutas directas**, donde el peso de cada arista es la distancia ortodrómica (en km) calculada mediante la fórmula de Haversine.

El núcleo algorítmico está implementado en **C++17** utilizando **listas de adyacencia** e incluye:
1. **Búsqueda en Anchura (BFS):** Cálculo de rutas con el mínimo número de escalas ($O(|V| + |E|)$).
2. **Algoritmo de Dijkstra (con montículo binario `priority_queue`):** Cálculo de rutas de mínima distancia en kilómetros ($O((|V| + |E|) \log |V|)$).
3. **Algoritmo de Kosaraju (doble DFS iterativa):** Detección de Componentes Fuertemente Conexas (CFC) y simulación de resiliencia ante el cierre progresivo de los mayores *super-hubs* internacionales.

## Estructura del proyecto
- `preparar_datos.py`: Descarga los datasets reales de OpenFlights (`airports.dat` y `routes.dat`), calcula distancias y genera el grafo limpio en `data/vuelos.graph`.
- `main.cpp`: Implementación del TAD Grafo y ejecución de los tres experimentos con medición de tiempos (`<chrono>`) y exportación a `.csv`.
- `generar_graficas.py`: Generación automática de las gráficas comparativas (`.png`) a partir de los resultados obtenidos en C++.
- `Makefile`: Automatización de compilación y ejecución en entornos Unix/Linux.
- `Memoria_AED1.pdf`: Memoria completa con el análisis teórico, experimental y conclusiones.

## Instrucciones de compilación y ejecución

### Requisitos
- Compilador `g++` con soporte para C++17.
- Python 3 con `matplotlib` (`pip install matplotlib`).
- Conexión a Internet para descargar los datos desde OpenFlights mediante `preparar_datos.py` (opcional si se utiliza directamente el fichero `data/vuelos.graph`ya preprocesado).

### En Linux / macOS / WSL (usando `make`)
```bash
#Ejecutar todo el flujo completo (descarga, compilación, experimentos y gráficas)
make todo
```

### O paso a paso: 
```bash
make datos      # Genera data/vuelos.graph
make            # Compila main.cpp -> ./vuelos
make ejecutar   # Ejecuta los experimentos y genera los archivos .csv
make graficas   # Genera las imágenes .png
```

### En Windows (PowerShell)
```PowerShell
python preparar_datos.py
g++ -std=c++17 -O2 main.cpp -o vuelos.exe
.\vuelos.exe
python generar_graficas.py
```