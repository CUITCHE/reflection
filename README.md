# Reflection
我很羡慕Java，Objective-C有自己的反射机制，但是，毕竟是C++，还有什么是C++不能做的？花两个下午+晚上的时间，撸出目前这个反射版本。功能还有待完善，如果有bug，也是情有可原，请各位网友大牛不吝赐教。

利用模板和虚函数建立C++的反射机制。

在每一种需要遍历含有Class结构的链表中，建立映射表，把每一次查询的结果缓存进去，加快第二次再查询的速度。

但是，使用这种机制的前提是，所有具有反射功能的类需要提前被使用过，你可以在程序最开始前把这些反射类都new一遍。

果然，不是自己写的编译器，这些工作只能交给程序员来完成了。

我也期待，C++的标准中能出现反射这种机制吧。

##反射机制的核心底层结构
```CPP
typedef struct cpp_class *Class;
typedef struct cpp_obj_list *Objts;
struct _property_;
struct _property_list;

struct cpp_class {
	Class super_class;		//父类isa结构
	char *name;				//类名字
	long property_count = 0;//类的所有属性的个数
	_property_list *properties;	//属性列表
	DWORD get_class_method;		//保存创建这个类的实例函数的地址。
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
```
##如何创建一个反射类
在我的代码中，我定义了一个基类，名为Baser。这个基类拥有具有反射机制的类的所有方法。为方便定义这样的反射类，我定义了一个宏，`reflection_declare`和`reflection_declare_end`，你可以用这一对宏完成反射类的声明，用`reflection_synthesize`完成反射类的定义。比如我代码中的例子：
```CPP
reflection_declare(Inheritor, Baser)
	~Inheritor()override{}
	property_synthesize(bool, fly);
	property_synthesize(string, reader);
reflection_declare_end;
```

析构函数被单独拿出来了，我认为，反射类可能需要自己去清理一些内存。故把析构函数单独拿出来。

###property_synthesize

通过`property_synthesize`宏来增加属性，在这个例子中会得到`bool& fly();`由于返回的是一个引用，所以你可以把这个返回值当左值或右值，都是可以的。`property_synthesize`宏也把fly变成__fly形成变量。

###add_properties

`void add_properties()`这个方法被写在`reflection_declare`中,但却没写在`reflection_synthesize`中，这一块的实现实在太不好弄了，我只好把这个属性增加单独列出来。在这个方法中，可以利用`void add_property()`模板函数来快捷增加属性。

###反射宏

####reflection_declare

reflection_declare(Inheritor, Baser)

`Inheritor`表示现在这个类的名字，而Baser表示基类，我把`reflection_declare`完整宏定义贴出来：

```CPP
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

//反射类声明结束
#ifndef reflection_declare_end
#define reflection_declare_end }
#endif
```

####reflection_synthesize

```CPP
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
```

当然，具体的细节还是要到代码中去查看。[Reflection.h](https://github.com/CUITCHE/reflection/blob/master/reflection/Reflection.h) 

##如何使用
假如，我们有Baser类和Inheritor类，它们是反射类。
Baser类有2个属性，`number<int>`,`price<double>`。
Inheritor类有1个属性，`fly<bool>`
我定义了一个Baser对象 abc和Inheritor对象inht;

###获取属性值

```CPP
int num = abc.valueForKey<int>("number");
double price = abc.valueForKey<double>("price");
```

如果找不到所给属性，就会调用相应类型的默认构造函数返回并在控制台打印出提示语，例如：

>CHE:属性不能为空，输入类型为：int

如果输入的类型与属性类型不匹配（严格匹配模式），则会有如下提示：

>CHE:number属性类型不匹配，输入类型为：double，程序期望类型：int

如果`number`的调用失败（出现上面的提示语），就会返回int()的值。


###设置属性的值

```CPP
abc.setValueForKey("number", 1568);
```

如果找不到所给属性，就什么也不会做，但依旧会在控制台打印出提示语。
###判断一个类是否是另外一个类的子类

```CPP
inht.isKindOfClass(abc.getClass());
```
是就返回true，否则返回false。

###返回一个类的所有属性
利用全局函数

```CPP
const char** cpp_getClass_properties(const Class id, long &count);
```

它接收Class和int&两个参数，返回存储属性名字的二维字符数组(char)，请不要管理它的内存，从它的返回值是可以看出，它不希望它给你的内存受到不安全的干扰（例如：delete它），count会告诉你这个Class对应的类的属性有多少个。如果发生错误，将会返回一个nullptr指针，并且count被赋值为0.
我们可以这样用：

```CPP
int count = 0;
const char **properties = cpp_getClass_properties(abc.getClass(), count);
for(int i=0; i<count; ++i){
	cout << properties[i] << endl;
}
```

###根据名字返回类的实例
很简单，利用全局函数

```CPP
void* cpp_getClass_instance(const char *class_name)
```
>这是一个模板函数，具体细节，请参考实现。class_name是类的名字。
如果名字错误，将会返回nullptr指针

或者调用类的静态成员方法

```CPP
static void* get_class();
```
>以上两个方法的返回值，请自行管理返回的内存。删除请用delete。

###准确获得类名

上一个函数，

```CPP
class_type* cpp_getClass_instance(const char *class_name)
```

其中用到了类名，类名是什么形式构成的？如何准确传参？你一定很困惑，为此我设计了一个类静态成员函数，利用编译器自带的typeid返回的name作为类名。

```CPP
static const char* cpp_getClassName();
```

```CPP
cout << Inheritor::cpp_getClassName() << endl;
```

>不出意外，将会输出'Inheritor'。

**关于如何使用反射类，请参考，代码中的[main.cpp](https://github.com/CUITCHE/reflection/blob/master/reflection/main.cpp) **