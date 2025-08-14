#!/usr/bin/env python3
# plot_qhp_heatmaps_grid.py
import argparse, os, re
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm, Normalize

def sanitize(s: str) -> str:
    return re.sub(r'[^A-Za-z0-9_.-]+', '_', str(s)).strip('_')

def normalize_columns(df: pd.DataFrame) -> pd.DataFrame:
    # lower-case headers
    mapping = {c: c.strip().lower() for c in df.columns}
    df = df.rename(columns=mapping)
    # allow alternative names
    variants = {"incr_wall_time": "incr wall time", "time_per_portion": "time per portion"}
    for k, v in variants.items():
        if k in df.columns and v not in df.columns:
            df = df.rename(columns={k: v})
    required = ["d", "eps", "delta", "speedup", "incr wall time", "time per portion"]
    for col in required:
        if col not in df.columns:
            raise ValueError(f"Missing column: '{col}'")
    if "formula" not in df.columns:
        df["formula"] = "psi"
    # numeric casts
    for col in ["d", "eps", "delta", "speedup", "incr wall time", "time per portion"]:
        df[col] = pd.to_numeric(df[col], errors="coerce")
    df["naive wall time"] = df["incr wall time"] * df["speedup"]
    return df

def build_grid(df: pd.DataFrame, value_col: str):
    d_vals = sorted(df["d"].dropna().unique().tolist())
    pairs_df = df[["eps", "delta"]].dropna().drop_duplicates().sort_values(["eps", "delta"])
    pairs = pairs_df[["eps", "delta"]].values.tolist()
    M = np.full((len(pairs), len(d_vals)), np.nan)
    for r, (e, De) in enumerate(pairs):
        for c, d in enumerate(d_vals):
            sub = df[(df["eps"] == e) & (df["delta"] == De) & (df["d"] == d)]
            if len(sub) == 1:
                M[r, c] = float(sub[value_col].values[0])
    ylabels = [f"{int(e)}/{int(De)}" for e, De in pairs]  # compact epsilon/Delta
    xlabels = [str(int(d)) for d in d_vals]
    return M, pairs, d_vals, ylabels, xlabels

def value_formatter(metric: str, v: float) -> str:
    if not np.isfinite(v):
        return ""
    if metric == "speedup":
        return f"{v:.2f}"  # two decimals
    # time metrics in ms: one decimal if <10ms, else integer; no units in text
    ms = v * 1e3
    return f"{ms:.1f}" if ms < 10 else f"{ms:.0f}"

def text_color_for_value(cmap, norm, v: float) -> str:
    v = max(v, np.finfo(float).eps)
    r, g, b, _ = cmap(norm(v))
    luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b
    return "black" if luminance > 0.6 else "white"

