#ifndef RSC_H__
#define RSC_H__

/********************************************************************
	created:	2015/04/13
	created:	13:4:2015   16:47
	file base:	RSC
	author:		CHE
	
	purpose:	RSC编译组件，用于词法分析
				1.根据传进来的路径，以只读模式打开文件，分析词法，
				并在当前目录下创建RscFiles目录，
				2.每个被生成的文件以“rsc_”开头
				3.只分析后缀名含有h的文件
				4.属性名和类型不能超过256长度
*********************************************************************/
#include <list>
#include <vector>
#include <string>
#include <fstream>
using namespace std;


#define  morphology_prefix "__CR"

struct Property
{
	string name;	//属性名字
	string type;	//属性类型
};
class RSC
{
public:
	RSC(const list<string> &fileFullNames_);
	~RSC();

	//开始分析
	void parse();

	//是否还没分析完，如果还有文件没有被分析就会返回true。
	//并且会消耗一条数据，从list中pop出去。
	bool next()const;

	//当前这条数据是否合法
	//有效返回true
	bool isValid()const;

	//返回当前被Parse过的文件路径
	const string& current_filePath()const;

	//返回属性的Property结构
	const vector<Property>& get_properties()const;

	//返回类名
	const string& class_name()const;

protected:
	bool isRelfection(long &pos);

	//把数据存到string
	void process();

	//处理类名
	void process_classname(long pos);

	//处理属性
	void process_properties();

	//得到属性的类型，返回属性类型名的长度
	long process_properties_type(long pos, char *&type_name);

	//得到属性的名字，返回属性名的长度
	long process_properties_name(long pos, char * &name);

	//获得文件的名字
	void get_filename(const string &fullName);
private:
	mutable list<string> fileFullNames;			//所有后缀名含有h的文件
	vector<Property> proerties;
	string the_class_name;			//类名
	ifstream reader;
	string buffer;
	string _fileName;		//当前file的名字，非全路径
};
#endif // RSC_H__

