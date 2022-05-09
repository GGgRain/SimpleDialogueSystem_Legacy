// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueWidgetBase.h"
#include "SpeechBubbleWidget_Handled.generated.h"

/**
 * 
 */

class USpeechBubbleHandleComponent;

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpeechBubbleWidgetAnimationPlayHandle, FName, AnimationName);

UCLASS()
class SIMPLEDIALOGUESYSTEM_API USpeechBubbleWidget_Handled : public UDialogueWidgetBase
{
	GENERATED_BODY()
public:

	USpeechBubbleWidget_Handled();

	UPROPERTY(EditAnywhere)
		TMap<FName, USpeechBubbleHandleComponent*> ConversationComponents;

public:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void AdjustRenderingLocation();

public:

	UPROPERTY(EditAnywhere)
		FVector LastPlayerCameraBoomLocation;

	UPROPERTY(EditAnywhere)
		float DisplayMargin = 0.2;

public:

	virtual void UpdateSpeakerBlockTextBasedOnCurrentDialogue() override;

	void TryAddConversationComponents(TMap<FName, USpeechBubbleHandleComponent*> InConversationComponents);

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
		FSpeechBubbleWidgetAnimationPlayHandle AnimationHandle;

};
