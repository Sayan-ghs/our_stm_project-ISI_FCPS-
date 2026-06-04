#include "main.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include<stdlib.h>
#include <math.h>
//#include <time.h>
#include"function.h"
#include"input.h"
// #include "input_13.h"
// #include "input_15.h"
//#include "input_20.h"

#define Max_Tasks 60

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

// #define RCC_AHB4ENR      (*(volatile uint32_t *)(RCC_BASE + 0xE0))

/* --- Simulation Parameters --- */

 #define n 3
//#define SIM_TICKS 30000

//#define n 5
//#define SIM_TICKS 30000

// #define n 7
#define SIM_TICKS 30000

//#define n 10
//#define SIM_TICKS 600

//#define n 11
//#define SIM_TICKS 600

//#define n 11
//#define SIM_TICKS 600

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

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

int main(void)
{
    fpu_enable();
    led_init(); // ADD THIS LINE

//    GPIOB_ODR |= 1;

   // IMPORTANT: Start with the clock reset
    SystemClock_Config_400MHz();

//	initialise_monitor_handles(); // This connects the board to your PC files
//	HAL_Init();


   uart_init();

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

//   printf("SystemCoreClock = %lu\r\n", SystemCoreClock);

//   while (1)
//   {
//       GPIOB_ODR ^= 1;
//
//       for (volatile uint32_t i = 0; i < 1000000; i++);
//
//       printf("running\r\n");
//   }

   DEMCR |= (1 << 24);
   DWT_CYCCNT = 0;
   DWT_CONTROL |= 1;

  Task task_set[Max_Tasks] = {0};
  Task only_EDF_list[Max_Tasks] = {0};
  Task srsp_and_EDF_list[Max_Tasks] = {0};
  Task job = {0};

  pc_system inp_detail[30];

  int size_EDF_list = 0;
  int task_accepted = 0;
  int size_SRSP_EDF_list = 0;
  int flag_srsp_and_edf_list = 0;
  int flag_only_edf_list = 0;
  int schedulability_flag = 0;
  int srsp_prio[Max_Tasks];
  int lst[Max_Tasks];
  int tot[Max_Tasks] = {0};
  int hit_or_miss[Max_Tasks];


  // int hit_miss_matrix[n][SIM_TICKS];
//    for(int i = 0;i<n;i++){
//        for(int j = 0; j< SIM_TICKS; j++){
//                if(j==0){
//                        hit_miss_matrix[i][j] = 1;
//                }
//                else{
//                        hit_miss_matrix[i][j] = -1;
//                }
//
//        }
//    }
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
  float min_util=0;
  float show=0;
  int m,k;
  int proc_util = 0;
  float util=0;
  float sch_ratio=0;
  int pos;

  // WCET
  int wcet[] = {3, 2, 4}; // n = 3, Sim_ticks = 40

//    int wcet[] = {3, 2, 5, 2, 2}; // n=5, sim_ticks = 120

//    int wcet[] = {3,2,4,2,2,2,4}; // n = 7, sim_ticks = 600

//      int wcet[] = {3,2,4,2,2,2,4,4,3,3}; //n=10
//      int wcet[] = {2,2,3,2,2,2,3,3,3,3}; //n=10_new

//    int wcet[] = {3,2,4,2,2,2,4,4,3,3,2}; //n=11

//    int wcet[] = {3,2,4,2,2,2,4,4,3,3,2,3}; //n=12

  // int wcet[] = {2,2,4,2,2,2,3,3,2,2,2,2,2}; // n = 13, sim_ticks = 600

//     int wcet[] = {2,2,4,2,2,2,3,3,2,2,2,2,2,2,2}; // n = 15, sim_ticks = 600

//   int wcet[] = {2,2,2,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,2}; // n = 20, sim_ticks = 600
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

      m = inp_detail[i].r;
      k = inp_detail[i].l;
      min_util = min_util + (((float)m / (float)k)*((float)task_set[i].exec_time/(float)task_set[i].period));
//        printf("Value: %f\n",((float)task_set[i].exec_time/(float)task_set[i].period));
//        printf("i: %d,   Max Util : %f\n",i,max_util);

  }
  // printf("min_util : %f\n", min_util);

  /***************    SUDDEN TASK INITIALIZATION ***********************************/
//    srand(time(NULL));  //random seed generator
  // int horizon = 10000;
//    int rand_time;
//    int arrival[Max_Tasks];             // only for sudden tasks
//    int periods[Max_Tasks];             // only for sudden tasks
//    int executionTimes[Max_Tasks];      // only for sudden tasks
//    int hard_or_weakly_hard[Max_Tasks];   // only for sudden tasks
//    int m_k_firm[Max_Tasks][2] = {0};   // only for sudden tasks
  int task_selected[Max_Tasks] = {0};
  for (int i=0;i<n;i++){
      task_selected[i]=1;
  }
  int zero[Max_Tasks]={0};   // only for sudden tasks
  int index[Max_Tasks]={0};  // only for sudden tasks


  // Obtaining sudden tasks
//    obtain_sudden_task_data(SIM_TICKS, &rand_time, arrival, periods, executionTimes, hard_or_weakly_hard);
  // Input Set 1
//    int rand_time = 5;
//
//    int arrival[5] = {120, 850, 2450, 7800, 15200};
//    int periods[5] = {90, 110, 125, 140, 150};
//    int executionTimes[5] = {1, 2, 2, 1, 2};
//
//    int hard_or_weakly_hard[5] = {0, 1, 0, 0, 1};
//
//    int m_k_firm[5][2] = {
//        {2,3},
//        {1,1},
//        {3,5},
//        {4,6},
//        {1,1}
//    };

  // Input Set 2
