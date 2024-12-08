/*
 * ads.c
 *
 *  Created on: Mar 21, 2020
 *      Author: i8-tech
 */


/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "spi.h"
#include "ads.h"
extern uint8_t RDATACflag;
extern uint8_t testshow;

int ledspeed;
uint8_t BiasDerivationP, BiasDerivationN;


/*----------------------------------------------------------------------------*/
/* Initialize ADS(s) Chips                                                              */
/*----------------------------------------------------------------------------*/
/** Initial ADS1299 as
        * Internal clock
        * START Pin hold low
        * Reset ADS once
        * PWDN Pin high
        * SPI turn on
*/
void ADS_Init(int main_ic)
{

	HAL_GPIO_WritePin(START,GPIO_PIN_RESET);  //ADS not Start any conversation
	HAL_GPIO_WritePin(CS0,GPIO_PIN_SET);  //ADS SPI turn off
	HAL_GPIO_WritePin(CS1,GPIO_PIN_SET);  //ADS SPI turn off
	HAL_GPIO_WritePin(CS2,GPIO_PIN_SET);  //ADS SPI turn off


	if (main_ic == 0)
	{
		HAL_GPIO_WritePin(CLKSEL0,GPIO_PIN_SET);    //Internal clock
		HAL_GPIO_WritePin(CLKSEL1,GPIO_PIN_RESET);   //External clock
		HAL_GPIO_WritePin(CLKSEL2,GPIO_PIN_RESET);    //External clock
	}
	if (main_ic == 1)
	{
		HAL_GPIO_WritePin(CLKSEL0,GPIO_PIN_RESET);    //External clock
		HAL_GPIO_WritePin(CLKSEL1,GPIO_PIN_SET);      //Internal clock
		HAL_GPIO_WritePin(CLKSEL2,GPIO_PIN_RESET);    //External clock
	}
	if (main_ic == 2)
	{
		HAL_GPIO_WritePin(CLKSEL0,GPIO_PIN_RESET);    //External clock
		HAL_GPIO_WritePin(CLKSEL1,GPIO_PIN_RESET);      //External clock
		HAL_GPIO_WritePin(CLKSEL2,GPIO_PIN_SET);    //Internal clock
	}

//	HAL_GPIO_WritePin(CLKSEL0,GPIO_PIN_RESET);    //External clock
//	HAL_GPIO_WritePin(CLKSEL1,GPIO_PIN_RESET);      //External clock
//	HAL_GPIO_WritePin(CLKSEL2,GPIO_PIN_RESET);    //External clock
//	HAL_Delay(1000);

	ADS_Write_Reg(0x01,0xB6, main_ic);  //defult = 0x96      sampling Rate = 250, clk output to two other ADS






	HAL_Delay(11*CSdelay);
	HAL_GPIO_WritePin(PWDN,GPIO_PIN_SET);    //To exit power-down mode, take the PWDN pin high
	HAL_GPIO_WritePin(RSTN,GPIO_PIN_SET);    //not reset ADS

	HAL_Delay(200);

	HAL_GPIO_WritePin(RSTN,GPIO_PIN_RESET);   //reset ADS
	HAL_Delay(11*CSdelay);
	HAL_GPIO_WritePin(RSTN,GPIO_PIN_SET);    //not reset ADS
	HAL_Delay(11*CSdelay);


	ADS_Stop_data_cont(1);     //Device Wakes Up in RDATAC Mode, so Send SDATAC Command so Registers can be Written
	ADS_Write_Reg(0x01,0xF6, main_ic); //0b1111 0110  //defult = 0x96      sampling Rate = 250, clk output to two other ADS

	HAL_Delay(100);

	ADS_Reset(0);
	ADS_Reset(2);

	HAL_Delay(2*CSdelay);

	ADS_Stop_data_cont(0);     //Device Wakes Up in RDATAC Mode, so Send SDATAC Command so Registers can be Written
	//ADS_Stop_data_cont(1);     //Device Wakes Up in RDATAC Mode, so Send SDATAC Command so Registers can be Written
	ADS_Stop_data_cont(2);     //Device Wakes Up in RDATAC Mode, so Send SDATAC Command so Registers can be Written
		HAL_Delay(100);
	//ADS_Write_Reg(0x01,0xF6, main_ic); //0b1111 0110  //defult = 0x96      sampling Rate = 250, clk output to two other ADS

	ADS_Internal_Ref_Active(0, 0xEC);  //Enable internal reference buffer, no BIAS measurement, BIASREF signal fed internally, Power on BIAS buffer
	ADS_Internal_Ref_Active(1, 0x7C);  //Enable internal reference buffer, no BIAS measurement, BIASREF signal fed internally, BIAS buffer is powered down
	ADS_Internal_Ref_Active(2, 0x7C);  //Enable internal reference buffer, no BIAS measurement, BIASREF signal fed internally, BIAS buffer is powered down

	ADS_Write_Reg(0x14,0xF0, 0);  //ALL_CHANNELS gpio output and 1
	ADS_Write_Reg(0x14,0xF0, 1);  //ALL_CHANNELS gpio output and 1
	ADS_Write_Reg(0x14,0xF0, 2);  //ALL_CHANNELS gpio output and 1



	HAL_Delay(CSdelay);
}
/*----------------------------------------------------------------------------*/
/* Write a value to a Register in ADS                                         */
/*----------------------------------------------------------------------------*/
/** Input as:
				* Reg = Register Number
				* val = Register Value
				* i   = ADS Number to write
*/
void ADS_Write_Reg(uint8_t Reg, uint8_t val, int i)
{
	uint8_t trans[3];
	if (i == 0) HAL_GPIO_WritePin(CS0,GPIO_PIN_RESET);  //ADS SPI turn on
	if (i == 1) HAL_GPIO_WritePin(CS1,GPIO_PIN_RESET);  //ADS SPI turn on
	if (i == 2) HAL_GPIO_WritePin(CS2,GPIO_PIN_RESET);  //ADS SPI turn on

	 HAL_Delay(2*CSdelay);                     //wait for ads spi to turn on
		trans[0] = Reg + 0x40;                 //regiter address + write command
		trans[1] = 0x00;                       //number of registger -1
		trans[2] = val;                        //register value
		HAL_SPI_Transmit(&hspi1,trans,3,1000);
	HAL_Delay(2*CSdelay);                    //wait for spi to transmit

	if (i == 0) HAL_GPIO_WritePin(CS0,GPIO_PIN_SET);  //ADS SPI turn off
	if (i == 1) HAL_GPIO_WritePin(CS1,GPIO_PIN_SET);  //ADS SPI turn off
	if (i == 2) HAL_GPIO_WritePin(CS2,GPIO_PIN_SET);  //ADS SPI turn off

	HAL_Delay(2*CSdelay);
}
/*----------------------------------------------------------------------------*/
/* Read value from a Register in ADS                                          */
/*----------------------------------------------------------------------------*/
/**


*/


