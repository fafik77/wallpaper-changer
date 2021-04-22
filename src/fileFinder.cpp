#include "fileFinder.h"

std::random_device dev;
std::mt19937 rng( time(0) );	//thread_local std::mt19937 gen{std::random_device{}()};
template<typename T>
T random(T min, T max) {
	return std::uniform_int_distribution<T>{min, max}(rng);
}


configFileContent* publifiedConfig= nullptr;

void vector_DFp::del_front(size_t amount)
{
	_pos_begin+= amount;
}
void vector_DFp::del_back()
{
	pop_back();
}
void vector_DFp::delEl( size_t pos )
{
	if( pos == 0 ) return del_front();
	if( pos == size()-1 ) return del_back();
	if( (pos+ _pos_begin)< size() )
		erase( begin()+(pos+ _pos_begin) );
}
vector_DFp::iterator vector_DFp::at_pos( size_t pos )
{
	if( (pos+ _pos_begin)< size() )
		return begin()+( pos+ _pos_begin );
	return begin();
}
size_t vector_DFp::get_size() const
{
	return size()-_pos_begin;
}

DirFileEnt::DirFileEnt(	const std::wstring& name_, const std::wstring& path_ ): name(name_), path(path_)
{
//	if( name_.find_first_of( L"0123456789" )!= name_.npos ) is_number= true;
}
std::wstring DirFileEnt::getName(bool afterNumber)const
{
	return name;
}
std::wstring DirFileEnt::getPathName(bool afterNumber)const
{
	return path+ (path.empty()? L"":L"\\")+ getName(afterNumber);
}


bool isDigits( wchar_t char_1 )
{
	if( char_1 >= L'0' && char_1 <= L'9' ) return true;
	return false;
}
size_t getDigitsAsNumber( const std::wstring& string1, const std::wstring& string2, long long* out_LL )
{
  if(out_LL){
	std::wistringstream sis1( string1 );
	std::wistringstream sis2( string2 );
	sis1>> out_LL[0];
	sis2>> out_LL[1];
	return sis1.tellg();
  }
return -1;
}

bool compare_paths_bool(const DirFileEnt* DE_a, const DirFileEnt* DE_b )
{
	std::wstring str_a, str_b;

	if(publifiedConfig->DirSortByFileName){
		str_a= DE_a->getName(0);
		str_b= DE_b->getName(0);
	} else {
		str_a= DE_a->getPathName(0);
		str_b= DE_b->getPathName(0);
	}

	size_t PosCurr= 0;
	size_t maxLen= str_a.length();
	if( str_b.length()< maxLen ) maxLen= str_b.length();

	while( PosCurr< maxLen )
	{
		wchar_t c_a= std::toupper(str_a.at(PosCurr));
		wchar_t c_b= std::toupper(str_b.at(PosCurr));

		if( publifiedConfig->sortDigitsAsNumbers ){
		  BYTE isDigits_for= isDigits( c_a ) | (2*isDigits( c_b ));
		  if( isDigits_for ){
			if( isDigits_for == 3 ){
				long long out_LL[2]= {0};
				size_t moved= getDigitsAsNumber( str_a.substr(PosCurr), str_b.substr(PosCurr), out_LL );
				bool change= out_LL[0] < out_LL[1];
				if( out_LL[0] != out_LL[1] ) return change;	//return number if not same

				PosCurr+= moved;			//if same, skip number part
				c_a= std::toupper(str_a.at(PosCurr));
				c_b= std::toupper(str_b.at(PosCurr));
			} else if( isDigits_for == 1 ) {
				return 1;
			} else { return false; }
		  }
		}

		int reslt= c_a- c_b;
		if( reslt ){
			if( c_a== '\\' ) return true;	//2 different strings, 1 ends at \\ (means its shorter, shorter=first)
			if( c_b== '\\' ) return false;
			return c_a< c_b;	//a!b, return a< b
		}
		//a==b equal, repeat
		++PosCurr;
	}
	//return ( a < b )
return 0;
}

