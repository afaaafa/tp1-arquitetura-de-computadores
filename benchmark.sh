#!/bin/bash
# benchmark_and_log.sh

OUTPUT_FILE="benchmark_results.csv"

echo "Method,Dataset,Elements,Dimensions,Repetitions,TotalUserTime,TotalSystemTime,AvgUserTime,AvgSystemTime" > $OUTPUT_FILE

for method in 1 2 3; do
  for dataset in datasets/small.csv datasets/medium.csv datasets/large.csv; do
    # Run the normalizer and capture its output
    output=$(./normalize $dataset $method 100)
    
    elements=$(echo "$output" | grep "Elements" | awk '{print $3}')
    dimensions=$(echo "$output" | grep "Dimensions" | awk '{print $3}')
    total_user=$(echo "$output" | grep "Total User Time" | awk '{print $5}')
    total_system=$(echo "$output" | grep "Total System Time" | awk '{print $5}')
    avg_user=$(echo "$output" | grep "Average User Time" | awk '{print $5}')
    avg_system=$(echo "$output" | grep "Average System Time" | awk '{print $5}')
    
    case $method in
      1) method_name="Newton-Raphson" ;;
      2) method_name="SSE" ;;
      3) method_name="Naive" ;;
    esac
    
    echo "$method_name,$dataset,$elements,$dimensions,100,$total_user,$total_system,$avg_user,$avg_system" >> $OUTPUT_FILE
  done
done

echo "Benchmark results saved to $OUTPUT_FILE"