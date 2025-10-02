docker run --rm -v "D:\U semestre 11\Pro paralela\Corte 2\Clase octubre 1\Tareas de la semana en clase\Actividad del miercoles":/work -w /work alhumaidyaroob/gnu-parallel ls
# create_csv.py
import csv
import random

def create_csv(filename, rows=1000000):
    with open(filename, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["value1", "value2", "value3"])  # encabezados
        for _ in range(rows):
            writer.writerow([random.randint(0, 1000),
                             random.randint(0, 1000),
                             random.randint(0, 1000)])

create_csv("data1.csv")
create_csv("data2.csv")
create_csv("data3.csv")
