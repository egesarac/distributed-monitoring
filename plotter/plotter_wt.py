import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

sns.set_palette("colorblind")

# Data extracted from the table
eps_values = [0.05, 0.1, 0.2, 0.4]
# eps_values = [1, 2, 4, 8]
# n_values = [2]
n_values = [2, 3, 4]
methods = ['EDM', 'ADM', 'ADM-F', 'ADM-FR', 'ADM-C', 'ADM-CR']
markers = ['o', 's', 'D', '^', 'v', 'P']  # Different shapes for each method

data = {
    2: {
        'EDM': [9.3985,10.5139,13.7389,18.4945],
        'ADM': [0.0180,5.8773,'oot','oot'],
        'ADM-F': [0.0017,0.0137,6.2907,'oot'],
        'ADM-FR': [0.0006,0.0022,0.0132,2.2280],
        'ADM-C': [0.0006,0.0026,0.0156,2.5440],
        'ADM-CR': [0.0005,0.0018,0.0087,1.1977],
    },
    3: {
        'EDM': [22.2457,30.3284,44.4894,49.5030],
        'ADM': [1.2092,'oot','oot','oot'],
        'ADM-F': [0.0014,0.0201,6.6749,'oot'],
        'ADM-FR': [0.0005,0.0028,0.0755,20.7458],
        'ADM-C': [0.0003,0.0012,0.0205,3.4808],
        'ADM-CR': [0.0002,0.0009,0.0134,2.1409],
    },
    4: {
        'EDM': [37.4117,52.0112,84.5577,102.8766],
        'ADM': ['oot','oot','oot','oot'],
        'ADM-F': [0.0031,0.0436,7.5063,'oot'],
        'ADM-FR': [0.0012,0.0103,0.4119,'oot'],
        'ADM-C': [0.0004,0.0016,0.0268,4.3823],
        'ADM-CR': [0.0003,0.0012,0.0198,3.0741],
    }
}

speedups = {
    2: {
        'ADM-F/ADM-C': [2.73, 5.27, 403.67, '-'],
        'ADM-C/ADM-CR': [1.24, 1.47, 1.79, 2.12],
        'EDM/ADM-CR': [18229, 5909, 1576, 15],
    },
    3: {
        'ADM-F/ADM-C': [4.83, 16.67, 324.86, '-'],
        'ADM-C/ADM-CR': [1.19, 1.41, 1.63, '-'],
        'EDM/ADM-CR': [93558, 35422, 3328, 23],

    },
    4: {
        'ADM-F/ADM-C': [8.71, 27.82, 280.25, '-'],
        'ADM-C/ADM-CR': [1.14, 1.28, 1.35, 1.43],
        'EDM/ADM-CR': [118471, 42383, 4273, 33],
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
    #     spd = f'Speedups:\nADM-F/ADM-C:{speedups[n]["ADM-F/ADM-C"][eps_values.index(eps)]}\nADM-C/ADM-CR:{speedups[n]["ADM-C/ADM-CR"][eps_values.index(eps)]}\nEDM/ADM-CR: {speedups[n]["EDM/ADM-CR"][eps_values.index(eps)]}'
    #     ax.text(eps+0.0025, 10000, spd, rotation=0, fontsize=10, verticalalignment='top')

    ax.set_xlabel('ε (seconds)', fontsize=12)  # Adjust the font size as needed
    if (n == 2):
        ax.set_ylabel('Time (seconds)', fontsize=12)   # Adjust the font size as needed
    ax.set_title(f'n={n}', fontsize=14)  # Adjust the font size as needed
    # ax.set_xscale('log')
    ax.set_yscale('log')
    ax.set_ylim(bottom=0, top=2e2)  # Adjust these values as needed
    ax.set_xlim(left=min(eps_values) * 0.9, right=max(eps_values) * 1.1)
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
