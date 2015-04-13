/****************************************************************************
** Reflection code from reading C++ file G:\git\reflection\rsc\inheritor.h
** 
** created by RSC. author: CHE
** 
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "G:\git\reflection\rsc\inheritor.h"
CHE_NAMESPACE_BEGIN

#ifndef NO_RSC
const char * Inheritor::cpp_getClassName()
{
	return "__CRInheritor";
}

void Inheritor::add_properties()
{
	//type:bool
	add_property(this, "fly", &Inheritor::fly);
	//type:string
	add_property(this, "reader", &Inheritor::reader);
}

#endif
CHE_NAMESPACE_END