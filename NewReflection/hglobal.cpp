//
#include "hglobal.h"
#include <stdarg.h>
#include <ctype.h>
CHE_NAMESPACE_BEGIN
MessageHandler_Function debug_handler;
void h_message_output(MsgType type, const char *s)
{
	if (debug_handler){
		debug_handler(type, s);
	}
	else{
		fprintf_s(stderr, "%s\n", s);
		fflush(stderr);
	}
}

void _Debug(const char *format, const char *func_name, ...)
{
	va_list args;
	va_start(args, format);

	try {
		h_message(DebugMsg, format, func_name, args);
	}
	catch (...){/*do nothing...*/ }
}
void _Warning(const char *format, const char *func_name, ...)
{
	va_list args;
	va_start(args, format);

	try {
		h_message(WarningMsg, format, func_name, args);
	}
	catch (...){/*do nothing...*/ }
}
void _Critical(const char *format, const char *func_name, ...)
{
	va_list args;
	va_start(args, format);

	try {
		h_message(CriticalMsg, format, func_name, args);
	}
	catch (...){/*do nothing...*/ }
}
void _Fatal(const char *format, const char *func_name, ...)
{
	va_list args;
	va_start(args, format);

	try {
		h_message(FatalMsg, format, func_name, args);
	}
	catch (...){/*do nothing...*/ }
}

void h_message(MsgType type, const char *format, const char *func_name, va_list ap)
{
	char buf[4096];
	char *p = buf;
	
	p += _vsnprintf_s(p, _countof(buf), _TRUNCATE, format, ap);
	va_end(ap);

	try {
		while (p > buf && isspace(p[-1])){
			*--p = '\0';
			*p++ = '\r';
			*p++ = '\n';
			*p = '\0';
		}
	}
	catch (...){
		buf[4095] = 0;
	}

	switch (type)
	{
	case che::DebugMsg:
		fprintf_s(stderr, "Debug:");
		break;
	case che::WarningMsg:
		fprintf_s(stderr, "Warning:");
		break;
	case che::FatalMsg:
		fprintf_s(stderr, "Fatal:");
		break;
	case che::CriticalMsg:
		fprintf_s(stderr, "Critical:");
		break;
	default:
		break;
	}
	fprintf_s(stderr, "%s. FROM %s\n", buf, func_name);
	fflush(stderr);
}

extern void h_printf(const char *format)
{
	while (*format){
		if (*format == '%')	throw std::invalid_argument("The corresponding parameter miss % placeholders ");
		putchar(*format++);
	}
}

CHE_NAMESPACE_END
