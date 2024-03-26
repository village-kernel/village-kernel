//###########################################################################
// System.h
// A mostly static class containing system wide functions.
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "System.h"
#include "HalHeaders.h"


/// @brief Constructor
ConcreteSystem::ConcreteSystem()
	:sysTicks(0),
	cyclesInMicro(0)
{}


/// @brief Destructor
ConcreteSystem::~ConcreteSystem()
{
}


/// @brief System Setup
void ConcreteSystem::Setup()
{
	//Config clock
	ConfigCoreDebug();
	ConfigSysTick();
	ConfigureForXtal();
}


/// @brief System Exit
void ConcreteSystem::Exit()
{

}


/// @brief Configure the main internal regulator output voltage
void ConcreteSystem::ConfigurePower()
{
	//Select regulator voltage output Scale 1 mode
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;
}


/// @brief Configure core debug
void ConcreteSystem::ConfigCoreDebug()
{
	//Enable Data Watchpoint and Trace unit and turn on its main counter
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}


/// @brief Config system tick
void ConcreteSystem::ConfigSysTick()
{
	//Select Systick clock source: SYSCLK_MHZ/8 = 168/8 = 21M
	SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;

	//Enable systick interrupt
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

	//Set systick reload value
	SysTick->LOAD = 21000;

	//Clear systick current value
	SysTick->VAL = 0;

	//Enable systick
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	//Calculate the cyclesInMicro variable
	cyclesInMicro = SystemCoreClock / microsInSec;
}


/// @brief Configures the system to use an hsi crystal as clock source.
/// @brief HSI：16Mhz, PLL: 168Mhz, System Clock = PLL1, PCLK1 = 42Mhz, PCLK2=84Mhz.
void ConcreteSystem::ConfigureForHsi()
{
	const uint32_t pll_m = 8;
	const uint32_t pll_n = 168;
	const uint32_t pll_p = 2;
	const uint32_t pll_q = 7;

	//Enable HSI
	RCC->CR |= ((uint32_t)RCC_CR_HSION);
	while ((RCC->CR & RCC_CR_HSIRDY_Msk) != RCC_CR_HSIRDY);

	//Configure the main PLL
	RCC->PLLCFGR = (RCC_PLLCFGR_PLLSRC_HSI) |
					(pll_m << RCC_PLLCFGR_PLLM_Pos) |
					(pll_n << RCC_PLLCFGR_PLLN_Pos) |
					(((pll_p >> 1u) - 1u) << RCC_PLLCFGR_PLLP_Pos) |
					(pll_q << RCC_PLLCFGR_PLLQ_Pos);

	//Enable the main PLL
	RCC->CR |= RCC_CR_PLLON;

	//Wait till the main PLL is ready
	while((RCC->CR & RCC_CR_PLLRDY) == 0) {}

	//Configure Flash prefetch, Instruction cache, Data cache and wait state
	FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

	//HCLK = SYSCLK / 1
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	//PCLK1 = HCLK / 4
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

	//PCLK2 = HCLK / 2
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

	//Select the main PLL as system clock source
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	//Wait till the main PLL is used as system clock source
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

	//Update the SystemCoreClock variable
	SystemCoreClockUpdate();

	//Update the cyclesInMicro variable
	cyclesInMicro = SystemCoreClock / microsInSec;
}


/// @brief Configures the system to use an hse crystal as clock source.
/// @brief XTAL：8Mhz, PLL: 168Mhz, System Clock = PLL1, PCLK1 = 42Mhz, PCLK2=84Mhz.
void ConcreteSystem::ConfigureForXtal()
{
	const uint32_t pll_m = 4;
	const uint32_t pll_n = 168;
	const uint32_t pll_p = 2;
	const uint32_t pll_q = 7;

	//Enable HSE
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);
	while ((RCC->CR & RCC_CR_HSERDY_Msk) != RCC_CR_HSERDY);

	//Configure the main PLL
	RCC->PLLCFGR = (RCC_PLLCFGR_PLLSRC_HSE) |
					(pll_m << RCC_PLLCFGR_PLLM_Pos) |
					(pll_n << RCC_PLLCFGR_PLLN_Pos) |
					(((pll_p >> 1u) - 1u) << RCC_PLLCFGR_PLLP_Pos) |
					(pll_q << RCC_PLLCFGR_PLLQ_Pos);

	//Enable the main PLL
	RCC->CR |= RCC_CR_PLLON;

	//Wait till the main PLL is ready
	while((RCC->CR & RCC_CR_PLLRDY) == 0) {}

	//Configure Flash prefetch, Instruction cache, Data cache and wait state
	FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

	//HCLK = SYSCLK / 1
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	//PCLK1 = HCLK / 4
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

	//PCLK2 = HCLK / 2
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

	//Select the main PLL as system clock source
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	//Wait till the main PLL is used as system clock source
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

	//Update the SystemCoreClock variable
	SystemCoreClockUpdate();

	//Update the cyclesInMicro variable
	cyclesInMicro = SystemCoreClock / microsInSec;
}


/// @brief Get system clock count
uint32_t ConcreteSystem::GetSysClkCounts()
{
	return sysTicks;
}


/// @brief System clock counter
void ConcreteSystem::SysTickCounter()
{
	sysTicks++;
}


/// @brief Delays for a specified number of microseconds.
void ConcreteSystem::DelayUs(uint32_t micros)
{
	uint32_t delayStart = DWT->CYCCNT;
	uint32_t delayCycles = (micros * cyclesInMicro);
	while ((DWT->CYCCNT - delayStart) < delayCycles);
}


/// @brief Delays for a specified number of milliseconds.
void ConcreteSystem::DelayMs(uint32_t millis)
{
	DelayUs(1000 * millis);
}


/// @brief Enables IRQ interrupts by clearing the I-bit in the CPSR.
void ConcreteSystem::EnableIRQ()
{
	__enable_irq();
}


/// @brief Disables IRQ interrupts by setting the I - bit in the CPSR.
void ConcreteSystem::DisableIRQ()
{
	__disable_irq();
}


/// @brief Reset STM32 core and peripherals
void ConcreteSystem::Reboot()
{
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}


/// @brief SysTick handler
extern "C" void __weak SysTick_Handler()
{
	kernel->system->SysTickCounter();
}
