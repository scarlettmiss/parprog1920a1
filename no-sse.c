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

int main() {
    double  ts, te;
    float *a,*b,*c;
    float *pa,*pb,*pc;
    float sum = 0,check = 0;

    a = (float*)malloc( N * M*sizeof(float));    //img array
	b = (float*)malloc( P * sizeof(float));    //pixel constant
	c = (float*)malloc( N * M *sizeof(float));	//final result

	if( a == NULL || b == NULL ||c == NULL) {
		exit(1);
	}
	//initialize arrays
    for(int i = 0; i <= N * M ; i++){
        a[i] = 2.0;
        c[i] = 1.0;
    }
    for(int i = 0; i < P; i++){
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
    for(int i = 1; i< N-1 ; i++){//ignore first and last row
        for(int j = 1; j< M-1;j++){//ignore first and last column
            pa = a + i*N-2;//go to next pos of a
            pb = b;   //go to begining of b
            sum = 0;  //reset sum
            for(int k = 0; k < P; k++){//sum of neighbor 8 pixels
                sum += (*pa) * (*pb);
                pa ++;
                pb ++;
            }
            *pc = sum;
            pc ++;
        }
    }

    get_walltime(&te);
    check = 0;
    for(int i = 0 ;i < P;i++){
        check += 2*b[i];
    }
    pc = c;
    //c without borders
    for(int i = 0; i< (N -2) * (M -2) ; i++){
        if(*pc != check) {
            printf("Error at c[%d], excpected '%f', get '%f'\n", i, check, *pc);
		}
		pc++;
    }
    printf("\n");

    //time it took
    printf("%lf\n", te - ts);
    // compute avg array element accesses /sec (total NROWSxMCOLSxPIXELS(1load+1store) element accesses)
    printf("%lf\n", (1.0*N*M*P)/((te-ts)*1e6));

    free(a);
    free(b);
    free(c);
    return 0;
}
