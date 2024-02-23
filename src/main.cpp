/** Copyright (C) 2021 fafik77 ( https://github.com/fafik77 )
	BGchanger.exe is used to change Wallpaper in Windows(min 2003)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a (partial)copy of the GNU General Public License
    along with this program.  If not, see https://www.gnu.org/licenses/gpl-3.0.html or https://www.gnu.org/licenses/.
**/


#if defined(UNICODE) && !defined(_UNICODE)
	#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
	#define UNICODE
#endif

//#include <tchar.h>
#include <windows.h>
#include "fileFinder.h"


	///Version
const char _Program_Version[]= "1.8.5";
	///Version release Date
const char _Program_VersionDate[]= "2024.02.23";
	///github link to sources
const char _Program_downloadSource[]= "https://github.com/fafik77/wallpaper-changer";
	///Google Drive download link
const char _Program_downloadExe[]= "https://drive.google.com/file/d/1-LBV6kRbCCtDYmLDh47I9CNFP_7onoWf/view";


imageDirExplorer* images= nullptr;
	///Declares hander for Windows procedure
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

	///Make the class name into a global variable
const char szClassName[]= "BGchanger_Win32ApiT";
const char my_window_title[]= "BGchanger WindowsApi32";
const UINT kCommandLineArgsMessageId= RegisterWindowMessageW( L"getsAdditionalCmdArgs" );
const UINT IDT_TIMER1= 0xFF1;
	// (cant use without admin) https://docs.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory
const int priv_sign_uuid[4]= {1598018825, 23380942, 48744897, 37702394};
const wchar_t BGchanger_Win_fp_out[]= L".BGchanger_Win.o.tmp";
OSVERSIONINFO osVerInfo;

void useCmdArgs(int argc, LPWSTR *argList, bool restricted=true, bool unrestrictedOnly=false);
	///@return true if /?help requested
bool printHelpScreen(const std::wstring arg_key_isHelp);


