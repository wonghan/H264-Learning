#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include <fstream>

#define TRACE_CONFIG_CONSOLE 1	// 此配置项表示是否允许向控制台中输出日志
#define TRACE_CONFIG_LOGOUT 1	// 此配置项表示是否允许向日志文件中输出日志

extern std::ofstream g_traceFile; // extern表示变量在其他文件中定义了，去其他文件中找

#endif