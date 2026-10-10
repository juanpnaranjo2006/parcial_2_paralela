# Registro de Interacción con la IA (Prompt Engineering y Generación de Código)

**Proyecto:** AI-Generated C++ Parallel Code Benchmarking  
**Asignatura:** Computación Paralela y Distribuida  
**Fecha:** Octubre 2026  

---

## 👤 Turno 1 - Solicitud del Usuario (Prompt)

como genero números en un rango de forma que la distribución sea uniforme?

---

## 🤖 Turno 1 - Respuesta de la IA

En C++ moderno (C++11 en adelante), la mejor forma de generar números con distribución uniforme es utilizando la biblioteca **`<random>`**.

---

### 1. Números reales (flotantes, ej. `[0.0, 1.0]`)
Ideal para el método de **Monte Carlo**:

```cpp
#include <random>
#include <iostream>

int main() {
    // 1. Dispositivo para obtener una semilla aleatoria del hardware
    std::random_device rd;

    // 2. Motor de números pseudoaleatorios (Mersenne Twister de 64 bits)
    std::mt19937_64 gen(rd());

    // 3. Distribución uniforme en el rango real [0.0, 1.0)
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // 4. Generación
    double x = dist(gen);
    double y = dist(gen);

    std::cout << "x: " << x << ", y: " << y << std::endl;
}
```

---

### 2. Números enteros (ej. `[min, max]`)
Si necesitas enteros entre $A$ y $B$ (ambos inclusivos):

```cpp
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dist(1, 100); // Rango [1, 100]

int numero = dist(gen);
```

---

### ⚠️ Importante para Programación Concurrente / Paralela (`std::thread`)
Si generas números dentro de cada hilo (como en [09-mcpi.cpp](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/09-mcpi.cpp)):

- **No compartas el mismo generador entre hilos** sin sincronización (no es *thread-safe* y causará condiciones de carrera o contención).
- **Crea el generador dentro de la función del hilo** (`monte_carlo_part`) con su propia semilla:

```cpp
void monte_carlo_part(size_t n_samples, double *result_ptr) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    size_t inside_circle = 0;
    for (size_t i = 0; i < n_samples; ++i) {
        double x = dist(gen);
        double y = dist(gen);
        if (x * x + y * y <= 1.0) {
            inside_circle++;
        }
    }
    *result_ptr = inside_circle;
}
```

---

## 👤 Turno 2 - Solicitud del Usuario (Prompt)

Con base en este contexto, podrías empezar por crear un archivo tester.cpp en la carpeta proyecto_parcial2 que sea capaz de llamar un archivo .cpp y lo ejecute y saque las métricas con todas las configuraciones posibles?

---

## 🤖 Turno 2 - Respuesta de la IA

He creado el archivo [tester.cpp](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/tester.cpp) y lo he verificado compilando exitosamente con `g++ -O3`.

### 🚀 ¿Qué hace este `tester.cpp`?

Cumple estrictamente con las especificaciones del proyecto:
1. **Espacio de búsqueda completo**:
   - **Tamaños de imagen**: $512\times512$, $1024\times1024$, $2048\times2048$, $4096\times4096$ px.
   - **Tamaños de kernel**: $3\times3$, $5\times5$, $7\times7$, $11\times11$.
   - **Hilos paralelos**: $1, 2, 4, 8, 16$ hilos (para secuencial evalúa 1 hilo).
2. **Protocolo y Repeticiones**:
   - Ejecuta **30 repeticiones** por cada combinación.
3. **Métricas calculadas automáticamente**:
   - Tiempo promedio de ejecución ($\text{ms}$)
   - Desviación estándar ($\text{ms}$)
   - Tiempo mínimo y máximo ($\text{ms}$)
   - Speedup ($S_p = T_1 / T_p$)
   - Eficiencia ($E_p = S_p / p$)
