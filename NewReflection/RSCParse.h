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

//�����ṹ��
struct Method 
{
	size_t mem_limit_symbol : 1;	//��Ա�����޶���:const
	size_t static_symbol : 1;		//��̬��Ա����
	size_t _zero : 30;
	string return_type;		//����ֵ�����磺cosnt int, int, int *, int &, const int*,const int&
	string function_name;	//������
	vector<string> arguments;//���������б�
	Method():mem_limit_symbol(0),static_symbol(0){}
};

//���Խṹ��
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
	//�ڿ�ʼ����ǰ������׼�����������ܻ��׳��쳣��if failed��exception
	void readyForParse();

	//׼������֮���ж���ݴ����Ƿ������������
	bool isValid()const;

	//������л���������
	void clear();

	//���з������������ѽ���洢����������ʧ�ܽ��׳��쳣
	void parse();

	//���ø��������
	//���ڸ�����ܲ������Է���ϵͳ�����Կ������ⲿ��Ϊ��
	void setSuperClassNameNull()const;

	//���ɴ���
	void generateCode();
protected:
	//���ע������
	void removeNoteContext();

	//ɾ��˫б��ע��
	void removeNoteForDoubleSlash();

	//ɾ��/**/ע��
	bool removeNoteForSlashStart();
protected:
	//����super
	void findSuperAndClass();

	//Ѱ�����ԣ������뻺����
	void findProperties();

	//_Off����ʶ���͵����
	bool parseProperty(string &typeName, string &propertyName, size_t _Off);

	//Ѱ�ҷ���
	void findMethod();

	//* \brief Ѱ��һ�������ڵ�text���п����Ǻ����Ĳ�����Ҳ�п����Ǻ�Ĳ�����֮����ж�
	//* \param _Off_Left���������ұߵ�һ��λ��
	//* \param _Off_Right����������ߵĵ�һ��λ��
	//* \param out������һ��SpecialString
	//* \param outMethod������һ��SpecialString����returnΪtrueʱ��Ч��Ϊ����ǰ�沿�ֵ�����
	//* \fn ������Ѱ����fileTextBuffer��[_Off_Left, _Off_Right]��Χ�ڵ�text�������ؿ����Ǻ���ͷ����Ϣ
	//* \return��true��Ҳ����һ����ȷ�ĺ�����������
	bool findTextInBracketsAndMethod(size_t _Off_Left, size_t _Off_Right, struct SpecialString *out, struct SpecialString *outMethod);
private:
	mutable string superName;	//���������
	mutable string className;	//����
	string *fileTextBuffer;
	string *writeTextBuffer;	//���ɵĴ��봮������
	vector<Method> methods;		//����
	vector<PropertyMethod> properties;//���Է���
};

