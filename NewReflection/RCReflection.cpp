/****************************************************************************
** Reflection code from reading C++ file G:\git\reflection\rsc\inheritor.h
**
** created by RSC. author: CHE
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
//#include "path"
#include "RCReflection.h"
CHE_NAMESPACE_BEGIN

//���з����б�
static vector<cpp_method> methodlist_RCReflection = {
	{0,"add",Function_Addr_Union<decltype(&RCReflection::add)>::addr_value(&RCReflection::add)},
	{ PROPERTY_FLAG,"flags",Function_Addr_Union<decltype(&RCReflection::flags)>::addr_value(&RCReflection::flags) }
};
// �����б�
// static vector<cpp_method*> properlist_RCReflection = {
// 	&(methodlist_RCReflection[0])
// };
//call����
static void* call_RCReflection[2] = { 0 };

static ObjectdefsPrivate::StayOrigin<id>::Type id_RCReflection_DATA = {
	//Class���ڴ����п����ж��Ƿ��м̳з����࣬����Ϊ0
	0,
	"__RCRCReflection",
	&methodlist_RCReflection,
};
id RCReflection::RCReflectionid = &id_RCReflection_DATA;
RCReflection::RCReflection()
{
}


RCReflection::~RCReflection()
{
}
const char * RCReflection::getClassname()
{
	return id_RCReflection_DATA.isa.classname;
}

Class RCReflection::superClass()
{
	return id_RCReflection_DATA.isa.superClass;
}

bool RCReflection::isKindOfClass(Class class_object)
{
	return superClass() == class_object;
}

Class RCReflection::getClass()
{
	return &id_RCReflection_DATA.isa;
}
CHE_NAMESPACE_END