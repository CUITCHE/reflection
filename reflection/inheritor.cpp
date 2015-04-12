#include "Inheritor.h"

CHE_NAMESPACE_BEGIN

reflection_synthesize(Inheritor, Baser);

void Inheritor::add_properties() {
	add_property(this, "fly", &Inheritor::fly, "reader", &Inheritor::reader);
}
CHE_NAMESPACE_END