int WINAPI WinMain(HINSTANCE hThisInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpszArgument,
					int nCmdShow)
{
	HANDLE hMutex;
	LPWSTR tempCmdArgLine( GetCommandLineW() );

	ZeroMemory(&osVerInfo, sizeof(OSVERSIONINFO));
	osVerInfo.dwOSVersionInfoSize= sizeof(OSVERSIONINFO);
	GetVersionEx(&osVerInfo);



	try{
			// Try to open the mutex.
		hMutex= OpenMutex( MUTEX_ALL_ACCESS, 0, szClassName );
		if( !hMutex ){	//run program
			hMutex= CreateMutex(0, 0, szClassName );
		} else {		//call 1st instance
			HWND main_window_handle= FindWindow( 0, my_window_title );
				//try to make an output file
			std::wstring pathFileCOut_str(512, char(0));
			GetEnvironmentVariableW(L"TEMP", &pathFileCOut_str.front(), pathFileCOut_str.size() );
			if( copyData_sendStruct_main::tryPathOutFile( pathFileCOut_str,BGchanger_Win_fp_out ) ){
				pathFileCOut_str.resize (512, char(0));
				GetEnvironmentVariableW(L"windir", &pathFileCOut_str.front(), pathFileCOut_str.size() );
				if( copyData_sendStruct_main::tryPathOutFile( pathFileCOut_str,L"Temp\\",BGchanger_Win_fp_out ) ){
					pathFileCOut_str.clear();
				}
			}

			copyData_sendStruct_main dataEx(priv_sign_uuid, tempCmdArgLine, &pathFileCOut_str.front() );

			COPYDATASTRUCT dataSSend;
			dataSSend.dwData= kCommandLineArgsMessageId;
			BYTE* buffData= NULL;
			size_t buffOutSize= 0;
			dataEx.getRawData((void**)&buffData, buffOutSize);
			dataSSend.cbData= buffOutSize;
			dataSSend.lpData= buffData;
				// process cmdArgs in 1st instance
			SendMessageW( main_window_handle, WM_COPYDATA, 0, (LPARAM)&dataSSend );
				//show help
			LPWSTR *szArgList= nullptr;
			int argCount= 0;
			bool shownHelp= false;
			szArgList= CommandLineToArgvW( tempCmdArgLine, &argCount );
			if( argCount>1 && printHelpScreen( szArgList[1] ) ){
				shownHelp= true;
				printf("help shown, rest of arguments passed to 1st instance");
			}
				//reclaim some space
			LocalFree(szArgList);

			if(!shownHelp && pathFileCOut_str.size()){
				Sleep(1); //give windows time to reindex the file
				BYTE tries= 0;
				for( ; tries<15; ++tries ){
					if( exists_Wfile( (pathFileCOut_str+ L".ulock").c_str() ) )
						break;
					Sleep(100);
				}
				if(tries>= 15){
					DeleteFileW( pathFileCOut_str.c_str() );
					DeleteFileW( (pathFileCOut_str+ L".ulock").c_str() );
				}

				HANDLE hOutFile= CreateFileW(pathFileCOut_str.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, (FILE_ATTRIBUTE_NORMAL|FILE_FLAG_DELETE_ON_CLOSE| FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN ), NULL ); //FILE_SHARE_READ
				if(hOutFile){
					char wBuff[514];
					DWORD readAmount= 0;
					do{
						ReadFile(hOutFile, (void*)wBuff, (sizeof(wBuff)-2)*sizeof(wBuff[0]), &readAmount, NULL );
						if(readAmount){
							wBuff[ readAmount ]= 0;
							wBuff[ readAmount+1 ]= 0;
							wprintf(L"%s", (wchar_t*)wBuff);
						}
					}while(readAmount);
					CloseHandle(hOutFile);
				}
			}
			if( pathFileCOut_str.size() ){	//cleanup
				DeleteFileW( pathFileCOut_str.c_str() );
				DeleteFileW( (pathFileCOut_str+ L".ulock").c_str() );
			}
			return 0x1;
		}
	} catch( ... ) {}


	LPWSTR *szArgList= nullptr;
	int argCount= 0;
	szArgList= CommandLineToArgvW( tempCmdArgLine, &argCount );


	images= new imageDirExplorer();
	wchar_t ownPth[MAX_PATH+100];
		/// When NULL is passed to GetModuleHandle, the handle of the exe itself is returned
	HMODULE hModule= GetModuleHandleW(NULL);
	if( hModule!= NULL )
	{	// Use GetModuleFileName() with module handle to get the path
		GetModuleFileNameW(hModule, ownPth, (sizeof(ownPth)));
		std::wstring temp_mePath(ownPth);
		temp_mePath= temp_mePath.substr(0, temp_mePath.find_last_of('\\') );
		images->_OwnPath= temp_mePath+ L"\\";
	}
	useCmdArgs( argCount, szArgList, false, true );

	images->_ImageConverter_exe=  L"ConvertImageG.exe";
	images->_ImageConverter_args= L"90";
	vectorString& fastPush= images->mainConfig.cfg_content._ImageExtProblematic;
	if( fastPush.empty() ){
		fastPush.push_back( ".png" );
		fastPush.push_back( ".tif" );
		fastPush.push_back( ".tiff" );
		fastPush.push_back( ".webp" );
	}
	images->mainConfig.Open("BGchanger.cfg");
	images->ArgsConfig.forcedImageChoosing= images->mainConfig.cfg_content.forcedImageChoosing;
	if( images->ArgsConfig.showLogTo.empty() ){	//set LogFile
		images->ArgsConfig.showLogTo= images->mainConfig.cfg_content.logFile;
		remove( images->ArgsConfig.showLogTo.c_str() );
		images->writeTime(images->ArgsConfig.showLogTo);	//write date time
	}

	images->prepStart();
	images->imageChange();


	useCmdArgs( argCount, szArgList, false );
	LocalFree(szArgList);

		///This is the handle for our window
	HWND hwnd;
		///Here messages to the application are saved
	MSG messages;
		///Data structure for the windowclass
//	WNDCLASSEXW wincl;
	WNDCLASSEX wincl;

		//The Window structure
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;      // This function is called by windows
	wincl.style = CS_DBLCLKS;                 // Catch double-clicks
	wincl.cbSize = sizeof (WNDCLASSEXW);

		//Use default icon and mouse-pointer
	wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;                 //No menu
	wincl.cbClsExtra = 0;                      //No extra bytes after the window class
	wincl.cbWndExtra = 0;                      //structure or the window instance
		//Use Windows's default colour as the background of the window
	wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

		//Register the window class, and if it fails quit the program
	if( !RegisterClassEx(&wincl) ){ return 0x10; }

		//The class is registered, let's create the program
	hwnd= CreateWindowEx(
	   0,                   //Extended possibilites for variation
	   szClassName,         //Classname
	   my_window_title,       //Title Text
	   WS_OVERLAPPEDWINDOW, //default window
	   CW_USEDEFAULT,       //Windows decides the position
	   CW_USEDEFAULT,       //where the window ends up on the screen
	   400,                  //The programs width
	   200,                	//and height in pixels
	   NULL,       			//The window is a child-window to desktop
	   NULL,                //No menu
	   hThisInstance,       //Program Instance handler
	   NULL                 //No Window Creation data
	);

	//Make the window visible on the screen, for Debugging Purposes only
	//ShowWindow(hwnd, nCmdShow);

	SetTimer(hwnd,             // handle to main window
	IDT_TIMER1,            // timer identifier
	2000,                 // 2-second interval
	(TIMERPROC) NULL);     // no timer callback

		//Run the message loop
	while( GetMessage (&messages, NULL, 0, 0) )
	{
			//Translate virtual-key messages into character messages
		TranslateMessage(&messages);
			//Send message to WindowProcedure
		DispatchMessage(&messages);
	}

		//The program return-value is 0 - The value that PostQuitMessage() gave
	KillTimer(hwnd, IDT_TIMER1);
	delete images;
	DestroyWindow( hwnd );
	return messages.wParam;
}


