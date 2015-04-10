#pragma once
#include "Reflection.h"
CHE_NAMESPACE_BEGIN
class Baser
{
public:
	Baser();
	virtual ~Baser();

	//获取自身的Class
	virtual Class getClass();

	//获取父类的Class
	virtual Class superClass();

	//如果this是class_object的子类，返回true
	virtual bool isKindOfClass(Class class_object);
	template<typename T>
	void setValueForKey(const char *propertyname, const T &val);

	template<typename T>
	const T& valueForKey(const char *propertyname);

	GETSETTER(int, number,Number)
	GETSETTER(double, price, Price)
protected:
	void initializeReflection(Baser *obj);

	template<typename GetFunc, typename SetFunc>
	void add_property(const char * propertyName, GetFunc _get_, SetFunc _set_);
private:
	Reflection<Baser> *reflection;
};

template <typename T>
inline void Baser::setValueForKey(const char * propertyname, const T & val)
{
	reflection->setValue_forKey(this, val, propertyname);
}

template <typename T>
inline const T & Baser::valueForKey(const char * propertyname)
{
	return reflection->value_Forkey<T>(this, propertyname);
}

template < typename GetFunc, typename SetFunc>
inline void Baser::add_property(const char * propertyName, GetFunc _get_, SetFunc _set_)
{
	reflection->add_property(propertyName, _get_, _set_);
}
CHE_NAMESPACE_END
