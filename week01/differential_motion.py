import argparse
import math
import matplotlib.pyplot as plt


def robot_position(x, y, theta, v, w, dt):
    x_new = x + v * math.cos(theta) * dt
    y_new = y + v * math.sin(theta) * dt
    theta = theta + w * dt
    return x_new, y_new, theta


def simulate_motion(x, y, theta, dt, v, w, steps):
    path = [(x, y, theta)]
    for _ in range(steps):
        x, y, theta = robot_position(x, y, theta, v, w, dt)
        path.append((x, y, theta))
    final_pose = (x, y, theta)
    return path, final_pose


def compute_total_distance(path):
    total = 0
    for i in range(0, len(path) - 1):
        x0, y0, _ = path[i]
        x1, y1, _ = path[i + 1]
        total += math.hypot(x1 - x0, y1 - y0)
    return total


def plot_path(path, output):
    x_vals = [p[0] for p in path]
    y_vals = [p[1] for p in path]

    plt.figure(figsize=(8, 6))
    plt.plot(x_vals, y_vals, 'b-', linewidth=2, label='robot path')
    plt.scatter(x_vals[0], y_vals[0], color='green', label='start point', zorder=5)
    plt.scatter(x_vals[-1], y_vals[-1], color='red', label='end point', zorder=5)
    plt.xlabel('X', fontsize=12)
    plt.ylabel('Y', fontsize=12)
    plt.title('Robot path', fontsize=14)
    plt.legend()
    plt.grid(True)
    plt.axis('equal')
    plt.savefig(output, dpi=150)
    plt.close()


def main():
    # x, y, theta = 0, 0, 0
    # v, w = 0.5, 0.2
    # dt = 0.1
    # steps = 100
    parser = argparse.ArgumentParser(description="Basic differential drive motion simulation")

    parser.add_argument("--x", type=float, default=0, help="Initial x position of the robot")
    parser.add_argument("--y", type=float, default=0, help="Initial y position of the robot")
    parser.add_argument("--theta", type=float, default=0, help="Initial robot heading angle in radians")
    parser.add_argument("--dt", type=float, default=0.1, help="Time step for the simulation in seconds")
    parser.add_argument("--v", type=float, default=0.5, help="Linear velocity of the robot in m/s")
    parser.add_argument("--w", type=float, default=0.2, help="Angular velocity of the robot in rad/s")
    parser.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    parser.add_argument("--out", type=str, default="output.png", help="Output image file name")


    args = parser.parse_args()

    path, final_pose = simulate_motion(args.x, args.y, args.theta, args.dt, args.v, args.w, args.steps)
    plot_path(path, args.out)

    fx, fy, ftheta = final_pose

    total_time = args.dt * args.steps
    total_distance = compute_total_distance(path)
    final_heading  = ftheta

    print(f"Final pose: x={fx:.3f}, y={fy:.3f}, theta={ftheta:.3f}")
    print(f"Total distance traveled: {total_distance:.3f} m")
    print(f"Final heading: {math.degrees(final_heading):.3f} deg")

if __name__ == "__main__":
    main()
