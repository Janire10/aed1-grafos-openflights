#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <cmath>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <iomanip>

using namespace std;

const double INF = numeric_limits<double>::infinity();

struct Aeropuerto {
    int id;
    string iata;
    double lat;
    double lon;
    string ciudad;
};

struct Arista {
    int destino;
    double distancia;
};

struct ResultadoRuta {
    bool existe;
    int escalas;
    double distancia_total;
    vector<int> camino;
    double tiempo_ms;
};

class RedAerea {
private:
    int num_nodos;
    vector<Aeropuerto> aeropuertos;
    vector<vector<Arista>> adj;      
    vector<vector<Arista>> adj_inv;  
    unordered_map<string, int> iata_a_id;
    vector<bool> nodo_activo;        

   void dfs_orden(int inicio, vector<bool>& visitado, stack<int>& pila) const {
        stack<pair<int, size_t>> st;
        visitado[inicio] = true;
        st.push({inicio, 0});

        while (!st.empty()) {
            int u = st.top().first;
            size_t& idx = st.top().second;

            if (idx < adj[u].size()) {
                int v = adj[u][idx].destino;
                ++idx;
                if (nodo_activo[v] && !visitado[v]) {
                    visitado[v] = true;
                    st.push({v, 0});
                }
            } else {
                pila.push(u);
                st.pop();
            }
        }
    }

    void dfs_componente(int inicio, vector<bool>& visitado, vector<int>& componente) const {
        stack<int> st;
        visitado[inicio] = true;
        st.push(inicio);

        while (!st.empty()) {
            int u = st.top();
            st.pop();
            componente.push_back(u);

            for (const auto& arista : adj_inv[u]) {
                int v = arista.destino;
                if (nodo_activo[v] && !visitado[v]) {
                    visitado[v] = true;
                    st.push(v);
                }
            }
        }
    }

public:
    RedAerea(const string& ruta_archivo) {
        ifstream archivo(ruta_archivo);
        if (!archivo.is_open()) {
            throw runtime_error("No se pudo abrir el archivo: " + ruta_archivo);
        }

        string linea;
        getline(archivo, linea);
        istringstream iss_cabecera(linea);
        int num_aristas;
        iss_cabecera >> num_nodos >> num_aristas;

        aeropuertos.resize(num_nodos);
        adj.resize(num_nodos);
        adj_inv.resize(num_nodos);
        nodo_activo.assign(num_nodos, true);

        for (int i = 0; i < num_nodos; ++i) {
            getline(archivo, linea);
            istringstream iss(linea);
            iss >> aeropuertos[i].id >> aeropuertos[i].iata
                >> aeropuertos[i].lat >> aeropuertos[i].lon;
            if (!(iss >> aeropuertos[i].ciudad)) {
                aeropuertos[i].ciudad = "Desconocida";
            }
            iata_a_id[aeropuertos[i].iata] = aeropuertos[i].id;
        }

        for (int i = 0; i < num_aristas; ++i) {
            if (!getline(archivo, linea)) break;
            istringstream iss(linea);
            int u, v;
            double dist;
            if (iss >> u >> v >> dist) {
                if (u >= 0 && u < num_nodos && v >= 0 && v < num_nodos) {
                    adj[u].push_back({v, dist});
                    adj_inv[v].push_back({u, dist});
                }
            }
        }
    }

    int obtener_id(const string& iata) const {
        auto it = iata_a_id.find(iata);
        return (it != iata_a_id.end()) ? it->second : -1;
    }

    string obtener_iata(int id) const {
        return aeropuertos[id].iata;
    }

    int total_nodos() const { return num_nodos; }

    void desactivar_nodo(int id) {
        if (id >= 0 && id < num_nodos) nodo_activo[id] = false;
    }

    void restaurar_nodos() {
        fill(nodo_activo.begin(), nodo_activo.end(), true);
    }

