import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt


# filenames
f1 = '../results/01gand.csv'
f2 = '../results/02gor.csv'
f3 = '../results/05until.csv'
f4 = '../results/03gimpf.csv'
f5 = '../results/04gimpft1.csv'

dtype = {"d": int, "eps": int, "ADM": float, "EDM": float, "FP": float,"speedup": float, "label": str}

fs = [f1, f2, f3, f4, f5]


fig, axs = plt.subplots(len(fs), 3)
fig.set_size_inches([12, 12.5])

for i in range(len(fs)):
    df = pd.read_csv(fs[i], dtype=dtype)

    data_fp = df.pivot_table('FP', 'eps', 'd', fill_value=float('nan'), dropna=False)
    data_combined = df.pivot_table('combined', 'eps', 'd', fill_value=float('nan'), dropna=False)
    data_speedup = df.pivot_table('speedup', 'eps', 'd', fill_value=float('nan'), dropna=False)
#    data_speedup_label = df.pivot_table(values='label', index='eps', columns='d', aggfunc=lambda x: ''.join(str(v) for v in x),
#                                    fill_value='')
#    data_speedup_label = data_speedup_label.map(lambda x: x.replace('\\n', '\n'))

    sns.heatmap(data_speedup, cmap='GnBu', annot=data_speedup, linewidth=0.5, ax=axs[i][0], vmin=0, vmax=60000)

    axs[i][0].set(xlabel="duration", ylabel="epsilon")
    axs[i][0].set_yticks([0.5, 1.5, 2.5, 3.5], labels=list(data_speedup.index.values))

    sns.heatmap(data_fp, cmap='GnBu', annot=data_fp, linewidth=0.5, ax=axs[i][1], vmin=0, vmax=100)

    axs[i][1].set(xlabel="duration", ylabel='')
    axs[i][1].set_yticks([0.5, 1.5, 2.5, 3.5], labels=list(data_fp.index.values))

    sns.heatmap(data_combined, cmap='GnBu', annot=data_combined, linewidth=0.5, ax=axs[i][2], vmin=0, vmax=60000)

    axs[i][2].set(xlabel="duration", ylabel='')
    axs[i][2].set_yticks([0.5, 1.5, 2.5, 3.5], labels=list(data_fp.index.values))

    if i==0:
        axs[i][0].set_xticks([0.5, 1.5, 2.5, 3.5], labels=data_speedup.columns)
        axs[i][0].set_title('Speed-up (times) ADM vs. EDM')
        axs[i][1].set_xticks([0.5, 1.5, 2.5, 3.5], labels=data_fp.columns)
        axs[i][1].set_title('False Positives (%)')
        axs[i][2].set_xticks([0.5, 1.5, 2.5, 3.5], labels=data_fp.columns)
        axs[i][2].set_title('Speedup (times) Combined vs. EDM')

rows = ['Phi {}'.format(row) for row in ['1', '2', '3', '4']]
for ax, row in zip(axs[:,0], rows):
    ax.annotate(row, xy=(0, 0.5), xytext=(-ax.yaxis.labelpad - 5, 0),
                xycoords=ax.yaxis.label, textcoords='offset points',
                size='large', ha='right', va='center', rotation=90)

plt.tight_layout()
plt.savefig("speedup.pdf", format="pdf", bbox_inches="tight")

