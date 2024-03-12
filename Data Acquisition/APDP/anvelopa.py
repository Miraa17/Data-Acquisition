import numpy as np
from scipy.signal import hilbert

signal = []

anvelopa=open("waveData.txt","r")
for line in anvelopa:
    signal.append(int(line))


#signal este vectorul aferent semnalului analizat
analytic_signal = hilbert(signal) 

#amplitude_envelope este anvelopa semnalului
amplitude_envelope = np.abs(analytic_signal)

anvelopa.close()
anvelopa=open("anvelopa.txt","a")

for i in amplitude_envelope:
    anvelopa.write(str(i)+"\n")
