#include "include/CylinderContainer.hpp"

double CylinderContainer::get_mass_kg() const {
    double COPPER_DENSITY_KGPM3 = 8940;
    
    return get_volume_m3() * COPPER_DENSITY_KGPM3;
}

double CylinderContainer::get_volume_m3() const {
    double pipe_exterior_diameter_m = pipe_interior_diameter_m_ + (2 * thickness_m_);
    double pipe_inner_radius_squared_m2 = std::pow(pipe_interior_diameter_m_ / 2, 2);
    double pipe_outer_radius_squared_m2 = std::pow(pipe_exterior_diameter_m / 2, 2);
    
    return M_PI * 
           pipe_length_m_ * 
           (pipe_outer_radius_squared_m2 - pipe_inner_radius_squared_m2);
}

double CylinderContainer::get_water_mass_kg() {
    double pipe_interior_volume_m3 = M_PI * 
                                     std::pow(pipe_interior_diameter_m_ / 2, 2) * 
                                     pipe_length_m_;
    return pipe_interior_volume_m3 * get_water_density_kgpm3(water_temperature_C_);
}

double CylinderContainer::get_pipe_cross_sectional_area_m2(bool is_inner_diameter = true){
    double pipe_diameter = is_inner_diameter 
                                ? pipe_interior_diameter_m_ 
                                : pipe_interior_diameter_m_ + (2 * thickness_m_);
    return M_PI * (std::pow(pipe_diameter, 2) / 4);
}

double CylinderContainer::get_flow_velocity(double water_temperature_C){
    return water_mass_flow_rate_kgps_ / 
           (get_water_density_kgpm3(water_temperature_C) * get_pipe_cross_sectional_area_m2());
}

double CylinderContainer::get_air_mass_flow_rate_kgps(double tempurature_C, 
                                                      Environment environment, 
                                                      double current_time){
    return environment.get_air_density_kgpm3(tempurature_C) * 
           get_pipe_cross_sectional_area_m2(/*is_inner*/ false) * 
           environment.get_wind_speed(current_time);
}

double CylinderContainer::get_pipe_surface_area_m2(bool is_inner_diameter = true){
    double pipe_diameter = is_inner_diameter 
                                ? pipe_interior_diameter_m_ 
                                : pipe_interior_diameter_m_ + (2 * thickness_m_);
    return M_PI * pipe_diameter * pipe_length_m_;
}

double CylinderContainer::get_fully_developed_velocity_in_pipe_m(double reynolds_number){
    return reynolds_number < LAMINAR_FLOW_UPPER_BOUND 
                                ? 0.05 * reynolds_number * pipe_interior_diameter_m_
                                : TURBULENT_FLOW_FULLY_DEVELOPED_DISTANCE_M * pipe_interior_diameter_m_;
}

double CylinderContainer::get_fully_developed_temperature_in_pipe_m(double reynolds_number, 
                                                                    double prandtl_number = 0.0){
    return reynolds_number < LAMINAR_FLOW_UPPER_BOUND 
                                ? 0.05 * reynolds_number * prandtl_number * pipe_interior_diameter_m_
                                : TURBULENT_FLOW_FULLY_DEVELOPED_DISTANCE_M * pipe_interior_diameter_m_;
}

