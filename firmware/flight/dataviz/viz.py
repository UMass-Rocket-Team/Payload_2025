import sys
import pandas as pd
import matplotlib.pyplot as plt

def main():
    if len(sys.argv) < 2:
        print("Usage: python viz.py <datalog.csv>")
        sys.exit(1)
        
    csv_file = sys.argv[1]
    df = pd.read_csv(csv_file)

    print(df.head())
    
    # Plot barometric readings: pressure, temperature, altitude
    plt.figure()
    plt.subplot(3, 1, 1)
    plt.plot(df['timestamp'], df['baro_pressure'], label='Pressure')
    plt.xlabel('Timestamp')
    plt.ylabel('Pressure')
    plt.legend()

    plt.subplot(3, 1, 2)
    plt.plot(df['timestamp'], df['baro_temperature'], label='Temperature')
    plt.xlabel('Timestamp')
    plt.ylabel('Temperature')
    plt.legend()

    plt.subplot(3, 1, 3)
    plt.plot(df['timestamp'], df['baro_altitude'], label='Altitude')
    plt.xlabel('Timestamp')
    plt.ylabel('Altitude')
    plt.legend()
    plt.tight_layout()
    plt.show()

    # Plot accelerometer data
    plt.figure()
    plt.plot(df['timestamp'], df['accel_x'], label='Accel X')
    plt.plot(df['timestamp'], df['accel_y'], label='Accel Y')
    plt.plot(df['timestamp'], df['accel_z'], label='Accel Z')
    plt.xlabel('Timestamp')
    plt.ylabel('Acceleration')
    plt.title('Accelerometer')
    plt.legend()
    plt.show()

    # Plot gyroscope data
    plt.figure()
    plt.plot(df['timestamp'], df['gyro_x'], label='Gyro X')
    plt.plot(df['timestamp'], df['gyro_y'], label='Gyro Y')
    plt.plot(df['timestamp'], df['gyro_z'], label='Gyro Z')
    plt.xlabel('Timestamp')
    plt.ylabel('Angular Velocity')
    plt.title('Gyroscope')
    plt.legend()
    plt.show()

    # Plot magnetometer data
    plt.figure()
    plt.plot(df['timestamp'], df['mag_x'], label='Mag X')
    plt.plot(df['timestamp'], df['mag_y'], label='Mag Y')
    plt.plot(df['timestamp'], df['mag_z'], label='Mag Z')
    plt.xlabel('Timestamp')
    plt.ylabel('Magnetic Field')
    plt.title('Magnetometer')
    plt.legend()
    plt.show()

    # Plot quaternion data
    plt.figure()
    plt.plot(df['timestamp'], df['quat_w'], label='Quat W')
    plt.plot(df['timestamp'], df['quat_x'], label='Quat X')
    plt.plot(df['timestamp'], df['quat_y'], label='Quat Y')
    plt.plot(df['timestamp'], df['quat_z'], label='Quat Z')
    plt.xlabel('Timestamp')
    plt.ylabel('Quaternion')
    plt.title('Orientation (Quaternion)')
    plt.legend()
    plt.show()

    # Plot fusion data: altitude and velocity
    plt.figure()
    plt.subplot(2, 1, 1)
    plt.plot(df['timestamp'], df['fusion_altitude'], label='Fusion Altitude')
    plt.xlabel('Timestamp')
    plt.ylabel('Altitude')
    plt.legend()

    plt.subplot(2, 1, 2)
    plt.plot(df['timestamp'], df['fusion_velocity'], label='Fusion Velocity')
    plt.xlabel('Timestamp')
    plt.ylabel('Velocity')
    plt.legend()
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()
