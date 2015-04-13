#include "RSC.h"

#define  morphology_property "property_synthesize("
#define  morphology_reflection "reflection_declare("

RSC::RSC(const list<string>& paths)
	:fileFullNames(paths)
	, proerties(50)
{
	fileFullNames.push_front("");
}

RSC::~RSC()
{
}

void RSC::parse()
{
	the_class_name.clear();
	proerties.clear();
	buffer.clear();
	auto &fileName = fileFullNames.front();
	get_filename(fileName);
	reader.open(fileName, ifstream::in);
	if (reader.is_open() == false) {
		fprintf_s(stderr, "打开文件：%s 失败\n", fileName.c_str());
		return;
	}
	process();
	long pos = 0;
	if (isRelfection(pos) == false) {
		return;
	}
	//提取类名字
	process_classname(pos);
	//属性处理
	process_properties();
}

bool RSC::next() const
{
	fileFullNames.pop_front();
	return !fileFullNames.empty();
}

bool RSC::isValid() const
{
	return !the_class_name.empty();
}

const string & RSC::current_filePath() const
{
	return fileFullNames.front();
}

const vector<Property>& RSC::get_properties() const
{
	return proerties;
}

const string & RSC::class_name() const
{
	return the_class_name;
}

bool RSC::isRelfection(long &pos)
{
	pos = buffer.find(morphology_reflection);
	return pos != string::npos;
}


void RSC::process()
{
	const int size = 1024 * 8;
	static char buf[size];
	while (!reader.eof()) {
		reader.getline(buf, size);
		buffer.append(buf);
	}
	reader.close();
}

void RSC::process_classname(long pos)
{
	while (buffer.at(pos++) != '(') {
		continue;
	}
	the_class_name;
	while (buffer.at(pos) != ',') {
		the_class_name += buffer.at(pos++);
	}
}

void RSC::process_properties()
{
	int pos = buffer.find(morphology_property, 0);
	const int size = sizeof morphology_property-1;
	int length = 0;
	char *__buffer = nullptr;
	Property p;
	while (pos != string::npos) {
		//找到第一个属性类型的坐标
		pos += size;
		length = process_properties_type(pos, __buffer);
		//proerties.at(valid_property_count).type.assign(__buffer);
		p.type.assign(__buffer);
		pos += (length + 2);
		length = process_properties_name(pos, __buffer);
		//proerties.at(valid_property_count).name.assign(__buffer);
		p.name.assign(__buffer);
		proerties.push_back(p);
		pos += length;

		pos = buffer.find(morphology_property, pos);
	}
}

long RSC::process_properties_type(long pos, char *&type_name)
{
	static char __buffer[256];
	int count = 0;
	while (buffer.at(pos) != ',') {
		__buffer[count] = buffer.at(pos);
		++count;
		++pos;
	}
	type_name = &__buffer[0];
	__buffer[count] = 0;
	return count;
}

long RSC::process_properties_name(long pos, char *& name)
{
	static char __buffer[256];
	int count = 0;
	while (buffer.at(pos) != ')') {
		__buffer[count] = buffer.at(pos);
		++count;
		++pos;
	}
	name = &__buffer[0];
	__buffer[count] = 0;
	return count;
}

void RSC::get_filename(const string & fullName)
{
	string::size_type pos = fullName.find_last_of("\\") + 1;
	_fileName = fullName.substr(pos, fullName.size() - pos);
}
