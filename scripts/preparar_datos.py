import urllib.request
import csv
import math
from pathlib import Path

Path("data").mkdir(exist_ok=True)

URL_AIRPORTS = "https://raw.githubusercontent.com/jpatokal/openflights/master/data/airports.dat"
URL_ROUTES = "https://raw.githubusercontent.com/jpatokal/openflights/master/data/routes.dat"

urllib.request.urlretrieve(URL_AIRPORTS, "data/airports.dat")
urllib.request.urlretrieve(URL_ROUTES, "data/routes.dat")

def haversine(lat1, lon1, lat2, lon2):
    R = 6371.0
    p1, p2 = math.radians(lat1), math.radians(lat2)
    dp = math.radians(lat2 - lat1)
    dl = math.radians(lon2 - lon1)
    a = math.sin(dp/2)**2 + math.cos(p1)*math.cos(p2)*math.sin(dl/2)**2
    return 2 * R * math.asin(math.sqrt(a))

aeropuertos = {}
with open("data/airports.dat", encoding="utf-8") as f:
    for row in csv.reader(f):
        try:
            aid = int(row[0])
            nombre, ciudad, pais, iata = row[1], row[2], row[3], row[4]
            lat, lon = float(row[6]), float(row[7])
            if iata != "\\N":
                aeropuertos[aid] = (iata, nombre, ciudad, pais, lat, lon)
        except (ValueError, IndexError):
            continue

id_to_idx = {aid: idx for idx, aid in enumerate(sorted(aeropuertos.keys()))}
idx_to_info = {idx: aeropuertos[aid] for aid, idx in id_to_idx.items()}

aristas = {}
with open("data/routes.dat", encoding="utf-8") as f:
    for row in csv.reader(f):
        try:
            orig, dest = int(row[3]), int(row[5])
            if orig in id_to_idx and dest in id_to_idx and orig != dest:
                u, v = id_to_idx[orig], id_to_idx[dest]
                lat1, lon1 = aeropuertos[orig][4], aeropuertos[orig][5]
                lat2, lon2 = aeropuertos[dest][4], aeropuertos[dest][5]
                dist = haversine(lat1, lon1, lat2, lon2)
                if (u, v) not in aristas or dist < aristas[(u, v)]:
                    aristas[(u, v)] = dist
        except (ValueError, IndexError):
            continue

with open("data/vuelos.graph", "w", encoding="utf-8") as out:
    out.write(f"{len(idx_to_info)} {len(aristas)}\n")
    for idx in range(len(idx_to_info)):
        iata, nombre, ciudad, pais, lat, lon = idx_to_info[idx]
        out.write(f"{idx} {iata} {lat:.6f} {lon:.6f} {ciudad.replace(' ', '_')}\n")
    for (u, v), dist in sorted(aristas.items()):
        out.write(f"{u} {v} {dist:.2f}\n")

print(f"Grafo generado: {len(idx_to_info)} aeropuertos y {len(aristas)} rutas dirigidas.")