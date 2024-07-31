#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <algorithm>

class ThermodynamicObject
{
protected:
    double temperature_C_;                // °C
    double water_temperature_C_;          // °C
    double water_out_temperature_C_;      // °C
    double emissivity_;                   // 0 (perfect reflector) to 1 (perfect emitter)
    double thickness_m_;                  // m
    double specific_heat_capacity_JpkgC_; // J/(kg * °C)

    static constexpr double LAMINAR_FLOW_UPPER_BOUND = 2300;
    static constexpr double TURBULENT_FLOW_FULLY_DEVELOPED_DISTANCE_M = 10.0;

public:
    ThermodynamicObject() : temperature_C_(15.5),
                            water_temperature_C_(15.5),
                            water_out_temperature_C_(15.5),
                            emissivity_(0.64),
                            thickness_m_(0.00055),                 // 0.55 mm thickness
                            specific_heat_capacity_JpkgC_(376.812) /* Copper */
    {
    }

    double get_water_out_temperature_C() const { return water_out_temperature_C_; }
    double get_water_density_kgpm3(double water_tempurature_C) const;
    double get_water_dynamic_viscosity_kgpms(double tempurature_C) const;
    double get_water_thermal_conductivity_WpmK(double tempurature_C) const;
    double get_water_specific_heat_capacity_JpkgC(double tempurature_C) const;

    double get_temperature() const { return temperature_C_; }
    virtual double get_mass_kg() const = 0;

    void set_temperature(double temp) { temperature_C_ = temp; }
    void set_thickness(double thick) { thickness_m_ = thick; }
    void set_emissivity(double emiss) { emissivity_ = std::clamp(emiss, 0.0, 1.0); } // Always between 0 and 1
    void set_water_temperature(double temp) { water_temperature_C_ = temp; }
    void set_specific_heat_capacity(double capacity) { specific_heat_capacity_JpkgC_ = capacity; }

    void add_tempurature(double total_energy_added_J)
    {
        double tempurature_delta_K = total_energy_added_J /
                                     (specific_heat_capacity_JpkgC_ * get_mass_kg());
        temperature_C_ += tempurature_delta_K;
    }
};
