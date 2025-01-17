﻿#ifndef _CELL_LOG_HPP_
#define _CELL_LOG_HPP_

#include"CELL.hpp"
#include"CELLTask.hpp"
#include<ctime>
class CELLLog
{
#if _DEBUG
	#ifndef CELLLog_DEBUG
		#define CELLLog_DEBUG(...) CELLLog::Debug(__VA_ARGS__)
	#endif
#else
	#ifndef CELLLog_DEBUG
		#define CELLLog_DEBUG(...)
	#endif
#endif // _DEBUG

	//Info
#define CELLLog_INFO(...) CELLLog::Debug(__VA_ARGS__)
	//Warring
#define CELLLog_WARNIG(...) CELLLog::Debug(__VA_ARGS__)
	//Error
#define CELLLog_ERROR(...) CELLLog::Debug(__VA_ARGS__)
private:
	CELLLog()
	{
		_taskServer.Start();
	}

	~CELLLog()
	{
		_taskServer.Close();
		if (_logFile)
		{
			Info("CELLLog fclose(_logFile)\n");
			fclose(_logFile);
			_logFile = nullptr;
		}
	}
public:
	static CELLLog& Instance()
	{
		static  CELLLog sLog;
		return sLog;
	}

	void setLogPath(const char* logName, const char* mode)
	{
		if (_logFile)
		{
			Info("CELLLog::setLogPath _logFile != nullptr\n");
			fclose(_logFile);
			_logFile = nullptr;
		}

		auto t = system_clock::now();
		auto tNow = system_clock::to_time_t(t);
		std::tm* now = std::localtime(&tNow);
		sprintf(logPath, "%s[%d-%d-%d_%d-%d-%d].txt", logName ,now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
		_logFile = fopen(logPath, mode);
		if (_logFile)
		{
			Info("CELLLog::setLogPath success,<%s,%s>\n", logPath, mode);
		}
		else {
			Info("CELLLog::setLogPath failed,<%s,%s>\n", logPath, mode);
		}
	}

	static void Error(const char* pStr)
	{
		Error("%s", pStr);
	}
	template<typename ...Args>
	static void Error(const char* pformat, Args ... args)  //可变参数
	{
		Echo("Error", pformat, args...);
	}

	static void Debug(const char* pStr)
	{
		Debug("%s", pStr);
	}
	template<typename ...Args>
	static void Debug(const char* pformat, Args ... args)  //可变参数
	{
		Echo("Debug", pformat, args...);
	}

	static void Warning(const char* pStr)
	{
		Warning("%s", pStr);
	}
	template<typename ...Args>
	static void Warning(const char* pformat, Args ... args)  //可变参数
	{
		Echo("Warning", pformat, args...);
	}

	static void Info(const char* pStr)  //可变参数
	{
		Info("%s", pStr);
	}
	template<typename ...Args>
	static void Info(const char* pformat, Args ... args)  //可变参数
	{
		Echo("Info",pformat, args...);
	}


	template<typename ...Args>
	static void Echo(const char* type,const char* pformat, Args ... args)  //可变参数
	{
		CELLLog* pLog = &Instance();
		pLog->_taskServer.addTask([=]() {
			if (pLog->_logFile)
			{
				auto t = system_clock::now();
				auto tNow = system_clock::to_time_t(t);
				//fprintf(pLog->_logFile, "%s", ctime(&tNow));
				std::tm* now = std::localtime(&tNow);
				fprintf(pLog->_logFile, "%s",type);
				fprintf(pLog->_logFile, "[%d-%d-%d %d:%d:%d]", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
				fprintf(pLog->_logFile, pformat, args...);
				fflush(pLog->_logFile);
			}
			printf(pformat, args...);
		});
	}
private:
	FILE* _logFile = nullptr;
	CELLTaskServer _taskServer;
	char logPath[256] = {};
};

#endif // !_CELL_LOG_HPP_
