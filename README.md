# AI-Generated C++ Parallel Code Benchmarking

Este proyecto tiene como objetivo evaluar y cuantificar el impacto de la paralelización en C++ (`std::thread`) aplicada al procesamiento de imágenes mediante convolución 2D con filtros Sobel ($3\times3$, $5\times5$, $7\times7$ y $11\times11$), comparando implementaciones secuenciales, generadas por IA, optimizadas y desarrolladas manualmente.

---

## 📁 Estructura del Proyecto

- **`tester.cpp`**: Orquestador automático del benchmark. Ejecuta de forma exhaustiva las combinaciones de tamaños de imagen, kernels e hilos con 30 repeticiones por configuración y exporta las métricas consolidadas en formato CSV compatible con Excel.
- **`secuencial.cpp`**: Implementación base de referencia en 1 hilo.
- **`paralelo_ia.cpp`**: Implementación paralela generada directamente por IA utilizando descomposición de dominio 1D por bloques de filas.
- **`paralelo_ia_opt.cpp`**: Implementación paralela con ajustes y optimizaciones adicionales.
- **`paralelo_manual.cpp`**: Implementación paralela desarrollada manualmente sin asistencia de IA.

---

## ⚙️ Requisitos Previos

- Compilador de C++ con soporte para C++11 o superior (`g++` / MinGW-w64 o `clang++`).
- Soporte para hilos POSIX / Win32 (`<thread>`).

---

## 🔨 Compilación

Se recomienda compilar todos los programas con la bandera de optimización `-O3` para garantizar el máximo rendimiento del compilador:

### 1. Compilar el Evaluador (Tester)
```bash
g++ -O3 tester.cpp -o tester.exe
```

### 2. Compilar las 4 Variantes
```bash
# Secuencial (Base de referencia)
g++ -O3 secuencial.cpp -o secuencial.exe

# Paralelo IA
g++ -O3 paralelo_ia.cpp -o paralelo_ia.exe

# Paralelo IA + Opt.
g++ -O3 paralelo_ia_opt.cpp -o paralelo_ia_opt.exe

# Paralelo Manual
g++ -O3 paralelo_manual.cpp -o paralelo_manual.exe
```

---

## 🚀 Ejecución Individual (Pruebas manuales)

Cada ejecutable acepta los siguientes parámetros por línea de comandos:

```bash
./<nombre_ejecutable> <tamano_imagen> <tamano_kernel> <num_hilos>
```

### Parámetros válidos:
- **`tamano_imagen`**: `512`, `1024`, `2048`, `4096`
- **`tamano_kernel`**: `3`, `5`, `7`, `11`
- **`num_hilos`**: `1`, `2`, `4`, `8`, `16` *(ignorado en la versión secuencial)*

### Ejemplos:
```bash
# Probar versión secuencial en imagen 1024x1024 con kernel 5x5
./secuencial.exe 1024 5 1

# Probar versión paralela IA en imagen 2048x2048 con kernel 7x7 en 8 hilos
./paralelo_ia.exe 2048 7 8

# Probar versión paralela manual en imagen 4096x4096 con kernel 11x11 en 16 hilos
./paralelo_manual.exe 4096 11 16
```

*Salida esperada:* Imprime en consola `TIME_MS:<tiempo_en_milisegundos>` correspondiente a la medición exclusiva del procesamiento convolucional.

---

## 📊 Ejecución Automatizada del Benchmark (`tester.exe`)

El programa `tester.exe` automatiza las corridas para todas las combinaciones posibles:
- **4 Tamaños de Imagen**: $512\times512$, $1024\times1024$, $2048\times2048$, $4096\times4096$ px.
- **4 Tamaños de Kernel Sobel**: $3\times3$, $5\times5$, $7\times7$, $11\times11$.
- **5 Niveles de Escalabilidad de Hilos**: $1, 2, 4, 8, 16$ hilos *(1 hilo para secuencial)*.
- **Protocolo de Evaluación**: **30 repeticiones por configuración** (para estabilidad estadística).

### Sintaxis del Tester:
```bash
./tester.exe <ruta_ejecutable> <Nombre_Variante> <archivo_salida.csv> [repeticiones]
```

### Comandos de Benchmark Completo (30 Repeticiones):

```bash
# 1. Benchmark Variante Secuencial
./tester.exe ./secuencial.exe Secuencial resultados_secuencial.csv 30

# 2. Benchmark Variante Paralelo IA
./tester.exe ./paralelo_ia.exe Paralelo_IA resultados_paralelo_ia.csv 30

# 3. Benchmark Variante Paralelo IA + Opt.
./tester.exe ./paralelo_ia_opt.exe Paralelo_IA_Opt resultados_paralelo_ia_opt.csv 30

# 4. Benchmark Variante Paralelo Manual
./tester.exe ./paralelo_manual.exe Paralelo_Manual resultados_paralelo_manual.csv 30
```

---

## 📈 Formato de Salida y Métricas

Los archivos `.csv` generados incluyen la cabecera `sep=;`, separadores por punto y coma (`;`) y coma decimal (`,`) para importación inmediata en Microsoft Excel (configuración regional en español).

### Columnas calculadas en cada CSV:
1. **`Variante`**: Nombre de la variante evaluada.
2. **`Tamano_Imagen`**: Dimensión $N \times N$ de la imagen de entrada.
3. **`Tamano_Kernel`**: Dimensión $K \times K$ del filtro Sobel aplicado.
4. **`Hilos`**: Cantidad de hilos ($p$) ejecutados en paralelo.
5. **`Repeticiones`**: Cantidad de corridas realizadas por configuración ($n = 30$).
6. **`Tiempo_Medio_ms`**: Tiempo promedio exclusivo de procesamiento ($T_p$ en ms).
7. **`Desv_Estandar_ms`**: Desviación estándar muestral ($\sigma$ en ms).
8. **`Tiempo_Min_ms`**: Tiempo mínimo registrado entre las repeticiones.
9. **`Tiempo_Max_ms`**: Tiempo máximo registrado entre las repeticiones.
10. **`Speedup`**: Factor de aceleración $S_p = \frac{T_1}{T_p}$.
11. **`Eficiencia`**: Eficiencia paralela $E_p = \frac{S_p}{p}$.
