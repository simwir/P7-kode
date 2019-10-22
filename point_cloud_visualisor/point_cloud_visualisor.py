import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv("point_cloud.csv")
arr = df.values

x = arr[:, 0]
y = arr[:, 1]

circle = plt.Circle((0, 0), 0.5, color='b', fill=False)
plt.scatter(x, y, cmap=plt.get_cmap("plasma"), c=np.arange(len(arr)), marker='.')
plt.scatter(0, 0, c="red", marker="X")
plt.axis('equal')
ax = plt.gca()
ax.add_artist(circle)
plt.show()