4. **Exportación a CSV**:
   - Genera automáticamente un archivo `.csv` listo para tabular o graficar (en Python/Matplotlib o Excel) para el artículo en formato revista Javeriana.

---

### 📋 Convención para los programas a evaluar

Tus archivos ejecutables (`secuencial.exe`, `paralelo_ia.exe`, etc.) solo necesitan aceptar los argumentos de línea de comandos e imprimir el tiempo puro de procesamiento (sin I/O de disco) con el formato `TIME_MS:<tiempo>` o el número flotante en milisegundos:

**Argumentos esperados por el ejecutable:**
```bash
./mi_programa.exe <tamano_imagen> <tamano_kernel> <num_hilos>
```

**Salida esperada (en `stdout`):**
```cpp
// Ejemplo en tu programa:
std::cout << "TIME_MS:" << duration_ms << std::endl;
```

---

### 💻 Ejemplo de Uso del Tester

```bash
# Compilar el tester
g++ -O3 tester.cpp -o tester.exe

# Ejecutar benchmark para la versión secuencial (30 repeticiones)
./tester.exe ./secuencial.exe Secuencial resultados_secuencial.csv 30

# Ejecutar benchmark para la versión paralelo IA (30 repeticiones)
./tester.exe ./paralelo_ia.exe Paralelo_IA resultados_ia.csv 30
```

---

## 👤 Turno 3 - Solicitud del Usuario (Prompt)

ok, ahora haz un archivo "secuencial.cpp" que sea compatible con este diseño. Por simplicidad quemale filtros sobel de los tamaños disponibles y que ignore el num_hilos

---

## 🤖 Turno 3 - Respuesta de la IA

He creado y compilado [secuencial.cpp](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/secuencial.cpp). He lanzado una prueba rápida con `tester.exe` para validar la integración.

---

## 🤖 Turno 4 - Respuesta de la IA

El archivo [secuencial.cpp](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/secuencial.cpp) está listo y probado.

### Resumen de características implementadas:
1. **Filtros Sobel quemados:**
   - $3\times3$, $5\times5$, $7\times7$ y $11\times11$ mediante la función `get_sobel_kernel()`.
2. **Manejo de bordes:**
   - Clamping (fijación al píxel del borde más cercano).
3. **Medición:**
   - Uso de `std::chrono::high_resolution_clock` midiendo **únicamente** la función de convolución 2D.
   - Argumento de hilos ignorado automáticamente.
   - Salida compatible con el tag `TIME_MS:<tiempo>`.

### Verificación:
Probamos la integración completa con [tester.cpp](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/tester.cpp), evaluando las 16 configuraciones ($512\times512 \dots 4096\times4096$ y kernels $3\times3 \dots 11\times11$), calculando tiempos medios, desviaciones estándar y exportando el CSV correctamente.

---

## 👤 Turno 5 - Solicitud del Usuario (Prompt)

puedes correr la prueba con 30 ejecuciones por configuracion?

---

## 🤖 Turno 5 - Respuesta de la IA

He lanzado la prueba completa con **30 ejecuciones por configuración** para la versión secuencial (480 ejecuciones en total: 4 tamaños de imagen $\times$ 4 kernels $\times$ 30 repeticiones).

Los resultados se están procesando y se guardarán automáticamente en `resultados_secuencial.csv` (con el encabezado `sep=,` incluido para abrir directamente en Excel). Te notificaré tan pronto concluyan las mediciones.

---

## 🤖 Turno 6 - Respuesta de la IA

El benchmark de la versión **Secuencial** con **30 repeticiones por configuración** ha finalizado exitosamente.

Los resultados consolidados quedaron guardados en [resultados_secuencial.csv](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/resultados_secuencial.csv).

### Resumen de Métricas (Base de Referencia $T_1$):

