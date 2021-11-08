#include "Galil.h"

Galil::Galil() {
	g = 0;
	memset(ReadBuffer, 0, sizeof(ReadBuffer)); //clear buffer
	ControlParameters[0] = 0;
	ControlParameters[1] = 0;
	ControlParameters[2] = 0;
	setPoint = 0;
	Functions->GOpen("192.168.1.120 -d", &g);
}

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

Galil::~Galil() {
	Functions->GClose(g);
}

// Write to all 16 bits of digital output, 1 command to the Galil
void Galil::DigitalOutput(uint16_t value){
	memset(ReadBuffer, 0, sizeof(ReadBuffer));
	char command[128] = "";
	int high = value & 0xFF00;
	//std::cout << "high:" << high << std::endl;
	int low = value & 0xFF;
	high >>= 8;
	std::cout << "high:" << high << std::endl;
	sprintf_s(command, "OP %d %d;", low, high);
	std::cout << "low:" << low << std::endl;
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	CheckSuccessfulWrite();
	
}

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
	sprintf_s(command, "OP %d %d;", low, high);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	CheckSuccessfulWrite();
}

// Write single bit to digital outputs. 'bit' specifies which bit
void Galil::DigitalBitOutput(bool val, uint8_t bit) {
	memset(ReadBuffer, 0, sizeof(ReadBuffer));
	char command[64] = "";

	int value = val;
	for (int i = 0; i < bit; i++) {
		value <<= 1;
	}
	int high = value & 0xFF00;
	int low = value & 0xFF;
	sprintf_s(command, "OP %d %d;", low, high);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	CheckSuccessfulWrite();
}

// DIGITAL INPUTS
// Return the 16 bits of input data
// Query the digital inputs of the GALIL, See Galil command library @IN
uint16_t Galil::DigitalInput() {
	int bit = 15;
	uint16_t value = 0;

	if (DigitalBitInput(bit)) {
		value |= 1;
		bit--;
	}
	for (int i = 0; i < 15; i++) {
		if (DigitalBitInput(bit))
			value |= 1;
		value <<= 1;
		bit--;
	}
	return value;
}
// Read either high or low byte, as specified by user in 'bank'
// 0 = low, 1 = high
uint8_t Galil::DigitalByteInput(bool bank) {
	int bit = 7;
	uint8_t value = 0;
	if (bank)
		bit = 15;
	if (DigitalBitInput(bit)) {
		value |= 1;
		bit--;
	}
	for (int i = 0; i < 7; i++) {
		if (DigitalBitInput(bit))
			value |= 1;
		value <<= 1;
		bit--;
	}
	return value;
}
// Read single bit from current digital inputs. Above functions
// may use this function
bool Galil::DigitalBitInput(uint8_t bit) {
	char command[128] = "";

	sprintf_s(command, "MG @IN[%d];", bit);
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), 0);
	int value = atoi(ReadBuffer);
	if (value == 1) 
		return 0;
	else 
		return 1;

}
// Check the string response from the Galil to check that the last 
// command executed correctly. 1 = succesful. NOT AUTOMARKED
bool Galil::CheckSuccessfulWrite() {
	char check = ReadBuffer[0];
	if (check != ':') {
		std::cout << "bad write" << std::endl;
		std::cout << ReadBuffer << std::endl;
		return false;
	}
	std::cout << "good write" << std::endl;
	return true;	
}
// ANALOG FUNCITONS
// Read Analog channel and return voltage			
float Galil::AnalogInput(uint8_t channel) {

	return 0;
}

// Write to any channel of the Galil, send voltages as
// 2 decimal place in the command string
void Galil::AnalogOutput(uint8_t channel, double voltage) {

}
// Configure the range of the input channel with
// the desired range code
void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {

}
// ENCODER / CONTROL FUNCTIONS
// Manually Set the encoder value to zero
void Galil::WriteEncoder() {

}
// Read from Encoder
int Galil::ReadEncoder() {

	return 0;
}
// Set the desired setpoint for control loops, counts or counts/sec
void Galil::setSetPoint(int s) {

}
// Set the proportional gain of the controller used in controlLoop()
void Galil::setKp(double gain) {

}
// Set the integral gain of the controller used in controlLoop()
void Galil::setKi(double gain) {

}
// Set the derivative gain of the controller used in controlLoop()
void Galil::setKd(double gain) {

}

// Operator overload for '<<' operator. So the user can say cout << Galil; This function should print out the
// output of GInfo and GVersion, with two newLines after each.
std::ostream& operator<<(std::ostream& output, Galil& galil) {

	return output;
}