bool stringEnds(const std::string& stringIn, const std::string& ends, bool CaseInsensit, std::string* rest )
{
	if( !ends.length() ) return true;
	std::string ends_w(ends);
	size_t findLen= ends_w.length();

	if( stringIn.length()< findLen ) return false;

	std::string lastPart= stringIn.substr( stringIn.length()-findLen, findLen );
	if(CaseInsensit){
		std::transform(lastPart.begin(), lastPart.end(), lastPart.begin(), ::toupper);
		std::transform(ends_w.begin(), ends_w.end(), ends_w.begin(), ::toupper);
	}
	if(rest){
		(*rest)= stringIn.substr( 0, stringIn.length()-findLen );
	}
	return ( lastPart == ends_w );
}
bool stringEnds(const std::string& stringIn, const vectorString& ends, bool CaseInsensit, std::string* rest )
{
	for( size_t temp_u=0; temp_u<ends.size(); ++temp_u ){
		if( stringEnds(stringIn, ends.at(temp_u), CaseInsensit, rest) ) return true;
	}
	return false;
}
bool stringEnds(const std::wstring& stringIn, const std::string& ends, bool CaseInsensit, std::wstring* rest )
{
		if( !ends.length() ) return true;
	std::wstring ends_w( ends.begin(), ends.end() );
	size_t findLen= ends_w.length();

	if( stringIn.length()< findLen ) return false;

	std::wstring lastPart= stringIn.substr( stringIn.length()-findLen, findLen );
	if(CaseInsensit){
		std::transform(lastPart.begin(), lastPart.end(), lastPart.begin(), ::toupper);
		std::transform(ends_w.begin(), ends_w.end(), ends_w.begin(), ::toupper);
	}
	if(rest){
		(*rest)= stringIn.substr( 0, stringIn.length()-findLen );
	}
	return ( lastPart == ends_w );
}
bool stringEnds(const std::wstring& stringIn, const vectorString& ends, bool CaseInsensit, std::wstring* rest )
{
	for( size_t temp_u=0; temp_u<ends.size(); ++temp_u ){
		if( stringEnds(stringIn, ends.at(temp_u), CaseInsensit, rest) ) return true;
	}
	return false;
}

imageDirExplorer::fileHKeeper_item* imageDirExplorer::fileHKeeper_list::getOrAdd(const std::string& name, std::string modeOpenOveride)
{
	for( auto atIt : tableItems){
		if( atIt->name== name ){
			return atIt;
		}
	}
	//not found make new
	if( modeOpenOveride.size()<3 ) modeOpenOveride+= ",ccs=UTF-8";
	FILE* out_file= fopen( name.c_str(), modeOpenOveride.c_str() );
	fileHKeeper_item* tempNew= new fileHKeeper_item(name, out_file);
	tableItems.push_back( tempNew );
	return tempNew;
}
bool imageDirExplorer::fileHKeeper_list::deleteEntry( const std::string& name, bool flushFile )
{
	for( auto atIt= tableItems.begin(); atIt!= tableItems.end(); ++atIt ){
		if( (*atIt)->name== name ){
			if(flushFile){
				(*atIt)->flush_file();
			}
			delete (*atIt);
			tableItems.erase( atIt );
			return true;
		}
	}
return false;
}
void imageDirExplorer::fileHKeeper_list::clear()
{
	while( tableItems.size() ){
		delete tableItems.back();
		tableItems.erase( tableItems.end()-1 );
	}
}
void imageDirExplorer::fileHKeeper_list::deleteOldItems()
{
	size_t atItt= tableItems.size();
	for( ; atItt!= 0; --atItt ){
		std::vector<fileHKeeper_item*>::iterator atIt= tableItems.begin()+(atItt-1);
		if( (*atIt)->time++ > 1 ){
			delete (*atIt);
			tableItems.erase(atIt);
		}
	}
}
int imageDirExplorer::fileHKeeper_list::removeFile( const std::string& name )
{
	deleteEntry(name);
	return remove( name.c_str() );
}

