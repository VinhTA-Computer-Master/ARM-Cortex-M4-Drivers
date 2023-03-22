/*
 * sysclk_driver.c
 *
 *  Created on: Dec 29, 2022
 *      Author: vinht
 */


#include "../Inc/sysclk_driver.h"

void SYSCLK_Init(SYSCLK_config* config){
		SET(RCC->CR, 0, 1, 0x1);  // Turn on HSI

	if(config->output == SYSCLK_HSI){  // HSI clock config
		int i=0;
		while(!READ(RCC->CR, 1, 0x1)){  // Wait till HSI is ready
			i++;
			if(i == 2000000) break;  // Wait timeout
		}
		if(i == 2000000) return;

		// Set bus prescalers
		CLEAR(RCC->CFGR, 4, 0xF);
		SET(RCC->CFGR, 4, config->AHB_conf, 0xF);		// Set AHB freq scaler
		CLEAR(RCC->CFGR, 10, 0x7);
		SET(RCC->CFGR, 10, config->APB1_conf, 0x7);		// Set APB1 scaler
		CLEAR(RCC->CFGR, 13, 0x7);
		SET(RCC->CFGR, 13, config->APB2_conf, 0x7);		// Set APB2 scaler

		CLEAR(RCC->CFGR, 0, 0x3);

	} else if(config->output == SYSCLK_HSE){  // HSE clock config
		SET(RCC->CR, 16, 1, 0x1);

		int i=0;
		while(!READ(RCC->CR, 16, 0x1)){  // Wait till HSE is ready
			i++;
			if(i == 2000000) break;  // Wait timeout
		}
		if(i == 2000000) return;

		// Set bus prescalers
		CLEAR(RCC->CFGR, 4, 0xF);
		SET(RCC->CFGR, 4, config->AHB_conf, 0xF);		// Set AHB freq scaler
		CLEAR(RCC->CFGR, 10, 0x7);
		SET(RCC->CFGR, 10, config->APB1_conf, 0x7);		// Set APB1 scaler
		CLEAR(RCC->CFGR, 13, 0x7);
		SET(RCC->CFGR, 13, config->APB2_conf, 0x7);		// Set APB2 scaler

		CLEAR(RCC->CFGR, 0, 0x3);
		SET(RCC->CFGR, 0, 1, 0x3);

	} else {  // PLL clock config

		unsigned pll_clk = PLL_CLK_Config(config->PLL_src, config->PLL_scaler, config->VCO_Mult_factor, config->VCO_Div_factor, config->PLL_Q);

		if(!pll_clk){
			CLEAR(RCC->CFGR, 0, 0x3);
			return;
		}

		double AHB_freq;
		if(config->AHB_conf == AHB_Div_2)
			AHB_freq = pll_clk/2;
		else if(config->AHB_conf == AHB_Div_4)
			AHB_freq = pll_clk/4;
		else if(config->AHB_conf == AHB_Div_8)
			AHB_freq = pll_clk/8;
		else if(config->AHB_conf == AHB_Div_16)
			AHB_freq = pll_clk/16;
		else if(config->AHB_conf == AHB_Div_64)
			AHB_freq = pll_clk/64;
		else if(config->AHB_conf == AHB_Div_128)
			AHB_freq = pll_clk/128;
		else if(config->AHB_conf == AHB_Div_256)
			AHB_freq = pll_clk/256;
		else if(config->AHB_conf == AHB_Div_512)
			AHB_freq = pll_clk/512;
		else
			AHB_freq = pll_clk;

		double APB1_freq;
		if(config->APB1_conf == APB_Div_2)
			APB1_freq = pll_clk/2;
		else if(config->APB1_conf == APB_Div_4)
			APB1_freq = pll_clk/4;
		else if(config->APB1_conf == APB_Div_8)
			APB1_freq = pll_clk/8;
		else if(config->APB1_conf == APB_Div_16)
			APB1_freq = pll_clk/16;
		else
			APB1_freq = pll_clk;

		double APB2_freq;
		if(config->APB2_conf == APB_Div_2)
			APB2_freq = pll_clk/2;
		else if(config->APB2_conf == APB_Div_4)
			APB2_freq = pll_clk/4;
		else if(config->APB2_conf == APB_Div_8)
			APB2_freq = pll_clk/8;
		else if(config->APB2_conf == APB_Div_16)
			APB2_freq = pll_clk/16;
		else
			APB2_freq = pll_clk;

		if(AHB_freq > AHB_Max_Freq)
			return;
		if(APB1_freq > APB1_Max_Freq)
			return;
		if(APB2_freq > APB2_Max_Freq)
			return;


		// Config flash interface
		uint8_t prefetch_latency = 0;
		if(pll_clk > 30000000 && pll_clk <= 60000000)
			prefetch_latency = 1;
		else if(pll_clk <= 90000000)
			prefetch_latency = 2;
		else if(pll_clk <= 120000000)
			prefetch_latency = 3;
		else if(pll_clk <= 150000000)
			prefetch_latency = 4;
		else if(pll_clk <= 168000000)
			prefetch_latency = 5;

		if(prefetch_latency > 0)
			FLASH->ACR = (0x7 << 8) | prefetch_latency;

		// Decrease PCLK to lower before switching HCLK
		SET(RCC->CFGR, 10, APB_Div_16, 0x7);
		SET(RCC->CFGR, 13, APB_Div_16, 0x7);
		RCC->CFGR = (RCC->CFGR & ~(0xF << 4)) | (config->AHB_conf << 4);

		// Set SYSCLK to PLL
		while(!READ(RCC->CR, 25, 0x1));
		RCC->CFGR = (RCC->CFGR & (~(0x3))) | (0x2);
		while(READ(RCC->CFGR, 2, 0x3) != 2);

		// Set bus prescalers
		RCC->CFGR = (RCC->CFGR & (~(0x7 << 10))) | (config->APB1_conf << 10);
		RCC->CFGR = (RCC->CFGR & (~(0x7 << 13))) | (config->APB2_conf << 13);

		sysclk_frequency = pll_clk;
		apb1_frequency = APB1_freq;
		apb2_frequency = APB2_freq;

		if(config->APB1_conf != APB_Div_1)
			apb1_timer = apb1_frequency*2;
		if(config->APB2_conf != APB_Div_1)
			apb2_timer = apb2_frequency*2;
	}

	// Setup system tick using TIM5 (32b CNT)
	// To be used by stm32f4xx library for time keeping
	TIM5_EN;
	CLEAR(TIM5->CR1, 7, 0x1);  // Disable ARPE buffer
	CLEAR(TIM5->CR1, 5, 0x3);  // Edge-aligned mode
	CLEAR(TIM5->CR1, 4, 0x1);  // Up-counting direction
	SET(TIM5->CR1, 2, 1, 0x1);	// Set URS bit to enable interrupt to only overflow
	CLEAR(TIM5->CR1, 1, 0x1);	// UDIS bit clear --> interrupt enabled
	CLEAR(TIM5->SMCR, 0, 0x7);	// Slave mode disabled
	SET(TIM5->DIER, 0, 1, 0x1);	// Set UIE bit to enable update interrupt

	// To keep systick, PSC=0 (increment every cycle) and ARR = full
	TIM5->PSC = 0;
	TIM5->ARR = 0xFFFFFFFF;

	// In 16MHz bus clock
	// Interrupt every 4.294B cycles ~ 268.435456sec
	// In 42MHz bus clock (max) -> 84MHz timer clock
	// Interrupt every 4.294B cycles ~ 51.13056sec
	SET(NVIC_ISER->ISER1, (50-32), 1, 0x1);

	SET(TIM5->CR1, 0, 1, 0x1);

	return;
}


