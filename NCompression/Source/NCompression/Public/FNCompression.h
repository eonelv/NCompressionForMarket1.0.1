// Copyright 2024 ngcod, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DEFINE_LOG_CATEGORY_STATIC(LogNgcod, Display, All);

/**
 * 
 */
class NCOMPRESSION_API FNCompression
{
public:
	FNCompression();
	~FNCompression();
	static double DecompressFileZLIB(const FString& SrcFile, const FString& DstFile, int32 BufferSize = 0, bool bUseSavedDir = false);
	static double CompressFileZLIB(const FString& SrcFile, const FString& DstFile, int32 BufferSize = 0, bool bUseSavedDir = false);
};
