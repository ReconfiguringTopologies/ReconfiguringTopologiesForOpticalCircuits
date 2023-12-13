import matplotlib.pyplot as plt

results = open("simulation_results_file.csv", "r")
lines = results.readlines();

x = []
y1 = []
y2 = []
y3 = []
y4 = []
y5 = []
y6 = []
line_no = 0

for i in range(16, 512+16, 16):
	x.append(i)
	
	line_no += 1
	v1 = lines[line_no][1:-1]
	y1.append(float(v1))
	
	line_no += 3
	v2 = lines[line_no][1:-1]
	y2.append(float(v2))

	'''
	line_no += 3
	v3 = lines[line_no][1:-1]
	y3.append(float(v3))
	'''

	'''
	line_no += 3
	v4 = lines[line_no][1:-1]
	y4.append(float(v4))

	line_no += 3
	v5 = lines[line_no][1:-1]
	y5.append(float(v5))

	line_no += 3
	v6 = lines[line_no][1:-1]
	y6.append(float(v6))
	'''

	line_no += 2

'''
plt.plot(x, y1, label='([1, 0], [0, 0], [0, 0])')
plt.plot(x, y2, label='([0, 1], [0, 0], [0, 0])')
plt.plot(x, y3, label='([0, 0], [1, 0], [0, 0])')
plt.plot(x, y4, label='([0, 0], [0, 1], [0, 0])')
plt.plot(x, y5, label='([0, 0], [0, 0], [1, 0])')
plt.plot(x, y6, label='([0, 0], [0, 0], [0, 1])')
'''
plt.plot(x, y1, label='Regular')
plt.plot(x, y2, label='Single Twist')
# plt.plot(x, y3, label='Double Twist')

plt.xlabel("Data rate (kb/s)")
plt.ylabel("Average delay (ms)")
plt.title("Data rate v.s. Delay")

plt.legend()
plt.show()