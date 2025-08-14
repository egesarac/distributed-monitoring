#!/usr/bin/env python3
# plot_qhp_heatmaps_grid.py — top labels once; duration on every subfigure
import argparse, os, re
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm, Normalize
from mpl_toolkits.axes_grid1 import make_axes_locatable

def normalize_columns(df: pd.DataFrame) -> pd.DataFrame:
    df = df.rename(columns={c: c.strip().lower() for c in df.columns})
    aliases = {"incr_wall_time": "incr wall time", "time_per_portion": "time per portion"}
    for k, v in aliases.items():
        if k in df.columns and v not in df.columns:
            df = df.rename(columns={k: v})
    need = ["d", "eps", "delta", "speedup", "incr wall time", "time per portion"]
    for c in need:
        if c not in df.columns:
            raise ValueError(f"Missing column: '{c}'")
    if "formula" not in df.columns:
        df["formula"] = "Psi"
    for c in ["d","eps","delta","speedup","incr wall time","time per portion"]:
        df[c] = pd.to_numeric(df[c], errors="coerce")
    df["naive wall time"] = df["incr wall time"] * df["speedup"]
    return df

def build_grid(df: pd.DataFrame, value_col: str):
    d_vals = sorted(df["d"].dropna().unique().tolist())
    pairs_df = df[["eps","delta"]].dropna().drop_duplicates().sort_values(["eps","delta"])
    pairs = pairs_df[["eps","delta"]].values.tolist()
    M = np.full((len(pairs), len(d_vals)), np.nan)
    for r,(e,De) in enumerate(pairs):
        for c,d in enumerate(d_vals):
            sel = df[(df["eps"]==e) & (df["delta"]==De) & (df["d"]==d)]
            if len(sel)==1:
                M[r,c] = float(sel[value_col].values[0])
    ylabels = [f"{int(e)}/{int(De)}" for e,De in pairs]  # epsilon/Delta
    xlabels = [str(int(d)) for d in d_vals]
    return M, ylabels, xlabels

def fmt_cell(metric: str, v: float) -> str:
    if not np.isfinite(v):
        return ""
    if metric == "speedup":
        return f"{v:.2f}"                 # two decimals, no unit
    ms = v * 1e3                          # times in ms
    return f"{ms:.1f}" if ms < 10 else f"{ms:.0f}"

def text_color(cmap, norm, v: float) -> str:
    v = max(v, np.finfo(float).eps)
    r,g,b,_ = cmap(norm(v))
    lum = 0.2126*r + 0.7152*g + 0.0722*b
    return "black" if lum > 0.6 else "white"

def draw_heatmap(ax, M, xlabels, ylabels, cmap_name, log_scale=True):
    vmin = np.nanmin(M); vmax = np.nanmax(M)
    if log_scale:
        vmin = max(vmin, np.finfo(float).eps)
    norm = LogNorm(vmin=vmin, vmax=vmax) if log_scale else Normalize(vmin=vmin, vmax=vmax)
    cmap = plt.get_cmap(cmap_name)

    im = ax.imshow(M, origin="lower", aspect="auto", cmap=cmap, norm=norm)

    # no outer spines
    for s in ax.spines.values():
        s.set_visible(False)

    # thin white gaps between cells
    ny, nx = M.shape
    ax.set_xticks(np.arange(-.5, nx, 1), minor=True)
    ax.set_yticks(np.arange(-.5, ny, 1), minor=True)
    ax.grid(which="minor", color="white", linestyle="-", linewidth=0.6)
    ax.tick_params(which="minor", bottom=False, left=False)

    # ticks
    ax.set_xticks(range(nx))
    ax.set_xticklabels(xlabels, fontsize=9)
    ax.set_yticks(range(ny))
    ax.set_yticklabels(ylabels, fontsize=9)

    # slim colorbar per plot
    divider = make_axes_locatable(ax)
    cax = divider.append_axes("right", size="3.5%", pad=0.05)
    cb = plt.colorbar(im, cax=cax)
    cb.ax.tick_params(labelsize=8)

    return cmap, norm

