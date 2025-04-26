import csv
import random
import os

files = {
    'small.csv': (100, 10),
    'medium.csv': (10000, 100),
    'large.csv': (100000, 300),
}

output_dir = 'datasets'
os.makedirs(output_dir, exist_ok=True)

# Seed for reproducible data
random.seed(42)

def generate_value():
    return round(random.uniform(0, 1000), 2)

for file_name, (rows, columns) in files.items():
    path = os.path.join(output_dir, file_name)
    with open(path, mode='w', newline='') as csv_file:
        writer = csv.writer(csv_file)
        
        # Header
        header = [f'column_{i+1}' for i in range(columns)]
        writer.writerow(header)
        
        # Data rows
        for _ in range(rows):
            row = [generate_value() for _ in range(columns)]
            writer.writerow(row)
    
    print(f'File "{file_name}" generated successfully!')

print("All files have been generated.")
