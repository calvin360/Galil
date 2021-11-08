#include "Galil.h"
#using <System.dll>
#include <Windows.h>
#include <conio.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

int main(void) {
	EmbeddedFunctions f;
	Galil* galil = new Galil(&f, "192.168.1.120 -d");
	
	while (!kbhit()) {
		galil->DigitalOutput(16);
		Sleep(1000);
		galil->DigitalOutput(0);
	}

	delete galil;
	return 0;
}