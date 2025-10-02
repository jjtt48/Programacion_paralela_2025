import csv
import random

def create_csv(filename, rows=1000000):
    with open(filename, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["value1", "value2", "value3"])  
        for _ in range(rows):
            writer.writerow([random.randint(0, 1000),
                             random.randint(0, 1000),
                             random.randint(0, 1000)])

create_csv("data1.csv")
create_csv("data2.csv")
create_csv("data3.csv")

