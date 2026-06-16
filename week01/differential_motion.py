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


def plot_path(path):
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
    plt.savefig('output.png', dpi=150)


def main():
    x, y, theta = 0, 0, 0
    v, w = 0.5, 0.2
    dt = 0.1
    steps = 100
    path, final_pose = simulate_motion(x, y, theta, dt, v, w, steps)
    plot_path(path)

    fx, fy, ftheta = final_pose
    print(f"saved, final pose: x={fx:.3f}, y={fy:.3f}, theta={ftheta:.3f}")


if __name__ == "__main__":
    main()
