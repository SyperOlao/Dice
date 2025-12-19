#include <iostream>
#include "core/Dice.h"
#include <iomanip>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>


static void printDist(const std::string& expr, int barWidth = 60) {
    auto spec = Dice::parse(expr);
    auto dist = Dice::distribution(spec);

    long long total = dist.totalWays();
    long long maxWays = 0;
    for (auto w : dist.ways) maxWays = std::max(maxWays, w);

    long double mean = 0.0L;
    long double m2 = 0.0L;
    for (size_t i = 0; i < dist.ways.size(); ++i) {
        int sum = dist.offset + static_cast<int>(i);
        long double p = (long double)dist.ways[i] / (long double)total;
        mean += p * (long double)sum;
        m2   += p * (long double)sum * (long double)sum;
    }
    long double var = m2 - mean * mean;

    std::cout << "\n=============================\n";
    std::cout << "Distribution for: " << expr << "\n";
    std::cout << "Total outcomes: " << total << "\n";
    std::cout << "Mean: " << (double)mean << " | Var: " << (double)var
              << " | StdDev: " << (double)std::sqrt((double)var) << "\n";
    std::cout << "-----------------------------\n";

    // header
    std::cout << "sum | ways       | prob        | bar\n";
    std::cout << "----+------------+-------------+----------------------------------------------\n";

    for (size_t i = 0; i < dist.ways.size(); ++i) {
        int sum = dist.offset + static_cast<int>(i);
        long long ways = dist.ways[i];
        long double p = (long double)ways / (long double)total;

        int len = (maxWays == 0) ? 0 : (int)std::llround((long double)ways * barWidth / (long double)maxWays);
        if (len < 0) len = 0;
        if (len > barWidth) len = barWidth;

        std::cout << std::setw(3) << sum << " | "
                  << std::setw(10) << ways << " | "
                  << std::setw(11) << std::fixed << std::setprecision(8) << (double)p << " | ";

        for (int k = 0; k < len; ++k) std::cout << '#';
        std::cout << "\n";
    }
}

int main() {
    std::vector<std::string> tests = {
        "1d6","2d6","3d6",
        "1d10","2d10","3d10"
    };

    for (auto& t : tests) printDist(t, 60);

    Dice d(123);
    std::cout << "\nroll(\"2d6+2,3d10\") = " << d.roll("2d6+2,3d10") << "\n";
}