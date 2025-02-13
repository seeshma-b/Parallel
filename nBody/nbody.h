#ifndef NBODY_H
#define NBODY_H

#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>
#include "utils.h"

//no namespace!!(?)
//doublecheck loop

const double G = 6.674e-11; //maths
const double SOFTENING = 1e-9;

struct Particle {
    double mass;
    double x, y, z;      // Position
    double vx, vy, vz;   // Velo
    double fx, fy, fz;   // Force

    Particle(double m, double px, double py, double pz, double vx_, double vy_, double vz_)
        : mass(m), x(px), y(py), z(pz), vx(vx_), vy(vy_), vz(vz_), fx(0), fy(0), fz(0) {}
};

// Simulations 
class NBodySimulation {
private:
    std::vector<Particle> particles;
    double timeStep;
    int numParticles;

public:
    NBodySimulation(int num, double dt);
    void initializeRandom();  // random particles
    void computeForces();     // Calculate gravitational forces
    void updateParticles();   // Update velo and position
    void runSimulation(int steps, int outputInterval);  // loopy
    void outputState(const std::string& filename); // output

};

#endif // NBODY_H