//    int rand_time = 8;
//
//    int arrival[8] = {75, 620, 1800, 3900, 7600, 11800, 20150, 29500};
//    int periods[8] = {85, 95, 105, 115, 130, 135, 145, 150};
//    int executionTimes[8] = {1, 2, 4, 3, 2, 1, 4, 3};
//
//    int hard_or_weakly_hard[8] = {1, 0, 0, 1, 0, 1, 0, 0};
//
//    int m_k_firm[8][2] = {
//        {1,1},
//        {2,4},
//        {3,5},
//        {1,1},
//        {4,7},
//        {1,1},
//        {2,3},
//        {5,8}
//    };

  // Input Set
  int  rand_time = 9;

  int arrival[9] = {
      78, 326, 891, 1437, 2684,
      4971, 8392, 16483, 28714
  };

  int periods[9] = {
      94, 121, 147, 103, 136,
      89, 128, 144, 111
  };

  int executionTimes[9] = {
      2, 3, 4, 2, 3,
      4, 2, 3, 4
  };

  // 5 ones (~56%), 4 zeros
  int hard_or_weakly_hard[9] = {
      1,0,1,1,0,
      1,0,1,0
  };

  int m_k_firm[9][2] = {
      {1,1},
      {2,4},
      {1,1},
      {1,1},
      {3,5},

      {1,1},
      {4,7},
      {1,1},
      {2,5}
  };

  // Input Set 3
//    int rand_time = 12;
//
//    int arrival[12] = {
//        118, 430, 1250, 2717, 5186, 8667,
//        10350, 14900, 18755, 22408, 26100, 29900
//    };
//
//    int periods[12] = {
//        88, 92, 100, 108, 116, 124,
//        132, 138, 142, 146, 148, 150
//    };
//
//    int executionTimes[12] = {
//        3, 3, 3, 4, 2, 4,
//        4, 3, 3, 5, 4, 4
//    };
//
//    int hard_or_weakly_hard[12] = {
//        1, 0, 1, 0, 1, 0,
//        0, 1, 1, 1, 1, 1
//    };
//
//    int m_k_firm[12][2] = {
//        {1,1},
//        {3,4},
//        {1,1},
//        {4,6},
//        {1,1},
//        {2,5},
//        {5,7},
//        {1,1},
//        {1,1},
//        {1,1},
//        {1,1},
//        {1,1}
//    };

  // Input Set
//    int rand_time = 18;
//
//    int arrival[18] = {
//        71, 263, 548, 889, 1327,
//        1941, 2718, 3642, 4821,
//        6284, 8013, 10097, 12539,
//        15381, 18764, 22349,
//        26471, 29782
//    };
//
//    int periods[18] = {
//        91, 118, 145, 103, 129,
//        87, 136, 110, 149,
//        95, 122, 140, 89,
//        127, 144, 104,
//        133, 97
//    };
//
//    int executionTimes[18] = {
//        2, 3, 4, 2, 3,
//        4, 2, 3, 4,
//        2, 3, 2, 4,
//        3, 2, 4,
//        3, 2
//    };
//
//    // 10 ones (~56%), 8 zeros
//    int hard_or_weakly_hard[18] = {
//        1,0,1,1,0,
//        1,0,1,1,
//        0,1,1,0,
//        1,0,1,
//        0,1
//    };
//
//    int m_k_firm[18][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {4,7},
//        {1,1},
//        {1,1},
//
//        {2,5},
//        {1,1},
//        {1,1},
//        {3,6},
//
//        {1,1},
//        {5,8},
//        {1,1},
//
//        {2,3},
//        {1,1}
//    };
//
//    // Input Set
//    int rand_time = 19;
//
//    int arrival[19] = {
//        84, 271, 493, 716, 1039,
//        1498, 1834, 2417, 3162, 4089,
//        5293, 6731, 8456, 10328, 12691,
//        15487, 18943, 23376, 28729
//    };
//
//    int periods[19] = {
//        92, 118, 146, 101, 129,
//        87, 135, 110, 148, 95,
//        122, 139, 89, 127, 143,
//        104, 133, 97, 150
//    };
//
//    int executionTimes[19] = {
//        2, 3, 4, 2, 3,
//        4, 2, 3, 4, 2,
//        3, 2, 4, 3, 2,
//        4, 3, 2, 4
//    };
//
//    // ~58% ones (11 ones, 8 zeros)
//    int hard_or_weakly_hard[19] = {
//        1,0,1,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1
//    };
//
//    int m_k_firm[19][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {4,7},
//        {1,1},
//        {1,1},
//        {2,5},
//
//        {1,1},
//        {1,1},
//        {3,6},
//        {1,1},
//        {4,8},
//
//        {1,1},
//        {2,3},
//        {1,1},
//        {1,1}
//    };


