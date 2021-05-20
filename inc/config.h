#ifndef CALCULATE_INTEGRAL_MULTIPLE_THREADS_PROCESS_CONFIG_H
#define CALCULATE_INTEGRAL_MULTIPLE_THREADS_PROCESS_CONFIG_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <cassert>
#include "errors.h"

struct program_setup {
    int threads;
    long max_iter_num, steps;
    double rel_precision, abs_precision;
};

struct integration_setup {
    int m;
    int* a1, * a2, * c;
    double x[2], y[2];

    ~integration_setup() {
        free(a1);
        free(a2);
        free(c);
    }
};

int read_config(std::string& filename, std::unordered_map<std::string, std::string>& config_data);
int extract_program_args(std::unordered_map<std::string, std::string>& config_data, program_setup& setup);
int extract_integration_setup(std::unordered_map<std::string, std::string>& config_data, integration_setup& setup);
int split_str(std::string s, const std::string& delim, int m, int* arr);

#endif //CALCULATE_INTEGRAL_MULTIPLE_THREADS_PROCESS_CONFIG_H
