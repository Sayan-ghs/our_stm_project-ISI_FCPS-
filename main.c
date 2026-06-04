#include "main.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include<stdlib.h>
#include <math.h>
//#include <time.h>
#include"function.h"
#include"input.h"
//#include "input_13.h"
// #include "input_15.h"
//#include "input_20.h"

//#define Max_Tasks 60

// extern void initialise_monitor_handles(void);

/* --- Hardware Addresses --- */
 #define RCC_BASE         0x58024400UL
 #define RCC_AHB4ENR      (*(volatile uint32_t *)(RCC_BASE + 0xE0))
 #define RCC_APB1LENR     (*(volatile uint32_t *)(RCC_BASE + 0xE8))
#define RCC_APB4ENR (*(volatile uint32_t *)(RCC_BASE + 0xF4))

 #define GPIOD_BASE       0x58020C00UL
 #define GPIOD_MODER      (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
 #define GPIOD_AFRH       (*(volatile uint32_t *)(GPIOD_BASE + 0x24))

 #define USART3_BASE      0x40004800UL
 #define USART3_CR1       (*(volatile uint32_t *)(USART3_BASE + 0x00))
 #define USART3_BRR       (*(volatile uint32_t *)(USART3_BASE + 0x0C))
 #define USART3_ISR       (*(volatile uint32_t *)(USART3_BASE + 0x1C))
 #define USART3_TDR       (*(volatile uint32_t *)(USART3_BASE + 0x28))

 #define DWT_CONTROL      (*(volatile uint32_t *)0xE0001000)
 #define DWT_CYCCNT       (*(volatile uint32_t *)0xE0001004)
 #define DEMCR            (*(volatile uint32_t *)0xE000EDFC)
 #define CPACR            (*(volatile uint32_t *)0xE000ED88)

 #define GPIOB_BASE       0x58020400UL
 #define GPIOB_MODER      (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
 #define GPIOB_ODR        (*(volatile uint32_t *)(GPIOB_BASE + 0x14))


// /* --- RCC Register Definitions --- */
// #define RCC_BASE         0x58024400UL
 #define RCC_CR           (*(volatile uint32_t *)(RCC_BASE + 0x00))
 #define RCC_CFGR         (*(volatile uint32_t *)(RCC_BASE + 0x10))
 #define RCC_D1CFGR       (*(volatile uint32_t *)(RCC_BASE + 0x18))
 #define RCC_D2CFGR       (*(volatile uint32_t *)(RCC_BASE + 0x1C))
 #define RCC_D3CFGR       (*(volatile uint32_t *)(RCC_BASE + 0x20))
 #define RCC_PLLCKSELR    (*(volatile uint32_t *)(RCC_BASE + 0x28))

/* --- Cortex-M7 Cache Control Registers --- */
#define SCB_BASE         0xE000ED00UL
#define SCB_CCR          (*(volatile uint32_t *)(SCB_BASE + 0x14))
#define SCB_ICIALLU      (*(volatile uint32_t *)(SCB_BASE + 0x50))
#define SCB_CSSELR       (*(volatile uint32_t *)(SCB_BASE + 0x24))
#define SCB_DCISW        (*(volatile uint32_t *)(SCB_BASE + 0x4C))

#define FAST_DATA __attribute__((section(".dtcm_data"))) __attribute__((aligned(32)))



/* Custom Bare-Metal Cache Enable Function */
//void cache_enable(void) {
//    // 1. Enable Instruction Cache (I-Cache)
//    if (!(SCB_CCR & (1UL << 17))) {
//        __asm("DSB");
//        __asm("ISB");
//        SCB_ICIALLU = 0UL;          // Invalidate I-Cache before enabling
//        __asm("DSB");
//        __asm("ISB");
//        SCB_CCR |= (1UL << 17);     // Set bit 17 to enable I-Cache
//        __asm("DSB");
//        __asm("ISB");
//    }
//
//    // 2. Enable Data Cache (D-Cache)
//    if (!(SCB_CCR & (1UL << 16))) {
//        SCB_CCR |= (1UL << 16);     // Set bit 16 to enable D-Cache
//        __asm("DSB");
//        __asm("ISB");
//    }
//}

// #define RCC_AHB4ENR      (*(volatile uint32_t *)(RCC_BASE + 0xE0))

/* --- Simulation Parameters --- */

// #define n 3
//#define SIM_TICKS 30000

//#define n 5
//#define SIM_TICKS 30000

// #define n 7
#define SIM_TICKS 30000

#define n 10
//#define SIM_TICKS 600

//#define n 11
//#define SIM_TICKS 600

//#define n 12


