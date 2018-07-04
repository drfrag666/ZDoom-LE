#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <lmcons.h>
#include <shlobj.h>
#define USE_WINDOWS_DWORD
#endif

#include "cmdlib.h"
#include "m_misc.h"
#include "i_system.h"

#if !defined(__APPLE__) && !defined(_WIN32)
#include <sys/stat.h>
#include <sys/types.h>
//#include "i_system.h"
#endif

#include "version.h"	// for GAMENAME

#if defined(_WIN32)

//===========================================================================
//
// IsProgramDirectoryWritable
//
// If the program directory is writable, then dump everything in there for
// historical reasons. Otherwise, known folders get used instead.
//
//===========================================================================

//===========================================================================
//
// GetKnownFolder
//
// Returns the known_folder if SHGetKnownFolderPath is available, otherwise
// returns the shell_folder using SHGetFolderPath.
//
//===========================================================================

//===========================================================================
//
// M_GetCachePath													Windows
//
// Returns the path for cache GL nodes.
//
//===========================================================================

FString M_GetCachePath(bool create)
{
	FString path;

	path = progdir;
	// Don't use GAME_DIR and such so that ZDoom and its child ports can
	// share the node cache.
	path += "/zdoom/cache";
	path.Substitute("//", "/");	// needed because progdir ends with a slash.
	return path;
}

//===========================================================================
//
// M_GetAutoexecPath												Windows
//
// Returns the expected location of autoexec.cfg.
//
//===========================================================================

FString M_GetAutoexecPath()
{
	return "$PROGDIR/autoexec.cfg";
}

//===========================================================================
//
// M_GetCajunPath													Windows
//
// Returns the location of the Cajun Bot definitions.
//
//===========================================================================

FString M_GetCajunPath(const char *botfilename)
{
	FString path;

	path << progdir << "zcajun/" << botfilename;
	if (!FileExists(path))
	{
		path = "";
	}
	return path;
}

//===========================================================================
//
// M_GetConfigPath													Windows
//
// Returns the path to the config file. On Windows, this can vary for reading
// vs writing. i.e. If $PROGDIR/zdoom-<user>.ini does not exist, it will try
// to read from $PROGDIR/zdoom.ini, but it will never write to zdoom.ini.
//
//===========================================================================

FString M_GetConfigPath(bool for_reading)
{
	FString path;
	HRESULT hr;

	// Construct a user-specific config name
	// construct "$PROGDIR/zdoom-$USER.ini"
	if (OSPlatform == os_Win2k)
	{
		TCHAR uname[UNLEN+1];
		DWORD unamelen = countof(uname);

		path = progdir;
		hr = GetUserName(uname, &unamelen);
		if (SUCCEEDED(hr) && uname[0] != 0)
		{
			// Is it valid for a user name to have slashes?
			// Check for them and substitute just in case.
			char *probe = uname;
			while (*probe != 0)
			{
				if (*probe == '\\' || *probe == '/')
					*probe = '_';
				++probe;
			}
			path << GAMENAMELOWERCASE "-" << uname << ".ini";
		}
		else
		{ // Couldn't get user name, so just use zdoom.ini
			path += GAMENAMELOWERCASE ".ini";
		}
	}
	else
	{
		path = progdir;
		path += GAMENAMELOWERCASE ".ini";
	}
	
	// If we are reading the config file, check if it exists. If not, fallback
	// to $PROGDIR/zdoom.ini
	if (for_reading)
	{
		if (!FileExists(path))
		{
			path = progdir;
			path << GAMENAMELOWERCASE ".ini";
		}
	}

	return path;
}

//===========================================================================
//
// M_GetScreenshotsPath												Windows
//
// Returns the path to the default screenshots directory.
//
//===========================================================================

// I'm not sure when FOLDERID_Screenshots was added, but it was probably
// for Windows 8, since it's not in the v7.0 Windows SDK.
static const GUID MyFOLDERID_Screenshots = { 0xb7bede81, 0xdf94, 0x4682, 0xa7, 0xd8, 0x57, 0xa5, 0x26, 0x20, 0xb8, 0x6f };

