// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueWidgetBase.h"
#include "DialogueWidget_Handled.generated.h"

/**
 * 
 */

class UDialogueHandleComponent;

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogueWidgetAnimationPlayHandle, FName, AnimationName);

UCLASS()
class SIMPLEDIALOGUESYSTEM_API UDialogueWidget_Handled : public UDialogueWidgetBase
{
	GENERATED_BODY()

public:

	UDialogueWidget_Handled();

	UPROPERTY(EditAnywhere)
		TMap<FName, UDialogueHandleComponent*> ConversationComponents;

public:
	UPROPERTY(EditAnywhere)
		FVector LastPlayerCameraBoomLocation;

public:

	
	virtual void UpdateSpeakerBlockTextBasedOnCurrentDialogue() override;

	void TryAddConversationComponents(TMap<FName, UDialogueHandleComponent*> InConversationComponents);

	void SetPlayerCameraBetweenTalkingComponents();

	void SetPlayerCameraLocationToLastLoc();

	void CollectLastPlayerCameraLocation();

public:

	virtual void UpdateText() override;

	void AdjustParentNameBorderWidget();

public:

	virtual void CallCommand(FString Command) override;

	FReply TryCallCommand_Dialogue(FString Command);
	FReply TryCallCommand_Handler(FString Command);
	FReply TryCallCommand_Game(FString Command);

public:

	virtual void OpenAction() override;

	virtual void CloseAction() override;

	virtual void OnDialogueWidgetBegin() override;// ������ ȭ�鿡 ó�� �׷��� �� ȣ��˴ϴ�.

	virtual void OnNextDialoguePlayed()  override; // ���� ���̾�αװ� ȭ�鿡 ó�� �׷��� �� ȣ��˴ϴ�.

	virtual void OnDialogueWidgetEnd()  override; // ������ ȭ�鿡�� ����� �� ȣ��˴ϴ�.

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
		FDialogueWidgetAnimationPlayHandle AnimationHandle;

};