//    // Input set 4
//    int rand_time = 25;
//
//    int arrival[25] = {
//        91, 237, 418, 689, 973,
//        1264, 1589, 2013, 2471, 2894,
//        3417, 3962, 4518, 5291, 6137,
//        7084, 8149, 9323, 10641, 12178,
//        14527, 17136, 20459, 23891, 28743
//    };
//
//    int periods[25] = {
//        87, 92, 145, 113, 126,
//        95, 134, 102, 148, 89,
//        119, 141, 97, 128, 136,
//        108, 150, 111, 124, 99,
//        138, 116, 144, 105, 131
//    };
//
//    int executionTimes[25] = {
//        2, 3, 4, 2, 3,
//        2, 4, 3, 2, 4,
//        3, 2, 4, 3, 2,
//        4, 3, 2, 4, 3,
//        2, 4, 3, 2, 4
//    };
//
//    // ~60% ones (15 ones, 10 zeros)
//    int hard_or_weakly_hard[25] = {
//        1,0,1,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1,1,0,1,1
//    };
//
//    int m_k_firm[25][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {4,6},
//        {1,1},
//        {1,1},
//        {5,8},
//
//        {1,1},
//        {1,1},
//        {2,6},
//        {1,1},
//        {4,7},
//
//        {1,1},
//        {3,6},
//        {1,1},
//        {1,1},
//        {2,5},
//
//        {1,1},
//        {1,1},
//        {5,7},
//        {1,1},
//        {1,1}
//    };
//
  // Input Set
//    int rand_time = 27;
//
//    int arrival[27] = {
//        82, 214, 451, 763, 1159,
//        1628, 2213, 2941, 3827, 4863,
//        6117, 7598, 9316, 11324, 13679,
//        16291, 19083, 21746, 24018, 25793,
//        27169, 28213, 28974, 29467, 29741,
//        29883, 29937
//    };
//
//    int periods[27] = {
//        92, 118, 145, 101, 129,
//        88, 136, 111, 148, 95,
//        123, 140, 90, 127, 144,
//        104, 132, 98, 150, 113,
//        139, 94, 125, 147, 107,
//        131, 96
//    };
//
//    int executionTimes[27] = {
//        2,3,4,2,3,
//        4,2,3,4,2,
//        3,2,4,3,2,
//        4,3,2,4,3,
//        2,4,3,2,4,
//        3,2
//    };
//
//    // 16 ones (~59%), 11 zeros
//    int hard_or_weakly_hard[27] = {
//        1,0,1,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0
//    };
//
//    int m_k_firm[27][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {4,7},
//        {1,1},
//        {1,1},
//        {2,5},
//
//        {1,1},
//        {1,1},
//        {3,6},
//        {1,1},
//        {5,8},
//
//        {1,1},
//        {2,3},
//        {1,1},
//        {1,1},
//        {4,6},
//
//        {1,1},
//        {1,1},
//        {3,7},
//        {1,1},
//        {5,9},
//
//        {1,1},
//        {2,5}
//    };

  // Input Set
//    int rand_time = 31;
//
//    int arrival[31] = {
//        63, 184, 391, 577, 891,
//        1238, 1584, 2039, 2576, 3197,
//        3874, 4628, 5481, 6417, 7523,
//        8769, 10041, 11487, 13162, 14931,
//        16804, 18792, 20936, 22891, 24137,
//        25384, 26489, 27416, 28291, 29137,
//        29841
//    };
//
//    int periods[31] = {
//        91, 117, 145, 102, 128,
//        89, 136, 111, 149, 95,
//        121, 140, 88, 127, 144,
//        104, 132, 98, 150, 113,
//        139, 93, 125, 147, 106,
//        134, 90, 118, 142, 100,
//        130
//    };
//
//    int executionTimes[31] = {
//        2,3,4,2,3,
//        4,2,3,4,2,
//        3,2,4,3,2,
//        4,3,2,4,3,
//        2,4,3,2,4,
//        2,3,4,2,3,
//        4
//    };
//
//    // 18 ones (~58%), 13 zeros
//    int hard_or_weakly_hard[31] = {
//        1,0,1,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1
//    };
//
//    int m_k_firm[31][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {4,7},
//        {1,1},
//        {1,1},
//        {2,5},
//
//        {1,1},
//        {1,1},
//        {3,6},
//        {1,1},
//        {4,8},
//
//        {1,1},
//        {5,7},
//        {1,1},
//        {1,1},
//        {2,3},
//
//        {1,1},
//        {1,1},
//        {4,6},
//        {1,1},
//        {3,7},
//
//        {1,1},
//        {5,8},
//        {1,1},
//        {1,1},
//        {2,5},
//
//        {1,1}
//    };


  // Input Set
//    int rand_time = 40;
//
//    int arrival[40] = {
//        73, 164, 291, 448, 612,
//        799, 931, 1147, 1362, 1588,
//        1849, 2117, 2433, 2784, 3198,
//        3571, 3964, 4378, 4892, 5437,
//        6071, 6793, 7541, 8398, 9247,
//        10184, 11263, 12379, 13611, 14938,
//        16427, 17986, 19652, 21417, 23293,
//        25184, 26983, 28179, 29161, 29874
//    };
//
//    int periods[40] = {
//        91, 104, 126, 88, 145,
//        97, 111, 138, 95, 117,
//        143, 108, 132, 89, 121,
//        150, 100, 115, 141, 106,
//        134, 94, 128, 112, 146,
//        98, 124, 136, 90, 119,
//        147, 102, 130, 109, 139,
//        86, 122, 144, 96, 114
//    };
//
//    int executionTimes[40] = {
//        2,3,4,2,3,
//        4,2,3,4,2,
//        3,2,4,3,2,
//        4,3,2,4,3,
//        2,4,3,2,4,
//        2,3,4,2,3,
//        4,2,3,2,4,
//        3,2,4,3,2
//    };
//
//    // 24 ones (60%), 16 zeros (40%)
//    int hard_or_weakly_hard[40] = {
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,1,0,1,0,
//        1,1,0,1,1,
//        0,1,0,1,1,
//        0,1,1,0,1,
//        0,1,1,0,1,
//        0,1,1,0,1
//    };
//
//    int m_k_firm[40][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {1,1},
//        {4,7},
//        {1,1},
//        {3,4},
//
//        {1,1},
//        {1,1},
//        {2,5},
//        {1,1},
//        {4,6},
//
//        {1,1},
//        {1,1},
//        {3,7},
//        {1,1},
//        {1,1},
//
//        {2,3},
//        {1,1},
//        {5,8},
//        {1,1},
//        {1,1},
//
//        {4,5},
//        {1,1},
//        {1,1},
//        {2,6},
//        {1,1},
//
//        {3,5},
//        {1,1},
//        {1,1},
//        {4,8},
//        {1,1},
//
//        {3,4},
//        {1,1},
//        {1,1},
//        {5,9},
//        {1,1}
//    };


  // Input Set
