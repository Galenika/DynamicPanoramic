#pragma once

#include <Windows.h>
#include <cstdio>

// Based On https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/zip_utils.h
class iZip
{
public:
	// Note, Paddings files contianed stuff from SDK i couldnt be bothered copying over
	// if you want them you can grab it all from source sdk
	enum eCompressionType
	{
		// Type of compression used for this file in the zip
		eCompressionType_Unknown = -1,
		eCompressionType_None = 0,
		eCompressionType_LZMA = 14
	};
	virtual void			Reset() = 0;
	virtual void			AddFileToZip(const char *relativename, const char *fullpath, eCompressionType compressionType = eCompressionType_None) = 0;
	virtual bool			FileExistsInZip(const char *pRelativeName) = 0;
	virtual void			Padding_0() = 0;
	virtual void			Padding_1() = 0;
	virtual void			RemoveFileFromZip(const char *relativename) = 0;
	virtual int				GetNextFilename(int id, char *pBuffer, int bufferSize, int &fileSize) = 0;
	virtual void			PrintDirectory(void) = 0;
	virtual unsigned int	EstimateSize(void) = 0;
	virtual void			AddBufferToZip(const char *relativename, void *data, int length, bool bTextMode, eCompressionType compressionType = eCompressionType_None) = 0;
	virtual void			Padding_2() = 0;
	virtual void			SaveToDisk(FILE *fout) = 0;
	virtual void			SaveToDisk(HANDLE hFileOut) = 0;
	virtual void			ParseFromBuffer(void *buffer, int bufferlength) = 0;
	virtual HANDLE			ParseFromDisk(const char *pFilename) = 0;
	virtual void			ForceAlignment(bool aligned, bool bCompatibleFormat, unsigned int alignmentSize = 0) = 0;
	virtual unsigned int	GetAlignment() = 0;
	virtual void			SetBigEndian(bool bigEndian) = 0;
	virtual void			ActivateByteSwapping(bool bActivate) = 0;

};