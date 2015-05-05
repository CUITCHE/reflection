#pragma once
#include <string>
#include <vector>
#include <list>
using namespace std;

//Range[left, right)
struct Range 
{
	size_t left;
	size_t right;
};

//函数结构体
struct Method 
{
	size_t mem_limit_symbol : 1;	//成员函数限定符:const
	size_t static_symbol : 1;		//静态成员函数
	size_t _zero : 30;
	string return_type;		//返回值，形如：cosnt int, int, int *, int &, const int*,const int&
	string function_name;	//函数名
	vector<string> arguments;//参数类型列表
	Method():mem_limit_symbol(0),static_symbol(0){}
};

//属性结构体
struct PropertyMethod 
{
	string type;
	string name;
};
class RSCParse
{
public:
	RSCParse(string *buffer);
	~RSCParse();
	//在开始分析前，做的准备工作，可能会抛出异常（if failed）exception
	void readyForParse();

	//准备工作之后，判断这份代码是否满足分析条件
	bool isValid()const;

	//清楚所有缓冲区内容
	void clear();

	//进行分析工作，并把结果存储到缓冲区。失败将抛出异常
	void parse();

	//设置父类的名字
	//由于父类可能不是来自反射系统，所以可以在外部置为空
	void setSuperClassNameNull()const;

	//生成代码
	void generateCode();
protected:
	//清除注释内容
	void removeNoteContext();

	//删除双斜杠注释
	void removeNoteForDoubleSlash();

	//删除/**/注释
	bool removeNoteForSlashStart();
protected:
	//分析super
	void findSuperAndClass();

	//寻找属性，并存入缓冲区
	void findProperties();

	//_Off：标识类型的起点
	bool parseProperty(string &typeName, string &propertyName, size_t _Off);

	//寻找方法
	void findMethod();

	//* \brief 寻找一个括号内的text，有可能是函数的参数，也有可能是宏的参数，之后会判断
	//* \param _Off_Left：左括号右边第一个位置
	//* \param _Off_Right：右括号左边的第一个位置
	//* \param out：返回一个SpecialString
	//* \param outMethod：返回一个SpecialString，当return为true时有效，为函数前面部分的内容
	//* \fn 函数会寻找在fileTextBuffer的[_Off_Left, _Off_Right]范围内的text，并返回可能是函数头的信息
	//* \return：true：也许是一个正确的函数参数内容
	bool findTextInBracketsAndMethod(size_t _Off_Left, size_t _Off_Right, struct SpecialString *out, struct SpecialString *outMethod);
private:
	mutable string superName;	//父类的名字
	mutable string className;	//类名
	string *fileTextBuffer;
	string *writeTextBuffer;	//生成的代码串缓冲区
	vector<Method> methods;		//方法
	vector<PropertyMethod> properties;//属性方法
};