    ResultadoRuta bfs_min_escalas(int origen, int destino) const {
        auto inicio = chrono::high_resolution_clock::now();

        vector<int> dist_saltos(num_nodos, -1);
        vector<double> dist_km(num_nodos, 0.0);
        vector<int> padre(num_nodos, -1);
        queue<int> cola;

        if (!nodo_activo[origen] || !nodo_activo[destino]) {
            return {false, -1, INF, {}, 0.0};
        }

        dist_saltos[origen] = 0;
        cola.push(origen);

        while (!cola.empty()) {
            int u = cola.front();
            cola.pop();

            if (u == destino) break;

            for (const auto& arista : adj[u]) {
                int v = arista.destino;
                if (nodo_activo[v] && dist_saltos[v] == -1) {
                    dist_saltos[v] = dist_saltos[u] + 1;
                    dist_km[v] = dist_km[u] + arista.distancia;
                    padre[v] = u;
                    cola.push(v);
                }
            }
        }

        auto fin = chrono::high_resolution_clock::now();
        double ms = chrono::duration<double, milli>(fin - inicio).count();

        if (dist_saltos[destino] == -1) {
            return {false, -1, INF, {}, ms};
        }

        vector<int> camino;
        for (int curr = destino; curr != -1; curr = padre[curr]) {
            camino.push_back(curr);
        }
        reverse(camino.begin(), camino.end());

        return {true, dist_saltos[destino] - 1, dist_km[destino], camino, ms};
    }

