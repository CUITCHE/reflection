#ifndef Reflection_H__
#define Reflection_H__

/********************************************************************
	created:	2015/04/10
	created:	10:4:2015   23:41
	file base:	Reflection
	author:		CHE
	
	purpose:	������� implement
*********************************************************************/
#include "hglobal.h"
#include <map>
using std::map;
CHE_NAMESPACE_BEGIN
typedef struct obj_class *Class;
struct _property_;
struct _property_list;

struct obj_class {
	Class super_class;	//����isa�ṹ
	char *name;	//������
	long instance_size;	//��Ĵ�С
	_property_list *properties;	//�����б�
};

struct _property_list {
	_property_ *_property;
	_property_list *next;
};

struct _property_ {
	char *property_name;
	DWORD get_method;
	DWORD set_method;
};

/*
ʹ��Meta��ʱ����һ��Ҫ�̳�HObject
*/
#include <map>
using std::map;
template<typename ClassName>
struct Reflection
{
	typedef ClassName class_type;
	Class id;
	Reflection(ClassName *obj);

	/*����������������������������Ϊmeta_obj��ȫ�ִ��ڵġ�*/
	~Reflection() {}

	//�������ԣ����룺��������get������ַ��set������ַ
	template<typename GetFunc, typename SetFunc>
	void add_property(const char *property_name, GetFunc _get_, SetFunc _set_);

	//����set���������룺Ҫ���õĶ���ָ�룬ֵ��������
	template<typename ValueType>
	bool setValue_forKey(class_type *obj, const ValueType &value, const char *property_name);

	//����get����
	template<typename RET>
	const RET& value_Forkey(class_type *obj, const char *property_name);

	//this��id��class_object������ͷ���true
	bool isSubOfClass(Class class_object);
protected:
	void initilaize_superclass(class_type *obj);

	//�������һ�����ԡ�
	//���ԣ���ʱ�������е�nextΪ��
	_property_list* find_last_property();

	//ͨ�����������ҵ�_property_�ṹ��ָ�룬������
	_property_* find_property_by_name(const char *name);

	template<typename T>
	void setValue_impl(DWORD class_address, DWORD setFunc, const T &val);

	template<typename T>
	const T& value_impl(DWORD class_address, DWORD getFunc);
private:
	//property_finder_cache�е��ڴ�����Class id��
	map<const char*, _property_ *> property_finder_cache;
};

template<typename ClassName>
inline Reflection<ClassName>::Reflection(ClassName * obj)
{
	id = new obj_class;
	id->instance_size = sizeof ClassName;
	const char *rawName = typeid(ClassName).raw_name();
	int length = strlen(rawName) + 1;
	id->name = new char[length];
	memcpy(id->name, rawName, sizeof(char)*length);

	id->properties = new _property_list;
	id->properties->next = nullptr;
	id->properties->_property = nullptr;
	id->super_class = nullptr;
	initilaize_superclass(obj);
}

template<typename ClassName>
inline bool Reflection<ClassName>::isSubOfClass(Class class_object)
{
	Class super = id->super_class;
looper:
	if (super == class_object) {
		return true;
	}
	super = super->super_class;
	if (super) {
		goto looper;
	}
	return false;
}

template<typename ClassName>
inline void Reflection<ClassName>::initilaize_superclass(class_type * obj)
{
	//������ص���ָ�룬
	//��T�Ѿ��Ƕ����࣬��᷵��nullptr
	Class baser = obj->superClass();
	id->super_class = baser;
}

template<typename ClassName>
inline _property_list * Reflection<ClassName>::find_last_property()
{
	_property_list *p = id->properties;
	while (p->next != nullptr) {
		p = p->next;
	}
	return p;
}

template<typename ClassName>
inline _property_ * Reflection<ClassName>::find_property_by_name(const char * name)
{
	auto iter = property_finder_cache.find(name);
	if (iter != property_finder_cache.end()) {
		return iter->second;
	}
	//�ڻ�����û���ҵ�����ʼ����
	Class ider = id;
looper1:
	_property_list *p = ider->properties;
	_property_ *node = nullptr;
	while (p) {
		node = p->_property;
		//�ҵ�
		if (strcmp(name, node->property_name) == 0) {
			//�򻺴������Ӽ�¼
			property_finder_cache.insert(make_pair(node->property_name, node));
			return node;
		}
		p = p->next;
	}
	//Ŷ�������û�ҵ����������ĸ����ѯ

	//�Լ��Ƕ������ˣ�����nullptr
	if (ider->super_class == nullptr) {
		return nullptr;
	}
	ider = ider->super_class;
	goto looper1;
}

template<typename ClassName>
template <typename GetFunc, typename SetFunc>
inline void Reflection<ClassName>::add_property(const char * property_name, GetFunc _get_, SetFunc _set_)
{
	//�ҵ����һ������
	_property_list *node = find_last_property();
	_property_list *next_now = nullptr;
	if (node->_property != nullptr) {
		node->next = new _property_list;
		next_now = node->next;
		next_now->next = nullptr;
	}
	else {
		next_now = node;
	}
	next_now->_property = new _property_;

	_property_ *now_property = next_now->_property;

	//�ѿո�ҲҪ����
	int name_length = strlen(property_name) + 1;
	now_property->property_name = new char[name_length];
	//��ɶ����ֵĿ���
	memcpy(now_property->property_name, property_name, sizeof(char)*name_length);
	//��¼get�����ĺ�����ַ
	_func_addr<GetFunc> d;
	d.func = _get_;
	now_property->get_method = d.addr;
	//��¼set�����ĺ�����ַ
	_func_addr<SetFunc> d2;
	d2.func = _set_;
	now_property->set_method = d2.addr;
}

template<typename ClassName>
template <typename ValueType>
inline bool Reflection<ClassName>::setValue_forKey(class_type * obj, const ValueType & value, const char * property_name)
{
	_property_ *the_property = find_property_by_name(property_name);
	if (the_property == nullptr) {
		return false;
	}
	//��ָ��ת����DWORD�����û��ʵ�ֺ����ĵ���
	DWORD class_address = (DWORD)obj;
	setValue_impl(class_address, the_property->set_method, value);
	return true;
}

template<typename ClassName>
template <typename RET>
inline const RET& Reflection<ClassName>::value_Forkey(class_type * obj, const char * property_name)
{
	_property_ *the_property = find_property_by_name(property_name);
	if (the_property == nullptr) {
		return RET();
	}
	//��ָ��ת����DWORD�����û��ʵ�ֺ����ĵ���
	DWORD class_address = (DWORD)obj;
	return value_impl<RET>(class_address, the_property->get_method);
}

template<typename ClassName>
template <typename T>
inline void Reflection<ClassName>::setValue_impl(DWORD class_address, DWORD setFunc, const T & val)
{
	typedef void(__stdcall *__SET)(const T &);
	__SET SET = (__SET)setFunc;
	__asm {
		mov ecx, class_address
	};
	//���set�����ĵ���
	SET(val);
}
template<typename ClassName>
template <typename T>
inline const T& Reflection<ClassName>::value_impl(DWORD class_address, DWORD getFunc)
{
	typedef const T &(__stdcall *__GET)(void);
	__GET GET = (__GET)getFunc;
	__asm {
		mov ecx, class_address
	};
	//���get�����ĵ���
	return GET();
}
CHE_NAMESPACE_END
#endif // Reflection_H__


