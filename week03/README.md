# C++ Robot Motion Simulation

## Overview
This project is a 2D kinematics simulation of a mobile robot written in C++. It supports both a simple **unicycle model** and a more realistic **differential drive model**.

The project was refactored into an object-oriented design using a `SimulationApp` class. The application logic is now separated from `main.cpp`, making the code easier to extend, test, and prepare for future ROS 2 integration.

In the differential drive model, the wheel speeds are converted into the robot's linear and angular velocity using its physical geometry:
```text
v = wheel_radius * (omega_right + omega_left) / 2
w = wheel_radius * (omega_right - omega_left) / wheel_base
```

The simulation tracks the robot's pose (`x`, `y`, `theta`), calculates path metrics using a dedicated analyzer, and exports the entire trajectory to a CSV file for further analysis or plotting.

---

## Project Structure

```text
├── CMakeLists.txt
├── README.md
├── config_unicycle.txt
├── config_differential.txt
├── include/
│   ├── logger.hpp
│   ├── path_analyzer.hpp
│   ├── robot.hpp
│   └── simulation_app.hpp
└── src/
├── main.cpp
├── path_analyzer.cpp
├── robot.cpp
└── simulation_app.cpp
```

**Key components:**

- `robot.hpp` / `robot.cpp`: Core robot data structures and `MobileRobot` implementation
- `simulation_app.hpp` / `simulation_app.cpp`: Main application class that handles setup, execution mode selection, config parsing, and simulation flow
- `path_analyzer.hpp` / `path_analyzer.cpp`: Static utility methods for trajectory analysis
- `logger.hpp`: Simple logging helper used for console output
- `main.cpp`: Minimal entry point that creates and runs `SimulationApp`
- `config_unicycle.txt`: Example config file for unicycle simulation
- `config_differential.txt`: Example config file for differential drive simulation

---

## Build

This project uses **CMake**.

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

If you are inside the `build/` directory, run the executable as:

```bash
./motion_sim
```

If you are in the project root, run it as:

```bash
./build/motion_sim
```

---

## Run

The program supports four usage modes.

### 1. Default Mode

Running without arguments executes three built-in differential drive demo tests:

```bash
./motion_sim
```

From the project root:

```bash
./build/motion_sim
```

| Test | Wheel speeds | Expected behavior |
|------|--------------|-------------------|
| 1 | `omega_left = omega_right` (`10`, `10`) | Moves nearly straight |
| 2 | `omega_left = -omega_right` (`-5`, `5`) | Rotates nearly in place |
| 3 | `omega_right > omega_left` (`5`, `10`) | Moves in an arc |

---

### 2. Unicycle Mode From CLI

Run with custom unicycle parameters:

```bash
./motion_sim <v> <w> <dt> <steps>
```
Example:

```bash
./motion_sim 0.5 0.2 0.1 100
```

This means:

```text
v     = 0.5
w     = 0.2
dt    = 0.1
steps = 100
```

From the project root:

```bash
./build/motion_sim 0.5 0.2 0.1 100
```

---

### 3. Differential Drive Mode From CLI

Run with wheel speeds, timestep, number of steps, wheel radius, and wheel base:

```bash
./motion_sim <omega_left> <omega_right> <dt> <steps> <wheel_radius> <wheel_base>
```

Example:

```bash
./motion_sim 5 10 0.1 100 0.05 0.3
```

This means:

```text
omega_left   = 5
omega_right  = 10
dt           = 0.1
steps        = 100
wheel_radius = 0.05
wheel_base   = 0.3
```

From the project root:

```bash
./build/motion_sim 5 10 0.1 100 0.05 0.3
```

---

### 4. Config File Mode

The simulation can also be configured using a text file with `key=value` pairs.

Run with:

```bash
./motion_sim config_unicycle.txt
```

or:

```bash
./motion_sim --config config_unicycle.txt
```

From the project root:

```bash
./build/motion_sim config_unicycle.txt
./build/motion_sim --config config_differential.txt
```

---

## Config Files

### Unicycle Config Example

```txt
mode=unicycle
v=1.0
w=0.5
dt=0.1
steps=100
csv=path_from_config.csv
```

Required keys:

```text
mode
v
w
dt
steps
```
Optional key:

```text
csv
```

If `csv` is not provided, the default output file is:

```text
path.csv
```
---

### Differential Drive Config Example

```txt
mode=differential
omega_left=5.0
omega_right=10.0
dt=0.1
steps=100
wheel_radius=0.05
wheel_base=0.3
csv=path_wheel_from_config.csv
```
Required keys:

```text
mode
omega_left
omega_right
dt
steps
```

Optional keys:

```text
wheel_radius
wheel_base
csv
```

Default values:

```text
wheel_radius = 0.05
wheel_base   = 0.3
csv          = path_wheel.csv
```

---

## Output

When the program runs, it:

1. Simulates the robot motion using the selected mode.
2. Prints a summary to the console.
3. Saves the trajectory to a CSV file.

The console summary includes:

- Final pose (`x`, `y`, `theta`)
- Total distance traveled
- Net displacement
- Final heading in degrees
- Maximum distance from origin

### CSV Files By Mode

- **Unicycle CLI mode**
  - `path.csv`

- **Differential drive CLI mode**
  - `path_wheel.csv`

- **Config file mode**
  - Uses the `csv` value from the config file if provided
  - Otherwise uses the mode default

- **Default test mode**
  - `path_wheel_straight.csv`
  - `path_wheel_rotate.csv`
  - `path_wheel_arc.csv`

Example console output:

```text
[INFO] Final pose: x=2.30857, y=3.51752, theta=2 rad (114.592 deg)
Total distance traveled: 5 m
Net displacement: 4.20743 m
Final heading in degrees: 114.592 deg
Maximum distance from origin: 4.20743 m
```

Example CSV content:

```csv
step,x,y,theta
0,0,0,0
1,0.05,0,0.02
2,0.09999,0.001,0.04
...
```

---

## Key Data Structures

```cpp
struct Pose {
double x;
double y;
double theta;
};

struct WheelCommand {
double omega_left;
double omega_right;
};

struct RobotGeometry {
double wheel_radius;
double wheel_base;
};

struct WheelSimulationConfig {
WheelCommand command;
double dt;
int steps;
};

struct SimulationConfig {
double v;
double w;
double dt;
int steps;
};
```

---

## Object-Oriented Design

The application is organized around the `SimulationApp` class.

`main.cpp` is intentionally minimal. It creates the app, calls `setup()`, and then runs `execute()`.

```cpp
int main(int argc, char* argv[]) {
    try {
        auto app = std::make_unique<SimulationApp>();
        app->setup(argc, argv);
        app->execute();
    }
    catch (const std::exception& e) {
        Logger::error(e.what());
        return 1;
    }

    return 0;
}
```

This keeps the program entry point simple and moves simulation-specific behavior into a dedicated class.

---

## Concepts Practiced

- C++ struct
- C++ class
- Object-oriented application design
- Constructor initialization lists
- `std::shared_ptr`
- `std::vector`
- `std::ofstream`
- `std::ifstream`
- `std::unordered_map`
- Command-line arguments with `argc` and `argv`
- Exception handling with `try` / `catch`
- Simple config file parsing
- Unicycle motion model
- Differential drive kinematics
- Path analysis
- Static utility classes
- `const` correctness and pass-by-reference
- CSV export