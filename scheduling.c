///* USER CODE BEGIN Header */
///**
//  ******************************************************************************
//  * @file           : main.c
//  * @brief          : Main program body
//  ******************************************************************************
//  * @attention
//  *
//  * Copyright (c) 2026 STMicroelectronics.
//  * All rights reserved.
//  *
//  * This software is licensed under terms that can be found in the LICENSE file
//  * in the root directory of this software component.
//  * If no LICENSE file comes with this software, it is provided AS-IS.
//  *
//  ******************************************************************************
//  */
///* USER CODE END Header */
///* Includes ------------------------------------------------------------------*/
//#include "main.h"
//
///* Private includes ----------------------------------------------------------*/
///* USER CODE BEGIN Includes */
//
///* USER CODE END Includes */
//
///* Private typedef -----------------------------------------------------------*/
///* USER CODE BEGIN PTD */
//
///* USER CODE END PTD */
//
///* Private define ------------------------------------------------------------*/
///* USER CODE BEGIN PD */
//
///* DUAL_CORE_BOOT_SYNC_SEQUENCE: Define for dual core boot synchronization    */
///*                             demonstration code based on hardware semaphore */
///* This define is present in both CM7/CM4 projects                            */
///* To comment when developping/debugging on a single core                     */
//#define DUAL_CORE_BOOT_SYNC_SEQUENCE
//
//#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
//#ifndef HSEM_ID_0
//#define HSEM_ID_0 (0U) /* HW semaphore 0*/
//#endif
//#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
//
///* USER CODE END PD */
//
///* Private macro -------------------------------------------------------------*/
///* USER CODE BEGIN PM */
//
///* USER CODE END PM */
//
///* Private variables ---------------------------------------------------------*/
//
//COM_InitTypeDef BspCOMInit;
//
///* USER CODE BEGIN PV */
//
///* USER CODE END PV */
//
///* Private function prototypes -----------------------------------------------*/
//void SystemClock_Config(void);
///* USER CODE BEGIN PFP */
//
///* USER CODE END PFP */
//
///* Private user code ---------------------------------------------------------*/
///* USER CODE BEGIN 0 */
//
///* USER CODE END 0 */
//
///**
//  * @brief  The application entry point.
//  * @retval int
//  */
//int main(void)
//{
//
//  /* USER CODE BEGIN 1 */
//
//  /* USER CODE END 1 */
///* USER CODE BEGIN Boot_Mode_Sequence_0 */
//#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
//  int32_t timeout;
//#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
///* USER CODE END Boot_Mode_Sequence_0 */
//
///* USER CODE BEGIN Boot_Mode_Sequence_1 */
//#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
//  /* Wait until CPU2 boots and enters in stop mode or timeout*/
//  timeout = 0xFFFF;
//  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
//  if ( timeout < 0 )
//  {
//  Error_Handler();
//  }
//#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
///* USER CODE END Boot_Mode_Sequence_1 */
//  /* MCU Configuration--------------------------------------------------------*/
//
//  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
//  HAL_Init();
//
//  /* USER CODE BEGIN Init */
//
//  /* USER CODE END Init */
//
//  /* Configure the system clock */
//  SystemClock_Config();
///* USER CODE BEGIN Boot_Mode_Sequence_2 */
//#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
///* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
//HSEM notification */
///*HW semaphore Clock enable*/
//__HAL_RCC_HSEM_CLK_ENABLE();
///*Take HSEM */
//HAL_HSEM_FastTake(HSEM_ID_0);
///*Release HSEM in order to notify the CPU2(CM4)*/
//HAL_HSEM_Release(HSEM_ID_0,0);
///* wait until CPU2 wakes up from stop mode */
//timeout = 0xFFFF;
//while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
//if ( timeout < 0 )
//{
//Error_Handler();
//}
//#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
///* USER CODE END Boot_Mode_Sequence_2 */
//
//  /* USER CODE BEGIN SysInit */
//
//  /* USER CODE END SysInit */
//
//  /* Initialize all configured peripherals */
//  /* USER CODE BEGIN 2 */
//
//  /* USER CODE END 2 */
//
//  /* Initialize leds */
//  BSP_LED_Init(LED_GREEN);
//  BSP_LED_Init(LED_YELLOW);
//  BSP_LED_Init(LED_RED);
//
//  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
//  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
//
//  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
//  BspCOMInit.BaudRate   = 115200;
//  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
//  BspCOMInit.StopBits   = COM_STOPBITS_1;
//  BspCOMInit.Parity     = COM_PARITY_NONE;
//  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
//  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
//  {
//    Error_Handler();
//  }
//
//  /* Infinite loop */
//  /* USER CODE BEGIN WHILE */
//  while (1)
//  {
//
//    /* USER CODE END WHILE */
//
//    /* USER CODE BEGIN 3 */
//  }
//  /* USER CODE END 3 */
//}
//
///**
//  * @brief System Clock Configuration
//  * @retval None
//  */
//void SystemClock_Config(void)
//{
//  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//
//  /** Supply configuration update enable
//  */
//  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
//
//  /** Configure the main internal regulator output voltage
//  */
//  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
//
//  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
//
//  /** Initializes the RCC Oscillators according to the specified parameters
//  * in the RCC_OscInitTypeDef structure.
//  */
//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
//  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
//  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
//  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Initializes the CPU, AHB and APB buses clocks
//  */
//  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
//                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
//  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
//  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
//  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
//  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
//  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
//  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
//
//  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
//  {
//    Error_Handler();
//  }
//}
//
///* USER CODE BEGIN 4 */
//
///* USER CODE END 4 */
//
///**
//  * @brief  This function is executed in case of error occurrence.
//  * @retval None
//  */
//void Error_Handler(void)
//{
//  /* USER CODE BEGIN Error_Handler_Debug */
//  /* User can add his own implementation to report the HAL error return state */
//  __disable_irq();
//  while (1)
//  {
//  }
//  /* USER CODE END Error_Handler_Debug */
//}
//#ifdef USE_FULL_ASSERT
///**
//  * @brief  Reports the name of the source file and the source line number
//  *         where the assert_param error has occurred.
//  * @param  file: pointer to the source file name
//  * @param  line: assert_param error line source number
//  * @retval None
//  */
//void assert_failed(uint8_t *file, uint32_t line)
//{
//  /* USER CODE BEGIN 6 */
//  /* User can add his own implementation to report the file name and line number,
//     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
//  /* USER CODE END 6 */
//}
//#endif /* USE_FULL_ASSERT */




