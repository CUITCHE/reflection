#include "Baser.h"

CHE_NAMESPACE_BEGIN
static Reflection<Baser> *s_Baser_reflection_obj = nullptr;
Baser::Baser()
	:reflection_obj(&s_Baser_reflection_obj)
{
	DWORD func;
	__asm {
		mov eax, offset Baser::add_properties;
		mov func, eax;
	}
	initializeReflection(this,func);
}

Baser::~Baser()
{
}

Class Baser::getClass()
{
	return s_Baser_reflection_obj->id;
}

Class Baser::superClass()
{
	return (*reflection_obj) ? (*reflection_obj)->id->super_class : nullptr;
}

bool Baser::isKindOfClass(Class class_object)
{
	return (*reflection_obj)->isSubOfClass(class_object);
}

const char * Baser::cpp_getClassName()
{
	return
#ifndef NO_RSC
		"Baser";
#else
		//如果使用RSC元编译组件，每个名字前加上
		//__CR，代表CHE Reflection
		"__CRBaser";
#endif
}
void Baser::add_properties()
{
	add_property(this, "number", &Baser::number, "price", &Baser::price);
}

void * Baser::get_class()
{
	return new Baser;
}


CHE_NAMESPACE_END