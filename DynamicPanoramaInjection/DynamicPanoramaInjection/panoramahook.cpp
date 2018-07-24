#include "stdafx.h"
#include <vector>
#include <string>
#include <Windows.h>
#include <Psapi.h>
#include <iostream>

// Requires PolyHook
#include "../../../PolyHook/PolyHook/PolyHook.hpp"

// iZip File Structure
#include "iZip.h"

#define INVALID_HANDLE 0

PLH::VEHHook ParseFromBufferHook;

// Files to Inject are stored here for injection
std::vector<std::pair<std::string/*File on Disk*/, std::string/*File in Panorama zip*/>> Injection_Filelist = {};

// Check if zip file is code.pbin
bool IsZipFile_Panorama(iZip* ZipFile)
{
	// Note, this is probably not needed as 
	// im pretty sure the only code using the
	// zip utils inside panorama.dll is 
	// in creation of panorama, but i guess this just
	// will futureproof it a littlebit :P

	char Filename[260] = { 0 };
	int FileSize = 0;

	int Fileid = ZipFile->GetNextFilename(-1, Filename, 260, FileSize);

	if (Filename[0] == 'p' && // There is probably a better way to check this
		Filename[1] == 'a' && // But im lazy and tired
		Filename[2] == 'n' &&
		Filename[3] == 'o' &&
		Filename[4] == 'r' &&
		Filename[5] == 'a' &&
		Filename[6] == 'm' &&
		Filename[7] == 'a')
	{
		return true;
	}

	return false;
}

//void __thiscall iZip::ParseFromBuffer(char *thisptr, void *Buffer, size_t Size)
void __stdcall Hooked_ParseFromBuffer(void* Buffer, int Size)
{
	// __thiscall - this pointer stored inside the ecx register
	// Probably a better way to get this but im too tired to do that now
	iZip* ZipFile = nullptr;
	__asm
	{
		mov ZipFile, ecx
	}

	// VEH Stuffs :P
	auto Object = ParseFromBufferHook.GetProtectionObject();

	// Call the original Parse from buffer
	ZipFile->ParseFromBuffer(Buffer, Size);

	if (/*IsZipFile_Panorama(ZipFile)*/ true) // I havn't accually testeed IsZipFile_Panorama and i cbf because im done working on this
	{
		// Loop though the files we wish to inject and inject them
		for (const auto& File : Injection_Filelist)
		{
			const char* File_Path = File.first.c_str();
			const char* File_Reletive = File.second.c_str();

			// If we overwrite a file, we want to delete it first.
			if (ZipFile->FileExistsInZip(File_Reletive))
			{
				ZipFile->RemoveFileFromZip(File_Reletive);
			}

			// Add our file to zip
			ZipFile->AddFileToZip(File_Reletive, File_Path);
		}
		// Unhook ParseFromBuffer as we do not need to 
		// Run this code for any more zip files to come
		ParseFromBufferHook.UnHook();

#ifdef _DEBUG
		// Output all files inside zip file
		ZipFile->PrintDirectory();
#endif
	}
}

// pattern scan pulled from a random source on internet
// sorry i cant remember where i pulled it from i was
// being lazy
// If anyone knows who made it ill happly update this 
// comment
DWORD FindPattern(HMODULE Module, std::string pattern)
{
#define INRANGE(x,a,b)    (x >= a && x <= b)
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

	const char* pat = pattern.c_str();
	DWORD firstMatch = 0;
	DWORD rangeStart = (DWORD)Module;
	MODULEINFO miModInfo;
	GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
	DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
	for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
	{
		if (!*pat) return firstMatch;

		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
		{
			if (!firstMatch) firstMatch = pCur;

			if (!pat[2]) return firstMatch;

			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;
			else pat += 2; //one ?
		}
		else
		{
			pat = pattern.c_str();
			firstMatch = 0;
		}
	}
	return NULL;
}

void HookPanoramaUI(const std::vector<std::pair<std::string, std::string>>& FilesToInject)
{
	Injection_Filelist = FilesToInject;

	HMODULE Panorama_Dll = GetModuleHandleA("panorama.dll");

	while (Panorama_Dll == INVALID_HANDLE)
	{
		Panorama_Dll = GetModuleHandleA("panorama.dll"); // If panorama.dll hasnt been loaded in yet
														// Wait until we can find panorama.dll
	}

	// Find Parse From Buffer in panorama.dll
	// This function is called when valve go to parse code.pbin after the 
	// cryto checks and stuff like that
	// If we hook this we can edit the zip
	// while it loads
	uint8_t* OriginalParseFromBuffer = (uint8_t*)FindPattern(Panorama_Dll, "55 8B EC 56 8D 71 04 8D 4E 0C E8 ? ? ? ? 8B 46 34 83 F8 FF 74 23 50 FF 15 ? ? ? ? 83 7E 44 00 B8 ? ? ? ? 74 03 8B 46 38 50 FF 15 ? ? ? ? C7 46 ? ? ? ? ? 80 7E 30 00 74 0E 8D 56 38 8D 4E 48 E8 ? ? ? ? 89 46 34 FF 75 0C");
	if (!OriginalParseFromBuffer)
	{
#ifdef _DEBUG
		std::cout << "Cannot Find ParseFromBuffer(), press enter to exit" << std::endl;
		std::cin.get();
#endif
		return;
	}

	// Hook Parse from buffer
	ParseFromBufferHook.SetupHook(OriginalParseFromBuffer, (uint8_t*)Hooked_ParseFromBuffer, PLH::VEHHook::VEHMethod::INT3_BP);
	ParseFromBufferHook.Hook();

}