//#include "stm32h7xx_hal.h"
//#include "main.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include"STM_function.h"
#include"input.h"
// #include "input_13.h"
// #include "input_15.h"
// #include "input_20.h"

// extern void initialise_monitor_handles(void);

/* --- Hardware Addresses --- */
// #define RCC_BASE         0x58024400UL
// #define RCC_AHB4ENR      (*(volatile uint32_t *)(RCC_BASE + 0xE0))
// #define RCC_APB1LENR     (*(volatile uint32_t *)(RCC_BASE + 0xE8))

// #define GPIOD_BASE       0x58020C00UL
// #define GPIOD_MODER      (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
// #define GPIOD_AFRH       (*(volatile uint32_t *)(GPIOD_BASE + 0x24))

// #define USART3_BASE      0x40004800UL
// #define USART3_CR1       (*(volatile uint32_t *)(USART3_BASE + 0x00))
// #define USART3_BRR       (*(volatile uint32_t *)(USART3_BASE + 0x0C))
// #define USART3_ISR       (*(volatile uint32_t *)(USART3_BASE + 0x1C))
// #define USART3_TDR       (*(volatile uint32_t *)(USART3_BASE + 0x28))

// #define DWT_CONTROL      (*(volatile uint32_t *)0xE0001000)
// #define DWT_CYCCNT       (*(volatile uint32_t *)0xE0001004)
// #define DEMCR            (*(volatile uint32_t *)0xE000EDFC)
// #define CPACR            (*(volatile uint32_t *)0xE000ED88)

