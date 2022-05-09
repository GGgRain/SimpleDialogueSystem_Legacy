// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidgetBase.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "DialogueButtonWidget.h"

#include "Kismet/GameplayStatics.h" 
#include "SaveManager.h"

#if WITH_EDITOR 
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#endif

#include "SaveGame/SlotData_FlowControl.h"

void UDialogueWidgetBase::InitializeDialogueWidget(UDialogueManager* InBaseDialogueManager) {
	BaseDialogueManager = InBaseDialogueManager;
}

void UDialogueWidgetBase::SetFirstDialogueUnitBasedOnCurrentDialogue() {
	if (BaseDialogueManager != nullptr) {
		if (BaseDialogueManager->HasDialogueUnit(BaseDialogueManager->StartingDialogueUnitKey)) {
			//���� ���̾�αװ� �ִ� ���...
			CurrentPlayingDialogueUnit = BaseDialogueManager->GetDialogueUnit(BaseDialogueManager->StartingDialogueUnitKey);
			InitializeBasedOnCurrentDialogue();
		}
	}
}

void UDialogueWidgetBase::NativeConstruct() {
	UUserWidget::NativeConstruct();

	ContextBlock = Cast<URichTextBlock>(GetWidgetFromName(TEXT("ContextBlockChild")));
	OwnerNameBlock = Cast<URichTextBlock>(GetWidgetFromName(TEXT("OwnerNameBlockChild")));
	SeletorScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("SeletorScrollBoxChild")));
	SkipButton = Cast<UButton>(GetWidgetFromName(TEXT("SkipButtonChild")));

}

void UDialogueWidgetBase::NativeDestruct() {
	UUserWidget::NativeDestruct();
	OnDialogueWidgetEnd();

	if (TimerManagerRef != nullptr) {
		TimerManagerRef->ClearTimer(TextUpdateHandle);
		TimerManagerRef->ClearAllTimersForObject(this);
	}
}

void UDialogueWidgetBase::ExplictDestruct() {
	NativeDestruct();
}

void UDialogueWidgetBase::StartUpdateText() {
	if (GetWorld()) {
		TimerManagerRef = &GetWorld()->GetTimerManager();
		if (TimerManagerRef != nullptr) {
			TimerManagerRef->SetTimer(TextUpdateHandle, this, &UDialogueWidgetBase::UpdateText, TextUpdateFrequency ,true , 0);
		}
	}
}


void UDialogueWidgetBase::UpdateText() {
	if (this) {

		FString ToString = ContextBlockText.ToString();

		if (NumberOfShowingChar > ToString.Len()) {
			StopUpdateText();
		}
		SkipDecorator();

		FString Str = ToString.Left(FMath::Clamp(NumberOfShowingChar, 0, ToString.Len()));
		if (bShouldAppendEndDecorator) {
			Str.Append("</>");
		}

		if (CustomTagMap.Contains(NumberOfShowingChar)) {
			for (int Index = 0; Index < CustomTagMap[NumberOfShowingChar].CustomTags.Num(); Index++) {
				CallCommand(CustomTagMap[NumberOfShowingChar].CustomTags[Index]);
			}
			CustomTagMap.Remove(NumberOfShowingChar);
		}


		ContextBlock->SetText(FText::FromString(*Str));

		NumberOfShowingChar++;
	}
}

void UDialogueWidgetBase::SkipDecorator() {
	//'<'�� �����ҽ�, ���� �Ǵ� </>�� �����ϴ��� Ž���� ��, ���� ���� �Ǵ� </>�� �����Ѵٸ� ã�� '<'�� ���� ��ɾ �����Ѵ�.

	FString ToString = ContextBlockText.ToString();

	if (ToString.IsValidIndex(NumberOfShowingChar - 1)) {
		if (ToString[NumberOfShowingChar - 1] == '<') {//���� ���ڷ����� ���� �ɺ��� ������ ã�Ҵٸ� :
			int StartIndex = ToString.Find("</>", ESearchCase::IgnoreCase, ESearchDir::FromStart, NumberOfShowingChar - 1);
			if (StartIndex != -1) { //���� ���ڷ����� ���� �ɺ��� ã�Ҵٸ� :
				int EndIndex = ToString.Find(">", ESearchCase::IgnoreCase, ESearchDir::FromStart, NumberOfShowingChar - 1);
				if (EndIndex != -1) { //���� ���ڷ����� ���� �ɺ��� ���� ã�Ҵٸ� :
					NumberOfShowingChar += EndIndex - (NumberOfShowingChar - 1);
				}
			}
		}
	}
}