double CylinderContainer::get_cylinder_convective_coefficient_Wpm2K(double water_temperature_C, 
                                                                    Environment environment = {}, 
                                                                    double current_time_s=-1.0){
    // Fluid properties and pipe diameter (the characteristic length) determined based on fluid
    double density_kgpm3, dynamic_viscosity_kgpms, flow_velocity_mps;
    double specific_heat_capacity_JpkgC, thermal_conductivity_WpmK, characteristic_length_m;
    if(current_time_s < 0){
        characteristic_length_m = pipe_interior_diameter_m_;
        dynamic_viscosity_kgpms = get_water_dynamic_viscosity_kgpms(water_temperature_C); // μ
        thermal_conductivity_WpmK = get_water_thermal_conductivity_WpmK(water_temperature_C); // k
        density_kgpm3 = get_water_density_kgpm3(water_temperature_C); // ρ
        flow_velocity_mps = get_flow_velocity(water_temperature_C);
        specific_heat_capacity_JpkgC = get_water_specific_heat_capacity_JpkgC(water_temperature_C); // C_p
    }
    else {
        double air_temperature_C = environment.get_ambient_temperature(current_time_s);
        characteristic_length_m = pipe_interior_diameter_m_ + (2 * thickness_m_); // Exterior Diameter
        dynamic_viscosity_kgpms = environment.get_air_dynamic_viscosity_kgpms(air_temperature_C); // μ
        thermal_conductivity_WpmK = environment.get_air_thermal_conductivity_WpmK(air_temperature_C); // k
        density_kgpm3 = environment.get_air_density_kgpm3(air_temperature_C); // ρ
        flow_velocity_mps = environment.get_wind_speed(current_time_s);
        specific_heat_capacity_JpkgC = environment.get_air_specific_heat_capacity_JpkgC(air_temperature_C); // C_p
    }
    
    double reynolds_number = (density_kgpm3 * flow_velocity_mps * characteristic_length_m) / 
                                dynamic_viscosity_kgpms;
    // Alternative method for calculating reynolds number
    // double mass_flow_rate_kgps = (current_time_s < 0)
    //                                     ? water_mass_flow_rate_kgps_ 
    //                                     : get_air_mass_flow_rate_kgps(air_temperature_C, environment, current_time_s);
    // [[maybe_unused]] double reynolds_number_2 = 4 * mass_flow_rate_kgps / 
    //                             (M_PI * dynamic_viscosity_kgpms * characteristic_length_m);

    double prandtl_number = specific_heat_capacity_JpkgC * dynamic_viscosity_kgpms / thermal_conductivity_WpmK;
    
    double nusselt_number;
    if(current_time_s < 0 && reynolds_number < LAMINAR_FLOW_UPPER_BOUND){ // Laminar flow
        bool is_fully_developed_temperature = 
                        pipe_length_m_ >= get_fully_developed_temperature_in_pipe_m(reynolds_number, prandtl_number);
        bool is_fully_developed_velocity = pipe_length_m_ >= get_fully_developed_velocity_in_pipe_m(reynolds_number);
        
        double graete_number = (pipe_interior_diameter_m_ / pipe_length_m_) * reynolds_number * prandtl_number;

        // Assumes constant surface tempurature across surface
        if(is_fully_developed_velocity && is_fully_developed_temperature){
            nusselt_number = 3.66;
        }
        else if(is_fully_developed_velocity && !is_fully_developed_temperature){
            nusselt_number = 3.66 + (0.0668 * graete_number) / (1 + 0.04 * std::pow(graete_number, 0.666));
        }
        else if(!is_fully_developed_velocity && !is_fully_developed_temperature){
            nusselt_number = (3.66 / 
                                tanh(std::pow(2.264 * graete_number, -0.333) + std::pow(1.7 * graete_number, -0.666)) +
                                (0.0499 * graete_number * tanh(std::pow(graete_number, -1)))) /
                                tanh(2.432 * std::pow(prandtl_number, 0.166) * std::pow(graete_number, -0.166));
        }
        else{
            throw std::runtime_error("Error: NOT fully developed velocity WITH fully developed temperature\n");
        }
    } 
    else if (current_time_s < 0) { // Turbulent flow
        double prandtl_power = (water_temperature_C < temperature_C_) ? 0.3 : 0.4; // water is heating vs cooling
        nusselt_number = 0.023 * std::pow(reynolds_number, 0.8) * std::pow(prandtl_number, prandtl_power);
    } 
    else { // Air
        // Churchill-Bernstein equation - used for flow over a cylinder
        nusselt_number = 0.3 + (0.62 * std::pow(reynolds_number, 0.5) * std::pow(prandtl_number, 0.333)) / 
                                std::pow(1 + std::pow(0.4 / prandtl_number, 0.666), 0.25) * 
                                std::pow(1 + std::pow(reynolds_number / 282000, 0.625), 0.8);
    }

    // Calculate convection coefficient (h)
    return (nusselt_number * thermal_conductivity_WpmK) / characteristic_length_m;
}

