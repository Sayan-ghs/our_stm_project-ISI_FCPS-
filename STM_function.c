#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include <string.h>
#include"STM_function.h"
// #include "input.h"
#include "input_13.h"
// #include "input_15.h"
// #include "input_20.h"

#define Line_size 256  // Or 512 if your input lines are very long


// Matrix A (order n*n ) multiplied by vector x (order n*1). Both float.
//float mat_vec_prod[10];
//float concat_vect[10];
//float vec_const_prod[10];
//
//
//float x_hm_1[10]; x_hm_0[10];
//float x_nom_1[10]; x_nom_0[10];
//float u_hm_1, u_nom_1, u_hm_0, u_nom_0;


//void remove_newline(char* str) {
//        size_t len = strlen(str);
//        if (len > 0 && str[len - 1] == '\n') {
//                str[len - 1] = '\0';
//        }
//}
//
//void parse_vector_line(char* line,
//                       float* vec,
//                       int size){
//
//    char* ptr = strchr(line, '=');
//    if(ptr)
//        ptr++;
//    char* token = strtok(ptr, " ");
//    int i = 0;
//    while(token != NULL && i < size){
//        vec[i] = atof(token);
//        token = strtok(NULL, " ");
//        i++;
//    }
//}

//float parse_single_float(char* line){
//    char* ptr = strchr(line, '=');
//    if(ptr)
//        ptr++;
//    return atof(ptr);
//}


//void load_config(const char* filename, pc_system* cfg){
//        FILE* fp = fopen(filename,"r");
//
//        if(fp == NULL){
//                printf("cannot open file\n");
//                return;
//        }
//
//        char line[Line_size];
//
//        while(fgets(line,sizeof(line),fp)){
//                remove_newline(line);
//                if(strlen(line) == 0){
//                        continue;
//                }
//
//                if(strncmp(line,"dim",3)==0){
//                        sscanf(line,"dim = %d",&cfg->dim);
//                }
//                else if(strncmp(line,"Ad",2)==0){
//                        for(int i=0;i<cfg->dim;i++){
//                                if(i == 0){
//                                        // First row is on the same line as "Ad ="
//                                        char* ptr = strchr(line, '=');
//                                        if(ptr){
//                                                ptr++;
//                                        }
//                                        char* token = strtok(ptr, " ");
//                                        int j = 0;
//                                        while(token != NULL && j<cfg->dim){
//                                                cfg->Ad[i][j] = atof(token);
//                                                token = strtok(NULL," ");
//                                                j++;
//                                        }
//                                } else {
//                                        fgets(line, sizeof(line), fp);
//                                        remove_newline(line);
//                                        char* token = strtok(line, " ");
//                                        int j = 0;
//                                        while(token != NULL && j < cfg->dim){
//                                        cfg->Ad[i][j] = atof(token);
//                                        token = strtok(NULL, " ");
//                                        j++;
//                                        }
//                                }
//                        }
//                }
//                else if(strncmp(line, "Bd", 2)==0){
//                        for(int i = 0; i<cfg->dim;i++){
//                                if(i == 0){
//                                        char* ptr = strchr(line, '=');
//                                        if(ptr) ptr++;
//                                        cfg->Bd[i] = atof(ptr);
//                                } else {
//                                        fgets(line,sizeof(line),fp);
//                                        remove_newline(line);
//                                        cfg->Bd[i] = atof(line);
//                                }
//                        }
//                }
//                else if(strncmp(line, "K =", 3)==0){
//                        char* ptr = strchr(line,'=');
//                        if(ptr) ptr++;
//                        char* token = strtok(ptr, " ");
//                        int i = 0;
//                        while(token != NULL && i<cfg->dim){
//                                cfg->K[i] = atof(token);
//                                token = strtok(NULL," ");
//                                i++;
//                        }
//                }
//                else if(strncmp(line, "A1", 2)==0){
//                        for(int i=0;i<(cfg->dim)+1;i++){
//                                if(i == 0){
//                                        char* ptr = strchr(line, '=');
//                                        if(ptr) ptr++;
//                                        char* token = strtok(ptr, " ");
//                                        int j = 0;
//                                        while(token != NULL && j < (cfg->dim)+1){
//                                                cfg->A1[i][j] = atof(token);
//                                                token = strtok(NULL, " ");
//                                                j++;
//                                        }
//                                } else {
//                                        fgets(line, sizeof(line),fp);
//                                        remove_newline(line);
//                                        char* token = strtok(line, " ");
//                                        int j = 0;
//                                        while(token != NULL && j< (cfg->dim)+1){
//                                                cfg->A1[i][j] = atof(token);
//                                                token = strtok(NULL, " ");
//                                                j++;
//                                        }
//                                }
//                        }
//                }
//                else if(strncmp(line, "A0", 2)==0){
//                        for(int i=0;i<(cfg->dim)+1;i++){
//                                if(i == 0){
//                                        char* ptr = strchr(line, '=');
//                                        if(ptr) ptr++;
//                                        char* token = strtok(ptr," ");
//                                        int j = 0;
//                                        while(token != NULL && j<(cfg->dim)+1){
//                                                cfg->A0[i][j] = atof(token);
//                                                token = strtok(NULL," ");
//                                                j++;
//                                        }
//                                } else {
//                                        fgets(line, sizeof(line),fp);
//                                        remove_newline(line);
//                                        char* token = strtok(line, " ");
//                                        int j = 0;
//                                        while(token != NULL && j<(cfg->dim)+1){
//                                                cfg->A0[i][j] = atof(token);
//                                                token = strtok(NULL," ");
//                                                j++;
//                                        }
//                                }
//                        }
//                }
//                else if(strncmp(line, "init_vect",9)==0){
//                        parse_vector_line(line, cfg->init_vect,cfg->dim);
//                }
//                else if(strncmp(line, "init_input", 10)==0){
//                        cfg->init_input = parse_single_float(line);
//                }
//                else if(strncmp(line, "ref =", 5)==0){
//                        parse_vector_line(
//                        line,
//                        cfg->ref,
//                        cfg->dim
//                        );
//                }
//                else if(strncmp(line, "h =", 3)==0){
//                        cfg->h = parse_single_float(line);
//                }
//                else if(strncmp(line,"r =",3)==0){
//                        cfg->r = parse_single_float(line);
//                }
//                else if(strncmp(line, "l =",3)==0){
//                        cfg->l = parse_single_float(line);
//                }
//                else if(strncmp(line, "P_Lya", 5)==0){
//                        for(int i=0;i<2*(cfg->dim);i++){
//                                char* row_data;
//                                if(i == 0){
//                                        row_data = strchr(line, '=');
//                                        if(row_data) row_data++;
//                                }
//                                else{
//                                        fgets(line, sizeof(line),fp);
//                                        remove_newline(line);
//                                        row_data = line;
//                                }
//                                char* token = strtok(row_data," ");
//                                int j = 0;
//                                while(token != NULL && j<2*(cfg->dim)){
//                                        cfg->P_Lya[i][j] = atof(token);
//                                        token = strtok(NULL," ");
//                                        j++;
//                                }
//                        }
//                }
//                else if(strncmp(line, "min_eig_lya", 11)==0){
//                        cfg->min_eig_lya = parse_single_float(line);
//                }
//                else if(strncmp(line, "d_safe", 6)==0){
//                        cfg -> d_safe = parse_single_float(line);
//                }
//        }
//        fclose(fp);
//}

