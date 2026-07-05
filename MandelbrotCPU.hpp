#ifndef MANDELBROTCPU_HPP
#define MANDELBROTCPU_HPP

#include <vector>
#include <algorithm>
#include <execution>
#include <chrono>
#include <iostream>
#include <cmath>
#include <QObject>
#include <thread>

#include "stb_image_write.h"


class MandelbrotGenCPU : public QObject {
    Q_OBJECT
private:
    // Contains the fractal data
    std::vector<uint32_t> pixelArray;
    int width{2560};
    int height{1440};

    double xLowerBound{-2.0};
    double xUpperBound{0.47};

    double yLowerBound{-1.12};
    double yUpperBound{1.12};

    int maxNumOfRounds{4000};

public:
    MandelbrotGenCPU() = default;

    MandelbrotGenCPU(double xLowerBound, double xUpperBound,
                     double yLowerBound, double yUpperBound,
                     int maxNumOfRounds,
                     int width, int height)
    :   xLowerBound(xLowerBound),
        xUpperBound(xUpperBound),
        yLowerBound(yLowerBound),
        yUpperBound(yUpperBound),
        maxNumOfRounds(maxNumOfRounds),
        width(width),
        height(height)
    {}

    void saveToPNG() {
        stbi_write_png("mandelbrot.png", width, height, 4, pixelArray.data(), width * 4);
    }

    void colorPixel(int numOfRounds, int index, int x, int y) {
        if (numOfRounds == maxNumOfRounds) pixelArray[index] = 0xFF000000;
        if (numOfRounds < maxNumOfRounds) {

            // Smooth shading if not black
            // v = n + 1 - ln(ln(sqrt(x²+y²))/ln(2)
            double v = numOfRounds + 1.0 - std::log(std::log(std::sqrt(static_cast<double>(x * x + y * y)))) / std::log(2.0);
            double funNumber = 0.2;
            //int colorValue = static_cast<int>(255.0 * (0.5 * std::sin(funNumber * v) + 0.5));

            uint32_t r = static_cast<int>(255.0 * (0.5 * std::sin(funNumber * v + 4.0) + 0.5));
            uint32_t g = static_cast<int>(255.0 * (0.5 * std::sin(funNumber * v + 2.0) + 0.5));
            uint32_t b = static_cast<int>(255.0 * (0.5 * std::sin(funNumber * v + 4.0) + 0.5));


            uint32_t smoothColor = (0xFF << 24) | (r << 16) | (g << 8) | b;
            pixelArray[index] = smoothColor;
        }
    }

    // Generates a Mandelbrot fractal using the CPU
    void generateMandelbrot() {
        // start a timer for benchmarking
        auto start = std::chrono::high_resolution_clock::now();

        pixelArray.resize(width*height);

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

                // Pre-calculate zx*zx and zy*zy for performance
                double zx2 = zx * zx;
                double zy2 = zy * zy;

                int numOfRounds{};
                
                while (zx2 + zy2 <= 100000 && numOfRounds < maxNumOfRounds) {
                    double temp_zx = zx2 - zy2 + cx;
                    zy = 2 * zx * zy + cy;
                    zx = temp_zx;
                    numOfRounds++;
                    zx2 = zx * zx;
                    zy2 = zy * zy;
                }

                colorPixel(numOfRounds, index, zx, zy);
            }
        });
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
        saveToPNG();
    }

    Q_INVOKABLE void startBenchmark() {
        std::thread([this]() {
            generateMandelbrot();
            emit imageReady();
        }).detach();
    }

signals:
    void imageReady();
};

#endif // MANDELBROTCPU_HPP