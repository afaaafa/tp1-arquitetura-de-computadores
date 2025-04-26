# 🧹 Vector Normalization Benchmark

## 📜 Project Overview

This project benchmarks different methods for normalizing feature vectors using:

- Newton-Raphson approximation (based on the famous Quake III hack)
- SSE (Streaming SIMD Extensions) hardware instructions
- Naïve approach using standard sqrt function

Normalization is a crucial step in machine learning and data preprocessing, ensuring that vectors have a consistent magnitude (usually 1). This improves numerical stability, convergence speed, and accuracy of algorithms.

## ⚙️ How to Compile and Run

### Compilation

```bash
gcc -O3 -msse -o normalize csv_feature_normalizer.c -lm
```

- `-O3`: Enables high-level optimizations.
- `-msse`: Allows use of SSE instructions.

### Execution

```bash
./normalize <dataset.csv> <method_number> <repetitions>
```

- `<dataset.csv>`: Path to your dataset file.
- `<method_number>`:
  - 1 → Newton-Raphson
  - 2 → SSE
  - 3 → Naïve
  - 4 → Benchmark all methods
- `<repetitions>`: Number of times to repeat the benchmark for averaging.

Example:

```bash
./normalize datasets/large.csv 1 100
```

## 🛠️ How to Generate Datasets

We provide a simple Python script to generate random datasets. There is a seed for reproducible data, but you can change in the souce code if you want.

```python
python3 dataset_generator.py
```

This script will create a folder called `datasets` with small, medium and large CSV files.

```python
files = {
    'small.csv': (100, 10),
    'medium.csv': (10000, 100),
    'large.csv': (100000, 300),
}
```

## 📊 Benchmark Automation and Data Collection

We provide a shell script to automate benchmarking and save results for further analysis (executable must call `normalize`):

```bash
./benchmark.sh <repetitions>
```

This script will:

1. Run all normalization methods against each dataset with the specified number of repetitions
2. Collect performance metrics for each combination
3. Save the results to a CSV file named `benchmark_results.csv`

### How to Use the Benchmark Script

1. Make sure the script is executable:

   ```bash
   chmod +x benchmark.sh
   ```

2. Run the script with the desired number of repetitions:

   ```bash
   ./benchmark.sh 10000
   ```

3. Access the results:
   ```bash
   cat benchmark_results.csv
   ```

The CSV file contains columns for method name, dataset, dimensions, elements, execution times, and other performance metrics, making it easy to perform further analysis or create visualizations with tools like Python's pandas and matplotlib.

If you don't specify repetitions, the script will display usage instructions:

```bash
Usage: ./benchmark.sh <repetitions>
Example: ./benchmark.sh 10000
```

The CSV file contains columns for method name, dataset, dimensions, elements, execution times, and other performance metrics, making it easy to perform further analysis or create visualizations with tools like Python's pandas and matplotlib.

## 📊 How to Interpret the Benchmark Results

After running the benchmark, you will see an output like this:

```
🏁 Starting Benchmark
Elements      : 1000
Dimensions    : 128
Method        : Newton-Raphson
Repetitions   : 100

📈 Resource Usage - Start
User time: 1.234567 seconds
System time: 0.123456 seconds
...

🔎 Results
Total User Time   : 1.234567 seconds
Total System Time : 0.123456 seconds
Average User Time : 0.012345 seconds
Average System Time : 0.001234 seconds
```

- Total User Time: Total CPU time spent executing user instructions.
- Total System Time: Total CPU time spent on system/kernel operations.
- Average Times: Useful to compare the performance between methods.
- Lower is better: Faster normalization indicates better optimization.

## 🧠 About Vector Normalization

Vector normalization is the process of scaling a vector so that its magnitude (length) is 1, without changing its direction. This is typically done by dividing each component of the vector by its Euclidean norm (also called the L2 norm):

Normalized Vector = Vector / ‖Vector‖

where

‖Vector‖ = √(x₁² + x₂² + ⋯ + xₙ²)

Normalization is essential because:

- It prevents features with large scales from dominating others.
- It improves convergence in optimization algorithms.
- It ensures better numerical stability in calculations.

## ✨ Implemented Methods Explained

| Method         | Description                                                                                                                                                                                                              |
| -------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| Newton-Raphson | An approximation method using one iteration of Newton-Raphson for computing the inverse square root. It's based on a famous optimization trick from the Quake III Arena source code. Extremely fast with good precision. |
| SSE            | Utilizes the \_mm_rsqrt_ss SSE instruction to compute an approximate inverse square root in parallel. Hardware-accelerated, very fast but slightly less accurate.                                                        |
| Naïve          | Traditional method using the standard sqrt function from the math library. Accurate but relatively slower.                                                                                                               |

## 📂 Project Structure

```
.
├── csv_feature_normalizer.c  # Main C program
├── dataset_generator.py      # Python script to generate datasets
├── benchmark.sh              # Shell script for automated benchmarking
├── benchmark_results.csv     # Generated benchmark data
└── README.md                 # This file
```

## 📢 Notes

- When benchmarking, repeat several times (e.g., 100+) to get stable average times.
- SSE requires hardware support. Almost all modern CPUs support SSE.
- Newton-Raphson is a tradeoff between speed and slight loss of precision — appropriate for many real-world applications where tiny errors are tolerable.
