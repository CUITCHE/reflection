#include "Baser.h"

CHE_NAMESPACE_BEGIN
static Reflection<Baser> *_reflection_ = nullptr;
Baser::Baser()
	:reflection(_reflection_)
{
	initializeReflection(this);
}


Baser::~Baser()
{
}

Class Baser::getClass()
{
	return reflection->id;
}

Class Baser::superClass()
{
	return reflection ? reflection->id->super_class : nullptr;
}

bool Baser::isKindOfClass(Class class_object)
{
	return reflection->isSubOfClass(class_object);
}

void Baser::initializeReflection(Baser *obj)
{
	if (_reflection_ == nullptr) {
		_reflection_ = new Reflection<Baser>(obj);
		reflection = _reflection_;
		add_property("Number", &Baser::getNumber, &Baser::setNumber);
		add_property("Price", &Baser::getPrice, &Baser::setPrice);
	}
}


CHE_NAMESPACE_END