#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define N 1000
#define M 1000
#define P 9



void get_walltime(double *wct){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    *wct = (double)(tp.tv_sec + tp.tv_usec/1000000.0);
}

int max_int(int a, int b){
	return (a < b) ? b:a;
}

int main() {
    double  ts, te;
    float *a,*b,*c;
    float *pa,*pb,*pc;
    float *pabase;
    float sum = 0,check = 0;

    //img array
    a = (float*)malloc( N * M *sizeof(float));
    //pixel constant
	b = (float*)malloc( P * sizeof(float));
	//final result
	c = (float*)malloc( N * M *sizeof(float));

	if( a == NULL || b == NULL ||c == NULL) {
		exit(1);
	}
	//initialize arrays
    for(int i = 0; i <= N * M ; i++){
        a[i] = 2.0;
        c[i] = 1.0;
    }

    for(int i = 0; i < P ; i++){
	    if( i == 4) b[i] = 5;
	    else b[i] = 0.5;
	}


    get_walltime(&ts);
    /**
     * if N=4 and M=4
     * |X|X|X|X|
     * |X|C|C|X|
     * |X|C|C|X|
     * |X|X|X|X|
     * x=ingored c=counted
     */
    pc = c;
    pabase = a;

    for(int i = 1; i<N-1 ; i++){//N rows of A and C
        for(int j = 1; j<M-1;j++){// for all columns of A
            pb = b;//go to begining of b
            pa = pabase; 	// start of A row to work on (repeated)
            sum = 0;  //reset sum
            //sum of neighbor 8 pixels
            for(int k = max_int(0,i-1); k < max_int(0,i-1) + 3; k++){//for sub array row
                for(int l = max_int(0,j-1); l < max_int(0,j-1) + 3; l++){//for sub array col
                    sum += (*pa) * (*pb);
                    pa++;
                    pb++;
                }
            }

            *pc = sum;
            // printf("c[%d,%d] = %f\n",i,j,*pc);
            pc++;

        }
        pabase += N;
    }

    get_walltime(&te);
    check = 0;
    for(int i=0 ;i<P;i++){
        check += 2*b[i];
    }
    pc = c;
    for(int i = 0; i< (N -2) *(M -2)   ; i++){
        if(*pc != check) {
            printf("Error at c[%d], excpected '%f', get '%f'\n", i, check, *pc);
		}
		pc++;
    }

    //time it took
    printf("%lf\n", te - ts);
    // compute avg array element accesses /sec (total NROWSxMCOLSxPIXELS(1load+1store) element accesses)
    printf("%lf\n", (1.0*N*M*P)/((te-ts)*1e6));

    free(a);
    free(b);
    free(c);
    return 0;
}
