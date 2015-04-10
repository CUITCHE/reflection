#ifndef HObjectdefs_conn_H__
#define HObjectdefs_conn_H__

/********************************************************************
	Copyright (C) 2014 CUIT·CHE.
	All rights reserved.
	Contact: 	CHE (hejun_753@163.com)
	created:	2014/10/27 21:21
	author:		CHE

	purpose:	定义一些在HObject对象在关联信号槽的时候 需要用到的定义
				前面眼花缭乱的模板来自Qt公布的源码。namespace che里的
				东西全是本人所写
	*********************************************************************/
#ifndef NAMESPACE_BEGIN_CHEPRIVATE
#define NAMESPACE_BEGIN_CHEPRIVATE namespace ObjectdefsPrivate{
#else
#undef NAMESPACE_BEGIN_CHEPRIVATE
#define NAMESPACE_BEGIN_CHEPRIVATE namespace ObjectdefsPrivate{
#endif // !NAMESPACE_BEGIN_CHEPRIVATE
#ifndef NAMESPACE_END_CHEPRIVATE
#define NAMESPACE_END_CHEPRIVATE }
#else
#undef NAMESPACE_END_CHEPRIVATE
#define NAMESPACE_END_CHEPRIVATE }
#endif // !NAMESPACE_END_CHEPRIVATE

