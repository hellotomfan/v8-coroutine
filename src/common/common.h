#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H


#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <stdarg.h>
#include <stdio.h>


typedef int8_t 		int8;
typedef int16_t 	int16;
typedef int32_t		int32;
typedef int64_t		int64;
typedef uint8_t 	uint8;
typedef uint16_t 	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;


typedef int SOCKET;
#define INVALID_SOCKET -1


enum 
{
	Socket_Error,
	Socket_Closed,
	Buffer_Read,
	Buffer_Write,
};

int GetCmdline(char *out, int n);
int GetCurrentTimeFmtStr(char *out, int n, const char *fmt);
bool IsSameDay(time_t t1, time_t t2);
const char* GetBasename(const char *file);
void daemonize(const char *cmd);
template <int size>
const char* CStringFormat(const char *fmt, ...)
{
	static char str[size] = {0};

	va_list args;
	va_start(args, fmt);

	int n = vsnprintf(str, size - 1, fmt, args);
	str[n] = 0;

	return str;
}



class CmdArgs
{
	public:
		CmdArgs(const char *_cmd): cmd(_cmd)
		{
		}
		void AddArg(const char *arg)
		{
			args.push_back(arg);
		}
		const char **GetCmdArgs()
		{
			static std::vector<const char*> t;
			t.clear();
			t.push_back(cmd.c_str());

			for (std::vector<std::string>::iterator iter = args.begin(); iter != args.end(); ++iter)
			{
				t.push_back(iter->c_str());
			}
			t.push_back(0);
			return &t[0];
		}
		const char *GetCmd()
		{
			return cmd.c_str();
		}

	private:
		std::string cmd;
		std::vector<std::string> args;
};


template <class Class>
class RefCountCallback
{
	public:
		typedef void (Class::*Method)();
		RefCountCallback(Class *_obj, Method _mem): obj(_obj), mem(_mem), ref(0) {}
		void Ref()      	 	{ ref++; }
		void Unref()    	 	{ if (--ref == 0) { (obj->*mem)(); } } 
		void SetRef(int _ref) 	{ ref = _ref; }
		void operator ()() 		{ return (obj->*mem)(); }
		int GetRef() 			{ return ref; }
	private:
		int ref;
		Class *obj;
		Method mem;

};


#endif