//    int rand_time = 20;
//
//    int arrival[20] = {
//        67, 213, 389, 742, 1186,
//        1693, 2247, 3011, 3928, 4876,
//        6159, 7624, 9341, 11287, 13652,
//        16491, 19837, 23164, 26719, 29683
//    };
//
//    int periods[20] = {
//        89, 116, 143, 97, 128,
//        105, 150, 111, 136, 92,
//        124, 147, 101, 132, 118,
//        95, 140, 108, 126, 145
//    };
//
//    int executionTimes[20] = {
//        2, 3, 4, 2, 3,
//        4, 2, 3, 4, 2,
//        3, 2, 4, 3, 2,
//        4, 3, 2, 4, 3
//    };
//
//    // 12 ones = 60%, 8 zeros = 40%
//    int hard_or_weakly_hard[20] = {
//        1,0,1,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0
//    };
//
//    int m_k_firm[20][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {4,6},
//        {1,1},
//        {1,1},
//        {2,5},
//
//        {1,1},
//        {1,1},
//        {3,6},
//        {1,1},
//        {4,7},
//
//        {1,1},
//        {2,3},
//        {1,1},
//        {1,1},
//        {5,8}
//    };

  // Input Set
//    int rand_time = 43;
//
//    int arrival[43] = {
//        61, 173, 384, 612, 887,
//        1204, 1568, 2019, 2487, 3071,
//        3796, 4512, 5329, 6248, 7311,
//        8479, 9782, 11164, 12783, 14539,
//        16392, 18264, 20193, 21987, 23216,
//        24398, 25489, 26347, 27186, 27991,
//        28642, 29083, 29357, 29506, 29618,
//        29697, 29763, 29811, 29846, 29873,
//        29891, 29921, 29947
//    };
//
//    int periods[43] = {
//        89, 117, 142, 103, 129,
//        94, 138, 112, 146, 97,
//        124, 140, 88, 132, 149,
//        101, 126, 144, 92, 136,
//        109, 148, 95, 120, 141,
//        87, 134, 106, 150, 114,
//        127, 145, 91, 137, 104,
//        143, 99, 122, 139, 90,
//        130, 118, 147
//    };
//
//    int executionTimes[43] = {
//        2,3,4,2,3,
//        4,2,3,4,2,
//        3,2,4,3,2,
//        4,3,2,4,3,
//        2,4,3,2,4,
//        2,3,4,2,3,
//        4,2,3,2,4,
//        3,2,4,3,2,
//        4,3,2
//    };
//
//    // 25 ones (~58%), 18 zeros
//    int hard_or_weakly_hard[43] = {
//        1,0,1,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1,1,0
//    };
//
//    int m_k_firm[43][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {4,7},
//        {1,1},
//        {1,1},
//        {2,5},
//
//        {1,1},
//        {1,1},
//        {3,6},
//        {1,1},
//        {5,8},
//
//        {1,1},
//        {2,3},
//        {1,1},
//        {1,1},
//        {4,6},
//
//        {1,1},
//        {1,1},
//        {3,7},
//        {1,1},
//        {5,9},
//
//        {1,1},
//        {2,6},
//        {1,1},
//        {1,1},
//        {4,5},
//
//        {1,1},
//        {1,1},
//        {3,4},
//        {1,1},
//        {2,7},
//
//        {1,1},
//        {4,8},
//        {1,1},
//        {1,1},
//        {5,6},
//
//        {1,1},
//        {1,1},
//        {3,5}
//    };

  // Input Set
//    int rand_time = 26;
//
//    int arrival[26] = {
//        79, 241, 463, 728, 1091,
//        1467, 1934, 2519, 3186, 3973,
//        4891, 5964, 7247, 8719, 10384,
//        12291, 14563, 16947, 19386, 21873,
//        24196, 25841, 27389, 28617, 29431,
//        29923
//    };
//
//    int periods[26] = {
//        91, 118, 146, 103, 129,
//        87, 135, 110, 148, 95,
//        122, 139, 89, 127, 143,
//        104, 133, 97, 150, 112,
//        136, 94, 125, 141, 108,
//        130
//    };
//
//    int executionTimes[26] = {
//        2, 3, 4, 2, 3,
//        4, 2, 3, 4, 2,
//        3, 2, 4, 3, 2,
//        4, 3, 2, 4, 3,
//        2, 4, 3, 2, 4,
//        3
//    };
//
//    // 15 ones (~58%), 11 zeros
//    int hard_or_weakly_hard[26] = {
//        1,0,1,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1
//    };
//
//    int m_k_firm[26][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {4,7},
//        {1,1},
//        {1,1},
//        {2,5},
//
//        {1,1},
//        {1,1},
//        {3,6},
//        {1,1},
//        {5,8},
//
//        {1,1},
//        {2,3},
//        {1,1},
//        {1,1},
//        {4,6},
//
//        {1,1},
//        {1,1},
//        {3,7},
//        {1,1},
//        {5,9},
//
//        {1,1}
//    };

  // Input Set
