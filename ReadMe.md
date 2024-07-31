# Physics Simulator Test
This project focuses on creating a basic, limited scope simulation of a solar panel heat transfer system. Specifically, via a water loop: storage tank -> outtake copper pipe -> solar panel -> intake copper pipe -> (back to) storage tank. This simulation approximates the transfer of heat between the environemnt, water, copper walls, and solar array for the given time of the simulation.

It is limited in many aspects and makes some assumptions that are not reflective of real-world observations. For example, heat transfer is calculated discretely (once per second) and assumes constant surface temperature of the copper walls before updating. This can lead to the simulation breaking down and reflecting inaccurate results, especially at very high, very low, or unusual input values. 

More optimization, learning, and safeguards are required to improve the simulation's accuracy and scope, especially under more extreme or edge case conditions. For example, this simulation provides little input verification and relies on the user to ensure proper formatting and expected values are in place.

## Setup
```
git clone https://github.com/ImprobableImprov/Physics-Simulator-Test.git
```
```
cd Physics-Simulator-Test
```
```
make
```
```
./PhysicsSimulatorTest
```

## Functional Tests and Examples
These cases are not meant to cover every failure or specific feature; however, it should give you an idea of the expected behavior of the simulation and what to expect from given inputs.

### 1. Directory and File specifications
 #### Test case 1: 
Directory layout:
 ```
Application
└── output
    ├── simulation_log.txt
└── input
    ├── overrides.txt
    ├── environemnt.txt
└── src
    └── include
        ├── Environemnt.hpp
        ├── ...
    ├── Environemnt.cpp
    ├── ...
├── README.md
├── Makefile
```
Run the simulation:
```
./PhysicsSimulatorTest
```
**Expected Results**: Given the input files are located within the `input` directory (as outlined above) no warning will be displayed, and the resulting overrides and environment fields will populate as outlined in the below test cases. Otherwise, a warning will appear in the console reading "Error opening input file: input/[input file name]" and the overrides or environment variables will not be displayed in the output text file `simulaiton_log.txt`.

### 2. INPUT: Overrides
The `overrides.txt` file allows the user to set starting values for the simulation that do not relate to the environment. For example, overriding the default thickness of the water storage tank's walls by combining `TANK_THICKNESS` and the corresponding, space-separated thickness value on a distinct line within the file. See the below section `Override Options` for more details on all overrides available

 #### Test case 2
 Given the following entries in overrides.txt
```overrides.txt
SIMULATION_DURATION 11
SIMULATION_TIME_STEP 3
simulation_duration 9
SIMULATION_TIME_STEP 2
```
 **Expected Result**: 
 1. Irrespective of the order of the above overrides, the output file `simulaion_log.txt` gives the following result in the `Time (s)` column.
 2. As can be seen in this example, since the end time of the simulation at time-equals 11 is not divisible by the time step (2), it is not displayed.
```simulation_log.txt
  Time (s)         ...
         0         ...
         2         ...
         4         ...
         6         ...
         8         ...
        10         ...
```
3. A warning will be printed to the console: "Unknown parameter: simulation_duration". Since the overrides are case-sensitive, the lowercase "simulation_duration" is rejected and unused.
4. A warning will be printed to the console: "Warning: input/overrides.txt contains multiple overrides for SIMULATION_TIME_STEP". The final override value (from top to bottom) will be used.
Console Output:
```
Unknown parameter: simulation_duration
Warning: input/overrides.txt contains multiple overrides for SIMULATION_TIME_STEP
```
-----------------------------------------------
 #### Test case 3: 
Given the following `overrides.txt` input file:
```overrides.txt
PANEL_TEMPERATURE 50.0
```
 **Expected Result**: The first entry on the `Solar Panel (°C)` column in `simulation_log.txt` should be equivalent to the override value
 ```simulation_log.txt
...   Water (To Panel) (°C)        Solar Panel (°C)       Pipe (Panel) (°C)      ...
...                   15.50                   50.00                   15.50      ...
...                   16.34                   49.90                   16.49      ...
...                     ...                     ...                     ...      ...
```

### 2. INPUT: Environment (from models)
The simulation expects input for the environmental conditions. It expects four space-separated values on each line representing the following values (listed top to bottom as they are listed left to right in `environment.txt`):

| Values | Type | Example | Units |
| - | - | - | - |
| time | integer | 2 | seconds |
| solar irradiation | double | 800.0 | W/(m^2) |
| ambient temperature | double | 25.2 | °C |
| wind velocity | double | 2.5 | m/s |