unsigned PLL_CLK_Config(enum PLL_SRC src, enum PLL_Out_Scaler P, int N, int M, int Q){
	if(N < 2 || N > 432)
		return 0;
	if(M < 2)
		return 0;

	double VCO_in_freq = 0;
	if(src == PLL_SRC_HSI) VCO_in_freq = HSI_Freq;  // 16MHz from HSI
	else if(src == PLL_SRC_HSE) VCO_in_freq = HSE_Freq;
	VCO_in_freq /= M;

	// Check VCO input freq first then check output freq
	if(VCO_in_freq < VCO_In_Low || VCO_in_freq > VCO_In_High)  // VCO input freq should range 1MHz->2MHz
		return 0;

	double VCO_out_freq = VCO_in_freq * N;
	if(VCO_out_freq < VCO_Out_Low || VCO_out_freq > VCO_Out_High)
		return 0;

	double PLL_out_freq = PLL_Max_Freq + 1;
	if(P == PLL_Out_Div_2)
		PLL_out_freq = VCO_out_freq/2;
	else if(P == PLL_Out_Div_4)
		PLL_out_freq = VCO_out_freq/4;
	else if(P == PLL_Out_Div_6)
		PLL_out_freq = VCO_out_freq/6;
	else if(P == PLL_Out_Div_8)
		PLL_out_freq = VCO_out_freq/8;

	if(PLL_out_freq > PLL_Max_Freq)
		return 0;

	if(PLL_out_freq/Q > PLL_Q_Max_Freq)
		return 0;

	// Turn on power output regulator
	/*if(PLL_out_freq > 16000000){
		PWR_EN;
		(void)(READ(RCC->APB1ENR, 14, 0x1));
		SET(PWR->CR, 14, 1, 0x1);
	}*/

	// Turn on HSE
	if(src == PLL_SRC_HSE){
		SET(RCC->CR, 16, 1, 0x1);
		while(!READ(RCC->CR, 16, 0x1));
	}

	// Config PLL
	CLEAR(RCC->CR, 24, 0x1);
	while(READ(RCC->CR, 25, 0x1));
	RCC->PLLCFGR = M | (N << 6) | (P << 16) | (src << 22) | (Q << 24);
	SET(RCC->CR, 24, 1, 0x1);
	while(!READ(RCC->CR, 25, 0x1));

	return PLL_out_freq;
}


