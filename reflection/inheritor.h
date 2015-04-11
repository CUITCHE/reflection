#pragma once
#include "Baser.h"
CHE_NAMESPACE_BEGIN

CHE_REFLECTION_BEGIN(Inheritor, Baser)
public:
void add_properties() override { add_property(this, "fly", &Inheritor::fly); }
property_synthesize(bool, fly);
CHE_REFLECTION_END

CHE_NAMESPACE_END