| Imagen (px) | Kernel | Tiempo Medio ($T_1$) | Desv. Estándar ($\sigma$) | Rango [Mín - Máx] |
| :---: | :---: | :---: | :---: | :---: |
| **512 × 512** | $3\times3$ | **2.74 ms** | $\pm 0.09\text{ ms}$ | [2.68 - 3.05] ms |
| **512 × 512** | $5\times5$ | **7.48 ms** | $\pm 0.57\text{ ms}$ | [7.22 - 10.42] ms |
| **512 × 512** | $7\times7$ | **13.23 ms** | $\pm 0.40\text{ ms}$ | [12.94 - 14.71] ms |
| **512 × 512** | $11\times11$ | **28.84 ms** | $\pm 0.73\text{ ms}$ | [28.20 - 31.32] ms |
| **1024 × 1024** | $3\times3$ | **11.10 ms** | $\pm 0.46\text{ ms}$ | [10.75 - 12.40] ms |
| **1024 × 1024** | $5\times5$ | **29.38 ms** | $\pm 0.46\text{ ms}$ | [28.97 - 30.72] ms |
| **1024 × 1024** | $7\times7$ | **52.57 ms** | $\pm 1.23\text{ ms}$ | [51.70 - 58.11] ms |
| **1024 × 1024** | $11\times11$ | **122.86 ms** | $\pm 3.92\text{ ms}$ | [120.37 - 141.28] ms |
| **2048 × 2048** | $3\times3$ | **44.58 ms** | $\pm 1.22\text{ ms}$ | [43.31 - 49.11] ms |
| **2048 × 2048** | $5\times5$ | **119.42 ms** | $\pm 3.25\text{ ms}$ | [116.42 - 131.24] ms |
| **2048 × 2048** | $7\times7$ | **209.77 ms** | $\pm 1.31\text{ ms}$ | [208.45 - 214.68] ms |
| **2048 × 2048** | $11\times11$ | **489.52 ms** | $\pm 5.25\text{ ms}$ | [484.14 - 513.37] ms |
| **4096 × 4096** | $3\times3$ | **176.65 ms** | $\pm 3.58\text{ ms}$ | [174.24 - 193.06] ms |
| **4096 × 4096** | $5\times5$ | **470.21 ms** | $\pm 4.14\text{ ms}$ | [464.98 - 485.60] ms |
| **4096 × 4096** | $7\times7$ | **853.71 ms** | $\pm 18.58\text{ ms}$ | [839.17 - 932.20] ms |
| **4096 × 4096** | $11\times11$ | **1975.83 ms** | $\pm 9.85\text{ ms}$ | [1955.13 - 2001.92] ms |

¿Deseas que procedamos a crear la versión **Paralelo IA** (`paralelo_ia.cpp`) usando `std::thread`?

---

## 👤 Turno 7 - Solicitud del Usuario (Prompt)

Podrías modificar el csv resultante para utilizar sep=; y los deciamles cambiarlos por , para que mi Excel lo interprete mejor

---

## 🤖 Turno 7 - Respuesta de la IA

Listo, he aplicado los cambios:

1. **[resultados_secuencial.csv](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/resultados_secuencial.csv)**:
   - Encabezado `sep=;` al inicio.
   - Separador de columnas por punto y coma (`;`).
   - Todos los números decimales formateados con coma (`,`).

2. **[tester.cpp](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/tester.cpp) actualizado y recompilado**:
   - Todas las futuras ejecuciones del benchmark generarán automáticamente los archivos `.csv` en este formato para que Excel en español los reconozca de inmediato sin pasos adicionales.

---

## 👤 Turno 8 - Solicitud del Usuario (Prompt)

Muy bien, ahora sí por favor haz el archivo "paralelo_ia.cpp" y apenas termines haz una pequeña prueba con 2 ejecuciones por configuración

---

## 🤖 Turno 8 - Respuesta de la IA

He creado y compilado [paralelo_ia.cpp](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/paralelo_ia.cpp).

He iniciado la prueba con **2 ejecuciones por configuración** (80 configuraciones en total: 4 tamaños de imagen $\times$ 4 kernels $\times$ 5 cantidades de hilos [1, 2, 4, 8, 16]). Te avisaré tan pronto termine.

