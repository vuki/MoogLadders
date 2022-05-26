import numpy as np
from matplotlib import pyplot as plt

try:
    import MoogLadder
except ModuleNotFoundError:
    import sys
    sys.path.insert(0, f'./build/lib.win-amd64-cpython-{sys.version_info.major}{sys.version_info.minor}')
    import MoogLadder

# flt_type = MoogLadder.ValimakiMoog
flt_type = MoogLadder.HuovilainenMoog

f0 = 1000
fc = 16000
fs = 48000
res = 0
drive = 0

ns = 2048
sine = np.sin(2 * np.pi * np.arange(ns) * f0 / fs)

# fake upsample by sample doubling
x = np.zeros(2 * ns)
x[0::2] = sine
x[1::2] = sine
frate = 2 * fs

flt = flt_type(frate)
flt.cutoff = fc
flt.resonance = res
flt.drive = drive

y = flt.process(x)

# decimate
y = y[1::2]

xspec = np.fft.rfft(sine * np.hanning(ns))
axspec = np.abs(xspec) * 2 / ns
yspec = np.fft.rfft(y * np.hanning(ns))
ayspec = np.abs(yspec) * 2 / ns
f = np.fft.rfftfreq(ns, 1 / fs)

sumsq_x = np.sum(axspec[1:]**2)
ytmp = np.copy(ayspec)
p = round(f0 * ns / fs)
ytmp[p - 4:p + 4] = 0
sumsq_y = np.sum(ytmp[1:]**2)
thd = 100 * np.sqrt(sumsq_y / sumsq_x)
print('Max input level:  {:.3f} dB'.format(20 * np.log10(np.max(axspec))))
print('Max output level: {:.3f} dB'.format(20 * np.log10(np.max(ayspec))))
print('THD: {:.3f}%'.format(thd))

fig, ax = plt.subplots(figsize=(10, 4), tight_layout=True)
ax.plot(f, 20 * np.log10(axspec), c='#c0c0c0')
ax.plot(f, 20 * np.log10(ayspec))
ax.set_xlim(100, 24000)
ax.set_ylim(-80, 0)
ax.grid()

# _, ax2 = plt.subplots(figsize=(10, 4), tight_layout=True)
# ax2.plot(y)
# ax2.grid()

plt.show()
