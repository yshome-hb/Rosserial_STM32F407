#include "arch_define.h"
#include "arch_init.h"
#include "delay.h"
#include "debug.h"

#undef	TAG
#define TAG		"arch_init"

int arch_mcu_init(void)
{
	//ota_init_app();
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
#if YS_LOG_LEVEL <= LOG_LEVEL_FATAL	
	debug_printf_init(YS_DEBUG_BAUDRATE);
#endif
	LOG_INFO_TAG(TAG, "<****** add mcu init func here ******>");

	return 0;
}
