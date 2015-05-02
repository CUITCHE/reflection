#include "RCReflection.h"
#include <iostream>
using namespace std;
using namespace che;

int main(int argc, char* argv[])
{
	RCReflection *abc = new RCReflection;
	int a = dync_call<int>(abc, "add", 1, 2);
	cout << a << endl;
	return 0;
}
/*
struct __block_impl
{
	void *isa;
	int Flags;
	int Reserved;
	void *FuncPtr;
};

struct __main_block_impl_0
{
	struct __block_impl impl;
	struct __main_block_desc_0 *Desc;
	__main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int flags = 0) {
		impl.isa = fp;
		impl.Flags = flags;
		impl.FuncPtr = fp;
		Desc = desc;
	}
	void operator()(){
		typedef void(*FUNC)(void);
		FUNC t = (FUNC)impl.FuncPtr;
		t();
	}
};

static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
	printf_s("hello\n");
}

static struct  __main_block_desc_0
{
	size_t reserved;
	size_t Block_size;
}__main_block_desc_0_DATA = { 0,sizeof(struct __main_block_impl_0) };

int main(int argc, char* argv[])
{
	(__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA))();
	return 0;
}*/