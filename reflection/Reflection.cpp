#include "Reflection.h"

CHE_NAMESPACE_BEGIN
//在链表中查找class_name的Class结构，并将结构存储在cache中
//不对外作接口用，用户拿到了也没什么用。
const Class cpp_find_Class(const char *class_name);

const Class cpp_find_Class(const char *class_name)
{
	static map<const char*, Class> finder_cache;
#ifndef NO_MULITHREAD
	static mutex mtx;
#endif
	{
		reflection_locker(mtx);
		auto iter = finder_cache.find(class_name);
		if (iter != finder_cache.end()) {
			return iter->second;
		}
	}

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
			p = p->next;
		}
	}
	catch (...) {
		tmp = nullptr;
	}

	{
		reflection_locker(mtx);
		finder_cache.insert(make_pair(class_name, tmp));
	}
	return tmp;
}

const char** cpp_getClass_properties(const Class id, long &count)
{
	if (id == nullptr) {
		return nullptr;
	}
	static map<const Class, char **> properties_cache;
#ifndef NO_MULITHREAD
	static mutex mtx;
#endif
	{
		reflection_locker(mtx);
		auto iter = properties_cache.find(id);
		if (iter != properties_cache.end()) {
			return (const char **)iter->second;
		}
	}
	
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
	{
		reflection_locker(mtx);
		properties_cache.insert(make_pair(id, __memeory));
	}
	return (const char **)__memeory;
}

void* cpp_getClass_instance(const char *class_name)
{
	Class tmp = cpp_find_Class(class_name);
	void *instance = tmp == nullptr ? nullptr : [=]() {
		typedef void*(__stdcall*GET_INSTANCE)();
		GET_INSTANCE __get = (GET_INSTANCE)tmp->get_class_method;
		return __get();
	}();
	return instance;
}

CHE_NAMESPACE_END
