// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/Button.h"

#include "TimerManager.h"

#include "DialogueManager.h"

#include "DialogueWidgetBase.generated.h"

/**
 * 
 */
class UDialogueButtonWidget;


//���̾�α� ������ �⺻ �����Դϴ�. Ŀ�ǵ� �ݰ� ���� ������ ���� ����, UUserWidget�� ���¿� ���� ���۸��� �����մϴ�.
//�߻� Ŭ�����μ�, �ݵ�� �������̵� �ϼż� ����ϼž��մϴ�.


UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogueWidgetOpen);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogueWidgetNext);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogueWidgetClose);

USTRUCT()
struct FCustomTagStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		TArray<FString> CustomTags;

	FCustomTagStruct() {}

	FCustomTagStruct(TArray<FString> Arr) {
		CustomTags.Append(Arr);
	}

	FCustomTagStruct(FString Str) {
		CustomTags.Add(Str);
	}

};


UCLASS(Abstract)
class SIMPLEDIALOGUESYSTEM_API UDialogueWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:

//Main Property

	UPROPERTY(EditAnywhere)
		class URichTextBlock* ContextBlock;

	UPROPERTY(EditAnywhere)
		class URichTextBlock* OwnerNameBlock;

	UPROPERTY(EditAnywhere)
		class UScrollBox* SeletorScrollBox;

	UPROPERTY(EditAnywhere)
		class UButton* SkipButton;

public:

	void InitializeDialogueWidget(UDialogueManager* InBaseDialogueManager);

public:

	UPROPERTY(EditAnywhere)
	TMap<int32, FCustomTagStruct> CustomTagMap; //�±� �����̳��Դϴ�. �ش� �±װ� ���۵Ǵ� ��ġ�� �±��� ������ ���� �����մϴ�.

	FText ContextBlockText; 

	FTimerManager* TimerManagerRef;

	FTimerHandle TextUpdateHandle;

public:

	UPROPERTY(EditAnywhere)
		class UDialogueManager* BaseDialogueManager; //�÷��� �� ���̾�α� �Ŵ���

	UPROPERTY(EditAnywhere)
		class UDialogueUnitBase* CurrentPlayingDialogueUnit; //�÷��� ���̶�� ��ŷ�� ���̾�α�

public:

	FTimerHandle AutoSkipHandle;

	void SkipCurrentUnit();


public:

//Event

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	//��������� �ı��ؾ��Ҷ� ����մϴ�. �ַ� RemoveFromParent�� �̿��� ���۷����� ������ GC�� ���� ��Ʈ������ ������ ������ ������ ����մϴ�.
	UFUNCTION(BlueprintCallable)
		void ExplictDestruct();

//Function

	UFUNCTION(BlueprintCallable)
		void InitializeBasedOnCurrentDialogue(); // ���� �������� ���̾�α� ������ ����Ͽ� �ʹ� �ؽ�Ʈ�� �̸�, �ؽ�Ʈ ǥ������� �缳���մϴ�.

	UFUNCTION(BlueprintCallable)
		void SetFirstDialogueUnitBasedOnCurrentDialogue();

	UFUNCTION(BlueprintCallable)
		void UpdateContextBlockTextBasedOnCurrentDialogue();

	UFUNCTION(BlueprintCallable)
		virtual void UpdateSpeakerBlockTextBasedOnCurrentDialogue(); // ����ȭ ������ �������̵尡 �����ϰ� ����.

	UFUNCTION(BlueprintCallable)
		void CollectCustomTagsFromCurrentContextBlockText();

	int NumberOfShowingChar = 0;

	bool bShouldAppendEndDecorator = false;

	void SkipDecorator();
	
	UFUNCTION(BlueprintCallable)
		void SkipContext();

	void CheckAndPlaySkippedCommand();

	float TextUpdateFrequency = 0.05;

	UFUNCTION(BlueprintCallable)
	void StartUpdateText();

	UFUNCTION(BlueprintCallable)
	virtual void UpdateText(); // �⺻���� �־�����, ���Ѵٸ� ����Ͽ� �߰����� ������ ������ �� �ֽ��ϴ�. ���Ѵ�� ��

	UFUNCTION(BlueprintCallable)
	virtual void StopUpdateText();

	void TryStartNextDialogue(); //���� ���̾�αװ� �ִٸ� ���� ���̾�α׸� �����Ű��, �ƴ϶�� ������ �����մϴ�.

	virtual void CallCommand(FString Command) {};


	UFUNCTION(BlueprintCallable)
	void AddToViewport_Implemented();

	virtual void OpenAction(); //�ִϸ��̼��� ����ϰ� ���� ������Ʈ�� �����ϰ� �Ѵٴ���, �������� �Ը���� ���� ������ ������ �� �ִ� �ڵ��Դϴ�.

	virtual void CloseAction();

//selector Function

	UPROPERTY(EditAnywhere, Category = Widget)
		TSubclassOf<UDialogueButtonWidget> DialogueButtonWidgetClass;

	UFUNCTION(BlueprintCallable)
		void AddSelectOptionButtonOnWidget();

	UFUNCTION(BlueprintCallable)
		void CallBack_OnSelectOptionButtonPressed(UDialogueUnit_Select_Option* Option);

	UFUNCTION(BlueprintCallable)
		bool CheckSwitchUnitAndRunSwitch();

	UPROPERTY()
		bool bIsSelectorCalled = false;

//custom event

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
		FDialogueWidgetOpen WidgetOpenDele;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
		FDialogueWidgetNext WidgetNextDele;
	
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
		FDialogueWidgetClose WidgetCloseDele;

	virtual void OnDialogueWidgetBegin() { WidgetOpenDele.Broadcast(); } // ������ ����Ʈ�� ó�� �׷��� �� ȣ��˴ϴ�.

	virtual void OnNextDialoguePlayed() { WidgetNextDele.Broadcast(); } // ���� ���̾�αװ� ȭ�鿡 ó�� �׷��� �� ȣ��˴ϴ�.

	virtual void OnDialogueWidgetEnd() { WidgetCloseDele.Broadcast();} // ������ ����Ʈ���� ������ ����� �� ȣ��˴ϴ�.

};