FString M_GetScreenshotsPath()
{
	FString path;

	path << progdir << "Screenshots/";
	return path;
}

//===========================================================================
//
// M_GetSavegamesPath												Windows
//
// Returns the path to the default save games directory.
//
//===========================================================================

FString M_GetSavegamesPath()
{
	FString path;

	path = progdir;
	return path;
}

#elif defined(__APPLE__)

//===========================================================================
//
// M_GetCachePath													Mac OS X
//
// Returns the path for cache GL nodes.
//
//===========================================================================

FString M_GetCachePath(bool create)
{
	FString path;

	char pathstr[PATH_MAX];
	FSRef folder;

	if (noErr == FSFindFolder(kUserDomain, kApplicationSupportFolderType, create ? kCreateFolder : 0, &folder) &&
		noErr == FSRefMakePath(&folder, (UInt8*)pathstr, PATH_MAX))
	{
		path = pathstr;
	}
	else
	{
		path = progdir;
	}
	path += "/zdoom/cache";
	return path;
}

//===========================================================================
//
// M_GetAutoexecPath												Mac OS X
//
// Returns the expected location of autoexec.cfg.
//
//===========================================================================

FString M_GetAutoexecPath()
{
	FString path;

	char cpath[PATH_MAX];
	FSRef folder;
	
	if (noErr == FSFindFolder(kUserDomain, kDocumentsFolderType, kCreateFolder, &folder) &&
		noErr == FSRefMakePath(&folder, (UInt8*)cpath, PATH_MAX))
	{
		path << cpath << "/" GAME_DIR "/autoexec.cfg";
	}
	return path;
}

//===========================================================================
//
// M_GetCajunPath													Mac OS X
//
// Returns the location of the Cajun Bot definitions.
//
//===========================================================================

FString M_GetCajunPath(const char *botfilename)
{
	FString path;

	// Just copies the Windows code. Should this be more Mac-specific?
	path << progdir << "zcajun/" << botfilename;
	if (!FileExists(path))
	{
		path = "";
	}
	return path;
}

//===========================================================================
//
// M_GetConfigPath													Mac OS X
//
// Returns the path to the config file. On Windows, this can vary for reading
// vs writing. i.e. If $PROGDIR/zdoom-<user>.ini does not exist, it will try
// to read from $PROGDIR/zdoom.ini, but it will never write to zdoom.ini.
//
//===========================================================================

FString M_GetConfigPath(bool for_reading)
{
	char cpath[PATH_MAX];
	FSRef folder;
	
	if (noErr == FSFindFolder(kUserDomain, kPreferencesFolderType, kCreateFolder, &folder) &&
		noErr == FSRefMakePath(&folder, (UInt8*)cpath, PATH_MAX))
	{
		FString path;
		path << cpath << "/" GAMENAMELOWERCASE ".ini";
		return path;
	}
	// Ungh.
	return GAMENAMELOWERCASE ".ini";
}

//===========================================================================
//
// M_GetScreenshotsPath												Mac OS X
//
// Returns the path to the default screenshots directory.
//
//===========================================================================

FString M_GetScreenshotsPath()
{
	FString path;
	char cpath[PATH_MAX];
	FSRef folder;
	
	if (noErr == FSFindFolder(kUserDomain, kDocumentsFolderType, kCreateFolder, &folder) &&
		noErr == FSRefMakePath(&folder, (UInt8*)cpath, PATH_MAX))
	{
		path << cpath << "/" GAME_DIR "/Screenshots/";
	}
	else
	{
		path = "~/";
	}
	return path;
}

//===========================================================================
//
// M_GetSavegamesPath												Mac OS X
//
// Returns the path to the default save games directory.
//
//===========================================================================

FString M_GetSavegamesPath()
{
	FString path;
	char cpath[PATH_MAX];
	FSRef folder;

	if (noErr == FSFindFolder(kUserDomain, kDocumentsFolderType, kCreateFolder, &folder) &&
		noErr == FSRefMakePath(&folder, (UInt8*)cpath, PATH_MAX))
	{
		path << cpath << "/" GAME_DIR "/Savegames/";
	}
	return path;
}

