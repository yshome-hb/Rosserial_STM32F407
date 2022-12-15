#include "arch_define.h"
#include "arch_init.h"

#undef	TAG
#define TAG		"arch_init"

int arch_mcu_init(void)
{
	//ota_init_app();
	HAL_Init();
	system_clock_init(168);
	delay_init();
#if YS_LOG_LEVEL <= LOG_LEVEL_FATAL	
	debug_printf_init(YS_DEBUG_BAUDRATE);
#endif

	return 0;
}
