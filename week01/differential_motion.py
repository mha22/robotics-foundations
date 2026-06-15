import math
import matplotlib.pyplot as plt

x = 0
y = 0
theta = 0
v = 0.5
w = 0.2
dt = 0.1

def robot_position(x, y, theta, v, w, dt):
    x_new = x + v * math.cos(theta) * dt
    y_new = y + v * math.sin(theta) * dt
    theta = theta + w * dt
    return x_new, y_new, theta


path  = [(x, y, theta)]
for _ in range(100):
    x, y, theta = robot_position(x, y, theta, v, w, dt)
    path.append((x, y, theta))

x_vals = [p[0] for p in path ]
y_vals = [p[1] for p in path ]



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
print(f"saved, final pose: x={x:.3f}, y={y:.3f}, theta={theta:.3f}")
