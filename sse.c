#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <emmintrin.h>

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
    float *pa0,*pa1,*pa2,*pa3;
    float *pb0,*pb1,*pb2,*pb3;
    float *pc;
    float *pabase;
    float sum = 0,check = 0;

    __m128 a0,a1,a2,a3;	// a quad of a row values
    __m128 b0,b1,b2,b3;	// quads of b row values
    __m128 sum0,sum1,sum2,sum3;	// quads of C row values (partial sums)
    __m128 t0,t1,t2;
    //img array
   	int check_a = posix_memalign((void**)&a, 16, N * M *sizeof(float));
    if(check_a != 0) {
        return 1;
    }
    int check_b = posix_memalign((void**)&b, 16, P *sizeof(float));
    if(check_b != 0) {
        free(a);
        return 1;
    }
    int check_c = posix_memalign((void**)&c, 16, N * M *sizeof(float));
    if(check_c != 0) {
        free(a);
        free(b);
        return 1;
    }

	if( a == NULL || b == NULL ||c == NULL) {
		exit(1);
	}
	//initialize arrays
    for(int i = 0; i <= N * M ; i++){
        a[i] = 2.0;
        c[i] = 1.0;
    }

    for(int i = 0; i < 16 ; i++){
	    if( i == 4) b[i] = 5;
	    else if(i < P) b[i] = 0.5;
	    else b[i] = 0;
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

    for(int i = 1; i<N-1 ; i+=4){//N rows of A and C
        for(int j = 1; j<M-1;j+=4){// for all columns of A
            pa0 = pabase;//go to pabase
            pa1 = pabase + 4;
            pa2 = pabase + 8;
            pa3 = pabase + 12;

            pb0 = b;//go to begining of b
            pb1 = pb0+4;
            pb2 = pb0+8;
            pb3 = pb0+12;


            //reset sum
            sum0 = _mm_xor_ps(sum0,sum0);
            sum1 = _mm_xor_ps(sum1,sum1);
            sum2 = _mm_xor_ps(sum2,sum2);
            sum3 = _mm_xor_ps(sum3,sum3);
            //sum of neighbor 8 pixels
            for(int k = max_int(0,i-1); k < max_int(0,i-1) + 3; k+=4){//for sub array row
                for(int l = max_int(0,j-1); l < max_int(0,j-1) + 3; l+=4){//for sub array col
                    a0 = _mm_load_ps(pa0);
                    a1 = _mm_load_ps(pa1);
                    a2 = _mm_load_ps(pa2);
                    a3 = _mm_load_ps(pa3);

                    b0 = _mm_load_ps(pb0);
                    b1 = _mm_load_ps(pb1);
                    b2 = _mm_load_ps(pb2);
                    b3 = _mm_load_ps(pb3);

                    sum0 = _mm_add_ps(sum0,_mm_mul_ps(a0,b0));
                    sum1 = _mm_add_ps(sum1,_mm_mul_ps(a1,b1));
                    sum2 = _mm_add_ps(sum2,_mm_mul_ps(a2,b2));
                    sum3 = _mm_add_ps(sum3,_mm_mul_ps(a3,b3));

                    pa0+=4;pa1+=4;pa2+=4;pa3+=4;
                    pb0+=4;pb1+=4;pb2+=4;pb3+=4;
                }
            }
            t0 = _mm_add_ps(_mm_unpackhi_ps(sum0,sum1),_mm_unpacklo_ps(sum0,sum1));
            t1 = _mm_add_ps(_mm_unpackhi_ps(sum2,sum3),_mm_unpacklo_ps(sum2,sum3));
            t2 = _mm_add_ps(_mm_unpackhi_ps(t0,t1),_mm_unpacklo_ps(t0,t1));
            _mm_store_ps(pc,_mm_shuffle_ps(t2,t2,0xd8));
            // printf("c[%d,%d] = %f\n",i,j,*pc);
            pc+=4;
        }
        pabase += 4*N;
    }

    get_walltime(&te);
    check = 18.0;
    // for(int i=0 ;i<P;i++){
    //     check += 2*b[i];
    // }
    // pc = c;
    for(int i = 0; i< (N -2) *(M -2)   ; i++){
        if(*pc != check) {
            //printf("Error at c[%d], excpected '%f', get '%f'\n", i, check, *pc);
		}
		pc++;
    }

    //time it took
    printf("%lf\n", te - ts);
    // compute avg array element accesses /sec (total NROWSxMCOLSxPIXELS(1load+1store) element accesses)
    printf("%lf\n", (1.0*N*M*P)/((te-ts)*1e6));

    free(a);
    free(b);
    // free(c);
    return 0;
}
