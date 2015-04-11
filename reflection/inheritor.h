#pragma once
#include "Baser.h"
#include <string>
using namespace std;
CHE_NAMESPACE_BEGIN

CHE_REFLECTION_BEGIN(Inheritor, Baser)
public:
	~Inheritor()override{}
	void add_properties() override { 
		add_property(this, "fly", &Inheritor::fly);
		add_property(this, "reader", &Inheritor::reader);
	}

	property_synthesize(bool, fly);
	property_synthesize(string, reader);
CHE_REFLECTION_END

CHE_NAMESPACE_END
