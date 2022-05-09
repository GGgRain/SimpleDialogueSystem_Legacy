// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/Object.h"
#include "DialogueManager.generated.h"

//DialogueSystem V.2

//���� ���� : 
//���� �� ���� ����ϵ��� ���Ǽ� ����
//����ȭ �����ϰ� ���� ����
//����ȭ + �淮ȭ (ex/ Dialogue ������ �̸��� ������Ʈ�� �̸����� ��ü,
UCLASS()
class SIMPLEDIALOGUESYSTEM_API UDialogueUnitBase : public UObject
{
	GENERATED_BODY()

public:

	UDialogueUnitBase() {}

//Dialogue control
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
		FName NextDialogueKey;

//Dialogue Property
public:

	//����! Display name�� �ƴ� �ĺ��� �̸��� ����մϴ�! 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context" )
		TArray<FName> SpeakerNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
		TArray<FName> ListenerNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
		FText ContextText;

public:

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Events")
		TMap<FName, UDialogueEvent*> Events;


public:

	//If this value is set to false, player can not touch the screen to skip context.

	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category = "Skip")
		bool bShouldEnableSkip = true;

	//When the text update is finished, after this time elipsed it automatically move to next unit. if this value is below 0, it will not used.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skip")
		float AutoSkipTimeLimit = -1;


public:

#if WITH_EDITOR

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
};

UCLASS()
class UDialogueUnit_Select_Option : public UObject
{
	GENERATED_BODY()

public:

	UDialogueUnit_Select_Option() {}

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName NextDialogueKey;

	//�������� ǥ�õǴ� �ؽ�Ʈ�Դϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText AnswerContextText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ActionText;

public:
#if WITH_EDITOR

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
};

UCLASS()
class SIMPLEDIALOGUESYSTEM_API UDialogueUnit_Select : public UDialogueUnitBase
{
	GENERATED_BODY()

public:
	UDialogueUnit_Select() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UDialogueUnit_Select_Option*> Options;

public:


#if WITH_EDITOR

	UFUNCTION(CallInEditor)
		void AddEmptyOption();

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif

};

UCLASS()
class SIMPLEDIALOGUESYSTEM_API UDialogueUnit_Switch : public UDialogueUnitBase
{
	GENERATED_BODY()

public:
	UDialogueUnit_Switch() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName SwitchKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FName, FName> SwitchMap;

public:

	//If it is true, It just uses one on the map.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bUseRandomOne = false;

	//Dialogue control

#if WITH_EDITOR

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif

};

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class SIMPLEDIALOGUESYSTEM_API UDialogueEvent : public UObject
{
	GENERATED_BODY()

public:

	UDialogueEvent() {}

public:

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveEventTrigger();

};



//�⺻������ Dialogue manager�� UDialogueBase �Ļ�Ŭ������ WrapperClass�� �������ϴ�.
UCLASS(Blueprintable)
class SIMPLEDIALOGUESYSTEM_API UDialogueManager : public UObject
{
	GENERATED_BODY()

public:
	UDialogueManager() {}

	UPROPERTY(EditAnywhere)
		FName StartingDialogueUnitKey;

	UPROPERTY()
		UDialogueUnitBase* PlayingDialogueUnit;

	UPROPERTY(EditAnywhere)
		TMap < FName, UDialogueUnitBase* > DialogueUnitMap;

	UFUNCTION(BlueprintCallable)
		UDialogueUnitBase* GetDialogueUnit(FName DialogueUnitKey);

	UFUNCTION(BlueprintCallable)
		bool HasDialogueUnit(FName DialogueUnitKey);

	UFUNCTION(BlueprintCallable)
		void GetNotifiedOnDialogueUnitBegin(FName DialogueUnitKey);

	UFUNCTION(BlueprintCallable)
		void GetNotifiedOnDialogueUnitEnd(FName DialogueUnitKey);

public:

#if WITH_EDITOR 

	UFUNCTION(BlueprintCallable, CallInEditor)
		UDialogueUnitBase* AddDialogueUnit();

	UFUNCTION(BlueprintCallable, CallInEditor)
		UDialogueUnitBase* AddSelectDialogueUnit();

	UFUNCTION(BlueprintCallable, CallInEditor)
		UDialogueUnitBase* AddSwitchDialogueUnit();

#endif

};