// #define GPIOB_BASE       0x58020400UL
// #define GPIOB_MODER      (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
// #define GPIOB_ODR        (*(volatile uint32_t *)(GPIOB_BASE + 0x14))


// /* --- RCC Register Definitions --- */
// #define RCC_BASE         0x58024400UL
// #define RCC_CR           (*(volatile uint32_t *)(RCC_BASE + 0x00))
// #define RCC_CFGR         (*(volatile uint32_t *)(RCC_BASE + 0x10))
// #define RCC_D1CFGR       (*(volatile uint32_t *)(RCC_BASE + 0x18))
// #define RCC_D2CFGR       (*(volatile uint32_t *)(RCC_BASE + 0x1C))
// #define RCC_D3CFGR       (*(volatile uint32_t *)(RCC_BASE + 0x20))
// #define RCC_PLLCKSELR    (*(volatile uint32_t *)(RCC_BASE + 0x28))

// #define RCC_AHB4ENR      (*(volatile uint32_t *)(RCC_BASE + 0xE0))

/* --- Simulation Parameters --- */

//#define n 3
//#define SIM_TICKS 40

//#define n 5
//#define SIM_TICKS 120

//#define n 7
//#define SIM_TICKS 600

//#define n 10
//#define SIM_TICKS 600

//#define n 11
//#define SIM_TICKS 600

//#define n 11
//#define SIM_TICKS 600

#define n 13
#define SIM_TICKS 30000

// #define n 15
// #define SIM_TICKS 600

// #define n 20
// #define SIM_TICKS 600

/* --- Global Storage (AXI SRAM) --- */
// Using globals prevents Stack Overflow for large simulations
// uint32_t overhead_archive[SIM_TICKS+1];
// uint8_t task_history[SIM_TICKS+1];
//Task tasks[30];

/* --- Hardware Setup Functions --- */
// void fpu_enable(void) {
//     CPACR |= ((3UL << 20) | (3UL << 22));
//     __asm("DSB");
//     __asm("ISB");
// }

/**
void uart_init(void) {
    RCC_AHB4ENR |= (1 << 3);
    RCC_APB1LENR |= (1 << 18);

    GPIOD_MODER &= ~((3 << 16) | (3 << 18));
    GPIOD_MODER |=  ((2 << 16) | (2 << 18));
    GPIOD_AFRH &= ~((0xF << 0) | (0xF << 4));
    GPIOD_AFRH |=  ((7 << 0) | (7 << 4));

    USART3_BRR = 64000000 / 115200; // Assuming 64MHz HSI
    USART3_CR1 |= (1 << 3) | (1 << 0);
}

void led_init(void) {
    RCC_AHB4ENR |= (1 << 1);  // Enable GPIOB clock
    GPIOB_MODER &= ~(3 << 0); // Clear mode bits for PB0
    GPIOB_MODER |=  (1 << 0); // Set PB0 to General Purpose Output
}

int __io_putchar(int ch) {
    while (!(USART3_ISR & (1 << 7)));
    USART3_TDR = ch;
    return ch;
}


void clock_reset_to_64mhz(void) {
    // 1. Enable HSI (Internal High-Speed Oscillator)
    RCC_CR |= (1 << 0); // HSION bit

    // 2. Wait until HSI is stable
    while (!(RCC_CR & (1 << 2))); // HSIRDY bit

    // 3. Set HSI as the System Clock (SW bits in RCC_CFGR)
    // 000: HSI used as system clock
    RCC_CFGR &= ~(0x7);

    // 4. Wait for HSI to be the active system clock (SWS bits)
    while ((RCC_CFGR & (0x38)) != 0x00);

    // 5. Disable all PLLs to ensure we aren't using an old multiplied clock
    RCC_CR &= ~( (1<<24) | (1<<26) | (1<<28) ); // PLL1, PLL2, PLL3 OFF
}
*/