---

## 🤖 Turno 9 - Respuesta de la IA

El archivo [paralelo_ia.cpp](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/paralelo_ia.cpp) ha sido creado, compilado y evaluado exitosamente.

### 📌 Implementación de `paralelo_ia.cpp`:
- **Paralelización:** Descomposición de dominio 1D por bloques de filas distribuido entre $p$ hilos utilizando `std::thread`.
- **Equivalencia:** Mismos kernels Sobel ($3\times3, 5\times5, 7\times7, 11\times11$) y misma política de bordes (*clamping*) que la versión secuencial.
- **Medición:** Uso de `std::chrono::high_resolution_clock` midiendo exclusivamente la sección paralela (lanzamiento y sincronización de hilos).

---

### 📊 Resultados de la prueba preliminar (2 repeticiones / config):
El archivo [test_paralelo_ia.csv](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/test_paralelo_ia.csv) se generó correctamente con formato regional (`sep=;`, separadores `;` y coma para decimales `,`).

#### Muestra destacada de escalabilidad (Imagen 4096 × 4096, Kernel 11 × 11):
| Hilos ($p$) | Tiempo Medio | Speedup ($S_p$) | Eficiencia ($E_p$) |
| :---: | :---: | :---: | :---: |
| **1 hilo** | 1949,28 ms | 1,00x | 100,00% |
| **2 hilos** | 1033,95 ms | 1,89x | 94,26% |
| **4 hilos** | 543,39 ms | 3,59x | 89,68% |
| **8 hilos** | 340,83 ms | 5,72x | 71,49% |
| **16 hilos** | 228,51 ms | **8,53x** | 53,32% |

¿Deseas que lancemos la prueba completa con las **30 repeticiones** para `paralelo_ia.exe` o prefieres continuar con las variantes **Paralelo IA + Opt.** y **Paralelo Manual**?

---

## 👤 Turno 10 - Solicitud del Usuario (Prompt)

Ahora hagamos la prueba con las 30 repiticiones para cada configuración de paralelo_ia

---

## 🤖 Turno 10 - Respuesta de la IA

He lanzado la prueba exhaustiva de **30 repeticiones por configuración** para `paralelo_ia` (80 configuraciones $\times$ 30 ejecuciones = 2.400 mediciones en total).

Los resultados se guardarán automáticamente en `resultados_paralelo_ia.csv`. Te avisaré en cuanto finalice el proceso.

---

## 🤖 Turno 11 - Respuesta de la IA

El benchmark completo de **30 repeticiones por configuración** para la variante **Paralelo IA** ha finalizado exitosamente (2.400 ejecuciones en total).

Los resultados detallados quedaron guardados en [resultados_paralelo_ia.csv](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/resultados_paralelo_ia.csv) (con separador `;` y coma decimal `,`).

---

### 📊 Muestra de Resultados Consolidados (30 ejecuciones)

#### 1. Imagen 1024 × 1024 (Kernel 11 × 11)
| Hilos ($p$) | Tiempo Medio ($T_p$) | Desv. Estándar ($\sigma$) | Speedup ($S_p$) | Eficiencia ($E_p$) |
| :---: | :---: | :---: | :---: | :---: |
| **1 hilo** | 123,14 ms | $\pm 1,96\text{ ms}$ | 1,00x | 100,00% |
| **2 hilos** | 62,59 ms | $\pm 1,07\text{ ms}$ | 1,97x | 98,37% |
| **4 hilos** | 35,04 ms | $\pm 5,00\text{ ms}$ | 3,51x | 87,87% |
| **8 hilos** | 23,17 ms | $\pm 3,27\text{ ms}$ | 5,32x | 66,44% |
| **16 hilos** | 16,12 ms | $\pm 2,68\text{ ms}$ | **7,64x** | 47,74% |

