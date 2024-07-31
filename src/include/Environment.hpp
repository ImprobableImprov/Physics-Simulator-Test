#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

class Environment
{
private:
    std::vector<double> times_;
    std::vector<double> solar_irradiances_;
    std::vector<double> ambient_temperatures_;
    std::vector<double> wind_speeds_;

public:
    Environment() : times_(0), solar_irradiances_(0), ambient_temperatures_(0), wind_speeds_(0) {}

    void read_environmental_conditions(const std::string &filename);

    double get_solar_irradiance_Wpm2(double time) const;
    double get_ambient_temperature(double time) const;
    double get_wind_speed(double time) const;
    double get_air_density_kgpm3(double tempurature_C) const;
    double get_air_dynamic_viscosity_kgpms(double tempurature_C) const;
    double get_air_thermal_conductivity_WpmK(double tempurature_C) const;
    double get_air_specific_heat_capacity_JpkgC(double tempurature_C) const;

private:
    double interpolate_data(double time,
                            const std::vector<double> &data_times,
                            const std::vector<double> &data) const;
};
