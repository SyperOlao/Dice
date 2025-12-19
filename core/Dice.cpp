//
// Created by SyperOlao on 10.12.2025.
//

#include "Dice.h"
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <algorithm>


Dice::Dice() {
    std::random_device rd;
    rng_ = std::mt19937(rd());
}

Dice::Dice(uint32_t seed) : rng_(seed) {}

int Dice::rollUniform1to(int sides) {
    if (sides <= 0) throw std::invalid_argument("Dice sides must be > 0");
    std::uniform_int_distribution<int> dist(1, sides);
    return dist(rng_);
}

int Dice::roll(const std::string& param) {
    return roll(parse(param));
}

int Dice::roll(const Spec& spec) {
    long long sum = 0;
    for (const auto& t : spec.terms) {
        if (t.isFlat) {
            sum += t.modifier;
            continue;
        }
        if (t.count <= 0 || t.sides <= 0)
            throw std::invalid_argument("Bad dice term: count/sides must be > 0");
        for (int i = 0; i < t.count; ++i) sum += rollUniform1to(t.sides);
        sum += t.modifier;
    }

    if (sum > std::numeric_limits<int>::max() || sum < std::numeric_limits<int>::min())
        throw std::overflow_error("Roll result overflowed int");
    return static_cast<int>(sum);
}


static std::string stripSpaces(std::string s) {
    s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char c){ return std::isspace(c); }), s.end());
    return s;
}

static bool isDigit(char c) { return c >= '0' && c <= '9'; }

static int parseUInt(const std::string& s, size_t& i) {
    if (i >= s.size() || !isDigit(s[i])) throw std::invalid_argument("Expected unsigned int");
    long long v = 0;
    while (i < s.size() && isDigit(s[i])) {
        v = v * 10 + (s[i] - '0');
        if (v > std::numeric_limits<int>::max()) throw std::overflow_error("Number too big");
        ++i;
    }
    return static_cast<int>(v);
}

static int parseSignedInt(const std::string& s, size_t& i) {
    if (i >= s.size() || (s[i] != '+' && s[i] != '-'))
        throw std::invalid_argument("Expected signed int (+/-N)");
    int sign = (s[i] == '-') ? -1 : 1;
    ++i;
    int v = parseUInt(s, i);
    return sign * v;
}

Dice::Spec Dice::parse(const std::string& param) {
    std::string s = stripSpaces(param);
    if (s.empty()) throw std::invalid_argument("Empty dice spec");

    Spec spec;
    size_t i = 0;

    auto parseTerm = [&]() -> Term {
        Term t;

        if (s[i] == '+' || s[i] == '-') {
            t.isFlat = true;
            t.modifier = parseSignedInt(s, i);
            return t;
        }


        size_t start = i;
        int n = 0;
        bool hasN = false;

        if (i < s.size() && isDigit(s[i])) {
            hasN = true;
            n = parseUInt(s, i);
        }

        if (i >= s.size() || (s[i] != 'd' && s[i] != 'D')) {
            throw std::invalid_argument("Expected 'd' in dice term near: " + s.substr(start));
        }
        ++i;

        int sides = parseUInt(s, i);
        if (!hasN) n = 1;

        t.count = n;
        t.sides = sides;

        if (i < s.size() && (s[i] == '+' || s[i] == '-')) {
            t.modifier = parseSignedInt(s, i);
        }
        return t;
    };

    while (i < s.size()) {
        Term t = parseTerm();
        spec.terms.push_back(t);

        if (i == s.size()) break;
        if (s[i] != ',') throw std::invalid_argument("Expected ',' at position " + std::to_string(i));
        ++i;
        if (i == s.size()) throw std::invalid_argument("Trailing comma in dice spec");
    }

    return spec;
}

std::string Dice::toString(const Spec& spec) {
    std::ostringstream out;
    for (size_t idx = 0; idx < spec.terms.size(); ++idx) {
        const auto& t = spec.terms[idx];
        if (idx) out << ",";
        if (t.isFlat) {
            if (t.modifier >= 0) out << "+" << t.modifier;
            else out << t.modifier;
        } else {
            out << t.count << "d" << t.sides;
            if (t.modifier > 0) out << "+" << t.modifier;
            else if (t.modifier < 0) out << t.modifier;
        }
    }
    return out.str();
}


long long Dice::Dist::totalWays() const {
    long long sum = 0;
    for (auto w : ways) sum += w;
    return sum;
}

Dice::Dist Dice::distributionNdS(int n, int s) {
    if (n <= 0 || s <= 0) throw std::invalid_argument("n and s must be > 0");

    Dist d;
    d.offset = 0;
    d.ways = {1};

    Dist die;
    die.offset = 1;
    die.ways.assign(s, 1);

    for (int i = 0; i < n; ++i) d = convolve(d, die);
    return d;
}

Dice::Dist Dice::shift(const Dist& d, int delta) {
    Dist r = d;
    r.offset += delta;
    return r;
}

Dice::Dist Dice::convolve(const Dist& a, const Dist& b) {
    Dist r;
    r.offset = a.offset + b.offset;
    r.ways.assign(a.ways.size() + b.ways.size() - 1, 0);

    for (size_t i = 0; i < a.ways.size(); ++i) {
        if (a.ways[i] == 0) continue;
        for (size_t j = 0; j < b.ways.size(); ++j) {
            if (b.ways[j] == 0) continue;
            r.ways[i + j] += a.ways[i] * b.ways[j];
        }
    }
    return r;
}

Dice::Dist Dice::distribution(const Spec& spec) {
    Dist total;
    total.offset = 0;
    total.ways = {1};

    for (const auto& t : spec.terms) {
        Dist part;
        if (t.isFlat) {
            part.offset = t.modifier;
            part.ways = {1};
        } else {
            part = distributionNdS(t.count, t.sides);
            if (t.modifier != 0) part = shift(part, t.modifier);
        }
        total = convolve(total, part);
    }
    return total;
}