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

//所有方法列表
static vector<cpp_method> methodlist_RCReflection = {
	{0,"add",Function_Addr_Union<decltype(&RCReflection::add)>::addr_value(&RCReflection::add)},
	{ PROPERTY_FLAG,"flags",Function_Addr_Union<decltype(&RCReflection::flags)>::addr_value(&RCReflection::flags) }
};
// 属性列表
// static vector<cpp_method*> properlist_RCReflection = {
// 	&(methodlist_RCReflection[0])
// };
//call数组
static void* call_RCReflection[2] = { 0 };

static ObjectdefsPrivate::StayOrigin<id>::Type id_RCReflection_DATA = {
	//Class，在代码中可以判断是否有继承反射类，这里为0
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