#else // Linux, et al.


FString GetUserFile (const char *file)
{
	FString path;
	struct stat info;

	path = NicePath("~/" GAME_DIR "/");

	if (stat (path, &info) == -1)
	{
		struct stat extrainfo;

		// Sanity check for ~/.config
		FString configPath = NicePath("~/.config/");
		if (stat (configPath, &extrainfo) == -1)
		{
			if (mkdir (configPath, S_IRUSR | S_IWUSR | S_IXUSR) == -1)
			{
				I_FatalError ("Failed to create ~/.config directory:\n%s", strerror(errno));
			}
		}
		else if (!S_ISDIR(extrainfo.st_mode))
		{
			I_FatalError ("~/.config must be a directory");
		}

		// This can be removed after a release or two
		// Transfer the old zdoom directory to the new location
		bool moved = false;
		FString oldpath = NicePath("~/." GAMENAMELOWERCASE "/");
		if (stat (oldpath, &extrainfo) != -1)
		{
			if (rename(oldpath, path) == -1)
			{
				I_Error ("Failed to move old " GAMENAMELOWERCASE " directory (%s) to new location (%s).",
					oldpath.GetChars(), path.GetChars());
			}
			else
				moved = true;
		}

		if (!moved && mkdir (path, S_IRUSR | S_IWUSR | S_IXUSR) == -1)
		{
			I_FatalError ("Failed to create %s directory:\n%s",
				path.GetChars(), strerror (errno));
		}
	}
	else
	{
		if (!S_ISDIR(info.st_mode))
		{
			I_FatalError ("%s must be a directory", path.GetChars());
		}
	}
	path += file;
	return path;
}

//===========================================================================
//
// M_GetCachePath														Unix
//
// Returns the path for cache GL nodes.
//
//===========================================================================

FString M_GetCachePath(bool create)
{
	// Don't use GAME_DIR and such so that ZDoom and its child ports can
	// share the node cache.
	FString path = NicePath("~/.config/zdoom/cache");
	if (create)
	{
		CreatePath(path);
	}
	return path;
}

//===========================================================================
//
// M_GetAutoexecPath													Unix
//
// Returns the expected location of autoexec.cfg.
//
//===========================================================================

FString M_GetAutoexecPath()
{
	return GetUserFile("autoexec.cfg");
}

//===========================================================================
//
// M_GetCajunPath														Unix
//
// Returns the location of the Cajun Bot definitions.
//
//===========================================================================

FString M_GetCajunPath(const char *botfilename)
{
	FString path;

	// Check first in ~/.config/zdoom/botfilename.
	path = GetUserFile(botfilename);
	if (!FileExists(path))
	{
		// Then check in SHARE_DIR/botfilename.
		path = SHARE_DIR;
		path << botfilename;
		if (!FileExists(path))
		{
			path = "";
		}
	}
	return path;
}

//===========================================================================
//
// M_GetConfigPath														Unix
//
// Returns the path to the config file. On Windows, this can vary for reading
// vs writing. i.e. If $PROGDIR/zdoom-<user>.ini does not exist, it will try
// to read from $PROGDIR/zdoom.ini, but it will never write to zdoom.ini.
//
//===========================================================================

FString M_GetConfigPath(bool for_reading)
{
	return GetUserFile(GAMENAMELOWERCASE ".ini");
}

//===========================================================================
//
// M_GetScreenshotsPath													Unix
//
// Returns the path to the default screenshots directory.
//
//===========================================================================

FString M_GetScreenshotsPath()
{
	return NicePath("~/" GAME_DIR "/screenshots/");
}

//===========================================================================
//
// M_GetSavegamesPath													Unix
//
// Returns the path to the default save games directory.
//
//===========================================================================

FString M_GetSavegamesPath()
{
	return NicePath("~/" GAME_DIR);
}

#endif
