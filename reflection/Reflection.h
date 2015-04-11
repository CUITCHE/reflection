#ifndef Reflection_H__
#define Reflection_H__

/********************************************************************
	created:	2015/04/10
	created:	10:4:2015   23:41
	file base:	Reflection
	author:		CHE
	
	purpose:	������� implement
*********************************************************************/
#include "hglobal.h"
#include <map>
using std::map;
CHE_NAMESPACE_BEGIN
//�����������������������Լ�get-set����������
#define property(__type__, _propertyName_)\
public:\
__type__ __##_propertyName_;\
public:\
void _propertyName_(const __type__ &val);\
const __type__& _propertyName_()const;\
__type__& _propertyName_()

//���Ե�get-set����ʵ�֣�Ĭ��ʵ��
#define synthesize(class_name, _propertyName_)\
void class_name::_propertyName_(const decltype(__##_propertyName_) &val){this->__##_propertyName_ = val;}\
const decltype(__##_propertyName_)& class_name::_propertyName_()const{return this->__##_propertyName_;}\
decltype(__##_propertyName_)& class_name::_propertyName_(){return this->__##_propertyName_;}

//Ĭ��ʵ�ַ���/*void _propertyName_(const __type__ &val) { this->__##_propertyName_ = val; }\*/
#define property_synthesize(__type__, _propertyName_) \
public:\
__type__& _propertyName_(){return this->__##_propertyName_;}\
private:\
__type__ __##_propertyName_

//������������ʼ
#ifndef CHE_REFLECTION_BEGIN
#define CHE_REFLECTION_BEGIN(class_name, super_class_name)\
class class_name;\
static Reflection<class_name> *s_##class_name##_reflection_obj = nullptr;\
class class_name :public super_class_name\
{public:\
class_name() :reflection_obj(&s_##class_name##_reflection_obj){\
	initializeReflection(this);\
}\
virtual ~class_name()override {}\
Class getClass()override { return (*reflection_obj)->id; }\
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
Reflection<class_name> **reflection_obj;
#endif

//��������������
#ifndef CHE_REFLECTION_END
#define CHE_REFLECTION_END };
#endif

typedef struct obj_class *Class;
struct _property_;
struct _property_list;

struct obj_class {
	Class super_class;	//����isa�ṹ
	char *name;	//������
	long instance_size;	//��Ĵ�С
	_property_list *properties;	//�����б�
};

struct _property_list {
	_property_ *_property;
	_property_list *next;
};

struct _property_ {
	char *property_name;
	char *property_type;	//����ʱ��������
	DWORD property_method;
};

/*
ʹ��Meta��ʱ����һ��Ҫ�̳�HObject
*/
#include <map>
using std::map;
template<typename ClassName>
struct Reflection final
{
	typedef ClassName class_type;
	Class id;
	Reflection(ClassName *obj);

	/*����������������������������Ϊmeta_obj��ȫ�ִ��ڵġ�*/
	~Reflection() {}

	//�������ԣ����룺��������get������ַ��set������ַ
	template<typename PropertyFunc>
	void add_property(const char *property_name, PropertyFunc get_set);

	//����set���������룺Ҫ���õĶ���ָ�룬ֵ��������
	template<typename ValueType>
	bool setValue_forKey(class_type *obj, const ValueType &value, const char *property_name);

	//����get����
	template<typename RET>
	const RET& value_Forkey(class_type *obj, const char *property_name);

	//this��id��class_object������ͷ���true
	bool isSubOfClass(Class class_object);
protected:
	void initilaize_superclass(class_type *obj);

	//�������һ�����ԡ�
	//���ԣ���ʱ�������е�nextΪ��
	_property_list* find_last_property();

	//ͨ�����������ҵ�_property_�ṹ��ָ�룬������
	_property_* find_property_by_name(const char *name);

	template<typename T>
	void setValue_impl(DWORD class_address, DWORD setFunc, const T &val);

	template<typename T>
	const T& value_impl(DWORD class_address, DWORD getFunc);

	//�Ƚ�ValueType��_property_�ṹ������������Ƿ�һ������һ���򷵻�false
	//��ָ��Ϊnullptr��Ҳ�᷵��false
	template<typename ValueType>
	bool type_compare(const _property_ *_property);
private:
	//property_finder_cache�е��ڴ�����Class id��
	map<const char*, _property_ *> property_finder_cache;
};

template<typename ClassName>
inline Reflection<ClassName>::Reflection(ClassName * obj)
{
	id = new obj_class;
	id->instance_size = sizeof ClassName;
	const char *rawName = typeid(ClassName).name();
	int length = strlen(rawName) + 1;
	id->name = new char[length];
	memcpy(id->name, rawName, sizeof(char)*length);

	id->properties = new _property_list;
	id->properties->next = nullptr;
	id->properties->_property = nullptr;
	id->super_class = nullptr;
	initilaize_superclass(obj);
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
	//������ص���ָ�룬
	//��T�Ѿ��Ƕ����࣬��᷵��nullptr
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
	//�ڻ�����û���ҵ�����ʼ����
	Class ider = id;
looper1:
	_property_list *p = ider->properties;
	_property_ *node = nullptr;
	while (p) {
		node = p->_property;
		//�ҵ�
		if (strcmp(name, node->property_name) == 0) {
			//�򻺴������Ӽ�¼
			property_finder_cache.insert(make_pair(node->property_name, node));
			return node;
		}
		p = p->next;
	}
	//Ŷ�������û�ҵ����������ĸ����ѯ

	//�Լ��Ƕ������ˣ�����nullptr
	if (ider->super_class == nullptr) {
		return nullptr;
	}
	ider = ider->super_class;
	goto looper1;
}

template<typename ClassName>
template <typename PropertyFunc>
inline void Reflection<ClassName>::add_property(const char * property_name, PropertyFunc get_set)
{
	//��ȫ���
	if (property_name == nullptr) {
		return;
	}
	//�ҵ����һ������
	_property_list *node = find_last_property();
	if (node->_property != nullptr) {
		node->next = new _property_list;
		node = node->next;
		node->next = nullptr;
	}
	node->_property = new _property_;

	_property_ *now_property = node->_property;

	//�ѿո�ҲҪ����
	int name_length = strlen(property_name) + 1;
	now_property->property_name = new char[name_length];
	//��ɶ����ֵĿ���
	memcpy(now_property->property_name, property_name, sizeof(char)*name_length);

	typedef ObjectdefsPrivate::StayOrigin<ObjectdefsPrivate::FunctionPointer<PropertyFunc>::ReturnType>::Type arg_type;
	//��ɶ��������ֵĿ���
	const char *arg_type_name = typeid(arg_type).name();
	name_length = strlen(arg_type_name) + 1;
	now_property->property_type = new char[name_length];
	memcpy(now_property->property_type, arg_type_name, sizeof(char)*name_length);

	//��¼get�����ĺ�����ַ
	_func_addr<PropertyFunc> d;
	d.func = get_set;
	now_property->property_method = d.addr;
}

template<typename ClassName>
template <typename ValueType>
inline bool Reflection<ClassName>::setValue_forKey(class_type * obj, const ValueType & value, const char * property_name)
{
	_property_ *the_property = find_property_by_name(property_name);
	//����ָ����
	if (type_compare<ValueType>(the_property) == false) {
		return false;
	}
	//��ָ��ת����DWORD�����û��ʵ�ֺ����ĵ���
	DWORD class_address = (DWORD)obj;
	setValue_impl(class_address, the_property->property_method, value);
	return true;
}

template<typename ClassName>
template <typename RET>
inline const RET& Reflection<ClassName>::value_Forkey(class_type * obj, const char * property_name)
{
	_property_ *the_property = find_property_by_name(property_name);
	//����ָ����
	if (type_compare<RET>(the_property) == false) {
		static const RET _default_ = RET();
		return _default_;
	}
	//��ָ��ת����DWORD�����û��ʵ�ֺ����ĵ���
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
	//���set�����ĵ���
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
	//���get�����ĵ���
	return GET();
}
template<typename ClassName>
template <typename ValueType>
inline bool Reflection<ClassName>::type_compare(const _property_ * _property)
{
	if (_property == nullptr) {
		fprintf(stderr, "CHE:���Բ���Ϊ�գ���Ӧ����Ϊ��%s\n", typeid(ValueType).name());
		return false;
	}
	const char *name = typeid(ValueType).name();
	if (strcmp(name, _property->property_type) == 0) {
		return true;
	}

	printf("CHE:���Ͳ�ƥ�䣬��Ӧ����Ϊ��%s\n", typeid(ValueType).name());
	return false;
}
CHE_NAMESPACE_END
#endif // Reflection_H__


