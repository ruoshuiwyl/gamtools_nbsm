#!/usr/bin/python3
import matplotlib.pyplot as plt

f = open("2018-07-03_16:54:33.txt", "r")
x_time = []
y_cpu = []
y_mem = []
while True:
    line = f.readline()
    if not line:
        break;
    words = line.split()
    x_time.append(int(words[1]))
    y_cpu.append(float(words[2]))
    y_mem.append(float(words[4]))
f.close();

plt.figure(num=1, figsize=(8,8))
plt.subplot(211)
plt.title('cpu', size=14)
# plt.xlabel('time', size=14)
plt.ylabel('cpu', size=14)
plt.plot(x_time, y_cpu)
plt.subplot(212)
plt.title('mem', size=14)
plt.xlabel('time', size=14)
plt.ylabel('mem(G)', size=14)
plt.plot(x_time, y_mem)
plt.savefig('cpu.png',format = 'png')