uint8_t ADS_Read_Reg(uint8_t Reg, int i)
{
	uint8_t trans[3];
	uint8_t recive[3];
	if (i == 0) HAL_GPIO_WritePin(CS0,GPIO_PIN_RESET);  //ADS SPI turn on
	if (i == 1) HAL_GPIO_WritePin(CS1,GPIO_PIN_RESET);  //ADS SPI turn on
	if (i == 2) HAL_GPIO_WritePin(CS2,GPIO_PIN_RESET);  //ADS SPI turn on
	  HAL_Delay(CSdelay);                     //wait for ads spi to turn on
		 trans[0] = Reg + 0x20;                 //regiter address + read command
		 trans[1] = 0x00;                       //number of registger -1
		 trans[2] = 0x00;                       //dummy
		//HAL_SPI_TransmitReceive_IT(&hspi1,trans,recive,3);
		HAL_SPI_TransmitReceive(&hspi1,trans,recive,3, 1000);
		HAL_Delay(CSdelay);
	if (i == 0) HAL_GPIO_WritePin(CS0,GPIO_PIN_SET);  //ADS SPI turn off
	if (i == 1) HAL_GPIO_WritePin(CS1,GPIO_PIN_SET);  //ADS SPI turn off
	if (i == 2) HAL_GPIO_WritePin(CS2,GPIO_PIN_SET);  //ADS SPI turn off
	HAL_Delay(CSdelay);
  return recive[2];
}
/*----------------------------------------------------------------------------*/
/* Commands in ADS                                          */
/*----------------------------------------------------------------------------*/
/**
*/
void ADS_Wakeup(void)
{
	uint8_t trans;
	HAL_GPIO_WritePin(CSN,GPIO_PIN_RESET);  //ADS SPI turn on
	HAL_Delay(CSdelay);                     //wait for ads spi to turn on
		trans = 0x02;      //wake
		HAL_SPI_Transmit(&hspi1,&trans,1, 1000);
	HAL_Delay(CSdelay);
  HAL_GPIO_WritePin(CSN,GPIO_PIN_SET);  //ADS SPI turn OFF
}
void ADS_Standby(void)
{
	uint8_t trans;
	HAL_GPIO_WritePin(CSN,GPIO_PIN_RESET);  //ADS SPI turn on
	HAL_Delay(CSdelay);                     //wait for ads spi to turn on
		trans = 0x04;      //standby
		HAL_SPI_Transmit(&hspi1,&trans,1,1000);
	HAL_Delay(CSdelay);
  HAL_GPIO_WritePin(CSN,GPIO_PIN_SET);  //ADS SPI turn OFF
}
void ADS_Reset(int i)
{
	uint8_t trans;
	if (i == 0) HAL_GPIO_WritePin(CS0,GPIO_PIN_RESET);  //ADS SPI turn on
	if (i == 1) HAL_GPIO_WritePin(CS1,GPIO_PIN_RESET);  //ADS SPI turn on
	if (i == 2) HAL_GPIO_WritePin(CS2,GPIO_PIN_RESET);  //ADS SPI turn on
	HAL_Delay(CSdelay);                     //wait for ads spi to turn on
		trans = 0x06;      //reset
		HAL_SPI_Transmit(&hspi1,&trans,1, 1000);
	HAL_Delay(CSdelay);
  	if (i == 0) HAL_GPIO_WritePin(CS0,GPIO_PIN_SET);  //ADS SPI turn off
	if (i == 1) HAL_GPIO_WritePin(CS1,GPIO_PIN_SET);  //ADS SPI turn off
	if (i == 2) HAL_GPIO_WritePin(CS2,GPIO_PIN_SET);  //ADS SPI turn off
}

