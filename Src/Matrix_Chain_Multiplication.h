/*
 * Matrix_Chain_Multiplication.c
 *
 *  Created on: Mar 1, 2023
 *      Author: vinht
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../Driver/Inc/stm32f4xx.h"
#include "../Driver/Inc/gpio_driver.h"
#include "../Driver/Inc/sysclk_driver.h"
#include "../Driver/Inc/LCD2004_I2C_driver.h"
#include "../Driver/Inc/RNG_driver.h"

extern volatile bool start;
unsigned total_comm_cycle = 0;
unsigned total_comm_byte = 0;

typedef struct{
	GPIO_Handle_t EN;
	GPIO_Handle_t D0;
	GPIO_Handle_t D1;
	GPIO_Handle_t D2;
	GPIO_Handle_t D3;
	GPIO_Handle_t D4;
	GPIO_Handle_t D5;
	GPIO_Handle_t D6;
	GPIO_Handle_t D7;
} Parallel_Transfer_Conf;

Parallel_Transfer_Conf TX, RX;

void TX_WriteData(uint8_t data){
	GPIO_WriteToPin(&(TX.D0), data & 1);
	GPIO_WriteToPin(&(TX.D1), data & (1 << 1));
	GPIO_WriteToPin(&(TX.D2), data & (1 << 2));
	GPIO_WriteToPin(&(TX.D3), data & (1 << 3));
	GPIO_WriteToPin(&(TX.D4), data & (1 << 4));
	GPIO_WriteToPin(&(TX.D5), data & (1 << 5));
	GPIO_WriteToPin(&(TX.D6), data & (1 << 6));
	GPIO_WriteToPin(&(TX.D7), data & (1 << 7));
}

uint8_t RX_ReadData(){
	return GPIO_ReadFromPin(&(RX.D0)) | (GPIO_ReadFromPin(&(RX.D1)) << 1) | (GPIO_ReadFromPin(&(RX.D2)) << 2) | (GPIO_ReadFromPin(&(RX.D3)) << 3) \
			| (GPIO_ReadFromPin(&(RX.D4)) << 4) | (GPIO_ReadFromPin(&(RX.D5)) << 5) | (GPIO_ReadFromPin(&(RX.D6)) << 6) | (GPIO_ReadFromPin(&(RX.D7)) << 7);
}

void Parallel_Communication_Init(){
	// TX Init
	//Parallel_Transfer_Conf TX = {};
	GPIO_Init(&(TX.D0), 'B', 8);
	GPIO_Init(&(TX.D1), 'B', 6);
	GPIO_Init(&(TX.D2), 'D', 7);
	GPIO_Init(&(TX.D3), 'D', 5);
	GPIO_Init(&(TX.D4), 'D', 3);
	GPIO_Init(&(TX.D5), 'D', 1);
	GPIO_Init(&(TX.D6), 'C', 12);
	GPIO_Init(&(TX.D7), 'C', 10);
	GPIO_Init(&(TX.EN), 'A', 10);

	GPIO_SetModer(&(TX.EN), MODER_OUTPUT);
	GPIO_SetModer(&(TX.D0), MODER_OUTPUT);
	GPIO_SetModer(&(TX.D1), MODER_OUTPUT);
	GPIO_SetModer(&(TX.D2), MODER_OUTPUT);
	GPIO_SetModer(&(TX.D3), MODER_OUTPUT);
	GPIO_SetModer(&(TX.D4), MODER_OUTPUT);
	GPIO_SetModer(&(TX.D5), MODER_OUTPUT);
	GPIO_SetModer(&(TX.D6), MODER_OUTPUT);
	GPIO_SetModer(&(TX.D7), MODER_OUTPUT);

	GPIO_SetPull(&(TX.EN), PUPDR_UP);
	GPIO_SetPull(&(TX.D0), PUPDR_UP);
	GPIO_SetPull(&(TX.D1), PUPDR_UP);
	GPIO_SetPull(&(TX.D2), PUPDR_UP);
	GPIO_SetPull(&(TX.D3), PUPDR_UP);
	GPIO_SetPull(&(TX.D4), PUPDR_UP);
	GPIO_SetPull(&(TX.D5), PUPDR_UP);
	GPIO_SetPull(&(TX.D6), PUPDR_UP);
	GPIO_SetPull(&(TX.D7), PUPDR_UP);

	GPIO_WriteToPin(&(TX.EN), 0);

	// RX Init
	GPIO_Init(&(RX.D0), 'B', 9);
	GPIO_Init(&(RX.D1), 'B', 7);
	GPIO_Init(&(RX.D2), 'B', 5);
	GPIO_Init(&(RX.D3), 'D', 6);
	GPIO_Init(&(RX.D4), 'D', 4);
	GPIO_Init(&(RX.D5), 'D', 2);
	GPIO_Init(&(RX.D6), 'D', 0);
	GPIO_Init(&(RX.D7), 'C', 11);
	GPIO_Init(&(RX.EN), 'C', 9);

	GPIO_SetModer(&(RX.EN), MODER_INPUT);
	GPIO_SetModer(&(RX.D0), MODER_INPUT);
	GPIO_SetModer(&(RX.D1), MODER_INPUT);
	GPIO_SetModer(&(RX.D2), MODER_INPUT);
	GPIO_SetModer(&(RX.D3), MODER_INPUT);
	GPIO_SetModer(&(RX.D4), MODER_INPUT);
	GPIO_SetModer(&(RX.D5), MODER_INPUT);
	GPIO_SetModer(&(RX.D6), MODER_INPUT);
	GPIO_SetModer(&(RX.D7), MODER_INPUT);

	/*GPIO_SetPull(&(RX.EN), PUPDR_UP);
	GPIO_SetPull(&(RX.D0), PUPDR_UP);
	GPIO_SetPull(&(RX.D1), PUPDR_UP);
	GPIO_SetPull(&(RX.D2), PUPDR_UP);
	GPIO_SetPull(&(RX.D3), PUPDR_UP);
	GPIO_SetPull(&(RX.D4), PUPDR_UP);
	GPIO_SetPull(&(RX.D5), PUPDR_UP);
	GPIO_SetPull(&(RX.D6), PUPDR_UP);
	GPIO_SetPull(&(RX.D7), PUPDR_UP);*/

	GPIO_IRQConfig(&(RX.EN), GPIO_EXTI_EVENT, GPIO_EXTI_EDGE_FALL);
}

