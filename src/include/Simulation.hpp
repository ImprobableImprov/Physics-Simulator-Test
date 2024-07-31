#include "SolarPanel.hpp"

class Simulation
{
private:
    Environment environment_;
    SolarPanel solar_panel_;
    CylinderContainer pipe_into_panel_,
                      pipe_into_tank_,
                      pipe_on_panel_ = CylinderContainer(/*is_tank*/ false,
                                                         /*is_exposed*/ false,
                                                         /*length_m*/ 4.0,
                                                         /*diameter_m*/ 0.04),
                      tank_ = CylinderContainer(/*is_tank*/ true,
                                                /*is_exposed*/ true,
                                                /*length_m*/ 2.0,
                                                /*diameter_m*/ 1.0);
    unsigned long duration_s_;
    unsigned int time_step_s_;
    unsigned int current_time_s_;

    static constexpr int FIRST_WIDTH = 10;
    static constexpr int SHORT_WIDTH = 15;
    static constexpr int MEDIUM_WIDTH = 20;
    static constexpr int LONG_WIDTH = 25;
    static constexpr int PRECISION = 2;

public:
    Simulation() : duration_s_(3600),
                   time_step_s_(60.0),
                   current_time_s_(0.0) {}

    void print_headers(std::ofstream &output_file);
    void print_data_line(std::ofstream &output_file);
    void read_simulation_constants(const std::string &filename);
    void run_simulation(const std::string &sim_overrides_file,
                        const std::string &environmental_file,
                        const std::string &output_filename);
};