void ADS_Start(void)
{
	HAL_GPIO_WritePin(START,GPIO_PIN_SET);  // Start a Sync Conversation
	HAL_Delay(CSdelay);
}

void ADS_Stop(void)
{
	HAL_GPIO_WritePin(START,GPIO_PIN_RESET);  //Stop a Sync Conversation
	HAL_Delay(CSdelay);
}



void ADS_Read_data_cont(int i)
	{
	uint8_t trans;
	if (i == 0) HAL_GPIO_WritePin(CS0,GPIO_PIN_RESET);  //ADS SPI turn on
	if (i == 1) HAL_GPIO_WritePin(CS1,GPIO_PIN_RESET);  //ADS SPI turn on
	if (i == 2) HAL_GPIO_WritePin(CS2,GPIO_PIN_RESET);  //ADS SPI turn on
	HAL_Delay(CSdelay);                     //wait for ads spi to turn on
		trans = 0x10;                         //RDATAC
		HAL_SPI_Transmit(&hspi1,&trans,1, 1000);
	HAL_Delay(CSdelay);
  if (i == 0) HAL_GPIO_WritePin(CS0,GPIO_PIN_SET);  //ADS SPI turn off
	if (i == 1) HAL_GPIO_WritePin(CS1,GPIO_PIN_SET);  //ADS SPI turn off
	if (i == 2) HAL_GPIO_WritePin(CS2,GPIO_PIN_SET);  //ADS SPI turn off
}
	

void ADS_Stop_data_cont(int i)
{
	uint8_t trans;
	if (i == 0) HAL_GPIO_WritePin(CS0,GPIO_PIN_RESET);  //ADS SPI turn on
	if (i == 1) HAL_GPIO_WritePin(CS1,GPIO_PIN_RESET);  //ADS SPI turn on
	if (i == 2) HAL_GPIO_WritePin(CS2,GPIO_PIN_RESET);  //ADS SPI turn on

	HAL_Delay(CSdelay);                     //wait for ads spi to turn on
		trans = 0x11;                        //SDATAC
		HAL_SPI_Transmit(&hspi1,&trans,1, 1000);
	HAL_Delay(CSdelay);

 	if (i == 0) HAL_GPIO_WritePin(CS0,GPIO_PIN_SET);  //ADS SPI turn off
	if (i == 1) HAL_GPIO_WritePin(CS1,GPIO_PIN_SET);  //ADS SPI turn off
	if (i == 2) HAL_GPIO_WritePin(CS2,GPIO_PIN_SET);  //ADS SPI turn off

}
void ADS_Read_data_Single(int i)
{
	uint8_t trans;
	if (i == 0) HAL_GPIO_WritePin(CS0,GPIO_PIN_RESET);  //ADS SPI turn on
	if (i == 1) HAL_GPIO_WritePin(CS1,GPIO_PIN_RESET);  //ADS SPI turn on
	if (i == 2) HAL_GPIO_WritePin(CS2,GPIO_PIN_RESET);  //ADS SPI turn on
	HAL_Delay(CSdelay);                     //wait for ads spi to turn on
		trans = 0x12;      //RDATA
		HAL_SPI_Transmit(&hspi1,&trans,1,1000);
	HAL_Delay(CSdelay);
  if (i == 0) HAL_GPIO_WritePin(CS0,GPIO_PIN_SET);  //ADS SPI turn off
	if (i == 1) HAL_GPIO_WritePin(CS1,GPIO_PIN_SET);  //ADS SPI turn off
	if (i == 2) HAL_GPIO_WritePin(CS2,GPIO_PIN_SET);  //ADS SPI turn off
}
/*----------------------------------------------------------------------------*/
/* Bias Signal Derivation (por shavad)                                                             */
/*----------------------------------------------------------------------------*/
/** Configure ADS1299 as
        * Confg3 Register
        *  Configuration Register 3 (address = 03h) (reset = 60h)
        * MSB bit = PD_REFBU(not) = 0;
        * 1 : Enable internal reference buffer
        * 0b 1110 0000  = 0xE0;
*/
void ADS_BIAS_SIGNAL_DERIVATION(uint8_t P, uint8_t N, int i)
{
	   ADS_Write_Reg(0x0D, P ,i);   //0b11111111 //route all Pch Positive to bias deriv.
	   ADS_Write_Reg(0x0E, N ,i);
	   //ADS_Write_Reg(0x0E, N ,i);   //0b11111111 //route all Nch negative to bias deriv.
	   HAL_Delay(CSdelay);
}
/*----------------------------------------------------------------------------*/
/* SRB1 Connection                                                             */
/*----------------------------------------------------------------------------*/
/** Configure ADS1299 as
        * MISC1: Miscellaneous 1 Register
        *  Configuration Register MISC1 (address = 15h) (reset = 00h)
        * bit 2 = Stimulus, reference, and bias 1
				This bit connects the SRB1 to all 4, 6, or 8 channels inverting
				inputs
				0 : Switches open
				1 : Switches closed
*/
void ADS_SRB1_Conection(uint8_t con, int i)
{
	if (con ==1)
	{
  	 ADS_Write_Reg(0x15, 0x20 ,i);   //SRB1 Switch close //connected
	   HAL_Delay(CSdelay);
	}
	if (con ==0)
	{
  	 ADS_Write_Reg(0x15, 0x00 ,i);   //SRB1 Switch open //not connected
	   HAL_Delay(CSdelay);
	}
}

