import csv
import matplotlib.pyplot as plt

plt.style.use('seaborn-v0_8-whitegrid')

#grafica 1 comparativa distancia
rutas, bfs_km, dij_km = [], [], []
bfs_ms, dij_ms = [], []

with open("resultados_rutas.csv", encoding="utf-8") as f:
    reader = csv.DictReader(f)
    for row in reader:
        rutas.append(f"{row['Origen']}→{row['Destino']}")
        bfs_km.append(float(row['BFS_Km']))
        dij_km.append(float(row['Dijkstra_Km']))
        bfs_ms.append(float(row['BFS_Tiempo_ms']))
        dij_ms.append(float(row['Dijkstra_Tiempo_ms']))

x = range(len(rutas))
width = 0.36

fig, ax = plt.subplots(figsize=(10, 5))
rects1 = ax.bar([i - width/2 for i in x], bfs_km, width, label='BFS (Mínimas escalas)', color='#3b82f6')
rects2 = ax.bar([i + width/2 for i in x], dij_km, width, label='Dijkstra (Mínima distancia)', color='#10b981')

ax.set_ylabel('Distancia total recorrida (km)', fontsize=11)
ax.set_title('Comparativa de Distancia: Ruta con menos escalas (BFS) vs. Ruta más corta (Dijkstra)', fontsize=12, fontweight='bold')
ax.set_xticks(list(x))
ax.set_xticklabels(rutas, fontsize=10)
ax.legend()

plt.tight_layout()
plt.savefig("grafica1_distancia_bfs_dijkstra.png", dpi=300)
plt.close()

#grafica 2 comparativa tiempos de ejecucion
fig, ax = plt.subplots(figsize=(10, 4.5))
ax.plot(rutas, bfs_ms, marker='o', linewidth=2, label='BFS - O(|V| + |E|)', color='#3b82f6')
ax.plot(rutas, dij_ms, marker='s', linewidth=2, label='Dijkstra (Min-Heap) - O((|V|+|E|) log |V|)', color='#f59e0b')

ax.set_ylabel('Tiempo de ejecución (ms)', fontsize=11)
ax.set_title('Rendimiento Computacional en C++: BFS vs. Dijkstra', fontsize=12, fontweight='bold')
ax.legend()

plt.tight_layout()
plt.savefig("grafica2_tiempos_ejecucion.png", dpi=300)
plt.close()

#grafica 3 resilencia ante el cierre progresivo de hubs
hubs_cerrados, etiquetas_hubs, pct_conectado = [], [], []

with open("resultados_resiliencia.csv", encoding="utf-8") as f:
    reader = csv.DictReader(f)
    for row in reader:
        hubs_cerrados.append(int(row['Hubs_Eliminados']))
        etiquetas_hubs.append(row['Ultimo_Hub_Cerrado'])
        pct_conectado.append(float(row['Porcentaje_Red_Conectada']))

fig, ax = plt.subplots(figsize=(10, 5))
ax.plot(hubs_cerrados, pct_conectado, marker='o', linewidth=2.5, color='#ef4444')
ax.fill_between(hubs_cerrados, pct_conectado, min(pct_conectado)-0.5, color='#ef4444', alpha=0.1)

for i, txt in enumerate(etiquetas_hubs):
    if i > 0:
        ax.annotate(txt, (hubs_cerrados[i], pct_conectado[i]), textcoords="offset points", xytext=(0, 8), ha='center', fontsize=8.5, fontweight='bold')

ax.set_xlabel('Número de super-hubs cerrados progresivamente', fontsize=11)
ax.set_ylabel('Porcentaje de aeropuertos en la CFC gigante (%)', fontsize=11)
ax.set_title('Resiliencia de la Red Aérea Mundial ante el Cierre de los Mayores Hubs', fontsize=12, fontweight='bold')
ax.set_xticks(hubs_cerrados)
ax.set_ylim(min(pct_conectado) - 0.5, max(pct_conectado) + 0.5)

plt.tight_layout()
plt.savefig("grafica3_resiliencia_hubs.png", dpi=300)
plt.close()

print("[OK] Las 3 graficas se han generado correctamente en formato PNG.")