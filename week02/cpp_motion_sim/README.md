# C++ Robot Motion Simulation

## Overview
This project is a simple 2D kinematics simulation of a mobile robot written in C++. It models the robot's movement over time given linear velocity (`v`) and angular velocity (`w`). The simulation tracks the robot's pose (`x`, `y`, `theta`), calculates the total distance traveled, and exports the entire trajectory to a CSV file for further analysis or plotting.

## Build
To compile the project, you can use any standard C++ compiler like `g++`. Run the following command in your terminal:
```bash
g++ main.cpp mobile_robot.cpp -o motion_sim
```
## Run
You can run the program in two ways.

Run with default parameters:

```bash
./motion_sim
```
Default values:

```text
v = 0.5
w = 0.2
dt = 0.1
steps = 100
```
Run with custom parameters from the command line:

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
Usage:

```text
./motion_sim <v> <w> <dt> <steps>
```
## Output
When you run the program, it does the following:

1. Simulates the robot motion using either the default parameters or the values passed from the command line.
2. Prints the final pose (`x`, `y`, `theta`) of the robot to the console.
3. Prints the total distance traveled.
4. Generates a `path.csv` file in the same directory containing the step-by-step trajectory.

**Example Console Output:**
```text
Final pose: x=2.30857, y=3.51752, theta=2
Total distance traveled: 5 m
```
**Example `path.csv` Content:**
```csv
step,x,y,theta
0,0,0,0
1,0.05,0,0.02
2,0.09999,0.001,0.04
...
```
## Concepts Practiced
- C++ struct
- C++ class
- `std::vector`
- `std::ofstream`
- command-line arguments with `argc` and `argv`
- basic robot motion model
