// reflection.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Baser.h"
#include <iostream>
using namespace std;
using namespace che;
int _tmain(int argc, _TCHAR* argv[])
{
	Baser abc;
	abc.setValueForKey("Number", 1568);
	cout << abc.valueForKey<int>("Number") << endl;
	return 0;
}

