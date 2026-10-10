#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

// Generador o selector de kernel tipo Sobel (horizontal/X) para 3x3, 5x5, 7x7 y 11x11
std::vector<float> get_sobel_kernel(size_t k_size) {
  std::vector<float> kernel(k_size * k_size, 0.0f);
  int half = static_cast<int>(k_size / 2);

  if (k_size == 3) {
    const float k3[3][3] = {
        {-1.0f, 0.0f, 1.0f}, {-2.0f, 0.0f, 2.0f}, {-1.0f, 0.0f, 1.0f}};
    for (int r = 0; r < 3; ++r) {
      for (int c = 0; c < 3; ++c) {
        kernel[r * 3 + c] = k3[r][c];
      }
    }
  } else if (k_size == 5) {
    const float k5[5][5] = {{-1.0f, -2.0f, 0.0f, 2.0f, 1.0f},
                            {-4.0f, -8.0f, 0.0f, 8.0f, 4.0f},
                            {-6.0f, -12.0f, 0.0f, 12.0f, 6.0f},
                            {-4.0f, -8.0f, 0.0f, 8.0f, 4.0f},
                            {-1.0f, -2.0f, 0.0f, 2.0f, 1.0f}};
    for (int r = 0; r < 5; ++r) {
      for (int c = 0; c < 5; ++c) {
        kernel[r * 5 + c] = k5[r][c];
      }
    }
  } else if (k_size == 7) {
    const float pascal7[7] = {1, 6, 15, 20, 15, 6, 1};
    const float diff7[7] = {-3, -2, -1, 0, 1, 2, 3};
    for (int r = 0; r < 7; ++r) {
      for (int c = 0; c < 7; ++c) {
        kernel[r * 7 + c] = pascal7[r] * diff7[c];
      }
    }
  } else if (k_size == 11) {
    const float pascal11[11] = {1, 10, 45, 120, 210, 252, 210, 120, 45, 10, 1};
    const float diff11[11]   = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
    for (int r = 0; r < 11; ++r) {
      for (int c = 0; c < 11; ++c) {
        kernel[r * 11 + c] = pascal11[r] * diff11[c];
      }
    }
  } else {
    for (int r = 0; r < static_cast<int>(k_size); ++r) {
      for (int c = 0; c < static_cast<int>(k_size); ++c) {
        kernel[r * k_size + c] = static_cast<float>(c - half);
      }
    }
  }
  return kernel;
}

// Función trabajadora para cada hilo (a implementar manualmente)
void convolution_worker(size_t start_row, size_t end_row, const float *input,
                        float *output, size_t width, size_t height,
                        const float *kernel, size_t k_size) {
  // TODO: Implementar lógica de convolución por hilo
}

// Convolución 2D Paralela Manual (a implementar manualmente)
void convolution_2d_parallel(const std::vector<float> &input,
                             std::vector<float> &output, size_t width,
                             size_t height, const std::vector<float> &kernel,
                             size_t k_size, size_t num_threads) {
  // TODO: Implementar lanzamiento y sincronización de hilos
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cerr << "Uso: " << argv[0]
              << " <image_size> <kernel_size> <num_threads>\n";
    return 1;
  }

  size_t img_size = std::stoul(argv[1]);
  size_t k_size = std::stoul(argv[2]);
  size_t num_threads = std::stoul(argv[3]);

  // TODO: Inicializar memoria, ejecutar medición con std::chrono e imprimir TIME_MS:<tiempo>

  return 0;
}
