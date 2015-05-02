#pragma once
#include "refc.h"
CHE_NAMESPACE_BEGIN
//这里我用作反射类的基类演示
class RCReflection
{
	___refc(RCReflection)
public:
	RCReflection();
	virtual ~RCReflection();
#pragma region 不会被rsc识别
/*
//这一段会被编进宏定义中
public:
	//返回类名
	static const char* getClassname();

	//返回Class结构
	static Class getClass();

	//返回父类id结构
	static Class superClass();

	//如果this是class_object的子类，返回true
	static bool isKindOfClass(Class class_object);
*/
public:
	template<typename T>
	void setValueForKey(const T &val, const char *propertyname)
	{
		Property<T, ObjectdefsPrivate::StayOrigin<this>::Type>::rc_property(propertyname, this) = val;
	}

	template<typename T>
	const T& valueForKey(const char *propertyname)
	{
		return Property<T, ObjectdefsPrivate::StayOrigin<this>::Type>::rc_property(propertyname, this);
	}
#pragma endregion 不会被rsc识别

public:
	int add(int a, int b) { return a + b; }
	__property(int, flags);
};

CHE_NAMESPACE_END