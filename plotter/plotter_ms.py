import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

sns.set_palette("colorblind")

# Data extracted from the table
eps_values = [0.05, 0.1, 0.15, 0.2, 0.25]
n_values = [2, 3, 4]
methods = ['EDM', 'ADM', 'ADM-F', 'ADM-FR']
markers = ['o', 's', 'D', '^']  # Different shapes for each method

data = {
    2: {
        'EDM': [2.8790, 3.9270, 5.1310, 5.6570, 5.9100],
        'ADM': [120.1700, 'oot', 'oot', 'oot', 'oot'],
        'ADM-F': [0.0040, 0.0150, 0.1930, 3.0080, 34.0920],
        'ADM-FR': [0.0010, 0.0020, 0.0040, 0.0080, 0.0220]
    },
    3: {
        'EDM': [14.3080, 22.8270, 46.8330, 40.9270, 45.9320],
        'ADM': [246.7660, 'oot', 'oot', 'oot', 'oot'],
        'ADM-F': [0.0030, 0.0300, 0.4050, 6.0930, 71.5560],
        'ADM-FR': [0.0010, 0.0020, 0.0050, 0.0160, 0.0450]
    },
    4: {
        'EDM': [71.8530, 156.8130, 207.8540, 241.7530, 343.3220],
        'ADM': ['oot', 'oot', 'oot', 'oot', 'oot'],
        'ADM-F': [0.0040, 0.0460, 0.6160, 9.2910, 106.7400],
        'ADM-FR': [0.0010, 0.0030, 0.0080, 0.0250, 0.0700]
    }
}

speedups = {
    2: {
        'ADM-F/ADM-FR': [round(5.865, 2), round(6.217, 2), round(51.757, 2), round(378.285, 1), 1562],
        'EDM/ADM-FR': [3835, 1595, 1373, round(711.517, 1), round(270.853, 1)]
    },
    3: {
        'ADM-F/ADM-FR': [round(4.986, 2), round(17.334, 2), round(75.857, 2), round(373.532, 1), 1585],
        'EDM/ADM-FR': [24250, 12997, 8779, 2509, 1017]
    },
    4: {
        'ADM-F/ADM-FR': [round(4.787, 2), round(17.298, 2), round(76.617, 2), round(370.993, 2), round(1527.183, 2)],
        'EDM/ADM-FR': [78421, 59032, 25855, 9653, 4912]
    }
}

# Function to plot the data
def plot_data(n, data, ax):
    for method, marker in zip(methods, markers):
        y_values = data[n][method]
        y_values = [float(y) if y != 'oot' else np.nan for y in y_values]  # Convert 'oot' to NaN for plotting
        ax.plot(eps_values, y_values, label=method, marker=marker, linestyle='-')

    # # Add vertical lines for each epsilon value
    # for eps in eps_values:
    #     ax.axvline(x=eps, color='black', linestyle='-', linewidth=1)
    #     spd = f'Speedups:\nADM-F/ADM-FR:{speedups[n]['ADM-F/ADM-FR'][eps_values.index(eps)]}\nEDM/ADM-FR:{speedups[n]['EDM/ADM-FR'][eps_values.index(eps)]}'
    #     ax.text(eps + 0.0025, 5000, spd, rotation=0, fontsize=6, verticalalignment='top')

    ax.set_xlabel('ε (seconds)', fontsize=12)  # Adjust the font size as needed
    if n == 2:
        ax.set_ylabel('Time (seconds)', fontsize=12)  # Adjust the font size as needed
    ax.set_title(f'n={n}', fontsize=14)  # Adjust the font size as needed
    # ax.set_xscale('log')
    ax.set_yscale('log')
    ax.set_ylim(bottom=0, top=1e3)  # Adjust these values as needed
    ax.set_xlim(left=min(eps_values) * 0.9, right=max(eps_values) * 1.1)
    ax.set_xticks(eps_values)
    ax.set_xticklabels(eps_values)
    # ax.legend(fontsize=8, loc='lower right')  # Adjust the font size of the legend
    ax.grid(True, which='both', linestyle='--', linewidth=0.5)


# Create a single figure with multiple subplots arranged in a grid
fig, axes = plt.subplots(1, len(n_values), figsize=(12, 4))  # Adjust the figsize as needed and specify the number of columns

# Collect handles and labels for the legend
handles, labels = [], []

# Plot data for each n value
for i, n in enumerate(n_values):
    ax = axes[i]  # Get the current axis for the subplot
    plot_data(n, data, ax)  # Plot data on the current subplot
    if i == 0:
        handles, labels = ax.get_legend_handles_labels()  # Collect handles and labels from the first subplot

# Create a single legend
# Add the legend to the rightmost subplot
axes[-1].legend(handles, labels, loc='lower right', fontsize=8)

plt.tight_layout(rect=[0, 0, 1, 0.95])  # Adjust subplot parameters to give specified padding and room for the legend
plt.show()