void UDialogueWidgetBase::StopUpdateText() {
	if (TimerManagerRef != nullptr) {
		TimerManagerRef->ClearTimer(TextUpdateHandle);
	}

	if (!CurrentPlayingDialogueUnit) {
		return;
	}

	if (TimerManagerRef) {
		if (/*CurrentPlayingDialogueUnit->bShouldEnableSkip == false && */CurrentPlayingDialogueUnit->AutoSkipTimeLimit >= 0) {
			TimerManagerRef->SetTimer(AutoSkipHandle, this, &UDialogueWidgetBase::SkipCurrentUnit, CurrentPlayingDialogueUnit->AutoSkipTimeLimit, false, CurrentPlayingDialogueUnit->AutoSkipTimeLimit);
		}
	}
}

void UDialogueWidgetBase::SkipContext() {
	FString ToString = ContextBlockText.ToString();
	if (NumberOfShowingChar > ToString.Len()) {
		if (!bIsSelectorCalled) {
			TryStartNextDialogue();
		}
	}
	else {
		NumberOfShowingChar += float(ToString.Len()) / 4.f;

		//���� ���ڷ����� �ȿ��� ��ŵ�� �����ٸ� ���ڷ����� ������ Ż���մϴ�
		FString Str = ToString.Left(FMath::Clamp(NumberOfShowingChar, 0, ToString.Len()));
		int StartIndex = Str.Find("<", ESearchCase::IgnoreCase, ESearchDir::FromEnd, NumberOfShowingChar - 1);
		int EndIndex = Str.Find(">", ESearchCase::IgnoreCase, ESearchDir::FromEnd, NumberOfShowingChar - 1);
		if (EndIndex < StartIndex || (EndIndex == -1 && StartIndex != -1)) {// < , >�� ��� ã�Ҵµ�, > < �����϶� || <�� ã�Ҵµ� >�� ��ã������
			int SkipIndex = ToString.Find(">", ESearchCase::IgnoreCase, ESearchDir::FromStart, NumberOfShowingChar - 1);
			NumberOfShowingChar += SkipIndex - (NumberOfShowingChar - 1) + 1;
		}
		CheckAndPlaySkippedCommand();
	}
}

void UDialogueWidgetBase::CheckAndPlaySkippedCommand() {
	TArray<int32> Keys;
	CustomTagMap.GetKeys(Keys);
	for (int32 Key : Keys) {
		if (Key <= NumberOfShowingChar) {
			for (int Index = 0; Index < CustomTagMap[Key].CustomTags.Num(); Index++) {
				CallCommand(CustomTagMap[Key].CustomTags[Index]);
			}
			CustomTagMap.Remove(Key);
		}
	}
}

void UDialogueWidgetBase::TryStartNextDialogue() {
	FString ToString = ContextBlockText.ToString();
	if (NumberOfShowingChar > ToString.Len()) {
		if (BaseDialogueManager != nullptr) {
			//���� ���̾�αװ� �ִ� ���...
			if (CurrentPlayingDialogueUnit != nullptr) {
				if (BaseDialogueManager->HasDialogueUnit(CurrentPlayingDialogueUnit->NextDialogueKey)) {
					CurrentPlayingDialogueUnit = BaseDialogueManager->GetDialogueUnit(CurrentPlayingDialogueUnit->NextDialogueKey);
					InitializeBasedOnCurrentDialogue();
					OnNextDialoguePlayed();
					StartUpdateText();
				}
				else {
					CloseAction();
				}
			}
			else {
				CloseAction();
			}

		}
	}
}
void UDialogueWidgetBase::InitializeBasedOnCurrentDialogue() {
	if (CurrentPlayingDialogueUnit != nullptr) {
		if (CurrentPlayingDialogueUnit->IsValidLowLevel()) {
			NumberOfShowingChar = 0;

			TArray<UWidget*> Widgets;
			if (SeletorScrollBox != nullptr) {
				Widgets = SeletorScrollBox->GetAllChildren();
				for (int i = 0; i < Widgets.Num(); i++) {
					Widgets[i]->RemoveFromParent();
				}
			}



			bIsSelectorCalled = false;


			if (!CheckSwitchUnitAndRunSwitch()) {

				UpdateContextBlockTextBasedOnCurrentDialogue();
				UpdateSpeakerBlockTextBasedOnCurrentDialogue();
			}

			if (SkipButton) {
				SkipButton->bIsEnabled = (CurrentPlayingDialogueUnit->bShouldEnableSkip);
			}
		}
	}
}

void UDialogueWidgetBase::SkipCurrentUnit() {
	if (!TimerManagerRef) {
		return;
	}

	TryStartNextDialogue();
}

