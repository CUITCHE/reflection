/*!
 * \file refc.h
 * \date 2015/05/02 14:48
 *
 * \author CHE
 * Contact: xiaohe918@vip.qq.com
 *
 * \brief 反射需要的宏定义，以及结构体声明处
 *
 * TODO: long description
 *
 * \note
*/
#ifndef refc_H__
#define refc_H__
#include "hglobal.h"
#include <map>
#include <vector>
using std::map;
using std::vector;
using std::make_pair;

CHE_NAMESPACE_BEGIN
#ifdef ___refc
#undef ___refc
#endif // ___refc

//只能在头文件.h中声明书写
//形如：
//class Object [: public Baser]{
//___refc
//public
//Object();
//}
//被声明后，该类将会进入反射类系统中，不需要其它声明限制
//它的每一个方法（除去构造、析构函数、静态方法、模版方法）将会进入反射类中。
#define ___meta(classname) classname##id
#define ___refc(classname) public:\
static const char* getClassname();\
static Class getClass();\
static Class superClass();\
static bool isKindOfClass(Class class_object);\
private: static id ___meta(classname);

#ifdef __property
#undef __property
#endif // __property
//用__property 标识一个成员变量
//生成以“_propertyName_”命名的方法，返回值为引用
#define __property(__type__, _propertyName_) public:\
__type__& _propertyName_(){return this->_##_propertyName_;}\
private:\
__type__ _##_propertyName_

typedef struct cpp_class *Class;

#ifdef PROPERTY_FLAG
#undef PROPERTY_FLAG
#endif // PROPERTY_FLAG
//标识是属性方法
#define PROPERTY_FLAG (uint32(-1))

struct cpp_method
{
	uint32 callIndex;	//采用什么方法去call方法
	const char *func_name;
	DWORD func_addr;
};

struct cpp_class
{
	Class superClass;			//父类isa结构指针
	const char *classname;		//类名
	vector<cpp_method> *functionlist;//方法列表（包含属性的方法）
};

//每个反射类都有一个id变量，存储了类的一些信息
typedef struct cpp_object
{
	struct cpp_class isa;
}*id;

template<typename Function> struct Function_Addr_Union
{
	static DWORD addr_value(Function f) {
		_func_addr<Function> __addr;
		__addr.func = f;
		return __addr.addr;
	}
};
//根据属性名字，找到属性结构体
cpp_method* find_property_by_name(const char *name, Class _id);

cpp_method* find_property_by_name_impl(const char *name, Class clss);

template<typename Ret, typename class_type> struct Property
{
	typedef Ret&(__stdcall class_type::*Function)(void);
	typedef Ret& ret;
	static ret rc_property(const char *propertyname, class_type *o) {
		Class _id = class_type::getClass();
		cpp_method *method = find_property_by_name(property_name, _id);
		Function f = (Function)method->func_addr;
		return o->*f();
	}
};

//根据方法名，找到cpp_method结构体
cpp_method* find_method_by_name(const char *name, Class _id);

cpp_method* find_method_impl(const char *name, Class _id);

//动态方法调用
template<typename Ret, typename... Args, class Obj>
Ret dync_call(Obj *o, const char *functionName, Args... args)
{
	Class clss = Obj::getClass();
	cpp_method *method = find_method_by_name(functionName, clss);
	typedef Ret(__stdcall *Function)(Args...);
	Function f = (Function)(method->func_addr);
	DWORD class_addr = (DWORD)o;
	__asm{
		mov ecx, class_addr
	}
	return f(args...);
}
CHE_NAMESPACE_END
#endif // refc_H__