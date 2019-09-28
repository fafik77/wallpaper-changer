#if defined(UNICODE) && !defined(_UNICODE)
	#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
	#define UNICODE
#endif

//#include <tchar.h>
#include "fileFinder.h"
#include <windows.h>


imageDirExplorer* images= nullptr;

	///Declares hander for Windows procedure
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

	///Make the class name into a global variable
const char szClassName[]= "BGchanger_Win32ApiT";
const char my_window_title[]= "BGchanger WindowsApi32";
const UINT kCommandLineArgsMessageId= RegisterWindowMessageW( L"getsAdditionalCmdArgs" );
const UINT IDT_TIMER1= 0xFF1;

void useCmdArgs(int argc, LPWSTR *argList, bool restricted=true, bool unrestrictedOnly=false);



int WINAPI WinMain(HINSTANCE hThisInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpszArgument,
					int nCmdShow)
{
	HANDLE hMutex;
	LPWSTR tempCmdArgLine( GetCommandLineW() );

	try{
			// Try to open the mutex.
		hMutex= OpenMutex( MUTEX_ALL_ACCESS, 0, szClassName );
		if( !hMutex ){	//run program
			hMutex= CreateMutex(0, 0, szClassName );
		} else {		//call 1st instance
			HWND main_window_handle= FindWindow( 0, my_window_title );

			COPYDATASTRUCT dataSSend;
			dataSSend.dwData= kCommandLineArgsMessageId;
			dataSSend.cbData= (wcslen(tempCmdArgLine) * sizeof(wchar_t) ) +1;
			dataSSend.lpData= tempCmdArgLine;

			SendMessageW( main_window_handle, WM_COPYDATA, 0, (LPARAM)&dataSSend );
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

	images->_ImageConverter_exe=  L"ConvertImage.exe";
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
		LPWSTR *szArgList= nullptr;
		int argCount= 0;
		szArgList= CommandLineToArgvW( (LPCWSTR)data->lpData, &argCount );
		useCmdArgs( argCount, szArgList);
		LocalFree(szArgList);
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
	while( argc> starts_i )
	{
		std::wstring arg_key= argList[starts_i];
		std::wstring arg_val; if(argc> (starts_i+1) ) arg_val= argList[starts_i+1];

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
		} else if( !restricted && arg_key== L"/log" ){
			item.showLogTo= std::string( arg_val.begin(), arg_val.end() );
			remove( item.showLogTo.c_str() );
			++starts_i;
		} else if( !restricted && (arg_key== L"/?" || arg_key== L"/h" || arg_key== L"/help") ){
			printf("\
  Version 1.6+\ton 2019.09.28\n\
Showing Help\n\
Argument order does mater, and catches all from begin to end\n\
=== Arguments ===\n\
  /cd <path>\t change path that program starts in. OR '~'for self path.\n\
  /log <pathFileName>\t logs the output to file.\n\
  /rescan\t rescans image Folder\n\
  /show\t shows image order.\n\
  /showall\t shows complete image order.\n\
  /next\t changes to the next bg.\n\
  /exit\t exits (other timer calls this with support for more formats)\n\
  /fput\t when providing an ImageFileName Argument[1]. Always rescans to find it\n\
  /nput\t ~reversed fput. do NOT rescan to find ImageFileName Argument[1]\n\
  [1]\t if not a switch ImageFileName is assumed to show now\n\
===  # End #  ===\n\
");
			delete images;
			exit(2);
		} else if( unrestrictedOnly ){	//if above args are not met && only early startup args are to be done, break
			++starts_i;
			continue;
		} else if( arg_key== L"/show" ){
			images->showImageList();
		} else if( arg_key== L"/showall" ){
			images->showFullImageList();
		} else if( arg_key== L"/rescan" ){
			images->rescan();
		} else if( arg_key== L"/next" ){
			images->imageChange();
		} else if( arg_key== L"/fput" ){
			item.forcedImageChoosing= true;
		} else if( arg_key== L"/nput" ){
			item.forcedImageChoosing= false;
		} else if( arg_key== L"/exit" ){
			PostQuitMessage(1);
			return;
		} else if( starts_i==1 ){	//not a switch Assuming ImageFileName
			images->imageChangeTo( arg_key );
		}
		++starts_i;
	} //while
}






