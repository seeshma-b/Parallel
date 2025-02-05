import matplotlib.pyplot as plt
import numpy as np

sizes, times = np.loadtxt('results.txt', unpack=True)
plt.figure()
plt.plot(sizes, times, marker='o')
plt.xscale('log')
plt.yscale('log')
plt.xlabel('Array Size')
plt.ylabel('Time')
plt.title('Merge Sort Performance')
plt.grid(True)
plt.savefig('mergeSort_plot.png')
plt.show()
