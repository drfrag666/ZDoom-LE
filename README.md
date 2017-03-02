 ZDoom LE (Legacy Edition), a fork of the ZDoom 2.8.1 maintenance branch (https://github.com/rheit/zdoom/tree/maint)
for Windows 9x and old machines. Created at vogons (http://www.vogons.org) by blue-green-frog and continued (for now)
by drfrag.
 Compiles with MinGW using Allegro DX8 libraries (Direct3D and Xinput support has been removed) and a modified
OpenAL-soft.

 To compile you need the original ZDoom maint repo, then overwrite with the files from this one.
 Compiles with CMake 2.8.12, CodeBlocks 16.01 (TDM-GCC 4.9.2) and NASM 2.10.09. You'll need the following libraries:
dx80_mgw.zip, openal-soft-1.12.854, mpg123-1.20-1-x86.zip, libsndfile-1.0.25-w32.setup.exe, fluidsynth.7z (optional)
and fmodapi42807win32-installer.exe (optional).
 
 Some notes:
 - Fluidsynth doesn't work with OpenAL.
 - Compiling with FModEX support should work but the exe would require Windows 98 to run.
 - The diff patch to OpenAL-soft can be applied on windows with gnu32 patch from an admin cmd prompt.
 - You can link directly against the dlls, if you get dinput undefined reference errors link to the static library
included in the Allegro minimal DX8 libraries.
 - Run CMake to generate a CodeBlocks makefile and fill the paths to libraries.
 
 You can follow the following guide:
 https://zdoom.org/wiki/Compile_ZDoom_on_Windows
 
 I need help to restore the low detail modes (r_detail).