def plot_grid(df, outpath, cmap_name="GnBu", global_scale=False, log_scale=True,
              fig_width=10.5, row_height=2.0):
    # optional: drop d=16 and d=32 globally
    df = df[~df["d"].isin([16, 32])].copy()

    formulas = list(df["formula"].dropna().unique())
    metrics = [
        ("speedup",         "Speed-up (times)"),
        ("incr wall time",  "Total time (ms)"),
        ("time per portion","Per-portion time (ms)"),
    ]
    nrows, ncols = len(formulas), len(metrics)

    fig = plt.figure(figsize=(fig_width, row_height*nrows))
    # a bit more left margin for labels; tighter between plots
    plt.subplots_adjust(left=0.30, right=0.985, top=0.95, bottom=0.08, wspace=0.16, hspace=0.35)

    axes = np.empty((nrows, ncols), dtype=object)
    # draw each row
    for r, f in enumerate(formulas):
        sub = df[df["formula"] == f]
        M_speed, ylab, xlab = build_grid(sub, "speedup")
        M_incr,  _,   _     = build_grid(sub, "incr wall time")
        M_tpp,   _,   _     = build_grid(sub, "time per portion")
        rowM = [M_speed, M_incr, M_tpp]

        for c,(metric, _title) in enumerate(metrics):
            ax = plt.subplot(nrows, ncols, r*ncols + c + 1)
            axes[r,c] = ax
            cmap, norm = draw_heatmap(
                ax,
                rowM[c],
                xlabels=xlab,                      # bottom labels: duration on every subfigure
                ylabels=ylab if c==0 else [""]*len(ylab),
                cmap_name=cmap_name,
                log_scale=log_scale
            )

            # annotate cells
            M = rowM[c]
            for rr in range(M.shape[0]):
                for cc in range(M.shape[1]):
                    v = M[rr, cc]
                    if not np.isfinite(v): continue
                    ax.text(cc, rr, fmt_cell(metric, v),
                            ha="center", va="center", fontsize=8, color=text_color(cmap, norm, v))

            # x label on EVERY subplot (as requested)
            ax.set_xlabel("duration", fontsize=10, labelpad=4)

            # y labels in the first column + formula name before epsilon/Delta
            if c == 0:
                ax.set_ylabel("epsilon/Delta", fontsize=10)
                ax.yaxis.set_label_coords(-0.18, 0.5)     # move left so it doesn't clash with ticks
                ax.text(-0.36, 0.5, str(f), rotation=90, transform=ax.transAxes,
                        ha="right", va="center", fontsize=11)
            else:
                ax.set_yticklabels([])

    # ---- column headers ONCE at the very top of the figure ----
    top_titles = [t for _, t in metrics]
    for c in range(ncols):
        # use the position of the first row in this column to center the header
        ax0 = axes[0, c]
        x0, y0, w, h = ax0.get_position().bounds
        xc = x0 + w/2.0
        fig.text(xc, 0.975, top_titles[c], ha="center", va="top", fontsize=12)

    # save
    fig.savefig(outpath, dpi=300, bbox_inches="tight")
    root, ext = os.path.splitext(outpath)
    if ext.lower() != ".pdf":
        fig.savefig(root + ".pdf", bbox_inches="tight")
    plt.close(fig)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--input", required=True, help="TSV/CSV with: formula,d,eps,Delta,speedup,incr wall time,time per portion")
    ap.add_argument("--out", required=True, help="Output path (.png or .pdf)")
    ap.add_argument("--delimiter", default=None, help="Override delimiter (default: .tsv→tab, else comma)")
    ap.add_argument("--cmap", default="GnBu")
    ap.add_argument("--no-log", action="store_true")
    ap.add_argument("--fig-width", type=float, default=10.5)
    ap.add_argument("--row-height", type=float, default=2.0)
    ap.add_argument("--keep-d16", action="store_true", help="Keep d=16 (d=16 is dropped by default)")
    args = ap.parse_args()

    # delimiter
    if args.delimiter is None:
        delim = "\t" if args.input.lower().endswith(".tsv") else ","
    else:
        delim = args.delimiter

    df = pd.read_csv(args.input, sep=delim)
    df = normalize_columns(df)

    plot_grid(
        df=df,
        outpath=args.out,
        cmap_name=args.cmap,
        log_scale=(not args.no_log),
        fig_width=args.fig_width,
        row_height=args.row_height,
    )

if __name__ == "__main__":
    main()