void handleCopyData( HWND mainWindHWND, LPARAM lParam)
{
	COPYDATASTRUCT* data = {0};
	data= (COPYDATASTRUCT*) lParam;

	if(data->dwData == kCommandLineArgsMessageId )
	{
		copyData_sendStruct_main uncodeData;
		if( uncodeData.setFromRawData(data->lpData, data->cbData) ){
			return;
		}
		if( uncodeData.sign_UUID== priv_sign_uuid ){
			LPWSTR *szArgList= nullptr;
			int argCount= 0;
			szArgList= CommandLineToArgvW( uncodeData.CmdArgLine, &argCount );
				//make a wFile under the given path n name
			images->ArgsConfig.hOutPipedToFile= CreateFileW(uncodeData.pathFileCOut, GENERIC_READ|GENERIC_WRITE, (FILE_SHARE_READ|FILE_SHARE_DELETE), NULL, CREATE_ALWAYS, (FILE_ATTRIBUTE_NORMAL), NULL ); // FILE_SHARE_READ|FILE_SHARE_WRITE, |FILE_FLAG_DELETE_ON_CLOSE
			useCmdArgs( argCount, szArgList);
			LocalFree(szArgList);
			if(images->ArgsConfig.hOutPipedToFile){
				FlushFileBuffers(images->ArgsConfig.hOutPipedToFile);
				CloseHandle(images->ArgsConfig.hOutPipedToFile);
				images->ArgsConfig.hOutPipedToFile= NULL;

				std::wstring tempULock_fileWStr(uncodeData.pathFileCOut);
				tempULock_fileWStr+= L".ulock";
				HANDLE tempULock_file= CreateFileW( tempULock_fileWStr.c_str() , GENERIC_WRITE, (FILE_SHARE_VALID_FLAGS), NULL, CREATE_ALWAYS, (FILE_ATTRIBUTE_NORMAL), NULL );
				CloseHandle(tempULock_file);
			}
		}
	}
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COPYDATA:
	{
		handleCopyData( hwnd, lParam );
		break;
	}
	case WM_TIMER:
	{
		if(wParam == IDT_TIMER1){
			images->WaitUntilTime();
		}
		return 0;
	}
	case WM_DESTROY:
			//send a WM_QUIT to the message queue
		PostQuitMessage(0);
		break;
	default:
			//for messages that we don't deal with
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
return 0;
}