#### 2. Imagen 4096 × 4096 (Kernel 11 × 11)
| Hilos ($p$) | Tiempo Medio ($T_p$) | Desv. Estándar ($\sigma$) | Speedup ($S_p$) | Eficiencia ($E_p$) |
| :---: | :---: | :---: | :---: | :---: |
| **1 hilo** | 1983,02 ms | $\pm 20,72\text{ ms}$ | 1,00x | 100,00% |
| **2 hilos** | 1015,99 ms | $\pm 12,57\text{ ms}$ | 1,95x | 97,59% |
| **4 hilos** | 547,73 ms | $\pm 19,58\text{ ms}$ | 3,62x | 90,51% |
| **8 hilos** | 342,78 ms | $\pm 12,68\text{ ms}$ | 5,79x | 72,31% |
| **16 hilos** | 265,41 ms | $\pm 10,58\text{ ms}$ | **7,47x** | 46,70% |

---

¿Continuamos con la siguiente variante: **C++ Paralelo IA + Opt.** (`paralelo_ia_opt.cpp`) implementando optimizaciones manuales (como vectorización SIMD / desenrollado de bucles / separabilidad de kernels)?

---

## 👤 Turno 12 - Solicitud del Usuario (Prompt)

creame solo el archivo y que sea idéntico al paralelo_ia por ahora

---

## 🤖 Turno 12 - Respuesta de la IA

He creado el archivo [paralelo_ia_opt.cpp](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/paralelo_ia_opt.cpp) con la misma estructura e implementación base de `paralelo_ia.cpp`, y lo he compilado exitosamente en `paralelo_ia_opt.exe`.

Queda listo para cuando quieras aplicar las optimizaciones manuales específicas (como vectorización SIMD AVX2, desenrollado de bucles, separabilidad 1D horizontal/vertical o tiling de caché).

---

## 👤 Turno 13 - Solicitud del Usuario (Prompt)

bien, puedes volver a compilar y correr las pruebas con 30 ejecuciones por configuración

---

## 🤖 Turno 13 - Respuesta de la IA

He recompilado [paralelo_ia_opt.cpp](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/paralelo_ia_opt.cpp) con `g++ -O3` y lanzado la prueba completa de **30 ejecuciones por configuración** (2.400 ejecuciones en total).

Los resultados consolidados se guardarán automáticamente en `resultados_paralelo_ia_opt.csv`. Te notificaré tan pronto finalicen las mediciones.

---

## 🤖 Turno 14 - Respuesta de la IA

El benchmark está en las últimas configuraciones (77/80). Finalizará en unos instantes.

---

## 🤖 Turno 15 - Respuesta de la IA

El benchmark avanza en la configuración 78/80. Esperando a que concluyan las últimas ejecuciones.

---

## 🤖 Turno 16 - Respuesta de la IA

El benchmark completo de **30 repeticiones por configuración** para **Paralelo IA + Opt.** ha concluido con éxito (2.400 ejecuciones).

Los resultados completos quedaron guardados en [resultados_paralelo_ia_opt.csv](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/resultados_paralelo_ia_opt.csv) con el formato configurado (`sep=;`, separadores `;` y coma decimal `,`).

---

### 📈 Resumen comparativo de muestra (Imagen 4096 × 4096, Kernel 11 × 11):

| Hilos ($p$) | Tiempo Medio ($T_p$) | Desv. Estándar ($\sigma$) | Speedup ($S_p$) | Eficiencia ($E_p$) |
| :---: | :---: | :---: | :---: | :---: |
| **1 hilo** | 1975,49 ms | $\pm 10,72\text{ ms}$ | 1,00x | 100,00% |
| **2 hilos** | 1017,74 ms | $\pm 15,84\text{ ms}$ | 1,94x | 97,05% |
| **4 hilos** | 555,77 ms | $\pm 17,78\text{ ms}$ | 3,55x | 88,86% |
| **8 hilos** | 345,54 ms | $\pm 11,27\text{ ms}$ | 5,72x | 71,46% |
| **16 hilos** | 265,96 ms | $\pm 7,78\text{ ms}$ | **7,43x** | 46,42% |

