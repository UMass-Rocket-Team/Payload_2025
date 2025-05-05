import numpy as np
import matplotlib.pyplot as plt
import csv
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.animation import FuncAnimation

keys = ['timestamp', 'baro_pressure', 'baro_temperature', 'baro_altitude', 'accel_x', 'accel_y', 'accel_z', 'gyro_x', 'gyro_y', 'gyro_z', 'mag_x', 'mag_y', 'mag_z', 'quat_w', 'quat_x', 'quat_y', 'quat_z', 'fusion_altitude', 'fusion_velocity', 'batt_voltage']

dout = {}

with open('log_f_p1.csv', 'r') as f:
    reader = csv.reader(f.readlines(), delimiter=',')

    element_dict = {}

    for row in reader:

        if row[0].isalpha():
            for eidx, element in enumerate(row):
                element_dict[element] = eidx
                dout[element] = []
            #  print(element_dict.keys())
        else:
            for eidx, element in enumerate(row):
                dout[keys[eidx]].append(float(element))

acc_x = dout['accel_x']
acc_y = dout['accel_y']
acc_z = dout['accel_z']
timevar = np.asarray(dout['timestamp']) / 1e6
acc_magnitude = np.sqrt(np.array(acc_x)**2 + np.array(acc_y)**2 + np.array(acc_z)**2)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

def update_quat(num, data, line):
    ax.clear()
    ax.quiver(0, 0, 0, data[0, num], data[1, num], data[2, num], length=1.0)
    ax.set_xlim([-1, 1])
    ax.set_ylim([-1, 1])
    ax.set_zlim([-1, 1])
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title('Orientation Animation')

quats = np.array([dout['quat_w'], dout['quat_x'], dout['quat_y'], dout['quat_z']])
orientations = np.zeros((3, len(quats[0])))

for i in range(len(quats[0])):
    w, x, y, z = quats[:, i]
    orientations[:, i] = [2*(x*z + w*y), 2*(y*z - w*x), 1 - 2*(x**2 + y**2)]

line, = ax.plot([], [], [], lw=2)
ani = FuncAnimation(fig, update_quat, frames=len(timevar), fargs=(orientations, line), interval=50)

plt.show()

def quat_to_euler(w, x, y, z):
    t0 = +2.0 * (w * x + y * z)
    t1 = +1.0 - 2.0 * (x * x + y * y)
    roll_x = np.arctan2(t0, t1)

    t2 = +2.0 * (w * y - z * x)
    t2 = +1.0 if t2 > +1.0 else t2
    t2 = -1.0 if t2 < -1.0 else t2
    pitch_y = np.arcsin(t2)

    t3 = +2.0 * (w * z + x * y)
    t4 = +1.0 - 2.0 * (y * y + z * z)
    yaw_z = np.arctan2(t3, t4)

    return roll_x, pitch_y, yaw_z

euler_angles = np.zeros((3, len(quats[0])))

for i in range(len(quats[0])):
    w, x, y, z = quats[:, i]
    euler_angles[:, i] = quat_to_euler(w, x, y, z)

euler_angles_deg = np.degrees(euler_angles)

plt.figure()
plt.plot(timevar, euler_angles_deg[0], label='Roll')
plt.plot(timevar, euler_angles_deg[1], label='Pitch')
plt.plot(timevar, euler_angles_deg[2], label='Yaw')
plt.legend()
plt.xlabel('Time (s)')
plt.ylabel('Angle (degrees)')
plt.title('Euler Angles over Time')

plt.figure()

plt.plot(timevar, dout['gyro_x'])
plt.plot(timevar, dout['gyro_y'])
plt.plot(timevar, dout['gyro_z'])


# plt.figure()
# plt.plot(timevar)
plt.figure()
plt.plot(timevar, dout['fusion_altitude'])
plt.figure()
plt.plot(timevar, acc_magnitude)

plt.show()