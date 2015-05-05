#include "RSCParse.h"
#include <exception>
#include <regex>

#define RSC_VERSION (uint32)(1<<1)
/*----------------------------------界外函数----------------------------------*/
//复制其它源过来，不管理内存
struct SpecialString
{
	char *raw_data;
	size_t size;
	SpecialString(char *raw, size_t s) :raw_data(raw), size(s) {}
	size_t length()const { return size; }
	char *begin() { return raw_data; }
	char *end() { return raw_data + size; }
	//消去str两端的空白字符
	void trim() {
		char *_begin = begin();
		char *_end = end();
		//去掉左边空白字符
		while (_begin < _end) {
			if (*_begin == ' ' || *_begin == '\t') {
				++_begin;
				--size;
				continue;
			}
			break;
		}
		raw_data = _begin;
		if (_begin == _end) {
			size = 0;
			return;
		}
		_begin = _begin + size - 1;	//取得最右边的字符地址
		while (_begin >= begin()) {
			if (*_begin == ' ' || *_begin == '\t') {
				--size;
				--_begin;
				continue;
			}
			break;
		}
	}

	//根据breaker分割str，并返回list
	list<SpecialString> split(char breaker) {
		list<SpecialString> result;
		char *s = begin();
		char *e = end();
		char *tmp;
		while (s < e) {
			tmp = s;
			while (*s != breaker && s < e) {
				++s;
				continue;
			}
			result.push_back({ tmp,((s - tmp) / sizeof(char)) });
			++s;
		}
		return result;
	}
};

RSCParse::RSCParse(string *buffer)
	:fileTextBuffer(buffer)
	, writeTextBuffer(nullptr)
{
}


RSCParse::~RSCParse()
{
	delete writeTextBuffer;
}

void RSCParse::readyForParse()
{
	clear();
	removeNoteContext();
}

bool RSCParse::isValid() const
{
	/*string::size_type pos = fileTextBuffer->find("___refc(", 0);
	if (pos == string::npos) {
		return false;
	}
	pos += 8;
	string::size_type pos2 = fileTextBuffer->find(')', pos);
	superName = fileTextBuffer->substr(pos, pos2 - pos);
	if (superName.compare("super") == 0) {
		return false;
	}*/
	string::size_type pos1 = fileTextBuffer->find("___refc", 0);

	if (pos1 != string::npos) {
		if (!(fileTextBuffer->find("#ifdef ___refc", 0) != string::npos
			&& fileTextBuffer->find("#undef ___refc", 0) != string::npos
			&& fileTextBuffer->find("#define ___refc", 0) != string::npos)) {
			return false;
		}
	}
	else{
		return false;
	}
	
	return true;
}

void RSCParse::parse()
{
	findSuperAndClass();
	findProperties();
	findMethod();
}

void RSCParse::setSuperClassNameNull() const
{
	superName.clear();
}

void RSCParse::generateCode()
{
	writeTextBuffer = new string;
}

void RSCParse::clear()
{
	writeTextBuffer->clear();
	properties.clear();
	methods.clear();
	superName.clear();
	className.clear();
}

void RSCParse::removeNoteContext()
{
	//删除双斜杠注释区
	removeNoteForDoubleSlash();

	//删除/**/注释
	auto ret = removeNoteForSlashStart();
	if (ret == false) {
		throw exception("code exists error!");
	}
}

void RSCParse::removeNoteForDoubleSlash()
{
	string::size_type posSlash = fileTextBuffer->find("//", 0);
	string::size_type posEnter;
	while (posSlash != string::npos) {
		posEnter = fileTextBuffer->find('\n', posSlash + 2);
		if (posEnter == string::npos) {
			break;
		}
		fileTextBuffer->erase(posSlash, posEnter - posSlash + 1);
		posSlash = fileTextBuffer->find("//", posSlash);
	}
}