// #define n 13
// #define SIM_TICKS 30000

//#define n 15
//#define SIM_TICKS 600

//#define n 20
//#define SIM_TICKS 600

/* --- Global Storage (AXI SRAM) --- */
// Using globals prevents Stack Overflow for large simulations
uint32_t overhead_archive[SIM_TICKS+1];
uint8_t task_history[SIM_TICKS+1];
//Task tasks[30];
//static int hit_miss_matrix[n][SIM_TICKS];

void fpu_enable(void) {
    CPACR |= ((3UL << 20) | (3UL << 22));
    __asm("DSB");
    __asm("ISB");
}


void uart_init(void) {
   RCC_AHB4ENR |= (1 << 3);
   RCC_APB1LENR |= (1 << 18);

   GPIOD_MODER &= ~((3 << 16) | (3 << 18));
   GPIOD_MODER |=  ((2 << 16) | (2 << 18));
   GPIOD_AFRH &= ~((0xF << 0) | (0xF << 4));
   GPIOD_AFRH |=  ((7 << 0) | (7 << 4));

//   USART3_BRR = 120000000 / 115200; // Assuming 64MHz HSI
   USART3_BRR = 100000000 / 115200;
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

//void SystemClock_Config_480MHz(void)
//{
//    /* Enable PWR clock */
//	RCC_APB4ENR |= (1 << 28);   // PWR clock enable
////    (void)RCC->APB4ENR;
//	(void)RCC_APB4ENR;
//
//    /* Direct SMPS supply */
//    PWR->CR3 &= ~(PWR_CR3_LDOEN | PWR_CR3_BYPASS);
//    PWR->CR3 |= PWR_CR3_SMPSEN;
//
//    /* Voltage Scale 0 for 480 MHz */
//    PWR->D3CR &= ~PWR_D3CR_VOS;
//    PWR->D3CR |= PWR_D3CR_VOS_1 | PWR_D3CR_VOS_0;
//    while ((PWR->D3CR & PWR_D3CR_VOSRDY) == 0);
//
//    /* Flash latency 4 wait states */
//    FLASH->ACR &= ~FLASH_ACR_LATENCY;
//    FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
//
//    /* Enable HSI 64 MHz */
//    RCC->CR |= RCC_CR_HSION;
//    while ((RCC->CR & RCC_CR_HSIRDY) == 0);
//
//    /* Disable PLL1 before configuration */
//    RCC->CR &= ~RCC_CR_PLL1ON;
//    while ((RCC->CR & RCC_CR_PLL1RDY) != 0);
//
//    /*
//       HSI = 64 MHz
//       PLLM = 4  -> 64 / 4 = 16 MHz
//       PLLN = 60 -> 16 * 60 = 960 MHz
//       PLLP = 2  -> 960 / 2 = 480 MHz SYSCLK
//    */
//
//    RCC->PLLCKSELR = 0;
//    RCC->PLLCKSELR |= RCC_PLLCKSELR_PLLSRC_HSI;
//    RCC->PLLCKSELR |= (4U << RCC_PLLCKSELR_DIVM1_Pos);
//
//    RCC->PLLCFGR = 0;
//    RCC->PLLCFGR |= RCC_PLLCFGR_PLL1VCOSEL;
//    RCC->PLLCFGR |= RCC_PLLCFGR_PLL1RGE_2;
//    RCC->PLLCFGR |= RCC_PLLCFGR_DIVP1EN;
//
//    RCC->PLL1DIVR = 0;
//    RCC->PLL1DIVR |= ((60U - 1U) << RCC_PLL1DIVR_N1_Pos);
//    RCC->PLL1DIVR |= ((2U  - 1U) << RCC_PLL1DIVR_P1_Pos);
//    RCC->PLL1DIVR |= ((5U  - 1U) << RCC_PLL1DIVR_Q1_Pos);
//    RCC->PLL1DIVR |= ((2U  - 1U) << RCC_PLL1DIVR_R1_Pos);
//
//    RCC->PLL1FRACR = 0;
//
//    /* Bus prescalers */
//    RCC->D1CFGR = 0;
//    RCC->D1CFGR |= RCC_D1CFGR_D1CPRE_DIV1;   // CPU = 480 MHz
//    RCC->D1CFGR |= RCC_D1CFGR_HPRE_DIV2;     // AHB = 240 MHz
//    RCC->D1CFGR |= RCC_D1CFGR_D1PPRE_DIV2;   // APB3 = 120 MHz
//
//    RCC->D2CFGR = 0;
//    RCC->D2CFGR |= RCC_D2CFGR_D2PPRE1_DIV2;  // APB1 = 120 MHz
//    RCC->D2CFGR |= RCC_D2CFGR_D2PPRE2_DIV2;  // APB2 = 120 MHz
//
//    RCC->D3CFGR = 0;
//    RCC->D3CFGR |= RCC_D3CFGR_D3PPRE_DIV2;   // APB4 = 120 MHz
//
//    /* Enable PLL1 */
//    RCC->CR |= RCC_CR_PLL1ON;
//    while ((RCC->CR & RCC_CR_PLL1RDY) == 0);
//
//    /* Select PLL1 as SYSCLK */
//    RCC->CFGR &= ~RCC_CFGR_SW;
//    RCC->CFGR |= RCC_CFGR_SW_PLL1;
//
//    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL1);
//
//    SystemCoreClock = 480000000U;
//}

void SystemClock_Config_400MHz(void)
{
    RCC_APB4ENR |= (1 << 28);
    (void)RCC_APB4ENR;

    PWR->CR3 &= ~(PWR_CR3_LDOEN | PWR_CR3_BYPASS);
    PWR->CR3 |= PWR_CR3_SMPSEN;

    /* Voltage Scale 1 for 400 MHz */
    PWR->D3CR &= ~PWR_D3CR_VOS;
    PWR->D3CR |= PWR_D3CR_VOS_1;
    while ((PWR->D3CR & PWR_D3CR_VOSRDY) == 0);

    /* Flash latency 4 wait states */
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_4WS;

    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0);

    RCC->CR &= ~RCC_CR_PLL1ON;
    while ((RCC->CR & RCC_CR_PLL1RDY) != 0);

    /*
       HSI = 64 MHz
       PLLM = 4  -> 16 MHz
       PLLN = 50 -> 800 MHz
       PLLP = 2  -> 400 MHz SYSCLK
    */

    RCC->PLLCKSELR = 0;
    RCC->PLLCKSELR |= RCC_PLLCKSELR_PLLSRC_HSI;
    RCC->PLLCKSELR |= (4U << RCC_PLLCKSELR_DIVM1_Pos);

    RCC->PLLCFGR = 0;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLL1VCOSEL;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLL1RGE_2;
    RCC->PLLCFGR |= RCC_PLLCFGR_DIVP1EN;

    RCC->PLL1DIVR = 0;
    RCC->PLL1DIVR |= ((50U - 1U) << RCC_PLL1DIVR_N1_Pos);
    RCC->PLL1DIVR |= ((2U  - 1U) << RCC_PLL1DIVR_P1_Pos);
    RCC->PLL1DIVR |= ((5U  - 1U) << RCC_PLL1DIVR_Q1_Pos);
    RCC->PLL1DIVR |= ((2U  - 1U) << RCC_PLL1DIVR_R1_Pos);

    RCC->PLL1FRACR = 0;

    RCC->D1CFGR = 0;
    RCC->D1CFGR |= RCC_D1CFGR_D1CPRE_DIV1;   // CPU = 400 MHz
    RCC->D1CFGR |= RCC_D1CFGR_HPRE_DIV2;     // AHB = 200 MHz
    RCC->D1CFGR |= RCC_D1CFGR_D1PPRE_DIV2;   // APB3 = 100 MHz

    RCC->D2CFGR = 0;
    RCC->D2CFGR |= RCC_D2CFGR_D2PPRE1_DIV2;  // APB1 = 100 MHz
    RCC->D2CFGR |= RCC_D2CFGR_D2PPRE2_DIV2;  // APB2 = 100 MHz

    RCC->D3CFGR = 0;
    RCC->D3CFGR |= RCC_D3CFGR_D3PPRE_DIV2;   // APB4 = 100 MHz

    RCC->CR |= RCC_CR_PLL1ON;
    while ((RCC->CR & RCC_CR_PLL1RDY) == 0);

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL1;

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL1);

    SystemCoreClock = 400000000U;
}

