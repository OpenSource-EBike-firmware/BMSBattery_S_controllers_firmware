/*
 * EGG OpenSource EBike firmware
 *
 * Copyright (C) Casainho, 2015, 2106, 2017.
 *
 * Released under the GPL License, Version 3
 */

#include <stdint.h>
#include <stdio.h>
#include "stm8s_gpio.h"
#include "stm8s_tim1.h"
#include "motor.h"
#include "gpio.h"
#include "motor.h"
#include "pwm.h"
#include "config.h"
#include "adc.h"

uint16_t ui16_PWM_cycles_counter = 0;
uint16_t ui16_PWM_cycles_counter_6 = 0;
uint16_t ui16_PWM_cycles_counter_total = 0;

uint16_t ui16_motor_speed_erps = 0;
uint8_t ui8_motor_rotor_position = 0; // in 360/256 degrees
uint8_t ui8_motor_rotor_absolute_position = 0; // in 360/256 degrees
uint8_t ui8_position_correction_value = 127; // in 360/256 degrees
uint8_t ui8_interpolation_angle = 0;

uint8_t ui8_motor_interpolation_state = NO_INTERPOLATION_60_DEGREES;
uint8_t ui8_motor_state = MOTOR_STATE_COAST;

uint8_t ui8_hall_sensors = 0;
uint8_t ui8_hall_sensors_last = 0;

uint8_t ui8_ADC_id_current = 0;

uint8_t ui8_ADC_motor_current_max;
uint8_t ui8_ADC_motor_regen_current_max;
uint8_t ui8_ADC_motor_current_zero_value;

uint8_t ui8_half_e_rotation_flag = 0;

void TIM1_UPD_OVF_TRG_BRK_IRQHandler(void) __interrupt(TIM1_UPD_OVF_TRG_BRK_IRQHANDLER)
{
//debug_pin_set ();
  adc_trigger ();

  hall_sensors_read_and_action ();

  motor_fast_loop ();

  TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
//debug_pin_reset ();
}

void hall_sensors_read_and_action (void)
{
  // read hall sensors signal pins and mask other pins
  ui8_hall_sensors = (GPIO_ReadInputData (HALL_SENSORS__PORT) & (HALL_SENSORS_MASK));
  if ((ui8_hall_sensors != ui8_hall_sensors_last) ||
      (ui8_motor_state == MOTOR_STATE_STOP))
  {
    ui8_hall_sensors_last = ui8_hall_sensors;

    switch (ui8_hall_sensors)
    {
      case 3:
      if (ui8_motor_interpolation_state != INTERPOLATION_360_DEGREES)
      {
	pwm_phase_a_enable_pwm (ui8_duty_cycle);
	pwm_phase_b_disable (ui8_duty_cycle);
	pwm_phase_c_enable_low (ui8_duty_cycle);
      }
      break;

      case 1:
      if (ui8_half_e_rotation_flag == 1)
      {
	ui8_half_e_rotation_flag = 0;
	ui16_PWM_cycles_counter_total = ui16_PWM_cycles_counter;
	ui16_PWM_cycles_counter = 0;
	ui16_motor_speed_erps = PWM_CYCLES_SECOND / ui16_PWM_cycles_counter_total; // this division takes ~4.2us
      }

      // update motor state based on motor speed
      if (ui16_motor_speed_erps > 100)
      {
//	ui8_motor_interpolation_state = INTERPOLATION_360_DEGREES;
//	ui8_motor_state = MOTOR_STATE_RUNNING;
//	pwm_init_bipolar_4q ();
      }
      else
      {
	ui8_motor_interpolation_state = NO_INTERPOLATION_60_DEGREES;
//        pwm_init_6_steps ();
      }

      if (ui8_motor_interpolation_state != INTERPOLATION_360_DEGREES)
      {
	pwm_phase_a_enable_pwm (ui8_duty_cycle);
	pwm_phase_b_enable_low (ui8_duty_cycle);
	pwm_phase_c_disable (ui8_duty_cycle);
      }
      else
      {
	ui8_motor_rotor_absolute_position = ANGLE_240 + MOTOR_ROTOR_DELTA_PHASE_ANGLE_RIGHT;
      }
      break;

      case 5:
      if (ui8_motor_interpolation_state != INTERPOLATION_360_DEGREES)
      {
	pwm_phase_a_enable_pwm (ui8_duty_cycle);
	pwm_phase_b_enable_low (ui8_duty_cycle);
	pwm_phase_c_disable (ui8_duty_cycle);
      }
      break;

      case 4:
      if (ui8_motor_interpolation_state != INTERPOLATION_360_DEGREES)
      {
	pwm_phase_a_enable_low (ui8_duty_cycle);
	pwm_phase_b_disable (ui8_duty_cycle);
	pwm_phase_c_enable_pwm (ui8_duty_cycle);
      }
      break;

      case 6:
      ui8_half_e_rotation_flag = 1;

      if (ui8_motor_interpolation_state != INTERPOLATION_360_DEGREES)
      {
	pwm_phase_a_enable_low (ui8_duty_cycle);
	pwm_phase_b_enable_pwm (ui8_duty_cycle);
	pwm_phase_c_disable (ui8_duty_cycle);
      }
      break;

      case 2:
      if (ui8_motor_interpolation_state != INTERPOLATION_360_DEGREES)
      {
	pwm_phase_a_disable (ui8_duty_cycle);
	pwm_phase_b_enable_pwm (ui8_duty_cycle);
	pwm_phase_c_enable_low (ui8_duty_cycle);
      }
      break;

      default:
      return;
      break;
    }

    ui16_PWM_cycles_counter_6 = 0;
  }
}

