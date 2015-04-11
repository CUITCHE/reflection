# Reflection
我很羡慕Java，Objective-C有自己的反射机制，但是，毕竟是C++，还有什么是C++不能做的？花两个下午+晚上的时间，撸出目前这个反射版本。功能还有待完善，如果有bug，也是情有可原，请各位网友大牛不吝赐教。
利用模板和虚函数建立C++的反射机制。
在每一种需要遍历含有Class结构的链表中，简历映射表，把每一次查询的结果缓存进去，加快第二次再查询的速度。
但是，使用这种机制的前提是，所有具有反射功能的类需要提前被使用过，你可以在程序最开始前把这些反射类都new一边。
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
```
##如何创建一个反射类
在我的代码中，我定义了一个基类，名为Baser。这个基类拥有具有反射机制的类的所有方法。为方便定义这样的反射类，我定义了一个宏，`CHE_REFLECTION_BEGIN`和`CHE_REFLECTION_END`，你可以用这一对宏完成反射类的声明与部分定义。比如我代码中的例子：
```CPP
CHE_REFLECTION_BEGIN(Inheritor, Baser)
public:
	void add_properties() override { add_property(this, "fly", &Inheritor::fly); }
	property_synthesize(bool, fly);
CHE_REFLECTION_END
```
###property_synthesize
通过`property_synthesize`宏来增加属性，会得到，在这个例子中，`bool& fly();`由于返回的是一个引用，所以你可以把这个返回值当左值或右值，都是可以的。
###add_properties
`void add_properties()`这个方法是增加属性到cpp反射系统中去，需要自己去实现，怎么去简便，我暂时还没想出来。也许你可以做到，到时候，请告诉我：[我的QQ空间](http://user.qzone.qq.com/839663632/)。

###CHE_REFLECTION_BEGIN
`Inheritor`表示现在这个类的名字，而Baser表示基类，我把`CHE_REFLECTION_BEGIN`完整宏定义贴出来：

```CPP
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
`CHE:属性不能为空，对应类型为：int`
在这里，如果`number`的调用失败，就会返回int()的值。

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
class_type* cpp_getClass(const char *class_name)
```
这是一个模板函数，具体细节，请参考实现。class_name是类的名字。
如果名字错误，将会返回nullptr指针

###准确获得类名
上一个函数，
```CPP
class_type* cpp_getClass(const char *class_name)
```
其中用到了类名，类名是什么形式构成的？如何准确传参？你一定很困惑，为此我设计了一个函数，利用编译器自带的typeid返回的name作为类名。
```CPP
const char* cpp_getClassName()
```
这也是一个模板函数，可以这样使用
```CPP
const char* name = cpp_getClassName<int>();
```
不出意外，返回的是'int'。
