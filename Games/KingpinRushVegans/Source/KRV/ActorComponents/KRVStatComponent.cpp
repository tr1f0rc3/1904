// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVStatComponent.h"
#include "KRV.h"

UKRVStatComponent::UKRVStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	//bWantsInitializeComponent = true;
}

UKRVStatComponent* UKRVStatComponent::LoadData(const FName& UnitName)
{
	return this;

}

UKRVStatComponent* UKRVStatComponent::AssignmentOperator(const UKRVStatComponent* const Other)
{
	RangedAttackStat = Other->RangedAttackStat;
	return this;

}

