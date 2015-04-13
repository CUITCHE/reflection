#ifndef RSC_H__
#define RSC_H__

/********************************************************************
	created:	2015/04/13
	created:	13:4:2015   16:47
	file base:	RSC
	author:		CHE
	
	purpose:	RSC������������ڴʷ�����
				1.���ݴ�������·������ֻ��ģʽ���ļ��������ʷ���
				���ڵ�ǰĿ¼�´���RscFilesĿ¼��
				2.ÿ�������ɵ��ļ��ԡ�rsc_����ͷ
				3.ֻ������׺������h���ļ�
				4.�����������Ͳ��ܳ���256����
*********************************************************************/
#include <list>
#include <vector>
#include <string>
#include <fstream>
using namespace std;


#define  morphology_prefix "__CR"

struct Property
{
	string name;	//��������
	string type;	//��������
};
class RSC
{
public:
	RSC(const list<string> &fileFullNames_);
	~RSC();

	//��ʼ����
	void parse();

	//�Ƿ�û�����꣬��������ļ�û�б������ͻ᷵��true��
	//���һ�����һ�����ݣ���list��pop��ȥ��
	bool next()const;

	//��ǰ���������Ƿ�Ϸ�
	//��Ч����true
	bool isValid()const;

	//���ص�ǰ��Parse�����ļ�·��
	const string& current_filePath()const;

	//�������Ե�Property�ṹ
	const vector<Property>& get_properties()const;

	//��������
	const string& class_name()const;

protected:
	bool isRelfection(long &pos);

	//�����ݴ浽string
	void process();

	//��������
	void process_classname(long pos);

	//��������
	void process_properties();

	//�õ����Ե����ͣ����������������ĳ���
	long process_properties_type(long pos, char *&type_name);

	//�õ����Ե����֣������������ĳ���
	long process_properties_name(long pos, char * &name);

	//����ļ�������
	void get_filename(const string &fullName);
private:
	mutable list<string> fileFullNames;			//���к�׺������h���ļ�
	vector<Property> proerties;
	string the_class_name;			//����
	ifstream reader;
	string buffer;
	string _fileName;		//��ǰfile�����֣���ȫ·��
};
#endif // RSC_H__

