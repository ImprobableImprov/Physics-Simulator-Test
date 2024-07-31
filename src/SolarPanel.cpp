#include "include/SolarPanel.hpp"

double SolarPanel::get_plate_convective_coefficient_Wpm2K(const Environment &environment, double current_time_s)
{
    double characteristic_length_m = length_m_;
    double film_temperature_C = (environment.get_ambient_temperature(current_time_s) + temperature_C_) / 2; // T_f, mean temperature
    double dynamic_viscosity_kgpms = environment.get_air_dynamic_viscosity_kgpms(film_temperature_C);       // μ
    double thermal_conductivity_WpmK = environment.get_air_thermal_conductivity_WpmK(film_temperature_C);   // k
    double air_density_kgpm3 = environment.get_air_density_kgpm3(film_temperature_C);                       // ρ
    double flow_velocity_mps = environment.get_wind_speed(current_time_s);
    double specific_heat_capacity_JpkgC = environment.get_air_specific_heat_capacity_JpkgC(film_temperature_C); // C_p

    double reynolds_number = (air_density_kgpm3 * flow_velocity_mps * characteristic_length_m) /
                             dynamic_viscosity_kgpms;
    double prandtl_number = specific_heat_capacity_JpkgC * dynamic_viscosity_kgpms / thermal_conductivity_WpmK;

    double transition_to_turbulent_flow_m = (TURBULENT_FLOW_LOWER_BOUND_FLAT_PLATE * dynamic_viscosity_kgpms) /
                                            (air_density_kgpm3 * flow_velocity_mps);
    double threshold_to_include_laminar_flow_m = LAMINAR_PLATE_MINIMUM_THRESHOLD * characteristic_length_m;

    double nusselt_number;
    if (transition_to_turbulent_flow_m <= threshold_to_include_laminar_flow_m)
    { // Only turbulent flow
        nusselt_number = 0.037 * std::pow(reynolds_number, 0.8) * std::pow(prandtl_number, 0.333);
    }
    else if (characteristic_length_m <= transition_to_turbulent_flow_m)
    { // Only laminar flow
        nusselt_number = 0.664 *
                         std::pow((air_density_kgpm3 * flow_velocity_mps) /
                                      (dynamic_viscosity_kgpms * characteristic_length_m),
                                  0.5) *
                         std::pow(prandtl_number, 0.333);
    }
    else
    { // Mixed flow
        nusselt_number = (0.037 * std::pow(reynolds_number, 0.8) - 871) * std::pow(prandtl_number, 0.333);
    }

    // Calculate convection coefficient (h)
    return (nusselt_number * thermal_conductivity_WpmK) / characteristic_length_m;
}

void SolarPanel::one_second_update_temperature(double intake_water_temperature_C,
                                               const Environment &environment,
                                               double current_time_s,
                                               CylinderContainer &panel_pipe)
{
    const double COPPER_THERMAL_CONDUCTIVITY_WPMK = 413.0;
    const double STEFAN_BOLTZMANN_CONST_WPM2K4 = 5.67e-8; // W / (m^2 * K^4)
    const double MIN_PANEL_EFFICIENCY = 0.05;

    double efficiency_drop_from_heat = 1.0 - efficiency_coefficient_ * ((temperature_C_ - MAX_IDEAL_TEMPURATURE_C) / 100);
    double panel_efficiency = temperature_C_ <= MAX_IDEAL_TEMPURATURE_C ? ideal_efficiency_
                                                                        : ideal_efficiency_ * std::clamp(efficiency_drop_from_heat, MIN_PANEL_EFFICIENCY, 1.0);

    double heat_from_sun_W = environment.get_solar_irradiance_Wpm2(current_time_s) *
                             panel_efficiency *
                             get_surface_area_m2();

    double ambient_temperature_C = environment.get_ambient_temperature(current_time_s);
    double panel_radiative_loss_W = STEFAN_BOLTZMANN_CONST_WPM2K4 *
                                    emissivity_ * get_surface_area_m2() *
                                    (std::pow(temperature_C_ + 273.15, 4) -
                                     std::pow(ambient_temperature_C + 273.15, 4));

    double contact_area_m2 = get_surface_area_m2() * PIPE_PANEL_CONTACT_PERCENTAGE;
    double pipe_length_in_contact_panel_m = contact_area_m2 / panel_pipe.get_pipe_interior_diameter_m();
    double panel_conductive_loss_to_pipe_W = COPPER_THERMAL_CONDUCTIVITY_WPMK *
                                             contact_area_m2 *
                                             (temperature_C_ - panel_pipe.get_temperature()) /
                                             pipe_length_in_contact_panel_m;

    double panel_convective_loss_air_W = get_plate_convective_coefficient_Wpm2K(environment, current_time_s) *
                                         get_surface_area_m2() *
                                         (temperature_C_ - ambient_temperature_C);

    double total_energy_added_W = heat_from_sun_W -
                                  panel_radiative_loss_W -
                                  panel_conductive_loss_to_pipe_W -
                                  panel_convective_loss_air_W;

    add_tempurature(total_energy_added_W);
    panel_pipe.add_tempurature(panel_conductive_loss_to_pipe_W);

    panel_pipe.one_second_update_temperature(intake_water_temperature_C, environment, current_time_s);
}
