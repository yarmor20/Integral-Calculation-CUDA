#include "../inc/config.h"


int read_config(std::string& filename, std::unordered_map<std::string, std::string>& config_data) {
    // TODO: finish read config
}


int extract_program_args(std::unordered_map<std::string, std::string>& config_data, program_setup& setup) {
    try {
        setup.max_iter_num = std::stol(config_data.at("max_iter_num "));
        assert(setup.max_iter_num > 0 && setup.max_iter_num <= 500'000'000 &&
            "<max_iter_num> should be in range (0, 500'000'000]");

        setup.steps = std::stol(config_data.at("steps "));
        assert(setup.steps > 0 && setup.steps <= setup.max_iter_num &&
            "<steps> should be in range (0, max_iter_num]");

        setup.threads = std::stoi(config_data.at("threads "));
        assert(setup.threads > 0 && "<threads> should be in range (0, 32 (recommended) or more)");

        setup.rel_precision = std::stod(config_data.at("rel_precision "));
        assert(setup.rel_precision <= 0.1 && "<rel_precision> should be > 0.1");

        setup.abs_precision = std::stod(config_data.at("abs_precision "));
        assert(setup.abs_precision <= 1.0 && "<abs_precision> should be > 1");

    }
    catch (std::out_of_range& e) {
        std::cout << "Missing a program setup argument in the config!\n"
            "Check for <max_iter_num>, <steps>, <threads>, <rel_precision>, <abs_precision>!" << std::endl;
        return NO_SUCH_CFG_ARG;
    }
    catch (std::invalid_argument& e) {
        std::cout << "Invalid argument! All config arguments should be numbers!" << std::endl;
        return INVALID_ARG;
    }
    return OK;
}


int extract_integration_setup(std::unordered_map<std::string, std::string>& config_data, integration_setup& setup) {
    setup.m = std::stoi(config_data.at("m "));
    assert(setup.m > 0 && "<m> should be > 0");

    setup.x[0] = std::stod(config_data.at("x1 "));
    setup.x[1] = std::stod(config_data.at("x2 "));

    setup.y[0] = std::stod(config_data.at("y1 "));
    setup.y[1] = std::stod(config_data.at("y2 "));

    // allocate memory for function parameters
    int* a1 = (int*)malloc(setup.m * sizeof(int));
    int* a2 = (int*)malloc(setup.m * sizeof(int));
    int* c = (int*)malloc(setup.m * sizeof(int));

    // extract function parameters
    int res = split_str(config_data.at("a1 "), " ", setup.m, a1);
    if (res) { return res; }

    res = split_str(config_data.at("a2 "), " ", setup.m, a2);
    if (res) { return res; }

    res = split_str(config_data.at("c "), " ", setup.m, c);
    if (res) { return res; }

    // add dunction parameters to integration setup
    setup.a1 = a1;
    setup.a2 = a2;
    setup.c = c;

    return OK;
}


inline int split_str(std::string s, const std::string& delim, int m, int* arr) {
    try {
        for (size_t i = 0; i < m; i++) {
            int end = s.find(delim);
            std::string token = s.substr(0, end);
            // check if it is a number
            for (char ch : token) {
                if (!std::isdigit(ch)) {
                    std::cout << "Wrong config array declaration! Try this format: <val> <val> ... <val>" << std::endl;
                    return INVALID_ARG;
                }
            }
            arr[i] = std::stoi(token);
            s = s.substr(end + delim.length());
        }
    }
    catch (std::invalid_argument& e) {
        std::cout << "Wrong config array declaration! Try this format: <val> <val> ... <val>" << std::endl;
        return INVALID_ARG;
    }
    return OK;
}