#include "RSC.h"
#include <io.h>
#include <cstdio>
#include <direct.h>
#include <windows.h>
int __invoke_main_parse(int argc, char *argv[]);
void find_all_dir(string path, string space, list<string> &paths);
void create_rscFiles_dictionary(const string &path);
void make_file(RSC *rsc);
void process(RSC *rsc);
bool isVaild(const char *__name);
void getFileName(const string& filePath, string& fileName);
FILE* create_newfile(const string &filePath);
void write_header(const string &headerFilePath, string &out);
void write_add_properties_func(const vector<Property>& data, const string &class_name, string &out);
void write_cpp_getClassName_func(const string &class_name, string &out);

static string rsc_dictionary;


int main(int argc, char *argv[])
{

	return __invoke_main_parse(argc, argv);
}

int __invoke_main_parse(int argc, char *argv[])
{
	list<string> paths;

	//获取当前路径
	char ptemp[1024];
	_getcwd(ptemp, 1024);

	//创建目录
	string current_path(ptemp);
	create_rscFiles_dictionary(current_path);

	//寻找目标文件
	current_path.append("\\");
	find_all_dir(current_path, "*.*", paths);

	//建立rsc对象
	RSC *rsc = new RSC(paths);
	process(rsc);

	system("pause");
	return 0;
}
//在当前目录下创建rscFiles目录，存在就不创建
void create_rscFiles_dictionary(const string &path)
{
	rsc_dictionary = path;
	rsc_dictionary.append("\\rscFiles");
	_mkdir(rsc_dictionary.c_str());
	rsc_dictionary.append("\\");
}
//判断文件后缀名是否包含h
bool isVaild(const char *__name)
{
	static string name;
	name = __name;
	return name.find(".h", name.size() - 5) != string::npos;
}
//递归获取所有当前目录下的所有符合条件的文件路径
void find_all_dir(string path, string space, list<string> &paths)
{
	long lf;
	_finddata_t info;
	if ((lf = _findfirst((path + space).c_str(), &info)) == -1L) {
		return;
	}
	_findnext(lf, &info);
	string tmp;
	while (_findnext(lf, &info) == 0) {
		tmp.clear();
		if (info.attrib == _A_SUBDIR) {
			tmp = path;
			tmp.append(info.name).append("\\");
			find_all_dir(tmp, "*.*", paths);
			continue;
		}
		if (isVaild(info.name) == true) {
			paths.push_back(path+string(info.name));
		}
	}
	_findclose(lf);
}

void process(RSC *rsc)
{
	while (rsc->next()) {
		rsc->parse();
		if (rsc->isValid() == false) {
			continue;
		}
		make_file(rsc);
	}
}

//filePath完整路径
void make_file(RSC *rsc)
{
	string fileName;
	string outFile;
	string fileABSpath;
	FILE *fp = nullptr;
	//创建新的文件名：rsc_xxxx.cpp
	getFileName(rsc->current_filePath(), fileName);

	//创建（打开）文件
	fileABSpath.append(rsc_dictionary).append(fileName);
	fp = create_newfile(fileABSpath);
	if (fp == nullptr) {
		fprintf_s(stderr, "%s No such file...\n", fileName.c_str());
		return;
	}

	write_header(rsc->current_filePath(), outFile);
	//加入命名空间宏
	outFile.append("CHE_NAMESPACE_BEGIN\n\n");

	//写入RSC控制宏
	outFile.append("#ifndef NO_RSC\n");

	write_cpp_getClassName_func(rsc->class_name(), outFile);
	write_add_properties_func(rsc->get_properties(), rsc->class_name(), outFile);

	outFile.append("#endif\n");

	outFile.append("CHE_NAMESPACE_END");

	fprintf_s(fp, "%s", outFile.c_str());
	fclose(fp);
	fprintf_s(stderr, "%s created success!\n", fileName.c_str());
}

//从路径中获取文件名
void getFileName(const string& filePath, string& fileName)
{
	string::size_type pos = filePath.find_last_of("\\") + 1;
	string::size_type posDot = filePath.find_last_of(".");
	fileName.append("rsc_").append(filePath.substr(pos, posDot - pos)).append(".cpp");
}

//根据filePath返回文件指针
FILE* create_newfile(const string &filePath)
{
	FILE *fp;
	fopen_s(&fp, filePath.c_str(), "w");
	return fp;
}

//写文件头
void write_header(const string &headerFilePath, string &out)
{
	//写注释
	out.append("/****************************************************************************\n");
	out.append("** Reflection code from reading C++ file ").append(headerFilePath).append("\n");
	out.append("** \n");
	out.append("** created by RSC. author: CHE\n");
	out.append("** \n");
	out.append("** WARNING! All changes made in this file will be lost!\n");
	out.append("*****************************************************************************/\n\n");
	//写#include <headerFilePath>
	out.append("#include \"").append(headerFilePath).append("\"\n");
}

//写cpp_getClassName函数
void write_cpp_getClassName_func(const string &class_name, string &out)
{
	out.append("const char * ").append(class_name).append("::cpp_getClassName()\n");
	out.append("{\n");
	out.append("\treturn \"").append(morphology_prefix).append(class_name).append("\";\n");
	out.append("}\n\n");
}

//写入add_properties函数
void write_add_properties_func(const vector<Property>& data, const string &class_name, string &out)
{
	out.append("void ").append(class_name).append("::add_properties()\n");
	out.append("{\n");
	for (size_t i = 0; i < data.size(); ++i) {
		out.append("\t//type:").append(data.at(i).type).append("\n");
		out.append("\tadd_property(this, \"").append(data.at(i).name).append("\", &").append(class_name).
			append("::").append(data.at(i).name).append(");\n");
	}
	out.append("}\n\n");
}