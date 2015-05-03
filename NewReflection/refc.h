/*!
 * \file refc.h
 * \date 2015/05/02 14:48
 *
 * \author CHE
 * Contact: xiaohe918@vip.qq.com
 *
 * \brief ������Ҫ�ĺ궨�壬�Լ��ṹ��������
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

//ֻ����ͷ�ļ�.h��������д
//���磺
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
//1. �������󣬸��ཫ����뷴����ϵͳ�У�����Ҫ�����������ơ�
//2. ����ÿһ����������ȥ���졢����������ģ�淽����������뷴�����С�
//2.1 �ײ⣬���ط�������Ա��̬��������Ա����������Ч��  
//3. ��������ʼ���κ�һ����ͬ������ࣨ���������ĳ������ࣩ���������ʹ���˷���ϵͳ���������������븸�������
//	 ��û��������null��
//4. ���书��ֻ�����ĳһ���࣬���������Լ��������࣬������___refc���޹�����Ż����㡣
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
//��__property ��ʶһ����Ա����
//�����ԡ�_propertyName_�������ķ���������ֵΪ����
#define __property(__type__, _propertyName_) public:\
__type__& _propertyName_(){return this->_##_propertyName_;}\
private:\
__type__ _##_propertyName_

typedef struct cpp_class *Class;

#ifdef PROPERTY_FLAG
#undef PROPERTY_FLAG
#endif // PROPERTY_FLAG
//��ʶ�����Է���
#define PROPERTY_FLAG (bool(1))
enum MethodFlags{
	//���Է���
	Property = 1,
	//���ط���
	Overload = 1 << 1,
	//��̬����
	Static = 1 << 2
};
struct cpp_method
{
	uint32 flag : 3;			//MethodFlagsֵ
	uint32 paramsCount : 30;	//����ֵ����+��������
	const char *func_name;
	DWORD func_addr;
	size_t *params;				//�洢����ֵ�Ͳ���hashcode������,params[0]�洢����ֵ����
	bool isProperty()const { return (flag & Property) != 0; }
	bool isOverload()const { return (flag & Overload) != 0; }
	bool isStatic()const { return (flag & Static) != 0; }
};

struct cpp_class
{
	Class superClass;			//����isa�ṹָ��
	const char *classname;		//����
	vector<cpp_method> *functionlist;//�����б��������Եķ�����
};

//ÿ�������඼��һ��id�������洢�����һЩ��Ϣ
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
//�����������֣��ҵ����Խṹ��
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

//���ݷ��������ҵ�cpp_method�ṹ��
cpp_method* find_method_by_name(const char *name, Class _id);

cpp_method* find_method_impl(const char *name, Class _id);

//�������غ����������������������cpp_method
cpp_method* find_overload_method_by_name(cpp_method *lastone, Class clss, const vector<size_t> &arguments);

//����ʱ�������ͷ���ֵ�Ƿ�һ�£���һ�·���false
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

//��̬��������
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
		//�Ƿ������غ���
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