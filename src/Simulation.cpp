
#include <functional>
#include <unordered_set>
#include <unordered_map>

#include "include/Simulation.hpp"

void Simulation::print_headers(std::ofstream &output_file)
{
    output_file << std::setw(FIRST_WIDTH) << "Time (s)"
                << std::setw(SHORT_WIDTH) << "Ambient (°C)"
                << std::setw(SHORT_WIDTH) << "Wind (m/s)"
                << std::setw(MEDIUM_WIDTH) << "Irradiance (W/m^2)"
                << std::setw(SHORT_WIDTH) << "Tank (°C)"
                << std::setw(LONG_WIDTH) << "Water (Tank) (°C)"
                << std::setw(LONG_WIDTH) << "Pipe (To Panel) (°C)"
                << std::setw(LONG_WIDTH) << "Water (To Panel) (°C)"
                << std::setw(LONG_WIDTH) << "Solar Panel (°C)"
                << std::setw(LONG_WIDTH) << "Pipe (Panel) (°C)"
                << std::setw(LONG_WIDTH) << "Water (Panel) (°C)"
                << std::setw(LONG_WIDTH) << "Pipe (To Tank) (°C)"
                << std::setw(LONG_WIDTH) << "Water (To Tank) (°C)"
                << "\n";
}

void Simulation::print_data_line(std::ofstream &output_file)
{
    const double solar_irradiance = environment_.get_solar_irradiance_Wpm2(current_time_s_);
    const double ambient_temperature = environment_.get_ambient_temperature(current_time_s_);
    const double wind_speed = environment_.get_wind_speed(current_time_s_);

    output_file << std::setw(FIRST_WIDTH) << current_time_s_
                << std::setw(SHORT_WIDTH - 1) 
                << std::fixed << std::setprecision(PRECISION) << ambient_temperature
                << std::setw(SHORT_WIDTH)     
                << std::fixed << std::setprecision(PRECISION) << wind_speed
                << std::setw(MEDIUM_WIDTH)    
                << std::fixed << std::setprecision(PRECISION) << solar_irradiance
                << std::setw(SHORT_WIDTH - 1) 
                << std::fixed << std::setprecision(PRECISION) << tank_.get_temperature()
                << std::setw(LONG_WIDTH - 1)  
                << std::fixed << std::setprecision(PRECISION) << tank_.get_water_out_temperature_C()
                << std::setw(LONG_WIDTH - 1)  
                << std::fixed << std::setprecision(PRECISION) << pipe_into_panel_.get_temperature()
                << std::setw(LONG_WIDTH - 1)  
                << std::fixed << std::setprecision(PRECISION) << pipe_into_panel_.get_water_out_temperature_C()
                << std::setw(LONG_WIDTH - 1)  
                << std::fixed << std::setprecision(PRECISION) << solar_panel_.get_temperature()
                << std::setw(LONG_WIDTH - 1)  
                << std::fixed << std::setprecision(PRECISION) << pipe_on_panel_.get_temperature()
                << std::setw(LONG_WIDTH - 1)  
                << std::fixed << std::setprecision(PRECISION) << pipe_on_panel_.get_water_out_temperature_C()
                << std::setw(LONG_WIDTH - 1)  
                << std::fixed << std::setprecision(PRECISION) << pipe_into_tank_.get_temperature()
                << std::setw(LONG_WIDTH - 1)  
                << std::fixed << std::setprecision(PRECISION) << pipe_into_tank_.get_water_out_temperature_C()
                << "\n";
}