#### Test Case 4:
Given the following `environment.txt` input file:
```environemnt.txt
2 800 25.0 2.0
8 1000 28.0 3.0
```
**Expected Results**: 
1. Any time(s) before the first entry in environemnt.txt should hold the values of the first row. For example, both time-equals 0 and 2 have the same starting values listed in the above given input file
2. Any time(s) after the last entry in environemnt.txt should hold the values of the last row. For example, both time-equals 8 and 10 have the same values listed in the last row of the above given input file
3. Values that do not fall on given times, as expressed in environment.txt, should linearly interpolate between the two nearest (time-wise) entries. For example, at time-equals 4 and 6 Solar Irradiance interpolates between the given values at time-equals 2 and time-equals 8, giving 866.67 and 933.33, respectfully
4. No error or warning will be thrown if the input file is improperly formatted; however, it will be apparent in the output file (`simulation_log.txt`) that values do not match expected input.

```simulation_log.txt
  Time (s)  Ambient (°C)     Wind (m/s)  Irradiance (W/m^2)     Tank (°C)       ...
         0         25.00           2.00              800.00         15.50       ...
         2         25.00           2.00              800.00         15.67       ...
         4         26.00           2.33              866.67         15.84       ...
         6         27.00           2.67              933.33         16.03       ...
         8         28.00           3.00             1000.00         16.23       ...
        10         28.00           3.00             1000.00         16.44       ...
```

### 3. OUTPUT: Results
The output file `simulaiton_log.txt` shows starting values at time-equals 0 on the first line below the headers. The headers should appear at the top of the output file: identify what each column represents, and what units that column is displayed in (e.g. `Solar Panel (°C)` represents the average temperature of the solar panel in degrees Celsius). Values for simulation time, environment conditions, object temperatures, and output water temperatures are given at each timestep of the simulation

#### Test Case 5:
Sun shining, wind blowing, and ambient temperatures above water and pipe temperatures
```environemnt.txt
10 1000 28.0 1.7
```
**Expected Output**: Water and Object temperature increases
```simulation.txt
  Time (s)  Ambient (°C)     Wind (m/s)  Irradiance (W/m^2)     Tank (°C)       Water (Tank) (°C)    Pipe (To Panel) (°C)   Water (To Panel) (°C)        Solar Panel (°C)       Pipe (Panel) (°C)      Water (Panel) (°C)     Pipe (To Tank) (°C)    Water (To Tank) (°C)
         0         28.00           1.70             1000.00         15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50
         5         28.00           1.70             1000.00         16.02                   15.57                   15.88                   15.79                   15.52                   15.62                   15.59                   15.90                   15.81
        10         28.00           1.70             1000.00         16.52                   15.64                   15.95                   15.85                   15.53                   15.72                   15.70                   16.01                   15.92
```
-----------------------------------------------

#### Test Case 6
No sun, no wind, and ambient temperatures start out equivalent to object and water temperatures
```environemnt.txt
10 0 15.5 0.0
```
**Expected Output**: No Water or Object temperature change
```
  Time (s)  Ambient (°C)     Wind (m/s)  Irradiance (W/m^2)     Tank (°C)       Water (Tank) (°C)    Pipe (To Panel) (°C)   Water (To Panel) (°C)        Solar Panel (°C)       Pipe (Panel) (°C)      Water (Panel) (°C)     Pipe (To Tank) (°C)    Water (To Tank) (°C)
         0         15.50           0.00                0.00         15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50
         5         15.50           0.00                0.00         15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50
        10         15.50           0.00                0.00         15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50
```
-----------------------------------------------

#### Test Case 7
No Sun and ambient temperature is below the object and water starting temperatures
```environemnt.txt
10 0 1.5 0.33
```
**Expected Output**: Water and Object temperatures decrease
```
  Time (s)  Ambient (°C)     Wind (m/s)  Irradiance (W/m^2)     Tank (°C)       Water (Tank) (°C)    Pipe (To Panel) (°C)   Water (To Panel) (°C)        Solar Panel (°C)       Pipe (Panel) (°C)      Water (Panel) (°C)     Pipe (To Tank) (°C)    Water (To Tank) (°C)
         0          1.50           0.33                0.00         15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50
         5          1.50           0.33                0.00         15.48                   15.50                   15.47                   15.46                   15.50                   15.48                   15.49                   15.46                   15.46
        10          1.50           0.33                0.00         15.46                   15.49                   15.47                   15.46                   15.49                   15.47                   15.48                   15.45                   15.44
```

### 4. WARNINGS: Temperature
Warnings are displayed when temperature breaks outside of expected values (typically between 0 and 100 °C for water). This can happen when starting or ambient temperatures are particularly high or low. It may also occur when values such as mass flow rate and surface area exceed standard expected values. 

Warnings are printed to the console and appear in the following format: "WARNING: Temperature too high to calculate water density. Assume temperature of 99.99 degrees Celsius"

