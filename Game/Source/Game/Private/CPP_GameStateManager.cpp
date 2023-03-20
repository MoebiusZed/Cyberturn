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
	for (ACPP_Tile* tile : this->RimRange)
	{
		tile->TileBody->SetMaterial(0, tile->NotValidMovementMaterial);
		tile->TileState = ETileState::Deactivated;
		tile->RenderStage = ERenderStage::NotValidTile;
		tile->bTileIsSelectable = false;
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
	// Reset values
	this->CurrentTurnIndex = 0;
	this->AllCharactersByTurnWeight.Empty();

	// Get all characters, and typecast from AActor to ACPP_BaseCharacter
	auto ReturnCharacters = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_BaseCharacter::StaticClass(), ReturnCharacters);
	for (AActor* character : ReturnCharacters)
	{
		this->AllCharactersByTurnWeight.Add(Cast<ACPP_BaseCharacter>(character));
	}

	// Order all characters by turn weight in ascending order
	for (int x = 0; x < this->AllCharactersByTurnWeight.Num(); x += 1)
	{
		for (int y = 0; y < this->AllCharactersByTurnWeight.Num() - 1; y += 1)
		{
			if (this->AllCharactersByTurnWeight[y]->InitiativeTurnWeight > this->AllCharactersByTurnWeight[y + 1]->InitiativeTurnWeight) {
				auto tmp = this->AllCharactersByTurnWeight[y];
				this->AllCharactersByTurnWeight[y] = this->AllCharactersByTurnWeight[y + 1];
				this->AllCharactersByTurnWeight[y + 1] = tmp;
			}
		}
	}
}

// Returns the next character in a turn
ACPP_BaseCharacter* UCPP_GameStateManager::GetNextCharactersTurn()
{
	if (this->AllCharactersByTurnWeight.IsEmpty()) return nullptr;

	ACPP_BaseCharacter* character = this->AllCharactersByTurnWeight[this->CurrentTurnIndex];
	character->CurrentAP += character->StartAP;
	character->CurrentAP = FMath::Clamp(character->CurrentAP, 0, character->MaxAP);

	// CurrentTurnIndex is incremented after getting the character so the first character in the turn order isn't skipped.
	this->CurrentTurnIndex = (this->CurrentTurnIndex + 1) % this->AllCharactersByTurnWeight.Num();

	return character;
}

int UCPP_GameStateManager::GetCharacterTurnIndex()
{
	// GetCurrentTurnIndex always reflects 1 turn in the future. So to get the array index
	// of the current character's turn, you subtract 1. This is because CurrentTurnIndex is
	// incremented after the current character for a turn is returned by GetNextCharactersTurn
	return this->CurrentTurnIndex - 1;
}

void UCPP_GameStateManager::UpdateCurrentTurnIndex(int num)
{
	this->CurrentTurnIndex = this->CurrentTurnIndex + num;

	if (this->CurrentTurnIndex >= this->AllCharactersByTurnWeight.Num()) this->CurrentTurnIndex = 0;
	if (this->CurrentTurnIndex < 0) this->CurrentTurnIndex = 0;
}

// KillCharacter can't kill the currently active character
// This is because it doesn't clean up stuff like ui. This can lead to a bug where 
// The UI of the agent is still on the screen
void UCPP_GameStateManager::KillCharacter(ACPP_BaseCharacter* character)
{
	if (this->AllCharactersByTurnWeight.Num() == 0) return;

	int currentlyActiveCharacter = this->GetCharacterTurnIndex();

	for (int index = 0; index < this->AllCharactersByTurnWeight.Num(); index += 1)
	{
		// Find the "character" location of the character to kill in "this->AllCharactersByTurnWeight"
		// the location if when found is index.
		if (this->AllCharactersByTurnWeight[index] == character)
		{
			// We do not want to kill the character if it's the currently active one.
			if (currentlyActiveCharacter == index) return;

			// Kills the "character"
			ACPP_BaseCharacter* character_to_delete = this->AllCharactersByTurnWeight[index];
			this->AllCharactersByTurnWeight.RemoveAt(index);
			character_to_delete->Destroy();

			// Keeps the next turn in sync. If the character killed comes before the currently
			// active character, then it causes the CurrentTurnIndex to skip the next characters turn.
			if (index < this->CurrentTurnIndex) this->UpdateCurrentTurnIndex(-1);

			break;
		}
	}
}


// Call this function after spawning a character to include it in the turn order.
// You will also need to register meta data again.
void UCPP_GameStateManager::AddCharacterToTurn(ACPP_BaseCharacter* character)
{
	if (this->AllCharactersByTurnWeight.Num() == 0) return;

	for (int x = 0; x < this->AllCharactersByTurnWeight.Num(); x += 1)
	{
		if (character->InitiativeTurnWeight < this->AllCharactersByTurnWeight[x]->InitiativeTurnWeight)
		{
			this->AllCharactersByTurnWeight.Insert(character, x);

			if (x < this->CurrentTurnIndex) this->UpdateCurrentTurnIndex(1);
		}
	}
}