bool imageDirExplorer::imageChangeTo(std::wstring imgName)
{
	std::wstring restPath;
	std::wstring pathBegins(cwd_my);
	std::wstring pathGoesBack(mainConfig.cfg_content.imageFolder.begin(), mainConfig.cfg_content.imageFolder.end());
	while( stringBegins(pathGoesBack, L"..", false) ){
		pathGoesBack.erase(0,3);
		pathBegins= pathBegins.substr( 0, pathBegins.find_last_of( L'\\' ) );
	}
	pathBegins+= pathBegins.back()==L'\\'? L"" : L"\\";
	pathBegins+= pathGoesBack;
	pathBegins+= pathBegins.back()==L'\\'? L"" : L"\\";

	bool didSetSucc= false;
	if( stringBegins( imgName, pathBegins, true, &restPath ) ){
		size_t valEl= ImgInList_find( restPath );
		if( valEl!= imgName.npos ){
			didSetSucc= true;
			if(image_p) SavedList_add();	//to not show again

			image_i= valEl;
			image_p= *DF_list_p.at_pos(valEl);
			if(!mainConfig.cfg_content.random){
				DF_list_p.del_front(valEl);
			}
			image_1Shown= 1;	//enforce imageChange() to show currently selected element
			imageChange();
			DF_list_p.delEl(valEl);
			if(mainConfig.cfg_content.saveLastImages) writeUtfLine( image_p->getPathName(), "BGchanger_selected.cfg", "w", true );
		}
	}
	if( !didSetSucc && ArgsConfig.forcedImageChoosing && exists_Wfile(imgName.c_str() ) ){
		if( !stringEnds( imgName, mainConfig.cfg_content.imageExt, true ) ){
			std::wstring msg_extNoMatch= L"! Error: requested \"" + imgName + L"\" .Ext doesnt match with config.imageExt\n";
			writeToMultiple(msg_extNoMatch);
			return didSetSucc;
		}
		didSetSucc= true;
		DirFileEnt tempFEntry;
		if( imgName.find(':') )
			tempFEntry.name= imgName;
		else{
			tempFEntry.path= cwd_my+ L"\\";
			size_t posNameBeg= imgName.find_last_of( L'\\' );
			if(posNameBeg!= imgName.npos) imgName= imgName.substr( posNameBeg );
			tempFEntry.name= imgName;
		}
		image_1Shown= 1;	//enforce imageChange() to show currently selected element
		imageChange( &tempFEntry );
	}
return didSetSucc;
}

BYTE imageDirExplorer::getImagesFromDir(const std::wstring& addPath, vectorDF_entry& out_vector)
{
	std::wstring pattern( mainConfig.cfg_content.imageFolder.begin(), mainConfig.cfg_content.imageFolder.end() );
	if( pattern.empty() ) pattern= L"./";
	bool doLoop= true;

	while( doLoop ){
		switch( pattern.back() )
		{
			case char(0):
			case '\r':
			case '\n':
			case '\\':
			case '/' : {
				pattern.pop_back();
				break;
			}
			default: {
				doLoop= false;
				break;
			}
		}
	}
	if( !addPath.empty() ){
		pattern.append(L"\\");
		pattern.append(addPath);
	}
	pattern.append(L"\\*");
	WIN32_FIND_DATAW data;
	HANDLE hFind;
	BYTE omit2= 0;

	if( (hFind= FindFirstFileW(pattern.c_str(), &data))!= INVALID_HANDLE_VALUE ){
		do{
			std::wstring curr_Name( data.cFileName );
			if( omit2< 2 ){ ++omit2; continue; } //omit  . or ..

			if( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){
				if( mainConfig.cfg_content.skipHiddenFolders && data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ){
				} else {
					if( !stringBegins(curr_Name, std::wstring(mainConfig.cfg_content.skipFoldersBeginning.begin(), mainConfig.cfg_content.skipFoldersBeginning.end()), true)
						&& !stringEnds(curr_Name, mainConfig.cfg_content.skipFoldersEnding, true ) )
						getImagesFromDir( (addPath.empty()? L"" : addPath+L"\\") +curr_Name, out_vector );
				}
			} else {
				if( stringEnds(curr_Name, mainConfig.cfg_content.imageExt, true) ){
					out_vector.push_back( DirFileEnt(curr_Name, addPath) );
				}
			}
		}while (FindNextFileW(hFind, &data) != 0);
		FindClose(hFind);
	} else {
		return 1;
	}
return 0;
}