/*----------------------------------------------------------------------------*/
/* Internal 4.5V VREFP Active ADS                                                              */
/*----------------------------------------------------------------------------*/
/** Configure ADS1299 as
        * Confg3 Register
        * Configuration Register 3 (address = 03h) (reset = 60h)
        * MSB bit = PD_REFBU(not) = 0;
        * Enable internal reference buffer
        * 0b 1110 0000  = 0xE0;

		7 PD_REFBUF R/W 0h Power-down reference buffer This bit determines the power-down reference buffer state. 0 : Power-down internal reference buffer 1 : Enable internal reference buffer
	  6:5 Reserved R/W 3h Reserved Always write 3h.
		4 BIAS_MEAS R/W 0h BIAS measurement This bit enables BIAS measurement. The BIAS signal may be measured with any channel. 0 : Open 1 : BIAS_IN signal is routed to the channel that has the MUX_Setting 010 (VREF)
	  3 BIASREF_INT R/W 0h BIASREF signal This bit determines the BIASREF signal source. 0 : BIASREF signal fed externally 1 : BIASREF signal (AVDD + AVSS) / 2 generated internally
		2 PD_BIAS R/W 0h BIAS buffer power This bit determines the BIAS buffer power state. 0 : BIAS buffer is powered down 1 : BIAS buffer is enabled
		1 BIAS_LOFF_SENS R/W 0h BIAS sense function This bit enables the BIAS sense function. 0 : BIAS sense is disabled 1 : BIAS sense is enabled
		0 BIAS_STAT R 0h BIAS lead-off status This bit determines the BIAS status. 0 : BIAS is connected 1 : BIAS is not connected
*/
void ADS_Internal_Ref_Active(int i, uint8_t dd)
{
	ADS_Write_Reg(0x03, dd, i);     //0b1 11 0 1100
	HAL_Delay(CSdelay);
}