bool RSCParse::removeNoteForSlashStart()
{
	string::size_type posSlash = fileTextBuffer->find("/*", 0);
	string::size_type posEnter;
	while (posSlash != string::npos) {
		posEnter = fileTextBuffer->find("*/", posSlash + 2);
		if (posEnter == string::npos) {
			return false;
		}
		//删除/**/内的内容
		fileTextBuffer->erase(posSlash, posEnter - posSlash + 2);
		posSlash = fileTextBuffer->find("/*", posSlash);
	}
	//再最后一次确保代码正确
	return fileTextBuffer->find("*/", posEnter) == string::npos;
}

void RSCParse::findSuperAndClass()
{
	//找到第一个class关键字
	string::size_type posClass = fileTextBuffer->find("class", 0);
	if (posClass == string::npos) {
		throw exception("no class");
	}
	char tmpChar = fileTextBuffer->at(++posClass);
	if (!(tmpChar == ' ' && tmpChar != '\n' && tmpChar != '{' && tmpChar != ':')) {
		throw exception("class has defined is error");
	}
	//过滤空格
	while (fileTextBuffer->at(++posClass) == ' ') {
		continue;
	}
	//寻找类名
	tmpChar = fileTextBuffer->at(posClass);
	while (tmpChar != ' ' && tmpChar != '\n' && tmpChar != '{' && tmpChar != ':') {
		className.push_back(tmpChar);
		tmpChar = fileTextBuffer->at(++posClass);
	}
	//寻找父类，如果存在
	//能否找到：号
	string::size_type pos2Dot = fileTextBuffer->find(':', posClass);
	//为了能正确找到父类名字，需要如下约束条件，即{的位置要大于:的位置
	string::size_type posLeft = fileTextBuffer->find('{', posClass);
	if (posLeft > pos2Dot) {
		//存在父类
		//过滤空格和回车
		tmpChar = fileTextBuffer->at(++pos2Dot);
		while (tmpChar == ' ' || tmpChar == '\n') {
			continue;
		}
		//寻找父类名字
		while (tmpChar != ' ' && tmpChar != '\n' && tmpChar != '{') {
			superName.push_back(tmpChar);
			tmpChar = fileTextBuffer->at(++posClass);
		}
	}

}

void RSCParse::findProperties()
{
	string::size_type pos = fileTextBuffer->find("__property(", 0);
	string _type, _property;
	while (pos != string::npos) {
		pos += 11;
		if (parseProperty(_type, _property, pos) == true) {
			properties.push_back({ _type,_property });
		}
		pos = fileTextBuffer->find("__property(", pos);
	}
}

bool RSCParse::parseProperty(string &typeName, string &propertyName, size_t _Off)
{
	string::size_type pos = fileTextBuffer->find(',', _Off);
	if (pos == string::npos) {
		return false;
	}
	typeName = fileTextBuffer->substr(_Off, pos - _Off);
	//过滤空格
	while (fileTextBuffer->at(++pos) == ' ') {
		continue;
	}
	_Off = fileTextBuffer->find(')', pos);
	if (_Off == string::npos) {
		return false;
	}
	propertyName = fileTextBuffer->substr(pos, _Off - pos);
	return true;
}

void RSCParse::findMethod()
{
	
}
bool RSCParse::findTextInBracketsAndMethod(size_t _Off_Left, size_t _Off_Right, struct SpecialString *out, struct SpecialString *outMethod)
{
	//匹配参数：(?<=\()(\S*\s*)*(?=\))
	//完全匹配：
	//((static|friend|virtual)\s*)*((volatile|const)\s*)*((\w+)\s*([\*,&]*)\s*(\w+)\s*)(\(\s*)(\s*(\w+)\s*([\*,&,\[,\]]*)\s*(\,?|(\w+)\s*))*\)\s*(;|const;|(const\s*\{)|\{)
	//如果参数中使用了()，将会导致结果不可预见
	out->raw_data = const_cast<char *>((fileTextBuffer->begin() + _Off_Left)._Ptr);
	out->size = _Off_Right - _Off_Left + 1;

	return false;
}


//从off位置逆寻找要求的字符
size_t findInverse(const string &str, char finder, size_t _Off = string::npos)
{
	if (_Off > str.size()) {
		_Off = str.size() - 1;
	}
	for (;_Off >= 0; --_Off) {
		if (str.at(_Off) == finder) {
			return _Off;
		}
	}
	return string::npos;
}