//    int rand_time = 15;
//
//    int arrival[15] = {
//        96, 284, 571, 913, 1426,
//        2017, 2794, 3812, 5039, 6541,
//        8367, 10614, 13582, 18739, 29481
//    };
//
//    int periods[15] = {
//        93, 121, 147, 104, 132,
//        88, 139, 110, 145, 97,
//        126, 141, 91, 136, 118
//    };
//
//    int executionTimes[15] = {
//        2, 3, 4, 2, 3,
//        4, 2, 3, 4, 2,
//        3, 2, 4, 3, 2
//    };
//
//    // 9 ones (~60%), 6 zeros
//    int hard_or_weakly_hard[15] = {
//        1,0,1,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0
//    };
//
//    int m_k_firm[15][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {4,7},
//        {1,1},
//        {1,1},
//        {2,5},
//
//        {1,1},
//        {1,1},
//        {3,6},
//        {1,1},
//        {5,8}
//    };

  // Input Set
//    int rand_time = 21;
//
//    int arrival[21] = {
//        88, 247, 516, 794, 1198,
//        1683, 2297, 3076, 4012, 5129,
//        6481, 8017, 9834, 11879, 14291,
//        17038, 20147, 23518, 26439, 28614,
//        29872
//    };
//
//    int periods[21] = {
//        92, 119, 146, 101, 128,
//        87, 134, 110, 149, 96,
//        123, 141, 89, 126, 144,
//        105, 132, 98, 150, 114,
//        137
//    };
//
//    int executionTimes[21] = {
//        2, 3, 4, 2, 3,
//        4, 2, 3, 4, 2,
//        3, 2, 4, 3, 2,
//        4, 3, 2, 4, 3,
//        2
//    };
//
//    // 12 ones (~57%), 9 zeros
//    int hard_or_weakly_hard[21] = {
//        1,0,1,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1
//    };
//
//    int m_k_firm[21][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {4,7},
//        {1,1},
//        {1,1},
//        {2,5},
//
//        {1,1},
//        {1,1},
//        {3,6},
//        {1,1},
//        {5,8},
//
//        {1,1},
//        {2,3},
//        {1,1},
//        {1,1},
//        {4,6},
//
//        {1,1}
//    };

  // Input Set
//    int rand_time = 24;
//
//    int arrival[24] = {
//        83, 219, 476, 751, 1129,
//        1574, 2138, 2863, 3721, 4694,
//        5847, 7196, 8793, 10641, 12876,
//        15329, 18147, 21183, 23964, 26291,
//        27938, 28976, 29547, 29918
//    };
//
//    int periods[24] = {
//        90, 117, 146, 103, 129,
//        87, 135, 111, 148, 96,
//        122, 140, 89, 127, 144,
//        105, 133, 98, 150, 114,
//        138, 93, 125, 141
//    };
//
//    int executionTimes[24] = {
//        2, 3, 4, 2, 3,
//        4, 2, 3, 4, 2,
//        3, 2, 4, 3, 2,
//        4, 3, 2, 4, 3,
//        2, 4, 3, 2
//    };
//
//    // 14 ones (~58%), 10 zeros
//    int hard_or_weakly_hard[24] = {
//        1,0,1,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1,1,0,1
//    };
//
//    int m_k_firm[24][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {4,7},
//        {1,1},
//        {1,1},
//        {2,5},
//
//        {1,1},
//        {1,1},
//        {3,6},
//        {1,1},
//        {5,8},
//
//        {1,1},
//        {2,3},
//        {1,1},
//        {1,1},
//        {4,6},
//
//        {1,1},
//        {1,1},
//        {3,7},
//        {1,1}
//    };

  // Input Set
//    int rand_time = 34;
//
//    int arrival[34] = {
//        74, 193, 417, 701, 1048,
//        1493, 2037, 2694, 3468, 4391,
//        5517, 6839, 8392, 10173, 12249,
//        14691, 17352, 19984, 22281, 24237,
//        25794, 27041, 27973, 28639, 29124,
//        29473, 29658, 29759, 29812, 29856,
//        29893, 29916, 29934, 29949
//    };
//
//    int periods[34] = {
//        91, 118, 145, 102, 129,
//        87, 136, 110, 149, 95,
//        122, 140, 89, 127, 144,
//        104, 133, 98, 150, 114,
//        138, 93, 125, 147, 108,
//        130, 96, 142, 100, 134,
//        90, 121, 146, 112
//    };
//
//    int executionTimes[34] = {
//        2,3,4,2,3,
//        4,2,3,4,2,
//        3,2,4,3,2,
//        4,3,4,4,3,
//        2,4,3,3,4,
//        2,4,4,2,3,
//        4,2,4,2
//    };
//
//    // 20 ones (~59%), 14 zeros
//    int hard_or_weakly_hard[34] = {
//        1,0,1,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1,1,0,1
//    };
//
//    int m_k_firm[34][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {4,7},
//        {1,1},
//        {1,1},
//        {2,5},
//
//        {1,1},
//        {1,1},
//        {3,6},
//        {1,1},
//        {5,8},
//
//        {1,1},
//        {2,3},
//        {1,1},
//        {1,1},
//        {4,6},
//
//        {1,1},
//        {1,1},
//        {3,7},
//        {1,1},
//        {5,9},
//
//        {1,1},
//        {2,6},
//        {1,1},
//        {1,1},
//        {4,5},
//
//        {1,1},
//        {1,1},
//        {3,4},
//        {1,1}
//    };
  // Input Set
