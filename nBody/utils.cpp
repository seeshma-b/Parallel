#include "utils.h"

// random number gen to std or not to std frfr
std::random_device rd;
std::mt19937 gen(rd());

double randomDouble(double min, double max) {
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}
