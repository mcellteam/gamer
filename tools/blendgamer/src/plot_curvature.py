
import sys
import pickle
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

mpl.rcParams["pdf.fonttype"] = 42
mpl.rcParams["ps.fonttype"] = 42

p_dict = pickle.loads(sys.stdin.buffer.read())
data = p_dict['data']
mixpoint = p_dict['mixpoint']
tmin = p_dict['tmin']
tmax = p_dict['tmax']
cmap = p_dict['cmap']
showplot = p_dict['showplot']
saveplot = p_dict['saveplot']
file_prefix = p_dict['file_prefix']
vlayer = p_dict['vlayer']


# Make Plot
fig = plt.figure(figsize=(8, 5))
ax = fig.add_axes([0.1, 0.05, 0.6, 0.9])

amin = np.amin(data)
amax = np.amax(data)

ax.hist(data, bins="auto")
ax.set_title("%s Distribution" % (file_prefix))
ax.axvline(amin, color="r", linestyle="dashed", linewidth=1)
ax.axvline(amax, color="r", linestyle="dashed", linewidth=1)

data[data < tmin] = tmin
data[data > tmax] = tmax
amin = np.amin(data)
amax = np.amax(data)

extend = "neither"

ax.axvline(tmin, color="g", linestyle="dashed", linewidth=2)
ax.axvline(tmax, color="g", linestyle="dashed", linewidth=2)

if tmin > amin and tmax < amax:
    extend = "both"
elif tmin > amin:
    extend = "min"
elif tmax < amax:
    extend = "max"

# Add axis for colorbar and plot it
ax = fig.add_axes([0.75, 0.05, 0.05, 0.9])

# Construct the norm and colorbar
if amin < 0 and amax > 0:
    norm = mpl.colors.TwoSlopeNorm(0, vmin=amin, vmax=amax)
    colors_neg = cmap(np.linspace(0, mixpoint, 256))
    colors_pos = cmap(np.linspace(mixpoint, 1, 256))

    all_colors = np.vstack((colors_neg, colors_pos))
    curvature_map = mpl.colors.LinearSegmentedColormap.from_list(
        "curvature_map", all_colors
    )
else:
    norm = mpl.colors.Normalize(vmin=amin, vmax=amax)
    curvature_map = cmap


cb = mpl.colorbar.Colorbar(
    ax, cmap=curvature_map, norm=norm, orientation="vertical"
)

ticks = cb.get_ticks()
ticks.sort()

if ticks[0] < amin:
    ticks = ticks[1:-1]
if ticks[-1] > amax:
    ticks = ticks[0:-2]

if amin != ticks[0]:
    ticks = np.insert(ticks, 0, amin)
if amax != ticks[-1]:
    ticks = np.append(ticks, amax)
cb.set_ticks(ticks)

ticklabels = [r"{:0.1f}".format(tick) for tick in ticks]

if extend == "neither":
    pass
elif extend == "both":
    ticklabels[0] = "< " + ticklabels[0]
    ticklabels[-1] = "> " + ticklabels[-1]
elif extend == "max":
    ticklabels[-1] = "> " + ticklabels[-1]
elif extend == "min":
    ticklabels[0] = "< " + ticklabels[0]
cb.set_ticklabels(ticklabels)
cb.ax.tick_params(labelsize=14)
cb.set_label("%s [$\mu m^{-1}$]" % (vlayer), size=16)

if saveplot:
    plt.savefig(file_prefix + ".pdf", format="pdf")
if showplot:
    plt.show()

