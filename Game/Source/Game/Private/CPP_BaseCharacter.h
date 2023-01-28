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
	
	UPROPERTY(BlueprintReadWrite)
	class UUserWidget* OverlayUI;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
