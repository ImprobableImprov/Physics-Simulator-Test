#include "include/Environment.hpp"

void Environment::read_environmental_conditions(const std::string &filename)
{
    std::ifstream input_file(filename);
    if (!input_file.is_open())
    {
        std::cerr << "Error opening input file: " << filename << std::endl;
        return;
    }

    times_.clear();
    solar_irradiances_.clear();
    ambient_temperatures_.clear();
    wind_speeds_.clear();

    double time, solar_irradiance, ambient_temperature, wind_speed;
    while (input_file >> time >> solar_irradiance >> ambient_temperature >> wind_speed)
    {
        times_.push_back(time);
        solar_irradiances_.push_back(solar_irradiance);
        ambient_temperatures_.push_back(ambient_temperature);
        wind_speeds_.push_back(wind_speed);
    }

    input_file.close();
}

double Environment::get_solar_irradiance_Wpm2(double time) const
{
    return interpolate_data(time, times_, solar_irradiances_);
}

double Environment::get_ambient_temperature(double time) const
{
    return interpolate_data(time, times_, ambient_temperatures_);
}

double Environment::get_wind_speed(double time) const
{
    return interpolate_data(time, times_, wind_speeds_);
}

double Environment::get_air_density_kgpm3(double tempurature_C) const
{
    if (tempurature_C < -160)
    {
        std::cerr << "WARNING: Temperature too low to calculate air density\n";
    }
    else if (tempurature_C > 412)
    {
        std::cerr << "WARNING: Temperature too high to calculate air density\n";
    }
    return (2.9576e-16 * std::pow(tempurature_C, 6) -
            3.9652642e-13 * std::pow(tempurature_C, 5) +
            2.1718680185e-10 * std::pow(tempurature_C, 4) -
            6.945751260987e-8 * std::pow(tempurature_C, 3) +
            1.78311601969631e-5 * std::pow(tempurature_C, 2) -
            4.71854397898636e-3 * tempurature_C +
            1.29163880414105);
}

double Environment::get_air_dynamic_viscosity_kgpms(double tempurature_C) const
{
    if (tempurature_C < -100.0)
    {
        std::cerr << "WARNING: Temperature too low to calculate air thermal conductivity\n";
    }
    else if (tempurature_C >= 1600.0)
    {
        std::cerr << "WARNING: Temperature too high to calculate air thermal conductivity\n";
    }
    return (-2.2495e-24 * std::pow(tempurature_C, 6) +
            1.30799548e-20 * std::pow(tempurature_C, 5) -
            3.15320505817e-17 * std::pow(tempurature_C, 4) +
            4.2741742090717e-14 * std::pow(tempurature_C, 3) -
            4.09241523945073e-11 * std::pow(tempurature_C, 2) +
            4.96170533680107e-8 * tempurature_C +
            1.71502137925242e-5);
}

double Environment::get_air_thermal_conductivity_WpmK(double tempurature_C) const
{ // Assumes pressure at 1 bar
    if (tempurature_C < -190.0)
    {
        std::cerr << "WARNING: Temperature too low to calculate air thermal conductivity\n";
    }
    else if (tempurature_C >= 1600.0)
    {
        std::cerr << "WARNING: Temperature too high to calculate air thermal conductivity\n";
    }
    return (-3.75e-21 * std::pow(tempurature_C, 6) +
            1.909230e-17 * std::pow(tempurature_C, 5) -
            4.016396062e-14 * std::pow(tempurature_C, 4) +
            4.901175449123e-11 * std::pow(tempurature_C, 3) -
            4.4075614398888e-8 * std::pow(tempurature_C, 2) +
            7.66069577308689e-5 * tempurature_C +
            2.43560822452597e-2);
}

double Environment::get_air_specific_heat_capacity_JpkgC(double tempurature_C) const
{ // Isobaric
    if (tempurature_C < -160)
    {
        std::cerr << "WARNING: Temperature too low to calculate air specific heat capacity\n";
    }
    else if (tempurature_C > 1600)
    {
        std::cerr << "WARNING: Temperature too high to calculate air specific heat capacity\n";
    }
    return (-3.46607e-17 * std::pow(tempurature_C, 6) +
            9.12184727e-14 * std::pow(tempurature_C, 5) +
            1.079641988814e-10 * std::pow(tempurature_C, 4) -
            5.71448440538998e-7 * std::pow(tempurature_C, 3) +
            5.77335351056597e-4 * std::pow(tempurature_C, 2) +
            8.97638457487819e-3 * tempurature_C +
            1005.28623891845);
}

double Environment::interpolate_data(double time,
                                     const std::vector<double> &data_times,
                                     const std::vector<double> &data) const
{
    if (data_times.empty() || data.empty())
        return 0.0;

    if (time <= data_times.front())
        return data.front();
    if (time >= data_times.back())
        return data.back();

    size_t idx = 0;
    while (idx < data_times.size() && time > data_times[idx])
    {
        idx++;
    }

    if (idx == 0)
    {
        return data[0];
    }
    else
    {
        double time1 = data_times[idx - 1];
        double time2 = data_times[idx];
        double x1 = data[idx - 1];
        double x2 = data[idx];

        return x1 + (x2 - x1) * (time - time1) / (time2 - time1);
    }
}