void transfer_matrix_pair(unsigned ** matrix1, unsigned ** matrix2, short X1, short Y1, short Y2){
	short r, c, offset;

	// Reset state in Arduino
	TX_WriteData(0x12);
	GPIO_WriteToPin(&(TX.EN), 0);
	usec_delay(50);
	GPIO_WriteToPin(&(TX.EN), 1);
	while(!READ(EXTI->PR, RX.EN.PinConfig.pin, 0x1));	// Wait for falling edge on EN
	SET(EXTI->PR, RX.EN.PinConfig.pin, 1, 0x1);

	// Transmit matrix1
	// TX size X/Y first
	TX_WriteData(X1);
	GPIO_WriteToPin(&(TX.EN), 0);
	usec_delay(50);
	GPIO_WriteToPin(&(TX.EN), 1);
	while(!READ(EXTI->PR, RX.EN.PinConfig.pin, 0x1));
	SET(EXTI->PR, RX.EN.PinConfig.pin, 1, 0x1);

	TX_WriteData(Y1);
	GPIO_WriteToPin(&(TX.EN), 0);
	usec_delay(50);
	GPIO_WriteToPin(&(TX.EN), 1);
	while(!READ(EXTI->PR, RX.EN.PinConfig.pin, 0x1));
	SET(EXTI->PR, RX.EN.PinConfig.pin, 1, 0x1);

	for(r = 0; r < X1; r++)
		for(c = 0; c < Y1; c++){
			for(offset = 0; offset < 4; offset++){
				TX_WriteData((matrix1[r][c] >> (offset*8)));

				//unsigned byte_elapsed = get_sys_tick();

				GPIO_WriteToPin(&(TX.EN), 0);
				GPIO_WriteToPin(&(TX.EN), 1);
				while(!READ(EXTI->PR, RX.EN.PinConfig.pin, 0x1));
				SET(EXTI->PR, RX.EN.PinConfig.pin, 1, 0x1);

				/*byte_elapsed = get_sys_tick() - byte_elapsed;
				total_comm_cycle += byte_elapsed;
				total_comm_byte++;*/
				//usec_delay(50);
			}
		}


	// TX matrix2
	// Reset state in Arduino
	TX_WriteData(0x22);
	GPIO_WriteToPin(&(TX.EN), 0);
	usec_delay(50);
	GPIO_WriteToPin(&(TX.EN), 1);
	while(!READ(EXTI->PR, RX.EN.PinConfig.pin, 0x1));
	SET(EXTI->PR, RX.EN.PinConfig.pin, 1, 0x1);

	TX_WriteData(Y1);
	GPIO_WriteToPin(&(TX.EN), 0);
	usec_delay(50);
	GPIO_WriteToPin(&(TX.EN), 1);
	while(!READ(EXTI->PR, RX.EN.PinConfig.pin, 0x1));
	SET(EXTI->PR, RX.EN.PinConfig.pin, 1, 0x1);

	TX_WriteData(Y2);
	GPIO_WriteToPin(&(TX.EN), 0);
	usec_delay(50);
	GPIO_WriteToPin(&(TX.EN), 1);
	while(!READ(EXTI->PR, RX.EN.PinConfig.pin, 0x1));
	SET(EXTI->PR, RX.EN.PinConfig.pin, 1, 0x1);

	//msec_delay(3);

	for(r = 0; r < Y1; r++)
		for(c = 0; c < Y2; c++){
			for(offset = 0; offset < 4; offset++){
				TX_WriteData((matrix2[r][c] >> (offset*8)));

				//unsigned byte_elapsed = get_sys_tick();

				GPIO_WriteToPin(&(TX.EN), 0);
				//usec_delay(50);
				GPIO_WriteToPin(&(TX.EN), 1);
				while(!READ(EXTI->PR, RX.EN.PinConfig.pin, 0x1));
				SET(EXTI->PR, RX.EN.PinConfig.pin, 1, 0x1);

				/*byte_elapsed = get_sys_tick() - byte_elapsed;
				total_comm_cycle += byte_elapsed;
				total_comm_byte++;*/
			}
		}

}

