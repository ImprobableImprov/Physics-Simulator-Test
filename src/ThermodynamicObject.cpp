#include "include/ThermodynamicObject.hpp"

double ThermodynamicObject::get_water_density_kgpm3(double water_tempurature_C) const
{
    if (water_tempurature_C < 0)
    {
        std::cerr << "WARNING: Temperature too low to calculate water density. Assume temperature of 0.01 degrees Celcius\n";
        water_tempurature_C = 0.01;
    }
    // This function is not accurate above 130 degrees Celcius, but liquid water temperature is capped between 0 and 100
    else if (water_tempurature_C >= 100)
    {
        std::cerr << "WARNING: Temperature too high to calculate water density. Assume temperature of 99.99 degrees Celcius\n";
        water_tempurature_C = 99.99;
    }
    return (-9.204453627e-8 * std::pow(water_tempurature_C, 4) +
            3.420742008672e-5 * std::pow(water_tempurature_C, 3) -
            7.08919807166417e-3 * std::pow(water_tempurature_C, 2) +
            4.37529454518197e-2 * water_tempurature_C +
            999.88826440573500000);
}

double ThermodynamicObject::get_water_thermal_conductivity_WpmK(double tempurature_C) const
{
    if (tempurature_C < 0.0)
    {
        std::cerr << "WARNING: Temperature too low to calculate water dynamic viscosity\n";
    }
    else if (tempurature_C >= 100.0)
    {
        std::cerr << "WARNING: Temperature too high to calculate water dynamic viscosity\n";
    }
    return (-4.303583e-10 * std::pow(tempurature_C, 4) +
            1.26438343e-7 * std::pow(tempurature_C, 3) -
            2.10952874077e-5 * std::pow(tempurature_C, 2) +
            2.4899100910016e-3 * tempurature_C +
            0.555726205152388);
}

double ThermodynamicObject::get_water_dynamic_viscosity_kgpms(double tempurature_C) const
{
    // Pa*s is equivalent to kg/(m*s)
    if (tempurature_C < 0.0)
    {
        std::cerr << "WARNING: Temperature too low to calculate water dynamic viscosity\n";
    }
    // This function is not accurate above 370 degrees Celcius, but liquid water temperature is capped between 0 and 100
    else if (tempurature_C >= 100.0)
    {
        std::cerr << "WARNING: Temperature too high to calculate water dynamic viscosity\n";
    }
    return tempurature_C < 95
               ? (2.77388442e-15 * std::pow(tempurature_C, 6) -
                  1.24359703683e-12 * std::pow(tempurature_C, 5) +
                  2.2981389243372e-10 * std::pow(tempurature_C, 4) -
                  2.31037210686735e-8 * std::pow(tempurature_C, 3) +
                  1.43393546700877e-6 * std::pow(tempurature_C, 2) -
                  6.06414092004945e-5 * tempurature_C +
                  1.79157254681817e-3)
               : (-4.5460686e-16 * std::pow(tempurature_C, 5) +
                  5.9247759433e-13 * std::pow(tempurature_C, 4) -
                  3.153065024333e-10 * std::pow(tempurature_C, 3) +
                  8.68688593636402e-8 * std::pow(tempurature_C, 2) -
                  1.29338619788223e-5 * tempurature_C +
                  9.66679340785643e-2);
}

double ThermodynamicObject::get_water_specific_heat_capacity_JpkgC(double tempurature_C) const
{ // Isobaric
    if (tempurature_C < -160.0)
    {
        std::cerr << "WARNING: Temperature too low to calculate water specific heat capacity\n";
    }
    // This function is not accurate above 320 degrees Celcius, but liquid water temperature is capped between 0 and 100
    else if (tempurature_C >= 100.0)
    {
        std::cerr << "WARNING: Temperature too high to calculate water specific heat capacity\n";
    }
    return (3.537165e-11 * std::pow(tempurature_C, 6) -
            2.853687405e-8 * std::pow(tempurature_C, 5) +
            9.00625896115e-6 * std::pow(tempurature_C, 4) -
            1.33933025300616e-3 * std::pow(tempurature_C, 3) +
            0.10443179606629200 * std::pow(tempurature_C, 2) -
            3.62516252242907000 * tempurature_C +
            4222.34973344988000000);
}
