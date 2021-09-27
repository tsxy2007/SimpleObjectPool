// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPoolSubsystem.h"
#include "ObjectPoolInterface.h"
#include "GameFramework/Actor.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

AActor* UObjectPoolSubsystem::SpawnActor(TSubclassOf<AActor> PoolObjectClass, const FTransform& Transform, bool bNoCollisionFail /*= false*/)
{
	
	check(PoolObjectClass->ImplementsInterface(UObjectPoolInterface::StaticClass()));
	UClass* Class = *PoolObjectClass;
	TArray<UObject*>& InActiveObjArr = InactiveObjectMap.FindOrAdd(Class);
	TArray<UObject*>& ActiveObjArr = ActiveObjectMap.FindOrAdd(Class);
	if (InActiveObjArr.Num())
	{
		AActor* actor = Cast<AActor>(InActiveObjArr.Pop());
		ActiveObjArr.Add(actor);
		actor->SetActorTransform(Transform);
		IObjectPoolInterface::Execute_OnSetUp(actor);
		return actor;
	}
	UWorld* World = GetGameInstance()->GetWorld();
	if (World)
	{
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = bNoCollisionFail ? ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding : ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* Actor = World->SpawnActor(Class, &Transform, ActorSpawnParameters);
		IObjectPoolInterface::Execute_OnSetUp(Actor);
		ActiveObjArr.Add(Actor);
		return Actor;
	}
	return nullptr;
}

void UObjectPoolSubsystem::Release(AActor* ReleaseActor)
{
	if (ReleaseActor)
	{
		UClass* Class = ReleaseActor->GetClass();
		bool bHasActive = ActiveObjectMap.Contains(Class);
		bool bHasInActive = InactiveObjectMap.Contains(Class);
		if (bHasActive&& bHasInActive)
		{
			TArray<UObject*>* ActiveObjArr = ActiveObjectMap.Find(Class);
			TArray<UObject*>* InactiveObjArr = InactiveObjectMap.Find(Class);
			const int32 ActiveActorIdx = ActiveObjArr->Find(ReleaseActor);
			if (ActiveActorIdx!=INDEX_NONE)
			{
				InactiveObjArr->Push(ReleaseActor);
				ActiveObjArr->RemoveAt(ActiveActorIdx);
				IObjectPoolInterface::Execute_OnReset(ReleaseActor);
			}
		}
	}
}

void UObjectPoolSubsystem::ReleaseAllByClass(TSubclassOf<AActor> PoolObjectClass)
{
	UClass* Class = *PoolObjectClass;
	bool bHasActive = ActiveObjectMap.Contains(Class);
	bool bHasInActive = InactiveObjectMap.Contains(Class);
	if (bHasActive && bHasInActive)
	{
		TArray<UObject*>* ActiveObjArr = ActiveObjectMap.Find(Class);
		TArray<UObject*>* InactiveObjArr = InactiveObjectMap.Find(Class);
		TArray<UObject*> TmpArr;
		TmpArr.Append(*ActiveObjArr);
		ActiveObjArr->Empty();
		for (auto& ActiveActor : TmpArr)
		{
			IObjectPoolInterface::Execute_OnReset(ActiveActor);
		}
		InactiveObjArr->Append(TmpArr);
	}
}
