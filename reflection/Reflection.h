#ifndef Reflection_H__
#define Reflection_H__

/********************************************************************
	created:	2015/04/10
	created:	10:4:2015   23:41
	file base:	Reflection
	author:		CHE
	
	purpose:	反射机制 implement
*********************************************************************/
#include "hglobal.h"
#include <map>
using std::map;
CHE_NAMESPACE_BEGIN
typedef struct obj_class *Class;
struct _property_;
struct _property_list;

struct obj_class {
	Class super_class;	//父类isa结构
	char *name;	//类名字
	long instance_size;	//类的大小
	_property_list *properties;	//属性列表
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
使用Meta类时，类一定要继承HObject
*/
#include <map>
using std::map;
template<typename ClassName>
struct Reflection
{
	typedef ClassName class_type;
	Class id;
	Reflection(ClassName *obj);

	/*析构函数，不考虑做析构任务。因为meta_obj是全局存在的。*/
	~Reflection() {}

	//增加属性，传入：属性名，get方法地址，set方法地址
	template<typename GetFunc, typename SetFunc>
	void add_property(const char *property_name, GetFunc _get_, SetFunc _set_);

	//调用set方法，传入：要设置的对象指针，值，属性名
	template<typename ValueType>
	bool setValue_forKey(class_type *obj, const ValueType &value, const char *property_name);

	//调用get方法
	template<typename RET>
	const RET& value_Forkey(class_type *obj, const char *property_name);

	//this的id是class_object的子类就返回true
	bool isSubOfClass(Class class_object);
protected:
	void initilaize_superclass(class_type *obj);

	//返回最后一个属性。
	//所以，此时的属性中的next为空
	_property_list* find_last_property();

	//通过属性名来找到_property_结构的指针，并返回
	_property_* find_property_by_name(const char *name);

	template<typename T>
	void setValue_impl(DWORD class_address, DWORD setFunc, const T &val);

	template<typename T>
	const T& value_impl(DWORD class_address, DWORD getFunc);
private:
	//property_finder_cache中的内存来自Class id。
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
	//这儿返回的是指针，
	//若T已经是顶级类，则会返回nullptr
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
	//在缓存中没有找到，则开始遍历
	Class ider = id;
looper1:
	_property_list *p = ider->properties;
	_property_ *node = nullptr;
	while (p) {
		node = p->_property;
		//找到
		if (strcmp(name, node->property_name) == 0) {
			//向缓存中增加记录
			property_finder_cache.insert(make_pair(node->property_name, node));
			return node;
		}
		p = p->next;
	}
	//哦，到这儿没找到，就向它的父类查询

	//自己是顶级类了，返回nullptr
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
	//找到最后一个属性
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

	//把空格也要算上
	int name_length = strlen(property_name) + 1;
	now_property->property_name = new char[name_length];
	//完成对名字的拷贝
	memcpy(now_property->property_name, property_name, sizeof(char)*name_length);
	//记录get方法的函数地址
	_func_addr<GetFunc> d;
	d.func = _get_;
	now_property->get_method = d.addr;
	//记录set方法的函数地址
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
	//将指针转换成DWORD，利用汇编实现函数的调用
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
	//将指针转换成DWORD，利用汇编实现函数的调用
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
	//完成set方法的调用
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
	//完成get方法的调用
	return GET();
}
CHE_NAMESPACE_END
#endif // Reflection_H__


