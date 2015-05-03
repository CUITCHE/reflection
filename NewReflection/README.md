#新的反射机制

##概述
前一个版本的[Reflection](https://github.com/CUITCHE/reflection/tree/master/reflection)，在对属性的支持上很不友好，需要用户编写很多代码，对一般的成员函数，静态成员函数等根本不支持，这两天心血来潮又把这个反射机制更新的一下。

##RSC工具
rsc工具还没更新，等有时间来做吧。这次虽然在编码上有一定程度的解放了双手，但对编写rsc工具的代码可不轻松。

##反射系统底层结构
> 本系统也在持续更新中，以下结构可能发生变化，请以代码[refc.h](https://github.com/CUITCHE/reflection/blob/master/NewReflection/refc.h)为准

```CPP
enum MethodFlags{
	//属性方法
	Property = 1,
	//重载方法
	Overload = 1 << 1,
	//静态方法
	Static = 1 << 2
};
struct cpp_method
{
	uint32 flag : 3;			//MethodFlags值
	uint32 paramsCount : 30;	//返回值类型+参数个数
	const char *func_name;
	DWORD func_addr;
	size_t *params;				//存储返回值和参数hashcode的数组,params[0]存储返回值类型
	bool isProperty()const { return (flag & Property) != 0; }
	bool isOverload()const { return (flag & Overload) != 0; }
	bool isStatic()const { return (flag & Static) != 0; }
};

struct cpp_class
{
	Class superClass;			//父类isa结构指针
	const char *classname;		//类名
	vector<cpp_method> *functionlist;//方法列表（包含属性的方法）
};

//每个反射类都有一个id变量，存储了类的一些信息
typedef struct cpp_object
{
	Class isa;
}*id;
```

##反射类声明
我用一小段代码说明：
```CPP
class Object [: [public|protected|private|] Baser]
{
	___refc(Baser)
public:
	Object();
	void say(int);
private:
	__property(int, flags);
	__property(string,name);
};
```

上面这段代码使用了两个宏：`___refc`和`__property`，下面我一一道来。
###___refc
* 用___refc标记一个类，表示这个类进入反射系统了。
* 用它声明后，不需要其它声明限制。
* 它的每一个方法（除去构造、析构函数、模版方法）将会进入反射类中。重载方法，成员静态方法，成员方法均可以进入反射系统
* 它可以起始于任何一个不同级别的类（顶级类或者某类的子类），如果父类使用了反射系统请如例子那样加入父类的名称，若没有请输入null（必须）。
* 反射功能只是针对某一个类，而不是它以及它的子类，必须有___refc宏修过的类才会满足。

####___refc和null宏
```CPP
#define ___refc(super) public:\
static const char* getClassname();\
static Class getClass();\
static Class superClass();\
static bool isKindOfClass(Class class_object);

#ifdef null
#undef null
#endif
#define null
```
###__property
用`__property`组合一个属性，其中包含了变量声明和属性方法定义。

比如：`__property(int, flags)`则会声明一个int的变量_flags

并定义了一个方法`int& flags(){return this->_flags;}`

####__property宏
```CPP
#ifdef __property
#undef __property
#endif // __property
#define __property(__type__, _propertyName_) public:\
__type__& _propertyName_(){return this->_##_propertyName_;}\
private:\
__type__ _##_propertyName_
```
##方法的动态调用
通俗的说法，方法动态调用就是通过函数名去调用一个方法，这样的调用只有在程序运行期才能决定调用。

在代码文件[RCReflection.h](https://github.com/CUITCHE/reflection/blob/master/NewReflection/RCReflection.h)中声明了一个RCReflection类，在[main.cpp](https://github.com/CUITCHE/reflection/blob/master/NewReflection/main.cpp)中给出了几个使用的例子。由于我的main.cpp可能每次更新都不一样，我把例子代码贴上来
```CPP
RCReflection *abc = new RCReflection;
int cc = 3;
int a = dync_call<int>(abc, "add", 1, std::ref(cc));
cout << a << endl;
a = dync_call<int>(abc, "add", 1,43);
cout << a << endl;
double b  = dync_call<double>(abc, "sub", 1, 2.3);
cout << b << endl;
```
请注意一下，如果你的函数的参数是引用类型，那么在用dync_call模版函数调用的时候，要用std::ref()包装起来，否则调用会失败。

##安全性
在用dync_call调用方法的时候，会检查参数和返回类型，参数个数不一致或类型不匹配都将造成调用失败，这是一种严格的调用机制。失败返回的将是返回类型的默认值，并会打印出相关调试信息。
