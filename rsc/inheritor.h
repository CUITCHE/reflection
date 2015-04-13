#pragma once
#include "Baser.h"
#include <string>
using namespace std;
CHE_NAMESPACE_BEGIN

reflection_declare(Inheritor, Baser)
	~Inheritor()override{}
	property_synthesize(bool, fly);
	property_synthesize(string, reader);
reflection_declare_end

CHE_NAMESPACE_END
