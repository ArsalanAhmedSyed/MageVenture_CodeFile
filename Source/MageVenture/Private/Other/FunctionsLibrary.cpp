
#include "Other/FunctionsLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Player/Enums/EInputModeType.h"
#include "Player/Interfaces/AttributeInterface.h"

float UFunctionsLibrary::CalculateDamage(float DamageAmount, AActor* Attacker, AActor* Defender)
{
	IAttributeInterface* AttackAttributeInterface = Cast<IAttributeInterface>(Attacker);
	IAttributeInterface* DefenderAttributeInterface = Cast<IAttributeInterface>(Defender);

	if(AttackAttributeInterface == nullptr || DefenderAttributeInterface == nullptr) { return INDEX_NONE; }

	const float AttackerStat = AttackAttributeInterface->GetInterfaceAttribute(EAttributeType::EAT_Attack).MaxValue;
	const float DefenderStat = DefenderAttributeInterface->GetInterfaceAttribute(EAttributeType::EAT_Defence).MaxValue;

	const float FinalDamage = DamageAmount * (AttackerStat / DefenderStat);

	return FMath::Max(FinalDamage, 0);
}

void UFunctionsLibrary::ChangeInputMode(const UObject* WorldObject, APlayerController* PlayerController, EInputModeType InputModeType, UUserWidget* WidgetToFocus)
{
	FInputModeGameAndUI GameAndUIMode{};
	FInputModeUIOnly UIOnlyMode{};
	const FInputModeGameOnly GameOnlyMode{};
	
	switch (InputModeType)
	{
	case EInputModeType::EIMT_GameAndUI:
		GameAndUIMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
		PlayerController->SetInputMode(GameAndUIMode);
		PlayerController->bShowMouseCursor = true;
		break;
	case EInputModeType::EIMT_UIOnly:
		UIOnlyMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
		PlayerController->SetInputMode(UIOnlyMode);
		PlayerController->bShowMouseCursor = true;
		break;
	case EInputModeType::EIMT_GameOnly:
		PlayerController->SetInputMode(GameOnlyMode);
		PlayerController->bShowMouseCursor = false;
		break;
	default:
		break;
	}
}
