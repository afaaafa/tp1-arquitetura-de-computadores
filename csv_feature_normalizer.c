#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <xmmintrin.h>

char * method_name(int method) { 
    if (method == 1)
        return "Newton-Raphson";
    else if (method == 2)
        return "SSE";
    else if(method == 3)
        return "Naive";
    return "";
}

void menu() {
    printf("\n\n");
    printf("  ____ ______     __    _   _                            _ _              \n");
    printf(" / ___/ ___\\ \\   / /   | \\ | | ___  _ __ _ __ ___   __ _| (_)_______ _ __ \n");
    printf("| |   \\___ \\\\ \\ / /    |  \\| |/ _ \\| '__| '_ ` _ \\ / _` | | |_  / _ \\ '__|\n");
    printf("| |___ ___) |\\ V /     | |\\  | (_) | |  | | | | | | (_| | | |/ /  __/ |   \n");
    printf(" \\____|____/  \\_/____  |_| \\_|\\___/|_|  |_| |_| |_|\\__,_|_|_/___\\___|_|   \n");
    printf("               |_____|                                                    \n");
    printf("\n\n");
}

void print_resource_usage(const char* label, struct rusage* usage) {
    printf("%s\n", label);
    printf("User time: %ld.%06ld seconds\n", usage->ru_utime.tv_sec, usage->ru_utime.tv_usec);
    printf("System time: %ld.%06ld seconds\n", usage->ru_stime.tv_sec, usage->ru_stime.tv_usec);
    printf("Maximum resident set size: %ld kilobytes\n", usage->ru_maxrss);
}

void print_start_benchmark(int num_elements, int num_dimensions, int method, int repetitions){
    printf("\n===========================================\n");
    printf("🏁 Starting Benchmark\n");
    printf("Elements      : %d\n", num_elements);
    printf("Dimensions    : %d\n", num_dimensions);
    printf("Method        : %s\n", method_name(method));
    printf("Repetitions   : %d\n", repetitions);
    printf("===========================================\n\n");
}

void print_benchmark_results(float total_user_time, float total_system_time, int repetitions){
    printf("\n===========================================\n");
    printf("🔎 Results\n");
    printf("Total User Time   : %.6f seconds\n", total_user_time);
    printf("Total System Time : %.6f seconds\n", total_system_time);
    printf("Average User Time : %.6f seconds\n", total_user_time / repetitions);
    printf("Average System Time : %.6f seconds\n", total_system_time / repetitions);
    printf("===========================================\n\n");
}

// Função de Newton-Raphson para calcular a raiz quadrada inversa (otimização Quake III)
float newton_raphson_inv_sqrt(float x) {
    // Primeira estimativa inicial (bit-level hack)
    float xhalf = 0.5f * x;
    int i = *(int*)&x;                   // Convert float para inteiro
    i = 0x5f3759df - (i >> 1);           // Manipulação de bits (truque do Quake III)
    x = *(float*)&i;                     // Converter de volta para float

    // Uma iteração de Newton-Raphson para refinar a estimativa
    x = x * (1.5f - (xhalf * x * x));  // Fórmula de Newton-Raphson
    
    return x;
}

// Função utilizando SSE para calcular a raiz quadrada inversa
float sse_inv_sqrt(float x) {
    __m128 input = _mm_set_ss(x);     // Carrega 'x' em um registrador SSE
    __m128 result = _mm_rsqrt_ss(input);    // Calcula a raiz quadrada inversa
    return _mm_cvtss_f32(result);           // Extrai o float do registrador SSE
}

void normalize_feature_vector(float* features, int length, int method) {
    float sum = 0.0f;
    for (int i = 0; i < length; i++) {
        sum += features[i] * features[i];
    }
    
    float inv_sqrt;
    if (method == 1) {
        inv_sqrt = newton_raphson_inv_sqrt(sum);  // Usando Newton-Raphson
    } else if (method == 2) {
        inv_sqrt = sse_inv_sqrt(sum);  // Usando SSE
    } else {
        inv_sqrt = 1.0f / sqrt(sum);  // Método padrão (naïve)
    }

    for (int i = 0; i < length; i++) {
        features[i] *= inv_sqrt;
    }
}

