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
	Galil g(&f,"192.168.0.120 -d");
	std::cout << "connected" << std::endl;

	while (!_kbhit()) {
		g.DigitalOutput(2356);
		double x=g.DigitalByteInput(0);
		std::cout << "high: " << x << std::endl;
		Sleep(1000);
		g.DigitalOutput(0);
		x = g.DigitalByteInput(0);
		std::cout << "low: " << x << std::endl;
		Sleep(1000);
	}
	return 0;
}