#### Test Case 8
Extremely high starting temperatures
```
TANK_WATER_TEMPERATURE 1000
```
```
**Expected Output**: Warning messages printed to console
...
WARNING: Temperature too high to calculate water dynamic viscosity
WARNING: Temperature too high to calculate water dynamic viscosity
WARNING: Temperature too high to calculate water density. Assume temperature of 99.99 degrees Celcius
WARNING: Temperature too high to calculate water density. Assume temperature of 99.99 degrees Celcius
WARNING: Temperature too high to calculate water specific heat capacity
WARNING: Temperature too high to calculate water specific heat capacity
...
```
```
  Time (s)  Ambient (°C)     Wind (m/s)  Irradiance (W/m^2)     Tank (°C)       Water (Tank) (°C)    Pipe (To Panel) (°C)   Water (To Panel) (°C)        Solar Panel (°C)       Pipe (Panel) (°C)      Water (Panel) (°C)     Pipe (To Tank) (°C)    Water (To Tank) (°C)
         0          1.50           0.33                0.00         15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50
         5          1.50           0.33                0.00       9395.84                   99.00                  103.35                   99.00                   15.50                   53.56                   46.96                   46.10                   43.18
        10          1.50           0.33                0.00      18760.92                   99.00                  100.71                   99.00                   15.51                   75.81                   72.43                   72.61                   70.53
```
-----------------------------------------------

#### Test Case 9 
High Water Mass Flow Rate
```
SIMULATION_DURATION 11
SIMULATION_TIME_STEP 5
MASS_FLOW_RATE 3
```
```
10 0 1.5 0.33
```
**Expected Output**: Warning messages printed to console
```
WARNING: Temperature too low to calculate water dynamic viscosity
WARNING: Temperature too low to calculate water dynamic viscosity
WARNING: Temperature too low to calculate water density. Assume temperature of 0.01 degrees Celcius
WARNING: Temperature too low to calculate water density. Assume temperature of 0.01 degrees Celcius
WARNING: Temperature too low to calculate water dynamic viscosity
WARNING: Temperature too low to calculate water dynamic viscosity
WARNING: Temperature too low to calculate water density. Assume temperature of 0.01 degrees Celcius
WARNING: Temperature too low to calculate water density. Assume temperature of 0.01 degrees Celcius
```
```
  Time (s)  Ambient (°C)     Wind (m/s)  Irradiance (W/m^2)     Tank (°C)       Water (Tank) (°C)    Pipe (To Panel) (°C)   Water (To Panel) (°C)        Solar Panel (°C)       Pipe (Panel) (°C)      Water (Panel) (°C)     Pipe (To Tank) (°C)    Water (To Tank) (°C)
         0          1.50           0.33                0.00         15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50                   15.50
         5          1.50           0.33                0.00         15.48                   15.50                   17.07                   15.33                   15.50                   15.38                   15.55                   17.76                   15.30
        10          1.50           0.33                0.00         15.46                   15.49                  -17.62                   19.43                   15.49                   18.12                   14.14                  -51.32                   21.89
```

## References
To simulate the thermodynamic system, many references to heat transfer equations and material data are used within this simulation. All such references can be found from the following sources:

1. Theodore L. Bergman, Adrienne S. Lavine, Frank P. Incropera, David P. De Witt "Fundamentals Of Heat And Mass Transfer", Seventh Edition John Wiley & Sons, Inc. (2011)
2. https://www.engineeringtoolbox.com/
3. https://en.wikipedia.org/wiki/Churchill%E2%80%93Bernstein_equation

## Defaults
The default values are as follows (more detailed descriptions can be found in the next section):
* Water
  * Water and Object starting temperatures: 15.5 °C
  * Water Mass Flow rate: 0.5 kg/s
* Pipes and Storage Tank
  * Exposed to the environment: True (exception: Pipe in Solar Panel: False)  
  * Pipe Length: 4 meters
  * Tank Height: 2 meters
  * Pipe interior diameter: 4 cm
  * Tank interior diameter: 1 m
  * Pipe Wall Thickness: 0.55 mm
  * Tank Wall Thickness: 2 mm
  * Emissivity: 64%
  * Specific heat capacity of Copper (tank and tubes): 376.812 J/(kg * °C)
* Solar Panel
  * Specific heat capacity: 826.23 J/(kg * °C)
  * Emissivity: 93%
  * Thickness: 5 cm
  * Width: 2 m
  * Length: 2 m
  * Ideal Efficiency: 22.5%
  * Efficiency coefficient: 26%

## Override Options
The below table provides all the overrides available to override and set starting values of the system from their default values.

