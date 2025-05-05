import pandas as pd

import matplotlib.pyplot as plt

# Load the data
data = pd.read_csv('timing_log.csv')

# Assuming the CSV has columns 'time' and 'value'
time = data['Received Time (s)']
value = data['Decoded Time (us)']

# Calculate the slope
slope = (value.iloc[-1] - value.iloc[0]) / (time.iloc[-1] - time.iloc[0])
print(f"Slope: {slope}")

# Create the plot
plt.figure(figsize=(10, 6))
plt.plot(time, value, marker='o')

# Add titles and labels
plt.title('Timing Log Data')
plt.xlabel('Time')
plt.ylabel('Value')

# Show the plot
plt.grid(True)
plt.show()