//void Error_Handler(void)
//{
//    __disable_irq();
//    while (1)
//    {
//    }
//}

/* --- Global Storage (Ultra-Fast 400 MHz DTCM) --- */
//__attribute__((section(".dtcmram"))) float x_nom_0[30][10];
//__attribute__((section(".dtcmram"))) float x_hm_0[30][10];
//__attribute__((section(".dtcmram"))) float x_nom_1[30][10];
//__attribute__((section(".dtcmram"))) float x_hm_1[30][10];
//
//__attribute__((section(".dtcmram"))) float u_nom_0[30];
//__attribute__((section(".dtcmram"))) float u_nom_1[30];
//__attribute__((section(".dtcmram"))) float u_hm_0[30];
//__attribute__((section(".dtcmram"))) float u_hm_1[30];

/* --- Global Storage (Forcing to the raw hardware block name) --- */
//__attribute__((section(".dtcm"))) float x_nom_0[30][10];
//__attribute__((section(".dtcm"))) float x_hm_0[30][10];
//__attribute__((section(".dtcm"))) float x_nom_1[30][10];
//__attribute__((section(".dtcm"))) float x_hm_1[30][10];
//
//__attribute__((section(".dtcm"))) float u_nom_0[30];
//__attribute__((section(".dtcm"))) float u_nom_1[30];
//__attribute__((section(".dtcm"))) float u_hm_0[30];
//__attribute__((section(".dtcm"))) float u_hm_1[30];

