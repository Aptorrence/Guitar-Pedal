#include "stm32h7xx_hal.h"
#include "stm32h7xx_it.h"

extern DMA_HandleTypeDef hdma_sai1_a;
extern DMA_HandleTypeDef hdma_sai1_b;
extern DMA_HandleTypeDef hdma_adc1;

/* Register-level SysTick timebase (common/driver/platform/stm32h7/st_systick.cc). */
extern void st_systick_inc(void);

void NMI_Handler(void)
{
    while (1)
    {
    }
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
    st_systick_inc();
}

void DMA1_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_sai1_a);
}

void DMA1_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_sai1_b);
}

void DMA1_Stream2_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc1);
}