void ADS_LEOF(void)
{
////////////////////////////////////////////////////////////////////ADS lead-off detection enable
/*	CONFIG4: Configuration Register 4 (address = 17h) (reset = 00h)
		7:4 Reserved R/W 0h Reserved Always write 0h
	  3 SINGLE_SHOT R/W 0h Single-shot conversion This bit sets the conversion mode. 0 : Continuous conversion mode 1 : Single-shot mode
		2 Reserved R/W 0h Reserved Always write 0h
		1 PD_LOFF_COMP R/W 0h Lead-off comparator power-down This bit powers down the lead-off comparators. 0 : Lead-off comparators disabled 1 : Lead-off comparators enabled
		0 Reserved R/W 0h Reserved Always write 0h
*/
	for (int ii=0; ii<3; ii++)
	{
		//  ADS_Write_Reg(0x01, 0xB3, ii); //sampling rate => 2k  0b11
		  ADS_SRB1_Conection(1, ii); //SRB conncted to all ch N
		
		  //No channels can connected to bias derivation while Leadd-off detection is ON
		  ADS_BIAS_SIGNAL_DERIVATION(0x00, 0x00, ii);

		//  Comparator positive side = 95%
		//  Lead-off current magnitude = 24 nA
		// AC lead-off detection at 7.8 Hz (fCLK / 216)
		  //ADS_Write_Reg(0x04, 0x01 ,ii);  // Lead-Off Control Register 		6nA
		  ADS_Write_Reg(0x04, 0x05 ,ii);  // Lead-Off Control Register			24nA
		//  ADS_Write_Reg(0x04, 0x09 ,ii);  // Lead-Off Control Register		6uA
		// enable/disable the lead-off comparators.  (disable)
		  ADS_Write_Reg(0x17, 0x00 ,ii);  //0b 0000 0010  CONFIG4

		//LOFF_SENSN: Negative Signal Lead-Off Detection Register (address = 10h) (reset = 00h)
			ADS_Write_Reg(0x10, 0x00 ,ii);

		 // channel state setting
			ADS_Write_Reg(0x05,0x00,ii);
			ADS_Write_Reg(0x06,0x00,ii);
			ADS_Write_Reg(0x07,0x00,ii);
			ADS_Write_Reg(0x08,0x00,ii);
			ADS_Write_Reg(0x09,0x00,ii);
			ADS_Write_Reg(0x0A,0x00,ii);
			ADS_Write_Reg(0x0B,0x00,ii);
			ADS_Write_Reg(0x0C,0x00,ii);
	}

	//LOFF_SENSP: Positive Signal Lead-Off Detection Register (address = 0Fh) (reset = 00h)
	//  ADS_Write_Reg(0x0F, 0xFC ,2); //1,2
	  ADS_Write_Reg(0x0F, 0xF8 ,2);
	  ADS_Write_Reg(0x0F, 0xFF ,1);
	  ADS_Write_Reg(0x0F, 0xFF ,0);

	  ADS_Write_Reg(0x03, 0xE8, 2);     // Enable bias buffer
	  ADS_Write_Reg(0x03, 0xE8, 1);     // Enable bias buffer
	  ADS_Write_Reg(0x03, 0xEC, 0);     //0b1 11 1 1100  enable bias buffer      //secondary IC biasAmp turn on


}


