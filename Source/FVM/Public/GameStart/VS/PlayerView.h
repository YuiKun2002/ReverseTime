#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerView.generated.h"

class UBoxComponent;
class UAudioComponent;
class UCameraComponent;	
class USpringArmComponent;
class UWidgetInteractionComponent;

/*
	ÕÊº“ ”Õº
*/
UCLASS()
class FVM_API APlayerView : public APawn
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerView")
	class UCameraComponent* M_UCameraComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerViewMove")
	float M_PlayerViewMoveSpeed = 1.f;

public:
	// Sets default values for this pawn's properties
	APlayerView();

protected:
	
	UFUNCTION()
	void MoveUp(float _Value);

	UFUNCTION()
	void MoveRight(float _Value);

	UFUNCTION()
	void UpView(float _Value);

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere, Category = "PlayerView")
	class USpringArmComponent* M_USpringArmComponent = nullptr;
};