def plot_grid(df: pd.DataFrame, outpath: str, cmap_name: str, global_scale: bool,
              log_scale: bool, fig_width: float, row_height: float, annot: bool):
    formulas = list(df["formula"].dropna().unique())
    nF = len(formulas)
    metrics = [
        ("speedup", "Speed-up (times)", "speedup"),
        ("incr wall time", "Total time (ms)", "incr"),
        ("time per portion", "Per-portion time (ms)", "tpp"),
    ]
    ncols = len(metrics)

    # Prepare grids and global limits
    grids = {}
    vlims = {m[0]: [np.inf, -np.inf] for m in metrics}
    for metric, _, _ in metrics:
        grids[metric] = []
        for f in formulas:
            sub = df[df["formula"] == f]
            M, pairs, d_vals, ylabels, xlabels = build_grid(sub, metric)
            grids[metric].append((M, pairs, d_vals, ylabels, xlabels))
            vmin = np.nanmin(M); vmax = np.nanmax(M)
            if np.isfinite(vmin) and vmin < vlims[metric][0]: vlims[metric][0] = vmin
            if np.isfinite(vmax) and vmax > vlims[metric][1]: vlims[metric][1] = vmax

    fig_height = row_height * nF
    fig = plt.figure(figsize=(fig_width, fig_height))
    # wider left margin and inter-column space to prevent overlaps
    plt.subplots_adjust(left=0.22, right=0.98, wspace=0.32, hspace=0.30)

    axes = np.empty((nF, ncols), dtype=object)

    for r, f in enumerate(formulas):
        for c, (metric, title, short) in enumerate(metrics):
            ax = plt.subplot(nF, ncols, r * ncols + c + 1)
            axes[r, c] = ax
            M, pairs, d_vals, ylabels, xlabels = grids[metric][r]

            # scaling
            if global_scale:
                vmin, vmax = vlims[metric]
            else:
                vmin, vmax = np.nanmin(M), np.nanmax(M)
            if log_scale:
                vmin = max(np.finfo(float).eps, vmin)
            norm = LogNorm(vmin=vmin, vmax=vmax) if log_scale else Normalize(vmin=vmin, vmax=vmax)
            cmap = plt.get_cmap(cmap_name)

            # heatmap
            im = ax.imshow(M, aspect="auto", origin="lower", norm=norm, cmap=cmap)

            # subtle grid lines
            ax.set_xticks(np.arange(-.5, M.shape[1], 1), minor=True)
            ax.set_yticks(np.arange(-.5, M.shape[0], 1), minor=True)
            ax.grid(which="minor", linestyle="-", linewidth=0.4, alpha=0.4)
            ax.tick_params(which="minor", bottom=False, left=False)

            # ticks & labels
            if r == nF - 1:
                ax.set_xlabel("duration (d)", fontsize=10, labelpad=6)
                ax.set_xticks(range(len(xlabels)))
                ax.set_xticklabels(xlabels, fontsize=9)
            else:
                ax.set_xticks([])

            if c == 0:
                ax.set_yticks(range(len(ylabels)))
                ax.set_yticklabels(ylabels, fontsize=9)
                ax.set_ylabel("epsilon / Delta", fontsize=10)
                # position y-label slightly right of the formula label
                ax.yaxis.set_label_coords(-0.10, 0.5)
                # formula name placed BEFORE the epsilon/Delta tag, with spacing
                ax.text(-0.20, 0.5, str(f), rotation=90, transform=ax.transAxes,
                        ha="right", va="center", fontsize=11)
            else:
                ax.set_yticks([])

            if r == 0:
                ax.set_title(title, fontsize=11, pad=8)

            # cell annotations
            if annot:
                for rr in range(M.shape[0]):
                    for cc in range(M.shape[1]):
                        v = M[rr, cc]
                        if not np.isfinite(v):
                            continue
                        txt = value_formatter(metric, v)
                        color = text_color_for_value(cmap, norm, v)
                        ax.text(cc, rr, txt, ha="center", va="center", fontsize=8, color=color)

    # shared colorbars per column with extra padding so they don't overlap the next panel
    for c in range(ncols):
        im = axes[0, c].images[0]
        cb = fig.colorbar(im, ax=axes[:, c].tolist(), fraction=0.040, pad=0.06)
        cb.ax.tick_params(labelsize=8)

    fig.savefig(outpath, dpi=300, bbox_inches="tight")
    root, ext = os.path.splitext(outpath)
    if ext.lower() != ".pdf":
        fig.savefig(root + ".pdf", bbox_inches="tight")
    plt.close(fig)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--input", required=True, help="TSV/CSV with: formula,d,eps,Delta,speedup,incr wall time,time per portion")
    ap.add_argument("--out", required=True, help="Output file (.png or .pdf). PDF also written alongside PNG.")
    ap.add_argument("--delimiter", default=None, help="Override delimiter. Default: tab for .tsv, comma otherwise.")
    ap.add_argument("--cmap", default="GnBu", help="Matplotlib colormap (default: GnBu).")
    ap.add_argument("--global-scale", action="store_true", help="Use identical color limits across formulas per metric.")
    ap.add_argument("--no-log", action="store_true", help="Use linear color scale (default: log).")
    ap.add_argument("--fig-width", type=float, default=10.5)
    ap.add_argument("--row-height", type=float, default=2.0)
    ap.add_argument("--no-annot", action="store_true")
    args = ap.parse_args()

    # delimiter
    if args.delimiter is None:
        delim = "\t" if args.input.lower().endswith(".tsv") else ","
    else:
        delim = args.delimiter

    df = pd.read_csv(args.input, sep=delim)
    df = normalize_columns(df)
    df = df[df["d"] != 16].copy()

    plot_grid(
        df=df,
        outpath=args.out,
        cmap_name=args.cmap,
        global_scale=args.global_scale,
        log_scale=(not args.no_log),
        fig_width=args.fig_width,
        row_height=args.row_height,
        annot=(not args.no_annot),
    )

if __name__ == "__main__":
    main()
