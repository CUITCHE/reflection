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
/*
class Object [: [public|protected|private|] Baser]
{
	___refc(Baser)
public:
	Object();
	void say(int);
private:
	__property(int, flags);
	__property(string,name);
};
*/
//1. 被声明后，该类将会进入反射类系统中，不需要其它声明限制。
//2. 它的每一个方法（除去构造、析构函数、模版方法）将会进入反射类中。
//2.1 亲测，重载方法，成员静态方法，成员方法，均有效。  
//3. 它可以起始于任何一个不同级别的类（顶级类或者某类的子类），如果父类使用了反射系统请如例子那样加入父类的名称
//	 若没有请输入null。
//4. 反射功能只是针对某一个类，而不是它以及它的子类，必须有___refc宏修过的类才会满足。
#define ___refc(super) public:\
static const char* getClassname();\
static Class getClass();\
static Class superClass();\
static bool isKindOfClass(Class class_object);

#ifdef null
#undef null
#endif
#define null

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
#define PROPERTY_FLAG (bool(1))
enum MethodFlags{
	//属性方法
	Property = 1,
	//重载方法
	Overload = 1 << 1,
	//静态方法
	Static = 1 << 2
};
struct cpp_method
{
	uint32 flag : 3;			//MethodFlags值
	uint32 paramsCount : 30;	//返回值类型+参数个数
	const char *func_name;
	DWORD func_addr;
	size_t *params;				//存储返回值和参数hashcode的数组,params[0]存储返回值类型
	bool isProperty()const { return (flag & Property) != 0; }
	bool isOverload()const { return (flag & Overload) != 0; }
	bool isStatic()const { return (flag & Static) != 0; }
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
	Class isa;
}*id;

template<typename Function> struct Function_Addr_Union
{
	static DWORD addr_value(Function f) {
		_func_addr<Function> __addr;
		__addr.func = f;
		return __addr.addr;
	}
	static Function func_value(DWORD addr) {
		_func_addr<Function> __addr;
		__addr.addr = addr;
		return __addr.func;
	}
};
//根据属性名字，找到属性结构体
cpp_method* find_property_by_name(const char *name, Class _id);

cpp_method* find_property_by_name_impl(const char *name, Class clss);

template<typename Ret, typename class_type> struct __Property
{
	typedef typename ObjectdefsPrivate::RemoveRef<Ret>::Type & ret;
	typedef ret(class_type::*Function)(void);
	
	static ret rc_property(const char *propertyname, class_type *o) {
		Class clss = class_type::getClass();
		cpp_method *method = find_property_by_name(propertyname, clss);
		if (method == nullptr) {
			static ObjectdefsPrivate::StayOrigin<ret>::Type ret_defalut = ObjectdefsPrivate::StayOrigin<ret>::Type();
			return ret_defalut;
		}
		Function f = Function_Addr_Union<Function>::func_value(method->func_addr);
		return (o->*f)();
	}
};

//根据方法名，找到cpp_method结构体
cpp_method* find_method_by_name(const char *name, Class _id);

cpp_method* find_method_impl(const char *name, Class _id);

//遇到重载函数就用下面这个方法查找cpp_method
cpp_method* find_overload_method_by_name(cpp_method *lastone, Class clss, const vector<size_t> &arguments);

//运行时检查参数和返回值是否一致，不一致返回false
template<typename Arg>
void calcArgumentsHashcode(vector<size_t> &arguments, Arg arg)
{
	arguments.push_back(typeid(Arg).hash_code());
}
template<typename Arg, typename...Args>
void calcArgumentsHashcode(vector<size_t> &arguments, Arg arg, Args... args)
{
	arguments.push_back(typeid(Arg).hash_code());
	calcArgumentsHashcode(arguments, args...);
}

bool checkCompatibleArguments(const vector<size_t> &arguments, const cpp_method *method);

//动态方法调用
template<typename Ret, typename... Args, class Obj>
Ret dync_call(Obj *o, const char *functionName, Args... args)
{
	static ObjectdefsPrivate::StayOrigin<Ret>::Type ret = ObjectdefsPrivate::StayOrigin<Ret>::Type();
	static vector<size_t> __arguments(10);
	__arguments.clear();
	__arguments.push_back(typeid(Ret).hash_code());
	calcArgumentsHashcode(__arguments, args...);

	Class clss = Obj::getClass();
	cpp_method *method = find_method_by_name(functionName, clss);
looper:
	if (method == nullptr) {
		fprintf_s(stderr, "'The function: [%s] is not found. If this function is overload-function, you need to check arguments'\n", functionName);
		return ret;
	}

	if (checkCompatibleArguments(__arguments, method) == false) {
		//是否是重载函数
		if (method->isOverload() == true) {
			method = find_overload_method_by_name(method, clss, __arguments);
			goto looper;
		}
		fprintf_s(stderr, "'The arguments of function: [%s] is not matching'\n", method->func_name);
		return ret;
	}
	
	if (method->isStatic() == true) {
		typedef Ret(*Function)(Args...);
		Function f = (Function)(method->func_addr);
		return f(args...);
	}
	typedef Ret(__stdcall *Function)(Args...);
	Function f = (Function)(method->func_addr);
	DWORD class_addr = (DWORD)o;
	__asm {
		mov ecx, class_addr
	}
	return f(args...);
}
CHE_NAMESPACE_END
#endif // refc_H__