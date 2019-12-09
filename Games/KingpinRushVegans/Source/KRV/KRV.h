// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(KRV, Log, All);
#define KRVLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define KRVLOG_S(Verbosity) UE_LOG(KRV, Verbosity, TEXT("%s"), *KRVLOG_CALLINFO)
#define KRVLOG(Verbosity, Format, ...) UE_LOG(KRV, Verbosity, TEXT("%s %s"), *KRVLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define KRVCHECK(Expr, ...) { if(!(Expr)) {KRVLOG(Error, TEXT("ASSERTION: %s"), TEXT("'"#Expr"'")); } }