// runs every 64us (PWM frequency)
void motor_fast_loop (void)
{
  // count number of fast loops / PWM cycles
  if (ui16_PWM_cycles_counter < PWM_CYCLES_COUNTER_MAX)
  {
    ui16_PWM_cycles_counter++;
    ui16_PWM_cycles_counter_6++;
  }
  else
  {
    ui16_PWM_cycles_counter = 0;
    ui16_PWM_cycles_counter_6 = 0;
    ui16_motor_speed_erps = 0;
    ui16_PWM_cycles_counter_total = 0;
    ui8_position_correction_value = 127;
    ui8_motor_interpolation_state = NO_INTERPOLATION_60_DEGREES;
    ui8_motor_state = MOTOR_STATE_STOP;
  }

#define DO_INTERPOLATION 1 // may be usefull to disable interpolation when debugging
#if DO_INTERPOLATION == 1
  // calculate the interpolation angle
  // interpolation seems a problem when motor starts, so avoid to do it at very low speed
  if (ui8_motor_interpolation_state == INTERPOLATION_60_DEGREES)
  {
    ui8_interpolation_angle = (ui16_PWM_cycles_counter_6 << 8) / ui16_PWM_cycles_counter_total;
#if MOTOR_TYPE == MOTOR_TYPE_Q85
    ui8_motor_rotor_position = ui8_motor_rotor_absolute_position + ui8_position_correction_value + ui8_interpolation_angle;
#elif MOTOR_TYPE == MOTOR_TYPE_EUC2
    ui8_motor_rotor_position = ui8_motor_rotor_absolute_position + ui8_position_correction_value - ui8_interpolation_angle;
#endif
  }
  else if (ui8_motor_interpolation_state == INTERPOLATION_360_DEGREES)
  {
    ui8_interpolation_angle = (ui16_PWM_cycles_counter << 8) / ui16_PWM_cycles_counter_total;
#if MOTOR_TYPE == MOTOR_TYPE_Q85
    ui8_motor_rotor_position = ui8_motor_rotor_absolute_position + ui8_position_correction_value + ui8_interpolation_angle;
#elif MOTOR_TYPE == MOTOR_TYPE_EUC2
    ui8_motor_rotor_position = ui8_motor_rotor_absolute_position + ui8_position_correction_value - ui8_interpolation_angle;
#endif
  }
  else // MOTOR_STATE_COAST || MOTOR_STATE_RUNNING_NO_INTERPOLATION_60_DEGREES
#endif
  {
    ui8_motor_rotor_position = ui8_motor_rotor_absolute_position;
  }

  pwm_duty_cycle_controller ();
}

void motor_set_mode_coast (void)
{
  TIM1_CtrlPWMOutputs(DISABLE);
  ui8_motor_state = MOTOR_STATE_COAST;
}

void motor_set_mode_run (void)
{
  TIM1_CtrlPWMOutputs(ENABLE);
}

void hall_sensor_init (void)
{
  GPIO_Init(HALL_SENSORS__PORT,
	    (GPIO_Pin_TypeDef)(HALL_SENSOR_A__PIN | HALL_SENSOR_B__PIN | HALL_SENSOR_C__PIN),
	    GPIO_MODE_IN_FL_NO_IT);
}

void motor_init (void)
{
  uint8_t ui8_counter = 0;
  uint16_t ui16_temp = 0;

//  /***************************************************************************************/
//  // motor overcurrent pin as external input pin interrupt
//  GPIO_Init(CURRENT_MOTOR_TOTAL_OVER__PORT,
//	    CURRENT_MOTOR_TOTAL_OVER__PIN,
//	    GPIO_MODE_IN_FL_IT); // with external interrupt
//
//  //initialize the Interrupt sensitivity
//  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD,
//			    EXTI_SENSITIVITY_FALL_LOW);
//  /***************************************************************************************/

  /***************************************************************************************/
  // reading some samples of ADC motor total current and average, to get the real zero value
  while (ui8_counter < 32)
  {
    ui16_temp += ui8_adc_read_motor_total_current ();
    ui8_counter++;
  }
  ui8_ADC_motor_current_zero_value = ui16_temp >> 5; // ui16_temp / 32
  /***************************************************************************************/

  motor_set_current_max (ADC_MOTOR_CURRENT_MAX);
  motor_set_regen_current_max (ADC_MOTOR_REGEN_CURRENT_MAX);
  motor_set_pwm_duty_cycle_ramp_inverse_step (PWM_DUTY_CYCLE_RAMP_INVERSE_STEP);
}

void motor_set_pwm_duty_cycle_target (uint8_t value)
{
  pwm_set_duty_cycle_target (value);
}

void motor_set_current_max (uint8_t value)
{
  ui8_ADC_motor_current_max = value;
}

void motor_set_regen_current_max (uint8_t value)
{
  ui8_ADC_motor_regen_current_max = value;
}

void motor_set_pwm_duty_cycle_ramp_inverse_step (uint8_t value)
{
  ui8_duty_cycle_ramp_inverse_step = value;
}

uint16_t motor_get_motor_speed_erps (void)
{
  return ui16_motor_speed_erps;
}

// motor overcurrent interrupt
void EXTI_PORTD_IRQHandler(void) __interrupt(EXTI_PORTD_IRQHANDLER)
{
    motor_set_mode_coast ();
    while (1) ; // infinite loop, user will need to reset the system
}

