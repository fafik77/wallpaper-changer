#include "fileFinder.h"

thread_local std::mt19937 gen{std::random_device{}()};
template<typename T>
T random(T min, T max) {
	return std::uniform_int_distribution<T>{min, max}(gen);
}


configFileContent* publifiedConfig= nullptr;

void vector_DFp::del_front()
{
	++_pos_begin;
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
vectorDF_entry::iterator vector_DFp::at_pos( size_t pos )
{
	if( (pos+ _pos_begin)< size() )
		return at( pos+ _pos_begin );
	return back();
}
size_t vector_DFp::get_size() const
{
	return size()-_pos_begin;
}

DirFileEnt::DirFileEnt(	const std::wstring& name_, const std::wstring& path_ ): name(name_), path(path_)
{
	if( name_.find_first_of( L"0123456789" )!= name_.npos ) is_number= true;
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


int compare_paths(const DirFileEnt* DE_a, const DirFileEnt* DE_b )
{
	bool option_nameOnly= publifiedConfig->DirSortByFileName;
	bool option_asNumbers= publifiedConfig->sortDigitsAsNumbers;
	std::wstring str_a, str_b;

	if(option_nameOnly){
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
		BYTE isDigits_for= isDigits( c_a ) | (2*isDigits( c_b ));

		if( option_asNumbers && isDigits_for ){
			if( isDigits_for == 3 ){
				long long out_LL[2]= {0};
				size_t moved= getDigitsAsNumber( str_a.substr(PosCurr), str_b.substr(PosCurr), out_LL );
				long long change= out_LL[0] - out_LL[1];
				if(change) return change;	//return number if not same

				PosCurr+= moved;			//if same, skip number part
				c_a= std::toupper(str_a.at(PosCurr));
				c_b= std::toupper(str_b.at(PosCurr));
			} else if( isDigits_for == 1 ) {
				return -1;
			} else { return 1; }
		}

		int reslt= c_a- c_b;
		if( reslt ){
			if( c_a == '\\' ) return 1;		//override check. 2 before 1
			if( c_b == '\\' ) return -1;	//override check. 1 before 2
			return reslt;	//a!b, return a - b
		}
		//a==b equal, repeat
		++PosCurr;
	}
	//return ( *(int*)a - *(int*)b );
return 0;
}


int compare_dfEnt_(const void* a, const void* b)
{
	DirFileEnt* DE_a= (DirFileEnt*)a;
	DirFileEnt* DE_b= (DirFileEnt*)b;
	return compare_paths(DE_a, DE_b);
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


bool stringBegins(const std::string& stringIn, const std::string& begins, bool CaseInsensit, std::string* rest )
{
	if( !begins.length() ) return true;
	std::string begins_w(begins);
	size_t findLen= begins_w.length();

	if( stringIn.length()< findLen ) return false;

	std::string firstPart= stringIn.substr( 0, findLen );
	if(CaseInsensit){
		std::transform(firstPart.begin(), firstPart.end(), firstPart.begin(), ::toupper);
		std::transform(begins_w.begin(), begins_w.end(), begins_w.begin(), ::toupper);
	}
	if(rest){
		(*rest)= stringIn.substr( findLen );
	}
	return ( firstPart == begins_w );
}
bool stringBegins(const std::wstring& stringIn, const std::wstring& begins, bool CaseInsensit, std::wstring* rest )
{
	if( !begins.length() ) return true;
	std::wstring begins_w(begins);
	size_t findLen= begins_w.length();

	if( stringIn.length()< findLen ) return false;

	std::wstring firstPart= stringIn.substr( 0, findLen );
	if(CaseInsensit){
		std::transform(firstPart.begin(), firstPart.end(), firstPart.begin(), ::toupper);
		std::transform(begins_w.begin(), begins_w.end(), begins_w.begin(), ::toupper);
	}
	if(rest){
		(*rest)= stringIn.substr( findLen );
	}
	return ( firstPart == begins_w );
}


BYTE imageDirExplorer::getImagesFromDir(const std::wstring& addPath, vectorDF_entry& out_vector)
{
	std::wstring pattern( mainConfig.cfg_content.imageFolder.begin(), mainConfig.cfg_content.imageFolder.end() );//(mainConfig.cfg_content.imageFolder);
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
					if( !stringBegins(curr_Name, std::wstring(mainConfig.cfg_content.skipFoldersBeginning.begin(), mainConfig.cfg_content.skipFoldersBeginning.end()), true) )
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
	if( mainConfig.cfg_content.sort ){
		qsort( &(DF_list.at(0)), DF_list.size(), sizeof(DF_list.at(0)), compare_dfEnt_ );
	}
}
void imageDirExplorer::showImageList()
{
	remove( ArgsConfig.showLogTo.c_str() );
	if( mainConfig.cfg_content.random )
		printf("  config.random is set to= %u.\n", mainConfig.cfg_content.random);
	printf(" images order= [\n");
	for( size_t temp_u=0; temp_u<DF_list.size(); ++temp_u ){
		writeUtfLine( DF_list.at(temp_u).getPathName(false), ArgsConfig.showLogTo );
		wprintf( L"%s\n", DF_list.at(temp_u).getPathName(false).c_str() );
	}
	printf(" ]end\n");
}
void imageDirExplorer::iterateImages()
{
	while(true)
	{
		imageChange();

		WaitTime();
	}
}

BYTE imageDirExplorer::WaitTime()
{
		///get time now
	std::time_t ttime_now= std::time(0);
	std::time_t ttime_pulse= ttime_now+ time_t(mainConfig.cfg_content.time);
	if(mainConfig.cfg_content.useSystemTime){
		ttime_now-= (ttime_now% 60);	//remove sec
		ttime_pulse= ttime_now- (ttime_now% dhms_time)% time_t(mainConfig.cfg_content.time);
		ttime_pulse+= time_t(mainConfig.cfg_content.time);
	}

std::tm* time_now= std::localtime( &ttime_pulse );
printf("next change: %ih:", time_now->tm_hour );
printf("%im .", time_now->tm_min );
printf("%i\n", time_now->tm_sec );

	while( ttime_now< ttime_pulse )	//compensate for sleep Lag moving it out of sync
	{
		if( exists_file("BGchangerNext.cfg") ){	//check if file exists
			remove("BGchangerNext.cfg");
			printf("# /next requested. changes image now!\n");
			return 1;
		}
		Sleep(2000);
		ttime_now= std::time(0);
	}
return 0;
}
void imageDirExplorer::imageChange()
{
	if(image_p){	//old image
		image_p->selected= 2;
		SavedList_add();
	}
	if( DF_list_p.get_size()==0 ){
		image_i= 0;
		SavedList_remove();
		prepStart();	//loop end, rescan for changes
	}

	if( !image_1Shown || !mainConfig.cfg_content.random ){	//new image
		image_1Shown= true;
		image_p= &*(DF_list_p.at_pos( image_i=0 ));
		image_p->selected= 1;
		DF_list_p.delEl( image_i );
	}
	else if( mainConfig.cfg_content.random ){	//new random image
		image_i= random( size_t(0), DF_list_p.get_size() );
		image_p= &*(DF_list_p.at_pos( image_i ));
		image_p->selected= 1;
		if( mainConfig.cfg_content.random != 2 )
			DF_list_p.delEl( image_i );
	}

		///TranscodedWallpaper
	std::wstring str_WinPaperPath(L"%userprofile%\\AppData\\Roaming\\Microsoft\\Windows\\Themes\\");
	std::wstring str_imgName;
	str_imgName+= std::wstring(mainConfig.cfg_content.imageFolder.begin(), mainConfig.cfg_content.imageFolder.end())+ L"\\";
	str_imgName+= image_p->getPathName(false);
	wprintf( L" %s\n", str_imgName.c_str() );

	if( exists_Wfile( str_imgName.c_str() ) ){
		std::string	temp_cwd;
		temp_cwd= getcwd( NULL, FILENAME_MAX*2 );
		std::wstring str_path;
		if( mainConfig.cfg_content.imageFolder.find(':')== temp_cwd.npos ){
			str_path+= std::wstring(temp_cwd.begin(), temp_cwd.end())+ L"\\";
		}
		if( stringEnds(str_imgName, _ImageExtProblematic, true ) ){	//image is Problematic, convert
			size_t posExtBeg= str_imgName.find_last_of( L'.' );
			std::wstring problematicFormat_ext= str_imgName.substr( posExtBeg );
			str_path+= L".JPG";
			DeleteFileW( L".JPG" );
			CopyFileW( str_imgName.c_str(), problematicFormat_ext.c_str(), true );

			std::wstring temp_exe_exe( _OwnPath );
			 temp_exe_exe+= _ImageConverter_exe;
			std::wstring temp_argStr( problematicFormat_ext+ L" .JPG " );
			 temp_argStr+= _ImageConverter_args+ L" ";
			 temp_argStr+= std::wstring( mainConfig.cfg_content.BG_Colour_RGB.begin(), mainConfig.cfg_content.BG_Colour_RGB.end() );

			ShellExecuteW(
				NULL,
				L"open",
				temp_exe_exe.c_str(),
				temp_argStr.c_str(),
				NULL,
				NULL
			);
			int waitedFor= 0;
			while( waitedFor< 15 ){
				if( exists_Wfile( L".jpg" ) ) break;
				Sleep(100); ++waitedFor;
			}
			DeleteFileW( problematicFormat_ext.c_str() );
		} else {
			str_path+= str_imgName;	//image is ok = not .PNG
		}


		SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *)str_path.c_str() , SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);

		std::wstring str_exe_regAdd(L" add \"HKCU\\Control Panel\\Desktop\" /v WallPaper /t REG_SZ /d ");
		str_exe_regAdd+= L"\""+ str_path+ L"\" /f";

		ShellExecuteW(	//add reg key (admin) with img path
			NULL,
			L"open",
			L"reg",
			str_exe_regAdd.c_str(),
			NULL,
			NULL
		);

		ShellExecuteW(	//reload desktop BG null
			NULL,
			L"open",
			L"cmd",
			L" /c RUNDLL32.EXE user32.dll,UpdatePerUserSystemParameters",
			NULL,
			NULL
		);
		ShellExecuteW(	//reload desktop BG 1,True
			NULL,
			L"open",
			L"cmd",
			L" /c RUNDLL32.EXE user32.dll,UpdatePerUserSystemParameters ,1,True",
			NULL,
			NULL
		);
		ShellExecuteW(	//reload desktop BG 0,False
			NULL,
			L"open",
			L"cmd",
			L" /c RUNDLL32.EXE user32.dll,UpdatePerUserSystemParameters ,0,False",
			NULL,
			NULL
		);

	} else {
		printf("  Not Exists, rescan...\n");
		prepStart();
		return imageChange();
	}

}
void imageDirExplorer::prepStart()
{
	getImagesFromDir();
	if(DF_list.empty() ){
		printf("!!! !Error! nothing to work with.\n");
		exit(404);
	}
	sortBy();

	DF_list_p.reserve( DF_list.size()+1 );
	for( auto on_it= DF_list.begin(); on_it!= DF_list.end(); ++on_it ){
		DF_list_p.push_back( on_it );
	}
	SavedList_read();
}
void imageDirExplorer::SavedList_remove()
{
	remove( "BGchanger_List.cfg" );
}
void imageDirExplorer::SavedList_read()
{
	if( !mainConfig.cfg_content.saveLastImages ) return;

	readUtfFile ListFile;
	if( ListFile.Open( "BGchanger_List.cfg" ) ){
		std::wstring Line;
		while( ListFile.readLine( Line ) ){
			if( Line.size() ){
				sellectImgInList( Line );
			}
		}
		if( image_p ) image_p->selected= 1;
		ListFile.Close();
	}
}

size_t imageDirExplorer::sellectImgInList( const std::wstring& strImagePath, bool revers )
{
	if( image_p && image_p->getPathName(false) == strImagePath ) return image_i;
	size_t temp_u= 0;
	size_t tempEnd= DF_list_p.get_size();

	for( ; temp_u< tempEnd; ++temp_u){
		DirFileEnt* currEnt= &*DF_list_p.at_pos( temp_u );
		if( currEnt->getPathName(false) == strImagePath ){
			lastItemName= currEnt->getPathName(false);
			DF_list_p.delEl( temp_u );
			return temp_u;
		}
	}
return -1;
}
void imageDirExplorer::SavedList_add()
{
	if( !mainConfig.cfg_content.saveLastImages ) return;

	if( image_p &&  lastItemName!= image_p->getPathName(false) )
		writeUtfLine( image_p->getPathName(false), "BGchanger_List.cfg" );
}
size_t imageDirExplorer::writeUtfLine( const std::wstring& strWrite, const std::string& file_out )
{
	size_t retVal= 0;
	std::wstring temp_endl( L"\n");

	FILE* out_file= fopen( file_out.c_str(), "a+,ccs=UTF-8" );
	if( out_file ){
	 retVal+= fwrite( strWrite.c_str() , wcslen(strWrite.c_str()) * sizeof(wchar_t), 1, out_file );
	 retVal+= fwrite( temp_endl.c_str(), wcslen(temp_endl.c_str()) * sizeof(wchar_t), 1, out_file );
	 fclose( out_file );
	} else return -1;
return retVal;
}

imageDirExplorer::readUtfFile::readUtfFile()
{
}
imageDirExplorer::readUtfFile::~readUtfFile()
{
	Close();
}
int imageDirExplorer::readUtfFile::Close()
{
	int retVal= 0;
	if(io_file){
		fclose(io_file);
		io_file= nullptr;
	}
return retVal;
}
bool imageDirExplorer::readUtfFile::Open(const std::string& open_file)
{
	io_file= fopen( open_file.c_str(), "r,ccs=UTF-8" );
return io_file;
}

size_t imageDirExplorer::readUtfFile::readTo( std::wstring& out_read )
{
	std::wstring temp_str;
	temp_str.resize( ReadOnce_size, 0 );

		//(taken from wiki) size_t readSize= fread( &temp_str.at(0) , wcslen(temp_str.c_str()) * sizeof(wchar_t), 1, io_file );
	size_t readSize= fread( &temp_str.at(0) , sizeof(wchar_t), ReadOnce_size, io_file );

	out_read.append( temp_str.substr(0, readSize) ) ;
	return readSize;
}
size_t imageDirExplorer::readUtfFile::readLine( std::wstring& out_Line )
{
	std::wstring* temp_str;
	std::wstring out_str;
	size_t readSize= 0;
	bool madeNew= false;

	if( ReadOnce_left.size() ){
		temp_str= &ReadOnce_left;
		readSize= temp_str->length();
	} else {
		temp_str= new std::wstring();
		madeNew= true;
		readSize= readTo( *temp_str );
	}

	while(true)
	{
		if(readSize && readSize!=std::wstring::npos){
			size_t newline= temp_str->find( L'\n' );
			if( newline!= temp_str->npos )
			{	//ok done
				out_str.append( temp_str->substr(0, newline ) );
				ReadOnce_left= temp_str->substr( newline+1 );
				break;
			} else {	//repeat
				readSize+= readTo( *temp_str );
			}
		} else {
			break;
		}
	}

	out_Line= out_str;
	if(madeNew){	//cleanup
		delete temp_str;
	}
return readSize;
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
  DWORD dwAttrib = GetFileAttributesW(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}