void matrix_vector_multiply(int n, float matrix[10][10], float *vector, float* mat_vec_prod){
        for(int i=0; i<n;i++){
        	    mat_vec_prod[i] = 0;
                for(int j=0; j<n; j++){
                	mat_vec_prod[i] += matrix[i][j] * vector[j];
                }
        }
}

//int * vector multiply
void vector_int_multiply(int n,float* vector,float scalar,float* vec_const_prod){
        for(int j=0; j<n; j++){
        	vec_const_prod[j] = vector[j]*scalar;
        }
}

// vector add, mul

void add_vector(int n, float* vector1, float* vector2, float* add_vec){
        for(int i=0; i<n;i++){
                add_vec[i] = vector1[i] + vector2[i];
        }
}

void sub_vector(int n, float* vector1, float* vector2, float* sub_vec){
        for(int i=0; i<n;i++){
                sub_vec[i] = vector1[i] - vector2[i];
        }
}

//vector_vector multiply
void vector_Vector_mul(int n, float* vector1, float* vector2, float* vec_vec_prod){
        *vec_vec_prod = 0;
        for(int i=0; i<n; i++){
               * vec_vec_prod += vector1[i] * vector2[i];
        }
}

//concatenate vector

void vector_concat(int n1, float* vec1, int n2, float* vec2, float* concat_vect) {
    for(int i = 0; i < n1; i++)
          concat_vect[i] = vec1[i];
    for(int j = 0; j < n2; j++)
        concat_vect[n1 + j] = vec2[j];
}


//int r, int l, int dim, float *A_d, float *B_d, float *P, float* x_nom_0, float u_nom_0, float* x_hm_0, float u_hm_0,
// float* x_nom_1, float u_nom_1, float* x_hm_1, float u_hm_1, float d_safe, float lamda_min,

void remove_first_element(Task* list, int size_list){
        for(int i=1;i<size_list;i++){
                list[i-1] = list[i];
        }
}

