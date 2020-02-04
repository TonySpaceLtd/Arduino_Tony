#include "tony-max11301.h"
#include "Arduino.h"

int16_t DAC_Compensate = 0;  //---- For adjust DAC Output


bool MAX11301::Config_deviceControl()
{
	//------ Clear GPO Ouput -------//
	Wire.beginTransmission(ADDRMAX11301);
    Wire.write(GPO_port_0_to_15);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.endTransmission();
	delay(40);

	Wire.beginTransmission(ADDRMAX11301);
    Wire.write(GPO_port_16_to_19);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.endTransmission();
	delay(40);
	//------------------------------//

	uint16_t regData = 0;

	regData = Reset;
	regData = (regData<<1)+BRST;
	regData = (regData<<1)+LPEN;
	regData = (regData<<1)+RS_CANCEL;
	regData = (regData<<1)+TMPPER;
	regData = (regData<<3)+TmPCTL;
	regData = (regData<<1)+THSHDN;
	regData = (regData<<1)+DACREF;
	regData = (regData<<2)+ADCconv;
	regData = (regData<<2)+DACCTL;
	regData = (regData<<2)+ADCCTL;

	uint16_t Data_1 = regData >> 8;
    uint8_t Data_2 = regData;
    Wire.beginTransmission(ADDRMAX11301);
    Wire.write(0x10);
    Wire.write(Data_1);
    Wire.write(Data_2);
    Wire.endTransmission();
	delay(40);

	//------ Adjust DAC Compensate Value -------//
	Basic_Config_Port_For_DACADC(0, 1024);  // Config Port 0 to DAC with monitoring  (1024 = 2.5V)
	delay(100);
	uint16_t dataADC = 0;
	bool max_ok = true;
	for(uint8_t i=0; i<200; i++)
	{
		float Voltage = 0;
		dataADC = readADC(0);  // Read ADC from port 0 (DAC Output)
		Voltage = float(10.0/4095)*dataADC;
		if(Voltage < 2.48)
		{
			DAC_Compensate++;
		}
		else if(Voltage > 2.50)
		{
			DAC_Compensate--;
		}
		else if(Voltage > 2.48 && Voltage < 2.50)
		{
			break;
		}
		writeDAC(0, 1024);  // 1024 = 2.5V
		delay(10);
		if(i >= 199)
		{
			max_ok = false;
		}
	}
	
	//------ End Adjust Compensate Value -----//
	Basic_Config_Port_For_GPI(0, 3276);
	return max_ok;
}

void MAX11301::Advance_Config_Port(uint8_t Port, uint8_t Mode, bool AVR_INV, uint8_t RANGE, uint8_t SAMPLES, uint8_t ASSOCIATED)
{
	uint16_t regData = 0;
	
	Port = constrain(Port, 0, 19);

	regData = Mode;
	regData = (regData<<1)+AVR_INV;
	regData = (regData<<3)+RANGE;
	regData = (regData<<3)+SAMPLES;
	regData = (regData<<5)+ASSOCIATED;

	uint16_t Data_1 = regData >> 8;
    uint8_t Data_2 = regData;
    Wire.beginTransmission(ADDRMAX11301);
    Wire.write(Config_Port_00+Port);
    Wire.write(Data_1);
    Wire.write(Data_2);
    Wire.endTransmission();
	delay(40);
}

void MAX11301::Basic_Config_Port(uint8_t Port, uint16_t Basic_Con)
{
	uint16_t regData = Basic_Con;
	uint16_t Data_1 = regData >> 8;
    uint8_t Data_2 = regData;
	
	Port = constrain(Port, 0, 19);
	
    Wire.beginTransmission(ADDRMAX11301);
    Wire.write(Config_Port_00+Port);
    Wire.write(Data_1);
    Wire.write(Data_2);
    Wire.endTransmission();
	delay(40);
}

