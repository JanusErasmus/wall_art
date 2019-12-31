/*

  WS2812B CPU and memory efficient library

  Date: 28.9.2016

  Author: Martin Hubacek
  	  	  http://www.martinhubacek.cz
  	  	  @hubmartin

  Licence: MIT License

 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "stm32f4xx_hal.h"
#include "ws2812.h"


static TIM_HandleTypeDef  TIM1_handle;
static TIM_OC_InitTypeDef tim2OC1;
static TIM_OC_InitTypeDef tim2OC2;
static uint32_t tim_period;
static uint32_t timer_reset_pulse_period;

static DMA_HandleTypeDef dmaUpdate = {0};
static DMA_HandleTypeDef dmaCC1 = {0};
static DMA_HandleTypeDef dmaCC2 = {0};
#define BUFFER_SIZE		 sizeof(ws2812bDmaBitBuffer)


// GPIO enable command
#define WS2812B_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
// LED output port
#define WS2812B_PORT GPIOC
// LED output pins
#define WS2812B_PINS (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)

// Define source arrays for my DMAs
static const uint32_t WS2812_IO_Low[] = {WS2812B_PINS << 16};
static const uint32_t WS2812_IO_High[] =  { WS2812B_PINS };

// This value sets number of periods to generate 50uS Treset signal
#define WS2812_RESET_PERIOD 50


static uint32_t _transferComplete = 1;
static uint8_t ws2812bDmaBitBuffer[24 * 32];

// Gamma correction table
const uint8_t gammaTable[] = {
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
		1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
		2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
		5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
		10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
		17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
		25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
		37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
		51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
		69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
		90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
		115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
		144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
		177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
		215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};

static void ws2812b_gpio_init(void)
{
	WS2812B_GPIO_CLK_ENABLE();
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Pin       = WS2812B_PINS;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(WS2812B_PORT, &GPIO_InitStruct);
}

static void TIM1_init(void)
{
	// TIM2 Periph clock enable
	__HAL_RCC_TIM1_CLK_ENABLE();

	// This computation of pulse length should work ok,
	// at some slower core speeds it needs some tuning.
	tim_period =  SystemCoreClock / 800000; // 0,125us period (10 times lower the 1,25us period to have fixed math below)
	timer_reset_pulse_period = (SystemCoreClock / (320 * 65)); // 60us just to be sure

	uint32_t cc1 = (10 * tim_period) / 30;
	uint32_t cc2 = (10 * tim_period) / 15;

	TIM1_handle.Instance = TIM1;

	TIM1_handle.Init.Period            = tim_period;
	TIM1_handle.Init.RepetitionCounter = 0;
	TIM1_handle.Init.Prescaler         = 0;
	TIM1_handle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
	TIM1_handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	HAL_TIM_PWM_Init(&TIM1_handle);

	HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

	tim2OC1.OCMode       = TIM_OCMODE_PWM1;
	tim2OC1.OCPolarity   = TIM_OCPOLARITY_HIGH;
	tim2OC1.Pulse        = cc1;
	tim2OC1.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	tim2OC1.OCFastMode   = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&TIM1_handle, &tim2OC1, TIM_CHANNEL_1);

	tim2OC2.OCMode       = TIM_OCMODE_PWM1;
	tim2OC2.OCPolarity   = TIM_OCPOLARITY_HIGH;
	tim2OC2.Pulse        = cc2;
	tim2OC2.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	tim2OC2.OCFastMode   = TIM_OCFAST_DISABLE;
	tim2OC2.OCIdleState  = TIM_OCIDLESTATE_RESET;
	tim2OC2.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	HAL_TIM_PWM_ConfigChannel(&TIM1_handle, &tim2OC2, TIM_CHANNEL_2);


	HAL_TIM_Base_Start(&TIM1_handle);
	HAL_TIM_PWM_Start(&TIM1_handle, TIM_CHANNEL_1);

	__HAL_TIM_DISABLE(&TIM1_handle);

}

void DMA_TransferCompleteHandler(DMA_HandleTypeDef *DmaHandle)
{
	// Stop timer
	TIM1->CR1 &= ~TIM_CR1_CEN;

	// Disable DMA
	__HAL_DMA_DISABLE(&dmaUpdate);
	__HAL_DMA_DISABLE(&dmaCC1);
	__HAL_DMA_DISABLE(&dmaCC2);

	// Disable the DMA requests
	__HAL_TIM_DISABLE_DMA(&TIM1_handle, TIM_DMA_UPDATE);
	__HAL_TIM_DISABLE_DMA(&TIM1_handle, TIM_DMA_CC1);
	__HAL_TIM_DISABLE_DMA(&TIM1_handle, TIM_DMA_CC2);

	// Set 50us period for Treset pulse
	//TIM2->PSC = 1000; // For this long period we need prescaler 1000
	TIM1->ARR = timer_reset_pulse_period;
	// Reset the timer
	TIM1->CNT = 0;

	// Generate an update event to reload the prescaler value immediately
	TIM1->EGR = TIM_EGR_UG;
	__HAL_TIM_CLEAR_FLAG(&TIM1_handle, TIM_FLAG_UPDATE);

	// Enable TIM2 Update interrupt for 50us Treset signal
	__HAL_TIM_ENABLE_IT(&TIM1_handle, TIM_IT_UPDATE);
	// Enable timer
	TIM1->CR1 |= TIM_CR1_CEN;

	// Manually set outputs to low to generate 50us reset impulse
	WS2812B_PORT->BSRR = WS2812_IO_Low[0];
}

void DMA_TransferError(DMA_HandleTypeDef *DmaHandle)
{
	volatile int i = 0;
	i++;

	printf("E %lu\n", DmaHandle->ErrorCode);
}



static void DMA2_init(void)
{
	// TIM2 Update event
	__HAL_RCC_DMA2_CLK_ENABLE();

	dmaUpdate.Instance = DMA2_Stream5;
	dmaUpdate.Init.Direction = DMA_MEMORY_TO_PERIPH;
	dmaUpdate.Init.PeriphInc = DMA_PINC_DISABLE;
	dmaUpdate.Init.MemInc = DMA_MINC_DISABLE;
	dmaUpdate.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	dmaUpdate.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	dmaUpdate.Init.Mode = DMA_CIRCULAR;
	dmaUpdate.Init.Priority = DMA_PRIORITY_VERY_HIGH;
	dmaUpdate.Init.Channel = DMA_CHANNEL_6;

	dmaUpdate.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	dmaUpdate.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	dmaUpdate.Init.MemBurst = DMA_MBURST_SINGLE;
	dmaUpdate.Init.PeriphBurst = DMA_PBURST_SINGLE;

	HAL_DMA_Init(&dmaUpdate);
	HAL_DMA_Start(&dmaUpdate, (uint32_t)WS2812_IO_High, (uint32_t)(&WS2812B_PORT->BSRR), BUFFER_SIZE);

	// TIM2 CC1 event
	dmaCC1.Instance = DMA2_Stream1;
	dmaCC1.Init.Direction = DMA_MEMORY_TO_PERIPH;
	dmaCC1.Init.PeriphInc = DMA_PINC_DISABLE;
	dmaCC1.Init.MemInc = DMA_MINC_ENABLE;
	dmaCC1.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	dmaCC1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	dmaCC1.Init.Mode = DMA_CIRCULAR;
	dmaCC1.Init.Priority = DMA_PRIORITY_VERY_HIGH;
	dmaCC1.Init.Channel = DMA_CHANNEL_6;

	dmaCC1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	dmaCC1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	dmaCC1.Init.MemBurst = DMA_MBURST_SINGLE;
	dmaCC1.Init.PeriphBurst = DMA_PBURST_SINGLE;


	HAL_DMA_Init(&dmaCC1);
	HAL_DMA_Start(&dmaCC1, (uint32_t)ws2812bDmaBitBuffer, (uint32_t)(&WS2812B_PORT->BSRR) + 2, BUFFER_SIZE);

	dmaUpdate.XferHalfCpltCallback = DMA_TransferCompleteHandler;

	// TIM2 CC2 event
	dmaCC2.Instance = DMA2_Stream2;
	dmaCC2.Init.Direction = DMA_MEMORY_TO_PERIPH;
	dmaCC2.Init.PeriphInc = DMA_PINC_DISABLE;
	dmaCC2.Init.MemInc = DMA_MINC_DISABLE;
	dmaCC2.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	dmaCC2.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	dmaCC2.Init.Mode = DMA_CIRCULAR;
	dmaCC2.Init.Priority = DMA_PRIORITY_VERY_HIGH;
	dmaCC2.Init.Channel = DMA_CHANNEL_6;

	dmaCC2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	dmaCC2.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	dmaCC2.Init.MemBurst = DMA_MBURST_SINGLE;
	dmaCC2.Init.PeriphBurst = DMA_PBURST_SINGLE;


	dmaCC2.XferCpltCallback  = DMA_TransferCompleteHandler;
	dmaCC2.XferErrorCallback = DMA_TransferError;

	HAL_DMA_Init(&dmaCC2);
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
	HAL_DMA_Start_IT(&dmaCC2, (uint32_t)WS2812_IO_Low, (uint32_t)&WS2812B_PORT->BSRR, BUFFER_SIZE);
}

// Transmit the frame buffer
int ws2812b_paint()
{
	int cnt = 1000;
	while(!_transferComplete && cnt--);

	if(_transferComplete)
	{
		// transmission complete flag
		_transferComplete = 0;

		// clear all DMA flags
		__HAL_DMA_CLEAR_FLAG(&dmaUpdate, DMA_FLAG_TCIF1_5 | DMA_FLAG_HTIF1_5 | DMA_FLAG_TEIF1_5);
		__HAL_DMA_CLEAR_FLAG(&dmaCC1, DMA_FLAG_TCIF1_5 | DMA_FLAG_HTIF1_5 | DMA_FLAG_TEIF1_5);
		__HAL_DMA_CLEAR_FLAG(&dmaCC2, DMA_FLAG_TCIF2_6 | DMA_FLAG_HTIF2_6 | DMA_FLAG_TEIF2_6);


		// configure the number of bytes to be transferred by the DMA controller
		dmaUpdate.Instance->NDTR = BUFFER_SIZE;
		dmaCC1.Instance->NDTR = BUFFER_SIZE;
		dmaCC2.Instance->NDTR = BUFFER_SIZE;

		// clear all TIM2 flags
		__HAL_TIM_CLEAR_FLAG(&TIM1_handle, TIM_FLAG_UPDATE | TIM_FLAG_CC1 | TIM_FLAG_CC2 | TIM_FLAG_CC3 | TIM_FLAG_CC4);

		// enable DMA channels
		__HAL_DMA_ENABLE(&dmaUpdate);
		__HAL_DMA_ENABLE(&dmaCC1);
		__HAL_DMA_ENABLE(&dmaCC2);

		// IMPORTANT: enable the TIM2 DMA requests AFTER enabling the DMA channels!
		__HAL_TIM_ENABLE_DMA(&TIM1_handle, TIM_DMA_UPDATE);
		__HAL_TIM_ENABLE_DMA(&TIM1_handle, TIM_DMA_CC1);
		__HAL_TIM_ENABLE_DMA(&TIM1_handle, TIM_DMA_CC2);

		TIM1->CNT = tim_period-1;

		// start TIM2
		__HAL_TIM_ENABLE(&TIM1_handle);

		return 1;
	}

	return 0;
}


void DMA2_Stream2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&dmaCC2);
}

void TIM1_UP_TIM10_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM1_handle);
}

// TIM2 Interrupt Handler gets executed on every TIM2 Update if enabled
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	TIM1->CR1 = 0; // disable timer

	// disable the TIM2 Update IRQ
	__HAL_TIM_DISABLE_IT(&TIM1_handle, TIM_IT_UPDATE);

	// Set back 1,25us period
	TIM1->ARR = tim_period;

	// Generate an update event to reload the Pre-scaler value immediately
	TIM1->EGR = TIM_EGR_UG;
	__HAL_TIM_CLEAR_FLAG(&TIM1_handle, TIM_FLAG_UPDATE);

	// set transfer_complete flag
	_transferComplete = 1;
}

void ws2812b_clear()
{
	memset(ws2812bDmaBitBuffer, 0xff, sizeof(ws2812bDmaBitBuffer));
}


void ws2812b_set_pixel(uint8_t row, uint16_t column, uint8_t red, uint8_t green, uint8_t blue)
{
	if(row  > 15)
		return;

	if(column > 15)
		return;

	// Apply gamma
		red = gammaTable[red];
		green = gammaTable[green];
		blue = gammaTable[blue];

	int offset = column * 24;
	int bit = row / 2;
	if(row && (row % 2))
	{
		offset = 24 * (31 - column);
	}
	uint8_t mask = (0x01 << (bit % 8));
	uint8_t not_mask = ~(mask);


	//Green
	for (int k = 0; k < 8; ++k)
	{
		if(green & (1 << (7 - k)))
			ws2812bDmaBitBuffer[k + offset] &= not_mask;
		else
			ws2812bDmaBitBuffer[k + offset] |= mask;
	}

	//red
	for (int k = 8; k < 16; ++k)
	{
		if(red & (1 << (15 - k)))
			ws2812bDmaBitBuffer[k + offset] &= not_mask;
		else
			ws2812bDmaBitBuffer[k + offset] |= mask;
	}

	//blue
	for (int k = 16; k < 24; ++k)
	{
		if(blue & (1 << (23 - k)))
			ws2812bDmaBitBuffer[k + offset] &= not_mask;
		else
			ws2812bDmaBitBuffer[k + offset] |= mask;
	}
}


void ws2812b_init()
{
	ws2812b_gpio_init();
	ws2812b_clear();

	DMA2_init();
	TIM1_init();

//	//set all pixels to the 'error'; on GPIO with first DMA transfer
//	for (int r = 0; r < 16; ++r) {
//		for (int c = 0; c < 16; ++c) {
//			ws2812b_set_pixel(r, c, 0, 0x80, 0);
//		}
//	}

	//clear lights but the firs DMA transfer has an error
	while(!ws2812b_paint())
		printf("WS2812: Clear\n");

	//clear again to cancel DMA error
	while(!ws2812b_paint())
			printf("WS2812: Clear\n");
}


void paint(uint8_t argc, char **argv)
{
    printf("Painting a single color...\n");

    if(argc > 3)
    {
        int red = atoi(argv[1]);
        int g = atoi(argv[2]);
        int b = atoi(argv[3]);

        //      printf("Set %c = %d\n", color, val);
        //      if(color == 'r')
        //        {
        for (int r = 0; r < 16; ++r) {
            for (int c = 0; c < 16; ++c) {
                ws2812b_set_pixel(r,c,red,g,b);
            }
        }
        //        }
        //      if(color == 'g')
        //        {
        //          for (int r = 0; r < 16; ++r) {
        //              for (int c = 0; c < 16; ++c) {
        //                  ws2812b_set_pixel(r,c,0,val,0);
        //              }
        //          }
        //        }
        //      if(color == 'b')
        //        {
        //          for (int r = 0; r < 16; ++r) {
        //              for (int c = 0; c < 16; ++c) {
        //                  ws2812b_set_pixel(r,c,0,0,val);
        //              }
        //          }
        //        }
    }

    ws2812b_paint();
}
