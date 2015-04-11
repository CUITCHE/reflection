#include "Reflection.h"
#include <mutex>
using std::mutex;
CHE_NAMESPACE_BEGIN
const Class cpp_find_class(const char *class_name)
{
	static map<const char*, Class> finder_cache;
	static mutex mtx;
	mtx.lock();
	auto iter = finder_cache.find(class_name);
	if (iter != finder_cache.end()) {
		mtx.unlock();
		return iter->second;
	}
	mtx.unlock();

	Objts p = class_list;
	Class tmp = nullptr;
	try {
		while (p != nullptr) {
			if (p->isa != nullptr) {
				if (strcmp(class_name, p->isa->name) == 0) {
					tmp = p->isa;
					break;
				}
			}
		}
	}
	catch (...) {
		tmp = nullptr;
	}
	__locker(mtx);
	finder_cache.insert(make_pair(class_name, tmp));
	return tmp;
}

const char** cpp_getClass_properties(const Class id, long &count)
{
	if (id == nullptr) {
		return nullptr;
	}
	static map<const Class, char **> properties_cache;
	static mutex mtx;
	mtx.lock();
	auto iter = properties_cache.find(id);
	if (iter != properties_cache.end()) {
		mtx.unlock();
		return (const char **)iter->second;
	}
	mtx.unlock();
	
	char **__memeory = nullptr;
	_property_list *properties = id->properties;
	_property_ *__property = nullptr;
	count = id->property_count;
	__memeory = new char *[count];
	int i = 0;
	try {
		for (i = 0; i < count; ++i) {
			__property = properties->_property;
			__memeory[i] = new char[__property->property_name_length];
			memcpy(__memeory[i], __property->property_name, sizeof(char)*__property->property_name_length);
			properties = properties->next;
		}
	}
	catch (...) {
		for (int j = 0; j < i; ++j) {
			delete[]__memeory[j];
		}
		delete[]__memeory;
		__memeory = nullptr;
		count = 0;
	}
	__locker(mtx);
	properties_cache.insert(make_pair(id, __memeory));
	return (const char **)__memeory;
}

void* cpp_getClass_instance(const char *class_name)
{
	Class tmp = cpp_find_class(class_name);
	void *instance = tmp == nullptr ? nullptr : [=]() {
		typedef void*(__stdcall*GET_INSTANCE)();
		GET_INSTANCE __get = (GET_INSTANCE)tmp->get_class_method;
		return __get();
	}();
	return instance;
}

CHE_NAMESPACE_END
