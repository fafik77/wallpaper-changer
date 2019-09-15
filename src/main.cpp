//#include <cstdio>
#include "fileFinder.h"



void useCmdArgs(int argc, char* argv[], configArgsContent& item)
{
  if(argc>1){
	int starts_i= 1;
		///&1 is /cd, &2 is next
	BYTE status_next= 0;
	while( argc>starts_i )
	{
		std::string arg_key= argv[starts_i];
		std::string arg_val; if(argc> starts_i+1 ) arg_val= argv[starts_i+1];

		if( arg_key== "/cd" ){
			if( !arg_val.empty() ){
				status_next|= 1;
				if( arg_val.length()==1 && arg_val.at(0)== '~' ){
					wchar_t ownPth[MAX_PATH+100];
						/// When NULL is passed to GetModuleHandle, the handle of the exe itself is returned
					HMODULE hModule= GetModuleHandleW(NULL);
					if( hModule!= NULL )
					{	// Use GetModuleFileName() with module handle to get the path
						GetModuleFileNameW(hModule, ownPth, (sizeof(ownPth)));
						std::wstring temp_mePath(ownPth);
						temp_mePath= temp_mePath.substr(0, temp_mePath.find_last_of('\\') );
						_wchdir( temp_mePath.c_str() );
					}
				} else {
					chdir( argv[starts_i+1] ); }
				printf("# arg /cd set\n");
				++starts_i;
			}
		} else if( arg_key== "/show" ){
			item.showOnly= true;
			item.showLogTo= arg_val;
			++starts_i;
		} else if( arg_key== "/next" ){
			status_next|= 2;
		} else if( arg_key== "/?" || arg_key== "/h" || arg_key== "/help" ){
			printf("\
Showing Help\n\
=== Arguments ===\n\
  /cd path\t change path that program starts in. OR '~'for self path\n\
  /show path\t saves file order to file and Exits.\n\
  /next\t changes to the next bg.(needs /cd)\n\
===  # End #  ===\n\
");
			exit(1);
		}
		++starts_i;
	} //while
	if( status_next & 2 ){
		if( !(status_next & 1) ){
			wchar_t ownPth[MAX_PATH+100];
			HMODULE hModule= GetModuleHandleW(NULL);
			if( hModule!= NULL )
			{	// Use GetModuleFileName() with module handle to get the path
				GetModuleFileNameW(hModule, ownPth, (sizeof(ownPth)));
				std::wstring temp_mePath(ownPth);
				temp_mePath= temp_mePath.substr(0, temp_mePath.find_last_of('\\') );
				_wchdir( temp_mePath.c_str() );
			}
		}
		std::ofstream fileNext( "BGchangerNext.cfg" );
		fileNext.flush();
		fileNext.close();
		exit(1);
	}
  }
}


int main(int argc, char* argv[])
//int wmain(int argc, wchar_t* argv[])
{
	imageDirExplorer images;
	wchar_t ownPth[MAX_PATH+100];
		/// When NULL is passed to GetModuleHandle, the handle of the exe itself is returned
	HMODULE hModule= GetModuleHandleW(NULL);
	if( hModule!= NULL )
	{	// Use GetModuleFileName() with module handle to get the path
		GetModuleFileNameW(hModule, ownPth, (sizeof(ownPth)));
		std::wstring temp_mePath(ownPth);
		temp_mePath= temp_mePath.substr(0, temp_mePath.find_last_of('\\') );
		images._OwnPath= temp_mePath+ L"\\";
	}
	useCmdArgs(argc, argv, images.ArgsConfig);

	images._ImageConverter_exe=  L"ConvertImage.exe";
	images._ImageConverter_args= L"90";
	images._ImageExtProblematic.push_back( ".png" );
	images._ImageExtProblematic.push_back( ".tif" );
	images._ImageExtProblematic.push_back( ".tiff" );
	images._ImageExtProblematic.push_back( ".webp" );

	remove("BGchangerNext.cfg");

	images.mainConfig.Open("BGchanger.cfg");
	images.prepStart();

	if(images.ArgsConfig.showOnly){
		images.showImageList();
		return 1;
	}
	images.iterateImages();		//main loop

return 0;
}