//    int rand_time = 35;
//
//    int arrival[35] = {
//        68, 194, 421, 733, 1096,
//        1527, 2091, 2748, 3514, 4389,
//        5421, 6618, 8029, 9584, 11376,
//        13348, 15491, 17763, 20158, 22491,
//        24418, 25984, 27163, 28041, 28684,
//        29123, 29409, 29581, 29694, 29759,
//        29804, 29839, 29872, 29906, 29941
//    };
//
//    int periods[35] = {
//        89, 117, 145, 102, 129,
//        87, 136, 110, 149, 95,
//        122, 140, 91, 127, 144,
//        104, 133, 98, 150, 114,
//        138, 93, 125, 147, 108,
//        130, 96, 142, 100, 134,
//        90, 121, 146, 112, 139
//    };
//
//    int executionTimes[35] = {
//        2,3,4,2,3,
//        4,2,3,4,2,
//        3,2,4,3,2,
//        4,3,2,4,3,
//        2,4,3,2,4,
//        2,3,4,2,3,
//        4,2,3,2,4
//    };
//
//    // 20 ones (~57%), 15 zeros
//    int hard_or_weakly_hard[35] = {
//        1,0,1,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0,
//        1,0,1,1,0,
//        1,1,0,1,0
//    };
//
//    int m_k_firm[35][2] = {
//        {1,1},
//        {2,4},
//        {1,1},
//        {1,1},
//        {3,5},
//
//        {1,1},
//        {4,7},
//        {1,1},
//        {1,1},
//        {2,5},
//
//        {1,1},
//        {1,1},
//        {3,6},
//        {1,1},
//        {5,8},
//
//        {1,1},
//        {2,3},
//        {1,1},
//        {1,1},
//        {4,6},
//
//        {1,1},
//        {1,1},
//        {3,7},
//        {1,1},
//        {5,9},
//
//        {1,1},
//        {2,6},
//        {1,1},
//        {1,1},
//        {4,5},
//
//        {1,1},
//        {1,1},
//        {3,4},
//        {1,1},
//        {2,7}
//    };

  // Entering random tasks
  for (int i=0;i<rand_time;i++){
      task_set[i+n].id = i+n+1;
      task_set[i+n].period = periods[i];
      // task_set[i+n].abs_deadline = periods[i]+arrival[i];
      task_set[i+n].abs_deadline = arrival[i];
      task_set[i+n].exec_time = executionTimes[i];
      task_set[i+n].remaining = executionTimes[i];

      // Obtaining weaky hard constraint for task
//        obtain_weakly_hard_m_k(hard_or_weakly_hard[i],m_k_firm[i]);
//        printf("m : %d, k : %d\n", m_k_firm[i][0],m_k_firm[i][1]);
  }

  // printf("\n Arrival :  \t");
  // for(int i = 0; i<rand_time;i++){
  //     printf("%d ",arrival[i]);
  // }
  //     printf("\n periods :  \t");
  // for(int i = 0; i<rand_time;i++){
  //     printf("%d ",periods[i]);
  // }
  //     printf("\n ExcTime :  \t");
  // for(int i = 0; i<rand_time;i++){
  //     printf("%d ",executionTimes[i]);
  // }


  // if (max_util <=1){
  // 	schedule_edf = 1;
  // }else{
  // 	schedule_edf = 0;
  // }


//    printf("Max Util : %f",max_util);
//    for(int j = 0; j<n; j++){
//    	printf("%d : %d, %d, %d, %d\n",task_set[j].id, task_set[j].period, task_set[j].exec_time, task_set[j].remaining, task_set[j].abs_deadline);
//    }


  /**********   All declaration and necessary initialization and computation */

