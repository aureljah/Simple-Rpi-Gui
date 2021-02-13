#include "Systemcall.hpp"
#include <Windows.h>

namespace syscall
{
	void usleep(int millisec)
	{
		Sleep(millisec);
	}

};
