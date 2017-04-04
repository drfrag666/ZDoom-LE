 ZDoom LE (Legacy Edition), a fork of the ZDoom 2.8.1 maintenance branch (https://github.com/rheit/zdoom/tree/maint)
for Windows 9x and old machines. It's compiled with pentium ii optimizations and low detail modes have been restored.
 Created at vogons (http://www.vogons.org) by blue-green-frog and continued by drfrag.
 Compiles with MinGW using Allegro DX9 libraries (Xinput support has been removed) and a modified OpenAL-soft.
 Can compile as well with allegro DX8 libraries but without Direct3D support.

 To compile you need the original ZDoom maint repo, then overwrite with the files from this one.
 Compiles with CMake 2.8.12, CodeBlocks 16.01 (TDM-GCC 4.9.2) and NASM 2.10.09. You'll need the following libraries:
dx9mgw.zip, dx80_mgw.zip (optional), openal-soft-1.12.854, mpg123-1.20-1-x86.zip, libsndfile-1.0.25-w32.setup.exe,
fluidsynth.7z (optional) and fmodapi42636win32-installer.exe (optional).
 To get win95 support you need TDM-GCC 4.6.1 instead, anyway it's better to install TDM-GCC 4.6.1 since it's much
faster than 4.9.2. A sample CmMakeCache.txt to generate a CodeBlocks makefile is included.
 
 Some notes:
 - Fluidsynth doesn't work with OpenAL.
 - Compiling with FModEX support should work but the exe would require Windows 98 to run.
 - The diff patch to OpenAL-soft can be applied on windows with gnu32 patch from an admin cmd prompt.
 - You can link directly against the dlls but not for DX (dinput).
 - Run CMake to generate a CodeBlocks makefile and fill or change the paths to executables and libraries.
 
 You can follow the following guide:
 https://zdoom.org/wiki/Compile_ZDoom_on_Windows
 