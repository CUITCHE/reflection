#include "RSCParse.h"
#include <exception>
#include <regex>

#define RSC_VERSION (uint32)(1<<1)
/*----------------------------------���⺯��----------------------------------*/
//��������Դ�������������ڴ�
struct SpecialString
{
	char *raw_data;
	size_t size;
	SpecialString(char *raw, size_t s) :raw_data(raw), size(s) {}
	size_t length()const { return size; }
	char *begin() { return raw_data; }
	char *end() { return raw_data + size; }
	//��ȥstr���˵Ŀհ��ַ�
	void trim() {
		char *_begin = begin();
		char *_end = end();
		//ȥ����߿հ��ַ�
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
		_begin = _begin + size - 1;	//ȡ�����ұߵ��ַ���ַ
		while (_begin >= begin()) {
			if (*_begin == ' ' || *_begin == '\t') {
				--size;
				--_begin;
				continue;
			}
			break;
		}
	}

	//����breaker�ָ�str��������list
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
	//ɾ��˫б��ע����
	removeNoteForDoubleSlash();

	//ɾ��/**/ע��
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
		//ɾ��/**/�ڵ�����
		fileTextBuffer->erase(posSlash, posEnter - posSlash + 2);
		posSlash = fileTextBuffer->find("/*", posSlash);
	}
	//�����һ��ȷ��������ȷ
	return fileTextBuffer->find("*/", posEnter) == string::npos;
}

void RSCParse::findSuperAndClass()
{
	//�ҵ���һ��class�ؼ���
	string::size_type posClass = fileTextBuffer->find("class", 0);
	if (posClass == string::npos) {
		throw exception("no class");
	}
	char tmpChar = fileTextBuffer->at(++posClass);
	if (!(tmpChar == ' ' && tmpChar != '\n' && tmpChar != '{' && tmpChar != ':')) {
		throw exception("class has defined is error");
	}
	//���˿ո�
	while (fileTextBuffer->at(++posClass) == ' ') {
		continue;
	}
	//Ѱ������
	tmpChar = fileTextBuffer->at(posClass);
	while (tmpChar != ' ' && tmpChar != '\n' && tmpChar != '{' && tmpChar != ':') {
		className.push_back(tmpChar);
		tmpChar = fileTextBuffer->at(++posClass);
	}
	//Ѱ�Ҹ��࣬�������
	//�ܷ��ҵ�����
	string::size_type pos2Dot = fileTextBuffer->find(':', posClass);
	//Ϊ������ȷ�ҵ��������֣���Ҫ����Լ����������{��λ��Ҫ����:��λ��
	string::size_type posLeft = fileTextBuffer->find('{', posClass);
	if (posLeft > pos2Dot) {
		//���ڸ���
		//���˿ո�ͻس�
		tmpChar = fileTextBuffer->at(++pos2Dot);
		while (tmpChar == ' ' || tmpChar == '\n') {
			continue;
		}
		//Ѱ�Ҹ�������
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
	//���˿ո�
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
	//ƥ�������(?<=\()(\S*\s*)*(?=\))
	//��ȫƥ�䣺
	//((static|friend|virtual)\s*)*((volatile|const)\s*)*((\w+)\s*([\*,&]*)\s*(\w+)\s*)(\(\s*)(\s*(\w+)\s*([\*,&,\[,\]]*)\s*(\,?|(\w+)\s*))*\)\s*(;|const;|(const\s*\{)|\{)
	//���������ʹ����()�����ᵼ�½������Ԥ��
	out->raw_data = const_cast<char *>((fileTextBuffer->begin() + _Off_Left)._Ptr);
	out->size = _Off_Right - _Off_Left + 1;

	return false;
}


//��offλ����Ѱ��Ҫ����ַ�
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