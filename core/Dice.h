//
// Created by SyperOlao on 10.12.2025.
//

#ifndef DICE_DICE_H
#define DICE_DICE_H
#include <string>
#include <vector>
#include <random>

class Dice {
public:
    struct Term {
        int count = 0;
        int sides = 0;
        int modifier = 0;
        bool isFlat = false;
    };

    struct Spec {
        std::vector<Term> terms;
    };

    Dice();
    explicit Dice(uint32_t seed);

    int roll(const std::string& param);
    int roll(const Spec& spec);

    static Spec parse(const std::string& param);
    static std::string toString(const Spec& spec);


    struct Dist {
        std::vector<long long> ways;
        int offset = 0;
        long long totalWays() const;
    };

    static Dist distributionNdS(int n, int s);
    static Dist distribution(const Spec& spec);

private:
    std::mt19937 rng_;

    static Dist convolve(const Dist& a, const Dist& b);
    static Dist shift(const Dist& d, int delta);
    int rollUniform1to(int sides);
};


#endif //DICE_DICE_H
