#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <chrono>
#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <map>
#include <algorithm>

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

// Estructura para almacenar el resultado consolidado de una configuración
struct BenchmarkResult {
    std::string variant_name;
    size_t image_size;
    size_t kernel_size;
    size_t num_threads;
    size_t runs;
    double mean_time_ms;
    double std_dev_ms;
    double min_time_ms;
    double max_time_ms;
    double speedup;     // Sp = T1 / Tp
    double efficiency;  // Ep = Sp / p
};

// Función para ejecutar un comando y capturar su salida estándar
std::string execute_command(const std::string& cmd) {
    std::array<char, 256> buffer;
    std::string result;
    FILE* pipe = POPEN(cmd.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("Error al abrir pipe para el comando: " + cmd);
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        result += buffer.data();
    }
    int return_code = PCLOSE(pipe);
    if (return_code != 0) {
        std::cerr << "[ADVERTENCIA] El comando retorno codigo: " << return_code << " -> " << cmd << "\n";
    }
    return result;
}

// Extrae el tiempo numérico (en ms) de la salida estándar del programa
// El programa objetivo debe imprimir una línea con "TIME_MS:<valor>" o simplemente el número en ms
double parse_time_from_output(const std::string& output) {
    std::istringstream stream(output);
    std::string line;
    while (std::getline(stream, line)) {
        size_t tag_pos = line.find("TIME_MS:");
        if (tag_pos != std::string::npos) {
            std::string val_str = line.substr(tag_pos + 8);
            return std::stod(val_str);
        }
    }
    // Si no tiene el tag TIME_MS, intentamos parsear el primer double encontrado en la última línea no vacía
    std::istringstream last_stream(output);
    double last_num = -1.0;
    std::string token;
    while (last_stream >> token) {
        try {
            last_num = std::stod(token);
        } catch (...) {}
    }
    if (last_num >= 0.0) return last_num;

    throw std::runtime_error("No se pudo extraer el tiempo de ejecucion de la salida:\n" + output);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "===============================================================\n";
        std::cout << "         AI-Generated C++ Parallel Code Benchmarker            \n";
        std::cout << "===============================================================\n";
        std::cout << "Uso:\n";
        std::cout << "  " << argv[0] << " <ruta_ejecutable> [nombre_variante] [archivo_salida.csv] [num_repeticiones]\n\n";
        std::cout << "Ejemplos:\n";
        std::cout << "  " << argv[0] << " ./secuencial.exe Secuencial resultados_secuencial.csv 30\n";
        std::cout << "  " << argv[0] << " ./paralelo_ia.exe Paralelo_IA resultados_ia.csv 30\n";
        std::cout << "===============================================================\n";
        return 1;
    }

    std::string executable_path = argv[1];
    std::string variant_name = (argc >= 3) ? argv[2] : "Variante";
    std::string output_csv = (argc >= 4) ? argv[3] : "benchmark_results.csv";
    size_t runs_per_config = (argc >= 5) ? std::stoul(argv[4]) : 30;

    // Configuraciones especificadas en el proyecto
    const std::vector<size_t> image_sizes = {512, 1024, 2048, 4096};
    const std::vector<size_t> kernel_sizes = {3, 5, 7, 11};
    
    // Si es secuencial puro, evaluamos solo con 1 hilo; si es paralelo, evaluamos 1, 2, 4, 8, 16 hilos
    std::vector<size_t> thread_counts;
    std::string lower_variant = variant_name;
    for (auto& c : lower_variant) c = static_cast<char>(tolower(c));

    if (lower_variant.find("secuencial") != std::string::npos) {
        thread_counts = {1};
    } else {
        thread_counts = {1, 2, 4, 8, 16};
    }

    std::cout << "\n>>> INICIANDO BENCHMARK: " << variant_name << " <<<\n";
    std::cout << "Ejecutable       : " << executable_path << "\n";
    std::cout << "Repeticiones     : " << runs_per_config << " por configuracion\n";
    std::cout << "Tamanos Imagen   : {512, 1024, 2048, 4096}\n";
    std::cout << "Tamanos Kernel   : {3x3, 5x5, 7x7, 11x11}\n";
    std::cout << "Hilos a evaluar  : {";
    for (size_t i = 0; i < thread_counts.size(); ++i) {
        std::cout << thread_counts[i] << (i + 1 < thread_counts.size() ? ", " : "");
    }
    std::cout << "}\n";
    std::cout << "Archivo de salida: " << output_csv << "\n\n";

    std::vector<BenchmarkResult> results;
    // Mapa para almacenar el tiempo base T1 por cada par (image_size, kernel_size)
    std::map<std::pair<size_t, size_t>, double> baseline_t1;

    size_t total_configs = image_sizes.size() * kernel_sizes.size() * thread_counts.size();
    size_t current_config = 0;

    for (size_t img_size : image_sizes) {
        for (size_t k_size : kernel_sizes) {
            for (size_t threads : thread_counts) {
                current_config++;
                std::cout << "[" << std::setw(2) << current_config << "/" << total_configs << "] "
                          << "Img: " << std::setw(4) << img_size << "x" << img_size 
                          << " | Kernel: " << std::setw(2) << k_size << "x" << k_size 
                          << " | Hilos: " << std::setw(2) << threads << " -> ";
                std::cout.flush();

                std::vector<double> times;
                times.reserve(runs_per_config);

                for (size_t r = 0; r < runs_per_config; ++r) {
                    // Formato de invocacion: <ejecutable> <img_size> <kernel_size> <num_threads>
                    std::ostringstream cmd;
                    cmd << executable_path << " " << img_size << " " << k_size << " " << threads;

                    try {
                        std::string output = execute_command(cmd.str());
                        double t_ms = parse_time_from_output(output);
                        times.push_back(t_ms);
                    } catch (const std::exception& e) {
                        std::cerr << "\n[ERROR en corrida " << (r + 1) << "]: " << e.what() << "\n";
                        return 1;
                    }
                }

                // Cálculo de métricas estadísticas
                double sum = std::accumulate(times.begin(), times.end(), 0.0);
                double mean = sum / times.size();

                double sq_sum = 0.0;
                double min_val = times[0];
                double max_val = times[0];
                for (double t : times) {
                    sq_sum += (t - mean) * (t - mean);
                    if (t < min_val) min_val = t;
                    if (t > max_val) max_val = t;
                }
                double std_dev = std::sqrt(sq_sum / times.size());

                // Guardar T1 para speedup
                if (threads == 1) {
                    baseline_t1[{img_size, k_size}] = mean;
                }

                double t1 = baseline_t1[{img_size, k_size}];
                double speedup = (threads == 1 || t1 == 0.0) ? 1.0 : (t1 / mean);
                double efficiency = speedup / static_cast<double>(threads);

                BenchmarkResult br{
                    variant_name,
                    img_size,
                    k_size,
                    threads,
                    runs_per_config,
                    mean,
                    std_dev,
                    min_val,
                    max_val,
                    speedup,
                    efficiency
                };
                results.push_back(br);

                std::cout << "Media: " << std::fixed << std::setprecision(2) << mean << " ms "
                          << "(+- " << std_dev << " ms) | Speedup: " 
                          << std::setprecision(2) << speedup << "x | Eficiencia: " 
                          << std::setprecision(2) << (efficiency * 100.0) << "%\n";
            }
        }
    }

    // Exportar a CSV compatible con Excel regional espanol (; y coma decimal)
    auto format_dec = [](double val, int prec = 4) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(prec) << val;
        std::string s = ss.str();
        std::replace(s.begin(), s.end(), '.', ',');
        return s;
    };

    std::ofstream file(output_csv);
    if (!file.is_open()) {
        std::cerr << "Error al abrir archivo para escribir CSV: " << output_csv << "\n";
        return 1;
    }

    file << "sep=;\n";
    file << "Variante;Tamano_Imagen;Tamano_Kernel;Hilos;Repeticiones;Tiempo_Medio_ms;Desv_Estandar_ms;Tiempo_Min_ms;Tiempo_Max_ms;Speedup;Eficiencia\n";
    for (const auto& r : results) {
        file << r.variant_name << ";"
             << r.image_size << ";"
             << r.kernel_size << ";"
             << r.num_threads << ";"
             << r.runs << ";"
             << format_dec(r.mean_time_ms) << ";"
             << format_dec(r.std_dev_ms) << ";"
             << format_dec(r.min_time_ms) << ";"
             << format_dec(r.max_time_ms) << ";"
             << format_dec(r.speedup) << ";"
             << format_dec(r.efficiency) << "\n";
    }
    file.close();

    std::cout << "\n[OK] Benchmark completado con exito. Resultados guardados en: " << output_csv << "\n";
    return 0;
}
