import numpy as np
from matplotlib import pyplot as plt
from matplotlib.ticker import ScalarFormatter

try:
    import MoogLadder
except ModuleNotFoundError:
    import sys
    sys.path.insert(0, f'./build/lib.win-amd64-cpython-{sys.version_info.major}{sys.version_info.minor}')
    import MoogLadder

flt_type = MoogLadder.ValimakiMoog
# flt_type = MoogLadder.DAngeloMoog
# flt_type = MoogLadder.MusicDSPMoog
# flt_type = MoogLadder.ImprovedMoog
# flt_type = MoogLadder.HuovilainenMoog
# flt_type = MoogLadder.RKSimulationMoog

fs = 96000

x = [0] * 4096
x[0] = 1

fig1, ax1 = plt.subplots(figsize=(8, 3.5), tight_layout=True)
f = np.fft.rfftfreq(2048, 1 / 48000)

r = 0.9
for c in (500, 1000, 5000, 10000, 15000):
    flt = flt_type(fs)
    flt.cutoff = c
    flt.resonance = r
    y = flt.process(x)
    yspec = np.fft.rfft(np.asarray(y))
    ax1.semilogx(f, 20 * np.log10(np.abs(yspec[:1025])), label=f'fc={c} Hz')
    # ax1.plot(f, 20 * np.log10(np.abs(yspec[:1025])), label=f'fc={c} Hz')

ax1.set_xlim(100, 24000)
ax1.set_ylim(bottom=-80)
ax1.grid(which='both')
# ax1.legend()
ax1.xaxis.set_major_formatter(ScalarFormatter())
plt.show()
