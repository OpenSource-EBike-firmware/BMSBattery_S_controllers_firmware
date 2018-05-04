/*
 * config.h
 *
 *  Automatically created by Flexible OpenSource firmware - Configuration tool
 *  Author: stancecoke
 *  Author: casainho
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define EBIKE_THROTTLE_TYPE EBIKE_THROTTLE_TYPE_TORQUE_SENSOR
#define EBIKE_THROTTLE_TYPE_THROTTLE_PAS_CURRENT_SPEED
#define EBIKE_THROTTLE_TYPE_THROTTLE_PAS_ASSIST_LEVEL_PAS_ONLY
#define PAS_NUMBER_MAGNETS 12
#define PAS_MAX_CADENCE_RPM 80
#define PAS_THRESHOLD 1.7
#define PAS_DIRECTION PAS_DIRECTION_RIGHT
#define ASSIST_LEVEL_0 0.0
#define ASSIST_LEVEL_1 0.25
#define ASSIST_LEVEL_2 0.5
#define ASSIST_LEVEL_3 1.0
#define ASSIST_LEVEL_4 2.0
#define ASSIST_LEVEL_5 4.0
#define BATTERY_LI_ION_CELLS_NUMBER 7
#define ADC_BATTERY_CURRENT_MAX 35
#define ADC_BATTERY_REGEN_CURRENT_MAX 35
#define MOTOR_ROTOR_OFFSET_ANGLE 0
#define MOTOR_ROTOR_ERPS_START_INTERPOLATION_60_DEGREES 20
#define ADC_MOTOR_CURRENT_MAX 120
#define ADC_MOTOR_REGEN_CURRENT_MAX 66
#define PWM_DUTY_CYCLE_RAMP_UP_INVERSE_STEP 35
#define PWM_DUTY_CYCLE_RAMP_DOWN_INVERSE_STEP 35
#define LI_ION_CELL_VOLTS_MAX 	4.25
#define LI_ION_CELL_VOLTS_100 	4.06
#define LI_ION_CELL_VOLTS_80 	3.93
#define LI_ION_CELL_VOLTS_60 	3.78
#define LI_ION_CELL_VOLTS_40 	3.60
#define LI_ION_CELL_VOLTS_20 	3.38
#define LI_ION_CELL_VOLTS_10 	3.25
#define LI_ION_CELL_VOLTS_0 	3.00

#endif /* _CONFIG_H_ */