int main(void) {

//      fpu_enable();

     // IMPORTANT: Start with the clock reset
//      clock_reset_to_64mhz();

//	initialise_monitor_handles(); // This connects the board to your PC files
//	HAL_Init();


//     uart_init();
//     led_init(); // ADD THIS LINE

    // Enable DWT Cycle Counter
//     DEMCR |= (1 << 24);
//     DWT_CYCCNT = 0;
//     DWT_CONTROL |= 1;

    Task task_set[30] = {0};
    Task only_EDF_list[30] = {0};
    Task srsp_and_EDF_list[30] = {0};
    Task job = {0};

    pc_system inp_detail[30];

    int size_EDF_list = 0;
    int size_SRSP_EDF_list = 0;
    int flag_srsp_and_edf_list = 0;
    int flag_only_edf_list = 0;
    int srsp_prio[30];
    int lst[30];
    int tot[30] = {0};
    int hit_or_miss[30];
    int hit_miss_matrix[n][SIM_TICKS];
    for(int i = 0;i<n;i++){
        for(int j = 0; j< SIM_TICKS; j++){
                if(j==0){
                        hit_miss_matrix[i][j] = 1;
                }
                else{
                        hit_miss_matrix[i][j] = -1;
                }
                // hit_miss_matrix[i][j] = 0;  
        }
    }
    for(int i = 0; i < n; i++) {
    	hit_or_miss[i] = 1;
    }
    for(int i = 0; i < n; i++) {
    	srsp_prio[i] = 1;
    }
    for(int i = 0; i < n; i++) {
		lst[i] = -1;
	}
    int job_selected=0;
    int schedule_edf;
    int best;
    int total_jobs = 0;
    int jobs_scheduled = 0;

    float x_nom_0[30][10], x_hm_0[30][10], x_nom_1[30][10], x_hm_1[30][10];
    float u_nom_0[30], u_nom_1[30], u_hm_0[30], u_hm_1[30];
    float max_util=0;
    int proc_util = 0;
    float util=0;
    float sch_ratio=0;
    int pos;

    // WCET
//    int wcet[] = {3, 2, 4}; // n = 3, Sim_ticks = 40

//    int wcet[] = {3, 2, 5, 2, 2}; // n=5, sim_ticks = 120

//    int wcet[] = {3,2,4,2,2,2,4}; // n = 7, sim_ticks = 600

//      int wcet[] = {3,2,4,2,2,2,4,4,3,3}; //n=10

//    int wcet[] = {3,2,4,2,2,2,4,4,3,3,2}; //n=11

//    int wcet[] = {3,2,4,2,2,2,4,4,3,3,2,3}; //n=12

    int wcet[] = {2,2,4,2,2,2,3,3,2,2,2,2,2}; // n = 13, sim_ticks = 600

    // int wcet[] = {2,2,4,2,2,2,3,3,2,2,2,2,2,2,2}; // n = 15, sim_ticks = 600

//    int wcet[] = {2,2,2,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,2}; // n = 20, sim_ticks = 600
//     char *files[] = {"CC","DC","LC"};

    for(int i=0; i<n; i++){

        // char filename[50];
        // // This creates "CC_inputs.txt", "DC_inputs.txt", etc.
        // sprintf(filename, "%s_inputs.txt", files[i]);

        // load_config(filename, &inp_detail[i]);
        inp_detail[i] = system_configs[i];


        task_set[i].id=i+1;
        task_set[i].period=inp_detail[i].h;
        task_set[i].abs_deadline=inp_detail[i].h;
        task_set[i].exec_time=wcet[i];
        task_set[i].remaining=wcet[i];

        memcpy(x_nom_0[i], inp_detail[i].init_vect, sizeof(float) * inp_detail[i].dim);
        memcpy(x_hm_0[i],  inp_detail[i].init_vect, sizeof(float) * inp_detail[i].dim);
        u_nom_0[i] = inp_detail[i].init_input;
        u_hm_0[i] = inp_detail[i].init_input;
        memcpy(x_nom_1[i], inp_detail[i].init_vect, sizeof(float) * inp_detail[i].dim);
        memcpy(x_hm_1[i],  inp_detail[i].init_vect, sizeof(float) * inp_detail[i].dim);
        u_nom_1[i] = inp_detail[i].init_input;
        u_hm_1[i] = inp_detail[i].init_input;

        max_util = max_util + ((float)task_set[i].exec_time/(float)task_set[i].period);
//        printf("Value: %f\n",((float)task_set[i].exec_time/(float)task_set[i].period));
//        printf("i: %d,   Max Util : %f\n",i,max_util);

    }
    if (max_util <=1){
    	schedule_edf = 1;
    }else{
    	schedule_edf = 0;
    }
//    printf("Max Util : %f",max_util);
//    for(int j = 0; j<n; j++){
//    	printf("%d : %d, %d, %d, %d\n",task_set[j].id, task_set[j].period, task_set[j].exec_time, task_set[j].remaining, task_set[j].abs_deadline);
//    }


    /**********   All declaration and necessary initialization and computation */

//    HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET); // LED ON
//     GPIOB_ODR |= (1 << 0); // LED ON

    /* --- Simulation Engine --- */
    for (int t = 0; t <= SIM_TICKS; t++) {
        //     uint32_t start = DWT_CYCCNT;

            // =========================================================================== //
            /****************************   SCHEDULING LOGIC ****************************** */
            // =========================================================================== //

            flag_srsp_and_edf_list = 0;
            flag_only_edf_list = 0;
            job_selected=0;
            best=100;
        //    printf("Time: %d\n",t);


            // *************************** Job Removal  ***********************************
			if (t!=0 && size_EDF_list!=0 && only_EDF_list[0].abs_deadline==t){
				while(1){
					 if(t == only_EDF_list[0].abs_deadline){
						 int i = only_EDF_list[0].id-1;
                                                 job = only_EDF_list[0];
                                                // if(t == 450 && i == 0){
                                                //         printf("\n 1st removal (before) :  deadline = %d , period = %d, lst = %d \n",job.abs_deadline, job.period, lst[i]);
                                                // }
						 lst[i] = (job.abs_deadline / job.period);
						 tot[i] = tot[i] + 1;
						 hit_or_miss[i]=0;
                                                 hit_miss_matrix[i][lst[i]-1] = 0;
                                                //  if(t == 450 && i == 0){
                                                //         printf("\n 1st removal (after) :  deadline = %d , period = %d, lst = %d \n",job.abs_deadline, job.period, lst[i]);
                                                // }
						 remove_first_element(only_EDF_list, size_EDF_list);
						 size_EDF_list--;

						 if (size_EDF_list ==0){
							 break;
						 }

					 }else{
						 break;
					 }
				}
			}


            // *************************** Job Arrival  ***********************************
            for(int i=0;i<n;i++){

//            	    if (t>=520 && t<=530){
//            	    	printf("t, Task 1 period, curr i : %d, %d, %d\n", t, task_set[0].period,i);
//            	    }
//            	    printf("Period : %d\n",task_set[i].period );
                    if(t % task_set[i].period == 0){
//                    	     printf("job arrival : %d, %d\n", t, task_set[i].id);
                            // curr = task_set[i].id;
                    	    if(t != SIM_TICKS){
//                    	        printf("job arrival : %d, %d\n", t, task_set[i].id);
                    	    	total_jobs += 1;
                    	    }

                            task_set[i].abs_deadline = t + task_set[i].period;
                            task_set[i].remaining = task_set[i].exec_time;

//                            if (t<=100){
//								printf("Time: %d, i: %d, SRSp-prio: %d\n", t, i, srsp_prio[i]);
//							}
                            // When total util > 1
                            if(schedule_edf==0){
                            	// SRSP Priority
                            //    if(t == 0){
                            //            srsp_prio[i] = 1;
                            //    }
                            //    else{
                            //            int pos = task_set[i].abs_deadline/task_set[i].period -1;
                            //            srsp_prio[i] = srspPriorityGen(pos, inp_detail[i], x_nom_0[i], u_nom_0[i], x_hm_0[i], u_hm_0[i], x_nom_1[i], &u_nom_1[i], x_hm_1[i], &u_hm_1[i], lst[i], tot[i], hit_or_miss[i]);
                            //            for (int k=0;k<inp_detail[i].dim;k++){
                            //            	x_nom_0[i][k]=  x_nom_1[i][k];
                            //            	x_hm_0[i][k] =  x_hm_1[i][k];
							// 			}
                            //            u_nom_0[i]=  u_nom_1[i];
                            //            u_hm_0[i] =  u_hm_1[i];
                            //    }


                            	if (t==0){
                            		pos=0;
                            	}else{
                            		pos = task_set[i].abs_deadline/task_set[i].period-1;
                            	}

//                            	printf("\n Task: %d,    perd: %d,   deadline: %d,  pos: %d \t",i+1, task_set[i].period, task_set[i].abs_deadline,pos);
                                // printf("\n Task: %d",i+1);


								srsp_prio[i] = srspPriorityGen(pos, inp_detail[i], x_nom_0[i], u_nom_0[i], x_hm_0[i], u_hm_0[i], x_nom_1[i], &u_nom_1[i], x_hm_1[i], &u_hm_1[i], lst[i], tot[i], hit_or_miss[i]);
								for (int k=0;k<inp_detail[i].dim;k++){
									x_nom_0[i][k] =  x_nom_1[i][k];
									x_hm_0[i][k] =  x_hm_1[i][k];
								}
								u_nom_0[i] =  u_nom_1[i];
								u_hm_0[i] =  u_hm_1[i];
								if(t == 0){
									srsp_prio[i] = 1;
								}
								// printf(" SRSP: %d \n",srsp_prio[i]);
//								if(i==8){ //8,9,10
//									printf("%d",hit_or_miss[i]);
//								}
//								if(i==9){ //8,9,10
//									printf("%d",hit_or_miss[i]);
//								}
//								if(i==10){
//									printf("%d",hit_or_miss[i]);
//								}
//								if(i==5){
//									printf("%d",hit_or_miss[i]);
//								}

                            //    if (i==0){
                            //    	printf("\n Task %d, t: %d, srsp: %d \n", i+1, t, srsp_prio[i]);
                            //    	printf("x_nom_1 = [ ");
							// 		for (int k=0;k<inp_detail[i].dim;k++){
							// 			printf("%f ",  x_nom_1[i][k]);
							// 		}
							// 		printf("] \n x_hm_1 = [ ");
							// 		for (int k=0;k<inp_detail[i].dim;k++){
							// 			printf("%f ",  x_hm_1[i][k]);
							// 		}
							// 		printf("] \n");
                            //    }


                                if(srsp_prio[i] == 0){
                                        binary_search_add_element(only_EDF_list, size_EDF_list, task_set[i]);
                                        size_EDF_list++;
                                }else if(srsp_prio[i] == 1){
                                        binary_search_add_element(srsp_and_EDF_list, size_SRSP_EDF_list, task_set[i]);
                                        size_SRSP_EDF_list++;
                                }

			    // when total util <=1, schedule liken EDF
                            }else{
                            	binary_search_add_element(srsp_and_EDF_list, size_SRSP_EDF_list, task_set[i]);
                            	size_SRSP_EDF_list++;
                            }
//                            printf("\n At arrival \n\n");
//                            printf("srsp_EDF_List\t");
//
//                            for(int k = 0; k<size_SRSP_EDF_list; k++){
//                            	printf("%d : %d, %d, %d, %d\n",srsp_and_EDF_list[k].id,srsp_and_EDF_list[k].period, srsp_and_EDF_list[k].exec_time, srsp_and_EDF_list[k].remaining, srsp_and_EDF_list[k].abs_deadline);
//                            }
                    }
            }

//            printf("\n After job arrival\n\n");
//            printf("srsp_EDF_List\t");
//
//            for(int k = 0; k<size_SRSP_EDF_list; k++){
//            	printf("%d : %d, %d, %d, %d\n",srsp_and_EDF_list[k].id,srsp_and_EDF_list[k].period, srsp_and_EDF_list[k].exec_time, srsp_and_EDF_list[k].remaining, srsp_and_EDF_list[k].abs_deadline);
//            }
//
//            printf("\n only_EDF_list: \t");
//
//            for(int k = 0; k<size_EDF_list; k++){
//            	printf("%d : %d, %d, %d, %d\n",only_EDF_list[k].id,only_EDF_list[k].period, only_EDF_list[k].exec_time, only_EDF_list[k].remaining,only_EDF_list[k].abs_deadline);
//            }

//			printf("Task_set\t");
//
//			for(int k = 0; k<n; k++){
//				printf("%d : %d, %d, %d, %d\n",task_set[k].id,task_set[k].period, task_set[k].exec_time, task_set[k].remaining, task_set[k].abs_deadline);
//			}

            // *************************** Job Selection  ***********************************
            while (size_SRSP_EDF_list != 0 || size_EDF_list != 0){
            	job_selected = 0;
            	flag_srsp_and_edf_list = 0;
            	flag_only_edf_list = 0;


                // Job Selection for Scheduling
                if(size_SRSP_EDF_list != 0){
                        job = srsp_and_EDF_list[0];
                        flag_srsp_and_edf_list = 1;
                }
                else{
                        job = only_EDF_list[0];
                        flag_only_edf_list = 1;
                }

                // Checking if 'job' is non-schedulable
                if(flag_only_edf_list ==1) {
                        if(t+job.remaining > job.abs_deadline){
                                int i = job.id-1;
                                lst[i] = (job.abs_deadline / job.period);
                                tot[i] = tot[i] + 1;
                                hit_or_miss[i]=0;
                                hit_miss_matrix[i][lst[i]-1] = 0;
                                // if(t == 450 && i == 0){
                                //         printf("\n 2nd removal :  deadline = %d , period = %d, lst = %d \n",job.abs_deadline, job.period, lst[i]);
                                // }

                                remove_first_element(only_EDF_list, size_EDF_list);
                                size_EDF_list--;
                                continue;
                        }else{
                            job_selected = 1;
                            break;
                        }
                }else{
                        job_selected = 1;
                        break;
                }

            }

            if (job_selected == 1){

                best = job.id-1;
//                printf("%d, %d\n", t, best);

                // Job Execution
//                printf("\n Job remaining before dec : %d, %d\n",job.id,job.remaining);
//                printf("\n Task remaining before dec : %d, %d\n",task_set[best].id,task_set[best].remaining);
                task_set[best].remaining = task_set[best].remaining - 1;
                job.remaining = job.remaining - 1;
                if(flag_srsp_and_edf_list == 1){
                	srsp_and_EDF_list[0].remaining = srsp_and_EDF_list[0].remaining - 1;
                }
                else if(flag_only_edf_list == 1){
                	only_EDF_list[0].remaining = only_EDF_list[0].remaining - 1;
                }
                proc_util += 1;
//                printf("\n Job remaining after dec : %d, %d\n",job.id,job.remaining);
//                printf("\n Task remaining after dec : %d, %d\n",task_set[best].id,task_set[best].remaining);

                // Job Removal on Completion
                if(job.remaining == 0){
//                	printf("flag_srsp_and_edf_list: %d, %d",job.id, flag_srsp_and_edf_list);
//                	printf("flag_only_edf_list: %d, %d",job.id, flag_only_edf_list);
                        if(flag_srsp_and_edf_list == 1){
                                remove_first_element(srsp_and_EDF_list, size_SRSP_EDF_list);
                                size_SRSP_EDF_list--;
                        }
                        else if(flag_only_edf_list == 1){
                                remove_first_element(only_EDF_list, size_EDF_list);
                                size_EDF_list--;
                        }
                        hit_or_miss[best]=1;
                        // printf("\n Before : %d ",jobs_scheduled);
                        hit_miss_matrix[best][(job.abs_deadline/job.period)-1] = 1;

                        // if(t != SIM_TICKS){
                        	jobs_scheduled += 1;
                            // printf("\n After : %d ",jobs_scheduled);
						// }
                }
            }
//
        //    if (t<=50){
		// 		printf("\n At the End \n\n");
		// 		printf("srsp_EDF_List\t");

		// 		for(int k = 0; k<size_SRSP_EDF_list; k++){
		// 			printf("%d, %d,  %d, %d\n",srsp_and_EDF_list[k].id,srsp_and_EDF_list[k].period, srsp_and_EDF_list[k].abs_deadline,srsp_and_EDF_list[k].remaining );
		// 		}

		// 		printf("\n only_EDF_list: \t");
        //                         printf("\ntask1 prio : %d, lst : %d, tot : %d, hit_or_miss : %d\n", srsp_prio[0], lst[0], tot[0], hit_or_miss[0]);

		// 		for(int k = 0; k<size_EDF_list; k++){
		// 			printf("%d ,%d , %d,  %d\n",only_EDF_list[k].id,only_EDF_list[k].period,only_EDF_list[k].abs_deadline, only_EDF_list[k].remaining);
		// 		}
        //    }

            /********************************************************************************/
            /********************************************************************************/

        //     uint32_t end = DWT_CYCCNT; //for overhead calculation

            // Store performance data
        //     overhead_archive[t] = end - start;
        //     task_history[t] = (uint8_t)best;
    }
//    HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET); // LED OFF
//     GPIOB_ODR &= ~(1 << 0); // LED OFF



    /******************************  PHASE 2: Post-Processing & Reporting  **************************** */
//    printf("SIMULATION COMPLETE. REPORTING RESULTS...\r\n");
/**   uint32_t sum = 0;
    uint32_t avg = 0;
    int max = 0;
    for (int i = 0; i <= SIM_TICKS; i++) {
       // Convert cycles to nanoseconds (assuming 64MHz)
       // ns = (cycles * 1000) / 64
       uint32_t ns = (overhead_archive[i] * 1000) / 64;
       uint32_t us_int = ns / 1000;
       uint32_t us_frac = ns % 1000;
//       if (task_history[i] != 100) {
//           printf("[%05dms] Task %d | Overhead: %lu.%03lu us\r\n",
//                   i, task_history[i], us_int, us_frac);
//       } else {
//           printf("[%05dms] Idle    | Overhead: %lu.%03lu us\r\n",
//                   i, us_int, us_frac);
//       }

       if(i != 0 && i != SIM_TICKS)
       {
       sum += 1000*us_int+ us_frac;
       if(max < us_int){
    	   max = us_int;
         }
       }
       // Optional: Slow down the print slightly so the terminal buffer doesn't choke
       if (i % 100 == 0) {
           for(volatile int d=0; d<10000; d++);
       }
    }
//    printf("%lu\r\n",sum);
    avg = sum / (SIM_TICKS-1);
    uint32_t avg_int = avg / 1000;
    uint32_t avg_frac = avg % 1000;
    printf("Average : %lu.%03lu \n",avg_int, avg_frac);
//
    printf("Max overhead : %d\n",max);
//
//
    sch_ratio = (float)jobs_scheduled / (float)total_jobs;
    util = (float)proc_util / (float) SIM_TICKS;
    printf("\n\n jobs_scheduled, total_jobs : (%d, %d)", jobs_scheduled, total_jobs);
    printf("\n units consumed, total bandwidth : (%d, %d)\n", proc_util,SIM_TICKS);
//    printf("\n\n Schedulability Ratio: %f", sch_ratio);
//    printf("\n Actual Utilization : %f \n", util);
    printf("--- ALL DATA REPORTED ---\r\n");
*/
    printf("\n\n jobs_scheduled, total_jobs : (%d, %d)", jobs_scheduled, total_jobs);
    printf("\n units consumed, total bandwidth : (%d, %d)\n", proc_util,SIM_TICKS);
    for(int i = 0; i<n; i++){
        printf("\n\n Task %d : \t",i+1);
        int x = SIM_TICKS / task_set[i].period;
        for(int j = 0; j<x;j++){
                printf("%d",hit_miss_matrix[i][j]);
        }
    }
}
