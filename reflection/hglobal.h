/********************************************************************
	Copyright (C) 2014 CUIT·CHE.
	All rights reserved.
	Contact: CHE (hejun_753@163.com)

	created:	2014/02/08
	created:	8:2:2014 19:40
	filename: 	e:\documents\visual studio 2013\Projects\HSystem\HSystem\hglobal.h
	file path:	e:\documents\visual studio 2013\Projects\HSystem\HSystem
	file base:	hglobal
	file ext:	h
	author:		Jq

	purpose:	hglobal.h 预处理，版本信息，宏定义
	*********************************************************************/
//
#ifndef HGLOBAL_H_
#define HGLOBAL_H_
#include "HObjectdefs_conn.h"
#include <stdio.h>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <cassert>


#ifdef CHE_NAMESPACE_BEGIN
#undef CHE_NAMESPACE_BEGIN
#define CHE_NAMESPACE_BEGIN namespace che{
#else
#define CHE_NAMESPACE_BEGIN namespace che{
#endif // CHE_NAMESPACE_BEGIN

#ifdef CHE_NAMESPACE_END
#undef CHE_NAMESPACE_END
#define CHE_NAMESPACE_END }
#else
#define CHE_NAMESPACE_END }
#endif // CHE_NAMESPACE_END

CHE_NAMESPACE_BEGIN
#ifndef CHE_DISABLE_COPY
#define CHE_DISABLE_COPY(Class) \
	Class(const Class&) = delete;\
	Class& operator=(const Class&)=delete;
#endif // !CHE_DISABLE_COPY

#ifndef CHE_DISABLE_COPY_RREF
#define CHE_DISABLE_COPY_RREF(Class) \
	Class(Class&&) = delete;\
	Class& operator=(Class&&)=delete;
#endif // !CHE_DISABLE_COPY

//类数据类定义宏
#define CHE_PRIVATE_DEFINED(Class) struct Class##Private

//声明类的数据对象获取方式
#define CHE_PRIVATE_DECLARE(Class)\
inline Class##Private* d_func(){return d_ptr.get();}\
inline const Class##Private* d_func()const { return d_ptr.get(); }\

//获得此类数据结构指针
#define Q_D(Class) Class##Private* d=d_func()
#define Q_DC(Class) const Class##Private* d=d_func()

#define __Thread_Current_Flag
//mutex便捷锁
#define __locker(_locker) std::lock_guard<mutex> _locker_((_locker))
//C++0x 便捷sleep，用于当前线程
#define __sleep(msec) std::this_thread::sleep_for(std::chrono::milliseconds(msec))
//移交线程使用权
#define __surrenderconsole__ __sleep(0)
//不可删除，用于指针副本，框架自行处理
#define __not_delete
//get 方法宏 assume type of num is int, getter(num, Num) will generate function - const int& getNum()const{return this->num;}
#define getter(_Type_, _variable_, _name_) inline const _Type_& get##_name_()const{return _variable_;}

//set 方法宏 assume type of num is int, setter(num, Num) will generate function - int setNum(int& val){this->num = val;}
#define setter(_Type_, _variable_, _name_) inline void set##_name_(const _Type_& val){this->_variable_ = val;}

//get-set 方法宏，Type的类型不要加const，可以加& or &&
#define getsetter(_Type_, _variable_, _name_) getter(_Type_, _variable_, _name_) \
	setter(_Type_, _variable_, _name_) \

//GETSETTER 大宏
#define GETSETTER(_Type_, _variable_, _name_) \
	private: _Type_ _variable_;\
	public:getsetter(_Type_, _variable_, _name_)\

