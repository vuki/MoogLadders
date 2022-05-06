import sys
sys.path.append('./build/lib.win-amd64-cpython-310')

import _MoogLadder

print(dir(_MoogLadder))

x = [0.0, 0.25, 0.5, -0.25, 0.0, 0.25, 0.5, -0.25, 0.0, 0.25, 0.5, -0.25]

# flt = _MoogLadder.MusicDSPMoog(48000)
flt = _MoogLadder.ImprovedMoog(48000)
flt.cutoff = 1500
y = flt.process(x)

print(y)
print(x)
print(flt.cutoff)
