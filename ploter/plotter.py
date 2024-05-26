import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from matplotlib import colormaps

filename = '../results/GAnd.csv'

df = pd.read_csv(filename)

fig, (ax2, ax3) = plt.subplots(1, 2)


d_header = [32, 16, 8, 4]
eps_header = [1, 2, 4, 8]

data_speedup = [
        [56623.38521, 34646.87393, 15870.04447, 6404.107085],
        [28936.10854, 17759.31089, 8803.134475, 4905.198941],
        [16327.5081, 10739.62168, 7090.823162, 6969.752278],
        [9521.199206, 9204.74636, 8097.44811, float('nan')]
        ]

data_speedup_label = [
        ['251us\n14.24s', '424us\n14.69s', '942us\n14.95s', '2.52s\n15.88s'],
        ['122us\n3.52s', '203us\n3.60s', '434us\n3.72s', '902us\n4.04s'],
        ['60us\n981ms', '95us\n1011ms', '154us\n1086ms', '175us\n1204ms'],
        ['32us\n307ms', '35us\n318ms', '43us\n348ms', 'na']
        ]

data_fp = [
        [0, 0, 7, 7],
        [0, 2, 6, 6],
        [0, 0, 4, 4],
        [0, 0, 3, float('nan')]
        ]

sns.heatmap(data_speedup, cmap='GnBu', annot=data_speedup_label, linewidth=0.5, ax=ax2, fmt='')

ax2.set(xlabel="eps", ylabel="duration")
ax2.set_xticks([0.5, 1.5, 2.5, 3.5], labels=eps_header)
ax2.set_yticks([0.5, 1.5, 2.5, 3.5], labels=d_header)
ax2.set_title('Speed-up (times) ADM vs. EDM')

sns.heatmap(data_fp, cmap='GnBu', annot=data_fp, linewidth=0.5, ax=ax3)

ax3.set(xlabel="eps")
ax3.set_xticks([0.5, 1.5, 2.5, 3.5], labels=eps_header)
ax3.set_yticks([0.5, 1.5, 2.5, 3.5], labels=d_header)
ax3.set_title('False Positives (%)')

plt.show()