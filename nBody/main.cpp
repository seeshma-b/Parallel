#include "nbody.h"
#include <iostream>
#include <cstdlib>

//loop kinda
int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usages: " << argv[0] << " <num_particles> <time_step> <iterations> <output_interval>\n";
        return 1;
    }

//more stds lol
    int numParticles = std::stoi(argv[1]);
    double timeStep = std::stod(argv[2]);
    int steps = std::stoi(argv[3]);
    int outputInterval = std::stoi(argv[4]);

    NBodySimulation simulation(numParticles, timeStep);
    simulation.runSimulation(steps, outputInterval);

    return 0;
}
