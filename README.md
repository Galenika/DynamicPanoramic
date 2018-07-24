# DynamicPanoramic

A Program used to edit Counter Strike's new Panorama UI

This method bypasses the crypto checks of code.pbin by injecting into the process and editing the zip file as it loads, but after the checks done with crypto by valve.

Please Ensure you load the DLL as soon as possable, the module MUST hook before CSGO parses the zip file else it will not work.

# Credits:
qtKite-> I had not touched csgo in a while and he pointed out a few things, such as VEH Hooking

PolyHook-> Is used to apply the VEH Hooking

UnknownCheats User __or_75-> His explaination of the checks valve is doing on the code.pbin (as i originally was looking at editing the code.pbin itself)

UnknownCheats User Slidy-> His observation that the code.pbin is just a zip file	

Probably some others that i have missed, if i have missed you feel free to leave a message saying who you are and how you may have helped.

Pattern Scanning-> I cant accually remember where I pulled this from, it was a csgo cheat source, if somebody could let me know ill update this file and my comments with the name of the owner of the code

# Use:
in dllmain.cpp i have an example use of the code, all you have todo is

1. Create a std::vector of std::pairs with both params as std::strings, the first string in each pair is the position on the disk of the file to inject, the second is where you wish it to be placed inside the panorama zip.
2. Inject as CSGO starts. Injecting later will cause this not to work due to hooking the parsing of panorama.
3. Win.

# Notes:
If you wish to edit panorama you may wish to check out WesserEsser's github OR dump it yourself for latest version: https://github.com/WasserEsser/panorama

If you have any improvements please let me know <3