void  analog_setting(ADS_SettingTypeDef device_setting)
{


	if ((device_setting.SamplingRate)=='1') //250 sps
	{
		ledspeed=10;
		ADS_Write_Reg(0x01,0xD6, 0);
		ADS_Write_Reg(0x01,0xF6, 1);  // main IC
		ADS_Write_Reg(0x01,0xD6, 2);
	}
	if ((device_setting.SamplingRate)=='2')  //500 sps
	{
		ledspeed=20;
		ADS_Write_Reg(0x01,0xD5, 0);
		ADS_Write_Reg(0x01,0xF5, 1);  // main IC
		ADS_Write_Reg(0x01,0xD5, 2);
	}
	if ((device_setting.SamplingRate)=='4') //1k  sps
	{
		ledspeed=40;
		ADS_Write_Reg(0x01,0xD4, 0);
		ADS_Write_Reg(0x01,0xF4, 1);  // main IC
		ADS_Write_Reg(0x01,0xD4, 2);
	}
	if ((device_setting.SamplingRate)=='8')    //2k  sps
	{
		ledspeed=80;
		ADS_Write_Reg(0x01,0xD3, 0);
		ADS_Write_Reg(0x01,0xF3, 1);  // main IC
		ADS_Write_Reg(0x01,0xD3, 2);
	}


	if (device_setting.LeadoffMode == 'L')
	{
			ADS_LEOF();

			ADS_Write_Reg(0x0F, 0xF8 + (device_setting.ExtraChannels & 0x07), 2);

			if (!(device_setting.ExtraChannels & 0x01))  ADS_Write_Reg(0x05,0x81,2);  //extra channels off
			if (!(device_setting.ExtraChannels & 0x02))  ADS_Write_Reg(0x06,0x81,2);
			if (!(device_setting.ExtraChannels & 0x04))  ADS_Write_Reg(0x07,0x81,2);
	}
	else
	{
		
			// normal operation (no lead-off)


		ADS_Write_Reg(0x03, 0xE8, 2);     //0b1 11 1 1000  enable bias buffer      //secondary IC biasAmp turn off
		ADS_Write_Reg(0x03, 0xE8, 1);     //0b1 11 1 1000  enable bias buffer      //main IC biasAmp turn off
		ADS_Write_Reg(0x03, 0xEC, 0);     //0b1 11 1 1100  enable bias buffer      //secondary IC biasAmp turn on

		
	  for (int i=0; i<3; i ++)
		{

			ADS_Write_Reg(0x17, 0x00 ,i);     //LEOF off
			ADS_Write_Reg(0x0F, 0x00 ,i);	 // lead off dis N
			ADS_Write_Reg(0x10, 0x00 ,i);	// lead off dis P
			ADS_Write_Reg(0x04, 0x00 ,i);
		}

		


	// SRB1 connection on/off
		ADS_SRB1_Conection(device_setting.SRBConnection == 'C' ? 1 : 0, 0);
		ADS_SRB1_Conection(device_setting.SRBConnection == 'C' ? 1 : 0, 1);
		ADS_SRB1_Conection(device_setting.SRBConnection == 'C' ? 1 : 0, 2);

		/*
		Bit 	Field 			Type 		Reset 	Description
		7 		PDn 				R/W 		0h 			Power-down:
																			This bit determines the channel power mode for the
																			corresponding channel.
																			0 : Normal operation
																			1 : Channel power-down.
																			When powering down a channel, TI recommends that the
																			channel be set to input short by setting the appropriate
																			MUXn[2:0] = 001 of the CHnSET register.

		6:4 	GAINn[2:0]	 R/W 		6h 			PGA gain:
																			These bits determine the PGA gain setting.
																			000 : 1
																			001 : 2
																			010 : 4
																			011 : 6
																			100 : 8
																			101 : 12
																			110 : 24
																			111 : Do not use

		3 		SRB2 				R/W 		0h 			SRB2 connection
																			This bit determines the SRB2 connection for the corresponding
																			channel.
																			0 : Open
																			1 : Closed

		2:0 	MUXn[2:0] 	R/W 		1h 			Channel input
																			These bits determine the channel input selection.
																			000 : Normal electrode input
																			001 : Input shorted (for offset or noise measurements)
																			010 : Used in conjunction with BIAS_MEAS bit for BIAS
																			measurements.
																			011 : MVDD for supply measurement
																			100 : Temperature sensor
																			101 : Test signal
																			110 : BIAS_DRP (positive electrode is the driver)
																			111 : BIAS_DRN (negative electrode is the driver)
		*/
			// ex channels on/off	+ gain
			uint8_t reg_gain = 0, exgain = 0;
			uint8_t ch_off_reg = 0x81; //channel off + input short
			if (device_setting.Gain == 1)  reg_gain = 0x00;
			if (device_setting.Gain == 2)  reg_gain = 0x10;
			if (device_setting.Gain == 4)  reg_gain = 0x20;
			if (device_setting.Gain == 6)  reg_gain = 0x30;
			if (device_setting.Gain == 8)  reg_gain = 0x40;
			if (device_setting.Gain == 12) reg_gain = 0x50;
			if (device_setting.Gain == 24) reg_gain = 0x60;

			if (device_setting.ExtraGain == 1)  exgain = 0x00;
			if (device_setting.ExtraGain == 2)  exgain = 0x10;
			if (device_setting.ExtraGain == 4)  exgain = 0x20;
			if (device_setting.ExtraGain == 6)  exgain = 0x30;
			if (device_setting.ExtraGain == 8)  exgain = 0x40;
			if (device_setting.ExtraGain == 12) exgain = 0x50;
			if (device_setting.ExtraGain == 24) exgain = 0x60;


			if (device_setting.ExtraChannels & 0x01) ADS_Write_Reg(0x05, exgain  ,2);  else ADS_Write_Reg(0x05, ch_off_reg  ,2);
			if (device_setting.ExtraChannels & 0x02) ADS_Write_Reg(0x06, exgain  ,2);  else ADS_Write_Reg(0x06, ch_off_reg  ,2);
			if (device_setting.ExtraChannels & 0x04) ADS_Write_Reg(0x07, exgain  ,2);  else ADS_Write_Reg(0x07, ch_off_reg  ,2);

			if (device_setting.Channels & 0x000001) ADS_Write_Reg(0x08, reg_gain  ,2);  else ADS_Write_Reg(0x08, ch_off_reg  ,2);
			if (device_setting.Channels & 0x000002) ADS_Write_Reg(0x09, reg_gain  ,2);  else ADS_Write_Reg(0x09, ch_off_reg  ,2);
			if (device_setting.Channels & 0x000004) ADS_Write_Reg(0x0A, reg_gain  ,2);  else ADS_Write_Reg(0x0A, ch_off_reg  ,2);
			if (device_setting.Channels & 0x000008) ADS_Write_Reg(0x0B, reg_gain  ,2);  else ADS_Write_Reg(0x0B, ch_off_reg  ,2);
			if (device_setting.Channels & 0x000010) ADS_Write_Reg(0x0C, reg_gain  ,2);  else ADS_Write_Reg(0x0C, ch_off_reg  ,2);

			if (device_setting.Channels & 0x000020) ADS_Write_Reg(0x05, reg_gain  ,1);  else ADS_Write_Reg(0x05, ch_off_reg  ,1);
			if (device_setting.Channels & 0x000040) ADS_Write_Reg(0x06, reg_gain  ,1);  else ADS_Write_Reg(0x06, ch_off_reg  ,1);
			if (device_setting.Channels & 0x000080) ADS_Write_Reg(0x07, reg_gain  ,1);  else ADS_Write_Reg(0x07, ch_off_reg  ,1);
			if (device_setting.Channels & 0x000100) ADS_Write_Reg(0x08, reg_gain  ,1);  else ADS_Write_Reg(0x08, ch_off_reg  ,1);
			if (device_setting.Channels & 0x000200) ADS_Write_Reg(0x09, reg_gain  ,1);  else ADS_Write_Reg(0x09, ch_off_reg  ,1);
			if (device_setting.Channels & 0x000400) ADS_Write_Reg(0x0A, reg_gain  ,1);  else ADS_Write_Reg(0x0A, ch_off_reg  ,1);
			if (device_setting.Channels & 0x000800) ADS_Write_Reg(0x0B, reg_gain  ,1);  else ADS_Write_Reg(0x0B, ch_off_reg  ,1);
			if (device_setting.Channels & 0x001000) ADS_Write_Reg(0x0C, reg_gain  ,1);  else ADS_Write_Reg(0x0C, ch_off_reg  ,1);

			if (device_setting.Channels & 0x002000) ADS_Write_Reg(0x05, reg_gain  ,0);  else ADS_Write_Reg(0x05, ch_off_reg  ,0);
			if (device_setting.Channels & 0x004000) ADS_Write_Reg(0x06, reg_gain  ,0);  else ADS_Write_Reg(0x06, ch_off_reg  ,0);
			if (device_setting.Channels & 0x008000) ADS_Write_Reg(0x07, reg_gain  ,0);  else ADS_Write_Reg(0x07, ch_off_reg  ,0);
			if (device_setting.Channels & 0x010000) ADS_Write_Reg(0x08, reg_gain  ,0);  else ADS_Write_Reg(0x08, ch_off_reg  ,0);
			if (device_setting.Channels & 0x020000) ADS_Write_Reg(0x09, reg_gain  ,0);  else ADS_Write_Reg(0x09, ch_off_reg  ,0);
			if (device_setting.Channels & 0x040000) ADS_Write_Reg(0x0A, reg_gain  ,0);  else ADS_Write_Reg(0x0A, ch_off_reg  ,0);
			if (device_setting.Channels & 0x080000) ADS_Write_Reg(0x0B, reg_gain  ,0);  else ADS_Write_Reg(0x0B, ch_off_reg  ,0);
			if (device_setting.Channels & 0x100000) ADS_Write_Reg(0x0C, reg_gain  ,0);  else ADS_Write_Reg(0x0C, ch_off_reg  ,0);


			//adjusting bias derivation
			uint8_t bias_derive[3] = {0x00, 0x00, 0x00};
			if (device_setting.ExtraChannels & 0x01) bias_derive[2] |= 0x01;  else bias_derive[2] &= 0xFE;
			if (device_setting.ExtraChannels & 0x02) bias_derive[2] |= 0x02;  else bias_derive[2] &= 0xFD;
			if (device_setting.ExtraChannels & 0x04) bias_derive[2] |= 0x04;  else bias_derive[2] &= 0xFB;

			if (device_setting.Channels & 0x000001) bias_derive[2] |= 0x08;  else bias_derive[2] &= 0xF7;
			if (device_setting.Channels & 0x000002) bias_derive[2] |= 0x10;  else bias_derive[2] &= 0xEF;
			if (device_setting.Channels & 0x000004) bias_derive[2] |= 0x20;  else bias_derive[2] &= 0xDF;
			if (device_setting.Channels & 0x000008) bias_derive[2] |= 0x40;  else bias_derive[2] &= 0xBF;
			if (device_setting.Channels & 0x000010) bias_derive[2] |= 0x80;  else bias_derive[2] &= 0x7F;

			if (device_setting.Channels & 0x000020) bias_derive[1] |= 0x01;  else bias_derive[1] &= 0xFE;
			if (device_setting.Channels & 0x000040) bias_derive[1] |= 0x02;  else bias_derive[1] &= 0xFD;
			if (device_setting.Channels & 0x000080) bias_derive[1] |= 0x04;  else bias_derive[1] &= 0xFB;
			if (device_setting.Channels & 0x000100) bias_derive[1] |= 0x08;  else bias_derive[1] &= 0xF7;
			if (device_setting.Channels & 0x000200) bias_derive[1] |= 0x10;  else bias_derive[1] &= 0xEF;
			if (device_setting.Channels & 0x000400) bias_derive[1] |= 0x20;  else bias_derive[1] &= 0xDE;
			if (device_setting.Channels & 0x000800) bias_derive[1] |= 0x40;  else bias_derive[1] &= 0xBE;
			if (device_setting.Channels & 0x001000) bias_derive[1] |= 0x80;  else bias_derive[1] &= 0x7E;

			if (device_setting.Channels & 0x002000) bias_derive[0] |= 0x01;  else bias_derive[0] &= 0xFE;
			if (device_setting.Channels & 0x004000) bias_derive[0] |= 0x02;  else bias_derive[0] &= 0xFD;
			if (device_setting.Channels & 0x008000) bias_derive[0] |= 0x04;  else bias_derive[0] &= 0xFB;
			if (device_setting.Channels & 0x010000) bias_derive[0] |= 0x08;  else bias_derive[0] &= 0xF7;
			if (device_setting.Channels & 0x020000) bias_derive[0] |= 0x10;  else bias_derive[0] &= 0xEF;
			if (device_setting.Channels & 0x040000) bias_derive[0] |= 0x20;  else bias_derive[0] &= 0xDE;
			if (device_setting.Channels & 0x080000) bias_derive[0] |= 0x40;  else bias_derive[0] &= 0xBE;
			if (device_setting.Channels & 0x100000) bias_derive[0] |= 0x80;  else bias_derive[0] &= 0x7E;

/////////// for test only (erase that lines for final release)
			for (int k=0; k < 3; k++)
			{
				if (device_setting.derivation == 0x00)  // None
					{
						ADS_BIAS_SIGNAL_DERIVATION(0x00, 0x00, k);
					}
				else if (device_setting.derivation == 0xF0)   // just P  //for refrential montage no need to fucking rout N channels to Bias Deriv.
					{
						ADS_BIAS_SIGNAL_DERIVATION(bias_derive[k], 0x00, k);
					}
				else if (device_setting.derivation == 0x0F)  // Just N
					{
						ADS_BIAS_SIGNAL_DERIVATION(0x00, 0xFF, k);
					}
				else if (device_setting.derivation == 0xFF)  // ALL
					{
						ADS_BIAS_SIGNAL_DERIVATION(bias_derive[k], 0xFF, k);
					}
				else
					ADS_BIAS_SIGNAL_DERIVATION(0x00, 0x00, k);
			}

//test signal seting
				if (device_setting.TestSignal=='T')
				{
						ADS_Write_Reg(0x02 ,0xD0 ,2); //register conf 2  = d0  0b110 1 0000  test internally signal is available
						ADS_Write_Reg(0x05, 0x65 ,2);  //input Test 24g ch1
						SetLED(3,3,3,1);
				}
				else
				{
						ADS_Write_Reg(0x02, 0xC2 , 2); //register conf 2  = d0  0b110 0 0000  test internally signal  not active
						SetLED(3,3,3,0);
				}

	}

}
void ADS_Start_data_full(void)
{
	ADS_Write_Reg(0x14,0xE0, 0);  // gpio0 output and 0
	ADS_Write_Reg(0x14,0xE0, 1);  // gpio0 output and 0
	ADS_Write_Reg(0x14,0xE0, 2);  // gpio0 output and 0
	ADS_Read_data_cont(0);
	ADS_Read_data_cont(1);
	ADS_Read_data_cont(2);
	ADS_Start();
	RDATACflag = 1;
	HAL_GPIO_WritePin(CS0,GPIO_PIN_SET);  //ADS SPI turn off
	HAL_GPIO_WritePin(CS1,GPIO_PIN_SET);  //ADS SPI turn off
	HAL_GPIO_WritePin(CS2,GPIO_PIN_SET);  //ADS SPI turn off
}


void ADS_Stop_data_Full()
{
	RDATACflag = 0;
	ADS_Stop_data_cont(0);
	ADS_Stop_data_cont(1);
	ADS_Stop_data_cont(2);
	ADS_Stop();
	ADS_Write_Reg(0x14,0xF0 ,0);  // gpio0 output and 0
	ADS_Write_Reg(0x14,0xF0 ,1);  // gpio0 output and 0
	ADS_Write_Reg(0x14,0xF0 ,2);  // gpio0 output and 0
}













void ADS_Start_data_one(void)
{
	ADS_Write_Reg(0x14,0xE0,1);  // gpio0 output and 0
	ADS_Start();
	ADS_Read_data_cont(1);
	RDATACflag = 1;
	HAL_GPIO_WritePin(CSN,GPIO_PIN_RESET);  //ADS SPI turn on
}