void UDialogueWidgetBase::UpdateContextBlockTextBasedOnCurrentDialogue() {
	if (CurrentPlayingDialogueUnit != nullptr) {
		if (CurrentPlayingDialogueUnit->IsValidLowLevel()) {
			ContextBlockText = CurrentPlayingDialogueUnit->ContextText;

			CollectCustomTagsFromCurrentContextBlockText();
		}
	}
}

void UDialogueWidgetBase::CollectCustomTagsFromCurrentContextBlockText() {

	FString ToString = ContextBlockText.ToString();

	CustomTagMap.Empty();
	FString StartTagSymbol = "{{";
	FString LastTagSymbol = "}}";


	bool bShouldContinue = true;
	// '{TAG:' Ű���带 ã���ϴ�.
	do {

		int StartIndex = ToString.Find(StartTagSymbol, ESearchCase::IgnoreCase, ESearchDir::FromStart);
		int LastIndex = ToString.Find(LastTagSymbol, ESearchCase::IgnoreCase, ESearchDir::FromStart);
#if WITH_EDITOR
		//checkf((StartIndex != -1 && LastIndex != -1), TEXT("There is a unclosed or misclosed Tag symbol in in Dialogue Manager '%s'-> Dialogue Unit '%s'  "), *BaseDialogueManager->GetFName().ToString(), *CurrentPlayingDialogueUnit->GetFName().ToString());
		//checkf((StartIndex <= LastIndex), TEXT("There is a '/}' Symbol before the '{TAG:' Symbol in Dialogue Manager '%s'-> Dialogue Unit '%s' "), *BaseDialogueManager->GetFName().ToString(), *CurrentPlayingDialogueUnit->GetFName().ToString());
#endif

		if ((StartIndex != -1 && LastIndex != -1) && (StartIndex <= LastIndex)) {
			//���������� TAG �ɺ��� ã���� ���...
			FString Str = ToString.Mid(StartIndex + StartTagSymbol.Len(), LastIndex - StartIndex - LastTagSymbol.Len() - StartTagSymbol.Len() + 2);

			//�̹� �ش� Ű�� �ε����� �����Ҷ� ( == ���ý���)
			if (CustomTagMap.Contains(StartIndex)) {
				CustomTagMap[StartIndex].CustomTags.Add(Str);
			}
			else {
				TArray<FString> Arr;
				Arr.Add(Str);
				CustomTagMap.Add(StartIndex, Arr);
			}

			ToString.RemoveAt(StartIndex, LastIndex - StartIndex + StartTagSymbol.Len() + LastTagSymbol.Len() - 2);
			bShouldContinue = true;
		}
		else {
			//TAG �ɺ��� �� ã���� ���...
			bShouldContinue = false;
		}
	} while (bShouldContinue);

	ContextBlockText = FText::FromString(ToString);
}


void UDialogueWidgetBase::UpdateSpeakerBlockTextBasedOnCurrentDialogue() {
	if (CurrentPlayingDialogueUnit != nullptr) {
		if (CurrentPlayingDialogueUnit->IsValidLowLevel()) {
			TArray<FName> SpeakerNames;
			SpeakerNames = CurrentPlayingDialogueUnit->SpeakerNames;
			FString FullNameString = FString();
			for (int i = 0; i < SpeakerNames.Num(); i++) {

				if (i != 0) {
					FullNameString.Append(",");
				}
				FullNameString.Append(SpeakerNames[i].ToString());
			}
			OwnerNameBlock->SetText(FText::FromString(FullNameString));
		}
	}
}


void UDialogueWidgetBase::AddSelectOptionButtonOnWidget() {
	if (CurrentPlayingDialogueUnit != nullptr) {
		UDialogueUnit_Select* Unit = Cast<UDialogueUnit_Select>(CurrentPlayingDialogueUnit);
		if (Unit) {
			if (SeletorScrollBox != nullptr) {

				for (int i = 0; i < Unit->Options.Num(); i++) {
					UDialogueButtonWidget* DialogueButtonWidget = CreateWidget<UDialogueButtonWidget>(this, DialogueButtonWidgetClass);
					DialogueButtonWidget->InitializeDialogueButtonWidget(Unit->Options[i], this);
					SeletorScrollBox->AddChild(DialogueButtonWidget);

					bIsSelectorCalled = true;
				}
			}
		}
	}
}

