#include "include/Simulation.hpp"

int main()
{
    Simulation simulation;
    simulation.run_simulation("input/overrides.txt",
                              "input/environment.txt",
                              "output/simulation_log.txt");
    return 0;
}
