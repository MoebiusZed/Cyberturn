// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GameStateManager.h"
#include "CPP_Tile.h"
#include "CPP_BaseCharacter.h"
#include "Kismet/GameplayStatics.h"

TArray<ACPP_Tile*> UCPP_GameStateManager::GetAllGameworldTiles()
{
	return this->AllGameworldTiles;
}

// This function is used to populate AllGameworldTiles
// Call this function every time new times are introduced to the game world
void UCPP_GameStateManager::RegisterGameworldTiles()
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Number of elements before %i"), AllGameworldTiles.Num()));
	AllGameworldTiles.Empty();
	auto OutTiles = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Tile::StaticClass(), OutTiles);
	for (AActor* tile : OutTiles)
	{
		AllGameworldTiles.Add(Cast<ACPP_Tile>(tile));
	}
	//if (GEngine) GEngine->AddOnScreenDebugMessage(2, 15.0f, FColor::Yellow, FString::Printf(TEXT("Number of elements after %i"), AllGameworldTiles.Num()));
}

TArray<ACPP_Tile*> UCPP_GameStateManager::GetRimRange()
{
	return this->RimRange;
}

// Sets the material and tile state data of all the tiles within MovementDistance of the StartTile.
void UCPP_GameStateManager::SetRimRange(const bool OriginIsAgent, ACPP_Tile* StartTile, int32 MovementDistance)
{
	// Resets Tiles from the last SetRimRange call.
	for (ACPP_Tile* tile : this->RimRange)
	{
		tile->TileState = ETileState::Deactivated;
		tile->RenderStage = ERenderStage::ToBeDemotedTile;
		tile->bTileIsSelectable = false;
	}

	// Initialize StartTile
	StartTile->TileState = ETileState::ToBeActivated;
	if (OriginIsAgent)
	{
		// Unless it's a player, these values should be left default
		StartTile->RenderStage = ERenderStage::ValidTile;
		StartTile->bTileIsSelectable = true;
	}
	TArray<ACPP_Tile*> Tiles = { StartTile };

	int ResumeIndex = 0;
	int EndIndex = 0;
	for (int x = 0; x < MovementDistance; x += 1)
	{
		ResumeIndex = EndIndex;
		EndIndex = Tiles.Num();

		// This means the entire grid has been searched and there are no new tiles that can be added
		if (ResumeIndex == EndIndex) break;

		for (int index = ResumeIndex; index < EndIndex; index += 1)
		{
			ACPP_Tile* Tile = Tiles[index];

			if (Tile->TileState == ETileState::ToBeActivated)
			{
				Tile->TileState = ETileState::Activated;

				if (Tile->NorthNeighbour && Tile->NorthNeighbour->TileState == ETileState::Deactivated)
				{
					Tile->NorthNeighbour->TileState = ETileState::ToBeActivated;
					Tiles.Add(Tile->NorthNeighbour);
					if (OriginIsAgent)
					{
						Tile->NorthNeighbour->RenderStage = ERenderStage::ValidTile;
						Tile->NorthNeighbour->bTileIsSelectable = true;
					}
				}
				if (Tile->SouthNeighbour && Tile->SouthNeighbour->TileState == ETileState::Deactivated)
				{
					Tile->SouthNeighbour->TileState = ETileState::ToBeActivated;
					Tiles.Add(Tile->SouthNeighbour);
					if (OriginIsAgent)
					{
						Tile->SouthNeighbour->RenderStage = ERenderStage::ValidTile;
						Tile->SouthNeighbour->bTileIsSelectable = true;
					}
				}
				if (Tile->EastNeighbour && Tile->EastNeighbour->TileState == ETileState::Deactivated)
				{
					Tile->EastNeighbour->TileState = ETileState::ToBeActivated;
					Tiles.Add(Tile->EastNeighbour);
					if (OriginIsAgent)
					{
						Tile->EastNeighbour->RenderStage = ERenderStage::ValidTile;
						Tile->EastNeighbour->bTileIsSelectable = true;
					}
				}
				if (Tile->WestNeighbour && Tile->WestNeighbour->TileState == ETileState::Deactivated)
				{
					Tile->WestNeighbour->TileState = ETileState::ToBeActivated;
					Tiles.Add(Tile->WestNeighbour);
					if (OriginIsAgent)
					{
						Tile->WestNeighbour->RenderStage = ERenderStage::ValidTile;
						Tile->WestNeighbour->bTileIsSelectable = true;
					}
				}
			}
		}
	}

	// Render Tile Materials
	for (ACPP_Tile* tile : this->AllGameworldTiles)
	{
		if (tile->RenderStage == ERenderStage::ToBeDemotedTile)
		{
			tile->TileBody->SetMaterial(0, tile->NotValidMovementMaterial);
			tile->RenderStage = ERenderStage::NotValidTile;
		}

		// !LastUpdatedTiles.Contains(tile) checks to see if the tile has already had it's material updated, if it hasn't then update material.
		// this->LastEntityWasAI is used to ignore the above ^ if the last entity that used SetRimRange was an AI
		// because AI doesn't update the materials of the tiles in its MovementDistance range like agents do.
		if (OriginIsAgent && tile->RenderStage == ERenderStage::ValidTile && (this->bWasLastCharacterAnEnemy || !RimRange.Contains(tile)))
		{
			tile->TileBody->SetMaterial(0, tile->ValidMovementMaterial);
		}
	}

	this->RimRange = Tiles;
	this->bWasLastCharacterAnEnemy = !OriginIsAgent;
}

