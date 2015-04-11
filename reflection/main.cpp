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

	cout << inht.isKindOfClass(abc.getClass()) << endl;
	inht.number() = 4235;
	cout << inht.valueForKey<int>("number") << endl;
	cout << inht.valueForKey<bool>("fly") << endl;

	long count = 0;
	//C++11的新语法？我才发现，类静态成员函数，可以被对类的对象调用……
	const char **properties = cpp_getClass_properties(abc.getClass(), count);
	const char **__pro = cpp_getClass_properties(Inheritor::getClass(), count);
	cout << properties[0] << "\t" << properties[1] << endl;
	cout << __pro[0] << endl;

	cout << Inheritor::cpp_getClassName() << endl;
	return 0;
}

