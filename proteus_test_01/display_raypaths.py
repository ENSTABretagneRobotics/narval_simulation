#! /usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt

# paths = np.genfromtxt('rayPaths.csv', delimiter=', ')
# x = paths[:,::3]
# y = paths[:,1::3]
# z = paths[:,2::3]

paths = np.load('rayPaths.npy')
x = paths[:,:,0]
y = paths[:,:,1]
z = paths[:,:,2]


_, ax = plt.subplots(1,1)
# ax.plot(x[:3, :10].T, z[:3, :10].T)
ax.plot(x.T, z.T)
ax.grid()

plt.show()
