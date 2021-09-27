// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Math/Transform.h"
#include "ObjectPoolSubsystem.generated.h"

class UClass;
class UGameInstance;
/**
 * 
 */
UCLASS()
class SIMPLEOBJECTPOOL_API UObjectPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable,Category="Pool",meta =(DisplayName = "SpawnActorByObjectPool"))
		AActor* SpawnActor(TSubclassOf<AActor> PoolObjectClass, const FTransform& Transform, bool bNoCollisionFail = false);

	UFUNCTION(BlueprintCallable, Category = "Pool", meta = (DisplayName = "ReleaseActorToPool"))
		void Release(AActor* ReleaseActor);

	UFUNCTION(BlueprintCallable, Category = "Pool", meta = (DisplayName = "ReleaseAllActorToPoolWithClass"))
		void ReleaseAllByClass(TSubclassOf<AActor> PoolObjectClass);
private:
		TMap<UClass*, TArray<UObject*> > ActiveObjectMap;

		TMap<UClass*, TArray<UObject*>> InactiveObjectMap;
};
