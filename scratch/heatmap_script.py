import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# Read the CSV file
df = pd.read_csv('/Users/dalilaoliva/CS243/ns-allinone-3.40/ns-3.40/heatmap1.csv')
# Convert 'AverageDelay' to numeric values
df['AverageDelay'] = pd.to_numeric(df['AverageDelay'], errors='coerce')

# Create a new column 'Twist' based on the row number
df['Twist'] = range(1, len(df) + 1)

# Create a custom 8x8 grid
grid_data = df.pivot_table(index='Twist', columns=["Width", "Length", "Height"], values="AverageDelay")

# Reshape the grid to 8x8
grid_data = grid_data.values.reshape(8, 8)

# Create the heatmap
plt.figure(figsize=(10, 8))
sns.heatmap(grid_data, annot=True, fmt=".2f", cmap="YlGnBu", cbar_kws={'label': 'Average Distance'})
plt.title('Average Delay Heatmap - 2x2x4')
plt.show()
