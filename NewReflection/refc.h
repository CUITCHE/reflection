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
//class Object [: public Baser]{
//___refc
//public
//Object();
//}
//�������󣬸��ཫ����뷴����ϵͳ�У�����Ҫ������������
//����ÿһ����������ȥ���졢������������̬������ģ�淽����������뷴�����С�
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
#define PROPERTY_FLAG (uint32(-1))

struct cpp_method
{
	uint32 callIndex;	//����ʲô����ȥcall����
	const char *func_name;
	DWORD func_addr;
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
//�����������֣��ҵ����Խṹ��
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

//���ݷ��������ҵ�cpp_method�ṹ��
cpp_method* find_method_by_name(const char *name, Class _id);

cpp_method* find_method_impl(const char *name, Class _id);

//��̬��������
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