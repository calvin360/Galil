#include "Galil.h"

//done
Galil::Galil() {
	g = 0;
	memset(ReadBuffer, 0, sizeof(ReadBuffer)); //clear buffer
	ControlParameters[0] = 0;
	ControlParameters[1] = 0;
	ControlParameters[2] = 0;
	setPoint = 0;
	Functions->GOpen("192.168.1.120 -d", &g);
}

//done
Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) {
	g = 0;
	Functions = Funcs;
	memset(ReadBuffer, 0, sizeof(ReadBuffer)); //clear buffer
	ControlParameters[0] = 0;
	ControlParameters[1] = 0;
	ControlParameters[2] = 0;
	setPoint = 0;
	Functions->GOpen(address, &g);
}

//done
Galil::~Galil() {
	Functions->GClose(g);
}

//------------------------------------------------------------------------------//
//done
// DIGITAL OUTPUTS
// Write to all 16 bits of digital output, 1 command to the Galil
void Galil::DigitalOutput(uint16_t value){
	memset(ReadBuffer, 0, sizeof(ReadBuffer));
	char command[128] = "";
	int high = value & 0xFF00;
	if (high > 0xFF) {
		int temp = ~high;
		sprintf_s(command, "IQ %d;", temp);
		Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	}
	int low = value & 0xFF;
	high >>= 8;
	sprintf_s(command, "OP %d, %d;", low, high);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	CheckSuccessfulWrite();
}

//done
// Write to one byte, either high or low byte, as specified by user in 'bank'
// 0 = low, 1 = high
void Galil::DigitalByteOutput(bool bank, uint8_t value) {
	memset(ReadBuffer, 0, sizeof(ReadBuffer));
	char command[64] = "";
	int high = 0;
	int low = 0;

	if (bank)
		high = value;
	else
		low = value;
	sprintf_s(command, "OP %d, %d;", low, high);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	CheckSuccessfulWrite();
}

//done
// Write single bit to digital outputs. 'bit' specifies which bit
void Galil::DigitalBitOutput(bool val, uint8_t bit) {
	memset(ReadBuffer, 0, sizeof(ReadBuffer));
	char command[64] = "";
	int value = val;

	for (int i = 0; i < bit; i++) {
		value <<= 1;
	}
	int high = value & 0xFF00;
	high >>= 8;
	int low = value & 0xFF;
	sprintf_s(command, "OP %d, %d;", low, high);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	CheckSuccessfulWrite();
}

//--------------------------------------------------------------------------//
// DIGITAL INPUTS
//done
// Return the 16 bits of input data
// Query the digital inputs of the GALIL, See Galil command library @IN
uint16_t Galil::DigitalInput() {
	uint8_t bit = 15;
	uint16_t value = 0;
	for (int i = 0; i < 15; i++) {
		if (DigitalBitInput(bit))
			value |= 1;
		value <<= 1;
		bit--;
	}
	//for lowest bit
	if (DigitalBitInput(0)) 
		value |= 1;
	
	return value;
}

//done
// Read either high or low byte, as specified by user in 'bank'
// 0 = low, 1 = high
uint8_t Galil::DigitalByteInput(bool bank) {
	uint8_t bit = 7;
	uint8_t value = 0;
	if (bank)
		bit = 15;
	for (int i = 0; i < 7; i++) {
		if (DigitalBitInput(bit))
			value |= 1;
		value <<= 1;
		bit--;
	}
	if (DigitalBitInput(0))
		value |= 1;

	return value;
}

//done
// Read single bit from current digital inputs. Above functions
// may use this function
bool Galil::DigitalBitInput(uint8_t bit) {
	char command[128] = "";

	sprintf_s(command, "MG @IN[%d];", bit);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	int value = atoi(ReadBuffer);
	if (value == 1) 
		return 1;
	else 
		return 0;

}

//done
// Check the string response from the Galil to check that the last 
// command executed correctly. 1 = succesful. NOT AUTOMARKED
bool Galil::CheckSuccessfulWrite() {
	char check = ReadBuffer[0];
	//std::cout << ReadBuffer << std::endl;
	if (check != ':') {
		std::cout << "bad write" << std::endl;
		std::cout << ReadBuffer << std::endl;
		return false;
	}
	std::cout << "good write" << std::endl;
	return true;	
}

//--------------------------------------------------------------------------//
// ANALOG FUNCITONS
// done
// Read Analog channel and return voltage			
float Galil::AnalogInput(uint8_t channel) {
	memset(ReadBuffer, 0, sizeof(ReadBuffer));
	char command[64] = "";

	sprintf_s(command, "MG @AN[%d];", channel);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	return (float)atof(ReadBuffer);
}

//done
// Write to any channel of the Galil, send voltages as
// 2 decimal place in the command string
void Galil::AnalogOutput(uint8_t channel, double voltage) {
	memset(ReadBuffer, 0, sizeof(ReadBuffer));
	char command[64] = "";

	sprintf_s(command, "AO %d, %.2lf;", channel, voltage);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	CheckSuccessfulWrite();

}

//done
// Configure the range of the input channel with
// the desired range code
void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {
	memset(ReadBuffer, 0, sizeof(ReadBuffer));
	char command[64] = "";

	sprintf_s(command, "AQ %d, %d;", channel, range);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	CheckSuccessfulWrite();
}

//done
// ENCODER / CONTROL FUNCTIONS
// Manually Set the encoder value to zero
void Galil::WriteEncoder() {
	memset(ReadBuffer, 0, sizeof(ReadBuffer));	// clear ReadBuffer
	char command[64] = "WE 0, 0;";

	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	CheckSuccessfulWrite();
}

//done
// Read from Encoder
int Galil::ReadEncoder() {
	char command[64] = "QE 0;";

	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	return atoi(ReadBuffer);
}

//done
// Set the desired setpoint for control loops, counts or counts/sec
void Galil::setSetPoint(int s) {
	setPoint = s;
}

//done
// Set the proportional gain of the controller used in controlLoop()
void Galil::setKp(double gain) {
	ControlParameters[0] = gain;
}

//done
// Set the integral gain of the controller used in controlLoop()
void Galil::setKi(double gain) {
	ControlParameters[1] = gain;
}

//done
// Set the derivative gain of the controller used in controlLoop()
void Galil::setKd(double gain) {
	ControlParameters[2] = gain;
}

//done
// Operator overload for '<<' operator. So the user can say cout << Galil; This function should print out the
// output of GInfo and GVersion, with two newLines after each.
std::ostream& operator<<(std::ostream& output, Galil& galil) {
	char info[64] = "";
	char version[64] = "";

	galil.Functions->GInfo(galil.g, info, sizeof(info));
	galil.Functions->GVersion(version, sizeof(version));

	output << info << std::endl;
	output << "\n" << std::endl;
	output << version << std::endl;
	output << "\n" << std::endl;

	return output;
}