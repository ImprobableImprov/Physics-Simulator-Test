#include "Environment.hpp"
#include "ThermodynamicObject.hpp"

class CylinderContainer : public ThermodynamicObject
{
  bool is_tank_;
  bool is_exposed_;
  double pipe_length_m_;
  double max_temperature_C_;
  double min_temperature_C_;
  double pipe_interior_diameter_m_;
  double water_mass_flow_rate_kgps_;

public:
  CylinderContainer() : is_tank_(false),
                        is_exposed_(true),
                        pipe_length_m_(4.0),
                        max_temperature_C_(99.0),
                        min_temperature_C_(1.0),
                        pipe_interior_diameter_m_(0.04),
                        water_mass_flow_rate_kgps_(0.5) {}

  CylinderContainer(bool is_tank,
                    bool is_exposed,
                    double length,
                    double interior_diameter) : is_tank_(is_tank),
                                                is_exposed_(is_exposed),
                                                pipe_length_m_(length),
                                                max_temperature_C_(99.0),
                                                min_temperature_C_(1.0),
                                                pipe_interior_diameter_m_(interior_diameter),
                                                water_mass_flow_rate_kgps_(0.5)
  {
    if (is_tank)
      thickness_m_ = 0.002; // 2 mm
  }

  void set_mass_flow_rate(double flow_rate) { water_mass_flow_rate_kgps_ = flow_rate; }
  void set_exposed(bool exposed) { is_exposed_ = exposed; }
  void set_pipe_length(double pipe_length) { pipe_length_m_ = pipe_length; }
  void set_max_temperature(bool max_temperature) { max_temperature_C_ = max_temperature; }
  void set_min_temperature(bool min_temperature) { min_temperature_C_ = min_temperature; }
  void set_pipe_interior_diameter(bool pipe_interior_diameter)
  {
    pipe_interior_diameter_m_ = pipe_interior_diameter;
  }
  double get_thickness_m() const { return thickness_m_; }
  double get_pipe_interior_diameter_m() { return pipe_interior_diameter_m_; }

  double get_mass_kg() const override;
  double get_volume_m3() const;
  double get_water_mass_kg();
  double get_flow_velocity(double temperature_C);
  double get_pipe_surface_area_m2(bool is_inner_diameter);
  double get_pipe_cross_sectional_area_m2(bool is_inner_diameter);
  double get_fully_developed_velocity_in_pipe_m(double reynolds_number);
  double get_fully_developed_temperature_in_pipe_m(double reynolds_number,
                                                   double prandtl_number);
  double get_air_mass_flow_rate_kgps(double tempurature_C, Environment environment, double current_time);
  double get_cylinder_convective_coefficient_Wpm2K(double water_temperature_C, Environment environment, double current_time_s);
  void one_second_update_temperature(double intake_water_energy_W, Environment environment, double current_time_s);
  void add_heat_to_water(double total_energy_added_W);
};