void useCmdArgs(int argc, LPWSTR *argList, bool restricted, bool unrestrictedOnly)
{
	configArgsContent& item= images->ArgsConfig;
	int starts_i= 1;
	std::wstring arg_key;
	std::wstring arg_val;

	while( argc> starts_i )
	{
		arg_key= argList[starts_i];
		if(argc> (starts_i+1) ) arg_val= argList[starts_i+1];
		else arg_val.clear();

		if( !restricted && arg_key== L"/cd" ){
			if( !arg_val.empty() ){
				if( arg_val.length()==1 && arg_val.at(0)== '~' ){
					_wchdir( images->_OwnPath.c_str() );
				} else
					_wchdir( arg_val.c_str() );
				images->cwd_update();
				printf("# arg /cd set\n");
				++starts_i;
			}
		} else if( arg_key== L"/log" ){
			if( restricted ){
				item.showThisLogTo= std::string( arg_val.begin(), arg_val.end() );
				remove( item.showThisLogTo.c_str() );
				images->writeTime(item.showThisLogTo);
			} else {	//(!restricted)
				item.showLogTo= std::string( arg_val.begin(), arg_val.end() );
				remove( item.showLogTo.c_str() );
				images->writeTime(item.showLogTo);
			}
			++starts_i;
		} else if( !restricted && arg_key== L"/wpstyle" && !arg_val.empty() ){
			SetWStringRegKeyValue( HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"WallpaperStyle", arg_val );
		} else if( !restricted && arg_key== L"/wptile"  && !arg_val.empty() ){
			SetWStringRegKeyValue( HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"TileWallpaper", arg_val );
		} else if( !restricted && printHelpScreen(arg_key) ){	//help
			delete images;
			exit(2);
		} else if( unrestrictedOnly ){	//if above args are not met && only early startup args are to be done, break
			++starts_i;
			continue;
		} else if( arg_key== L"/pwd" ){
			images->log_pwd();
		} else if( arg_key== L"/show" ){
			images->showImageList();
		} else if( arg_key== L"/showall" ){
			images->showFullImageList();
		} else if( arg_key== L"/rescan" ){
			images->rescan();
		} else if( arg_key== L"/next" ){
			images->imageChange();
			images->whatWPDisplayed();
		} else if( arg_key== L"/when" ){
			images->whenWPChange();
		} else if( arg_key== L"/what" ){
			images->whatWPDisplayed();
		} else if( arg_key== L"/wpshow" ){
			ShellExecuteW(NULL, NULL, images->getCurrImage().c_str(), NULL, NULL, SW_SHOWNORMAL);
		} else if( arg_key== L"/wpexplore" || arg_key== L"/wpexplorer" ){
			std::wstring temp_pathString= L"/select, \""+ replaceAll(images->getCurrImage(), L"/", L"\\") +L"\"";
			ShellExecuteW(NULL, L"open", L"explorer", temp_pathString.c_str(), NULL, SW_SHOWNORMAL);
		} else if( arg_key== L"/wpprev" || arg_key== L"/wprev" ){
			ShellExecuteW(NULL, NULL, images->getPrevImage().c_str(), NULL, NULL, SW_SHOWNORMAL);
		} else if( arg_key== L"/reshow" ){
			images->reshowWP();
		} else if( arg_key== L"/redraw" ){
			images->refreshDesktop();
		} else if( arg_key== L"/fput" ){
			item.forcedImageChoosing= true;
		} else if( arg_key== L"/nput" ){
			item.forcedImageChoosing= false;
		} else if( arg_key== L"/exit" ){
			images->printExitMsg();
			PostQuitMessage(1);
			return;
		} else if( starts_i==1 ){	//not a switch Assuming ImageFileName
			images->imageChangeTo( arg_key );
		}
		++starts_i;
	} //while
	images->free_singleLog();
}

bool printHelpScreen(const std::wstring arg_key_isHelp)
{
	if(arg_key_isHelp== L"/?" || arg_key_isHelp== L"/h" || arg_key_isHelp== L"/help"){
		printf("    Copyright (C) 2021 fafik77 ( https://github.com/fafik77 )\n\
    This program comes with ABSOLUTELY NO WARRANTY; for details read 'License (BGchanger).txt'\n\
    This is free software, and you are welcome to redistribute it under GNU 3 License\n\n");
		printf("  Version %s\ton %s\n", _Program_Version, _Program_VersionDate);
		printf("  Link source %s\n", _Program_downloadSource);
		printf("  Link download %s\n", _Program_downloadExe);
		printf("\
Showing Help\n\
Argument order does mater, and catches all from begin to end\n\
=== Arguments ===\n\
  /cd <path>\t change path that program starts in. OR '~'for self path.\n\
  /log <path>\t logs the output to file. can be used /log filename.log /show\n\
  /pwd \t prints current working dir (cd).\n\
  /rescan\t rescans image Folder\n\
  /show\t shows image order.\n\
  /showall\t shows complete image order.\n\
  /next\t changes to the next bg.\n\
  /when\t shows when next wallpaper will change\n\
  /what\t shows the name of current wallpaper image.\n\
  /wpshow\t lunches current wallpaper in image explorer.\n\
  /wpexplore\t selects current wallpaper in explorer.\n\
  /wpstyle <number>\t set WallpaperStyle (def 6). use /redraw to apply\n\
  /wptile  <number>\t set TileWallpaper (def 0).  use /redraw to apply\n\
  /wpprev\t lunches previous wallpaper in image explorer.\n\
  /reshow\t shows again current wallpaper.\n\
  /redraw\t refreshes desktop wallpaper.\n\
  /exit\t exits (other timer calls this with support for more formats)\n\
  /fput\t when providing an ImageFileName Argument[1]. Always rescans to find it\n\
  /nput\t ~reversed fput. do NOT rescan to find ImageFileName Argument[1]\n\
  [1]\t if not a switch ImageFileName is assumed to show now\n\
===  # End #  ===\n\
");
		return true;
	}
return false;
}





