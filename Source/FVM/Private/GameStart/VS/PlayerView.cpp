#include "GameStart/VS/PlayerView.h"
#include <Kismet/GameplayStatics.h>
#include <Camera/CameraComponent.h>
#include <Components/BoxComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/SpringArmComponent.h>
#include <Components/WidgetInteractionComponent.h>

APlayerView::APlayerView()
{
	PrimaryActorTick.bCanEverTick = true;

	this->M_UCameraComponent = this->CreateDefaultSubobject<UCameraComponent>(TEXT("M_UCameraComponent"));
	this->M_USpringArmComponent = this->CreateDefaultSubobject<USpringArmComponent>(TEXT("M_USpringArmComponent"));

	this->M_USpringArmComponent->SetupAttachment(this->GetRootComponent());
	this->M_UCameraComponent->SetupAttachment(this->M_USpringArmComponent);
}

void APlayerView::BeginPlay()
{
	Super::BeginPlay();


	this->SetActorLocation(FVector(0.f, 0.f, 118.f));
}

void APlayerView::MoveUp(float _Value)
{
	if (_Value != 0.f)
	{
		this->AddActorLocalOffset(FVector(0.f, 0.f, _Value * this->M_PlayerViewMoveSpeed));
	}
}

void APlayerView::MoveRight(float _Value)
{
	if (_Value != 0.f)
	{
		this->AddActorLocalOffset(FVector(0.f, _Value * this->M_PlayerViewMoveSpeed, 0.f));
	}
}

void APlayerView::UpView(float _Value)
{
	if (_Value != 0.f)
	{
		this->M_UCameraComponent->SetFieldOfView(this->M_UCameraComponent->FieldOfView + _Value);
	}
}

void APlayerView::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("PlayerViewMoveUp", this, &APlayerView::MoveUp);
	PlayerInputComponent->BindAxis("PlayerViewMoveRight", this, &APlayerView::MoveRight);

	PlayerInputComponent->BindAxis("PlayerViewUp", this, &APlayerView::UpView);
}