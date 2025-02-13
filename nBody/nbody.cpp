#include "nbody.h"

NBodySimulation::NBodySimulation(int num, double dt) : numParticles(num), timeStep(dt) {
    particles.reserve(numParticles);
    initializeRandom();
}

// random initialize particles plus ew math
void NBodySimulation::initializeRandom() {
    for (int i = 0; i < numParticles; i++) {
        double mass = randomDouble(1e20, 1e25);
        double x = randomDouble(-1e11, 1e11);
        double y = randomDouble(-1e11, 1e11);
        double z = randomDouble(-1e11, 1e11);
        double vx = randomDouble(-1e3, 1e3);
        double vy = randomDouble(-1e3, 1e3);
        double vz = randomDouble(-1e3, 1e3);
        particles.emplace_back(mass, x, y, z, vx, vy, vz);
    }
}

// Compute gravitational forces plus ew maths
void NBodySimulation::computeForces() {
    for (auto& p : particles) {
        p.fx = p.fy = p.fz = 0;
    }

    for (size_t i = 0; i < particles.size(); i++) {
        for (size_t j = i + 1; j < particles.size(); j++) {
            double dx = particles[j].x - particles[i].x;
            double dy = particles[j].y - particles[i].y;
            double dz = particles[j].z - particles[i].z;
            double distSq = dx * dx + dy * dy + dz * dz + SOFTENING;
            double invDist = 1.0 / sqrt(distSq);
            double invDistCube = invDist * invDist * invDist;
            double force = G * particles[i].mass * particles[j].mass * invDistCube;

            double fx = force * dx;
            double fy = force * dy;
            double fz = force * dz;

            particles[i].fx += fx;
            particles[i].fy += fy;
            particles[i].fz += fz;
            particles[j].fx -= fx;
            particles[j].fy -= fy;
            particles[j].fz -= fz;
        }
    }
}

// Updating positions and velo
void NBodySimulation::updateParticles() {
    for (auto& p : particles) {
        double invMass = 1.0 / p.mass;
        double ax = p.fx * invMass;
        double ay = p.fy * invMass;
        double az = p.fz * invMass;

        p.vx += ax * timeStep;
        p.vy += ay * timeStep;
        p.vz += az * timeStep;

        p.x += p.vx * timeStep;
        p.y += p.vy * timeStep;
        p.z += p.vz * timeStep;
    }
}

// Main loopy
void NBodySimulation::runSimulation(int steps, int outputInterval) {
    for (int i = 0; i < steps; i++) {
        computeForces();
        updateParticles();
        if (i % outputInterval == 0) {
            outputState("output.log");
        }
    }
}

// Output
void NBodySimulation::outputState(const std::string& filename) {
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile) {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    outFile << numParticles; //DOUBLECHEEKS
    for (const auto& p : particles) {
        outFile << "\t" << p.mass << "\t" << p.x << "\t" << p.y << "\t" << p.z;
        outFile << "\t" << p.vx << "\t" << p.vy << "\t" << p.vz;
        outFile << "\t" << p.fx << "\t" << p.fy << "\t" << p.fz;
    }
    outFile << "\n";
}