// Função para ler dados de um arquivo CSV
float** read_csv(const char* filename, int* num_elements, int* num_dimensions) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Determine the number of elements and dimensions
    *num_elements = 0;
    *num_dimensions = 0;
    char line[4096];
    while (fgets(line, sizeof(line), file)) {
        if (*num_elements == 0) {
            char* token = strtok(line, ",");
            while (token) {
                (*num_dimensions)++;
                token = strtok(NULL, ",");
            }
        }
        (*num_elements)++;
    }
    rewind(file);

    // Allocate memory for the features
    float** features = (float**)malloc(*num_elements * sizeof(float*));
    for (int i = 0; i < *num_elements; i++) {
        features[i] = (float*)malloc(*num_dimensions * sizeof(float));
    }

    // Read the data
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        int j = 0;
        char* token = strtok(line, ",");
        while (token) {
            features[i][j++] = atof(token);
            token = strtok(NULL, ",");
        }
        i++;
    }

    fclose(file);
    return features;
}

// Função para medir o tempo de execução usando a biblioteca 'resources'
void get_resource_usage(struct rusage* usage) {
    getrusage(RUSAGE_SELF, usage);
}

float** copy_features(float** original, int num_elements, int num_dimensions) {
    float** copy = (float**)malloc(num_elements * sizeof(float*));
    for (int i = 0; i < num_elements; i++) {
        copy[i] = (float*)malloc(num_dimensions * sizeof(float));
        memcpy(copy[i], original[i], num_dimensions * sizeof(float));
    }
    return copy;
}

void free_features(float** features, int num_elements) {
    for (int i = 0; i < num_elements; i++) {
        free(features[i]);
    }
    free(features);
}

void benchmark_method(float** features, int num_elements, int num_dimensions, int method) {
    struct rusage start_usage, end_usage;
    get_resource_usage(&start_usage);

    for (int i = 0; i < num_elements; i++) {
        normalize_feature_vector(features[i], num_dimensions, method);
    }

    get_resource_usage(&end_usage);

    print_resource_usage("Start Usage\n", &start_usage);
    print_resource_usage("\nEnd Usage\n", &end_usage);
}

void run_benchmark_multiple_times(float** features, int num_elements, int num_dimensions, int method, int repetitions) {
    print_start_benchmark(num_elements, num_dimensions, method, repetitions);

    double total_user_time = 0.0;
    double total_system_time = 0.0;

    struct rusage start_usage, end_usage;
    get_resource_usage(&start_usage);

    for (int i = 0; i < repetitions; i++) {
        float** features_copy = copy_features(features, num_elements, num_dimensions);

        for (int j = 0; j < num_elements; j++) {
            normalize_feature_vector(features_copy[j], num_dimensions, method);
        }

        free_features(features_copy, num_elements);
    }

    get_resource_usage(&end_usage);

    print_resource_usage("📈 Resource Usage - Start\n", &start_usage);
    print_resource_usage("\n📈 Resource Usage - End", &end_usage);

    total_user_time = (end_usage.ru_utime.tv_sec - start_usage.ru_utime.tv_sec) + 
                      (end_usage.ru_utime.tv_usec - start_usage.ru_utime.tv_usec) / 1e6;

    total_system_time = (end_usage.ru_stime.tv_sec - start_usage.ru_stime.tv_sec) + 
                        (end_usage.ru_stime.tv_usec - start_usage.ru_stime.tv_usec) / 1e6;

    print_benchmark_results(total_user_time, total_system_time, repetitions);
}


int main(int arc, char **argv, char **argenv) {
    menu();

    if (arc != 4 ) {
        printf("Usage: %s <csv_file.csv> <method_number> <repetitions>\n", argv[0]);
        printf("method_number: \n\t1 - Newton-Raphson, \n\t2 - SSE, \n\t3 - Naïve\n\t4 - All Methods (benchmark all methods)\n");
        exit(EXIT_FAILURE);
    }

    const char * filename = argv[1];
    int method = atoi(argv[2]);
    int repetitions = atoi(argv[3]);

    int num_elements, num_dimensions;
    float** features = read_csv(filename, &num_elements, &num_dimensions);

    if(method < 1 || method > 4){
        printf("Invalid method!\n");
        exit(EXIT_FAILURE);
    }

    if(method == 4){
        for(int i = 1; i <= 3; i++){
            run_benchmark_multiple_times(features, num_elements, num_dimensions, i, repetitions);
        }
    } 
    else{
        run_benchmark_multiple_times(features, num_elements, num_dimensions, method, repetitions);
    }

    // Free allocated memory
    for (int i = 0; i < num_elements; i++) {
        free(features[i]);
    }
    free(features);

    return 0;
}