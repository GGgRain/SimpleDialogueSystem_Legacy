// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueManager.h"
#include "DialogueHandleComponent.generated.h"

//���̾�αװ� ����Ǵµ��� �ʿ��� ������ ���̾�α��� �����Ȳ�� �Ѱܹް� ó���ϴ� ���� ���Ǵ� ������Ʈ�Դϴ�.
//

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLEDIALOGUESYSTEM_API UDialogueHandleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDialogueHandleComponent();

	virtual void BeginPlay() override;

public:	
	
	UPROPERTY(EditAnywhere, Category = Dialogue)
		FName IDName; //����� �̸��Դϴ�. ���̾�α� ȣ�⿡�� �ĺ������ ���˴ϴ�.

	UPROPERTY(EditAnywhere, Category = Dialogue)
		FText DisplayName; //Npc�� �̸��Դϴ�. ǥ�⿡ ���Ǵ� �̸��Դϴ�.


public:
	//Dialogue Properties

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> DialogueWidgetClass;

	//���̾�α׸� �̿��� ��ȭ���� ���Ǵ� ������ ���۷����Դϴ�.
	UPROPERTY()
		class UDialogueWidgetBase* DialogueWidgetPtr;
	//���̾�α� ��ȭ�� ����� ��ȭ �Ŵ����Դϴ�.
	UPROPERTY(EditAnywhere, Category = Dialogue)
		class UDialogueManager* DialogueManager;

	//��ȭ�� ����� ������Ʈ�Դϴ�.
	UPROPERTY(VisibleAnywhere)
		class UAnimaleseSpeechAudioComponent* AnimaleseSpeechAudioComponent;

public:

	UFUNCTION(BlueprintCallable)
		void SetDialogueAsset(UDialogueManager* InDialogueManager);

	UFUNCTION(BlueprintCallable)
		void ClearDialogueAsset();

public:

	//�����ϰ� �ִ� ���̾�α׸� �����մϴ�.
	UFUNCTION(BlueprintCallable)
		void StartDialogue();

	UFUNCTION(BlueprintCallable)
		void StartTransientDialogue(TMap<FName, UDialogueHandleComponent*> SwapMap, bool bShouldSwapSelfToThisComponent = true);

public:

	UFUNCTION(BlueprintCallable)
		bool IsDialoguePlaying();

public:

	//�������� �ؽ�Ʈ�� ������Ʈ �Ǿ����� ȣ��˴ϴ�.
	UFUNCTION()
		virtual void OnStringPlayed(UDialogueWidgetBase* Widget, FString String, float Duration, int32 CurrentIndex);

	UFUNCTION()
		virtual void OnStringListened(UDialogueWidgetBase* Widget);

	//���� Ŀ�ǵ带 �����Ҷ� ȣ��˴ϴ�.
	UFUNCTION()
		virtual void OnCustomCommandCalled(FString Command);

};
