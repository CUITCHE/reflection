// reflection.cpp : 定义控制台应用程序的入口点。
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

	/*判断Inheritor是否是Baser的子类*/
	cout << inht.isKindOfClass(abc.getClass()) << endl;
	inht.number() = 4235;
	/*返回inht对象的“number”属性的值*/
	cout << inht.valueForKey<int>("number") << endl;
	/*返回inht对象的“fly”属性的值*/
	cout << inht.valueForKey<bool>("fly") << endl;

	/*类型不匹配的情况*/
	cout << inht.valueForKey<double>("number") << endl;

	/*属性名为空*/
	cout << inht.valueForKey<int>("") << endl;

	/*获取对象的所有属性*/
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

	//获取Inheritor类的名字
	cout << Inheritor::cpp_getClassName() << endl;
	return 0;
}

