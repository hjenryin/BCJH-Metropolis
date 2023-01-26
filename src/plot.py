from matplotlib import pyplot as plt
import numpy as np
import argparse


parser = argparse.ArgumentParser()
parser.add_argument("-f", type=str, default="../out/history")
fn = parser.parse_args().f
print("plotting", fn)
with open(fn+".csv") as f:
    data = f.readlines()
for i in range(len(data)):
    data[i] = data[i].strip()
    data[i] = data[i].split(",")
data = np.array(data)
data = data.astype(float)
score = data[:, 0]/1000
temperature = data[:, 1]
fig, ax2 = plt.subplots()
ax1 = ax2.twinx()
ax1.set_xlabel("time")
ax1.set_ylabel("score (x1000)")
ax2.set_ylabel("temperature")
n = data.shape[0]
t = np.arange(1, n+1)
ax2.set_ylim(0, np.max(temperature)*1.1)

ax1.plot(t, score, label="score", color="orange")
ax2.plot(t, temperature, label="temperature")
fig.legend()
fig.savefig(fn+".png")