FAST_DATA float x_nom_0[30][10];
FAST_DATA float x_hm_0[30][10];
FAST_DATA float x_nom_1[30][10];
FAST_DATA float x_hm_1[30][10];

FAST_DATA float u_nom_0[30];
FAST_DATA float u_nom_1[30];
FAST_DATA float u_hm_0[30];
FAST_DATA float u_hm_1[30];

int main(void)
{
    fpu_enable();
    SCB_EnableICache();
    SCB_EnableDCache();
    led_init(); // ADD THIS LINE

//    GPIOB_ODR |= 1;

   // IMPORTANT: Start with the clock reset
    SystemClock_Config_400MHz();

//	initialise_monitor_handles(); // This connects the board to your PC files
//	HAL_Init();


   uart_init();
//   printf("SystemCoreClock = %lu\r\n", SystemCoreClock);

//     DWT_Init();
//     SysTick_Config(SYSCLK_HZ / 1000UL);
//
//     for (volatile uint32_t i = 0; i < 100000; i++);
//
//     random_seed_init();

  // Enable DWT Cycle Counter
   DEMCR |= (1 << 24);
   DWT_CYCCNT = 0;
   DWT_CONTROL |= 1;

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

//    float x_nom_0[30][10], x_hm_0[30][10], x_nom_1[30][10], x_hm_1[30][10];
//    float u_nom_0[30], u_nom_1[30], u_hm_0[30], u_hm_1[30];
    float max_util=0;
    int proc_util = 0;
    float util=0;
    float sch_ratio=0;
    int pos;

    // WCET
//    int wcet[] = {3, 2, 4}; // n = 3, Sim_ticks = 40

//    int wcet[] = {3, 2, 5, 2, 2}; // n=5, sim_ticks = 120

//    int wcet[] = {3,2,4,2,2,2,4}; // n = 7, sim_ticks = 600

//      int wcet[] = {2,2,3,2,2,2,3,3,2,3}; //n=10
      int wcet[] = {3,2,4,2,2,2,4,4,3,3}; //n=10_upd

//    int wcet[] = {3,2,4,2,2,2,4,4,3,3,2}; //n=11
//    int wcet[] = {2,2,4,2,2,2,4,4,4,4,3}; // modified 11_1
//      int wcet[] = {2,2,3,2,2,2,3,3,2,2,2}; //modified 11_2

//    int wcet[] = {2,2,4,2,2,2,3,3,2,2,2,2}; //n=12

//    int wcet[] = {2,2,4,2,2,2,3,3,2,2,2,2,2}; // n = 13, sim_ticks = 600

//    int wcet[] = {2,2,4,2,2,2,3,3,2,2,2,2,2,2,2}; // n = 15, sim_ticks = 600

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
    GPIOB_ODR |= (1 << 0); // LED ON

    /* --- Simulation Engine --- */
//    printf("\nhello1\n");
    for (int t = 0; t <= SIM_TICKS; t++) {
//    	printf("\nHello loop\n");
            uint32_t start = DWT_CYCCNT;

            // =========================================================================== //
            /****************************   SCHEDULING LOGIC ****************************** */
            // =========================================================================== //

            flag_srsp_and_edf_list = 0;
            flag_only_edf_list = 0;
            job_selected=0;
            best=100;
//            if(t<= 200){
//                printf("Time: %d\n",t);
//            }


            // *************************** Job Removal  ***********************************
			if (t!=0 && size_EDF_list!=0 && only_EDF_list[0].abs_deadline==t){
				while(1){
					 if(t == only_EDF_list[0].abs_deadline){
						 int i = only_EDF_list[0].id-1;
						 job = only_EDF_list[0];
						 lst[i] = (job.abs_deadline / job.period);
						 tot[i] = tot[i] + 1;
						 hit_or_miss[i]=0;
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
//                                if(t == 0){
//                                        srsp_prio[i] = 1;
//                                }
//                                else{
//                                        int pos = task_set[i].abs_deadline/task_set[i].period;
//                                        srsp_prio[i] = srspPriorityGen(pos, inp_detail[i], x_nom_0[i], u_nom_0[i], x_hm_0[i], u_hm_0[i], x_nom_1[i], &u_nom_1[i], x_hm_1[i], &u_hm_1[i], lst[i], tot[i], hit_or_miss[i]);
//                                        for (int k=0;k<inp_detail[i].dim;k++){
//                                        	x_nom_0[i][k]=  x_nom_1[i][k];
//                                        	x_hm_0[i][k] =  x_hm_1[i][k];
//										}
//                                        u_nom_0[i]=  u_nom_1[i];
//                                        u_hm_0[i] =  u_hm_1[i];
//                                }


                            	if (t==0){
                            		pos=0;
                            	}else{
                            		pos = task_set[i].abs_deadline/task_set[i].period-1;
                            	}

//                            	printf("\n Task: %d,    perd: %d,   deadline: %d,  pos: %d \t",i+1, task_set[i].period, task_set[i].abs_deadline,pos);


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
//								printf(" SRSP: %d \n",srsp_prio[i]);
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

//                                if (i==0){
//                                	printf("\n Task %d, t: %d, srsp: %d \n", i+1, t, srsp_prio[i]);
//                                	printf("x_nom_1 = [ ");
//									for (int k=0;k<inp_detail[i].dim;k++){
//										printf("%f ",  x_nom_1[i][k]);
//									}
//									printf("] \n x_hm_1 = [ ");
//									for (int k=0;k<inp_detail[i].dim;k++){
//										printf("%f ",  x_hm_1[i][k]);
//									}
//									printf("] \n");
//                                }


                                if(srsp_prio[i] == 0){
                                        binary_search_add_element(only_EDF_list, size_EDF_list, task_set[i]);
                                        size_EDF_list++;
                                }else{
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
                        if(t != SIM_TICKS){
                        	jobs_scheduled += 1;
						}
                }
            }
//
//            if (t<=200){
//				printf("\n At the End \n\n");
//				printf("srsp_EDF_List\t");
//
//				for(int k = 0; k<size_SRSP_EDF_list; k++){
//					printf("%d, %d,  %d, %d\n",srsp_and_EDF_list[k].id,srsp_and_EDF_list[k].period, srsp_and_EDF_list[k].abs_deadline,srsp_and_EDF_list[k].remaining );
//				}
//
//				printf("\n only_EDF_list: \t");
//
//				for(int k = 0; k<size_EDF_list; k++){
//					printf("%d ,%d , %d,  %d\n",only_EDF_list[k].id,only_EDF_list[k].period,only_EDF_list[k].abs_deadline, only_EDF_list[k].remaining);
//				}
//            }

            /********************************************************************************/
            /********************************************************************************/

            uint32_t end = DWT_CYCCNT;

            // Store performance data
            overhead_archive[t] = end - start;
            task_history[t] = (uint8_t)best;
    }
//    HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET); // LED OFF
    GPIOB_ODR &= ~(1 << 0); // LED OFF



    /******************************  PHASE 2: Post-Processing & Reporting  **************************** */
//    printf("SIMULATION COMPLETE. REPORTING RESULTS...\r\n");
    uint32_t sum = 0;
    uint32_t avg = 0;
    int max = 0;
    for (int i = 0; i <= SIM_TICKS; i++) {
       // Convert cycles to nanoseconds (assuming 64MHz)
       // ns = (cycles * 1000) / 64
       uint32_t ns = (overhead_archive[i] * 1000) / 400;
       uint32_t us_int = ns / 1000;
       uint32_t us_frac = ns % 1000;
//       if (i<=250){
//		   if (task_history[i] != 100) {
//			   printf("[%05dms] Task %d | Overhead: %lu.%03lu us\r\n",
//					   i, task_history[i], us_int, us_frac);
//		   } else {
//			   printf("[%05dms] Idle    | Overhead: %lu.%03lu us\r\n",
//					   i, us_int, us_frac);
//		   }
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
//    sch_ratio = (float)jobs_scheduled / (float)total_jobs;
//    util = (float)proc_util / (float) SIM_TICKS;
//    printf("\n\n jobs_scheduled, total_jobs : (%d, %d)", jobs_scheduled, total_jobs);
//    printf("\n units consumed, total bandwidth : (%d, %d)\n", proc_util,SIM_TICKS);
//    printf("\n\n Schedulability Ratio: %f", sch_ratio);
//    printf("\n Actual Utilization : %f \n", util);
//    printf("--- ALL DATA REPORTED ---\r\n");

}


//updated code edited on : 04/06