void imageDirExplorer::sortBy()
{
	publifiedConfig= &mainConfig.cfg_content;
		///random && saveLastImages
	bool randSave( mainConfig.cfg_content.random && mainConfig.cfg_content.saveLastImages );

	if( mainConfig.cfg_content.sort || randSave ){
		if( randSave ){
			publifiedConfig->sortDigitsAsNumbers= 1;
			publifiedConfig->DirSortByFileName= 0;
		}
		std::sort( DF_list_p.begin(), DF_list_p.end(), compare_paths_bool );
	}
}
void imageDirExplorer::showImageList()
{
	if( mainConfig.cfg_content.random ){
		std::wstringstream tempwstr;
		tempwstr<< "`config.random=1` is set to= true" << std::endl;
		writeToMultiple( tempwstr.str() );
	}
	writeToMultiple(L"images order= [\n",writeTo_console);
	writeToMultiple(L"\nimage order= in list[", writeTo_files);
	for( size_t temp_u=0; temp_u<DF_list_p.get_size(); ++temp_u ){
		std::wstring tempWrite= L"  "+ (*DF_list_p.at_pos(temp_u))->getPathName(false);
		writeToMultiple(tempWrite);
		writeToMultiple(L"\n",writeTo_console);
	}
	writeToMultiple(L"] end. image order\n");
	if( ArgsConfig.showLogTo.size() ){
		list_writeUtfLine.getOrAdd( ArgsConfig.showLogTo, "a+" )->flush_file();
	}
	if( ArgsConfig.showThisLogTo.size() ){
		list_writeUtfLine.getOrAdd( ArgsConfig.showThisLogTo, "a+" )->flush_file();
	}
}
void imageDirExplorer::showFullImageList()
{
	vector_DFp DF_list_tempP;
	DF_list_tempP.reserve( DF_list.size()+1 );
	for( auto on_it= DF_list.begin(); on_it!= DF_list.end(); ++on_it ){
		DF_list_tempP.push_back( &*on_it );
	}

	if( mainConfig.cfg_content.random ){
		std::wstringstream tempwstr;
		tempwstr<< "`config.random=1` is set to= true" << std::endl;
		writeToMultiple( tempwstr.str() );
	}
	writeToMultiple(L"images order= [\n",writeTo_console);
	writeToMultiple(L"\nimage order= in list[", writeTo_files);
	for( size_t temp_u=0; temp_u<DF_list_tempP.get_size(); ++temp_u ){
		std::wstring tempWrite= L"  "+ (*DF_list_p.at_pos(temp_u))->getPathName(false);
		writeToMultiple(tempWrite);
		writeToMultiple(L"\n",writeTo_console);
	}
writeToMultiple(L"] end. image order\n");
	if( ArgsConfig.showLogTo.size() ){
		list_writeUtfLine.getOrAdd( ArgsConfig.showLogTo, "a+" )->flush_file();
	}
	if( ArgsConfig.showThisLogTo.size() ){
		list_writeUtfLine.getOrAdd( ArgsConfig.showThisLogTo, "a+" )->flush_file();
	}
}
BYTE imageDirExplorer::WaitUntilTime()
{
	list_writeUtfLine.deleteOldItems();
		///get time now
	std::time_t ttime_now= std::time(0);

	if(!ttime_pulse){
		std::time_t ttime_every(mainConfig.cfg_content.time);
		ttime_pulse= ttime_now+ ttime_every;

		if(mainConfig.cfg_content.useSystemTime){
			ttime_pulse= ttime_now;
			timeToTime time_every( ttime_every );
			std::tm* time_point= std::localtime( &ttime_now );

			if(time_every.hour) ttime_pulse-= (time_point->tm_hour % time_every.hour)* 3600;
			if(time_every.min)  ttime_pulse-= (time_point->tm_min % time_every.min)* 60;
			else ttime_pulse-= time_point->tm_min* 60;

			ttime_pulse-= time_point->tm_sec;
			ttime_pulse+= ttime_every;
		}
		std::tm* time_now= std::localtime( &ttime_pulse );
		printf("next change: %ih:", time_now->tm_hour );
		printf("%im .", time_now->tm_min );
		printf("%i\n", time_now->tm_sec );
	} else if( ttime_now>= ttime_pulse ){
		if(mainConfig.cfg_content.readjustTimeAfterSleep){
			if( ttime_now> (ttime_pulse+ time_t(100) ) ){
				ttime_pulse= 0;
				printf("readjusting Time...\n");
				return WaitUntilTime();
			}
		}
		ttime_pulse= 0;
		imageChange();
	}
return 0;
}
void imageDirExplorer::whenWPChange()
{
	struct std::tm tstruct;
	char buf[80]= {0};
	tstruct= *std::localtime( &ttime_pulse );
		// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
		// for more information about date/time format
	strftime(buf, sizeof(buf), "%X", &tstruct);

	const std::string tempTimeStr( buf );
	std::wstring tempStr( tempTimeStr.begin(), tempTimeStr.end() );

	tempStr= L"next wallpaper change " + tempStr + L"\n";
	return writeToMultiple( tempStr );
}
void imageDirExplorer::whatWPDisplayed()
{
	std::wstring tempStr( L" WP: " );
	if(image_p)
		tempStr+= image_p->getPathName();
	else
		tempStr+= L"Error img not found";
	tempStr+= L"\n";

	if(_PrevImage_stringPath!= _CurrImage_stringPath){	//if custom image displayed
		tempStr+= L"override: "+ _CurrImage_stringPath+ L"\n";
	}

	return writeToMultiple( tempStr );
}
void imageDirExplorer::printExitMsg()
{
	std::wstring tempStr( L"Exiting..\n" );

	return writeToMultiple( tempStr );
}
void imageDirExplorer::free_singleLog()
{
	if( ArgsConfig.showThisLogTo.size() ){
		list_writeUtfLine.closeFile( ArgsConfig.showThisLogTo, true );
		ArgsConfig.showThisLogTo.clear();
	}
}
void imageDirExplorer::imageChange(DirFileEnt* overide)
{
	if(image_p && image_1Shown>1 && !overide){	//old image
		image_p->selected= 2;
		SavedList_add();
	}
	if( DF_list_p.get_size()==0 ){
		image_i= 0;
		if( mainConfig.cfg_content.saveLastImages )
			SavedList_remove();
		prepStart();	//loop end, rescan for changes
	}

	if( !image_1Shown ){
		image_1Shown= true;
		if( SavedList_getCurrentWP( "BGchanger_selected.cfg" ) ){
			//find the image and delete it from list..
			//..again show this image
		} else {
			image_1Shown= 2;
		}
	}

	if( image_1Shown!= 1 && !overide){	//refresh currently selected image
		if( mainConfig.cfg_content.random ){	//new random image
			image_i= random( size_t(0), DF_list_p.get_size()-1 );
			image_p= *(DF_list_p.at_pos( image_i ));
			image_p->selected= 1;
			if( mainConfig.cfg_content.random != 2 )
				DF_list_p.delEl( image_i );
		} else {								//new next image
			image_p= *(DF_list_p.at_pos( image_i=0 ));
			image_p->selected= 1;
			DF_list_p.delEl( image_i );
		}
		if(mainConfig.cfg_content.saveLastImages) writeUtfLine( image_p->getPathName(), "BGchanger_selected.cfg", "w", true );
	} else if( image_1Shown ) image_1Shown= 2;

		///TranscodedWallpaper.jpg
		///std::wstring str_WinPaperPath(L"%userprofile%\\AppData\\Roaming\\Microsoft\\Windows\\Themes\\");
	std::wstring str_imgName(mainConfig.cfg_content.imageFolder.begin(), mainConfig.cfg_content.imageFolder.end());
	if(str_imgName.back()!= L'\\') str_imgName+= L"\\";
	if(overide)	str_imgName= overide->getPathName(false);
	else 		str_imgName+= image_p->getPathName(false);
	wprintf( L" %s\n", str_imgName.c_str() );


	if( exists_Wfile( str_imgName.c_str() ) ){
		std::wstring str_path;
		if( mainConfig.cfg_content.imageFolder.find(':')== str_path.npos ){
			str_path+= cwd_my+ L"\\";
		}
		if(overide) _CurrImage_stringPath= str_imgName;
		else {
					_CurrImage_stringPath= str_path+ str_imgName;
					_PrevImage_stringPath= _CurrImage_stringPath;
		}
		if( stringEnds(str_imgName, mainConfig.cfg_content._ImageExtProblematic, true ) ){	//image is Problematic, convert
			size_t posExtBeg= str_imgName.find_last_of( L'.' );
			problematicFormat_ext= str_imgName.substr( posExtBeg );

			str_path+= L".JPG";		//delete temp conversion files
			SetFileAttributesW( problematicFormat_ext.c_str(), 0 );
			DeleteFileW( problematicFormat_ext.c_str() );
			SetFileAttributesW( L".JPG", 0 );
			DeleteFileW( L".JPG" );
			Sleep(5);	//give NTFS some time to index that such file no longer exists

			CopyFileW( str_imgName.c_str(), problematicFormat_ext.c_str(), false );	//fusking winApi never shows what arguments do
			std::wstring temp_exe_exe( _OwnPath );
			 temp_exe_exe+= _ImageConverter_exe;
			std::wstring temp_argStr( problematicFormat_ext+ L" .JPG " );
			 temp_argStr+= _ImageConverter_args+ L" ";
			 temp_argStr+= std::wstring( mainConfig.cfg_content.BG_Colour_RGB.begin(), mainConfig.cfg_content.BG_Colour_RGB.end() );

			wprintf( L" Converting \"%s\" to .jpeg\n", problematicFormat_ext.c_str() );
			ShellExecuteW(
				NULL,
				L"open",
				temp_exe_exe.c_str(),
				temp_argStr.c_str(),
				NULL,
				0
			);
			int waitedFor= 0;
			while( waitedFor< 15 ){
				if( exists_Wfile( L".JPG" ) ) break;
				Sleep(100); ++waitedFor;
			}
printf("waitedFor = %i00 ms\n", waitedFor);
			if(waitedFor>= 15){
				std::wstring temp_errMsg( L"! Error ! could not convert image from " );
				temp_errMsg+= problematicFormat_ext + L" to .jpeg\n";
				writeToMultiple(temp_errMsg);
			}
			SetFileAttributesW( problematicFormat_ext.c_str(), 0 );
			DeleteFileW( problematicFormat_ext.c_str() );
			Sleep(5);	//give NTFS some time to index (again)
		} else if(mainConfig.cfg_content.convertUTFNames && stringContainsUTFchars(str_imgName) ) {
			size_t posExtBeg= str_imgName.find_last_of( L'.' );
			if(overide) str_path+= L"_";
			str_path+= str_imgName.substr( posExtBeg );
			SetFileAttributesW( str_path.c_str(), 0 );
			DeleteFileW(str_path.c_str());
			CopyFileW( str_imgName.c_str(), str_path.c_str(), false );	//fusking winApi never shows what arguments do
		} else {
			if(overide)	str_path= str_imgName;
			else		str_path+= str_imgName;	//image is ok = not .PNG
		}
		if(!overide) _CurrRegImage= str_path;

		SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *)str_path.c_str() , SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
			//new on 2020-02-16
		SetWStringRegKeyValue( HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"WallPaper", str_path );
			//new on 2020-02-16
		refreshDesktop();
	} else {
		printf("  Not Exists, rescan...\n");
		prepStart();
		return imageChange();
	}

}
bool imageDirExplorer::stringContainsUTFchars(const std::wstring& wstrIn)
{
		///last char of ANSII
	const wchar_t fUTF= 127;
	for(std::wstring::const_iterator iterWStr= wstrIn.begin(); iterWStr!= wstrIn.end(); ++iterWStr){
		if( (*iterWStr)>fUTF ) return 1;
	}
	return 0;
}
void imageDirExplorer::log_pwd()
{
	std::wstring temp_pwd_path= L"show pwd >\n"+ get_pwd() + L"\n";
	writeToMultiple(temp_pwd_path);
}
void imageDirExplorer::reshowWP()
{
	if( _CurrRegImage.empty() || !exists_Wfile(_CurrRegImage.c_str()) ){
		writeToMultiple( L"reshow! failed, image NOT found\n" );
		return;
	}
	if( _PrevImage_stringPath.size() )
		_CurrImage_stringPath= _PrevImage_stringPath;	//restore path so /wpshow will work

	std::wstring temp_errMsg= L"ReShowing current Wallpaper image";
	if(image_p)
		temp_errMsg+= L"\n RP: "+ image_p->getPathName();	//get image name to display
	temp_errMsg+= L"\n";

	writeToMultiple(temp_errMsg);

	SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *)_CurrRegImage.c_str() , SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
	SetWStringRegKeyValue( HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"WallPaper", _CurrRegImage );
	refreshDesktop();
}
void imageDirExplorer::refreshDesktop()
{
	ShellExecuteW(	//reload desktop BG null
		NULL,
		L"open",
		L"cmd",
		L" /c RUNDLL32.EXE user32.dll,UpdatePerUserSystemParameters",
		NULL,
		0
	);
	ShellExecuteW(	//reload desktop BG 1,True
		NULL,
		L"open",
		L"cmd",
		L" /c RUNDLL32.EXE user32.dll,UpdatePerUserSystemParameters ,1,True",
		NULL,
		0
	);
	ShellExecuteW(	//reload desktop BG 0,False
		NULL,
		L"open",
		L"cmd",
		L" /c RUNDLL32.EXE user32.dll,UpdatePerUserSystemParameters ,0,False",
		NULL,
		0
	);
}
void imageDirExplorer::prepStart( bool LoadShownImg )
{
	getImagesFromDir();
	if(DF_list.empty() ){
		printf("!!! !Error! nothing to work with.\n");
		if( ArgsConfig.showLogTo.size() ){
			writeUtfLine( L"!!! !Error 404! nothing to work with", ArgsConfig.showLogTo );
			list_writeUtfLine.clear();
		}
		PostQuitMessage(404);
		exit(404);
	}

	DF_list_p.reserve( DF_list.size()+1 );
	for( auto on_it= DF_list.begin(); on_it!= DF_list.end(); ++on_it ){
		DF_list_p.push_back( &*on_it );
	}

		//validate that cwd+imageFolder is not UTF path
	if(mainConfig.cfg_content.convertUTFNames && (stringContainsUTFchars(cwd_my)) ){
		writeToMultiple(L"Warning! config.convertUTFNames=1 ! can not avoid UTF names because this path contains UTF characters\n");
		mainConfig.cfg_content.convertUTFNames= false;
	}

	sortBy();
	if(LoadShownImg) SavedList_read();
}
void imageDirExplorer::rescan()
{
	prepStart();
	SavedList_getCurrentWP( "BGchanger_selected.cfg" );
}
void imageDirExplorer::SavedList_remove()
{
	list_writeUtfLine.removeFile( "BGchanger_List.cfg" );
}
void imageDirExplorer::SavedList_read()
{
	if( !mainConfig.cfg_content.saveLastImages ) return;

	readUtfFile ListFile;
	std::wstring Line;
	size_t valDelEl;
	if( ListFile.Open( "BGchanger_List.cfg" ) ){
		while( ListFile.readLine( Line ) ){
			if( Line.size() ){
				valDelEl= ImgInList_find( Line );
				if( valDelEl!= Line.npos ){
					DF_list_p.delEl( valDelEl );
					lastItemName= Line;
				}
			}
		}
		ListFile.Close();
	}
}
bool imageDirExplorer::SavedList_getCurrentWP( const std::string& fileName )
{
	bool retVal= false;
	if( !mainConfig.cfg_content.saveLastImages ) return false;

	readUtfFile ListFile;
	if( ListFile.Open( fileName ) ){
		std::wstring Line;
		while( ListFile.readLine( Line ) ){
			if( Line.size() ){
				size_t valDelEl= ImgInList_find( Line );
				if( valDelEl!= Line.npos ){
					image_i= valDelEl;
					image_p= *DF_list_p.at_pos( valDelEl );
					DF_list_p.delEl( valDelEl );
					retVal= true;
					break;
				}
			}
		}
		ListFile.Close();
	}
return retVal;
}

