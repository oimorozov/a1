#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <fstream>

struct Circle {
    double x, y, r;
    double r_sq;
};

double run_monte_carlo(const std::vector<Circle>& circles, long long n_points, double min_x, double max_x, double min_y, double max_y) {
    if (min_x >= max_x || min_y >= max_y) return 0.0;

    double bbox_area = (max_x - min_x) * (max_y - min_y);

    static std::mt19937 rng(1500);
    std::uniform_real_distribution<double> distX(min_x, max_x);
    std::uniform_real_distribution<double> distY(min_y, max_y);

    long long hits = 0;
    for (long long i = 0; i < n_points; ++i) {
        double px = distX(rng);
        double py = distY(rng);

        bool inside_all = true;
        for (const auto& c : circles) {
            if ((px - c.x) * (px - c.x) + (py - c.y) * (py - c.y) > c.r_sq) {
                inside_all = false;
                break;
            }
        }
        if (inside_all) hits++;
    }
    return bbox_area * static_cast<double>(hits) / n_points;
}


int main() {

    std::ios::sync_with_stdio(false);  // Отключить синхронизацию между iostream и stdio.
    std::cin.tie(nullptr);        // Отключить синхронизацию между std::cin и std::cout.

    std::vector<Circle> circles(3);
    for (int i = 0; i < 3; ++i) {
        std::cin >> circles[i].x >> circles[i].y >> circles[i].r;
        circles[i].r_sq = circles[i].r * circles[i].r;
    }

    double narrow_min_x = std::max({circles[0].x - circles[0].r, circles[1].x - circles[1].r, circles[2].x - circles[2].r});
    double narrow_max_x = std::min({circles[0].x + circles[0].r, circles[1].x + circles[1].r, circles[2].x + circles[2].r});
    double narrow_min_y = std::max({circles[0].y - circles[0].r, circles[1].y - circles[1].r, circles[2].y - circles[2].r});
    double narrow_max_y = std::min({circles[0].y + circles[1].y, circles[1].y + circles[1].r, circles[2].y + circles[2].r});

    double wide_min_x = std::min({circles[0].x - circles[0].r, circles[1].x - circles[1].r, circles[2].x - circles[2].r});
    double wide_max_x = std::max({circles[0].x + circles[0].r, circles[1].x + circles[1].r, circles[2].x + circles[2].r});
    double wide_min_y = std::min({circles[0].y - circles[0].r, circles[1].y - circles[1].r, circles[2].y - circles[2].r});
    double wide_max_y = std::max({circles[0].y + circles[0].r, circles[1].y + circles[1].r, circles[2].y + circles[2].r});

    std::ofstream data_file("data.csv");
    data_file << "N,Narrow_Area,Wide_Area\n";

    const int num_runs_for_average = 10;

    for (int n = 100; n <= 100000; n += 500) {
        std::cout << "Processing N = " << n << std::endl;
        
        double narrow_area_sum = 0;
        double wide_area_sum = 0;

        for (int i = 0; i < num_runs_for_average; ++i) {
            narrow_area_sum += run_monte_carlo(circles, n, narrow_min_x, narrow_max_x, narrow_min_y, narrow_max_y);
            wide_area_sum += run_monte_carlo(circles, n, wide_min_x, wide_max_x, wide_min_y, wide_max_y);
        }

        data_file << n << "," 
                  << std::fixed << std::setprecision(10) << narrow_area_sum / num_runs_for_average << "," 
                  << std::fixed << std::setprecision(10) << wide_area_sum / num_runs_for_average << "\n";
    }
    
    data_file.close();
    std::cout << "OK" << std::endl;

    return 0;
}