//    HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET); // LED ON
   GPIOB_ODR |= (1 << 0); // LED ON

  /* --- Simulation Engine --- */

  for (int t = 0; t <= SIM_TICKS; t++) {
           uint32_t start = DWT_CYCCNT;
      // printf("hello2, t : %d\n",t);
          // =========================================================================== //
          /****************************   SCHEDULING LOGIC ****************************** */
          // =========================================================================== //
          schedulability_flag = 0;
          flag_srsp_and_edf_list = 0;
          flag_only_edf_list = 0;
          job_selected=0;
          best=100;
      //    printf("\nTime: %d\n",t);


          // *************************** Job Removal  ***********************************
			if (t!=0 && size_EDF_list!=0 && only_EDF_list[0].abs_deadline==t){
				while(1){
					 if(t == only_EDF_list[0].abs_deadline){
						 int i = only_EDF_list[0].id-1;
                      //  printf("hello2, i : %d\n",i);
                      job = only_EDF_list[0];
                      // if(t == 450 && i == 0){
                      //         printf("\n 1st removal (before) :  deadline = %d , period = %d, lst = %d \n",job.abs_deadline, job.period, lst[i]);
                      // }
                      if (i<n){
                              lst[i] = (job.abs_deadline / job.period);
                              tot[i] = tot[i] + 1;
                              hit_or_miss[i]=0;
//                                hit_miss_matrix[i][lst[i]-1] = 0;
                      }else{
                          // printf("hello2, i : %d\n",i);
                             zero[i-n]+=1;
                      }
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
//                            if(schedule_edf==0){
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
                              }else if(srsp_prio[i] == 1){
                                      binary_search_add_element(srsp_and_EDF_list, size_SRSP_EDF_list, task_set[i]);
                                      size_SRSP_EDF_list++;
                              }

			    // when total util <=1, schedule liken EDF
//                            }else{
//                            	binary_search_add_element(srsp_and_EDF_list, size_SRSP_EDF_list, task_set[i]);
//                            	size_SRSP_EDF_list++;
//                            }
//                            printf("\n At arrival \n\n");
//                            printf("srsp_EDF_List\t");
//
//                            for(int k = 0; k<size_SRSP_EDF_list; k++){
//                            	printf("%d : %d, %d, %d, %d\n",srsp_and_EDF_list[k].id,srsp_and_EDF_list[k].period, srsp_and_EDF_list[k].exec_time, srsp_and_EDF_list[k].remaining, srsp_and_EDF_list[k].abs_deadline);
//                            }
                  }
          }

          // Sudden task arrival
          for (int i=0;i<rand_time;i++){
              if(t >= arrival[i]){
                  // printf(" i : %d, t : %d, arrival : %d, task_selected : %d\n", i+n, t, arrival[i],task_selected[i+n]);
                  // printf("\nt-arrival : %d, period : %d, mod : %d\n",t-arrival[i], task_set[i+n].period, ((t-arrival[i]) % task_set[i+n].period));
                  if((t == arrival[i]) || ((t-arrival[i]) % task_set[i+n].period == 0)){

                      // Call admission control
                      if(t+task_set[i+n].period > SIM_TICKS){
                          task_set[i+n].abs_deadline = task_set[i+n].abs_deadline + task_set[i+n].period; //adjust next period
                          task_set[i+n].remaining = task_set[i+n].exec_time;
                          continue;
                      }

                      if (task_selected[i+n]==0){
                          show = min_util + ((float)m_k_firm[i][0]/(float)m_k_firm[i][1]) * ((float)task_set[i+n].exec_time/(float)task_set[i+n].period);
                          // printf("show : %f\n",show);
                          //overload
                          if (show > 1){
                              task_set[i+n].abs_deadline = task_set[i+n].abs_deadline + task_set[i+n].period; //adjust next period
                              task_set[i+n].remaining = task_set[i+n].exec_time;
                              continue;
                          }else{

                              task_selected[i+n]=1;
                              task_accepted += 1;
                              min_util = min_util + ((float)m_k_firm[i][0]/(float)m_k_firm[i][1]) * ((float)task_set[i+n].exec_time/(float)task_set[i+n].period);
                              max_util = max_util + ((float)task_set[i+n].exec_time/(float)task_set[i+n].period);
                          }
                      }

                      //After successful admission
                      task_set[i+n].abs_deadline = task_set[i+n].abs_deadline  + task_set[i+n].period;
                      task_set[i+n].remaining = task_set[i+n].exec_time;
                      index[i] += 1;
                      // printf("i : %d, t : %d, arrival : %d, task_selected : %d, wcet = %d\n", i+n, t, arrival[i],task_selected[i+n], task_set[i+n].remaining);

                      if(t != SIM_TICKS){
                          total_jobs += 1;  // new job released
                      }


                      if (hard_or_weakly_hard[i]==1){
                          srsp_prio[i+n] = 1;
                      }else if (hard_or_weakly_hard[i]==0){
                          srsp_prio[i+n] = weakly_hard_requirement_check(m_k_firm[i], zero[i], index[i]);
                      }
                      // printf("srsp_prio : %d\n", srsp_prio[i+n]);

                      if(srsp_prio[i+n] == 0){
                              //  printf("id : %d, hello edf\n",i+n);
                              binary_search_add_element(only_EDF_list, size_EDF_list, task_set[i+n]);
                              size_EDF_list++;
                              // printf("\n only_EDF_list: \t");
                              // printf("\ntask1 prio : %d, lst : %d, tot : %d, hit_or_miss : %d\n", srsp_prio[0], lst[0], tot[0], hit_or_miss[0]);
                              // for(int k = 0; k<size_EDF_list; k++){
                              //     printf("%d ,%d , %d,  %d\n",only_EDF_list[k].id,only_EDF_list[k].period,only_EDF_list[k].abs_deadline, only_EDF_list[k].remaining);
                              // }
                      }else if(srsp_prio[i+n] == 1){
                              // printf("id : %d, hello srsp\n",i+n);
                              binary_search_add_element(srsp_and_EDF_list, size_SRSP_EDF_list, task_set[i+n]);
                              size_SRSP_EDF_list++;
                      }
                  }
              }
          }



      //    printf("\n After job arrival\n\n");
      //    printf("srsp_EDF_List\t");

      //    for(int k = 0; k<size_SRSP_EDF_list; k++){
      //    	printf("%d : %d, %d, %d, %d\n",srsp_and_EDF_list[k].id,srsp_and_EDF_list[k].period, srsp_and_EDF_list[k].exec_time, srsp_and_EDF_list[k].remaining, srsp_and_EDF_list[k].abs_deadline);
      //    }

      //    printf("\n only_EDF_list: \t");

      //    for(int k = 0; k<size_EDF_list; k++){
      //    	printf("%d : %d, %d, %d, %d\n",only_EDF_list[k].id,only_EDF_list[k].period, only_EDF_list[k].exec_time, only_EDF_list[k].remaining,only_EDF_list[k].abs_deadline);
      //    }

			// printf("Task_set\t");

			// for(int k = 0; k<n; k++){
			// 	printf("%d : %d, %d, %d, %d\n",task_set[k].id,task_set[k].period, task_set[k].exec_time, task_set[k].remaining, task_set[k].abs_deadline);
			// }

          // *************************** Job Selection  ***********************************
          while (size_SRSP_EDF_list != 0 || size_EDF_list != 0){
              // printf("\n hello : while loop on\n");
          	job_selected = 0;
          	flag_srsp_and_edf_list = 0;
          	flag_only_edf_list = 0;
              schedulability_flag = 0;

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

                              if (i<n){
                                      lst[i] = (job.abs_deadline / job.period);
                                      tot[i] = tot[i] + 1;
                                      hit_or_miss[i]=0;
//                                        hit_miss_matrix[i][lst[i]-1] = 0;
                              }else{
                                  zero[i-n]+=1;
                              }

                              // if(t == 450 && i == 0){
                              //         printf("\n 2nd removal :  deadline = %d , period = %d, lst = %d \n",job.abs_deadline, job.period, lst[i]);
                              // }

                              remove_first_element(only_EDF_list, size_EDF_list);
                              size_EDF_list--;
                              flag_only_edf_list = 0;
                              continue;
                      }else{
                          job_selected = 1;
                          break;
                      }
              }

              else if(flag_srsp_and_edf_list == 1){
                      if(t+job.remaining > job.abs_deadline){
                          int i = job.id-1;
                          schedulability_flag = 1;
                          flag_srsp_and_edf_list = 0;
                          break;
                      }
                      else{
                          job_selected = 1;
                          break;
                      }
              }
          }

          if(schedulability_flag == 1){
              break;
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
                      // hit_miss_matrix[best][(job.abs_deadline/job.period)-1] = 1;
                      if(best<n){
//                            hit_miss_matrix[best][(job.abs_deadline/job.period)-1] = 1;
                      }
                      // if (best<n){
                      //     hit_or_miss[best]=1;
                      //     hit_miss_matrix[best][(job.abs_deadline/job.period)-1] = 1;
                      // }
                      if(t != SIM_TICKS){
                      	jobs_scheduled += 1;
						}
                      flag_only_edf_list = 0;
                      flag_srsp_and_edf_list = 0;
              }
          }
          else{
              flag_only_edf_list = 0;
              flag_srsp_and_edf_list = 0;
              continue;
          }