size_t imageDirExplorer::ImgInList_find( const std::wstring& strImagePath )
{
	DirFileEnt temp_findEnt( strImagePath );
		///get pos of last \\ in path
	size_t PosLastSl= strImagePath.find_last_of( L"\\" );
	if( PosLastSl!= strImagePath.npos ){
		temp_findEnt.name= strImagePath.substr( PosLastSl+1 );
		temp_findEnt.path= strImagePath.substr( 0, PosLastSl );
	}

	vector_DFp::iterator thatIt= std::lower_bound( DF_list_p.begin_f(), DF_list_p.end(), &temp_findEnt, compare_paths_bool );
	if( thatIt!= DF_list_p.end() && (*thatIt)->getPathName()== temp_findEnt.getPathName() ){
		return thatIt- DF_list_p.begin_f();
	} else {
		std::wstring temp_errMsg( L"List has non existing: \"" );
		temp_errMsg+= strImagePath+ L"\"\n";
		writeToMultiple(temp_errMsg);
		writeToMultiple(L"\n",writeTo_console);
		return -1;
	}

	return -1;
}

void imageDirExplorer::SavedList_add()
{
	image_p_old= image_p;
	if( !mainConfig.cfg_content.saveLastImages ) return;
	if( image_p && lastItemName!= image_p->getPathName(false) ){	//we dont want the same image twice in the done list
		writeUtfLine( image_p->getPathName(false), "BGchanger_List.cfg" );
	}
}
size_t imageDirExplorer::writeUtfLine( const std::wstring& strWrite, const std::string& file_out, std::string modeOpenOveride, bool quickDiscard )
{
	size_t retVal= 0;
	std::wstring temp_endl( L"\n");

	modeOpenOveride+= ",ccs=UTF-8";
	FILE* out_file= nullptr;
	if(quickDiscard){
		out_file= fopen( file_out.c_str(), modeOpenOveride.c_str() );
	} else {
		out_file= list_writeUtfLine.getOrAdd( file_out.c_str(), modeOpenOveride )->get_file();
	}

	if( out_file ){
	 retVal+= fwrite( strWrite.c_str() , strWrite.size() * sizeof(wchar_t), 1, out_file );
	 retVal+= fwrite( temp_endl.c_str(), temp_endl.size() * sizeof(wchar_t), 1, out_file );
	 if(quickDiscard) fclose( out_file );
	} else return -1;
return retVal;
}
void imageDirExplorer::writeToMultiple(const std::wstring& strWrite, writeToMultiple_enum writeToWhere)
{
	if(writeToWhere & writeTo_console){
		wprintf( L"%s", strWrite.c_str() );
		if(ArgsConfig.hOutPipedToFile){
			DWORD bytesToWrite= strWrite.size()*sizeof(wchar_t);
			DWORD bytesWritten;
			WriteFile(ArgsConfig.hOutPipedToFile, strWrite.c_str(), bytesToWrite, &bytesWritten, NULL);
			if(bytesWritten!= bytesToWrite){
			}
		}
	}

	if(writeToWhere & writeTo_files) {
		if( ArgsConfig.showLogTo.size() ) writeUtfLine( strWrite, ArgsConfig.showLogTo );
		if( ArgsConfig.showThisLogTo.size() ) writeUtfLine( strWrite, ArgsConfig.showThisLogTo );
	}
}
size_t imageDirExplorer::writeTime(const std::string& fileOut)
{
	time_t now= time(0);
	struct tm tstruct;
	char buf[80]= {0};
	tstruct= *localtime( &now );
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d  %X", &tstruct);

	const std::string tempTimeStr( buf );
	const std::wstring tempTime( tempTimeStr.begin(), tempTimeStr.end() );
	return writeUtfLine( tempTime, fileOut );
}

bool exists_file(const std::string& name)
{
	if(FILE *file= fopen(name.c_str(), "r")) {
		fclose(file);
		return true;
	} else {
		return false;
	}
}
bool exists_Wfile(LPCWSTR szPath)
{
  DWORD dwAttrib= GetFileAttributesW(szPath);

	return(dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}













