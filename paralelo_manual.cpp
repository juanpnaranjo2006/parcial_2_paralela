#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

// Generador o selector de kernel tipo Sobel (horizontal/X) para 3x3, 5x5, 7x7 y
// 11x11
std::vector<std::vector<float>> get_sobel_kernel(size_t k_size) {
  std::vector<std::vector<float>> kernel(k_size,
                                         std::vector<float>(k_size, 0.0f));

  if (k_size == 3) {
    const float k3[3][3] = {
        {-1.0f, 0.0f, 1.0f}, {-2.0f, 0.0f, 2.0f}, {-1.0f, 0.0f, 1.0f}};
    for (int r = 0; r < 3; ++r) {
      for (int c = 0; c < 3; ++c) {
        kernel[r][c] = k3[r][c];
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
        kernel[r][c] = k5[r][c];
      }
    }
  } else if (k_size == 7) {
    const float pascal7[7] = {1, 6, 15, 20, 15, 6, 1};
    const float diff7[7] = {-3, -2, -1, 0, 1, 2, 3};
    for (int r = 0; r < 7; ++r) {
      for (int c = 0; c < 7; ++c) {
        kernel[r][c] = pascal7[r] * diff7[c];
      }
    }
  } else if (k_size == 11) {
    const float pascal11[11] = {1, 10, 45, 120, 210, 252, 210, 120, 45, 10, 1};
    const float diff11[11] = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
    for (int r = 0; r < 11; ++r) {
      for (int c = 0; c < 11; ++c) {
        kernel[r][c] = pascal11[r] * diff11[c];
      }
    }
  }
  return kernel;
}

// Función trabajadora para cada hilo
void convolution_worker(size_t start_row, size_t end_row,
                        const std::vector<std::vector<float>> &input,
                        std::vector<std::vector<float>> &output, size_t width,
                        size_t height,
                        const std::vector<std::vector<float>> &kernel,
                        size_t k_size) {
  int k_half = static_cast<int>(k_size / 2);
  int w = static_cast<int>(width);
  int h = static_cast<int>(height);

  for (size_t y = start_row; y < end_row; ++y) {
    for (int x = 0; x < w; ++x) {
      float sum = 0.0f;
      for (int ky = -k_half; ky <= k_half; ++ky) {
        int img_y = static_cast<int>(y) + ky;
        // Tratamiento de bordes: Clamp
        if (img_y < 0) {
          img_y = 0;
        } else if (img_y >= h) {
          img_y = h - 1;
        }

        for (int kx = -k_half; kx <= k_half; ++kx) {
          int img_x = x + kx;
          if (img_x < 0) {
            img_x = 0;
          } else if (img_x >= w) {
            img_x = w - 1;
          }

          sum += input[img_y][img_x] * kernel[ky + k_half][kx + k_half];
        }
      }
      output[y][x] = sum;
    }
  }
}

// Convolución 2D Paralela Manual
void convolution_2d_parallel(const std::vector<std::vector<float>> &input,
                             std::vector<std::vector<float>> &output,
                             size_t width, size_t height,
                             const std::vector<std::vector<float>> &kernel,
                             size_t k_size, size_t num_threads) {

  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  size_t rows_per_thread = height / num_threads;
  size_t remaining_rows = height % num_threads;

  size_t current_row = 0;
  for (size_t t = 0; t < num_threads; ++t) {
    size_t start_row = current_row;
    size_t end_row = start_row + rows_per_thread + (t < remaining_rows ? 1 : 0);
    current_row = end_row;

    threads.emplace_back(convolution_worker, start_row, end_row,
                         std::cref(input), std::ref(output), width, height,
                         std::cref(kernel), k_size);
  }

  for (auto &th : threads) {
    if (th.joinable()) {
      th.join();
    }
  }
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

  // Inicialización de imagen sintética y memoria
  std::vector<std::vector<float>> input(img_size,
                                        std::vector<float>(img_size, 0.0f));
  std::vector<std::vector<float>> output(img_size,
                                         std::vector<float>(img_size, 0.0f));

  for (size_t r = 0; r < img_size; ++r) {
    for (size_t c = 0; c < img_size; ++c) {
      input[r][c] = static_cast<float>(((r * img_size + c) % 256) / 255.0f);
    }
  }

  std::vector<std::vector<float>> kernel = get_sobel_kernel(k_size);

  auto start = std::chrono::high_resolution_clock::now();
  convolution_2d_parallel(input, output, img_size, img_size, kernel, k_size,
                          num_threads);
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double, std::milli> elapsed_ms = end - start;
  std::cout << "TIME_MS:" << elapsed_ms.count() << "\n";

  return 0;
}
