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
	auto name = typeid(Baser).name();
	long length = strlen(name);
	int i = length - 1;
	for (; i >= 0; --i) {
		if (name[i] == ':')break;
	}
	return name + i+1;
}
void Baser::add_properties()
{
	add_property(this, "number", &Baser::number, "price", &Baser::price);
	//add_property(this, "price", &Baser::price);
}

void * Baser::get_class()
{
	return new Baser;
}


CHE_NAMESPACE_END