/*
 * CrossPlatform.hpp
 *
 *  Created on: Jan 11, 2021
 *      Author: andrewrubinstein
 */

#ifndef CROSSPLATFORM_HPP_
#define CROSSPLATFORM_HPP_
#include <inttypes.h>
#include <iostream>
#include <thread>

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR '\\'
#define libreadline 0
#else
#define PATH_SEPARATOR '/'
#if defined(libreadline)
#define libreadline 1
#include <readline/readline.h>
#include <readline/history.h>
#endif
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
#if libreadline == 0
static void readLine(std::string &line, std::string prompt)
{
    std::cout<<prompt;
    getLine(std::cin, line);
}
static void getLine(std::istream &ascal_cin, std::string &line)
{
    getline(ascal_cin, line);
}
#else

static void readLine(std::string &line, std::string prompt)
{
    char *readLineBuffer = nullptr;
    if(prompt.size())
        readLineBuffer = readline(prompt.c_str());
    else
        readLineBuffer = readline("");

    auto bufCleaner = std::make_unique<char*>(readLineBuffer);
    if(readLineBuffer)
    {
        line = readLineBuffer;
        if (line.size() > 0) {
          add_history(readLineBuffer);
        }
    }
}
static void getLine(std::istream &ascal_cin, std::string &line)
{
    //if you get compiler errors you can not use readLine
    if(ascal_cin.rdbuf() != std::cin.rdbuf())
        getline(ascal_cin, line);
    else
        readLine(line, "");
}
#endif

};

#endif /* CROSSPLATFORM_HPP_ */
