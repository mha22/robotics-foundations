# C++ Robot Motion Simulation

## Overview
This project is a simple 2D kinematics simulation of a mobile robot written in C++. It models the robot's movement over time given linear velocity ($v$) and angular velocity ($w$). The simulation tracks the robot's pose ($x$, $y$, $\theta$), calculates the total distance traveled, and exports the entire trajectory to a CSV file for further analysis or plotting.

## Build
To compile the project, you can use any standard C++ compiler like `g++`. Run the following command in your terminal:
```bash
g++ main.cpp mobile_robot.cpp -o motion_sim
```

## Run
After a successful build, you can run the executable:

```bash
./motion_sim
```
## Output
When you run the program, it performs the simulation for 100 steps and does the following:
1. Prints the final pose ($x$, $y$, $\theta$) of the robot to the console.
2. Prints the total distance traveled.
3. Generates a `path.csv` file in the same directory containing the step-by-step trajectory.

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
* C++ struct
* C++ class
* std::vector
* std::ofstream
* basic robot motion model