void MAX11301::Basic_Config_Port_For_DACADC(uint8_t Port, int16_t Output)
{
	Port = constrain(Port, 0, 19);

	Output = Output+DAC_Compensate;
	Output = constrain(Output, 0, 4095);
	uint16_t regData1 = DACADC;
	uint16_t regData2 = Output;
	uint16_t Data_1 = regData1 >> 8;
    uint8_t Data_2 = regData1;
	uint16_t Data_3 = regData2 >> 8;
    uint8_t Data_4 = regData2;
    Wire.beginTransmission(ADDRMAX11301);
    Wire.write(Config_Port_00+Port);
    Wire.write(Data_1);
    Wire.write(Data_2);
    Wire.endTransmission();
	delay(40);

	Wire.beginTransmission(ADDRMAX11301);
    Wire.write(Config_Port_00+Port+64);
    Wire.write(Data_3);
    Wire.write(Data_4);
    Wire.endTransmission();
	delay(40);
}

void MAX11301::Basic_Config_Port_For_GPI(uint8_t Port, uint16_t Threshold)
{
	Port = constrain(Port, 0, 19);
	Threshold = constrain(Threshold, 0, 0x0fff);

	uint16_t regData1 = GPItype;
	uint16_t regData2 = Threshold;
	uint16_t Data_1 = regData1 >> 8;
    uint8_t Data_2 = regData1;
	uint16_t Data_3 = regData2 >> 8;
    uint8_t Data_4 = regData2;
    Wire.beginTransmission(ADDRMAX11301);
    Wire.write(Config_Port_00+Port);
    Wire.write(Data_1);
    Wire.write(Data_2);
    Wire.endTransmission();
	delay(40);

	Wire.beginTransmission(ADDRMAX11301);
    Wire.write(Config_Port_00+Port+64);
    Wire.write(Data_3);
    Wire.write(Data_4);
    Wire.endTransmission();
	delay(40);
}

void MAX11301::Basic_Config_Port_For_GPO(uint8_t Port, int16_t Output)
{
	Port = constrain(Port, 0, 19);

	Output = Output+DAC_Compensate;
	Output = constrain(Output, 0, 4095);
	
	uint16_t regData1 = GPOtype;
	uint16_t regData2 = Output;
	uint16_t Data_1 = regData1 >> 8;
    uint8_t Data_2 = regData1;
	uint16_t Data_3 = regData2 >> 8;
    uint8_t Data_4 = regData2;
    Wire.beginTransmission(ADDRMAX11301);
    Wire.write(Config_Port_00+Port);
    Wire.write(Data_1);
    Wire.write(Data_2);
    Wire.endTransmission();
	delay(40);

	Wire.beginTransmission(ADDRMAX11301);
    Wire.write(Config_Port_00+Port+64);
    Wire.write(Data_3);
    Wire.write(Data_4);
    Wire.endTransmission();
	delay(40);
}

uint16_t MAX11301::readADC(uint8_t Port)
{

	Port = constrain(Port, 0, 19);
	
	uint8_t getData[2];
	uint16_t dataADC = 0;
	uint8_t count  = 0;

	Wire.beginTransmission(ADDRMAX11301);
	Wire.write(ADC_data_port_00+Port);
	Wire.endTransmission();

	Wire.requestFrom(ADDRMAX11301,2);
	while(Wire.available())
	{
		getData[count] = Wire.read();
		count++;
	}

  dataADC = (getData[0] << 8)+getData[1];
  
  
  if(dataADC >= 5300)
  {
	  dataADC = 0;
	  // Serial.println("ADC Fail !");
	  // Serial.println();
  }
  else
  {
	return(dataADC);
  }
}

void MAX11301::writeDAC(uint8_t Port, int16_t Output)
{
	int16_t Value = Output+DAC_Compensate;
	Value = constrain(Value, 0, 4095);
    uint16_t Data_1 = Value >> 8;
    uint8_t Data_2 = Value;
	
	Port = constrain(Port, 0, 19);

    Wire.beginTransmission(ADDRMAX11301);
    Wire.write(DAC_data_port_00+Port);
    Wire.write(Data_1);
    Wire.write(Data_2);
    Wire.endTransmission();
}

