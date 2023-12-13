import csv
f = open("simulation_results_file_data.csv", "r")

Lines = f.readlines()
new_lines = []

for line in Lines:
    if "torus" in line:
        new_lines.append(line)
    if "Average delay" in line:
        new_lines.append(line)
    if "Maximum delay" in line:
        new_lines.append(line)
        
# print('line 1: ', new_lines)
with open("output_data.csv", "w", newline='') as txt_file:
    writer = csv.writer(txt_file)
    field = ["Dimensions","Twists", "Average Delay", "Maximum Delay"]
    writer.writerow(field)
    for index in range(0, len(new_lines), 3):
        print("line:" , new_lines[index+1])
        x = new_lines[index].split("-")
        dim = x[2].split(".")
        avg = new_lines[index+1].split(":")
        maxd = new_lines[index+2].split(":")
        print("x", x)
        print("dim", dim)
        print("avg", avg)
        print("max", maxd)
        writer.writerow([x[1], dim[0], avg[1], maxd[1]])