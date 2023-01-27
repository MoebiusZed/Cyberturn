// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GameStateManager.h"
#include "CPP_Tile.h"
#include "CPP_BaseCharacter.h"
#include "Kismet/GameplayStatics.h"

TArray<ACPP_Tile*> UCPP_GameStateManager::GetRimRange()
{
	return this->RimRange;
}

// This function is used to order all the characters in the game world by their assigned turn weight value
// Call this function any time a new character joins a battle (and at start)
void UCPP_GameStateManager::RegisterAllCharactersByTurnWeight()
{
	GetAllCharactersByTurnWeight.Empty();
	auto ReturnCharacters = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_BaseCharacter::StaticClass(), ReturnCharacters);
	for (AActor* character : ReturnCharacters)
	{
		GetAllCharactersByTurnWeight.Add(Cast<ACPP_BaseCharacter>(character));
	}
	for (int x = 0; x < GetAllCharactersByTurnWeight.Num(); x += 1)
	{
		for (int y = 0; y < GetAllCharactersByTurnWeight.Num() - 1; y += 1)
		{
			if (GetAllCharactersByTurnWeight[y]->InitiativeTurnWeight > GetAllCharactersByTurnWeight[y + 1]->InitiativeTurnWeight) {
				auto tmp = GetAllCharactersByTurnWeight[y];
				GetAllCharactersByTurnWeight[y] = GetAllCharactersByTurnWeight[y + 1];
				GetAllCharactersByTurnWeight[y + 1] = tmp;
			}
		}
	}
}

// This function is used to populate AllGameworldTiles
// Call this function every time new times are introduced to the game world
void UCPP_GameStateManager::RegisterGameworldTiles()
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Number of elements before %i"), AllGameworldTiles.Num()));
	GetAllGameworldTiles.Empty();
	auto OutTiles = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Tile::StaticClass(), OutTiles);
	for (AActor* tile : OutTiles)
	{
		GetAllGameworldTiles.Add(Cast<ACPP_Tile>(tile));
	}
	//if (GEngine) GEngine->AddOnScreenDebugMessage(2, 15.0f, FColor::Yellow, FString::Printf(TEXT("Number of elements after %i"), AllGameworldTiles.Num()));
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
	for (ACPP_Tile* tile : this->GetAllGameworldTiles)
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