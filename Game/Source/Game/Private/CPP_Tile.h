// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_Tile.generated.h"

UENUM()
enum class ETileState : uint8
{
	Deactivated		UMETA(DisplayName = "Deactivated"),
	ToBeActivated	UMETA(DisplayName = "ToBeActivated"),
	Activated		UMETA(DisplayName = "Activated"),
};

UENUM()
enum class ERenderStage : uint8
{
	NotValidTile	UMETA(DisplayName = "NotValidTile"),
	ToBeDemotedTile	UMETA(DisplayName = "ToBeDemotedTile"),
	ValidTile		UMETA(DisplayName = "ValidTile"),
};

UCLASS()
class GAME_API ACPP_Tile : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ACPP_Tile();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ETileState TileState = ETileState::Deactivated;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ERenderStage RenderStage = ERenderStage::NotValidTile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bTileIsSelectable = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ACPP_Tile* NorthNeighbour = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ACPP_Tile* SouthNeighbour = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ACPP_Tile* EastNeighbour = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ACPP_Tile* WestNeighbour = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterial* ValidMovementMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* NotValidMovementMaterial;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* TileBody;
};
