/**
 ******************************************************************************
 * @file			stm32f1xx_it.c
 * @author			Kevin Weiss
 * @date			12.03.2020
 * @brief			Handles all the system interrupts.
 ******************************************************************************
 */

/******************************************************************************/
/*            Cortex-M3 Processor Interruption and Exception Handlers         */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {

}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
	while (1) {
	}
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void) {
	while (1) {
	}
}

/**
 * @brief This function handles Prefetch fault, memory access fault.
 */
void BusFault_Handler(void) {
	while (1) {
	}
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void) {
	while (1) {
	}
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void) {
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void) {

}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void) {

}