//
      //    if (t<=200){
				// printf("\n At the End \n\n");
				// printf("srsp_EDF_List\t");

				// for(int k = 0; k<size_SRSP_EDF_list; k++){
				// 	printf("%d, %d,  %d, %d\n",srsp_and_EDF_list[k].id,srsp_and_EDF_list[k].period, srsp_and_EDF_list[k].abs_deadline,srsp_and_EDF_list[k].remaining );
				// }

				// printf("\n only_EDF_list: \t");
              // // printf("\ntask1 prio : %d, lst : %d, tot : %d, hit_or_miss : %d\n", srsp_prio[0], lst[0], tot[0], hit_or_miss[0]);

				// for(int k = 0; k<size_EDF_list; k++){
				// 	printf("%d ,%d , %d,  %d\n",only_EDF_list[k].id,only_EDF_list[k].period,only_EDF_list[k].abs_deadline, only_EDF_list[k].remaining);
				// }
          // }

          /********************************************************************************/
          /********************************************************************************/

           uint32_t end = DWT_CYCCNT; //for overhead calculation

          // Store performance data
           overhead_archive[t] = end - start;
           task_history[t] = (uint8_t)best;
  }
//    HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET); // LED OFF
   GPIOB_ODR &= ~(1 << 0); // LED OFF


  if(schedulability_flag == 1){
//        printf("\nTasks are not schedulable ! \n");
  }
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
//    printf("\n\n jobs_scheduled, total_jobs : (%d, %d)", jobs_scheduled, total_jobs);
//    printf("\n units consumed, total bandwidth : (%d, %d)\n", proc_util,SIM_TICKS);
//    printf("\n\n Schedulability Ratio: %f", sch_ratio);
//    printf("\n Actual Utilization : %f \n", util);
//    printf("--- ALL DATA REPORTED ---\r\n");

//    printf("\n\n jobs_scheduled, total_jobs : (%d, %d)", jobs_scheduled, total_jobs);
//    printf("\n units consumed, total bandwidth : (%d, %d)\n", proc_util,SIM_TICKS);
//    for(int i = 0; i<n; i++){
//        printf("\n\n Task %d : \t",i+1);
//        int x = SIM_TICKS / task_set[i].period;
//        for(int j = 0; j<x;j++){
//                printf("%d",hit_miss_matrix[i][j]);
//        }
//    }
//    printf("\nDynamic tasks : %d\n",rand_time);
//    printf("\n Task Accepted : %d\n", task_accepted);
//    printf("\n Total util demand: %f\n",max_util);
}
