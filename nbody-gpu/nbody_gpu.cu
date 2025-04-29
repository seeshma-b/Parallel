#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <cstdlib>

#define G 6.674e-11
#define SOFTENING 1e-1

__global__ void compute_forces(int n, double *x, double *y, double *z, double *mass, double *fx, double *fy, double *fz) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= n) return;

    double fx_i = 0, fy_i = 0, fz_i = 0;
    for (int j = 0; j < n; ++j) {
        if (i == j) continue;
        double dx = x[j] - x[i];
        double dy = y[j] - y[i];
        double dz = z[j] - z[i];
        double distSqr = dx*dx + dy*dy + dz*dz + SOFTENING;
        double invDist = rsqrt(distSqr);
        double invDist3 = invDist * invDist * invDist;
        double F = G * mass[i] * mass[j] * invDist3;
        fx_i += dx * F;
        fy_i += dy * F;
        fz_i += dz * F;
    }
    fx[i] = fx_i;
    fy[i] = fy_i;
    fz[i] = fz_i;
}

__global__ void update_positions(int n, double dt, double *x, double *y, double *z, double *vx, double *vy, double *vz, double *fx, double *fy, double *fz, double *mass) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= n) return;
    vx[i] += fx[i] / mass[i] * dt;
    vy[i] += fy[i] / mass[i] * dt;
    vz[i] += fz[i] / mass[i] * dt;
    x[i] += vx[i] * dt;
    y[i] += vy[i] * dt;
    z[i] += vz[i] * dt;
}

void random_init(int n, std::vector<double>& x, std::vector<double>& y, std::vector<double>& z,
                 std::vector<double>& vx, std::vector<double>& vy, std::vector<double>& vz,
                 std::vector<double>& mass) {
    std::mt19937 gen(42);
    std::uniform_real_distribution<double> pos(-1.0, 1.0);
    std::uniform_real_distribution<double> vel(-0.1, 0.1);
    std::uniform_real_distribution<double> m(0.9, 1.1);
    for (int i = 0; i < n; ++i) {
        x[i] = pos(gen); y[i] = pos(gen); z[i] = pos(gen);
        vx[i] = vel(gen); vy[i] = vel(gen); vz[i] = vel(gen);
        mass[i] = m(gen);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " <num_particles> <dt> <steps> <print_interval> <block_size>\n";
        return 1;
    }

    int n = atoi(argv[1]);
    double dt = atof(argv[2]);
    int steps = atoi(argv[3]);
    int print_interval = atoi(argv[4]);
    int blockSize = atoi(argv[5]);

    std::vector<double> x(n), y(n), z(n);
    std::vector<double> vx(n), vy(n), vz(n);
    std::vector<double> fx(n), fy(n), fz(n);
    std::vector<double> mass(n);

    random_init(n, x, y, z, vx, vy, vz, mass);

    double *d_x, *d_y, *d_z, *d_vx, *d_vy, *d_vz, *d_fx, *d_fy, *d_fz, *d_mass;
    size_t bytes = n * sizeof(double);

    cudaMalloc(&d_x, bytes); cudaMalloc(&d_y, bytes); cudaMalloc(&d_z, bytes);
    cudaMalloc(&d_vx, bytes); cudaMalloc(&d_vy, bytes); cudaMalloc(&d_vz, bytes);
    cudaMalloc(&d_fx, bytes); cudaMalloc(&d_fy, bytes); cudaMalloc(&d_fz, bytes);
    cudaMalloc(&d_mass, bytes);

    cudaMemcpy(d_x, x.data(), bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_y, y.data(), bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_z, z.data(), bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_vx, vx.data(), bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_vy, vy.data(), bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_vz, vz.data(), bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_mass, mass.data(), bytes, cudaMemcpyHostToDevice);

    int gridSize = (n + blockSize - 1) / blockSize;

    for (int step = 0; step < steps; ++step) {
        compute_forces<<<gridSize, blockSize>>>(n, d_x, d_y, d_z, d_mass, d_fx, d_fy, d_fz);
        update_positions<<<gridSize, blockSize>>>(n, dt, d_x, d_y, d_z, d_vx, d_vy, d_vz, d_fx, d_fy, d_fz, d_mass);
        if (step % print_interval == 0) {
            cudaMemcpy(x.data(), d_x, bytes, cudaMemcpyDeviceToHost);
            std::cout << n << '\t';
            for (int i = 0; i < n; ++i) std::cout << x[i] << '\t';
            std::cout << '\n';
        }
    }

    cudaFree(d_x); cudaFree(d_y); cudaFree(d_z);
    cudaFree(d_vx); cudaFree(d_vy); cudaFree(d_vz);
    cudaFree(d_fx); cudaFree(d_fy); cudaFree(d_fz);
    cudaFree(d_mass);

    return 0;
}
