// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPP_BaseCharacter.generated.h"

UCLASS()
class ACPP_BaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACPP_BaseCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitiativeTurnWeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxAP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int StartAP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentAP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name = "Unset";
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadWrite)
	class UUserWidget* OverlayUI;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, BlueprintReadWrite)
	TArray<TEnumAsByte<EObjectTypeQuery>> TileType;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, BlueprintReadWrite)
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
