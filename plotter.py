import pandas as pd
import matplotlib.pyplot as plt

# Load CSV file
file_path = "data_pendulum.csv"   # change if needed
data = pd.read_csv(file_path)

# Extract columns
t = data["time"]
theta1 = data["theta1"]
theta2 = data["theta2"]
omega1 = data["omega1"]
omega2 = data["omega2"]
current = data["current"]
voltage = data["voltage"]

omega1_calc = [0]
for i in range(1, len(theta1)):
    dt = t[i] - t[i-1]
    dtheta = theta1[i] - theta1[i-1]
    omega1_calc.append(dtheta / dt)

# Create figure with multiple subplots
plt.figure(figsize=(12, 10))

# Theta
plt.subplot(2, 1, 1)
#plt.plot(t, theta1, label="theta1")
plt.plot(t, theta2, label="theta2")
plt.ylabel("Angle (rad)")
plt.legend()
plt.grid()

# Omega
plt.subplot(2, 1, 2)
#plt.plot(t, omega1, label="omega1")
#plt.plot(t, omega1_calc, label="omega1_calc")
plt.plot(t, omega2, label="omega2")
plt.ylabel("Angular velocity (rad/s)")
plt.legend()
plt.grid()

# Current + Voltage
#plt.subplot(3, 1, 3)
#plt.plot(t, current, label="current")
#plt.plot(t, voltage, label="voltage")
#plt.xlabel("Time (s)")
#plt.ylabel("Signal")
#plt.legend()
#plt.grid()

plt.tight_layout()
plt.show()