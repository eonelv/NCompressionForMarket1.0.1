// Copyright 2024 ngcod, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NCompressionFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class NCOMPRESSION_API UNCompressionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "NCompression")
	static double DecompressFileZLIB(const FString& SrcFile, const FString& DstFile, int32 BufferSize = 0, bool bUseSavedDir = false);

	UFUNCTION(BlueprintCallable, Category = "NCompression")
	static double CompressFileZLIB(const FString& SrcFile, const FString& DstFile, int32 BufferSize = 0, bool bUseSavedDir = false);
};
