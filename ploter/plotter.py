import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
from matplotlib import colormaps

# filenames
f1 = '../results/gand.csv'
f2 = '../results/gor.csv'
f3 = '../results/gimpf.csv'
f4 = '../results/gimpft1.csv'

dtype = {"d": int, "eps": int, "EDM": float, "ADM": float, "FP": float,"speedup": float, "label": str}

fs = [f1, f2, f3, f4]


fig, axs = plt.subplots(len(fs), 2)
fig.set_size_inches([8, 10])

for i in range(len(fs)):
    df = pd.read_csv(fs[i], dtype=dtype)

    data_fp = df.pivot_table('FP', 'eps', 'd', fill_value=float('nan'), dropna=False)
    data_speedup = df.pivot_table('speedup', 'eps', 'd', fill_value=float('nan'), dropna=False)
    data_speedup_label = df.pivot_table(values='label', index='eps', columns='d', aggfunc=lambda x: ''.join(str(v) for v in x),
                                    fill_value='')
    data_speedup_label = data_speedup_label.applymap(lambda x: x.replace('\\n', '\n'))

    sns.heatmap(data_speedup, cmap='GnBu', annot=data_speedup_label, linewidth=0.5, ax=axs[i][0], fmt='', vmin=0, vmax=60000)

    axs[i][0].set(xlabel="eps", ylabel="duration")
    axs[i][0].set_yticks([0.5, 1.5, 2.5, 3.5], labels=list(data_speedup.index.values))

    sns.heatmap(data_fp, cmap='GnBu', annot=data_fp, linewidth=0.5, ax=axs[i][1], vmin=0, vmax=100)

    axs[i][1].set(xlabel="eps", ylabel='')
    axs[i][1].set_yticks([0.5, 1.5, 2.5, 3.5], labels=list(data_fp.index.values))

    if i==0:
        axs[i][0].set_xticks([0.5, 1.5, 2.5, 3.5], labels=data_speedup.columns)
        axs[i][0].set_title('Speed-up (times) ADM vs. EDM')
        axs[i][1].set_xticks([0.5, 1.5, 2.5, 3.5], labels=data_fp.columns)
        axs[i][1].set_title('False Positives (%)')

rows = ['Phi {}'.format(row) for row in ['1', '2', '3', '4']]
for ax, row in zip(axs[:,0], rows):
    ax.annotate(row, xy=(0, 0.5), xytext=(-ax.yaxis.labelpad - 5, 0),
                xycoords=ax.yaxis.label, textcoords='offset points',
                size='large', ha='right', va='center')

plt.tight_layout()
plt.savefig("speedup.pdf", format="pdf", bbox_inches="tight")

