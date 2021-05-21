#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <iostream>
#include <unordered_map>

#include <chrono>
#include <atomic>

#include "inc/config.h"
#include "inc/utils.h"


#define M_PI 3.14159265358979323846 
#define CUDA_CALL(x) { cudaError_t cuda_error__ = (x); if (cuda_error__) printf("CUDA Error: " #x " returned \"%s\"\n", cudaGetErrorString(cuda_error__)); }


__device__ double func_to_integrate(int* m, int* a1, int* a2, int* c, double* x, double* y) {
    double result = 0;
    for (int  i = 0; i < *m; i++) {
        double multiplier = pow(*x - a1[i], 2) + pow(*y - a2[i], 2);
        result += c[i] * exp(-1 / M_PI * multiplier) * cos(M_PI * multiplier);
    }
    return -result;
}


__global__ void integrateKernel(double* intgr_bounds, double* delta, int* m, int* a1, int* a2, int* c, double* res) {
    // get the boundaries for the x and y axis
    double x_start = intgr_bounds[threadIdx.x], x_end = intgr_bounds[threadIdx.x + 1], x = x_start;
    double y_start = intgr_bounds[blockIdx.x], y_end = intgr_bounds[blockIdx.x + 1], y = y_start;

    // integrate
    double temp_res = 0;
    while (x < x_end) {
        while (y < y_end) {
            temp_res += func_to_integrate(m, a1, a2, c, &x, &y);
            y += (*delta);
        }
        y = y_start;
        x += (*delta);
    }
    res[(blockIdx.x) * blockDim.x + (threadIdx.x)] = temp_res * (*delta) * (*delta);
}


int main() {
    std::string config = "config.txt";
    std::unordered_map<std::string, std::string> config_data;

    // Extract program setup
    int res = read_config(config, config_data);
    if (res) { return res; }

    program_setup prg_setup{};
    res = extract_program_args(config_data, prg_setup);
    if (res) { return res; }

    integration_setup intgr_setup{};
    res = extract_integration_setup(config_data, intgr_setup);
    if (res) { return res; }

    // Get block integration bounds
    double* integration_bounds = get_integration_bounds(intgr_setup, prg_setup);
    double delta = fabs(intgr_setup.x[1] - intgr_setup.x[0]) / sqrt(prg_setup.steps);

    // Allocate space for device copies of
    double* d_delta;
    CUDA_CALL(cudaMalloc(&d_delta, sizeof(double)));
    CUDA_CALL(cudaMemcpy(d_delta, &delta, sizeof(double), cudaMemcpyHostToDevice));

    double* d_intgr_bounds;
    CUDA_CALL(cudaMalloc(&d_intgr_bounds, (prg_setup.threads + 1) * sizeof(double)));
    CUDA_CALL(cudaMemcpy(d_intgr_bounds, integration_bounds, (prg_setup.threads + 1) * sizeof(double), cudaMemcpyHostToDevice));

    int* d_m;
    CUDA_CALL(cudaMalloc(&d_m, sizeof(int)));
    CUDA_CALL(cudaMemcpy(d_m, &intgr_setup.m, sizeof(int), cudaMemcpyHostToDevice));

    int* d_a1;
    CUDA_CALL(cudaMalloc(&d_a1, intgr_setup.m * sizeof(int)));
    CUDA_CALL(cudaMemcpy(d_a1, intgr_setup.a1, intgr_setup.m * sizeof(int), cudaMemcpyHostToDevice));

    int* d_a2;
    CUDA_CALL(cudaMalloc(&d_a2, intgr_setup.m * sizeof(int)));
    CUDA_CALL(cudaMemcpy(d_a2, intgr_setup.a2, intgr_setup.m * sizeof(int), cudaMemcpyHostToDevice));

    int* d_c;
    CUDA_CALL(cudaMalloc(&d_c, intgr_setup.m * sizeof(int)));
    CUDA_CALL(cudaMemcpy(d_c, intgr_setup.c, intgr_setup.m * sizeof(int), cudaMemcpyHostToDevice));

    double* d_res;
    CUDA_CALL(cudaMalloc(&d_res, prg_setup.threads * prg_setup.threads * sizeof(double)));

    // Call kernel
    integrateKernel <<<prg_setup.threads, prg_setup.threads>>>(d_intgr_bounds, d_delta, d_m, d_a1, d_a2, d_c, d_res);

    // Copy the result back to the host
    double* results = new double[prg_setup.threads * prg_setup.threads];
    cudaMemcpy(results, d_res, prg_setup.threads * prg_setup.threads * sizeof(double), cudaMemcpyDeviceToHost);

    // Iterate over every partial result (Bad approach) TODO: Rewrite this chunk of code regarding better aproaches
    double result = 0;
    for (int i = 0; i < (prg_setup.threads * prg_setup.threads); i++) {
        result += results[i];
    }

    std::cout << "Result: " << result << std::endl;

    // Free device memory
    cudaFree(d_delta); cudaFree(d_intgr_bounds), cudaFree(d_m);
    cudaFree(d_a1); cudaFree(d_a2), cudaFree(d_c); cudaFree(d_res);

    // Free host memory
    free(results);

    return 0;
}