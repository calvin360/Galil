#include "Galil.h"
#using <System.dll>
#include <Windows.h>
#include <conio.h>
#include<gclibo.h>

#include<gclib.h>
#include<gclib_errors.h>
#include<gclib_record.h>
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<string>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;


int main() {
	EmbeddedFunctions f;
	std::cout << "connecting" << std::endl;
	Galil galil(&f,"192.168.0.120 -d");
	std::cout << "connected" << std::endl;

	while (!_kbhit()) {
		galil.DigitalOutput(0xfe);
		//std::cout << "high" << std::endl;
		Sleep(1000);
		galil.DigitalOutput(0);
		//std::cout << "low" << std::endl;
		Sleep(1000);
	}
	return 0;
}