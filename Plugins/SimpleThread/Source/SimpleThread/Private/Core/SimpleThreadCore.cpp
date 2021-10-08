/*
本.cpp文件 对应
Core\SimpleThreadType.h
Interface\ProxyInterface.h
里某些函数的具体实现

*/
#include "Core\SimpleThreadType.h"
#include "Interface\ProxyInterface.h"

// 随机ID生成器
uint64 SpawnUniqueID()
{
	return FMath::Abs( FMath::RandRange(100, 1256343) + FDateTime::Now().GetTicks() );
}

FSimpleThreadHandle::FSimpleThreadHandle()
	:GUIDTypeA(SpawnUniqueID())
	,GUIDTypeB(SpawnUniqueID())
	,GUIDTypeC(SpawnUniqueID())
	,GUIDTypeD(SpawnUniqueID())
{

}

IThreadProxy::IThreadProxy()
	:SimpleThreadHandle( MakeShareable(new FSimpleThreadHandle) )
{

}

IThreadProxy::~IThreadProxy()
{

}