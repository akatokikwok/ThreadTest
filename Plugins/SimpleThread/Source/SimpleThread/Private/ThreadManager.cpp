#include "ThreadManager.h"

TSharedPtr<FThreadManagement> FThreadManagement::ThreadManagement = nullptr;

TSharedRef<FThreadManagement> FThreadManagement::Get()
{
	if (ThreadManagement.IsValid()) {// �������ָ��Ϊ��,�͸�������1��
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

FWeakThreadHandle FThreadManagement::CreatetThread(const FThradLambda& ThreadLambda)
{
	// �����̴߳��������ڲ���lambda
	TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable());
	ThreadProxy->GetThreadLambda() = ThreadLambda;
	
	/* ʹ��Ŀ���̴߳������߳�ʵ����ע���̳߳�,��󷵻�1�������*/
	return UpdateThreadPool(ThreadProxy);
}

FWeakThreadHandle FThreadManagement::UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy)
{
	ThreadProxy->CreateSafeThread();// ʹ���̴߳���ȥ�����߳�ʵ��
	Pool.Add(ThreadProxy);// �̳߳���ע�� ���߳�����

	return ThreadProxy->GetThreadHandle();//�����̴߳������ �����
}

