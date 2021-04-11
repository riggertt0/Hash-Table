import numpy as np
import matplotlib.pyplot as plt

stat = np.genfromtxt ("Statistic.csv",delimiter=',')
stat_size = len (stat)

if (stat_size == 0):
    exit()

fig = plt.figure (figsize = (7, 7))
ax  = fig.add_subplot (1,1,1)

ax.bar (list (range (stat_size)), stat)

# Warning: magic constants!
mean = 6507 / 400

ax.plot ([0, stat_size], [mean, mean], label = "Среднее значение", color = "red", alpha = 0.5)

ax.set_title  ("Распределение вашей хеш-функции", fontsize = 20)
ax.set_ylabel ("Количество элементов", fontsize = 20)
ax.set_xlabel ("Номер ячейки массива", fontsize = 20)

ax.grid (True, alpha = 0.3)
ax.legend ()
plt.savefig ("Graphics/Graphic.pdf")
