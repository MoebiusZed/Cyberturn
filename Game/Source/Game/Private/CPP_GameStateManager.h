// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CPP_GameStateManager.generated.h"

UCLASS()
class UCPP_GameStateManager : public UGameInstance
{
	GENERATED_BODY()

private:
	// RimRange is the output of SetRimRange.
	TArray<class ACPP_Tile*> RimRange;

	bool bWasLastCharacterAnEnemy = false;

	// This is used by GetNextCharactersTurn to control the turn order of 
	// the character in GetAllCharactersByTurnWeight
	int CurrentTurnIndex = 0;
	TArray<class ACPP_BaseCharacter*> AllCharactersByTurnWeight;

	// This variable lists all the tiles in the game. It's populated by RegisterGameworldTiles
	TArray<class ACPP_Tile*> AllGameworldTiles;

public:
	UFUNCTION(BlueprintCallable, Category = "Rim")
	TArray<class ACPP_Tile*> GetAllGameworldTiles();

	// Gets all the tiles in the game world
	UFUNCTION(BlueprintCallable, Category = "Rim")
	void RegisterGameworldTiles();
	


	// This function returns all the tiles in a range. The output will match the last SetRimRange function call.
	UFUNCTION(BlueprintCallable, Category = "Rim")
	TArray<class ACPP_Tile*> GetRimRange();

	// Find all the tiles in a range (MovementDistance) from a tile (StartTile)
	UFUNCTION(BlueprintCallable, Category = "Rim")
	void SetRimRange(const bool OriginIsAgent, class ACPP_Tile* StartTile, int32 MovementDistance);

	UFUNCTION(BlueprintCallable, Category = "Rim")
	void ClearRimRange();



	UFUNCTION(BlueprintCallable, Category = "TurnOrder")
	TArray<class ACPP_BaseCharacter*> GetAllCharactersByTurnWeight();

	// Find all actors in current level of type CPP_BaseCharacter.
	UFUNCTION(BlueprintCallable, Category = "TurnOrder")
	void RegisterAllCharactersByTurnWeight();

	UFUNCTION(BlueprintCallable, Category = "TurnOrder")
	class ACPP_BaseCharacter* GetNextCharactersTurn();

private:
	bool AreAllCharactersDead();
};