void CylinderContainer::one_second_update_temperature(double intake_water_temperature_C, 
                                           Environment environment, 
                                           double current_time){
    
    if(get_mass_kg() <= 0.0 || specific_heat_capacity_JpkgC_ <= 0.0){
        throw std::invalid_argument( "Error: Pipe mass or specific heat <= 0" );
    }

    static const int MAX_ITERATIONS = 25;
    static const double TEMPURATURE_THRESHOLD_C = 0.001;

    intake_water_temperature_C = std::clamp(intake_water_temperature_C, 
                                            min_temperature_C_, 
                                            max_temperature_C_);
                                            
    /// (1) Update water tempurature
    if(is_tank_){
        double heat_added_W = (intake_water_temperature_C - water_temperature_C_) * 
                              water_mass_flow_rate_kgps_ * 
                              get_water_specific_heat_capacity_JpkgC(intake_water_temperature_C);
        add_heat_to_water(heat_added_W); // evaluating over a single second -> Joules
    }
    else{
        set_water_temperature(intake_water_temperature_C);
    }

    /// (2) Calculate heat gained for pipe
    if(is_exposed_)
    {
        /// (2a) Heat trasnfer from SUN --> Copper Pipe
        double solar_absorbtion_W = environment.get_solar_irradiance_Wpm2(current_time) * 
                                        get_pipe_surface_area_m2(/*is_inner*/ false) * emissivity_;

        /// (2b) Convective Heat Transfer between Copper Pipe and Air
        double air_heat_transfer_coefficient = get_cylinder_convective_coefficient_Wpm2K(0, 
                                                                                         environment, 
                                                                                         current_time);
        double heat_transfered_to_air_W = air_heat_transfer_coefficient * 
                                            get_pipe_surface_area_m2(/*is_inner*/ false) * 
                                            (temperature_C_ - 
                                             environment.get_ambient_temperature(current_time));

        double heat_added_by_environment_W = solar_absorbtion_W - heat_transfered_to_air_W;

        add_tempurature(heat_added_by_environment_W); // evaluating over a single second -> Joules
    }

    /// (3) Heat Transfer between Pipe and Water
    double starting_water_temperature_C = water_temperature_C_;
    double previous_water_out_tempurature_C = starting_water_temperature_C;

    // Mean water tempurature is unknown since the tempurature out of the pipe is being calculated
    // (3a) Initially assume the final tempurature is equal to the pipe tempurature
    double mean_water_temperature_C = (starting_water_temperature_C + temperature_C_) / 2;
    double water_heat_transfer_coefficient, updated_water_out_temperature_C;

    // (3b) Repeating the calculation, updating the output (and mean) tempurature each step
    // (3c) Exit when completing MAX_ITERATIONS or when the delta between of the last calculation is 
    //      less then TEMPURATURE_THRESHOLD_C
    for(int iterations = 0; iterations < MAX_ITERATIONS; iterations++){
        water_heat_transfer_coefficient = get_cylinder_convective_coefficient_Wpm2K(mean_water_temperature_C);
        updated_water_out_temperature_C = 
                temperature_C_ - (temperature_C_ - starting_water_temperature_C) * 
                                    exp(((-get_pipe_surface_area_m2() * 
                                            water_heat_transfer_coefficient) / 
                                          (water_mass_flow_rate_kgps_ * 
                                            get_water_specific_heat_capacity_JpkgC(mean_water_temperature_C))
                                        ));
        
        updated_water_out_temperature_C = std::clamp(updated_water_out_temperature_C, 
                                                     min_temperature_C_, 
                                                     max_temperature_C_);
        mean_water_temperature_C = (starting_water_temperature_C + updated_water_out_temperature_C) / 2;

        if(std::abs(previous_water_out_tempurature_C - updated_water_out_temperature_C) < TEMPURATURE_THRESHOLD_C ||
           iterations == (MAX_ITERATIONS - 1)){
            break;
        }

        previous_water_out_tempurature_C = updated_water_out_temperature_C;
    }

    /// (4) Update Cylinder temps
    double heat_transfered_to_water_W = water_mass_flow_rate_kgps_ * 
                                        get_water_specific_heat_capacity_JpkgC(mean_water_temperature_C) * 
                                        (updated_water_out_temperature_C - starting_water_temperature_C);

    add_tempurature(-heat_transfered_to_water_W); // evaluating over a single second -> Joules

    water_out_temperature_C_ = updated_water_out_temperature_C;
}

void CylinderContainer::add_heat_to_water(double total_energy_added_J) { 
    double water_tempurature_delta_K = total_energy_added_J / 
                                        (get_water_specific_heat_capacity_JpkgC(water_temperature_C_) * 
                                         get_water_mass_kg());
    water_temperature_C_ += water_tempurature_delta_K; 
}
