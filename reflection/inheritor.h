#pragma once
#include "Baser.h"
CHE_NAMESPACE_BEGIN
class inheritor :public Baser
{
public:
	inheritor();
	virtual ~inheritor();
	GETSETTER(bool, fly,Fly)
public:
	static Reflection<Baser> *_reflection_;
};
CHE_NAMESPACE_END
