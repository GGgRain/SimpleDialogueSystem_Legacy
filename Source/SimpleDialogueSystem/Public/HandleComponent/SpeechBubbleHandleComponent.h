// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpeechBubbleHandleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLEDIALOGUESYSTEM_API USpeechBubbleHandleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpeechBubbleHandleComponent();

	virtual void BeginPlay() override;


public:

	UPROPERTY(EditAnywhere , Category = "SpeechBubble")
		FName IDName; //����� �̸��Դϴ�. ���̾�α� ȣ�⿡�� �ĺ������ ���˴ϴ�.

	UPROPERTY(EditAnywhere , Category = "SpeechBubble")
		FText DisplayName; //Npc�� �̸��Դϴ�. ǥ�⿡ ���Ǵ� �̸��Դϴ�.


public:
	//Dialogue Properties

	UPROPERTY(EditAnywhere, Category = "SpeechBubble")
		TSubclassOf<UUserWidget> SpeechBubbleWidgetClass;
	//���̾�α׸� �̿��� ��ȭ���� ���Ǵ� ������ ���۷����Դϴ�.
	UPROPERTY()
		class UDialogueWidgetBase* SpeechBubbleWidgetPtr;
	//���̾�α� ��ȭ�� ����� ��ȭ �Ŵ����Դϴ�.
	UPROPERTY(EditAnywhere, Category = "SpeechBubble")
		class UDialogueManager* DialogueManager;

	//��ȭ�� ����� ������Ʈ�Դϴ�.
	UPROPERTY(EditAnywhere, Category = "SpeechBubble")
		class UAnimaleseSpeechAudioComponent* AnimaleseSpeechAudioComponent;

	UPROPERTY(EditAnywhere, Category = "SpeechBubble")
		FVector WidgetOffset;

	UPROPERTY(EditAnywhere, Category = "SpeechBubble")
		FName WidgetAttachTargetComponentName;


public:

	UFUNCTION(BlueprintCallable)
		void SetDialogueAsset(UDialogueManager* InDialogueManager);

	UFUNCTION(BlueprintCallable)
		void ClearDialogueAsset();

public:

	UFUNCTION(BlueprintCallable)
		void StartDialogue();


	UFUNCTION(BlueprintCallable)
		void StartTransientDialogue(TMap<FName, USpeechBubbleHandleComponent*> SwapMap, bool bShouldSwapSelfToThisComponent = true);
		

	UFUNCTION(BlueprintCallable)
		void EndDialogue();

	//Immediately CloseDialogue
	UFUNCTION(BlueprintCallable)
		void CloseDialogue();

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
