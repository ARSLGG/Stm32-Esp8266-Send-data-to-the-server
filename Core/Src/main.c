#include "main.h"
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal_uart.h"

#define Wifi_name "Wifi-name"
#define Wifi_pass "Wifi-pass"
#define Server "Server" /*example : muharremarslan.com or 192.168.1.2*/
char Tx_buffer[250];
char Rx_buffer[500];
int Rx_indx;
char *read;

UART_HandleTypeDef huart1;
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
void ESP8266_INIT();
void Send_data(int32_t value);
void clear_Rxbuffer();

int main(void) {

	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	/*Send ESP8266 commands*/
	ESP8266_INIT();
	while (1) {
		int cnt;
		Send_data(cnt++); /*Our meter data to be sent */
		HAL_Delay(5000);
	}
}
/* Clear Rx_Buffer */
void clear_Rxbuffer() {
	for (int i = 0; i < 500; i++)
		Rx_buffer[i] = 0;
	Rx_indx = 0;
}
/* Esp8266 start */
void ESP8266_INIT() {
	sprintf(Tx_buffer, "AT+RST\r\n");
	HAL_UART_Transmit_IT(&huart1, (uint8_t*) Tx_buffer, strlen(Tx_buffer));
	HAL_Delay(3000);
	clear_Rxbuffer();
	do {
		sprintf(Tx_buffer, "AT\r\n");
		HAL_UART_Transmit_IT(&huart1, (uint8_t*) Tx_buffer, strlen(Tx_buffer));
		HAL_Delay(500);
		read = strstr(Rx_buffer, "OK");
	} while (read == NULL);
	clear_Rxbuffer();

	do {
		sprintf(Tx_buffer, "AT+CWMODE=1\r\n");
		HAL_UART_Transmit_IT(&huart1, (uint8_t*) Tx_buffer, strlen(Tx_buffer));
		HAL_Delay(500);
		read = strstr(Rx_buffer, "OK");
	} while (read == NULL);
	clear_Rxbuffer();

	char str[100];
	do {
		strcpy(str, "AT+CWJAP=\"");
		strcat(str, Wifi_name);
		strcat(str, "\",\"");
		strcat(str, Wifi_pass);
		strcat(str, "\"\r\n");
		sprintf(Tx_buffer, "%s", str);
		HAL_UART_Transmit_IT(&huart1, (uint8_t*) Tx_buffer, strlen(Tx_buffer));
		HAL_Delay(6000);
		read = strstr(Rx_buffer, "OK");
	} while (read == NULL);
	clear_Rxbuffer();
}

/* Esp8266 send data */
void Send_data(int32_t value) {
	char local_txA[200];
	char local_txB[50];
	int len;

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	clear_Rxbuffer();
	do {
		sprintf(Tx_buffer, "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", Server);
		HAL_UART_Transmit_IT(&huart1, (uint8_t*) Tx_buffer, strlen(Tx_buffer));
		HAL_Delay(100);
		read = strstr(Rx_buffer, "CONNECT");
	} while (read == NULL);
	clear_Rxbuffer();
	do {
		sprintf(local_txA,
				"GET /valua.php?value=%ld HTTP/1.0\r\nHost: %s\r\n\r\n", value,
				Server);
		len = strlen(local_txA);
		sprintf(local_txB, "AT+CIPSEND=%d\r\n", len);
		HAL_UART_Transmit_IT(&huart1, (uint8_t*) local_txB, strlen(local_txB));
		HAL_Delay(100);
		read = strstr(Rx_buffer, ">");
	} while (read == NULL);
	HAL_UART_Transmit_IT(&huart1, (uint8_t*) local_txA, strlen(local_txA));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 7;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 8;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
		Error_Handler();
	}
}

static void MX_USART1_UART_Init(void) {

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
}

static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void Error_Handler(void) {
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