| Override name | Description |
| - | - |
| SIMULATION_DURATION | the duration of the simulation in seconds (e.g. 3600 represents 1 hour) |
| SIMULATION_TIME_STEP | number of (simulation) seconds between data entries to the output file (e.g. 5 outputs at time equals 0, 5, 10, 15, ...) |
| PANEL_WIDTH | the width of the entire solar panel array in meters |
| PANEL_LENGTH | the length of the entire solar panel array in meters  |
| PANEL_TEMPERATURE | starting temperature of the solar panel array in °C |
| PANEL_HEAT_CAPACITY | the (average) specific heat capacity of the solar panel array in J/(kg*C) |
| PANEL_IDEAL_EFFICIENCY | efficiency (represented as a decimal between 0 and 1) of the panel to convert solar irradiation into energy at ideal temperatures |
| PANEL_EMISSIVITY | heat emissivity represented as a number between 0 (perfect reflector) and 1 (perfect emitter) |
| PANEL_THICKNESS | thickness of the solar panel array in meters |
| PANEL_EFFICIENCY_COEFFICIENT | percentage drop per °C above the PANEL_IDEAL_EFFICIENCY max temperature |
| MASS_FLOW_RATE | amount of water that is passing through the entire system (measured in kg/s) |
| WATER_MAX_TEMPERATURE | setting a maximum temperature that water flowing through the system will arbitrarily not exceed in °C |
| WATER_MIN_TEMPERATURE | setting a minimum temperature that water flowing through the system will arbitrarily not exceed in °C |
| TANK_WALL_TEMPERATURE | temperature of the copper wall of the tank in °C |
| TANK_WATER_TEMPERATURE | temperature of the water in the tank in °C |
| TANK_HEAT_CAPACITY | - |
| TANK_EMISSIVITY | - |
| TANK_THICKNESS | - |
| TANK_MASS_FLOW_RATE | amount of water that is passing through the tank (measured in kg/s) |
| TANK_EXPOSED | is the tank exposed to the elements (and thus effected by sun, wind, etc) represented as a 0 (for not exposed) and 1 (for exposed) |
| TANK_PIPE_LENGTH | the length (or height) of the tank in meters |
| TANK_MAX_TEMPERATURE | setting the maximum temperature that water flowing throuh the tank will arbitrarily not exceed in °C |
| TANK_MIN_TEMPERATURE | setting the minimum temperature that water flowing throuh the tank will arbitrarily not exceed in °C |
| TANK_INTERIOR_DIAMETER | the interior diameter of the tank measured in meters (i.e. not including the thickness of the walls) |
| PIPE2TANK_WALL_TEMPERATURE | - |
| PIPE2TANK_WATER_TEMPERATURE | - |
| PIPE2TANK_HEAT_CAPACITY | - |
| PIPE2TANK_EMISSIVITY | - |
| PIPE2TANK_THICKNESS | - |
| PIPE2TANK_MASS_FLOW_RATE | - |
| PIPE2TANK_EXPOSED | - |
| PIPE2TANK_PIPE_LENGTH | - |
| PIPE2TANK_MAX_TEMPERATURE | - |
| PIPE2TANK_MIN_TEMPERATURE | - |
| PIPE2TANK_INTERIOR_DIAMETER | - |
| PIPE2PANEL_WALL_TEMPERATURE | - |
| PIPE2PANEL_WATER_TEMPERATURE | - |
| PIPE2PANEL_HEAT_CAPACITY | - |
| PIPE2PANEL_EMISSIVITY | - |
| PIPE2PANEL_THICKNESS | - |
| PIPE2PANEL_MASS_FLOW_RATE | - |
| PIPE2PANEL_EXPOSED | - |
| PIPE2PANEL_PIPE_LENGTH | - |
| PIPE2PANEL_MAX_TEMPERATURE | - |
| PIPE2PANEL_MIN_TEMPERATURE | - |
| PIPE2PANEL_INTERIOR_DIAMETER | - |
| PIPE_PANEL_WALL_TEMPERATURE | - |
| PIPE_PANEL_WATER_TEMPERATURE | - |
| PIPE_PANEL_HEAT_CAPACITY | - |
| PIPE_PANEL_EMISSIVITY | - |
| PIPE_PANEL_THICKNESS | - |
| PIPE_PANEL_MASS_FLOW_RATE | - |
| PIPE_PANEL_EXPOSED | - |
| PIPE_PANEL_PIPE_LENGTH | - |
| PIPE_PANEL_MAX_TEMPERATURE | - |
| PIPE_PANEL_MIN_TEMPERATURE | - |
| PIPE_PANEL_INTERIOR_DIAMETER | - |

## Known Limitations
This simulation fails to produce accurate results outside of some ranges. 
* Mass flow rates over 2 kg/s,
* Particularly large pipe surface areas, and
* Extreme temperatures where water would freeze or boil
