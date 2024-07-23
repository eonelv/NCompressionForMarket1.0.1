// Copyright 2024 ngcod, Inc. All Rights Reserved.
#include "FNCompression.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"


THIRD_PARTY_INCLUDES_START
#include "ThirdParty/zlib/1.2.13/include/zlib.h"
THIRD_PARTY_INCLUDES_END

static void *zalloc(void *opaque, unsigned int size, unsigned int num)
{
	return FMemory::Malloc(size * num);
}

static void zfree(void *opaque, void *p)
{
	FMemory::Free(p);
}

FNCompression::FNCompression()
{
}

FNCompression::~FNCompression()
{
}

double FNCompression::DecompressFileZLIB(const FString& SrcFile, const FString& DstFile, int32 BufferSize, bool bUseSavedDir)
{
	const double StartTime = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64());
	if (BufferSize <= 0)
	{
		BufferSize = 1024 * 20;
	}
	FString ProjectPersistent = FPaths::ProjectPersistentDownloadDir();
	if (bUseSavedDir)
	{
		ProjectPersistent = FPaths::ProjectSavedDir();
	}
	const FString SourceFileName = FString::Printf(TEXT("%s/%s"), *ProjectPersistent, *SrcFile);
	const gzFile compressedFile = gzopen(TCHAR_TO_UTF8(*SourceFileName), "rb");
	if (compressedFile == nullptr) {
		UE_LOG(LogNgcod, Warning, TEXT("DecompressFileZLIB, Error opening compressed file"));
		return -1;
	}
	// create zlib stream
	TArray<uint8> inputBuffer;
	inputBuffer.AddUninitialized(BufferSize);
	TArray<uint8> outputBuffer;
	outputBuffer.AddUninitialized(BufferSize);
	
	z_stream stream;
	stream.zalloc = &zalloc;
	stream.zfree = &zfree;
	stream.next_in = nullptr;
	stream.avail_in = 0;
	stream.total_out = 0;
	int ret = 0;

	// initialize zlib stream
	inflateInit(&stream);

	// open file to write
	const auto FileManager = &IFileManager::Get();
	TSharedPtr<FArchive> Ar = nullptr;
	
	const FString TargetFileName = FString::Printf(TEXT("%s/%s"), *ProjectPersistent, *DstFile);
	Ar = MakeShareable(FileManager->CreateFileWriter(*TargetFileName, EFileWrite::FILEWRITE_AllowRead));
	if (nullptr == Ar)
	{
		UE_LOG(LogNgcod, Warning, TEXT("DecompressFileZLIB, Create writter Error: Path=%s"), *TargetFileName);
		return -1;
	}
	
	// decompress
	while (true)
	{
		// 从压缩文件中读取一些数据到缓冲区
		const int bytesRead = gzread(compressedFile, inputBuffer.GetData(), inputBuffer.Num());
		if (bytesRead <= 0)
		{
			break;
		}

		// set zlib input
		stream.next_in = inputBuffer.GetData();
		stream.avail_in = bytesRead;

		// 解压数据
		do
		{
			stream.total_out = 0;
			stream.avail_out = outputBuffer.Num(); // 输出缓冲区大小
			stream.next_out = outputBuffer.GetData();      // 输出缓冲区起始地址
			ret = inflate(&stream, Z_NO_FLUSH);
			if (ret != Z_OK && ret != Z_STREAM_END)
			{
				UE_LOG(LogNgcod, Warning, TEXT("DecompressFileZLIB, Error inflating: %d"), ret);
				break;
			}

			// write decompressed data to output file
			if (stream.total_out != 0)
			{
				Ar->Serialize(outputBuffer.GetData(), stream.total_out);
			}
		}
		while (stream.avail_in > 0);
	}
	const bool bSaveSuccess = !Ar->IsError() && !Ar->IsCriticalError();

	Ar->Flush();
	Ar->Close();
	if (!bSaveSuccess)
	{
		UE_LOG(LogNgcod, Warning, TEXT("DecompressFileZLIB, Write File Error: Path=%s"), *TargetFileName);
		return -1;
	}
	// 清理
	inflateEnd(&stream);
	gzclose(compressedFile);

	const double timeCost = (FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64()) - StartTime) / 1000.0f;
	UE_LOG(LogNgcod, Display, TEXT("DecompressFileZLIB cost time: %f"), timeCost);
	return timeCost;
}

