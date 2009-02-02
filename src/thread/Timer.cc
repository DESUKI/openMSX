// $Id$

#include "Timer.hh"
#include "probed_defs.hh"
#ifdef HAVE_GETTIMEOFDAY
#include <sys/time.h>
#include <ctime>
#endif
#ifdef HAVE_USLEEP
#include <unistdp.hh>
#endif
#ifdef WIN32
#include <windows.h>
#endif
#include <SDL.h>

namespace openmsx {

namespace Timer {

static inline unsigned long long getSDLTicks()
{
	return static_cast<unsigned long long>(SDL_GetTicks()) * 1000;
}

unsigned long long getTime()
{
/* QueryPerformanceCounter() has problems on modern CPUs,
 *  - on dual core CPUs time can ge backwards (a bit) when your process
 *    get scheduled on the other core
 *  - the resolution of the timer can vary on CPUs that can change its
 *    clock frequency (for power managment)
#ifdef WIN32
	static LONGLONG hfFrequency = 0;

	LARGE_INTEGER li;
	if (!hfFrequency) {
		if (QueryPerformanceFrequency(&li)) {
			hfFrequency = li.QuadPart;
		} else {
			return getSDLTicks();
		}
	}
	QueryPerformanceCounter(&li);

	// Assumes that the timer never wraps. The mask is just to
	// ensure that the multiplication doesn't wrap.
	return (li.QuadPart & ((long long)-1 >> 20)) * 1000000 / hfFrequency;
*/
#ifdef HAVE_GETTIMEOFDAY
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return static_cast<unsigned long long>(tv.tv_sec) * 1000000 +
	       static_cast<unsigned long long>(tv.tv_usec);
#else
	return getSDLTicks();
#endif
}

/*#ifdef WIN32
static void CALLBACK timerCallback(unsigned int,
                                   unsigned int,
                                   unsigned long eventHandle,
                                   unsigned long,
                                   unsigned long)
{
    SetEvent((HANDLE)eventHandle);
}
#endif*/

void sleep(unsigned long long us)
{
/*#ifdef WIN32
	us /= 1000;
	if (us > 0) {
		static HANDLE timerEvent = NULL;
		if (timerEvent == NULL) {
			timerEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		}
		UINT id = timeSetEvent(us, 1, timerCallback, (DWORD)timerEvent,
		                       TIME_ONESHOT);
		WaitForSingleObject(timerEvent, INFINITE);
		timeKillEvent(id);
	}
*/
#ifdef HAVE_USLEEP
	usleep(us);
#else
	SDL_Delay((Uint32)us / 1000);
#endif
}

} // namespace Timer

} // namespace openmsx
