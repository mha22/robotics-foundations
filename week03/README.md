# C++ Robot Motion Simulation

## Overview
This project is a 2D kinematics simulation of a mobile robot written in C++. It started as a simple unicycle model driven by linear velocity (`v`) and angular velocity (`w`), and now also supports a more realistic **differential drive** model driven directly by the left and right wheel speeds.

In the differential drive model, the wheel speeds are converted into the robot's linear and angular velocity using its physical geometry (wheel radius and wheel base):
```text
v = wheel_radius * (omega_right + omega_left) / 2
w = wheel_radius * (omega_right - omega_left) / wheel_base
```
The simulation tracks the robot's pose (`x`, `y`, `theta`), calculates path metrics using a dedicated analyzer, and exports the entire trajectory to a CSV file for further analysis or plotting. This design brings the model conceptually closer to a real robot and prepares it for ROS 2 and motor control.

---

## Project Structure

```text

├── CMakeLists.txt
├── README.md
├── include/
│   ├── mobile_robot.hpp
│   ├── path_analyzer.hpp
│   └── types.hpp
└── src/
├── main.cpp
├── mobile_robot.cpp
├── path_analyzer.cpp
└── robot.cpp

```

**Key components:**
- `types.hpp`: Core data structures (`Pose`, `WheelCommand`, `RobotGeometry`, etc.)
- `mobile_robot.hpp/cpp`: Robot state and motion models
- `path_analyzer.hpp/cpp`: Static methods for trajectory analysis (distance, displacement, heading)
- `robot.cpp`: Simulation runner and output formatting
- `main.cpp`: Entry point and CLI argument handling

---

## Build

This project uses **CMake**.

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```
---

## Run

You can run the program in three ways.

### 1. Default mode (differential drive demo tests)
Running without arguments executes three built-in test scenarios that validate the differential drive behavior:

```bash
./motion_sim
```

If you are running from the project root instead of the `build/` directory, use:

```bash
./build/motion_sim
```

| Test | Wheel speeds | Expected behavior |
|------|--------------|-------------------|
| 1    | `omega_left = omega_right` (10, 10)  | moves (nearly) straight |
| 2    | `omega_left = -omega_right` (-5, 5)  | rotates (nearly) in place |
| 3    | `omega_right > omega_left` (5, 10)   | curves to one side |

### 2. Unicycle mode (`v`, `w`)
Run with custom velocity parameters from the command line:

```bash
./motion_sim <v> <w> <dt> <steps>
```

Example:

```bash
./motion_sim 0.5 0.2 0.1 100
```

This means:

```text
v = 0.5
w = 0.2
dt = 0.1
steps = 100
```

If running from the project root:

```bash
./build/motion_sim 0.5 0.2 0.1 100
```

### 3. Differential drive mode (wheel speeds)
Run with wheel speeds, wheel radius, and wheel base from the command line:

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

If running from the project root:

```bash
./build/motion_sim 5 10 0.1 100 0.05 0.3
```

---

## Output

When you run the program, it does the following:

1. Simulates the robot motion using the selected mode.
2. Prints a detailed summary to the console showing:
   - Final pose ($x$, $y$, $\theta$ in radians and degrees)
   - Total distance traveled (accumulated arc length)
   - Net displacement (straight-line distance from start to end)
   - Final heading in degrees
   - Maximum distance from origin
3. Generates a trajectory CSV file.

### CSV files by mode
- **Unicycle mode**
  - `path.csv`

- **Differential drive mode (custom CLI run)**
  - `path_wheel.csv`

- **Default test mode**
  - `path_wheel_straight.csv`
  - `path_wheel_rotate.csv`
  - `path_wheel_arc.csv`

**Example Console Output:**

```text
Final pose: x=2.30857, y=3.51752, theta=2 rad (114.592 deg)
Total distance traveled: 5 m
Net displacement: 4.20743 m
Final heading in degrees: 114.592 deg
Maximum distance from origin: 4.20743 m
```

**Example CSV Content:**

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
```

---

## Concepts Practiced

- C++ struct
- C++ class
- Constructor overloading
- `std::vector`
- `std::ofstream`
- Command-line arguments with `argc` and `argv`
- Unicycle motion model
- Differential drive kinematics
- Path analysis (distance, displacement, heading)
- Static utility classes
- `const` correctness and pass-by-reference
- Basic file export to CSV
