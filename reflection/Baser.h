#pragma once
#include "Reflection.h"

CHE_NAMESPACE_BEGIN

#ifndef REFLECTION
#define REFLECTION (*reflection_obj)
#define REFLECTION_CLASS(obj) (*(obj->reflection_obj))
#endif // !REFLECTION

class Baser
{
public:
	Baser();
	virtual ~Baser();

	//获取自身的Class
	static Class getClass();

	//获取父类的Class
	virtual Class superClass();

	//如果this是class_object的子类，返回true
	virtual bool isKindOfClass(Class class_object);

	//返回类的名字
	static const char* cpp_getClassName();

	template<typename T>
	void setValueForKey(const char *propertyname, const T &val);

	template<typename T>
	const T& valueForKey(const char *propertyname);

	property_synthesize(int, number);
	property_synthesize(double, price);
protected:
	template<typename class_type>
	friend void initializeReflection(class_type *obj, DWORD func);

	template<typename class_type>
	inline void add_property(class_type *obj){}

	template<typename class_type, typename PropertyFunc>
	void add_property(class_type *obj, const char * propertyName, PropertyFunc get_set);

	template<typename class_type, typename PropertyFunc, typename... Args>
	void add_property(class_type *obj, const char * propertyName, PropertyFunc get_set, Args... args);

	virtual void add_properties();

	static void* get_class();
public:
	Reflection<Baser> **reflection_obj;
};

template<typename class_type>
void initializeReflection(class_type *obj, DWORD func)
{
	typedef void(*ADD)(void);
	if (REFLECTION_CLASS(obj) == nullptr) {
		REFLECTION_CLASS(obj) = new Reflection<class_type>(obj);
		
		ADD __add = (ADD)func;
		DWORD target = (DWORD)obj;
		__asm {
			mov ecx, target;
		}
		__add();
	}
}

template <typename T>
inline void Baser::setValueForKey(const char * propertyname, const T & val)
{
	(*reflection_obj)->setValue_forKey(this, val, propertyname);
}

template <typename T>
inline const T & Baser::valueForKey(const char * propertyname)
{
	return (*reflection_obj)->value_Forkey<T>(this, propertyname);
}

template <typename class_type, typename PropertyFunc>
inline void Baser::add_property(class_type *obj, const char * propertyName, PropertyFunc get_set)
{
	(*(obj->reflection_obj))->add_property(propertyName, get_set);
}
template <typename class_type, typename PropertyFunc, typename ...Args>
inline void Baser::add_property(class_type * obj, const char * propertyName, PropertyFunc get_set, Args ...args)
{
	add_property(obj, propertyName, get_set);
	add_property(obj, args...);
}
CHE_NAMESPACE_END
