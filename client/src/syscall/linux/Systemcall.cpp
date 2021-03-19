#include "Systemcall.hpp"
#include <unistd.h>

namespace systemcall
{
    /* Sleep/wait for (int) millisecond */
	void sys_usleep(int millisec)
	{
		if (millisec >= 1000)
		{
			int sec = millisec / 1000;
			sleep(sec);
		}
		else
		{
			usleep(millisec * 1000);
		}
	}

}
