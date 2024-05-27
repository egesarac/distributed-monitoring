import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

sns.set_palette("colorblind")

# Data extracted from the table
eps_values = [0.05, 0.1, 0.2, 0.4]
# eps_values = [1, 2, 4, 8]
# n_values = [2]
n_values = [2, 3, 4]
methods = ['SMT', 'ORIG', 'FINE', 'F-REL', 'COARSE', 'C-REL']
markers = ['o', 's', 'D', '^', 'v', 'P']  # Different shapes for each method

data = {
    2: {
        'SMT': [9.3985,10.5139,13.7389,18.4945],
        'ORIG': [0.0180,5.8773,'oot','oot'],
        'FINE': [0.0017,0.0137,6.2907,'oot'],
        'F-REL': [0.0006,0.0022,0.0132,2.2280],
        'COARSE': [0.0006,0.0026,0.0156,2.5440],
        'C-REL': [0.0005,0.0018,0.0087,1.1977],
    },
    3: {
        'SMT': [22.2457,30.3284,44.4894,49.5030],
        'ORIG': [1.2092,'oot','oot','oot'],
        'FINE': [0.0014,0.0201,6.6749,'oot'],
        'F-REL': [0.0005,0.0028,0.0755,20.7458],
        'COARSE': [0.0003,0.0012,0.0205,3.4808],
        'C-REL': [0.0002,0.0009,0.0134,2.1409],
    },
    4: {
        'SMT': [37.4117,52.0112,84.5577,102.8766],
        'ORIG': ['oot','oot','oot','oot'],
        'FINE': [0.0031,0.0436,7.5063,'oot'],
        'F-REL': [0.0012,0.0103,0.4119,'oot'],
        'COARSE': [0.0004,0.0016,0.0268,4.3823],
        'C-REL': [0.0003,0.0012,0.0198,3.0741],
    }
}

speedups = {
    2: {
        'FINE/COARSE': [2.73, 5.27, 403.67, '-'],
        'COARSE/C-REL': [1.24, 1.47, 1.79, 2.12],
        'SMT/C-REL': [18229, 5909, 1576, 15],
    },
    3: {
        'FINE/COARSE': [4.83, 16.67, 324.86, '-'],
        'COARSE/C-REL': [1.19, 1.41, 1.63, '-'],
        'SMT/C-REL': [93558, 35422, 3328, 23],

    },
    4: {
        'FINE/COARSE': [8.71, 27.82, 280.25, '-'],
        'COARSE/C-REL': [1.14, 1.28, 1.35, 1.43],
        'SMT/C-REL': [118471, 42383, 4273, 33],
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
    #     spd = f'Speedups:\nFINE/COARSE:{speedups[n]["FINE/COARSE"][eps_values.index(eps)]}\nCOARSE/C-REL:{speedups[n]["COARSE/C-REL"][eps_values.index(eps)]}\nSMT/C-REL: {speedups[n]["SMT/C-REL"][eps_values.index(eps)]}'
    #     ax.text(eps+0.0025, 10000, spd, rotation=0, fontsize=10, verticalalignment='top')

    ax.set_xlabel('ε (in seconds)', fontsize=12)  # Adjust the font size as needed
    if (n == 2):
        ax.set_ylabel('Time (seconds)', fontsize=12)   # Adjust the font size as needed
    ax.set_title(f'n={n}', fontsize=14)  # Adjust the font size as needed
    ax.set_xscale('log')
    ax.set_yscale('log')
    ax.set_ylim(bottom=0, top=2e2)  # Adjust these values as needed
    ax.set_xlim(left=min(eps_values) * 0.9, right=max(eps_values) * 1.5)
    ax.set_xticks(eps_values)
    ax.set_xticklabels(eps_values)
    # ax.legend(fontsize=12, loc='lower right')  # Adjust the font size of the legend
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
