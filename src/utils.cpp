#include "../inc/utils.h"


double* get_integration_bounds(integration_setup& intgr_setup, program_setup& prg_setup) {
    double region_for_thread = fabs(intgr_setup.x[1] - intgr_setup.x[0]) / prg_setup.threads;
    double delta_x = fabs(intgr_setup.x[1] - intgr_setup.x[0]) / sqrt(prg_setup.steps);

    /* don't forget to free a dynamically allocated variable */
    auto* bounds = new double[prg_setup.threads + 1];
    bounds[0] = intgr_setup.x[0];

    double x = intgr_setup.x[0];
    double bound = intgr_setup.x[0] + region_for_thread;
    for (int th = 1; th <= prg_setup.threads; th++) {
        while (x < bound) {
            x += delta_x;
        }
        bounds[th] = x;
        bound += region_for_thread;
    }
    return bounds;
}
