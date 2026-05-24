#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>
//#include <input.h>
#include"input.h"
// #include "input_13.h"
// #include "input_15.h"
// #include "input_20.h"

/* 1. THE STRUCTURE DEFINITION */
// Placing this here allows main.c AND functions.c to use it.
typedef struct {
    int id;
    int period;
    int exec_time;
    int remaining;
    int abs_deadline;
} Task;


// System Structure
//typedef struct{
//	int dim;
//	float Ad[10][10];
//	float Bd[10];
//	float K[10];
//	float A1[15][15];
//	float A0[15][15];
//	float init_vect[10];
//	float init_input;
//	float ref[10];
//	int h;
//	int r;
//	int l;
//	float P_Lya[20][20];
//	float min_eig_lya;
//	float d_safe;
//} pc_system;

//void remove_newline(char* str);
//void parse_vector_line(char* line, float* vec, int size);
//float parse_single_float(char* line);
//void load_config(const char* filename, pc_system* cfg);
void matrix_vector_multiply(int n, float matrix[10][10], float *vector, float* mat_vec_prod);
void vector_int_multiply(int n,float* vector,float scalar,float* vec_const_prod);
void add_vector(int n, float* vector1, float* vector2, float* add_vec);
void sub_vector(int n, float* vector1, float* vector2, float* sub_vec);
void vector_Vector_mul(int n, float* vector1, float* vector2, float* vec_vec_prod);
void vector_concat(int n1, float* vec1, int n2, float* vec2, float* concat_vect) ;
void remove_first_element(Task* list, int size_list);
void binary_search_add_element(Task* list, int size_list, Task new_task);
int srspPriorityGen(int j, pc_system cs, float* x_nom_0, float u_nom_0, float* x_hm_0, float u_hm_0,
		float* x_nom_1, float* u_nom_1, float* x_hm_1, float* u_hm_1, int lst, int tot, int hit_or_miss);
int randi_range(int min, int max);
int compare_int(const void *a, const void *b);
void obtain_sudden_task_data(
    int horizon,
    int *rand_time,
    int arrival[],
    int periods[],
    int executionTimes[],
    int hard_or_weakly_hard[]
);        
void obtain_weakly_hard_m_k(
    int hard_or_weakly_hard,
    int m_k_firm[2]
);
int weakly_hard_requirement_check(int m_k_firm[2], int zer, int ind);
#endif
