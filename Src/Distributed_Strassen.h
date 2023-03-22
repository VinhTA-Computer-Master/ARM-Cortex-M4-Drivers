/*
 * Distributed_Strassen.c
 *
 *  Created on: Mar 4, 2023
 *      Author: vinht
 */

// This app parallelizes Strassen Algorithm on square matrices with STM32F407 and Adruino Due
/*
 *   a   *   b   = 			 c
 * |A|B| * |E|F| = | AE + BG | AF + BH |
 * |C|D|   |G|H|   | CE + DG | CF + DH |
 *
 * A, B, C square matrices of same size, dimension divisible by 2
 * # of intermediate matrices = 4 + 4 + 4*2 + 4 = 20
 * A = (40x40) = 1600*20 = 32k elements
 *
 * To minimize memory usage:
 *  Do top row of C first then free A & B matrices
 *  Then do 2nd row of C and free C & D & E & F & G & H
 *  Then initialize sub-C matrices (4) and do matrix additions
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../Driver/Inc/stm32f4xx.h"
#include "../Driver/Inc/gpio_driver.h"
#include "../Driver/Inc/sysclk_driver.h"
#include "../Driver/Inc/LCD2004_I2C_driver.h"
#include "../Driver/Inc/RNG_driver.h"
#include "Matrix_Chain_Multiplication.h"

void matrix_addition(unsigned ** A, unsigned ** B, unsigned ** C, short row, short col){
	for(short r=0; r<row; r++)
		for(short c=0; c<col; c++)
			C[r][c] = A[r][c] + B[r][c];
}

void Strassen(){
	LCD2004_Config_t LCD2004_conf = {true, true, true, true, 2, 0, 0};
	LCD2004_Init(&LCD2004_conf);
	LCD2004_WriteString(&LCD2004_conf, "Matrix Multiply", 15);
	LCD2004_SetCursor(&LCD2004_conf, 1, 0);
	LCD2004_WriteString(&LCD2004_conf, "Button to Start", 15);

	start = false;
	while(!start);
	LCD2004_ClearScreen(&LCD2004_conf);
	LCD2004_WriteString(&LCD2004_conf, "Transferring Matrix", 19);

	Parallel_Communication_Init();

	unsigned **A, **B, **C, **D, **E, **F, **G, **H, \
		**AE, **BG, **AF, **BH, **CE, **DG, **CF, **DH, **C11, **C12, **C21, **C22;

	short N = 40, r;

	A = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		A[r] = (unsigned*) calloc(N, sizeof(unsigned));

	B = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		B[r] = (unsigned*) calloc(N, sizeof(unsigned));

	C = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		C[r] = (unsigned*) calloc(N, sizeof(unsigned));

	D = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		D[r] = (unsigned*) calloc(N, sizeof(unsigned));

	E = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		E[r] = (unsigned*) calloc(N, sizeof(unsigned));

	F = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		F[r] = (unsigned*) calloc(N, sizeof(unsigned));

	G = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		G[r] = (unsigned*) calloc(N, sizeof(unsigned));

	H = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		H[r] = (unsigned*) calloc(N, sizeof(unsigned));

	AE = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		AE[r] = (unsigned*) calloc(N, sizeof(unsigned));

	BG = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		BG[r] = (unsigned*) calloc(N, sizeof(unsigned));

	AF = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		AF[r] = (unsigned*) calloc(N, sizeof(unsigned));

	BH = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		BH[r] = (unsigned*) calloc(N, sizeof(unsigned));

	CE = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		CE[r] = (unsigned*) calloc(N, sizeof(unsigned));

	DG = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		DG[r] = (unsigned*) calloc(N, sizeof(unsigned));

	CF = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		CF[r] = (unsigned*) calloc(N, sizeof(unsigned));

	DH = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		DH[r] = (unsigned*) calloc(N, sizeof(unsigned));

	unsigned elapsed = get_sys_tick();

	matrix_multiply(A, E, AE, N, N, N);
	matrix_multiply(B, G, BG, N, N, N);
	matrix_multiply(A, F, AF, N, N, N);
	matrix_multiply(B, H, BH, N, N, N);

	elapsed = get_sys_tick() - elapsed;

	for(r = 0; r < N; r++){
		free(A[r]);
		free(B[r]);
	}
	free(A);
	free(B);

	unsigned elapsed2 = get_sys_tick();

	matrix_multiply(C, E, CE, N, N, N);
	matrix_multiply(D, G, DG, N, N, N);
	matrix_multiply(C, F, CF, N, N, N);
	matrix_multiply(D, H, DH, N, N, N);

	elapsed2 = get_sys_tick() - elapsed2;

	for(r = 0; r < N; r++){
		free(C[r]);
		free(D[r]);
		free(E[r]);
		free(F[r]);
		free(G[r]);
		free(H[r]);
	}
	free(C);
	free(D);
	free(E);
	free(F);
	free(G);
	free(H);

	C11 = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		C11[r] = (unsigned*) calloc(N, sizeof(unsigned));

	C12 = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		C12[r] = (unsigned*) calloc(N, sizeof(unsigned));

	C21 = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		C21[r] = (unsigned*) calloc(N, sizeof(unsigned));

	C22 = (unsigned**) malloc(N*sizeof(unsigned*));
	for(r=0; r<N; r++)
		C22[r] = (unsigned*) calloc(N, sizeof(unsigned));

	unsigned elapsed3 = get_sys_tick();

	matrix_addition(AE, BG, C11, N, N);
	matrix_addition(AF, BH, C12, N, N);
	matrix_addition(CE, DG, C21, N, N);
	matrix_addition(CF, DH, C22, N, N);

	elapsed3 = get_sys_tick() - elapsed3;

	unsigned total = elapsed + elapsed2 + elapsed3;
	unsigned usec = total/(apb1_timer/1000);

	for(r = 0; r < N; r++){
		free(AE[r]);
		free(AF[r]);
		free(BG[r]);
		free(BH[r]);
		free(CE[r]);
		free(CF[r]);
		free(DG[r]);
		free(DH[r]);
		free(C11[r]);
		free(C12[r]);
		free(C21[r]);
		free(C22[r]);
	}
	free(AE);
	free(AF);
	free(BG);
	free(BH);
	free(CE);
	free(CF);
	free(DG);
	free(DH);
	free(C11);
	free(C12);
	free(C21);
	free(C22);

	A = (unsigned**) malloc(N*2*sizeof(unsigned*));
	for(r = 0; r < (N*2); r++)
		A[r] = (unsigned*) calloc(N*2, sizeof(unsigned));

	B = (unsigned**) malloc(N*2*sizeof(unsigned*));
	for(r=0; r < (N*2); r++)
		B[r] = (unsigned*) calloc(N*2, sizeof(unsigned));

	C = (unsigned**) malloc(N*2*sizeof(unsigned*));
	for(r=0; r < (N*2); r++)
		C[r] = (unsigned*) calloc(N*2, sizeof(unsigned));

	elapsed = get_sys_tick();
	matrix_multiply(A, B, C, N*2, N*2, N*2);
	elapsed = get_sys_tick() - elapsed;

	free(A);
}
