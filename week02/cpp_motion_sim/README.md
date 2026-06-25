# C++ Robot Motion Simulation

## Overview
This project is a 2D kinematics simulation of a mobile robot written in C++. It started as a simple unicycle model driven by linear velocity (`v`) and angular velocity (`w`), and now also supports a more realistic **differential drive** model driven directly by the left and right wheel speeds.

In the differential drive model, the wheel speeds are converted into the robot's linear and angular velocity using its physical geometry (wheel radius and wheel base):
```text
v = wheel_radius * (omega_right + omega_left) / 2
w = wheel_radius * (omega_right - omega_left) / wheel_base
```
The simulation tracks the robot's pose (`x`, `y`, `theta`), calculates the total distance traveled, and exports the entire trajectory to a CSV file for further analysis or plotting. This design brings the model conceptually closer to a real robot and prepares it for ROS 2 and motor control.

## Build
To compile the project, you can use any standard C++ compiler like `g++`. Run the following command in your terminal:
```bash
g++ main.cpp robot.cpp -o motion_sim
```
## Run
You can run the program in three ways.

### 1. Default mode (differential drive demo tests)
Running without arguments executes three built-in test scenarios that validate the differential drive behavior:

```bash
./motion_sim
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
### 3. Differential drive mode (wheel speeds)
Run with wheel speeds and wheel radius from the command line:

```bash
./motion_sim <omega_left> <omega_right> <dt> <steps> <wheel_radius>
```
Example:
```bash
./motion_sim 5 10 0.1 100 0.05
```
This means:
```text
omega_left   = 5
omega_right  = 10
dt           = 0.1
steps        = 100
wheel_radius = 0.05
wheel_base   = 0.3   (fixed default)
```
## Output
When you run the program, it does the following:

1. Simulates the robot motion using the selected mode.
2. Prints the final pose (`x`, `y`, `theta`) of the robot to the console.
3. Prints the total distance traveled.
4. Generates a trajectory CSV file in the same directory:
   - Unicycle mode → `path.csv`
   - Differential drive mode → `path_wheel.csv`

**Example Console Output:**
```text
Final pose: x=2.30857, y=3.51752, theta=2
Total distance traveled: 5 m
```
**Example CSV Content:**
```csv
step,x,y,theta
0,0,0,0
1,0.05,0,0.02
2,0.09999,0.001,0.04
...
```
## Key Data Structures
```cpp
struct WheelCommand {
double omega_left;
double omega_right;
};

struct RobotGeometry {
double wheel_radius;
double wheel_base;
};
```
## Concepts Practiced
- C++ struct
- C++ class
- constructor overloading
- `std::vector`
- `std::ofstream`
- command-line arguments with `argc` and `argv`
- unicycle motion model
- differential drive kinematics