// Function to read simulation constants from input file
void Simulation::read_simulation_constants(const std::string &filename)
{
    std::ifstream inputFile(filename);
    if (!inputFile.is_open())
    {
        std::cerr << "Error opening input file: " << filename << std::endl;
        return;
    }

    std::string paramName;
    double paramValue;
    std::unordered_set<std::string> updated_parameters;
    static const std::unordered_map<std::string, std::function<void(double)>> parameterMap = {
        {"SIMULATION_DURATION",	        [this](double value){ duration_s_ = static_cast<unsigned long>(value); }},
        {"SIMULATION_TIME_STEP",	    [this](double value){ time_step_s_ = static_cast<unsigned int>(value); }},

        {"PANEL_WIDTH",	                [this](double value){ solar_panel_.set_wdith(value); }},
        {"PANEL_LENGTH",	            [this](double value){ solar_panel_.set_length(value); }},
        {"PANEL_TEMPERATURE",	        [this](double value){ solar_panel_.set_temperature(value); }},
        {"PANEL_HEAT_CAPACITY",	        [this](double value){ solar_panel_.set_specific_heat_capacity(value); }},
        {"PANEL_IDEAL_EFFICIENCY",	    [this](double value){ solar_panel_.set_ideal_efficiency(value); }},
        {"PANEL_EMISSIVITY",	        [this](double value){ solar_panel_.set_emissivity(value); }},
        {"PANEL_THICKNESS",	            [this](double value){ solar_panel_.set_thickness(value); }},
        {"PANEL_EFFICIENCY_COEFFICIENT",[this](double value){ solar_panel_.set_efficiency_coefficient(value); }},

        {"MASS_FLOW_RATE",              [this](double value){ tank_.set_mass_flow_rate(value);
                                                              pipe_into_tank_.set_mass_flow_rate(value); 
                                                              pipe_into_panel_.set_mass_flow_rate(value); 
                                                              pipe_on_panel_.set_mass_flow_rate(value); }},
        {"WATER_MAX_TEMPERATURE",	    [this](double value){ tank_.set_max_temperature(value);
                                                              pipe_into_tank_.set_max_temperature(value);
                                                              pipe_into_panel_.set_max_temperature(value); 
                                                              pipe_on_panel_.set_max_temperature(value); }},
        {"WATER_MIN_TEMPERATURE",	    [this](double value){ tank_.set_min_temperature(value);
                                                              pipe_into_tank_.set_min_temperature(value);
                                                              pipe_into_panel_.set_min_temperature(value); 
                                                              pipe_on_panel_.set_min_temperature(value); }},

        {"TANK_WALL_TEMPERATURE",	    [this](double value){ tank_.set_temperature(value); }},
        {"TANK_WATER_TEMPERATURE",	    [this](double value){ tank_.set_water_temperature(value); }},
        {"TANK_HEAT_CAPACITY",	        [this](double value){ tank_.set_specific_heat_capacity(value); }},
        {"TANK_EMISSIVITY",	            [this](double value){ tank_.set_emissivity(value); }},
        {"TANK_THICKNESS",	            [this](double value){ tank_.set_thickness(value); }},
        {"TANK_MASS_FLOW_RATE",	        [this](double value){ tank_.set_mass_flow_rate(value); }},
        {"TANK_EXPOSED",	            [this](double value){ tank_.set_exposed(static_cast<bool>(value)); }},
        {"TANK_PIPE_LENGTH",	        [this](double value){ tank_.set_pipe_length(value); }},
        {"TANK_MAX_TEMPERATURE",	    [this](double value){ tank_.set_max_temperature(value); }},
        {"TANK_MIN_TEMPERATURE",	    [this](double value){ tank_.set_min_temperature(value); }},
        {"TANK_INTERIOR_DIAMETER",	    [this](double value){ tank_.set_pipe_interior_diameter(value); }},

        {"PIPE2TANK_WALL_TEMPERATURE",	[this](double value){ pipe_into_tank_.set_temperature(value); }},
        {"PIPE2TANK_WATER_TEMPERATURE",	[this](double value){ pipe_into_tank_.set_water_temperature(value); }},
        {"PIPE2TANK_HEAT_CAPACITY",	    [this](double value){ pipe_into_tank_.set_specific_heat_capacity(value); }},
        {"PIPE2TANK_EMISSIVITY",	    [this](double value){ pipe_into_tank_.set_emissivity(value); }},
        {"PIPE2TANK_THICKNESS",	        [this](double value){ pipe_into_tank_.set_thickness(value); }},
        {"PIPE2TANK_MASS_FLOW_RATE",	[this](double value){ pipe_into_tank_.set_mass_flow_rate(value); }},
        {"PIPE2TANK_EXPOSED",	        [this](double value){ pipe_into_tank_.set_exposed(static_cast<bool>(value)); }},
        {"PIPE2TANK_PIPE_LENGTH",	    [this](double value){ pipe_into_tank_.set_pipe_length(value); }},
        {"PIPE2TANK_MAX_TEMPERATURE",	[this](double value){ pipe_into_tank_.set_max_temperature(value); }},
        {"PIPE2TANK_MIN_TEMPERATURE",	[this](double value){ pipe_into_tank_.set_min_temperature(value); }},
        {"PIPE2TANK_INTERIOR_DIAMETER",	[this](double value){ pipe_into_tank_.set_pipe_interior_diameter(value); }},

        {"PIPE2PANEL_WALL_TEMPERATURE",	[this](double value){ pipe_into_panel_.set_temperature(value); }},
        {"PIPE2PANEL_WATER_TEMPERATURE",[this](double value){ pipe_into_panel_.set_water_temperature(value); }},
        {"PIPE2PANEL_HEAT_CAPACITY",	[this](double value){ pipe_into_panel_.set_specific_heat_capacity(value); }},
        {"PIPE2PANEL_EMISSIVITY",	    [this](double value){ pipe_into_panel_.set_emissivity(value); }},
        {"PIPE2PANEL_THICKNESS",	    [this](double value){ pipe_into_panel_.set_thickness(value); }},
        {"PIPE2PANEL_MASS_FLOW_RATE",	[this](double value){ pipe_into_panel_.set_mass_flow_rate(value); }},
        {"PIPE2PANEL_EXPOSED",	        [this](double value){ pipe_into_panel_.set_exposed(static_cast<bool>(value)); }},
        {"PIPE2PANEL_PIPE_LENGTH",	    [this](double value){ pipe_into_panel_.set_pipe_length(value); }},
        {"PIPE2PANEL_MAX_TEMPERATURE",	[this](double value){ pipe_into_panel_.set_max_temperature(value); }},
        {"PIPE2PANEL_MIN_TEMPERATURE",	[this](double value){ pipe_into_panel_.set_min_temperature(value); }},
        {"PIPE2PANEL_INTERIOR_DIAMETER",[this](double value){ pipe_into_panel_.set_pipe_interior_diameter(value); }},

        {"PIPE_PANEL_WALL_TEMPERATURE",	[this](double value){ pipe_on_panel_.set_temperature(value); }},
        {"PIPE_PANEL_WATER_TEMPERATURE",[this](double value){ pipe_on_panel_.set_water_temperature(value); }},
        {"PIPE_PANEL_HEAT_CAPACITY",	[this](double value){ pipe_on_panel_.set_specific_heat_capacity(value); }},
        {"PIPE_PANEL_EMISSIVITY",	    [this](double value){ pipe_on_panel_.set_emissivity(value); }},
        {"PIPE_PANEL_THICKNESS",	    [this](double value){ pipe_on_panel_.set_thickness(value); }},
        {"PIPE_PANEL_MASS_FLOW_RATE",	[this](double value){ pipe_on_panel_.set_mass_flow_rate(value); }},
        {"PIPE_PANEL_EXPOSED",	        [this](double value){ pipe_on_panel_.set_exposed(static_cast<bool>(value)); }},
        {"PIPE_PANEL_PIPE_LENGTH",	    [this](double value){ pipe_on_panel_.set_pipe_length(value); }},
        {"PIPE_PANEL_MAX_TEMPERATURE",	[this](double value){ pipe_on_panel_.set_max_temperature(value); }},
        {"PIPE_PANEL_MIN_TEMPERATURE",	[this](double value){ pipe_on_panel_.set_min_temperature(value); }},
        {"PIPE_PANEL_INTERIOR_DIAMETER",[this](double value){ pipe_on_panel_.set_pipe_interior_diameter(value); }}};

    while (inputFile >> paramName >> paramValue)
    {
        if (updated_parameters.count(paramName))
        {
            std::cerr << "Warning: " << filename << " contains multiple overrides for " 
                      << paramName << std::endl;
        }
        else
        {
            updated_parameters.insert(paramName);
        }

        auto parameter_iterator = parameterMap.find(paramName);
        if (parameter_iterator != parameterMap.end())
        {
            parameter_iterator->second(paramValue);
        }
        else
        {
            std::cerr << "Unknown parameter: " << paramName << std::endl;
        }
    }

    double solar_panel_pipe_length_m = solar_panel_.get_surface_area_m2() *
                                       solar_panel_.get_pipe_contact_percentage() /
                                       pipe_on_panel_.get_pipe_interior_diameter_m();
    pipe_on_panel_.set_pipe_length(solar_panel_pipe_length_m);

    inputFile.close();
}

