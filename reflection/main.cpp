// reflection.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "inheritor.h"
#include <iostream>
#include <string>
using namespace std;
using namespace che;

int _tmain(int argc, _TCHAR* argv[])
{
	Baser abc;
	abc.setValueForKey("number", 1568);
	cout << abc.valueForKey<int>("number") << endl;

	Inheritor inht;

	/*�ж�Inheritor�Ƿ���Baser������*/
	cout << inht.isKindOfClass(abc.getClass()) << endl;
	inht.number() = 4235;
	/*����inht����ġ�number�����Ե�ֵ*/
	cout << inht.valueForKey<int>("number") << endl;
	/*����inht����ġ�fly�����Ե�ֵ*/
	cout << inht.valueForKey<bool>("fly") << endl;

	/*���Ͳ�ƥ������*/
	cout << inht.valueForKey<double>("number") << endl;

	/*������Ϊ��*/
	cout << inht.valueForKey<int>("") << endl;

	/*��ȡ�������������*/
	long count = 0;
	const char **properties = cpp_getClass_properties(abc.getClass(), count);
	for (int i = 0; i < count; ++i) {
		cout << properties[i] << "\t";
	}
	cout << endl;
	const char **__pro = cpp_getClass_properties(Inheritor::getClass(), count);
	for (int i = 0; i < count; ++i) {
		cout << __pro[i] << "\t";
	}
	cout << endl;

	//��ȡInheritor�������
	cout << Inheritor::cpp_getClassName() << endl;
	return 0;
}

