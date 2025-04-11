#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include <vector>
#include <string>
#include <omp.h>

const double G = 6.674e-11;
const double SOFTENING = 1e-1;

struct simulation {
  size_t nbpart;
  
  std::vector<double> mass;
  std::vector<double> x, y, z;
  std::vector<double> vx, vy, vz;
  std::vector<double> fx, fy, fz;

  simulation(size_t nb)
    : nbpart(nb), mass(nb),
      x(nb), y(nb), z(nb),
      vx(nb), vy(nb), vz(nb),
      fx(nb), fy(nb), fz(nb) {}
};

void random_init(simulation& s) {
  std::random_device rd;  
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dismass(0.9, 1.);
  std::normal_distribution<> dispos(0., 1.);
  std::normal_distribution<> disvel(0., 1.);

  for (size_t i = 0; i < s.nbpart; ++i) {
    s.mass[i] = dismass(gen);
    s.x[i] = dispos(gen);
    s.y[i] = dispos(gen);
    s.z[i] = 0.;
    s.vx[i] = s.y[i] * 1.5;
    s.vy[i] = -s.x[i] * 1.5;
    s.vz[i] = 0.;
  }

  double meanmass = 0, meanmassvx = 0, meanmassvy = 0, meanmassvz = 0;
  for (size_t i = 0; i < s.nbpart; ++i) {
    meanmass += s.mass[i];
    meanmassvx += s.mass[i] * s.vx[i];
    meanmassvy += s.mass[i] * s.vy[i];
    meanmassvz += s.mass[i] * s.vz[i];
  }
  for (size_t i = 0; i < s.nbpart; ++i) {
    s.vx[i] -= meanmassvx / meanmass;
    s.vy[i] -= meanmassvy / meanmass;
    s.vz[i] -= meanmassvz / meanmass;
  }
}

void init_solar(simulation& s) {
  enum Planets {SUN, MERCURY, VENUS, EARTH, MARS, JUPITER, SATURN, URANUS, NEPTUNE, MOON};
  s = simulation(10);

  double AU = 1.496e11;
  s.mass = {1.9891e30, 3.285e23, 4.867e24, 5.972e24, 6.39e23, 1.898e27, 5.683e26, 8.681e25, 1.024e26, 7.342e22};
  s.x = {0, 0.39*AU, 0.72*AU, 1.0*AU, 1.52*AU, 5.20*AU, 9.58*AU, 19.22*AU, 30.05*AU, 1.0*AU + 3.844e8};
  s.y = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  s.z = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  s.vx = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  s.vy = {0, 47870, 35020, 29780, 24130, 13070, 9680, 6800, 5430, 29780 + 1022};
  s.vz = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

void reset_forces(simulation& s) {
#pragma omp parallel for
  for (size_t i = 0; i < s.nbpart; ++i) {
    s.fx[i] = 0.;
    s.fy[i] = 0.;
    s.fz[i] = 0.;
  }
}

void compute_forces(simulation& s) {
#pragma omp parallel
  {
    std::vector<double> fx_local(s.nbpart, 0.);
    std::vector<double> fy_local(s.nbpart, 0.);
    std::vector<double> fz_local(s.nbpart, 0.);

#pragma omp for schedule(dynamic)
    for (size_t i = 0; i < s.nbpart; ++i) {
      for (size_t j = 0; j < s.nbpart; ++j) {
        if (i != j) {
          double dx = s.x[j] - s.x[i];
          double dy = s.y[j] - s.y[i];
          double dz = s.z[j] - s.z[i];
          double distSqr = dx * dx + dy * dy + dz * dz + SOFTENING;
          double invDist = 1.0 / std::sqrt(distSqr);
          double force = G * s.mass[i] * s.mass[j] * invDist * invDist;

          fx_local[i] += force * dx * invDist;
          fy_local[i] += force * dy * invDist;
          fz_local[i] += force * dz * invDist;
        }
      }
    }

#pragma omp critical
    for (size_t i = 0; i < s.nbpart; ++i) {
      s.fx[i] += fx_local[i];
      s.fy[i] += fy_local[i];
      s.fz[i] += fz_local[i];
    }
  }
}

void integrate(simulation& s, double dt) {
#pragma omp parallel for
  for (size_t i = 0; i < s.nbpart; ++i) {
    s.vx[i] += (s.fx[i] / s.mass[i]) * dt;
    s.vy[i] += (s.fy[i] / s.mass[i]) * dt;
    s.vz[i] += (s.fz[i] / s.mass[i]) * dt;
    s.x[i] += s.vx[i] * dt;
    s.y[i] += s.vy[i] * dt;
    s.z[i] += s.vz[i] * dt;
  }
}

void dump_state(const simulation& s) {
  std::cout << s.nbpart;
  for (size_t i = 0; i < s.nbpart; ++i)
    std::cout << '\t' << s.mass[i]
              << '\t' << s.x[i] << '\t' << s.y[i] << '\t' << s.z[i]
              << '\t' << s.vx[i] << '\t' << s.vy[i] << '\t' << s.vz[i]
              << '\t' << s.fx[i] << '\t' << s.fy[i] << '\t' << s.fz[i];
  std::cout << '\n';
}

void load_from_file(simulation& s, const std::string& filename) {
  std::ifstream in(filename);
  size_t nbpart;
  in >> nbpart;
  s = simulation(nbpart);
  for (size_t i = 0; i < s.nbpart; ++i)
    in >> s.mass[i] >> s.x[i] >> s.y[i] >> s.z[i] >> s.vx[i] >> s.vy[i] >> s.vz[i] >> s.fx[i] >> s.fy[i] >> s.fz[i];
  if (!in.good()) throw std::runtime_error("Error loading file");
}

int main(int argc, char* argv[]) {
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0] << " <input> <dt> <nbstep> <printevery>\n";
    return -1;
  }

  std::string input(argv[1]);
  double dt = std::atof(argv[2]);
  size_t nbstep = std::atol(argv[3]);
  size_t printevery = std::atol(argv[4]);

  simulation s(1);

  try {
    size_t n = std::stoul(input);
    s = simulation(n);
    random_init(s);
  } catch (...) {
    if (input == "planet")
      init_solar(s);
    else
      load_from_file(s, input);
  }

  for (size_t step = 0; step < nbstep; ++step) {
    if (step % printevery == 0)
      dump_state(s);

    reset_forces(s);
    compute_forces(s);
    integrate(s, dt);
  }
}