void request_matrix(){	// Wait for Arduino computation done signal
	TX_WriteData(0x25);
	//while(GPIO_ReadFromPin(RX.EN));
	GPIO_WriteToPin(&(TX.EN), 0);
	GPIO_WriteToPin(&(TX.EN), 1);
	//while(!GPIO_ReadFromPin(RX.EN));
	while(!READ(EXTI->PR, RX.EN.PinConfig.pin, 0x1));	// Wait for Arduino ACK
	EXTI->PR |= (1 << RX.EN.PinConfig.pin);
	return;
}

void receive_matrix(unsigned ** requested, short X, short Y){
	TX_WriteData(0);

	GPIO_IRQConfig(&(RX.EN), GPIO_EXTI_EVENT, GPIO_EXTI_EDGE_RISE);

	GPIO_WriteToPin(&(TX.EN), 0);

	usec_delay(10);

	for(short r = 0; r < X; r++)
		for(short c = 0; c < Y; c++){
			requested[r][c] = 0;
			for(short offset = 0; offset < 4; offset++){
				GPIO_WriteToPin(&(TX.EN), 0);
				while(!READ(EXTI->PR, RX.EN.PinConfig.pin, 0x1));	// Wait for Arduino data prep
				GPIO_WriteToPin(&(TX.EN), 1);

				requested[r][c] |= RX_ReadData() << (offset * 8);

				EXTI->PR |= (1 << RX.EN.PinConfig.pin);
				//usec_delay(5);
			}
		}

	GPIO_IRQConfig(&(RX.EN), GPIO_EXTI_EVENT, GPIO_EXTI_EDGE_FALL);
}

void matrix_multiply(unsigned ** A, unsigned ** B, unsigned ** result, short rowA, short colA, short colB){
	for(short r = 0; r < rowA; r++)
		for(short c = 0; c < colB; c++)
			for(short k = 0; k < colA; k++)
				result[r][c] += A[r][k] * B[k][c];
}

