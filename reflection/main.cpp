// reflection.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Baser.h"
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
	return 0;
}

