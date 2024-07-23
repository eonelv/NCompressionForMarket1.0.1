// Copyright 2024 ngcod, Inc. All Rights Reserved.


#include "NCompressionFunctionLibrary.h"

#include "FNCompression.h"

double UNCompressionFunctionLibrary::DecompressFileZLIB(const FString& SrcFile, const FString& DstFile, int32 BufferSize, bool bUseSavedDir)
{
	return FNCompression::DecompressFileZLIB(SrcFile, DstFile, BufferSize, bUseSavedDir);
}

double UNCompressionFunctionLibrary::CompressFileZLIB(const FString& SrcFile, const FString& DstFile, int32 BufferSize, bool bUseSavedDir)
{
	return FNCompression::CompressFileZLIB(SrcFile, DstFile, BufferSize, bUseSavedDir);
}