void Matrix_Chain_Multiplication(){
	LCD2004_Config_t LCD2004_conf = {true, true, true, true, 2, 0, 0};
	LCD2004_Init(&LCD2004_conf);
	LCD2004_WriteString(&LCD2004_conf, "Matrix Multiply", 15);
	LCD2004_SetCursor(&LCD2004_conf, 1, 0);
	LCD2004_WriteString(&LCD2004_conf, "Button to Start", 15);

	start = false;
	while(!start);
	//LCD2004_ClearScreen(&LCD2004_conf);
	//LCD2004_WriteString(&LCD2004_conf, "Transferring Matrix", 19);

	total_comm_cycle = 0;
	total_comm_byte = 0;

	Parallel_Communication_Init();

	/*
	 * A*B*C*D*E*F*G*H - same size (50x50)
	 * To save memory: allocate memory at every level and free after level is completed
	 * 1st level: A*B*C*D*E*F*G*H
	 * 2nd level: AB*CD*EF*GH
	 * 3rd level: ABCD*EFGH
	 */

	unsigned **A, **B, **C, **D, **AB, **CD, **ABCD, \
			**E, **F, **G, **H, **EF, **GH, **EFGH, \
			**I, **J, **K, **L, **IJ, **KL, **IJKL, \
			**M, **N, **O, **P, **MN, **OP, **MNOP, \
			**ABCDEFGH, **IJKLMNOP,**final;
	short rowA = 50, colA = 50, colB = 50, colC = 50, colD = 50, \
			colE = 50, colF = 50, colG = 50, colH = 50, \
			colI = 50, colJ = 50, colK = 50, colL = 50, \
			colM = 50, colN = 50, colO = 50, colP = 50;
	short r,c;
	unsigned TX_start, TX_stop, RX_start, RX_stop, totalTX = 0, \
			TX_start2, TX_stop2, RX_start2, RX_stop2, totalRX = 0, \
			elapsed, elapsed2, total_elapsed = 0, mem_elapsed, total_mem_elapsed = 0;

	// Initialize 1st level 1st half branch - ABCDEFGH
	mem_elapsed = get_sys_tick();
	A = (unsigned**) malloc(rowA * sizeof(unsigned*));
	for(r = 0; r < rowA; r++)
		A[r] = (unsigned*) calloc(colA, sizeof(unsigned));

	B = (unsigned**) malloc(colA * sizeof(unsigned*));
	for(r = 0; r < colA; r++)
		B[r] = (unsigned*) calloc(colB, sizeof(unsigned));

	C = (unsigned**) malloc(colB * sizeof(unsigned*));
	for(r = 0; r < colB; r++)
		C[r] = (unsigned*) calloc(colC, sizeof(unsigned));

	D = (unsigned**) malloc(colC * sizeof(unsigned*));
	for(r = 0; r < colC; r++)
		D[r] = (unsigned*) calloc(colD, sizeof(unsigned));

	E = (unsigned**) malloc(colD * sizeof(unsigned*));
	for(r = 0; r < colD; r++)
		E[r] = (unsigned*) calloc(colE, sizeof(unsigned));

	F = (unsigned**) malloc(colE * sizeof(unsigned*));
	for(r = 0; r < colE; r++)
		F[r] = (unsigned*) calloc(colF, sizeof(unsigned));

	G = (unsigned**) malloc(colF * sizeof(unsigned*));
	for(r = 0; r < colF; r++)
		G[r] = (unsigned*) calloc(colG, sizeof(unsigned));

	H = (unsigned**) malloc(colG * sizeof(unsigned*));
	for(r = 0; r < colG; r++)
		H[r] = (unsigned*) calloc(colH, sizeof(unsigned));

	AB = (unsigned**) malloc(rowA * sizeof(unsigned*));
	for(r = 0; r < rowA; r++)
		AB[r] = (unsigned*) calloc(colB, sizeof(unsigned));

	CD = (unsigned**) malloc(colB * sizeof(unsigned*));
	for(r = 0; r < colB; r++)
		CD[r] = (unsigned*) calloc(colD, sizeof(unsigned));

	EF = (unsigned**) malloc(colD * sizeof(unsigned*));
	for(r = 0; r < colD; r++)
		EF[r] = (unsigned*) calloc(colF, sizeof(unsigned));

	GH = (unsigned**) malloc(colF * sizeof(unsigned*));
	for(r = 0; r < colF; r++)
		GH[r] = (unsigned*) calloc(colH, sizeof(unsigned));

	for(r = 0; r < rowA; r++)
		for(c = 0; c < colA; c++)
			A[r][c] = c + r*colA;

	for(r = 0; r < colA; r++)
		for(c = 0; c < colB; c++)
			B[r][c] = c + r*colB;

	for(r = 0; r < colB; r++)
		for(c = 0; c < colC; c++)
			C[r][c] = c + r*colC;

	for(r = 0; r < colC; r++)
		for(c = 0; c < colD; c++)
			D[r][c] = c + r*colD;

	for(r = 0; r < colD; r++)
		for(c = 0; c < colE; c++)
			E[r][c] = c + r*colE;

	for(r = 0; r < colE; r++)
		for(c = 0; c < colF; c++)
			F[r][c] = c + r*colF;

	for(r = 0; r < colF; r++)
		for(c = 0; c < colG; c++)
			G[r][c] = c + r*colG;

	for(r = 0; r < colG; r++)
		for(c = 0; c < colH; c++)
			H[r][c] = c + r*colH;
	mem_elapsed = get_sys_tick() - mem_elapsed;
	total_mem_elapsed += mem_elapsed;

	/********************
	 * Level 1.1 KERNEL STARTS	*
	 ********************/
	// Level 1.1 kernel (A*B & E*F)
	TX_start = get_sys_tick();
	transfer_matrix_pair(E, F, colD, colE, colF);
	TX_stop = get_sys_tick();

	elapsed = get_sys_tick();
	matrix_multiply(A, B, AB, rowA, colA, colB);
	//matrix_multiply(E, F, EF, colD, colE, colF);
	request_matrix();
	elapsed = get_sys_tick() - elapsed;

	RX_start = get_sys_tick();
	receive_matrix(EF, colD, colF);
	RX_stop = get_sys_tick();

	// Level 1.2 (C*D & G*H)
	TX_start2 = get_sys_tick();
	transfer_matrix_pair(G, H, colF, colG, colH);
	TX_stop2 = get_sys_tick();

	elapsed2 = get_sys_tick();
	matrix_multiply(C, D, CD, colB, colC, colD);
	//matrix_multiply(G, H, GH, colF, colG, colH);
	request_matrix();
	elapsed2 = get_sys_tick() - elapsed2;

	RX_start2 = get_sys_tick();
	receive_matrix(GH, colF, colH);
	RX_stop2 = get_sys_tick();

	/********************
	 * KERNEL ENDs		*
	 ********************/

	total_elapsed = elapsed + elapsed2;// + elapsed3;
	totalRX = RX_stop - RX_start + RX_stop2 - RX_start2;// + RX_stop3 - RX_start3;
	totalTX = TX_stop - TX_start + TX_stop2 - TX_start2;// + TX_stop3 - TX_start3;

	// Free up memory space for next level of computation
	mem_elapsed = get_sys_tick();
	for(r = 0; r < rowA; r++)
		free(A[r]);
	for(r = 0; r < colA; r++)
		free(B[r]);
	for(r = 0; r < colB; r++)
		free(C[r]);
	for(r = 0; r < colC; r++)
		free(D[r]);
	for(r = 0; r < colD; r++)
		free(E[r]);
	for(r = 0; r < colE; r++)
		free(F[r]);
	for(r = 0; r < colF; r++)
		free(G[r]);
	for(r = 0; r < colG; r++)
		free(H[r]);

	free(A);
	free(B);
	free(C);
	free(D);
	free(E);
	free(F);
	free(G);
	free(H);

	// Initialize 2nd level 1st half branch
	ABCD = (unsigned**) malloc(rowA * sizeof(unsigned*));
	for(r = 0; r < rowA; r++)
		ABCD[r] = (unsigned*) calloc(colD, sizeof(unsigned));

	EFGH = (unsigned**) malloc(colD * sizeof(unsigned*));
	for(r = 0; r < colD; r++)
		EFGH[r] = (unsigned*) calloc(colH, sizeof(unsigned));
	mem_elapsed = get_sys_tick() - mem_elapsed;
	total_mem_elapsed += mem_elapsed;

	elapsed = 0; elapsed2 = 0; //elapsed3 = 0;
	RX_stop = 0; RX_start = 0; RX_stop2 = 0; RX_start2 = 0;// RX_stop3 = 0; RX_start3 = 0;
	TX_stop = 0; TX_start = 0; TX_stop2 = 0; TX_start2 = 0;// TX_stop3 = 0; TX_start3 = 0;

	/********************
	 * LEVEL 2.1 KERNEL STARTS	*
	 ********************/
	// Level 2 kernel (AB*CD & EF*GH)
	TX_start = get_sys_tick();
	transfer_matrix_pair(EF, GH, colD, colF, colH);
	TX_stop = get_sys_tick();

	elapsed = get_sys_tick();
	//matrix_multiply(EF, GH, EFGH, colD, colF, colH);
	matrix_multiply(AB, CD, ABCD, rowA, colB, colD);
	request_matrix();
	elapsed = get_sys_tick() - elapsed;

	RX_start = get_sys_tick();
	receive_matrix(EFGH, colD, colH);
	RX_stop = get_sys_tick();

	/********************
	 * KERNEL ENDs		*
	 ********************/

	total_elapsed = elapsed;
	totalRX = RX_stop - RX_start;// + RX_stop2 - RX_start2;
	totalTX = TX_stop - TX_start;// + TX_stop2 - TX_start2;

	// Free up memory space for 2nd half branch of computation
	mem_elapsed = get_sys_tick();
	for(short r = 0; r < rowA; r++)
		free(AB[r]);
	for(short r = 0; r < colB; r++)
		free(CD[r]);
	for(short r = 0; r < colD; r++)
		free(EF[r]);
	for(short r = 0; r < colF; r++)
		free(GH[r]);

	free(AB);
	free(CD);
	free(EF);
	free(GH);

	// Initialize 1st level 2nd half branch - IJKLMNOP
	I = (unsigned**) malloc(colH * sizeof(unsigned*));
	for(r = 0; r < colH; r++)
		I[r] = (unsigned*) calloc(colI, sizeof(unsigned));

	J = (unsigned**) malloc(colI * sizeof(unsigned*));
	for(r = 0; r < colI; r++)
		J[r] = (unsigned*) calloc(colJ, sizeof(unsigned));

	K = (unsigned**) malloc(colJ * sizeof(unsigned*));
	for(r = 0; r < colJ; r++)
		K[r] = (unsigned*) calloc(colK, sizeof(unsigned));

	L = (unsigned**) malloc(colK * sizeof(unsigned*));
	for(r = 0; r < colK; r++)
		L[r] = (unsigned*) calloc(colL, sizeof(unsigned));

	M = (unsigned**) malloc(colL * sizeof(unsigned*));
	for(r = 0; r < colL; r++)
		M[r] = (unsigned*) calloc(colM, sizeof(unsigned));

	N = (unsigned**) malloc(colM * sizeof(unsigned*));
	for(r = 0; r < colM; r++)
		N[r] = (unsigned*) calloc(colN, sizeof(unsigned));

	O = (unsigned**) malloc(colN * sizeof(unsigned*));
	for(r = 0; r < colN; r++)
		O[r] = (unsigned*) calloc(colO, sizeof(unsigned));

	P = (unsigned**) malloc(colO * sizeof(unsigned*));
	for(r = 0; r < colO; r++)
		P[r] = (unsigned*) calloc(colP, sizeof(unsigned));

	IJ = (unsigned**) malloc(colH * sizeof(unsigned*));
	for(r = 0; r < colH; r++)
		IJ[r] = (unsigned*) calloc(colJ, sizeof(unsigned));

	KL = (unsigned**) malloc(colJ * sizeof(unsigned*));
	for(r = 0; r < colJ; r++)
		KL[r] = (unsigned*) calloc(colL, sizeof(unsigned));

	for(r = 0; r < colH; r++)
		for(c = 0; c < colI; c++)
			I[r][c] = c + r*colI;

	for(r = 0; r < colI; r++)
		for(c = 0; c < colJ; c++)
			J[r][c] = c + r*colJ;

	for(r = 0; r < colJ; r++)
		for(c = 0; c < colK; c++)
			K[r][c] = c + r*colK;

	for(r = 0; r < colK; r++)
		for(c = 0; c < colL; c++)
			L[r][c] = c + r*colL;

	for(r = 0; r < colL; r++)
		for(c = 0; c < colM; c++)
			M[r][c] = c + r*colM;

	for(r = 0; r < colM; r++)
		for(c = 0; c < colN; c++)
			N[r][c] = c + r*colN;

	for(r = 0; r < colN; r++)
		for(c = 0; c < colO; c++)
			O[r][c] = c + r*colO;

	for(r = 0; r < colO; r++)
		for(c = 0; c < colP; c++)
			P[r][c] = c + r*colP;

	mem_elapsed = get_sys_tick() - mem_elapsed;
	total_mem_elapsed += mem_elapsed;

	elapsed = 0; elapsed2 = 0; //elapsed3 = 0;
	RX_stop = 0; RX_start = 0; RX_stop2 = 0; RX_start2 = 0;// RX_stop3 = 0; RX_start3 = 0;
	TX_stop = 0; TX_start = 0; TX_stop2 = 0; TX_start2 = 0;// TX_stop3 = 0; TX_start3 = 0;

	/********************
	 * LEVEL 1.2 KERNEL STARTS	*
	 ********************/
	// Level 1.1 kernel (I*J & K*L)
	TX_start = get_sys_tick();
	transfer_matrix_pair(I, J, colH, colI, colJ);
	TX_stop = get_sys_tick();

	elapsed = get_sys_tick();
	matrix_multiply(K, L, KL, colJ, colK, colL);
	//matrix_multiply(I, J, IJ, colH, colI, colJ);
	request_matrix();
	elapsed = get_sys_tick() - elapsed;

	RX_start = get_sys_tick();
	receive_matrix(IJ, colH, colJ);
	RX_stop = get_sys_tick();

	// Due to memory limit, kernel has to be broken midways
	//	to free 1st (I/J/K/L) memory and allocate 2nd half (MNOP)
	mem_elapsed = get_sys_tick();
	for(r = 0; r < colH; r++)
		free(I[r]);
	for(r = 0; r < colI; r++)
		free(J[r]);
	for(r = 0; r < colJ; r++)
		free(K[r]);
	for(r = 0; r < colK; r++)
		free(L[r]);
	free(I);
	free(J);
	free(K);
	free(L);

	MN = (unsigned**) malloc(colL * sizeof(unsigned*));
	for(r = 0; r < colL; r++)
		MN[r] = (unsigned*) calloc(colN, sizeof(unsigned));

	OP = (unsigned**) malloc(colN * sizeof(unsigned*));
	for(r = 0; r < colP; r++)
		OP[r] = (unsigned*) calloc(colP, sizeof(unsigned));

	mem_elapsed = get_sys_tick() - mem_elapsed;
	total_mem_elapsed += mem_elapsed;


	// Level 1.2 (M*N & O*P)
	TX_start2 = get_sys_tick();
	transfer_matrix_pair(O, P, colN, colO, colP);
	TX_stop2 = get_sys_tick();

	elapsed2 = get_sys_tick();
	matrix_multiply(M, N, MN, colL, colM, colN);
	//matrix_multiply(O, P, OP, colN, colO, colP);
	request_matrix();
	elapsed2 = get_sys_tick() - elapsed2;

	RX_start2 = get_sys_tick();
	receive_matrix(OP, colN, colP);
	RX_stop2 = get_sys_tick();

	/********************
	 * KERNEL ENDs		*
	 ********************/

	total_elapsed = elapsed + elapsed2;
	totalRX = RX_stop - RX_start + RX_stop2 - RX_start2;
	totalTX = TX_stop - TX_start + TX_stop2 - TX_start2;

	// Free up memory space for next level of computation
	mem_elapsed = get_sys_tick();
	for(r = 0; r < colL; r++)
		free(M[r]);
	for(r = 0; r < colM; r++)
		free(N[r]);
	for(r = 0; r < colN; r++)
		free(O[r]);
	for(r = 0; r < colO; r++)
		free(P[r]);

	free(M);
	free(N);
	free(O);
	free(P);

	// Initialize 2nd level 2nd half branch
	IJKL = (unsigned**) malloc(colH * sizeof(unsigned*));
	for(r = 0; r < colH; r++)
		IJKL[r] = (unsigned*) calloc(colL, sizeof(unsigned));

	MNOP = (unsigned**) malloc(colL * sizeof(unsigned*));
	for(r = 0; r < colL; r++)
		MNOP[r] = (unsigned*) calloc(colP, sizeof(unsigned));

	mem_elapsed = get_sys_tick() - mem_elapsed;
	total_mem_elapsed += mem_elapsed;

	elapsed = 0; elapsed2 = 0;
	RX_stop = 0; RX_start = 0; RX_stop2 = 0; RX_start2 = 0;
	TX_stop = 0; TX_start = 0; TX_stop2 = 0; TX_start2 = 0;

	/********************
	 * LEVEL 2.2 KERNEL STARTS	*
	 ********************/
	// Level 2 kernel (IJ*KL & MN*OP)
	TX_start = get_sys_tick();
	transfer_matrix_pair(IJ, KL, colH, colJ, colL);
	TX_stop = get_sys_tick();

	elapsed = get_sys_tick();
	matrix_multiply(MN, OP, MNOP, colL, colN, colP);
	//matrix_multiply(IJ, KL, IJKL, colH, colJ, colL);
	request_matrix();
	elapsed = get_sys_tick() - elapsed;

	RX_start = get_sys_tick();
	receive_matrix(IJKL, colH, colL);
	RX_stop = get_sys_tick();

	/********************
	 * KERNEL ENDs		*
	 ********************/

	total_elapsed = elapsed;
	totalRX = RX_stop - RX_start;// + RX_stop2 - RX_start2;
	totalTX = TX_stop - TX_start;// + TX_stop2 - TX_start2;

	// Free up memory space for 3rd & 4th levels of computation
	mem_elapsed = get_sys_tick();
	for(short r = 0; r < colH; r++)
		free(IJ[r]);
	for(short r = 0; r < colJ; r++)
		free(KL[r]);
	for(short r = 0; r < colK; r++)
		free(MN[r]);
	for(short r = 0; r < colN; r++)
		free(OP[r]);

	free(IJ);
	free(KL);
	free(MN);
	free(OP);

	// Initialize 3rd & 4th levels
	ABCDEFGH = (unsigned**) malloc(rowA * sizeof(unsigned*));
	for(r = 0; r < rowA; r++)
		ABCDEFGH[r] = (unsigned*) calloc(colH, sizeof(unsigned));

	IJKLMNOP = (unsigned**) malloc(colH * sizeof(unsigned*));
	for(r = 0; r < colH; r++)
		IJKLMNOP[r] = (unsigned*) calloc(colP, sizeof(unsigned));

	final = (unsigned**) malloc(rowA * sizeof(unsigned*));
	for(r = 0; r < rowA; r++)
		final[r] = (unsigned*) calloc(colP, sizeof(unsigned));

	mem_elapsed = get_sys_tick() - mem_elapsed;
	total_mem_elapsed += mem_elapsed;

	/********************
	 * LEVEL 3 & 4 KERNEL STARTS	*
	 ********************/
	// Level 3 kernel (ABCD * EFGH & IJKL * MNOP)
	TX_start = get_sys_tick();
	transfer_matrix_pair(ABCD, EFGH, rowA, colD, colH);
	TX_stop = get_sys_tick();

	elapsed = get_sys_tick();
	matrix_multiply(IJKL, MNOP, IJKLMNOP, colH, colL, colP);
	//matrix_multiply(ABCD, EFGH, ABCDEFGH, rowA, colD, colH);
	request_matrix();
	elapsed = get_sys_tick() - elapsed;

	RX_start = get_sys_tick();
	receive_matrix(ABCDEFGH, rowA, colH);
	RX_stop = get_sys_tick();

	// Level 4 kernel
	TX_start2 = get_sys_tick();
	transfer_matrix_pair(ABCDEFGH, IJKLMNOP, rowA, colH, colP);
	TX_stop2 = get_sys_tick();

	elapsed2 = get_sys_tick();
	//matrix_multiply(ABCDEFGH, IJKLMNOP, final, colH, colL, colP);
	request_matrix();
	elapsed2 = get_sys_tick() - elapsed2;

	RX_start2 = get_sys_tick();
	receive_matrix(final, rowA, colP);
	RX_stop2 = get_sys_tick();

	/********************
	 * KERNEL ENDs		*
	 ********************/

	total_elapsed += elapsed + elapsed2;
	totalRX += RX_stop - RX_start + RX_stop2 - RX_start2;
	totalTX += TX_stop - TX_start + TX_stop2 - TX_start2;

	mem_elapsed = get_sys_tick();
	for(short r = 0; r < rowA; r++)
		free(ABCD[r]);
	for(short r = 0; r < colD; r++)
		free(EFGH[r]);
	for(short r = 0; r < colH; r++)
		free(IJKL[r]);
	for(short r = 0; r < colL; r++)
		free(MNOP[r]);
	for(short r = 0; r < rowA; r++)
		free(ABCDEFGH[r]);
	for(short r = 0; r < colH; r++)
		free(IJKLMNOP[r]);
	for(short r = 0; r < rowA; r++)
		free(final[r]);

	free(ABCD);
	free(EFGH);
	free(IJKL);
	free(MNOP);
	free(ABCDEFGH);
	free(IJKLMNOP);
	free(final);
	mem_elapsed = get_sys_tick() - mem_elapsed;
	total_mem_elapsed += mem_elapsed;

	unsigned usec = (total_elapsed)/(apb1_timer/1000000);
	char buffer[10];
	itoa(usec, buffer, 10);

	LCD2004_ClearScreen(&LCD2004_conf);
	LCD2004_WriteString(&LCD2004_conf, "Arduino Comp Done", 17);
	LCD2004_SetCursor(&LCD2004_conf, 1, 0);
	LCD2004_WriteString(&LCD2004_conf, "COMP usec: ", 11);
	LCD2004_WriteString(&LCD2004_conf, buffer, 10);

	unsigned RX_msec = (totalRX)/(apb1_timer/1000);
	/*if(RX_overlap == 0)
		RX_msec = (totalRX)/(apb1_timer/1000);
	else
		RX_msec = ((RX_overlap-1)*(0xFFFFFFFF/(apb1_timer/1000))) + (RX_stop/(apb1_timer/1000)) + ((0xFFFFFFFF - RX_start)/(apb1_timer/1000));*/

	unsigned TX_msec = (totalTX)/(apb1_timer/1000);
	/*if(TX_overlap == 0)
		TX_msec = (totalTX)/(apb1_timer/1000);
	else
		TX_msec = ((TX_overlap-1)*(0xFFFFFFFF/(apb1_timer/1000))) + (TX_stop/(apb1_timer/1000)) + ((0xFFFFFFFF - TX_start)/(apb1_timer/1000));*/

	unsigned comm_msec = RX_msec + TX_msec;
	unsigned mem_usec = total_mem_elapsed/(apb1_timer/1000000);

	memset(buffer, 0, 10);
	itoa(comm_msec, buffer, 10);
	LCD2004_SetCursor(&LCD2004_conf, 2, 0);
	LCD2004_WriteString(&LCD2004_conf, "COMM msec: ", 11);
	LCD2004_WriteString(&LCD2004_conf, buffer, 9);

	memset(buffer, 0, 10);
	itoa(mem_usec, buffer, 10);
	LCD2004_SetCursor(&LCD2004_conf, 3, 0);
	LCD2004_WriteString(&LCD2004_conf, "MEM usec: ", 10);
	LCD2004_WriteString(&LCD2004_conf, buffer, 10);
}