void clock_boost(unsigned out_freq){		// out_freq in MHz
	if(out_freq < 50 || out_freq > 168)
		return;	// Use HSI clock

	unsigned PLL_Q;		// Reminder: PLLQ comes after PPLN
	if(out_freq > 96)
		PLL_Q = out_freq/24 + ((out_freq % 24 > 0)? 1:0);	// Cap clock at 48MHz
	else //if(out_freq > 48)
		PLL_Q = 4;		// Cap clock at sysclk/2

	sysclk_conf.output = SYSCLK_PLL;
	sysclk_conf.PLL_src = PLL_SRC_HSE;
	sysclk_conf.PLL_scaler = PLL_Out_Div_2;
	sysclk_conf.VCO_Mult_factor = (out_freq*2);		// PLLN
	sysclk_conf.VCO_Div_factor = 8;
	sysclk_conf.PLL_Q = PLL_Q;
	sysclk_conf.AHB_conf = AHB_Div_1;
	sysclk_conf.APB1_conf = APB_Div_4;
	sysclk_conf.APB2_conf = APB_Div_2;
	SYSCLK_Init(&sysclk_conf);
}


void MCO1_CLK_Config(enum MCO1_Config source, enum MCO_Scaler scale, bool output){
	if(source == MCO1_SRC_HSI){
		// Check if HSI is on and ready
		if(READ(RCC->CR, 0, 0x3) != 3)
			return;
		CLEAR(RCC->CFGR, 21, 0x3);
	} else if(source == MCO1_SRC_HSE){
		if(READ(RCC->CR, 16, 0x3) != 3){
			SET(RCC->CR, 16, 1, 0x1);
			while(!READ(RCC->CR, 17, 0x1));
		}
		CLEAR(RCC->CFGR, 21, 0x3);
		SET(RCC->CFGR, 21, 0x2, 0x3);
	} else if(source == MCO1_SRC_LSE){  // Warning: No check if clock is active!
		CLEAR(RCC->CFGR, 21, 0x3);
		SET(RCC->CFGR, 21, 0x1, 0x3);
	} else {  // source == PLL
		if(READ(RCC->CR, 24, 0x3) != 0x3)
			return;
		CLEAR(RCC->CFGR, 21, 0x3);
		SET(RCC->CFGR, 21, 4, 0x3);
	}

	CLEAR(RCC->CFGR, 24, 0x7);
	SET(RCC->CFGR, 24, scale, 0x7);

	if(output){
		GPIO_Handle_t PA8;
		GPIO_Init(&PA8, 'A', 8);
		GPIO_SetModer(&PA8, MODER_ALT_FUNC);
	}
}


void MCO2_CLK_Config(enum MCO2_Config source, enum MCO_Scaler scale, bool output){
	if(source == MCO2_SRC_SYSCLK){
		// Source from SYSCLK --> no active check needed
		CLEAR(RCC->CFGR, 30, 0x3);
	} else if(source == MCO2_SRC_PLLI2S){
		if(READ(RCC->CR, 26, 0x3) != 3)
			return;
		CLEAR(RCC->CFGR, 30, 0x3);
		SET(RCC->CFGR, 30, 1, 0x3);
	} else if(source == MCO2_SRC_HSE){  // Warning: No check if clock is active!
		CLEAR(RCC->CFGR, 30, 0x3);
		SET(RCC->CFGR, 30, 2, 0x3);
	} else {  // source == PLL
		if(READ(RCC->CR, 24, 0x3) != 0x3)
			return;
		CLEAR(RCC->CFGR, 30, 0x3);
		SET(RCC->CFGR, 30, 4, 0x3);
	}

	CLEAR(RCC->CFGR, 27, 0x7);
	SET(RCC->CFGR, 27, scale, 0x7);

	if(output){
		GPIO_Handle_t PC9;
		GPIO_Init(&PC9, 'C', 9);
		GPIO_SetModer(&PC9, MODER_ALT_FUNC);
	}
}
