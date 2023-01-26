// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Tile.h"

// Sets default values
ACPP_Tile::ACPP_Tile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->TileBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	TileBody->SetupAttachment(this->RootComponent);
}

void ACPP_Tile::BeginPlay()
{
	Super::BeginPlay();
}

void ACPP_Tile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}