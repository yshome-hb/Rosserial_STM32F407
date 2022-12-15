#ifndef __SERVO_H__
#define __SERVO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "stm32f4xx.h" 

#define SERVO_PULSE_MIN     510     // the shortest pulse sent to a servo  
#define SERVO_PULSE_MAX     2490    // the longest pulse sent to a servo 
#define SERVO_PULSE_DEFAULT 1500    // default pulse width when servo is attached
#define REFRESH_INTERVAL    20000   // minimum time to refresh servos in microseconds 

#define SERVOS_PER_TIMER    8       // the maximum number of servos controlled by one timer 
#define MAX_SERVOS          SERVOS_PER_TIMER

typedef struct {
    GPIO_TypeDef* port; 
    uint16_t pin;
    bool enable;
    uint16_t ticks;

} servo_handle_t;

void servo_init(void);
void servo_attach(uint8_t id, uint16_t ticks);
void servo_detach(uint8_t id);


#ifdef __cplusplus
}
#endif

#endif