---

Actualmente tienes listos y con sus benchmarks de 30 corridas:
1. `secuencial.cpp` $\rightarrow$ [resultados_secuencial.csv](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/resultados_secuencial.csv)
2. `paralelo_ia.cpp` $\rightarrow$ [resultados_paralelo_ia.csv](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/resultados_paralelo_ia.csv)
3. `paralelo_ia_opt.cpp` $\rightarrow$ [resultados_paralelo_ia_opt.csv](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/resultados_paralelo_ia_opt.csv)

¿Deseas que preparemos el archivo base para la variante final: **C++ Paralelo Manual** (`paralelo_manual.cpp`)?

---

## 👤 Turno 17 - Solicitud del Usuario (Prompt)

sí, dejame listo el archivo base, que sea muy parecido al paralelo ia pero que tenga vacías las partes relacionadas las funciones de convolution worker, convolution_2d_parallel y en el main solo dejame lo del uso y la lectura de los 3 parametros

---

## 🤖 Turno 17 - Respuesta de la IA

He creado el archivo base [paralelo_manual.cpp](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/paralelo_manual.cpp) listo para que lo desarrolles a mano:

- Incluye los headers necesarios y la función `get_sobel_kernel()`.
- Deja vacías las funciones `convolution_worker` y `convolution_2d_parallel`.
- En `main` contiene únicamente la validación del uso y la lectura de los 3 parámetros (`img_size`, `k_size`, `num_threads`).
- Compila correctamente con `g++ -O3`.

---

## 👤 Turno 18 - Solicitud del Usuario (Prompt)

ya completé la codificación de la versión manual, podrías hacer la prueba con las 30 repeticiones por configuración?

---

## 🤖 Turno 18 - Respuesta de la IA

He sincronizado y compilado [paralelo_manual.cpp](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/paralelo_manual.cpp) con `g++ -O3`.

He iniciado la prueba completa de **30 ejecuciones por configuración** (80 configuraciones $\times$ 30 ejecuciones = 2.400 mediciones en total).

Los resultados se guardarán automáticamente en `resultados_paralelo_manual.csv`. Te notificaré en cuanto termine la ejecución.

---

## 🤖 Turno 19 - Respuesta de la IA

El benchmark va avanzando por la configuración 58/80. Continuamos esperando a que finalicen las pruebas.

---

## 🤖 Turno 20 - Respuesta de la IA

El benchmark va por la configuración 70/80. En breve concluyen las pruebas restantes.

---

## 🤖 Turno 21 - Respuesta de la IA

El benchmark está procesando la configuración 78/80. Quedan solo las últimas configuraciones.

---

## 🤖 Turno 22 - Respuesta de la IA

El benchmark está procesando la configuración 79/80. Finalizará en breves momentos.

---

## 🤖 Turno 23 - Respuesta de la IA

El benchmark completo de **30 repeticiones por configuración** para la variante **C++ Paralelo Manual** ha finalizado con éxito (2.400 mediciones en total).

Los resultados detallados quedaron guardados en [resultados_paralelo_manual.csv](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/resultados_paralelo_manual.csv) (y sincronizados en tu carpeta de Git en el Escritorio) con formato `sep=;`, separador `;` y coma decimal `,`.

---

### 📊 Resumen de Resultados: Paralelo Manual (30 corridas)

