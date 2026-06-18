
# Robot Motion Simulation

## What is this project?

This project simulates the motion of a differential drive robot in a 2D plane.
The robot moves with a given linear velocity and angular velocity, and at
each time step its new position is computed. At the end, the full
trajectory is plotted using `matplotlib` and saved as an image.

## Project Structure

```text
├── differential_motion.py    # Main simulation script
├── README.md                  # This file
└── output.png                 # Default output (generated)
```

The code consists of:
- `robot_position()`: Updates robot state for one time step
- `simulate_motion()`: Runs simulation and builds trajectory
- `compute_total_distance()`: Sums distances between path points
- `plot_path()`: Generates and saves trajectory plot
- `main()`: Parses arguments and orchestrates execution

## Robot Motion Equations

The robot's state is described by three values: $x$, $y$, and the heading
angle $\theta$. At each time step $dt$, given the linear velocity $v$ and
angular velocity $w$, the new values are computed as follows:

$$x_{new} = x + v \cdot \cos(\theta) \cdot dt$$

$$y_{new} = y + v \cdot \sin(\theta) \cdot dt$$

$$\theta_{new} = \theta + w \cdot dt$$

## How to run

1. Install the requirements:
```bash
pip install matplotlib
```
2. Run the program with custom parameters:

```bash
python differential_motion.py --x 0 --y 0 --theta 0 --v 0.5 --w 0.2 --dt 0.1 --steps 100 --out trajectory.png
```
## Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| `--x`     | `0`     | Initial x position (meters) |
| `--y`     | `0`     | Initial y position (meters) |
| `--theta` | `0`     | Initial heading (radians) |
| `--v`     | `0.5`   | Linear velocity (m/s) |
| `--w`     | `0.2`   | Angular velocity (rad/s) |
| `--dt`    | `0.1`   | Time step (seconds) |
| `--steps` | `100`   | Number of simulation steps |
| `--out`   | `output.png` | Output filename |

## Example Commands

### Straight motion ($w = 0$)
```bash
python differential_motion.py --x 0 --y 0 --theta 0 --v 0.5 --w 0 --dt 0.1 --steps 100 --out straight.png
```
### Gentle turn ($w = 0.2$)
```bash
python differential_motion.py --x 0 --y 0 --theta 0 --v 0.5 --w 0.2 --dt 0.1 --steps 100 --out gentle_turn.png
```
### Sharp turn ($w = 0.5$)
```bash
python differential_motion.py --x 0 --y 0 --theta 0 --v 0.5 --w 0.5 --dt 0.1 --steps 100 --out sharp_turn.png
```
The turning radius is given by $r = v/w$. Higher $w$ produces tighter curves.

## Observations

- وقتی $w = 0$ باشد، مسیر مستقیم است.
- هرچه $w$ بیشتر شود، انحنای مسیر بیشتر می‌شود.
- $v$ تعیین می‌کند ربات در هر گام چه‌قدر جلو برود.
