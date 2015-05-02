#include "refc.h"
#include <algorithm>
using namespace std;
CHE_NAMESPACE_BEGIN

cpp_method* find_property_by_name(const char *name, Class clss)
{
	if (name == nullptr) {
		return nullptr;
	}
	return find_property_by_name_impl(name, clss);
}

cpp_method* find_property_by_name_impl(const char *name, Class clss)
{
	cpp_method *method = find_method_impl(name, clss);
	return method != nullptr && method->callIndex == PROPERTY_FLAG ? method : nullptr;
}

cpp_method* find_method_by_name(const char *name, Class _id)
{
	if (name == nullptr) {
		return nullptr;
	}
	cpp_method *method = find_method_impl(name, _id);
	return method;
}

cpp_method* find_method_impl(const char *name, Class clss)
{
	static map<const char*, cpp_method*> _cache;
	auto iter = _cache.find(name);
	if (iter != _cache.end()) {
		return iter->second;
	}
	auto index = find_if(clss->functionlist->begin(), clss->functionlist->end(),
		[=](const cpp_method &val) {return !strcmp(name, val.func_name); });

	return index != clss->functionlist->end() ? 
		_cache.insert(make_pair((*index).func_name, &(*index))), &(*index) : nullptr;
}

CHE_NAMESPACE_END