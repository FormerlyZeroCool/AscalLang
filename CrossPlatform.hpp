/*
 * CrossPlatform.hpp
 *
 *  Created on: Jan 11, 2021
 *      Author: andrewrubinstein
 */

#ifndef CROSSPLATFORM_HPP_
#define CROSSPLATFORM_HPP_
#include <inttypes.h>
#include <thread>

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif
class CrossPlatform {
public:
	CrossPlatform() {}
#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
static void usleep(__int64 usec)
{
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}
#else
static void usleep(uint64_t usec){
	std::this_thread::sleep_for(std::chrono::microseconds(usec));
}
#endif
//Converts an ascal style file path with . as the path separator to a native filepath in windows/unix
static std::string convertFilePath(std::string fp)
{
	uint32_t c = 0;
    for(; c<fp.size(); c++)
    {
        if(fp[c] == '.')
            fp[c] = (PATH_SEPARATOR);
    }
    while(fp[--c] != '.' && c) {}
    fp[c] = fp[c]*(!c) + PATH_SEPARATOR*(c!=0);
    return fp;
}
};
#endif /* CROSSPLATFORM_HPP_ */