NAMESPACE_BEGIN_CHEPRIVATE
	template<bool> struct bool_trial {};
	template <typename T> struct RemoveRef { typedef T Type; };
	template <typename T> struct RemoveRef < T& > { typedef T Type; };
	template <typename T> struct RemoveConstRef { typedef T Type; };
	template <typename T> struct RemoveConstRef < const T& > { typedef T Type; };
	template <typename T> struct RemoveConst { typedef T Type; };
	template <typename T> struct RemoveConst < const T > { typedef T Type; };

	template <typename T> struct StayOrigin { typedef T Type; };
	template <typename T> struct StayOrigin < T& > { typedef T Type; };
	template <typename T> struct StayOrigin < const T& > { typedef T Type; };
	template <typename T> struct StayOrigin < const T > { typedef T Type; };
	template <typename T> struct StayOrigin < T&& > { typedef T Type; };
	template <typename T> struct StayOrigin < const T&& > { typedef T Type; };
	template <typename T> struct StayOrigin < T * > { typedef T Type; };
	template <typename T> struct StayOrigin <const T * > { typedef T Type; };
	
	template <typename T> struct PointerLevel { enum { Level = 0 }; };
	template <typename T> struct PointerLevel < T * > { enum { Level = 1 + PointerLevel<T>::Level }; };

	/*ApplyReturnValue define*/
	template <typename T>
	struct ApplyReturnValue {
		void *data;
		explicit ApplyReturnValue(void *data_) : data(data_) {}
	};
	template<typename T, typename U>
	void operator,(const T &value, const ApplyReturnValue<U> &container) {
		if (container.data)
			*reinterpret_cast<U*>(container.data) = value;
	}
	template<typename T, typename U>
	void operator,(T &&value, const ApplyReturnValue<U> &container) {
		if (container.data)
			*reinterpret_cast<U*>(container.data) = value;
	}

	template<typename T>
	void operator,(T, const ApplyReturnValue<void> &) {}
	/*List define*/
	template <typename...> struct List {};
	template <typename Head, typename... Tail> struct List < Head, Tail... > { typedef Head Car; typedef List<Tail...> Cdr; };
	template <typename, typename> struct List_Append;
	template <typename... L1, typename...L2> struct List_Append < List<L1...>, List<L2...> > { typedef List<L1..., L2...> Value; };
	template <typename L, int N> struct List_Left {
		typedef typename List_Append<List<typename L::Car>, typename List_Left<typename L::Cdr, N - 1>::Value>::Value Value;

	};
	template <typename L> struct List_Left < L, 0 > { typedef List<> Value; };

	/*FunctionPointer define*/
	template <int...> struct IndexesList {};
	template <typename IndexList, int Right> struct IndexesAppend;
	template <int... Left, int Right> struct IndexesAppend < IndexesList<Left...>, Right >{
		typedef IndexesList<Left..., Right> Value;
	};
	template <int N> struct Indexes{
		typedef typename IndexesAppend<typename Indexes<N - 1>::Value, N - 1>::Value Value;
	};
	template <> struct Indexes < 0 > { typedef IndexesList<> Value; };
	template<typename Func> struct FunctionPointer { enum { ArgumentCount = -1, IsPointerToMemberFunction = false }; };

	template <typename, typename, typename, typename> struct FunctorCall;
	template <int... II, typename... SignalArgs, typename R, typename Function>
	struct FunctorCall < IndexesList<II...>, List<SignalArgs...>, R, Function > {
		static void call(Function f, void **arg) {
			f((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II + 1]))...), ApplyReturnValue<R>(arg[0]);
		}
	};
	template <int... II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
	struct FunctorCall < IndexesList<II...>, List<SignalArgs...>, R, SlotRet(Obj::*)(SlotArgs...) > {
		static void call(SlotRet(Obj::*f)(SlotArgs...), Obj *o, void **arg) {
			(o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II + 1]))...), ApplyReturnValue<R>(arg[0]);
		}
	};
	template <int... II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
	struct FunctorCall < IndexesList<II...>, List<SignalArgs...>, R, SlotRet(Obj::*)(SlotArgs...) const > {
		static void call(SlotRet(Obj::*f)(SlotArgs...) const, Obj *o, void **arg) {
			(o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II + 1]))...), ApplyReturnValue<R>(arg[0]);
		}
	};

	template<class Obj, typename Ret, typename... Args> struct FunctionPointer < Ret(Obj::*) (Args...) >
	{
		typedef Obj Object;
		typedef List<Args...>  Arguments;
		typedef Ret ReturnType;
		typedef Ret(Obj::*Function) (Args...);
		enum { ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = true };
		template <typename SignalArgs, typename R>
		static void call(Function f, Obj *o, void **arg) {
			FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
		}
	};
	template<class Obj, typename Ret, typename... Args> struct FunctionPointer < Ret(Obj::*) (Args...) const >
	{
		typedef Obj Object;
		typedef List<Args...>  Arguments;
		typedef Ret ReturnType;
		typedef Ret(Obj::*Function) (Args...) const;
		enum { ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = true };
		template <typename SignalArgs, typename R>
		static void call(Function f, Obj *o, void **arg) {
			FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
		}
	};

	template<typename Ret, typename... Args> struct FunctionPointer < Ret(*) (Args...) >
	{
		typedef List<Args...> Arguments;
		typedef Ret ReturnType;
		typedef Ret(*Function) (Args...);
		enum { ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = false };
		template <typename SignalArgs, typename R>
		static void call(Function f, void *, void **arg) {
			FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, arg);
		}
	};

	template<typename Function, int N> struct Functor
	{
		template <typename SignalArgs, typename R>
		static void call(Function &f, void *, void **arg) {
			FunctorCall<typename Indexes<N>::Value, SignalArgs, R, Function>::call(f, arg);
		}
	};
	/*AreArgumentsCompatible define*/
	template<typename A1, typename A2> struct AreArgumentsCompatible {
		static int test(const typename RemoveRef<A2>::Type&);
		static char test(...);
		static const typename RemoveRef<A1>::Type &dummy();
		enum { value = sizeof(test(dummy())) == sizeof(int) };

	};
	template<typename A1, typename A2> struct AreArgumentsCompatible < A1, A2& > { enum { value = false }; };
	template<typename A> struct AreArgumentsCompatible < A&, A& > { enum { value = true }; };
	// void as a return value
	template<typename A> struct AreArgumentsCompatible < void, A > { enum { value = true }; };
	template<typename A> struct AreArgumentsCompatible < A, void > { enum { value = true }; };
	template<> struct AreArgumentsCompatible < void, void > { enum { value = true }; };

	/*CheckCompatibleArguments define*/
	template <typename List1, typename List2> struct CheckCompatibleArguments { enum { value = false }; };
	template <> struct CheckCompatibleArguments < List<>, List<> > { enum { value = true }; };
	template <typename List1> struct CheckCompatibleArguments < List1, List<> > { enum { value = true }; };
	template < typename Arg1, typename Arg2, typename... Tail1, typename... Tail2>
	struct CheckCompatibleArguments < List<Arg1, Tail1...>, List<Arg2, Tail2...> > {
		enum {
			value = (AreArgumentsCompatible<typename RemoveConstRef<Arg1>::Type, typename RemoveConstRef<Arg2>::Type>::value
			&& CheckCompatibleArguments<List<Tail1...>, List<Tail2...>>::value)
		};
	};