#### 1. Imagen 1024 × 1024 (Kernel 11 × 11)
| Hilos ($p$) | Tiempo Medio ($T_p$) | Desv. Estándar ($\sigma$) | Speedup ($S_p$) | Eficiencia ($E_p$) |
| :---: | :---: | :---: | :---: | :---: |
| **1 hilo** | 101,29 ms | $\pm 3,90\text{ ms}$ | 1,00x | 100,00% |
| **2 hilos** | 52,15 ms | $\pm 1,68\text{ ms}$ | 1,94x | 97,13% |
| **4 hilos** | 29,41 ms | $\pm 3,74\text{ ms}$ | 3,44x | 86,09% |
| **8 hilos** | 18,90 ms | $\pm 2,43\text{ ms}$ | 5,36x | 67,00% |
| **16 hilos** | 13,54 ms | $\pm 1,46\text{ ms}$ | **7,48x** | 46,77% |

#### 2. Imagen 4096 × 4096 (Kernel 11 × 11)
| Hilos ($p$) | Tiempo Medio ($T_p$) | Desv. Estándar ($\sigma$) | Speedup ($S_p$) | Eficiencia ($E_p$) |
| :---: | :---: | :---: | :---: | :---: |
| **1 hilo** | 1629,89 ms | $\pm 19,82\text{ ms}$ | 1,00x | 100,00% |
| **2 hilos** | 851,17 ms | $\pm 24,58\text{ ms}$ | 1,91x | 95,74% |
| **4 hilos** | 487,22 ms | $\pm 26,90\text{ ms}$ | 3,35x | 83,63% |
| **8 hilos** | 304,27 ms | $\pm 15,99\text{ ms}$ | 5,36x | 66,96% |
| **16 hilos** | 203,69 ms | $\pm 13,18\text{ ms}$ | **8,00x** | 50,01% |

---

### 📂 Estado de las 4 Variantes del Proyecto:
| Variante | Archivo C++ | Archivo CSV de Resultados (30 runs) |
| :--- | :--- | :--- |
| **Secuencial** | `secuencial.cpp` | [resultados_secuencial.csv](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/resultados_secuencial.csv) |
| **Paralelo IA** | `paralelo_ia.cpp` | [resultados_paralelo_ia.csv](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/resultados_paralelo_ia.csv) |
| **Paralelo IA + Opt.** | `paralelo_ia_opt.cpp` | [resultados_paralelo_ia_opt.csv](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/resultados_paralelo_ia_opt.csv) |
| **Paralelo Manual** | `paralelo_manual.cpp` | [resultados_paralelo_manual.csv](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/resultados_paralelo_manual.csv) |

¿Deseas que generemos un script de Python con `matplotlib` / `seaborn` o una consolidación para graficar y comparar las 4 variantes en el formato del artículo?

---

## 👤 Turno 24 - Solicitud del Usuario (Prompt)

Ahora quiero que hagas un readme explicando como compilar cada código y ejecutar las pruebas

---

## 🤖 Turno 24 - Respuesta de la IA

He creado el archivo [README.md](file:///c:/Users/juanp/Documents/Semestre%20VII/Paralela/codigo/proyecto_parcial2/README.md) (y lo he sincronizado también en tu repositorio Git en el Escritorio).

### 📖 Contenido incluido en el `README.md`:
1. **Descripción General:** Objetivo del proyecto y comparación de las 4 variantes.
2. **Estructura de Archivos:** Detalle del rol de cada código `.cpp` y del evaluador automatizado.
3. **Instrucciones de Compilación:** Comandos completos con la bandera `-O3` para el tester y los 4 programas.
4. **Ejecución Individual:** Parámetros de entrada por línea de comandos (`<tamano_imagen> <tamano_kernel> <num_hilos>`) y ejemplos de prueba manual.
5. **Ejecución Automatizada (`tester.exe`):** Comandos listos para correr el protocolo de **30 repeticiones por configuración** para cada variante.
6. **Métricas y Formato CSV:** Explicación de las métricas calculadas ($T_p, \sigma, S_p, E_p$) y el formato regional con punto y coma y comas decimales para Excel.

---

## 👤 Turno 25 - Solicitud del Usuario (Prompt)

ultima pregunta, como exporto esta conversación para el informe que voy a realizar?

---

