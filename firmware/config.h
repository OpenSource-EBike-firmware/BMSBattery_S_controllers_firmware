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
#define PAS_DIRECTION PAS_DIRECTION_RIGHT
#define ASSIST_LEVEL_0 0.0
#define ASSIST_LEVEL_1 0.2
#define ASSIST_LEVEL_2 0.4
#define ASSIST_LEVEL_3 0.6
#define ASSIST_LEVEL_4 0.8
#define ASSIST_LEVEL_5 1.0
#define BATTERY_LI_ION_CELLS_NUMBER 13
#define ADC_BATTERY_CURRENT_MAX 35
#define ADC_BATTERY_REGEN_CURRENT_MAX 35
#define MOTOR_ROTOR_OFFSET_ANGLE 210 // 210 adjusted as a good value for Q11 direct drive motor
#define FOC_READ_ID_CURRENT_ANGLE_ADJUST 115
#define FOC_READ_ID_CURRENT_ANGLE_ADJUST_INVERT 242
#define MOTOR_ROTOR_ERPS_START_INTERPOLATION_60_DEGREES 10
#define ADC_MOTOR_CURRENT_MAX 120
#define ADC_MOTOR_REGEN_CURRENT_MAX 66
#define PWM_DUTY_CYCLE_RAMP_UP_INVERSE_STEP 10
#define PWM_DUTY_CYCLE_RAMP_DOWN_INVERSE_STEP 10

#define EBIKE_REGEN_EBRAKE_LIKE_COAST_BRAKES

#endif /* _CONFIG_H_ */