NAMESPACE_END_CHEPRIVATE
namespace che {
	enum MsgType {
		//调试信息
		DebugMsg,
		//警告信息
		WarningMsg,
		//致命信息
		FatalMsg,
		//危险性信息
		CriticalMsg,
		//一般输出性信息
		GeneralMsg
	};

	// 继承关系模版
	template<typename _Deriver, typename _Baser> struct DerivedRelationship
	{
		static int desert(_Baser *base) { return 1; }
		static char desert(void *ibase) { return 0; }
		//if _Deriver is derived by _Baser, the Value = true.
		enum { Value = (sizeof(int) == sizeof(desert((_Deriver*)nullptr))) };
	};


	// 指针安全释放模版
	inline void che_pointer_del(void) {}
	template<typename Pointer, typename... Args>
	inline void che_pointer_del(Pointer p, Args... args) {
		static_assert(ObjectdefsPrivate::PointerLevel<Pointer>::Level == 1, "Type must be a 1-level pointer!");
		if (p != nullptr)	delete p;
		che_pointer_del(args...);
	}

	// 在删除指针之前执行_Fn函数
	inline void che_pointer_before_del(void){}
	template<typename Pointer,typename _Fn, typename... Args>
	inline void che_pointer_before_del(Pointer p, _Fn func, Args... args) {
		static_assert(ObjectdefsPrivate::PointerLevel<Pointer>::Level == 1, "Type must be a 1-level pointer!");
		if (p != nullptr) {
			(p->*func)();
			delete p;
		}
		che_pointer_before_del(args...);
	}
	// 数组指针安全释放模版
	inline void che_array_pointer_del(void) {}
	template<typename T, typename... Args>
	inline void che_array_pointer_del(T p, Args... args) {
		static_assert(sizeof(p) / sizeof(ObjectdefsPrivate::StayOrigin<T>::Type) != 1 || sizeof(ObjectdefsPrivate::StayOrigin<T>::Type) == 4, "Fixed array must need be deleted by system!");
		if (p != nullptr)	delete[]p;
		che_dyadic_array_del(args...);
	}

	// 二维数组指针安全释放模版
	inline void che_dyadic_array_del(void) {}
	template<typename T, typename... Args>
	inline void che_dyadic_array_del(T p, int n, Args... args) {
		static_assert(ObjectdefsPrivate::PointerLevel<T>::Level == 2, "Type must be a 2-level pointer!");
		if (p == nullptr)	return;
		for (int i = 0; i < n; ++i) {
			che_pointer_del(p[i]);
		}
		delete p;
		che_dyadic_array_del(args...);
	}
	// 在运行到特殊代码时才会申请内存，用此模版来避免不必要的内存申请
	template<typename T>
	void _memory_dynamic_crt(T& pointer) {
		static_assert(ObjectdefsPrivate::PointerLevel<T>::Level == 1, "Type must be a 1-level pointer.");
		if (pointer == nullptr) {
			pointer = new ObjectdefsPrivate::StayOrigin<T>::Type;
		}
	}
	// args是类型T构造函数的参数
	template<typename T, typename... Args>
	bool _memory_dynamic_crt(T& pointer, Args... args) {
		static_assert(ObjectdefsPrivate::PointerLevel<T>::Level == 1, "Type must be a 1-level pointer.");
		if (pointer == nullptr) {
			pointer = new ObjectdefsPrivate::StayOrigin<T>::Type(args...);
			return true;
		}
		return false;
	}
} //namesapce che
#endif // HObjectdefs_conn_H__
