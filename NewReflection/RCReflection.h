#pragma once
#include "refc.h"
CHE_NAMESPACE_BEGIN
//����������������Ļ�����ʾ
class RCReflection
{
	___refc(RCReflection)
public:
	RCReflection();
	virtual ~RCReflection();
#pragma region ���ᱻrscʶ��
/*
//��һ�λᱻ����궨����
public:
	//��������
	static const char* getClassname();

	//����Class�ṹ
	static Class getClass();

	//���ظ���id�ṹ
	static Class superClass();

	//���this��class_object�����࣬����true
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
#pragma endregion ���ᱻrscʶ��

public:
	int add(int a, int b) { return a + b; }
	__property(int, flags);
};

CHE_NAMESPACE_END