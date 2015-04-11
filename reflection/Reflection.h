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
#ifndef CHE_REFLECTION_BEGIN
#define CHE_REFLECTION_BEGIN(class_name, super_class_name)\
class class_name;\
static Reflection<class_name> *s_##class_name##_reflection_obj = nullptr;\
class class_name :public super_class_name\
{public:\
Reflection<class_name> **reflection_obj;\
class_name() :reflection_obj(&s_##class_name##_reflection_obj){\
	initializeReflection(this);\
}\
virtual ~class_name()override {}\
static Class getClass() { return s_##class_name##_reflection_obj->id; }\
Class superClass()override { return super_class_name::getClass(); }\
bool isKindOfClass(Class class_object)override { return (*reflection_obj)->isSubOfClass(class_object); }\
template<typename T>\
void setValueForKey(const char *propertyname, const T &val) {\
	(*reflection_obj)->setValue_forKey(this, val, propertyname);\
}\
template<typename T>\
const T& valueForKey(const char *propertyname) {\
	return (*reflection_obj)->value_Forkey<T>(this, propertyname);\
}\
protected:\
static class_name* get_class(){return new class_name;}
#endif

//反射类声明结束
#ifndef CHE_REFLECTION_END
#define CHE_REFLECTION_END };
#endif

typedef struct cpp_class *Class;
typedef struct cpp_obj_list *Objts;
struct _property_;
struct _property_list;

struct cpp_class {
	Class super_class;		//父类isa结构
	char *name;				//类名字
	long property_count = 0;//类的大小
	_property_list *properties;	//属性列表
	DWORD get_class_method;		//保存创建这个类的实例函数的地址。一般为Lambda表达式形成的

};

struct _property_list {
	_property_ *_property;
	_property_list *next;
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

//根据类返回类的全名字
template<typename class_type>
const char* cpp_getClassName() {
	return typeid(class_type).name();
}

//根据类的名字获取类的实例，如果没有就会返回nullptr，注意内存泄漏问题
template<typename class_type>
class_type* cpp_getClass(const char *class_name) {
	Class tmp = cpp_find_class(class_name);
	class_type *instance = tmp == nullptr ? nullptr : [=]() {
		typedef class_type*(__stdcall*GET_INSTANCE)();
		GET_INSTANCE __get = (GET_INSTANCE)tmp->get_class_method;
		return __get();
	}();
}

//在链表中查找class_name的Class结构，并将结构存储在cache中
const Class cpp_find_class(const char *class_name);

//返回Class结构的所有被添加过的属性名称，二维形式返回，请不要管理这份内存，否则会出现系统崩溃问题！。
//尤其，请注意，是这个id下的属性名称的二维，且不包含它的父类结构的属性
//也会返回属性的个数
const char** cpp_getClass_properties(const Class id, long &count);

/*
使用Meta类时，类一定要继承HObject
*/
#include <map>
using std::map;
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

	//返回最后一个属性。
	//所以，此时的属性中的next为空
	_property_list* find_last_property();

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
	void insert_class_to_Objts();
private:
	//property_finder_cache中的内存来自Class id。
	map<const char*, _property_ *> property_finder_cache;
};

template<typename ClassName>
inline Reflection<ClassName>::Reflection(ClassName * obj)
{
	id = new ObjectdefsPrivate::StayOrigin<Class>::Type;
	const char *rawName = typeid(ClassName).name();
	int length = strlen(rawName) + 1;
	id->name = new char[length];
	//存下类名字
	memcpy(id->name, rawName, sizeof(char)*length);
	//初始化属性链表结构
	id->properties = new _property_list;
	id->properties->next = nullptr;
	id->properties->_property = nullptr;
	id->super_class = nullptr;
	initilaize_superclass(obj);
	//处理类实例创建Lambda函数
	DWORD d;
	__asm {
		mov eax, offset ClassName::get_class
		mov d,eax
	}
	id->get_class_method = d;

	//向class_list中添加子节点
	insert_class_to_Objts();
}

template<typename ClassName>
inline bool Reflection<ClassName>::isSubOfClass(Class class_object)
{
	Class super = id->super_class;
looper:
	if (super == class_object) {
		return true;
	}
	super = super->super_class;
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
inline _property_list * Reflection<ClassName>::find_last_property()
{
	_property_list *p = id->properties;
	while (p->next != nullptr) {
		p = p->next;
	}
	return p;
}

template<typename ClassName>
inline _property_ * Reflection<ClassName>::find_property_by_name(const char * name)
{
	auto iter = property_finder_cache.find(name);
	if (iter != property_finder_cache.end()) {
		return iter->second;
	}
	//在缓存中没有找到，则开始遍历
	Class ider = id;
looper1:
	_property_list *p = ider->properties;
	_property_ *node = nullptr;
	while (p) {
		node = p->_property;
		//找到
		if (strcmp(name, node->property_name) == 0) {
			goto looper2;
		}
		p = p->next;
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
	//向缓存中增加记录
	property_finder_cache.insert(make_pair(name, node));
	return node;
}

template<typename ClassName>
inline void Reflection<ClassName>::insert_class_to_Objts()
{
	Objts p = class_list;
	while (p->next != nullptr) {
		p = p->next;
	}
	p->isa = id;
	p->next = new ObjectdefsPrivate::StayOrigin<Objts>::Type;
}

template<typename ClassName>
template <typename PropertyFunc>
inline void Reflection<ClassName>::add_property(const char * property_name, PropertyFunc get_set)
{
	//安全检查
	if (property_name == nullptr) {
		return;
	}
	//找到最后一个属性
	_property_list *node = find_last_property();
	if (node->_property != nullptr) {
		node->next = new _property_list;
		node = node->next;
		node->next = nullptr;
	}
	node->_property = new _property_;

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

	//记录get方法的函数地址
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
		fprintf(stderr, "CHE:属性不能为空，对应类型为：%s\n", typeid(ValueType).name());
		return false;
	}
	const char *name = typeid(ValueType).name();
	if (strcmp(name, _property->property_type) == 0) {
		return true;
	}

	fprintf(stderr, "CHE:类型不匹配，对应类型为：%s\n", typeid(ValueType).name());
	return false;
}
CHE_NAMESPACE_END

#endif // Reflection_H__