void UCPP_GameStateManager::ClearRimRange()
{
	// Resets Tiles from the last SetRimRange call.
	for (ACPP_Tile* tile : this->RimRange)
	{
		tile->TileState = ETileState::Deactivated;
		tile->RenderStage = ERenderStage::ToBeDemotedTile;
		tile->bTileIsSelectable = false;
	}

	// Render Tile Materials
	for (ACPP_Tile* tile : this->AllGameworldTiles)
	{
		if (tile->RenderStage == ERenderStage::ToBeDemotedTile)
		{
			tile->TileBody->SetMaterial(0, tile->NotValidMovementMaterial);
			tile->RenderStage = ERenderStage::NotValidTile;
		}

		// !LastUpdatedTiles.Contains(tile) checks to see if the tile has already had it's material updated, if it hasn't then update material.
		// this->LastEntityWasAI is used to ignore the above ^ if the last entity that used SetRimRange was an AI
		// because AI doesn't update the materials of the tiles in its MovementDistance range like agents do.
		if (tile->RenderStage == ERenderStage::ValidTile && (this->bWasLastCharacterAnEnemy || !RimRange.Contains(tile)))
		{
			tile->TileBody->SetMaterial(0, tile->ValidMovementMaterial);
		}
	}

	this->RimRange.Empty();
}

TArray<ACPP_BaseCharacter*> UCPP_GameStateManager::GetAllCharactersByTurnWeight()
{
	return this->AllCharactersByTurnWeight;
}

// This function is used to order all the characters in the game world by their assigned turn weight value
// Call this function any time a new character joins a battle (and at start)
void UCPP_GameStateManager::RegisterAllCharactersByTurnWeight()
{
	// Reset 
	CurrentTurnIndex = 0;
	AllCharactersByTurnWeight.Empty();
	auto ReturnCharacters = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_BaseCharacter::StaticClass(), ReturnCharacters);
	for (AActor* character : ReturnCharacters)
	{
		AllCharactersByTurnWeight.Add(Cast<ACPP_BaseCharacter>(character));
	}
	for (int x = 0; x < AllCharactersByTurnWeight.Num(); x += 1)
	{
		for (int y = 0; y < AllCharactersByTurnWeight.Num() - 1; y += 1)
		{
			if (AllCharactersByTurnWeight[y]->InitiativeTurnWeight > AllCharactersByTurnWeight[y + 1]->InitiativeTurnWeight) {
				auto tmp = AllCharactersByTurnWeight[y];
				AllCharactersByTurnWeight[y] = AllCharactersByTurnWeight[y + 1];
				AllCharactersByTurnWeight[y + 1] = tmp;
			}
		}
	}
}

// Returns the next character in a turn
// Characters with 0 or less hp are skipped.
ACPP_BaseCharacter* UCPP_GameStateManager::GetNextCharactersTurn()
{
	if (this->AllCharactersByTurnWeight.IsEmpty() || this->AreAllCharactersDead()) return nullptr;

	ACPP_BaseCharacter* character;
	for (;;)
	{
		character = this->AllCharactersByTurnWeight[CurrentTurnIndex];

		// CurrentTurnIndex is incremented after getting the character so the first character in the turn order isn't skipped.
		CurrentTurnIndex = (CurrentTurnIndex + 1) % this->AllCharactersByTurnWeight.Num();

		if (character->Health > 0.0f) break;
	}
	return character;
}

bool UCPP_GameStateManager::AreAllCharactersDead()
{
	int result = 0;
	for (auto character : this->AllCharactersByTurnWeight) {
		if (character->Health < 0.0f) result += 1;
	}

	if (result >= this->AllCharactersByTurnWeight.Num()) {
		return true;
	}
	else
	{
		return false;
	}
}