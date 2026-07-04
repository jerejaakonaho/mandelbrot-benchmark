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
    int width{2560*2};
    int height{1440*2};

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

        __float128 xLowerBound{-2.0};
        __float128 xUpperBound{0.47};

        __float128 yLowerBound{-1.12};
        __float128 yUpperBound{1.12};

        std::vector<size_t> rows;
        rows.resize(height);
        for (size_t i{0}; i < height; ++i) {
            rows[i] = i;
        }

        std::for_each(std::execution::par, rows.begin(), rows.end(), [&](size_t i){
            for (size_t j{}; j < width; ++j) {
                // The coordinate is calculated with: i * width + j
                int index = i * width + j;

                // Scale the pixel to the Mandelbrot coordinates

                __float128 cx{xLowerBound + static_cast<__float128>(j) / width * (xUpperBound - xLowerBound)};
                __float128 cy{yLowerBound + static_cast<__float128>(i)/ height * (yUpperBound - yLowerBound)};

                // Mandelbrot: Z_n+1 = Z²_n + C, where Z starts at 0.
                __float128 zx{};
                __float128 zy{};

                int numOfRounds{};
                int maxNumOfRounds{4000};
                while (zx * zx + zy * zy <= 4.0 && numOfRounds < maxNumOfRounds) {
                    __float128 temp_zx = zx * zx - zy * zy + cx;
                    zy = 2 * zx * zy + cy;
                    zx = temp_zx;
                    numOfRounds++;
                }

                // color the pixel using smooth shading

                //  v = n + 1 - ln(ln(sqrt(zx²+zy²))/ln(2)
                double v = numOfRounds + 1.0 - std::logl(std::logl(std::sqrtl(static_cast<double>(zx * zx + zy * zy)))) / std::log(2.0);


                // Scale the value to the range [0, 1]
                double funNumber = 0.8;
                int colorValue = static_cast<int>(255.0 * (0.5 * std::sin(funNumber * v) + 0.5));

                uint32_t smoothColor = (0xFF << 24) | (0x00 << 16) | (0x00 << 8) | colorValue;

                if (numOfRounds == maxNumOfRounds) pixelArray[index] = 0xFF000000;
                if (numOfRounds < maxNumOfRounds) pixelArray[index] = smoothColor;
            }
        });
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    }
};

#endif // MANDELBROTCPU_HPP