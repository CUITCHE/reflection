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

static size_t * __function_params_hashcode_0() {
	static size_t hashcode[] = { typeid(int).hash_code(),typeid(int).hash_code(),typeid(std::reference_wrapper<int>).hash_code() };
	return hashcode;
}
static size_t * __function_params_hashcode_1() {
	static size_t hashcode[] = { typeid(int).hash_code() };
	return hashcode;
}
static size_t * __function_params_hashcode_2() {
	static size_t hashcode[] = { typeid(int).hash_code(),typeid(int).hash_code(),typeid(int).hash_code() };
	return hashcode;
}
static size_t * __function_params_hashcode_3() {
	static size_t hashcode[] = { typeid(double).hash_code(),typeid(int).hash_code(),typeid(double).hash_code() };
	return hashcode;
}
//所有方法列表
static vector<cpp_method> methodlist_RCReflection = {
	//int add(int a, int &b)
	{ Overload,3,"add",Function_Addr_Union<int(RCReflection::*)(int,int&)const>::addr_value(&RCReflection::add),__function_params_hashcode_0() },
	//int add(int a, int b)
	{ Overload,3,"add",Function_Addr_Union<int(RCReflection::*)(int,int)>::addr_value(&RCReflection::add),__function_params_hashcode_2() },
	//double sub(int a, double b)
	{ Static,3,"sub",Function_Addr_Union<double(*)(int,double)>::addr_value(&RCReflection::sub),__function_params_hashcode_3() },
	//property flags
	{ Property,1,"flags",Function_Addr_Union<decltype(&RCReflection::flags)>::addr_value(&RCReflection::flags),__function_params_hashcode_1() }
};

//class structure
static ObjectdefsPrivate::StayOrigin<Class>::Type class_RCReflection = {
	//Class，在代码中可以判断是否有继承反射类，这里为0
	0,
	"__RCRCReflection",
	&methodlist_RCReflection
};

//cpp_object
static ObjectdefsPrivate::StayOrigin<id>::Type object_RCReflection = {
	&class_RCReflection
};

//id pointer
static id id_RCReflection_DATA = &object_RCReflection;

RCReflection::RCReflection()
{
}


RCReflection::~RCReflection()
{
}
const char * RCReflection::getClassname()
{
	return id_RCReflection_DATA->isa->classname;
}

Class RCReflection::superClass()
{
	return id_RCReflection_DATA->isa->superClass;
}

bool RCReflection::isKindOfClass(Class class_object)
{
	Class clss = superClass();
	if (clss == nullptr) {
		return false;
	}
	bool ret = false;
	while (clss != nullptr) {
		if (clss == class_object) {
			ret = true;
			break;
		}
		clss = clss->superClass;
	}
	return ret;
}

Class RCReflection::getClass()
{
	return id_RCReflection_DATA->isa;
}
CHE_NAMESPACE_END