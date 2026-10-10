#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>

// Generador o selector de kernel tipo Sobel (horizontal/X) para 3x3, 5x5, 7x7 y 11x11
std::vector<float> get_sobel_kernel(size_t k_size) {
    std::vector<float> kernel(k_size * k_size, 0.0f);
    int half = static_cast<int>(k_size / 2);

    if (k_size == 3) {
        // Sobel 3x3 horizontal
        const float k3[3][3] = {
            {-1.0f, 0.0f, 1.0f},
            {-2.0f, 0.0f, 2.0f},
            {-1.0f, 0.0f, 1.0f}
        };
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                kernel[r * 3 + c] = k3[r][c];
            }
        }
    } else if (k_size == 5) {
        // Sobel extendido 5x5
        const float k5[5][5] = {
            {-1.0f,  -2.0f, 0.0f,  2.0f, 1.0f},
            {-4.0f,  -8.0f, 0.0f,  8.0f, 4.0f},
            {-6.0f, -12.0f, 0.0f, 12.0f, 6.0f},
            {-4.0f,  -8.0f, 0.0f,  8.0f, 4.0f},
            {-1.0f,  -2.0f, 0.0f,  2.0f, 1.0f}
        };
        for (int r = 0; r < 5; ++r) {
            for (int c = 0; c < 5; ++c) {
                kernel[r * 5 + c] = k5[r][c];
            }
        }
    } else if (k_size == 7) {
        // Sobel extendido 7x7 (Pesos binomiales en Y x Gradiente en X)
        const float pascal7[7] = {1, 6, 15, 20, 15, 6, 1};
        const float diff7[7]   = {-3, -2, -1, 0, 1, 2, 3};
        for (int r = 0; r < 7; ++r) {
            for (int c = 0; c < 7; ++c) {
                kernel[r * 7 + c] = pascal7[r] * diff7[c];
            }
        }
    } else if (k_size == 11) {
        // Sobel extendido 11x11 (Pesos binomiales en Y x Gradiente en X)
        const float pascal11[11] = {1, 10, 45, 120, 210, 252, 210, 120, 45, 10, 1};
        const float diff11[11]   = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
        for (int r = 0; r < 11; ++r) {
            for (int c = 0; c < 11; ++c) {
                kernel[r * 11 + c] = pascal11[r] * diff11[c];
            }
        }
    } else {
        // Filtro genérico por si se pasa otro tamaño impar
        for (int r = 0; r < static_cast<int>(k_size); ++r) {
            for (int c = 0; c < static_cast<int>(k_size); ++c) {
                kernel[r * k_size + c] = static_cast<float>(c - half);
            }
        }
    }
    return kernel;
}

// Convolución 2D Secuencial (medición exclusiva del procesamiento)
void convolution_2d_sequential(
    const std::vector<float>& input,
    std::vector<float>& output,
    size_t width,
    size_t height,
    const std::vector<float>& kernel,
    size_t k_size
) {
    int k_half = static_cast<int>(k_size / 2);
    int w = static_cast<int>(width);
    int h = static_cast<int>(height);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float sum = 0.0f;
            for (int ky = -k_half; ky <= k_half; ++ky) {
                int img_y = y + ky;
                // Tratamiento de bordes: Clamp (fijar al borde)
                if (img_y < 0) img_y = 0;
                else if (img_y >= h) img_y = h - 1;

                int k_row = (ky + k_half) * static_cast<int>(k_size);
                int img_row = img_y * w;

                for (int kx = -k_half; kx <= k_half; ++kx) {
                    int img_x = x + kx;
                    if (img_x < 0) img_x = 0;
                    else if (img_x >= w) img_x = w - 1;

                    float pixel = input[img_row + img_x];
                    float weight = kernel[k_row + (kx + k_half)];
                    sum += pixel * weight;
                }
            }
            output[y * w + x] = sum;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <image_size> <kernel_size> [num_threads]\n";
        return 1;
    }

    size_t img_size = std::stoul(argv[1]);
    size_t k_size = std::stoul(argv[2]);
    // El argumento num_threads se ignora en la versión secuencial
    (void)argc;

    size_t total_pixels = img_size * img_size;

    // Inicialización de imagen sintética y memoria (fuera de la medición de tiempo)
    std::vector<float> input_image(total_pixels);
    std::vector<float> output_image(total_pixels, 0.0f);

    // Llenamos con un patrón determinista
    for (size_t i = 0; i < total_pixels; ++i) {
        input_image[i] = static_cast<float>((i % 256) / 255.0f);
    }

    std::vector<float> kernel = get_sobel_kernel(k_size);

    // -------------------------------------------------------------
    // MEDICIÓN EXCLUSIVA DEL TIEMPO DE PROCESAMIENTO
    // -------------------------------------------------------------
    auto start_time = std::chrono::high_resolution_clock::now();

    convolution_2d_sequential(input_image, output_image, img_size, img_size, kernel, k_size);

    auto end_time = std::chrono::high_resolution_clock::now();
    // -------------------------------------------------------------

    std::chrono::duration<double, std::milli> elapsed_ms = end_time - start_time;

    // Imprimir tiempo en formato estándar para tester.cpp
    std::cout << "TIME_MS:" << elapsed_ms.count() << "\n";

    // Pequeño checksum para evitar que el compilador optimice o elimine cálculos
    volatile float dummy = output_image[0] + output_image[total_pixels / 2] + output_image[total_pixels - 1];
    (void)dummy;

    return 0;
}
