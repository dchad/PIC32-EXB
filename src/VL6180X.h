/* 
 * File:   VL6180X.h
 * Author: larry
 *
 * Created on November 21, 2014, 11:22 AM
 */

#ifndef VL6180X_H
#define	VL6180X_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <plib.h>


#define IDENTIFICATION__MODEL_ID				0x00
#define IDENTIFICATION__MODEL_REV_MAJOR			0x01
#define IDENTIFICATION__MODEL_REV_MINOR			0x02
#define IDENTIFICATION__MODULE_REV_MAJOR		0x03
#define IDENTIFICATION__MODULE_REV_MINOR		0x04
#define IDENTIFICATION__DATE_HI					0x06
#define IDENTIFICATION__DATE_LO					0x07
#define IDENTIFICATION__TIME_H					0x08
#define IDENTIFICATION__TIME_L					0x09
#define SYSTEM__MODE_GPIO0						0X10
#define SYSTEM__MODE_GPIO1						0X11
#define SYSTEM__HISTORY_CTRL					0x12
#define SYSTEM__INTERRUPT_CONFIG_GPIO			0x14
#define SYSTEM__INTERRUPT_CLEAR					0x15
#define SYSTEM__FRESH_OUT_OF_RESET				0x16
#define SYSTEM__GROUPED_PARAMETER_HOLD			0x17
#define SYSRANGE__START							0x18
#define SYSRANGE__THRESH_HIGH					0x19
#define SYSRANGE__THRESH_LOW					0x1A
#define SYSRANGE__INTERMEASUREMENT_PERIOD		0x1B
#define SYSRANGE__MAX_CONVERGENCE_TIME			0x1C
#define SYSRANGE__CROSSTALK_COMPENSATION_RATE	0x1E
#define SYSRANGE__CROSSTALK_VALID_HEIGHT		0x21
#define SYSRANGE__EARLY_CONVERGENCE_ESTIMATE	0x22
#define SYSRANGE__PART_TO_PART_RANGE_OFFSET		0x24
#define SYSRANGE__RANGE_IGNORE_VALID_HEIGHT		0x25
#define SYSRANGE__RANGE_IGNORE_THRESHOLD		0x26
#define SYSRANGE__MAX_AMBIENT_LEVEL_MULT		0x2C
#define SYSRANGE__RANGE_CHECK_ENABLES			0x2D
#define SYSRANGE__VHV_RECALIBRATE				0x2E
#define SYSRANGE__VHV_REPEAT_RATE				0x31
#define SYSALS__START							0x38
#define SYSALS__THRESH_HIGH						0x3A
#define SYSALS__THRESH_LOW						0x3C
#define SYSALS__INTERMEASUREMENT_PERIOD			0x3E
#define SYSALS__ANALOGUE_GAIN					0x3F
#define SYSALS__INTEGRATION_PERIOD				0x40
#define RESULT__RANGE_STATUS					0x4D
#define RESULT__ALS_STATUS						0x4E
#define RESULT__INTERRUPT_STATUS_GPIO			0x4F
#define RESULT__ALS_VAL							0x50
#define RESULT__HISTORY_BUFFER_x_H				0x52
#define RESULT__HISTORY_BUFFER_x_L				0x60
#define RESULT__RANGE_VAL						0x62
#define RESULT__RANGE_RAW						0x64
#define RESULT__RANGE_RETURN_RATE				0x66
#define RESULT__RANGE_REFERENCE_RATE			0x68
#define RESULT__RANGE_RETURN_SIGNAL_COUNT		0x6C
#define RESULT__RANGE_REFERENCE_SIGNAL_COUNT	0x70
#define RESULT__RANGE_RETURN_AMB_COUNT			0x74
#define RESULT__RANGE_REFERENCE_AMB_COUNT		0x78
#define RESULT__RANGE_RETURN_CONV_TIME			0x7C
#define RESULT__RANGE_REFERENCE_CONV_TIME		0x80
#define READOUT__AVERAGING_SAMPLE_PERIOD		0x10A
#define FIRMWARE__BOOTUP						0x119
#define FIRMWARE__RESULT_SCALER					0x120
#define I2C_SLAVE__DEVICE_ADDRESS				0x212
#define INTERLEAVED_MODE__ENABLE				0x2A3

/* Interrupt Control */
#define RANGE_INT_DISABLED              0
#define RANGE_INT_LEVEL_LOW             1
#define RANGE_INT_LEVEL_HIGH            2
#define RANGE_INT_OUT_OF_WINDOW         3
#define RANGE_INT_NEW_SAMPLE_READY      4
#define ALS_INT_DISABLED                0
#define ALS_INT_LEVEL_LOW               8
#define ALS_INT_LEVEL_HIGH              16
#define ALS_INT_OUT_OF_WINDOW           24
#define ALS_INT_NEW_SAMPLE_READY        32
#define CLEAR_RANGE_INT					((uint8_t)1)
#define CLEAR_ALS_INT                   ((uint8_t)2)
#define CLEAR_ERR_INT                   ((uint8_t)4)


void VL6180X_write(uint16_t index, uint8_t data);
uint8_t VL6180X_read(uint16_t index);
bool VL6180X_isFreshOutOfReset();
void VL6180X_clearResetFlag();
void VL6180X_init();
void VL6180X_setInterrupt(uint8_t int_flags);
void VL6180X_startSingleRangeMeasurement();
void VL6180X_startContinuousRangeMeasurement();
uint8_t VL6180X_getRangeResult();
bool VL6180X_isRangeResultReady();
bool VL6180X_testConnection();
uint8_t VL6180X_dataValidation();
uint16_t VL6180X_rangeReturnRate();
bool VL6180X_isRangeDeviceReady();


void VL6180X_clearInterruptFlag(uint8_t clear_flags);

#endif	/* VL6180X_H */

