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
	return method != nullptr && method->isProperty() ? method : nullptr;
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

cpp_method* find_overload_method_by_name(cpp_method *lastone, Class clss, const vector<size_t> &arguments)
{
	const char *funcName = lastone->func_name;

	auto index = find_if(clss->functionlist->begin(), clss->functionlist->end(),
		[=](const cpp_method &val) {return lastone == &val; });
	if (index != clss->functionlist->end()) {
		index = find_if(index + 1, clss->functionlist->end(),
			[=](const cpp_method &val)
		{
			if (strcmp(funcName, val.func_name) == 0) {
				return checkCompatibleArguments(arguments, &val);
			}
			return false;

		});
	}
	return index != clss->functionlist->end() ? &(*index) : nullptr;
}

bool checkCompatibleArguments(const vector<size_t> &arguments, const cpp_method *method)
{
	static size_t *pointer;
	pointer = method->params;
	bool ret = true;
	if (arguments.size() == method->paramsCount) {
		for each(auto &val in arguments) {
			if (val != *pointer++) {
				ret = false;
				break;
			}
		}
	}
	else {
		ret = false;
	}
	return ret;
}

CHE_NAMESPACE_END