//反射宏
#define REFLECTION(_name_, _OBJ_)\
	T::_reflection_->add_property(_p##_name_, &_OBJ_::get##_name_, &_OBJ_::set##_name_)
typedef signed char int8, *Pint8;
typedef signed short int16, *PINT16;
typedef signed int int32, *PINT32;
typedef signed __int64 int64, *PINT64;
typedef unsigned char uint8, *PUINT8;
typedef unsigned short uint16, *PUINT16;
typedef unsigned int uint32, *PUINT32;
typedef unsigned __int64 uint64, *PUINT64;
typedef unsigned char byte;

typedef unsigned long DWORD;

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef float FLOAT;
typedef FLOAT*PFLOAT;
typedef BOOL *PBOOL;
typedef BOOL *LPBOOL;
typedef BYTE *PBYTE;
typedef BYTE *LPBYTE;
typedef int *PINT;
typedef int *LPINT;
typedef WORD *PWORD;
typedef WORD *LPWORD;
typedef long *LPLONG;
typedef DWORD *PDWORD;
typedef DWORD *LPDWORD;
typedef void *LPVOID;
typedef const void *LPCVOID;

typedef int INT;
typedef unsigned int UINT;
typedef unsigned int *PUINT;

typedef unsigned int SOCKET;
typedef unsigned int size_t;


#ifndef NOMINMAX

#ifndef max
#define max(a,b)(((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)(((a) < (b)) ? (a) : (b))
#endif

#endif// NOMINMAX

#define MAKEWORD(a, b)((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l) ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l) ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w) ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w) ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

#ifndef CheZeroMemory
#define CheZeroMemory(Destination,Length) memset((Destination),0,(Length))
#else
#undef CheZeroMemory
#define CheZeroMemory(Destination,Length) memset((Destination),0,(Length))
#endif // !ZeroMemory

#ifndef NULL
#define NULL 0
#endif // !NULL

#ifndef inline
#define inline inline
#endif // !inline

#ifndef forever
#define forever for(;;)
#else
#undef forever
#define forever for(;;)
#endif // !forever

#ifndef move_quick
#define move_quick(memory) std::move((memory))	//使用std::move语义转移内存，可以提高性能
#else
#undef move_quick
#define move_quick(memory) std::move((memory))
#endif // !move_q

//只能建立在栈上
#define ONLY_STACK(Class) void* operator new(size_t)=delete;\
		void operator delete(void *)=delete

void _Debug(const char *format, const char *func_name, ...);
void _Warning(const char *format, const char *func_name, ...);
void _Critical(const char *format, const char *func_name, ...);
void _Fatal(const char *format, const char *func_name, ...);


typedef std::function<void(MsgType type, const char *s)> MessageHandler_Function;
extern MessageHandler_Function debug_handler;
inline void hInstallMessageHandler(MessageHandler_Function func){ debug_handler = func; }
void h_message_output(MsgType type, const char *s);
inline void h_message(MsgType type, const char *format, const char *func_name, va_list ap);

inline void che_noop(void) {}

#ifndef __func__
//打印本宏定义所在函数的名称
#define __func __FUNCTION__
#else
#define __func __func__
#endif // !__func

#define Debug(format, ...) _Debug(format,__func,__VA_ARGS__)
//#define Warning(format, ...) _Warning(format,__func,__VA_ARGS__)
#define Critical(format, ...) _Critical(format,__func,__VA_ARGS__)
#define Fatal(format, ...) _Fatal(format,__func,__VA_ARGS__)

class HDebug;
class HNoDebug;
#ifdef _DEBUG
inline HDebug hDebug();
inline HDebug hWarning();
#else
inline HNoDebug hDebug();
#endif // _DEBUG

// 
//
// Min/Max type values
//
#define INT8_MIN        (-127i8 - 1)
#define SHORT_MIN       (-32768)
#define INT16_MIN       (-32767i16 - 1)
#define INT_MIN         (-2147483647 - 1)
#define INT32_MIN       (-2147483647i32 - 1)
#define LONG_MIN        (-2147483647L - 1)
#define LONGLONG_MIN    (-9223372036854775807i64 - 1)
#define LONG64_MIN      (-9223372036854775807i64 - 1)
#define INT64_MIN       (-9223372036854775807i64 - 1)
#define INT128_MIN      (-170141183460469231731687303715884105727i128 - 1)

#ifdef _WIN64
#define INT_PTR_MIN     (-9223372036854775807i64 - 1)
#define LONG_PTR_MIN    (-9223372036854775807i64 - 1)
#define PTRDIFF_T_MIN   (-9223372036854775807i64 - 1)
#define SSIZE_T_MIN     (-9223372036854775807i64 - 1)
#else
#define INT_PTR_MIN     (-2147483647 - 1)
#define LONG_PTR_MIN    (-2147483647L - 1)
#define PTRDIFF_T_MIN   (-2147483647 - 1)
#define SSIZE_T_MIN     (-2147483647L - 1)
#endif

#define INT8_MAX        127i8
#define UINT8_MAX       0xffui8
#define BYTE_MAX        0xff
#define SHORT_MAX       32767
#define INT16_MAX       32767i16
#define USHORT_MAX      0xffff
#define UINT16_MAX      0xffffui16
#define WORD_MAX        0xffff
#define INT_MAX         2147483647
#define INT32_MAX       2147483647i32
#define UINT_MAX        0xffffffff
#define UINT32_MAX      0xffffffffui32
#define LONG_MAX        2147483647L
#define ULONG_MAX       0xffffffffUL
#define DWORD_MAX       0xffffffffUL
#define LONGLONG_MAX    9223372036854775807i64
#define LONG64_MAX      9223372036854775807i64
#define INT64_MAX       9223372036854775807i64
#define ULONGLONG_MAX   0xffffffffffffffffui64
#define DWORDLONG_MAX   0xffffffffffffffffui64
#define ULONG64_MAX     0xffffffffffffffffui64
#define DWORD64_MAX     0xffffffffffffffffui64
#define UINT64_MAX      0xffffffffffffffffui64
#define INT128_MAX      170141183460469231731687303715884105727i128
#define UINT128_MAX     0xffffffffffffffffffffffffffffffffui128

#undef SIZE_T_MAX

#ifdef _WIN64
#define INT_PTR_MAX     9223372036854775807i64
#define UINT_PTR_MAX    0xffffffffffffffffui64
#define LONG_PTR_MAX    9223372036854775807i64
#define ULONG_PTR_MAX   0xffffffffffffffffui64
#define DWORD_PTR_MAX   0xffffffffffffffffui64
#define PTRDIFF_T_MAX   9223372036854775807i64
#define SIZE_T_MAX      0xffffffffffffffffui64
#define SSIZE_T_MAX     9223372036854775807i64
#define _SIZE_T_MAX     0xffffffffffffffffui64
#else
#define INT_PTR_MAX     2147483647 
#define UINT_PTR_MAX    0xffffffff
#define LONG_PTR_MAX    2147483647L
#define ULONG_PTR_MAX   0xffffffffUL
#define DWORD_PTR_MAX   0xffffffffUL
#define PTRDIFF_T_MAX   2147483647
#define SIZE_T_MAX      0xffffffff
#define SSIZE_T_MAX     2147483647L
#define _SIZE_T_MAX     0xffffffffUL
#endif
#define signals public
#define slots
#define emit

	extern void h_printf(const char *format);

	template<typename T, typename... Args>
	void h_printf(const char *format, T value, Args... args){
		try{
			while (*format){
				if (*format == '%' && *(++format) != '%'){
					std::cout << value;
					return h_printf(format, args...);
				}
				std::cout << *format++;
			}
		}
		catch (std::invalid_argument e){
			fprintf_s(stderr, "\n%s\n", e.what());
		}
	}
	template<typename... Args>
	void Warning(const char *format, Args... args){
		h_printf("Warning: ");
		h_printf(format, args...);
		putchar('\n');
	}
#ifdef _DEBUG
	template<typename... Args>
	void debug(const char *format, Args... args){
		h_printf("Debug: ");
		h_printf(format, args...);
		putchar('\n');
	}
#else
	template<typename... Args>
	void debug(const char *format, Args... args){}
#endif // _DEBUG
	class HLog;
	extern HLog *logger;
	//全局结构体定义区
#ifndef _func_addr_
	template<typename Func>
	union _func_addr {
		DWORD addr;
		Func func;
	};
#define _func_addr_
#endif // !_func_addr

	
	CHE_NAMESPACE_END
#endif	//HGLOBAL_H_