void binary_search_add_element(Task* list, int size_list, Task new_task){
        int low = 0;
        int high = size_list-1;
//        int mid;
        int count;

        // if no jobs currently in list
        if (size_list == 0){
                list[0] = new_task;

        }else{
                // when deadline of job is greater than the last deadline in the
                // list add next job at the end of the list
                if(new_task.abs_deadline >= list[size_list-1].abs_deadline){
                        list[size_list] = new_task;

                // else, add next_job following binary search in  list
                }else{

                        while(low != high){
                                int mid = (low + high)/2;
                                if(list[mid].abs_deadline <= new_task.abs_deadline){
                                        low = mid+1;
                                }else{
                                        high = mid;
                                }
                        }

                        if(low==high){
                                count = size_list;
                                while (count > low){
                                        list[count] = list[count-1];
                                        count--;
                                }
                                list[count]=new_task;
                        }
                }

        }
}

int srspPriorityGen(int j, pc_system cs, float* x_nom_0, float u_nom_0, float* x_hm_0, float u_hm_0,
		float* x_nom_1, float* u_nom_1, float* x_hm_1, float* u_hm_1, int lst, int tot, int hit_or_miss){

         float tmp1[10];
         float tmp2[10];
         float e1[10];
         float e0[10];
         float z[10];
         float zTPz;
         float u;
         float x_nom_2[10];
         float x_hm_2[10];


//         printf("\n lst: %d, tot: %d, hit_or_miss: %d \t", lst, tot, hit_or_miss);


        // %%%%%%%%%%%%%  For current step: plant-control execution %%%%%%%%%%%%%%%

        // Nominal current state
        matrix_vector_multiply(cs.dim,cs.Ad, x_nom_0, tmp1);
		vector_int_multiply(cs.dim, cs.Bd, u_nom_0, tmp2);
		add_vector(cs.dim, tmp1, tmp2, x_nom_1);


		// Nominal input
		sub_vector(cs.dim, x_nom_0, cs.ref, tmp1);
		vector_Vector_mul(cs.dim, cs.K, tmp1, &u);
		*u_nom_1 = (-1)* u;

		// Hit miss current state
		matrix_vector_multiply(cs.dim,cs.Ad, x_hm_0, tmp1);
		vector_int_multiply(cs.dim, cs.Bd, u_hm_0, tmp2);
		add_vector(cs.dim, tmp1,tmp2,x_hm_1);

		if(hit_or_miss == 1){
			// Hit-miss input (hit)
			sub_vector(cs.dim, x_hm_0, cs.ref, tmp1);
			vector_Vector_mul(cs.dim, cs.K, tmp1, &u);
			*u_hm_1 = (-1)* u;

		} else if(hit_or_miss == 0){
			//miss
			*u_hm_1 = u_hm_0;
		}



		float dp_N_H;
		float denominator;
		float part1;
		float part2;
		float gu;

//		denominator = cs.l - ceil(cs.r * cs.l);
		denominator = cs.l - cs.r;
		part1 = floor((cs.l / denominator) * (tot + 1));
		part2 = floor(tot * (cs.l/denominator));
		gu = part1 - part2;

//		printf(" gu: %f \n",gu);

        if((j+1)-lst>=gu || lst == -1){

        	    // x_nom_2
                matrix_vector_multiply(cs.dim,cs.Ad, x_nom_1, tmp1);
                vector_int_multiply(cs.dim, cs.Bd, *u_nom_1, tmp2);
                add_vector(cs.dim, tmp1, tmp2, x_nom_2);

                // x_hm_2
                matrix_vector_multiply(cs.dim,cs.Ad,x_hm_1, tmp1);
                vector_int_multiply(cs.dim, cs.Bd, *u_hm_1, tmp2);
                add_vector(cs.dim, tmp1,tmp2,x_hm_2);


                sub_vector(cs.dim,x_nom_2,x_hm_2, e1);
                sub_vector(cs.dim,x_nom_1,x_hm_1, e0);

                vector_concat(cs.dim, e1, cs.dim, e0, z);
                // printf("\n z : \t");
                // for(int x = 0; x<2*cs.dim;x++){
                //         printf("%f ",z[x]);
                // }

                matrix_vector_multiply(2*cs.dim,cs.P_Lya, z, tmp1);
                // printf("\n tmp1 : \t");
                // for(int x = 0; x<2*cs.dim;x++){
                //         printf("%f ",tmp1[x]);
                // }
                vector_Vector_mul(2*cs.dim, z, tmp1, &zTPz);

                dp_N_H = sqrtf(zTPz);
                // printf("\n dp_n_h : %f, zTPz : %f\n",dp_N_H, zTPz);

                if(dp_N_H <= sqrtf(2* cs.min_eig_lya) * cs.d_safe){
                        return 0;
                }
                else{
                        return 1;
                }
        }
        else{
                return 1;
        }

}

