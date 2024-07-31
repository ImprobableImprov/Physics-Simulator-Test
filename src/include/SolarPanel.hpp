#include "CylinderContainer.hpp"

class Environment;
class CylinderContainer;

class SolarPanel : public ThermodynamicObject
{
private:
    double width_m_;
    double length_m_;
    double ideal_efficiency_;       // Efficiency at ideal temperatures
    double efficiency_coefficient_; // Efficiency drop per °C over ideal temp (%/°C)

    static constexpr double MAX_IDEAL_TEMPURATURE_C = 25.0;
    static constexpr double AVERAGE_PANEL_DENSITY_KGPM3 = 2400;
    static constexpr double PIPE_PANEL_CONTACT_PERCENTAGE = 0.7;
    static constexpr double LAMINAR_PLATE_MINIMUM_THRESHOLD = 0.05;
    static constexpr double TURBULENT_FLOW_LOWER_BOUND_FLAT_PLATE = 5e5;

public:
    SolarPanel() : width_m_(2),
                   length_m_(2),
                   ideal_efficiency_(0.225),
                   efficiency_coefficient_(0.26)
    {
        // Assume glass (77%; 840), silicon (14%; 705), and aluminum (9%; 897)
        specific_heat_capacity_JpkgC_ = 826.23;
        thickness_m_ = 0.05;
        emissivity_ = 0.93;
    }

    void set_wdith(double wid) { width_m_ = wid; }
    void set_length(double len) { length_m_ = len; }
    void set_ideal_efficiency(double efficiency) { ideal_efficiency_ = efficiency; }
    void set_efficiency_coefficient(double coefficient) { efficiency_coefficient_ = coefficient; }

    double get_pipe_contact_percentage() const { return PIPE_PANEL_CONTACT_PERCENTAGE; }
    double get_surface_area_m2() const { return length_m_ * width_m_; }
    double get_panel_efficiency() const
    {
        if (temperature_C_ <= MAX_IDEAL_TEMPURATURE_C)
            return ideal_efficiency_;

        return ideal_efficiency_ * 
               (1.0 - efficiency_coefficient_ * (temperature_C_ - MAX_IDEAL_TEMPURATURE_C));
    }

    double get_mass_kg() const override
    {
        double panel_volume_m3 = length_m_ * width_m_ * thickness_m_;

        return panel_volume_m3 * AVERAGE_PANEL_DENSITY_KGPM3;
    }

    double get_plate_convective_coefficient_Wpm2K(const Environment &environment, 
                                                  double current_time_s);
    void one_second_update_temperature(double intake_water_temperature_C,
                                       const Environment &environment,
                                       double current_time,
                                       CylinderContainer &pipe);
};
