# script.py
import csv
import sys
import time
import statistics

if len(sys.argv) < 2:
    print("Usage: python script.py <csv_file>")
    sys.exit(1)

csv_file = sys.argv[1]

start = time.time()

values = []

with open(csv_file, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
        values.append(int(row["value1"]))

avg = sum(values) / len(values)
median = statistics.median(values)

end = time.time()

print(f"File: {csv_file}")
print(f"Average of value1: {avg}")
print(f"Median of value1: {median}")
print(f"Execution time: {end - start:.4f} seconds\n")