void UDialogueWidgetBase::CallBack_OnSelectOptionButtonPressed(UDialogueUnit_Select_Option* Option) {
	if (CurrentPlayingDialogueUnit != nullptr) {
		UDialogueUnit_Select* Unit = Cast<UDialogueUnit_Select>(CurrentPlayingDialogueUnit);
		if (Unit) {

			TArray<FString> CustomTags;

			FString StartTagSymbol = "{{";
			FString LastTagSymbol = "}}";

			bool bShouldContinue = true;
			do {

				int StartIndex = Option->ActionText.Find(StartTagSymbol, ESearchCase::IgnoreCase, ESearchDir::FromStart);
				int LastIndex = Option->ActionText.Find(LastTagSymbol, ESearchCase::IgnoreCase, ESearchDir::FromStart);


				if ((StartIndex != -1 && LastIndex != -1) && (StartIndex <= LastIndex)) {
					//���������� TAG �ɺ��� ã���� ���...
					FString Str = Option->ActionText.Mid(StartIndex + StartTagSymbol.Len(), LastIndex - StartIndex - LastTagSymbol.Len() - StartTagSymbol.Len() + 2);
					CustomTags.Add(Str);
					Option->ActionText.RemoveAt(StartIndex, LastIndex - StartIndex + StartTagSymbol.Len() + LastTagSymbol.Len() - 2);
					bShouldContinue = true;
				}
				else {
					//TAG �ɺ��� �� ã���� ���...
					bShouldContinue = false;
				}
			} while (bShouldContinue);

			for (int i = 0; i < CustomTags.Num(); i++) {
				CallCommand(CustomTags[i]);
			}


			if (BaseDialogueManager->HasDialogueUnit(Option->NextDialogueKey)) {
				CurrentPlayingDialogueUnit = BaseDialogueManager->GetDialogueUnit(Option->NextDialogueKey);
				InitializeBasedOnCurrentDialogue();
				OnNextDialoguePlayed();
				StartUpdateText();
			}
			else {
				CloseAction();
			}

		}
	}
	else {
		CloseAction();
	}
	
}


bool UDialogueWidgetBase::CheckSwitchUnitAndRunSwitch() {

	bool IsSucceed = false;
	if (CurrentPlayingDialogueUnit != nullptr && BaseDialogueManager != nullptr) {
		if (UDialogueUnit_Switch* Casted = Cast<UDialogueUnit_Switch>(CurrentPlayingDialogueUnit)) {
			//Casted->SwitchKey

			FName KeyValue = NAME_None; //ã�� �ڵ� ��������

			if (Casted->bUseRandomOne) {
				int Num = Casted->SwitchMap.Num();

				if (Num > 0) {
					TArray<FName> Keys;
					Casted->SwitchMap.GetKeys(Keys);

					KeyValue = Keys[FMath::RandRange(0, Num - 1)];
				}
			}
			else if (UGameInstance* Instance = UGameplayStatics::GetGameInstance(this)) {
				if (USaveManager* SaveSubsystem = Instance->GetSubsystem<USaveManager>()) {
					if (USlotData* Data = SaveSubsystem->GetCurrentData()) {
						if (USlotData_FlowControl* CastedData = Cast<USlotData_FlowControl>(Data)) {

							KeyValue = FName(CastedData->GetValueByKey(Casted->SwitchKey));

						}
					}
				}
			}

			if (KeyValue != NAME_None) {
				if (Casted->SwitchMap.Contains(KeyValue)) {
					FName NextUnitName = Casted->SwitchMap[KeyValue];
					if (this->BaseDialogueManager->DialogueUnitMap.Contains(NextUnitName)) {
						CurrentPlayingDialogueUnit = this->BaseDialogueManager->DialogueUnitMap[NextUnitName];

						IsSucceed = true;
						InitializeBasedOnCurrentDialogue();
					}
				}
			}
#if WITH_EDITOR
			else {

				const FText notificationText = FText::FromString(FString("Switch value for key" + Casted->SwitchKey.ToString() + "was not vaild!"));
				FNotificationInfo info(notificationText);
				info.bFireAndForget = true;
				info.FadeInDuration = 0.5f;
				info.FadeOutDuration = 1.0f;
				info.ExpireDuration = 4.0f;

				FSlateNotificationManager::Get().AddNotification(info);
			}
#endif
		}
	}

	return IsSucceed;
}


void UDialogueWidgetBase::AddToViewport_Implemented() {
	if (this) {
		AddToViewport();
		OnDialogueWidgetBegin();
		OpenAction();
	}
}

void UDialogueWidgetBase::OpenAction() {
	StartUpdateText();
}

void UDialogueWidgetBase::CloseAction() {
	if (SkipButton) {
		SkipButton->bIsEnabled = false;
	}
}