void Simulation::run_simulation(const std::string &sim_overrides_file,
                                const std::string &environmental_file,
                                const std::string &output_filename)
{

    static constexpr int ONE_SECOND = 1;

    read_simulation_constants(sim_overrides_file);
    environment_.read_environmental_conditions(environmental_file);

    std::ofstream output_file(output_filename);
    if (!output_file.is_open())
    {
        std::cerr << "Error opening output file: " << output_filename << std::endl;
        return;
    }

    print_headers(output_file);
    print_data_line(output_file);

    current_time_s_ = 0.0;
    while (current_time_s_ <= duration_s_)
    {
        tank_.one_second_update_temperature(pipe_into_tank_.get_water_out_temperature_C(),
                                            environment_,
                                            current_time_s_);
        pipe_into_panel_.one_second_update_temperature(tank_.get_water_out_temperature_C(),
                                                       environment_,
                                                       current_time_s_);
        solar_panel_.one_second_update_temperature(pipe_into_panel_.get_water_out_temperature_C(),
                                                   environment_,
                                                   current_time_s_,
                                                   pipe_on_panel_);
        pipe_into_tank_.one_second_update_temperature(pipe_on_panel_.get_water_out_temperature_C(),
                                                      environment_,
                                                      current_time_s_);

        current_time_s_ += ONE_SECOND;

        if (std::fmod(current_time_s_, time_step_s_) != 0)
            continue;

        print_data_line(output_file);
    }

    output_file.close();
}