void MAX11301::writeGPO(uint8_t Port, bool Output)
{
	uint8_t addrPort = 0;
	uint8_t getData[2];
	uint16_t readGPO = 0;
	uint16_t checkGPO = 0;
	uint16_t shiftBit = 1;
	uint8_t count  = 0;
	
	Port = constrain(Port, 0, 19);

    if(Port <= 15)
	{
		addrPort = GPO_port_0_to_15;
	}
	else
	{
		addrPort = GPO_port_16_to_19;
	}
	
	Wire.beginTransmission(ADDRMAX11301);
	Wire.write(addrPort);
	Wire.endTransmission();

	Wire.requestFrom(ADDRMAX11301,2);
	while(Wire.available())
	{
		getData[count] = Wire.read();
		count++;
	}

	readGPO = (getData[0] << 8)+getData[1];

	checkGPO = readGPO;

	if(addrPort == GPO_port_0_to_15)
	{
		shiftBit = shiftBit << Port;
		checkGPO = checkGPO << 15-Port;
		checkGPO = checkGPO >> 15;
	}
	else if(addrPort == GPO_port_16_to_19)
	{
		shiftBit = shiftBit << Port-16;
		checkGPO = checkGPO << 12;
		checkGPO = checkGPO << 19-Port;
		checkGPO = checkGPO >> (15);
	}

	if(checkGPO != Output)
	{
		if(Output == 1)
		{
			readGPO = readGPO | shiftBit;
		}
		else
		{
			readGPO = readGPO ^ shiftBit;
		}
	}

	uint16_t Data_1 = readGPO >> 8;
    uint8_t Data_2 = readGPO;
    Wire.beginTransmission(ADDRMAX11301);
    Wire.write(addrPort);
    Wire.write(Data_1);
    Wire.write(Data_2);
    Wire.endTransmission();
}

void MAX11301::write_speedGPO(uint8_t Port, uint16_t Value)
{
	uint8_t addrPort = 0;
    uint16_t Data_1 = Value >> 8;
    uint8_t Data_2 = Value;
	
	Port = constrain(Port, 0, 19);

    if(Port <= 15)
	{
		addrPort = GPO_port_0_to_15;
	}
	else
	{
		addrPort = GPO_port_16_to_19;
	}
	
    Wire.beginTransmission(ADDRMAX11301);
    Wire.write(addrPort);
    Wire.write(Data_1);
    Wire.write(Data_2);
    Wire.endTransmission();
}


bool MAX11301::readGPI(uint8_t Port)
{
	uint8_t addrPort = 0;
	uint8_t getData[2];
	uint16_t dataGPI = 0;
	uint8_t count  = 0;
	
	Port = constrain(Port, 0, 19);

    if(Port <= 15)
	{
		addrPort = GPI_port_0_to_15;
	}
	else
	{
		addrPort = GPI_port_16_to_19;
	}

	Wire.beginTransmission(ADDRMAX11301);
	Wire.write(addrPort);
	Wire.endTransmission();

	Wire.requestFrom(ADDRMAX11301,2);
	while(Wire.available())
	{
		getData[count] = Wire.read();
		count++;
	}

	dataGPI = (getData[0] << 8)+getData[1];

	if(addrPort == GPI_port_0_to_15)
	{
		dataGPI = dataGPI << 15-Port;
		dataGPI = dataGPI >> 15;
	}
	else if(addrPort == GPI_port_16_to_19)
	{
		dataGPI = dataGPI << 12;
		dataGPI = dataGPI << 19-Port;
		dataGPI = dataGPI >> (15);
	}
	return(dataGPI);
}

bool MAX11301::readGPO(uint8_t Port)
{
	uint8_t addrPort = 0;
	uint8_t getData[2];
	uint16_t dataGPO = 0;
	uint8_t count  = 0;
	
	Port = constrain(Port, 0, 19);

    if(Port <= 15)
	{
		addrPort = GPO_port_0_to_15;
	}
	else
	{
		addrPort = GPO_port_16_to_19;
	}

	Wire.beginTransmission(ADDRMAX11301);
	Wire.write(addrPort);
	Wire.endTransmission();

	Wire.requestFrom(ADDRMAX11301,2);
	while(Wire.available())
	{
		getData[count] = Wire.read();
		count++;
	}

	dataGPO = (getData[0] << 8)+getData[1];

	if(addrPort == GPI_port_0_to_15)
	{
		dataGPO = dataGPO << 15-Port;
		dataGPO = dataGPO >> 15;
	}
	else if(addrPort == GPI_port_16_to_19)
	{
		dataGPO = dataGPO << 12;
		dataGPO = dataGPO << 19-Port;
		dataGPO = dataGPO >> (15);
	}
	return(dataGPO);
}