    ResultadoRuta dijkstra_min_distancia(int origen, int destino) const {
        auto inicio = chrono::high_resolution_clock::now();

        vector<double> dist(num_nodos, INF);
        vector<int> saltos(num_nodos, 0);
        vector<int> padre(num_nodos, -1);

        using Par = pair<double, int>;
        priority_queue<Par, vector<Par>, greater<Par>> pq;

        if (!nodo_activo[origen] || !nodo_activo[destino]) {
            return {false, -1, INF, {}, 0.0};
        }

        dist[origen] = 0.0;
        pq.push({0.0, origen});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (d > dist[u]) continue;
            if (u == destino) break;

            for (const auto& arista : adj[u]) {
                int v = arista.destino;
                if (!nodo_activo[v]) continue;

                if (dist[u] + arista.distancia < dist[v]) {
                    dist[v] = dist[u] + arista.distancia;
                    saltos[v] = saltos[u] + 1;
                    padre[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        auto fin = chrono::high_resolution_clock::now();
        double ms = chrono::duration<double, milli>(fin - inicio).count();

        if (dist[destino] == INF) {
            return {false, -1, INF, {}, ms};
        }

        vector<int> camino;
        for (int curr = destino; curr != -1; curr = padre[curr]) {
            camino.push_back(curr);
        }
        reverse(camino.begin(), camino.end());

        return {true, saltos[destino] - 1, dist[destino], camino, ms};
    }

    vector<vector<int>> componentes_fuertemente_conexas(double& tiempo_ms) const {
        auto inicio = chrono::high_resolution_clock::now();

        vector<bool> visitado(num_nodos, false);
        stack<int> pila;

        for (int i = 0; i < num_nodos; ++i) {
            if (nodo_activo[i] && !visitado[i]) {
                dfs_orden(i, visitado, pila);
            }
        }

        fill(visitado.begin(), visitado.end(), false);
        vector<vector<int>> sccs;

        while (!pila.empty()) {
            int u = pila.top();
            pila.pop();
            if (nodo_activo[u] && !visitado[u]) {
                vector<int> componente;
                dfs_componente(u, visitado, componente);
                sccs.push_back(componente);
            }
        }

        sort(sccs.begin(), sccs.end(), [](const vector<int>& a, const vector<int>& b) {
            return a.size() > b.size();
        });

        auto fin = chrono::high_resolution_clock::now();
        tiempo_ms = chrono::duration<double, milli>(fin - inicio).count();
        return sccs;
    }

    vector<pair<int, int>> top_hubs(int k) const {
        vector<pair<int, int>> grados;
        for (int i = 0; i < num_nodos; ++i) {
            int grado = static_cast<int>(adj[i].size() + adj_inv[i].size());
            grados.push_back({grado, i});
        }
        sort(grados.rbegin(), grados.rend());
        if (k < static_cast<int>(grados.size())) grados.resize(k);
        return grados;
    }
};

string formatear_ruta(const RedAerea& red, const vector<int>& camino) {
    string res;
    for (size_t i = 0; i < camino.size(); ++i) {
        res += red.obtener_iata(camino[i]);
        if (i + 1 < camino.size()) res += " -> ";
    }
    return res;
}

int main() {
    try {
        RedAerea red("data/vuelos.graph");
        cout << "=== RED AEREA CARGADA (" << red.total_nodos() << " aeropuertos) ===\n\n";

        vector<pair<string, string>> rutas_prueba = {
            {"ALC", "SYD"}, // Alicante -> Sídney
            {"MAD", "AKL"}, // Madrid -> Auckland
            {"LEI", "HNL"}, // Almería -> Honolulú
            {"VLC", "NRT"}, // Valencia -> Tokio Narita
            {"BIO", "EZE"}, // Bilbao -> Buenos Aires
            {"SCQ", "SIN"}, // Santiago -> Singapur
            {"AGP", "ANC"}, // Málaga -> Anchorage (Alaska)
            {"SVQ", "ICN"}  // Sevilla -> Seúl
        };

        ofstream csv_rutas("resultados/resultados_rutas.csv");
        csv_rutas << "Origen,Destino,BFS_Escalas,BFS_Km,BFS_Tiempo_ms,Dijkstra_Escalas,Dijkstra_Km,Dijkstra_Tiempo_ms,Diferencia_Km\n";

        cout << "--- EXPERIMENTO 1: BFS vs DIJKSTRA ---\n";
        for (const auto& [orig_str, dest_str] : rutas_prueba) {
            int u = red.obtener_id(orig_str);
            int v = red.obtener_id(dest_str);
            if (u == -1 || v == -1) continue;

            ResultadoRuta r_bfs = red.bfs_min_escalas(u, v);
            ResultadoRuta r_dij = red.dijkstra_min_distancia(u, v);

            if (r_bfs.existe && r_dij.existe) {
                double ahorro_km = r_bfs.distancia_total - r_dij.distancia_total;
                cout << orig_str << " -> " << dest_str << ":\n";
                cout << "  [BFS]      " << r_bfs.escalas << " escalas | " << fixed << setprecision(1)
                     << r_bfs.distancia_total << " km | Ruta: " << formatear_ruta(red, r_bfs.camino) << "\n";
                cout << "  [Dijkstra] " << r_dij.escalas << " escalas | "
                     << r_dij.distancia_total << " km | Ruta: " << formatear_ruta(red, r_dij.camino) << "\n";

                csv_rutas << orig_str << "," << dest_str << ","
                          << r_bfs.escalas << "," << r_bfs.distancia_total << "," << r_bfs.tiempo_ms << ","
                          << r_dij.escalas << "," << r_dij.distancia_total << "," << r_dij.tiempo_ms << ","
                          << ahorro_km << "\n";
            }
        }
        csv_rutas.close();

        double tiempo_scc = 0.0;
        auto sccs = red.componentes_fuertemente_conexas(tiempo_scc);

        cout << "\n--- EXPERIMENTO 2: COMPONENTES FUERTEMENTE CONEXAS ---\n";
        cout << "Total de CFC encontradas: " << sccs.size() << " (Tiempo: " << tiempo_scc << " ms)\n";
        cout << "Tamano de la CFC gigante (nucleo mundial): " << sccs[0].size() << " aeropuertos ("
             << (100.0 * sccs[0].size() / red.total_nodos()) << "%)\n";

        ofstream csv_scc("resultados/resultados_cfc.csv");
        csv_scc << "Componente_ID,Tamano,Porcentaje\n";
        for (size_t i = 0; i < min<size_t>(15, sccs.size()); ++i) {
            csv_scc << (i + 1) << "," << sccs[i].size() << ","
                    << (100.0 * sccs[i].size() / red.total_nodos()) << "\n";
        }
        csv_scc.close();

        cout << "\n--- EXPERIMENTO 3: SIMULACION DE CAIDA DE HUBS ---\n";
        auto hubs = red.top_hubs(15);

        ofstream csv_resiliencia("resultados/resultados_resiliencia.csv");
        csv_resiliencia << "Hubs_Eliminados,Ultimo_Hub_Cerrado,Tamano_CFC_Gigante,Porcentaje_Red_Conectada\n";
        csv_resiliencia << "0,Ninguno," << sccs[0].size() << ","
                        << (100.0 * sccs[0].size() / red.total_nodos()) << "\n";

        for (size_t i = 0; i < hubs.size(); ++i) {
            int hub_id = hubs[i].second;
            red.desactivar_nodo(hub_id);
            double t_aux = 0.0;
            auto sccs_actual = red.componentes_fuertemente_conexas(t_aux);
            double pct = 100.0 * sccs_actual[0].size() / red.total_nodos();

            cout << "Cerrando #" << (i + 1) << " (" << red.obtener_iata(hub_id)
                 << ", grado " << hubs[i].first << ") -> Nucleo conectado: "
                 << sccs_actual[0].size() << " (" << pct << "%)\n";

            csv_resiliencia << (i + 1) << "," << red.obtener_iata(hub_id) << ","
                            << sccs_actual[0].size() << "," << pct << "\n";
        }
        csv_resiliencia.close();
        red.restaurar_nodos();

        cout << "\n[OK] Resultados exportados a resultados_rutas.csv, resultados_cfc.csv y resultados_resiliencia.csv\n";

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}