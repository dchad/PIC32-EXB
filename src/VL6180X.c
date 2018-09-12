#include "VL6180X.h"
#include "i2c.h"

void VL6180X_write(uint16_t index, uint8_t data)
{
	//IdleI2C1();
	//StartI2C1();
   i2c_master_start();
   
    // Device write address
    //IdleI2C1();
    //MasterWriteI2C1(0x52);
   i2c_master_send(0x52);
   
    // Register address first byte
    //IdleI2C1();
    //MasterWriteI2C1(index>>8);
   i2c_master_send(index >> 8);

	// Register address second byte
	//IdleI2C1();
   // MasterWriteI2C1(index & 0xFF);
   i2c_master_send(index & 0xFF);
   
	//Data byte
	//IdleI2C1();
	//MasterWriteI2C1(data);
   i2c_master_send(data);

    // P
    //IdleI2C1();
    //StopI2C1();
   i2c_master_stop();

	return;

}

uint8_t VL6180X_read(uint16_t index)
{
	uint8_t tempByte;

	//IdleI2C1();
	//StartI2C1();
   i2c_master_start();

    // Device write address
    //IdleI2C1();
    //MasterWriteI2C1(0x52);
   i2c_master_send(0x52);
   
    // Register address first byte
    //IdleI2C1();
    //MasterWriteI2C1(index>>8);
   i2c_master_send(index >> 8);
   
	// Register address second byte
	//IdleI2C1();
   // MasterWriteI2C1(index & 0xFF);
   i2c_master_send(index & 0xFF);
   
	// R
   // IdleI2C1();
   // RestartI2C1();
   i2c_master_restart();
   
	// Read operation
	//IdleI2C1();
   // MasterWriteI2C1(0x53);
   i2c_master_send(0x53);

	//Read byte
	//IdleI2C1();
	//tempByte =  MasterReadI2C1();
   tempByte = i2c_master_recv();

	//IdleI2C1();
	//NotAckI2C1();
   i2c_master_ack(1); //NACK, no more data bytes required.
    
    // P
    //IdleI2C1();
    //StopI2C1();
   i2c_master_stop();

	return tempByte;

}

void VL6180X_init()
{
	//necessary for initialization of VL6180X,
    //see application note "AN4545: VL6180X basic ranging application note"
    //mandatory : private registers
    VL6180X_write(0x0207, 0x01);
    VL6180X_write(0x0208, 0x01);
    VL6180X_write(0x0096, 0x00);
    VL6180X_write(0x0097, 0xfd);
    VL6180X_write(0x00e3, 0x00);
    VL6180X_write(0x00e4, 0x04);
    VL6180X_write(0x00e5, 0x02);
    VL6180X_write(0x00e6, 0x01);
    VL6180X_write(0x00e7, 0x03);
    VL6180X_write(0x00f5, 0x02);
    VL6180X_write(0x00d9, 0x05);
    VL6180X_write(0x00db, 0xce);
    VL6180X_write(0x00dc, 0x03);
    VL6180X_write(0x00dd, 0xf8);
    VL6180X_write(0x009f, 0x00);
    VL6180X_write(0x00a3, 0x3c);
    VL6180X_write(0x00b7, 0x00);
    VL6180X_write(0x00bb, 0x03c);
    VL6180X_write(0x00b2, 0x09);
    VL6180X_write(0x00ca, 0x09);
    VL6180X_write(0x0198, 0x01);
    VL6180X_write(0x01b0, 0x17);
    VL6180X_write(0x01ad, 0x00);
    VL6180X_write(0x00ff, 0x05);
    VL6180X_write(0x0100, 0x05);
    VL6180X_write(0x0199, 0x05);
    VL6180X_write(0x01a6, 0x1b);
    VL6180X_write(0x01ac, 0x3e);
    VL6180X_write(0x01a7, 0x1f);
    VL6180X_write(0x0030, 0x00);

    //recommended : public registers
    VL6180X_write(SYSTEM__MODE_GPIO1, 0x10);	//Interrupt, Active Low
    VL6180X_write(READOUT__AVERAGING_SAMPLE_PERIOD, 0x30);
    VL6180X_write(SYSALS__ANALOGUE_GAIN, 0x46);
    VL6180X_write(SYSRANGE__VHV_REPEAT_RATE, 0xff);
    VL6180X_write(SYSALS__INTEGRATION_PERIOD, 0x63);
    VL6180X_write(SYSRANGE__VHV_RECALIBRATE, 0x01);

    //optional : public registers
    VL6180X_write(SYSRANGE__INTERMEASUREMENT_PERIOD, 0x09);
    VL6180X_write(SYSALS__INTERMEASUREMENT_PERIOD, 0x31);
    VL6180X_write(SYSTEM__INTERRUPT_CONFIG_GPIO, 0x24);
    //VL6180X_write(SYSRANGE__RANGE_CHECK_ENABLES, 0x00);
}

uint8_t VL6180X_getRangeResult()
{
	return VL6180X_read(RESULT__RANGE_VAL);
}

bool VL6180X_isFreshOutOfReset()
{
	if(VL6180X_read(SYSTEM__FRESH_OUT_OF_RESET) == 1)
		return 1;
	else
		return 0;
}

void VL6180X_clearResetFlag()
{
	VL6180X_write(SYSTEM__FRESH_OUT_OF_RESET, 0x00);
}
void VL6180X_setInterrupt(uint8_t int_flags)
{
	VL6180X_write(SYSTEM__INTERRUPT_CONFIG_GPIO, int_flags);
}


void VL6180X_startRangeMeasurement()
{
	VL6180X_write(SYSRANGE__START, 0x03);
}

bool VL6180X_isRangeResultReady()
{
	if((VL6180X_read(RESULT__INTERRUPT_STATUS_GPIO)&0x07) != 0x04){
		return 0;
	}else{
		return 1;
	}
}

uint16_t VL6180X_rangeReturnRate(){
	uint16_t rate = 0;
	rate += VL6180X_read(RESULT__RANGE_RETURN_RATE);
	rate = rate << 8;
	rate += VL6180X_read(RESULT__RANGE_RETURN_RATE + 1);
	return rate;
}

bool VL6180X_testConnection(){
	return VL6180X_read(IDENTIFICATION__MODEL_ID) == 0xB4;
}

uint8_t VL6180X_dataValidation()
{
	return VL6180X_read(RESULT__RANGE_STATUS);
}

bool VL6180X_isRangeDeviceReady()
{
    if((VL6180X_read(RESULT__RANGE_STATUS)&0x01) != 0)
        return true;
    else
        return false;
}

void VL6180X_clearInterruptFlag(uint8_t clear_flags)
{
	VL6180X_write(SYSTEM__INTERRUPT_CLEAR, clear_flags);
}