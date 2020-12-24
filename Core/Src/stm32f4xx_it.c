
#include "main.h"
#include "stm32f4xx_it.h"
char Rx_data[1];

extern UART_HandleTypeDef huart1;

extern char Rx_buffer[500];
extern int Rx_indx;


void NMI_Handler(void)
{
}


void HardFault_Handler(void)
{

  while (1)
  {

  }
}


void MemManage_Handler(void)
{
  while (1)
  {

  }
}


void BusFault_Handler(void)
{

  while (1)
  {

  }
}


void UsageFault_Handler(void)
{

  while (1)
  {

  }
}


void SVC_Handler(void)
{

}

void DebugMon_Handler(void)
{

}


void PendSV_Handler(void)
{

}


void SysTick_Handler(void)
{

  HAL_IncTick();

}


void USART1_IRQHandler(void)
{

  HAL_UART_IRQHandler(&huart1);
        uint8_t i;
  	  	if (Rx_data[0] != '\n')
  	  			{
  	  		Rx_buffer[Rx_indx++] = Rx_data[0]; /*Gelen Dataları Rx_buffer'ın içine at*/
  	  	}
  	  	HAL_UART_Receive_IT(&huart1, (uint8_t*) Rx_data, 1);

}
