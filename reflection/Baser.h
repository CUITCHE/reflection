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

	template<typename T>
	void setValueForKey(const char *propertyname, const T &val);

	template<typename T>
	const T& valueForKey(const char *propertyname);

	property_synthesize(int, number);
	property_synthesize(double, price);
protected:
	template<typename class_type>
	friend void initializeReflection(class_type *obj);

	template<typename class_type, typename PropertyFunc>
	void add_property(class_type *obj, const char * propertyName, PropertyFunc get_set);

	virtual void add_properties();

	static Baser* get_class() { return new Baser; }
public:
	Reflection<Baser> **reflection_obj;
};

template<typename class_type>
void initializeReflection(class_type *obj)
{
	if (REFLECTION_CLASS(obj) == nullptr) {
		REFLECTION_CLASS(obj) = new Reflection<class_type>(obj);
		obj->add_properties();
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
CHE_NAMESPACE_END
