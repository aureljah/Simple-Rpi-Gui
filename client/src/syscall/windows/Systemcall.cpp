#include "Systemcall.hpp"
#include <Windows.h>

namespace systemcall
{
	void sys_usleep(int millisec)
	{
		Sleep(millisec);
	}

};
