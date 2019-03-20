# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <omp.h>
/*
 * Heat equation
 * Creates png file with simulated heat equation
 * Uses OpenMP module for parallelism
 * Uses math.h module to find power of the location change rate
 */

void write_to_output(double* loc[], double* temp[], int size){
    FILE * fp;
    /* open the file for writing*/
    fp = fopen("output.dat","w");
    fprintf(fp,"# X        Y        Z\n");
    for(int i =0; i < size; ++i){
        for(int b =0; b < size; ++b){
            fprintf(fp,"  %f %f %f\n",loc[i][b],loc[b][i],temp[i][b]);
        }
    }
    fclose(fp);
}

void print_data(double* arr[],int size){
    //Print data from 2d array
    for (int i = 0; i < size; i++){
        for(int b = 0; b < size; b++){
            printf("%f\n", arr[i][b]);
        }
    }
}

int main(){
    /*
     * Main Function
     * Contains code for simulating transfer by using Heat transfer equation
     * Uses malloc to allocate memory to data holders
     */
    int size = 500;
    double t = 9e-7;
    double a_var = 0.01;


    double* loc[size];
    for(int i = 0; i < size; i++){
        loc[i] = malloc(size * sizeof(double));
    }

    double* temp[size];
    for (int i = 0; i < size; i++){
        temp[i] = malloc(size * sizeof(double));
    }

    double* temp_rate[size];
    for (int i = 0; i < size; i++){
        temp_rate[i] = malloc(size * sizeof(double));
    }
    double rate = 0.0002;
    double outer = 20.0;
    double inner = 50.0;

    /*
     * Assigning values to data holders via OpenMP parallelism
     */

#pragma omp parallel shared (loc, temp)
    {
        double value = 0;
#pragma omp for
        for (int i = 0; i < 500; i++) {
            for (int b = 0; b < 500; b++) {
                loc[i][b] = value;
                value += rate;
            }
            value = 0;
        }
#pragma omp for
        for (int i = 0; i < size; i++) {
            for (int b = 0; b < size; b++) {
                if (i < 150 || i > 349) {
                    temp[i][b] = outer;
                } else if (b < 150 || b > 349) {
                    temp[i][b] = outer;
                } else{
                    temp[i][b] = inner;
                }
            }
        }
    }
    printf("Initial Temperature data has been created \n");
    write_to_output(loc,temp,size);

    /*
     * Calculating heat equation
     */
#pragma omp parallel shared(temp, temp_rate)
    {
#pragma omp for
        for(int a= 0; a < 10000; a++) {
            for (int i = 1; i < size - 1; i++) {
                for (int b = 1; b < size - 1; b++) {
                    double x_part = (temp[i + 1][b] - 2 * temp[i][b] + temp[i - 1][b]) / pow(rate,2);
                    double y_part = (temp[i][b+1] - 2 * temp[i][b] + temp[i][b-1]) / pow(rate,2);
                    temp_rate[i][b] = (a_var*a_var)*(x_part+y_part);
                    temp[i][b] = temp[i][b] + temp_rate[i][b] * t;
                }
            }
        }
    }
    print_data(temp,size);
}