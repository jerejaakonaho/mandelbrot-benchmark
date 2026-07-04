#ifndef MANDELBROTCPU_HPP
#define MANDELBROTCPU_HPP

#include <vector>
#include <algorithm>
#include <execution>
#include <chrono>
#include <iostream>
#include <cmath>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


class MandelbrotGenCPU {
private:
    // Contains the fractal data
    std::vector<uint32_t> pixelArray;
    int width{2560};
    int height{1440};

public:
    void saveToPNG() {
        stbi_write_png("mandelbrot.png", width, height, 4, pixelArray.data(), width * 4);
    }

    // Generates a Mandelbrot fractal using the CPU
    void generateMandelbrot() {
        // start a timer for benchmarking
        auto start = std::chrono::high_resolution_clock::now();

        pixelArray.resize(width*height);

        // Mandelbrot range

        double xLowerBound{-2.0};
        double xUpperBound{0.47};

        double yLowerBound{-1.12};
        double yUpperBound{1.12};

        std::vector<size_t> rows;
        rows.resize(height);
        std::iota(rows.begin(), rows.end(), 0);

        std::for_each(std::execution::par, rows.begin(), rows.end(), [&](size_t i){
            for (size_t j{}; j < width; ++j) {
                // The coordinate is calculated with: i * width + j
                int index = i * width + j;

                // Scale the pixel to the Mandelbrot coordinates

                double cx{xLowerBound + static_cast<double>(j) / width * (xUpperBound - xLowerBound)};
                double cy{yLowerBound + static_cast<double>(i)/ height * (yUpperBound - yLowerBound)};

                // Mandelbrot: Z_n+1 = Z²_n + C, where Z starts at 0.
                double zx{};
                double zy{};

                int numOfRounds{};
                int maxNumOfRounds{4000};

                // Pre-calculate zx*zx and zy*zy for performance
                double zx2 = zx * zx;
                double zy2 = zy * zy;
                
                while (zx2 + zy2 <= 4.0 && numOfRounds < maxNumOfRounds) {
                    double temp_zx = zx2 - zy2 + cx;
                    zy = 2 * zx * zy + cy;
                    zx = temp_zx;
                    numOfRounds++;
                    zx2 = zx * zx;
                    zy2 = zy * zy;
                }

                if (numOfRounds == maxNumOfRounds) pixelArray[index] = 0xFF000000;
                if (numOfRounds < maxNumOfRounds) {
                    // Smooth shading if not black
                    // v = n + 1 - ln(ln(sqrt(zx²+zy²))/ln(2)
                    double v = numOfRounds + 1.0 - std::logl(std::logl(std::sqrtl(static_cast<double>(zx * zx + zy * zy)))) / std::log(2.0);
                    double funNumber = 0.8;
                    int colorValue = static_cast<int>(255.0 * (0.5 * std::sin(funNumber * v) + 0.5));

                    uint32_t smoothColor = (0xFF << 24) | (0x00 << 16) | (0x00 << 8) | colorValue;
                    pixelArray[index] = smoothColor;
                }
            }
        });
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    }
};

#endif // MANDELBROTCPU_HPP