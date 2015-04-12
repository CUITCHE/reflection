#ifndef Reflection_H__
#define Reflection_H__

/********************************************************************
	created:	2015/04/10
	created:	10:4:2015   23:41
	file base:	Reflection
	author:		CHE

	purpose:	反射机制 implement
				在正确使用反射类的同时，需要把具有反射功能的类都有实例化一遍，
				不然有些功能不能正常使用。
*********************************************************************/
#include "hglobal.h"
#include <map>
#include <mutex>
using std::mutex;
using std::map;
using std::make_pair;
CHE_NAMESPACE_BEGIN
//属性声明。包含变量定义以及get-set方法的声明
#define property(__type__, _propertyName_)\
public:\
__type__ __##_propertyName_;\
public:\
void _propertyName_(const __type__ &val);\
const __type__& _propertyName_()const;\
__type__& _propertyName_()

//属性的get-set方法实现，默认实现
#define synthesize(class_name, _propertyName_)\
void class_name::_propertyName_(const decltype(__##_propertyName_) &val){this->__##_propertyName_ = val;}\
const decltype(__##_propertyName_)& class_name::_propertyName_()const{return this->__##_propertyName_;}\
decltype(__##_propertyName_)& class_name::_propertyName_(){return this->__##_propertyName_;}

//默认实现方法/*void _propertyName_(const __type__ &val) { this->__##_propertyName_ = val; }\*/
#define property_synthesize(__type__, _propertyName_) \
public:\
__type__& _propertyName_(){return this->__##_propertyName_;}\
private:\
__type__ __##_propertyName_

//反射类声明开始
#ifndef reflection_declare
#define static_obj s_##class_name##_reflection_obj
#define reflection_declare(class_name, super_class_name)\
class class_name :public super_class_name\
{public:\
	Reflection<class_name> **reflection_obj;\
	class_name();\
	static Class getClass();\
	virtual Class superClass()override;\
	virtual bool isKindOfClass(Class class_object)override;\
	static const char* cpp_getClassName();\
template<typename T>\
void setValueForKey(const char *propertyname, const T &val) {\
	(*reflection_obj)->setValue_forKey(this, val, propertyname);\
}\
template<typename T>\
const T& valueForKey(const char *propertyname) {\
	return (*reflection_obj)->value_Forkey<T>(this, propertyname);\
}\
protected:\
static void* get_class();\
virtual void add_properties()override;\
public:
#endif

#ifndef reflection_synthesize

#define reflection_synthesize(class_name, super_class_name)\
static Reflection<class_name> *static_obj = nullptr;\
class_name::class_name():reflection_obj(&static_obj){\
_func_addr<void(class_name::*)()> d;\
d.func = &class_name::add_properties;\
initializeReflection(this,d.addr);\
}\
Class class_name::getClass(){\
	return static_obj->id;\
}\
Class class_name::superClass() { \
return super_class_name::getClass(); \
}\
bool class_name::isKindOfClass(Class class_object) { \
return (*reflection_obj)->isSubOfClass(class_object);\
}\
const char* class_name::cpp_getClassName() {\
		auto name = typeid(class_name).name();\
		long length = strlen(name);\
		int i = length-1;\
		for (; i>=0; --i){\
			if(name[i]==':')break;\
		}\
	return name+i+1;\
}\
void* class_name::get_class(){\
return new class_name;\
}
#endif

//反射类声明结束
#ifndef reflection_declare_end
#define reflection_declare_end }
#endif

typedef struct cpp_class *Class;
typedef struct cpp_obj_list *Objts;
struct _property_;
struct _property_list;

struct cpp_class {
	Class super_class;		//父类isa结构
	char *name;				//类名字
	long property_count = 0;//类的所有属性的个数
	_property_list *properties = 0;	//属性列表
	DWORD get_class_method;		//保存创建这个类的实例函数的地址。

};

struct _property_list {
	_property_ *_property = nullptr;
	_property_list *next = nullptr;
};

struct _property_ {
	char *property_name;
	long property_name_length;	//属性名的长度
	char *property_type;		//运行时的类型名
	DWORD property_method;
};

struct cpp_obj_list {
	Class isa = nullptr;
	cpp_obj_list *next = nullptr;
};
static Objts class_list = new ObjectdefsPrivate::StayOrigin<Objts>::Type;

//如果你不打算在多线程的情况下使用反射，请定义宏NO_MULITHREAD，
//以此来提升反射在单线程中的效率
#ifndef NO_MULITHREAD
#define reflection_locker(mtx) __locker(mtx)
#else
#define reflection_locker(mtx)
#endif // !NO_MULITHREAD

//根据类的名字获取类的实例，如果没有就会返回nullptr，注意内存泄漏问题
void* cpp_getClass_instance(const char *class_name);

//返回Class结构的所有被添加过的属性名称，二维形式返回，请不要管理这份内存，否则会出现系统崩溃问题！。
//尤其，请注意，是这个id下的属性名称的二维，且不包含它的父类结构的属性
//也会返回属性的个数
const char** cpp_getClass_properties(const Class id, long &count);

/*
使用Meta类时，类一定要继承HObject
*/

template<typename ClassName>
struct Reflection final
{
	typedef ClassName class_type;
	Class id;
	Reflection(ClassName *obj);

	/*析构函数，不考虑做析构任务。因为meta_obj是全局存在的。*/
	~Reflection() {}

	//增加属性，传入：属性名，get方法地址，set方法地址
	template<typename PropertyFunc>
	void add_property(const char *property_name, PropertyFunc get_set);

	//调用set方法，传入：要设置的对象指针，值，属性名
	template<typename ValueType>
	bool setValue_forKey(class_type *obj, const ValueType &value, const char *property_name);

	//调用get方法
	template<typename RET>
	const RET& value_Forkey(class_type *obj, const char *property_name);

	//this的id是class_object的子类就返回true
	bool isSubOfClass(Class class_object);
protected:
	void initilaize_superclass(class_type *obj);

	//通过属性名来找到_property_结构的指针，并返回
	_property_* find_property_by_name(const char *name);

	template<typename T>
	void setValue_impl(DWORD class_address, DWORD setFunc, const T &val);

	template<typename T>
	const T& value_impl(DWORD class_address, DWORD getFunc);

	//比较ValueType和_property_结构里面的类型名是否一样，不一样则返回false
	//若指针为nullptr，也会返回false
	template<typename ValueType>
	bool type_compare(const _property_ *_property);

	//向全局Objts结构体插入id，形成链表。
	void insert_Class_to_Objts();
private:
	//property_finder_cache中的内存来自Class id。
	map<const char*, _property_ *> property_finder_cache;
};

template<typename ClassName>
inline Reflection<ClassName>::Reflection(ClassName * obj)
{
	id = new ObjectdefsPrivate::StayOrigin<Class>::Type;
	const char *rawName = ClassName::cpp_getClassName();
	int length = strlen(rawName) + 1;
	id->name = new char[length];
	//存下类名字
	memcpy(id->name, rawName, sizeof(char)*length);

	//创建父类指向
	initilaize_superclass(obj);
	//创建类获取实例的函数DWORD值
	DWORD d;
	__asm {
		mov eax, offset ClassName::get_class;
		mov d, eax;
	}
	id->get_class_method = d;

	//向class_list中添加子节点
	insert_Class_to_Objts();
}

template<typename ClassName>
inline bool Reflection<ClassName>::isSubOfClass(Class class_object)
{
	Class super = id->super_class;
looper:
	if (super == class_object && class_object) {
		return true;
	}
	super = super ? super->super_class : 0;
	if (super) {
		goto looper;
	}
	return false;
}

template<typename ClassName>
inline void Reflection<ClassName>::initilaize_superclass(class_type * obj)
{
	//这儿返回的是指针，
	//若T已经是顶级类，则会返回nullptr
	Class baser = obj->superClass();
	id->super_class = baser;
}

template<typename ClassName>
inline _property_ * Reflection<ClassName>::find_property_by_name(const char * name)
{
	if (name == nullptr) {
		return nullptr;
	}
#ifndef NO_MULITHREAD
	static mutex mtx;
#endif
	{
		reflection_locker(mtx);
		auto iter = property_finder_cache.find(name);
		if (iter != property_finder_cache.end()) {
			return iter->second;
		}
	}
	//在缓存中没有找到，则开始遍历
	Class ider = id;
looper1:
	_property_list *p = ider->properties;
	_property_ *node = nullptr;
	try {
		while (p) {
			node = p->_property;
			//找到
			if (strcmp(name, node->property_name) == 0) {
				goto looper2;
			}
			p = p->next;
		}
	}
	catch (...) {
		node = nullptr;
		goto looper2;
	}
	//哦，到这儿没找到，就向它的父类查询

	//自己是顶级类了，返回nullptr
	if (ider->super_class == nullptr) {
		node = nullptr;
		goto looper2;
	}
	ider = ider->super_class;
	goto looper1;
looper2:
	{
		reflection_locker(mtx);
		//向缓存中增加记录
		property_finder_cache.insert(make_pair(name, node));
	}
	return node;
}

template<typename ClassName>
inline void Reflection<ClassName>::insert_Class_to_Objts()
{
#ifndef NO_MULITHREAD
	static mutex mtx;
#endif
	//在链表首插入
	Objts node = new ObjectdefsPrivate::StayOrigin<Objts>::Type;
	node->isa = id;
	reflection_locker(mtx);
	node->next = class_list;
	class_list = node;
}

template<typename ClassName>
template <typename PropertyFunc>
inline void Reflection<ClassName>::add_property(const char * property_name, PropertyFunc get_set)
{
	//安全检查
	if (property_name == nullptr) {
		return;
	}
	//插入链首
	//_property_list *node = find_last_property();
	_property_list *node = new _property_list;
	node->_property = new _property_;
	node->next = id->properties;
	
	//改写id的属性链表的指向
	id->properties = node;

	_property_ *now_property = node->_property;

	//把空格也要算上
	int name_length = strlen(property_name) + 1;
	now_property->property_name = new char[name_length];
	now_property->property_name_length = name_length;
	//完成对名字的拷贝
	memcpy(now_property->property_name, property_name, sizeof(char)*name_length);

	typedef ObjectdefsPrivate::StayOrigin<ObjectdefsPrivate::FunctionPointer<PropertyFunc>::ReturnType>::Type arg_type;
	//完成对类型名字的拷贝
	const char *arg_type_name = typeid(arg_type).name();
	name_length = strlen(arg_type_name) + 1;
	now_property->property_type = new char[name_length];
	memcpy(now_property->property_type, arg_type_name, sizeof(char)*name_length);

	//记录get-set方法的函数地址
	_func_addr<PropertyFunc> d;
	d.func = get_set;
	now_property->property_method = d.addr;

	//属性个数+1
	++id->property_count;
}

template<typename ClassName>
template <typename ValueType>
inline bool Reflection<ClassName>::setValue_forKey(class_type * obj, const ValueType & value, const char * property_name)
{
	_property_ *the_property = find_property_by_name(property_name);
	//附带指针检查
	if (type_compare<ValueType>(the_property) == false) {
		return false;
	}
	//将指针转换成DWORD，利用汇编实现函数的调用
	DWORD class_address = (DWORD)obj;
	setValue_impl(class_address, the_property->property_method, value);
	return true;
}

template<typename ClassName>
template <typename RET>
inline const RET& Reflection<ClassName>::value_Forkey(class_type * obj, const char * property_name)
{
	_property_ *the_property = find_property_by_name(property_name);
	//附带指针检查
	if (type_compare<RET>(the_property) == false) {
		static const RET _default_ = RET();
		return _default_;
	}
	//将指针转换成DWORD，利用汇编实现函数的调用
	DWORD class_address = (DWORD)obj;
	return value_impl<RET>(class_address, the_property->property_method);
}

template<typename ClassName>
template <typename T>
inline void Reflection<ClassName>::setValue_impl(DWORD class_address, DWORD setFunc, const T & val)
{
	typedef T &(__stdcall *__SET)(void);
	__SET SET = (__SET)setFunc;
	__asm {
		mov ecx, class_address
	};
	//完成set方法的调用
	SET() = val;
}
template<typename ClassName>
template <typename T>
inline const T& Reflection<ClassName>::value_impl(DWORD class_address, DWORD getFunc)
{
	typedef T &(__stdcall *__GET)(void);
	__GET GET = (__GET)getFunc;
	__asm {
		mov ecx, class_address
	};
	//完成get方法的调用
	return GET();
}
template<typename ClassName>
template <typename ValueType>
inline bool Reflection<ClassName>::type_compare(const _property_ * _property)
{
	if (_property == nullptr) {
		fprintf(stderr, "CHE:属性不能为空，输入类型为：%s\n", typeid(ValueType).name());
		return false;
	}
	const char *name = typeid(ValueType).name();
	if (strcmp(name, _property->property_type) == 0) {
		return true;
	}

	fprintf(stderr, "CHE:属性：%s，类型不匹配，输入类型为：%s，程序期望类型：%s，\n", _property, 
		typeid(ValueType).name(), _property->property_type);
	return false;
}
CHE_NAMESPACE_END

#endif // Reflection_H__


