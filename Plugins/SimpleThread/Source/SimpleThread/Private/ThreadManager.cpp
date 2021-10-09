#include "ThreadManager.h"

TSharedPtr<FThreadManagement> FThreadManagement::ThreadManagement = nullptr;

TSharedRef<FThreadManagement> FThreadManagement::Get()
{
	if (ThreadManagement.IsValid()) {// 如果智能指针为空,就给他创建1个
		ThreadManagement = MakeShareable(new FThreadManagement());
	}
	return ThreadManagement.ToSharedRef();
}

void FThreadManagement::Destroy()
{
	if (ThreadManagement.IsValid()) {
		ThreadManagement = nullptr;
	}
}

void FThreadManagement::CleanAllThread()
{

}

void FThreadManagement::CleanThread(FWeakThreadHandle Handle)
{

}

bool FThreadManagement::ProceduralProgress(FWeakThreadHandle Handle)
{
	return false;
}

bool FThreadManagement::Do(FWeakThreadHandle Handle)
{
//	ThreadProxy->CreateSafeThread();// 使用线程代理去创建线程实例

	return false;
}

FWeakThreadHandle FThreadManagement::CreatetThread(const FThradLambda& ThreadLambda)
{
	// 创建线程代理并更新内部的lambda
	TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable());
	ThreadProxy->GetThreadLambda() = ThreadLambda;
	
	/* 使用目标线程代理创建线程实例并注册线程池,最后返回1根弱句柄*/
	return UpdateThreadPool(ThreadProxy);
}

FWeakThreadHandle FThreadManagement::UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy)
{
	Pool.Add(ThreadProxy);// 线程池里注册 本线程容器

	return ThreadProxy->GetThreadHandle();//返回线程里的 弱句柄
}