double FNCompression::CompressFileZLIB(const FString& SrcFile, const FString& DstFile, int32 BufferSize, bool bUseSavedDir)
{
	const double StartTime = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64());
	if (BufferSize <= 0)
	{
		BufferSize = 1024 * 20;
	}
	FString ProjectPersistent = FPaths::ProjectPersistentDownloadDir();
	if (bUseSavedDir)
	{
		ProjectPersistent = FPaths::ProjectSavedDir();
	}
	const FString SourceFileName = FString::Printf(TEXT("%s/%s"), *ProjectPersistent, *SrcFile);

	// Open file to read
	const auto FileManager = &IFileManager::Get();
	TSharedPtr<FArchive> ArReader = nullptr;
	ArReader = MakeShareable(FileManager->CreateFileReader(*SourceFileName, EFileWrite::FILEWRITE_AllowRead));
	if (nullptr == ArReader)
	{
		UE_LOG(LogNgcod, Warning, TEXT("CompressFileZLIB, Create writter Error: Path=%s"), *SourceFileName);
		return -1;
	}

	const FString TargetFileName = FString::Printf(TEXT("%s/%s"), *ProjectPersistent, *DstFile);
	TSharedPtr<FArchive> ArWriter = nullptr;
	ArWriter = MakeShareable(FileManager->CreateFileWriter(*TargetFileName, EFileWrite::FILEWRITE_AllowRead));
	if (nullptr == ArWriter)
	{
		UE_LOG(LogNgcod, Warning, TEXT("CompressFileZLIB, Create writter Error: Path=%s"), *TargetFileName);
		return -1;
	}
	// create zlib stream
	TArray<uint8> inputBuffer;
	inputBuffer.AddUninitialized(BufferSize);
	TArray<uint8> outputBuffer;
	outputBuffer.AddUninitialized(BufferSize);
	
	z_stream stream;
	stream.zalloc = &zalloc;
	stream.zfree = &zfree;
	stream.next_in = nullptr;
	stream.avail_in = 0;
	stream.total_out = 0;
	int ret = 0;

	if (deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK)
	{
		UE_LOG(LogNgcod, Warning, TEXT("CompressFileZLIB, Error initializing zlib compressor: Path=%s"), *TargetFileName);
		ArWriter->Flush();
		ArWriter->Close();
		ArReader->Close();
		return -1;
	}
	
	int64 totalSize = ArReader->TotalSize();
	int64 byteReaded = 0;
	// decompress
	while (totalSize > 0)
	{
		byteReaded = BufferSize;
		if (totalSize < BufferSize)
		{
			byteReaded = totalSize;
		}
		ArReader->Serialize(inputBuffer.GetData(), byteReaded);
		
		totalSize -= byteReaded;
		// set zlib input
		stream.next_in = inputBuffer.GetData();
		stream.avail_in = byteReaded;

		// 解压数据
		do
		{
			stream.total_out = 0;
			stream.avail_out = outputBuffer.Num(); // 输出缓冲区大小
			stream.next_out = outputBuffer.GetData();      // 输出缓冲区起始地址
			ret = deflate(&stream, Z_NO_FLUSH); // 使用 Z_FINISH 标志来完成压缩

			if (ret != Z_OK && ret != Z_STREAM_END)
			{
				UE_LOG(LogNgcod, Warning, TEXT("CompressFileZLIB, Error inflating: %d"), ret);
				deflateEnd(&stream);
				return -1;
			}

			// write compressed data to output file
			if (stream.total_out != 0)
			{
				ArWriter->Serialize(outputBuffer.GetData(), stream.total_out);
			}
		}
		while (stream.avail_in > 0);
	}
	do
	{
		stream.avail_out = static_cast<uInt>(outputBuffer.Num());
		ret = deflate(&stream, Z_FINISH);
		if (ret != Z_OK && ret != Z_STREAM_END)
		{
			UE_LOG(LogNgcod, Warning, TEXT("CompressFileZLIB, Error finishing deflate: : %d"), ret);
			deflateEnd(&stream);
			return 1;
		}
		if (stream.total_out != 0)
		{
			ArWriter->Serialize(outputBuffer.GetData(), stream.total_out);
		}
	} while (stream.avail_out == 0);
	const bool bSaveSuccess = !ArWriter->IsError() && !ArWriter->IsCriticalError();

	// 清理
	deflateEnd(&stream);
	
	ArWriter->Flush();
	ArWriter->Close();
	ArReader->Close();
	if (!bSaveSuccess)
	{
		UE_LOG(LogNgcod, Warning, TEXT("CompressFileZLIB, Write File Error: Path=%s"), *TargetFileName);
		return -1;
	}
	
	const double timeCost = (FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64()) - StartTime) / 1000.0f;
	UE_LOG(LogNgcod, Display, TEXT("CompressFileZLIB cost time: %f"), timeCost);
	return timeCost;
}
