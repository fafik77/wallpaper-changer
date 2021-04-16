#include "configIO.h"

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

	bool retVal= firstPart == begins_w;
	if(retVal && rest){
		(*rest)= stringIn.substr( findLen );
	}
	return retVal;
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

	bool retVal= firstPart == begins_w;
	if(retVal && rest){
		(*rest)= stringIn.substr( findLen );
	}
	return retVal;
}
int replaceAll_inPlace(std::wstring& str, const std::wstring& rep_from, const std::wstring& rep_to, size_t moveBy)
{
	size_t posLast= 0;
	size_t posNew= 0;
	int ret_amount= 0;
	if(moveBy==size_t(-1)) moveBy= rep_to.length();
	while( (posNew= str.find(rep_from, posLast))!= str.npos ){
		str.replace(posNew, rep_from.length(), rep_to);
		posLast= posNew+ moveBy;
		++ret_amount;
	}
	return ret_amount;
}
std::wstring replaceAll(std::wstring str, const std::wstring& rep_from, const std::wstring& rep_to, size_t moveBy)
{
	std::wstring ret_str(str);
	replaceAll_inPlace(ret_str, rep_from, rep_to, moveBy);
	return ret_str;
}
	///hKey= HKEY_CURRENT_USER
LONG GetStringRegKey(HKEY hKey, const std::string &strValueName, std::string &strValue)
{
	strValue.clear();
	char szBuffer[512];
	DWORD dwBufferSize= sizeof(szBuffer);
	ULONG nError;
	nError= RegQueryValueExA(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	if(nError== ERROR_SUCCESS){
		strValue= szBuffer;
	}
	return nError;
}
LONG GetWStringRegKey(HKEY hKey, const std::wstring &strValueName, std::wstring &strValue, const std::wstring &strDefaultValue)
{
	strValue= strDefaultValue;
	WCHAR szBuffer[512];
	DWORD dwBufferSize= sizeof(szBuffer);
	ULONG nError;
	nError= RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	if(nError== ERROR_SUCCESS){
		strValue= szBuffer;
	}
	return nError;
}
LONG SetWStringRegKeyValue(const HKEY under_hKey, const std::wstring& under_path, const std::wstring& under_name, const std::wstring& setValue )
{
	LONG retVal= 0;
	HKEY h_hKey;

	if( RegOpenKeyExW(under_hKey, under_path.c_str(), 0, KEY_SET_VALUE, &h_hKey)== ERROR_SUCCESS ){
		//path= L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"
		retVal= RegSetValueExW(h_hKey, under_name.c_str(), 0, REG_SZ, (LPBYTE)setValue.c_str(), ((wcslen(setValue.c_str()) + 1)* sizeof(WCHAR)));
		RegCloseKey(h_hKey);
	}
return retVal;
}
int removeBeginingWhiteSpace(std::string &str)
{
	int retVal= 0;
	std::string::iterator it= str.begin();

	while( it!=str.end() && ((*it)==' ' || (*it)=='\t') ){
		++it;
	}

	if(it!= str.begin()){
		retVal= it- str.begin();
		str.erase(str.begin(), it );
	}

	return retVal;
}


struct_uuid::struct_uuid(const int& p1,const short& p2,const short& p3,const short& p4,const LONGLONG& p5)
{
	memcpy(data+ 0,&p1,4);
	memcpy(data+ 4,&p2,2);
	memcpy(data+ 6,&p3,2);
	memcpy(data+ 8,&p4,2);
	memcpy(data+ 10,((BYTE*)&p5)+0,6);
}
copyData_sendStruct_main::~copyData_sendStruct_main()
{
	if(b_owned_CmdArgLine && CmdArgLine){
		delete CmdArgLine;
		CmdArgLine= NULL;
	}
	if(b_owned_pathFileCOut && pathFileCOut){
		delete pathFileCOut;
		pathFileCOut= NULL;
	}
}
size_t copyData_sendStruct_main::getRawData(void** outBuffer, size_t &outSize)
{
	size_t requiredSizeBytes= 0;
	size_t lastBytePos= sizeof(copyData_sendStruct_main_primaryBin);
	BYTE** p_writeBuff= (BYTE**)outBuffer;	//map the proper typeof
	outSize= requiredSizeBytes= getSize();
		//make required space
	(*p_writeBuff)= new BYTE[requiredSizeBytes];
		//copy the header
	memcpy( (*p_writeBuff), static_cast<copyData_sendStruct_main_primaryBin*>(this), sizeof(copyData_sendStruct_main_primaryBin));
	if(CmdArgLine){ //copy CmdArgLine
		memcpy( (*p_writeBuff)+lastBytePos, CmdArgLine, length_of_CmdArgLine );
		lastBytePos+= length_of_CmdArgLine;
		(*p_writeBuff)[lastBytePos++]= char(0);
	}
	if(pathFileCOut){ //copy path for 2nd instance output shared file
		memcpy( (*p_writeBuff)+lastBytePos, pathFileCOut, length_of_pathFileCOut );
		lastBytePos+= length_of_pathFileCOut;
		(*p_writeBuff)[lastBytePos++]= char(0);
	}
	return requiredSizeBytes;
}
int copyData_sendStruct_main::setFromRawData(const void* inBuffer, const size_t &inSize)
{
	size_t lastBytePos= sizeof(copyData_sendStruct_main_primaryBin);
	if( inSize>=lastBytePos ){
			//map the proper typeof
		BYTE* inBufferByte= (BYTE*)inBuffer;
			//set the header data
		memcpy( static_cast<copyData_sendStruct_main_primaryBin*>(this), inBufferByte , sizeof(copyData_sendStruct_main_primaryBin));
		if(b_owned_CmdArgLine && CmdArgLine)
			delete CmdArgLine;
		CmdArgLine= NULL;
		if(b_owned_pathFileCOut && pathFileCOut)
			delete pathFileCOut;
		pathFileCOut= NULL;

		if( inSize>=(lastBytePos+ length_of_CmdArgLine+ length_of_pathFileCOut+ bool(length_of_CmdArgLine)) ){
			if(length_of_CmdArgLine){
				b_owned_CmdArgLine= true;
				CmdArgLine= new wchar_t[(length_of_CmdArgLine/sizeof(wchar_t)) +1];
				CmdArgLine[(length_of_CmdArgLine/sizeof(wchar_t))]= 0;
				memcpy(CmdArgLine, inBufferByte+lastBytePos, length_of_CmdArgLine);
				lastBytePos+= length_of_CmdArgLine+ 1;
			}

			if(length_of_pathFileCOut){
				b_owned_pathFileCOut= true;
				pathFileCOut= new wchar_t[(length_of_pathFileCOut/sizeof(wchar_t)) +1];
				pathFileCOut[(length_of_pathFileCOut/sizeof(wchar_t))]= 0;
				memcpy(pathFileCOut ,inBufferByte+lastBytePos, length_of_pathFileCOut);
				lastBytePos+= length_of_pathFileCOut+ 1;
			}
			return 0; //ok
		}
		return 2; //inSize< size declared in use
	}
	return 1; //inSize< header
}
bool copyData_sendStruct_main::tryPathOutFile(std::wstring &ioTryPath, const wchar_t* withPath, const wchar_t* withFile)
{
	size_t fullSize= ioTryPath.size();
	ioTryPath= ioTryPath.c_str();
	if(ioTryPath.back()!= L'\\' )
		ioTryPath.append( L"\\" );
	ioTryPath.append( withPath );
	if(withFile) ioTryPath.append( withFile );
	if(ioTryPath.size()< fullSize){
		HANDLE hOutFile= CreateFileW(ioTryPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, (FILE_ATTRIBUTE_NORMAL), NULL );
		if(hOutFile){
			CloseHandle(hOutFile);
			return 0;
		}
	}
	return 1;
}

readUtfFile::readUtfFile(size_t bufferSize): ReadOnce_size(bufferSize)
{
}
readUtfFile::~readUtfFile()
{
	Close();
}
int readUtfFile::Close()
{
	int retVal= 0;
	if(io_file){
		retVal= fclose(io_file);
		io_file= nullptr;
	}
return retVal;
}
bool readUtfFile::Open(const std::string& open_file)
{
	io_file= fopen( open_file.c_str(), "r,ccs=UTF-8" );
return io_file;
}

size_t readUtfFile::readTo( std::wstring& out_read )
{
	std::wstring temp_str;
	temp_str.resize( ReadOnce_size, 0 );

		///(taken from wiki) size_t readSize= fread( &temp_str.at(0) , wcslen(temp_str.c_str()) * sizeof(wchar_t), 1, io_file );
	size_t readSize= fread( &temp_str.at(0) , sizeof(wchar_t), ReadOnce_size, io_file );

	out_read.append( temp_str.substr(0, readSize) ) ;
	return readSize;
}
size_t readUtfFile::readLine( std::wstring& out_Line )
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
size_t readUtfFile::readLine( std::string& out_Line )
{
	std::wstring temp_WstrToAstr;
	size_t retVal= readLine( temp_WstrToAstr );
	out_Line= std::string( temp_WstrToAstr.begin(), temp_WstrToAstr.end() );
	return retVal;
}


bool stringLineSeparate2(const std::string& str_in, vectorString& out, const std::string& separator, bool separateAll)
{
	size_t lastPos= 0;
	size_t onPos= 0;

	if( separateAll ){
		while( onPos!= str_in.npos ){
			lastPos= onPos;
			onPos= str_in.find(separator, lastPos+separator.length() );

			if(lastPos) lastPos+=separator.length();
			out.push_back( str_in.substr( lastPos, (onPos-lastPos) ) );
			if( out.back().empty() ) out.pop_back();
		}
	} else {
		onPos= str_in.find(separator, lastPos);
		if( onPos==str_in.npos ) return 0;
		out.push_back( str_in.substr(0, onPos) );
		out.push_back( str_in.substr(onPos+1) );
	}
	return true;
}



configFile::configFile()
{
}
configFile::~configFile()
{
}

BYTE configFile::getBoolFromValue(const std::string& val, const BYTE valueOnError)
{
	std::string valBetter(val);
	while( valBetter.size() && ( valBetter.at(0)== ' ' || valBetter.at(0)== '\t' ) )	//remove spaces from start
		valBetter.erase(0,1);

	BYTE retVal= -1;
		//try simple 0, 1
	if(valBetter.size() == 1){
		if( valBetter.at(0)== '0' ) retVal= 0;
		else if( valBetter.at(0)== '1' ) retVal= 1;
	}
		//try str(true, false)
	if(retVal== BYTE(-1) ){
		if( valBetter== "false" ) retVal= 0;
		else if( valBetter== "true" ) retVal= 1;
	}
		//try get number
	if(retVal== BYTE(-1) ){
		size_t temp_st= 0;
		std::istringstream temp_sis( valBetter );
		temp_sis>> temp_st;
		if( !temp_sis.bad() )
			retVal= temp_st;
	}

if(retVal== BYTE(-1) ) retVal= valueOnError;
return retVal;
}

bool configFile::Open(const std::string& file)
{
	open_file= file;
	readUtfFile cfgFstr;

	if( cfgFstr.Open( file ) ){
		std::string line;
		while( cfgFstr.readLine(line) ){
			vectorString str_out;
			if( stringLineSeparate2(line, str_out) ){
				size_t temp_st= 0;
				if( str_out.at(0) == "random" ){
					std::istringstream temp_sis(str_out.at(1));
					temp_sis>> temp_st;
					cfg_content.random= temp_st;
					printf("# config.random= %u, ", cfg_content.random);
					if( cfg_content.random == 1 )	printf( "random no repeat" );
					else if( cfg_content.random == 2 )	printf( "with repeat" );
					else printf( "NOT random" );
					printf("\n");
				} else if( str_out.at(0) == "sort" ){
					cfg_content.sort= getBoolFromValue(str_out.at(1));
				} else if( str_out.at(0) == "sortDigitsAsNumbers" ){
					cfg_content.sortDigitsAsNumbers= getBoolFromValue(str_out.at(1));
				} else if( str_out.at(0) == "DirSortByFileName" ){
					cfg_content.DirSortByFileName= getBoolFromValue(str_out.at(1));
				} else if( str_out.at(0) == "imageExt" ){
					stringLineSeparate2(str_out.at(1), cfg_content.imageExt, " ", true);
				} else if( str_out.at(0) == "convertExt" ){
					stringLineSeparate2(str_out.at(1),  cfg_content._ImageExtProblematic, " ", true);
				} else if( str_out.at(0) == "convertUTFNames" ){
					cfg_content.convertUTFNames= getBoolFromValue(str_out.at(1));
				} else if( str_out.at(0) == "skipFoldersBeginning" ){
					cfg_content.skipFoldersBeginning= str_out.at(1);
				}  else if( str_out.at(0) == "skipFoldersEnding" ){
					cfg_content.skipFoldersEnding= str_out.at(1);
				} else if( str_out.at(0) == "skipHiddenFolders" ){
					cfg_content.skipHiddenFolders= getBoolFromValue(str_out.at(1));
				} else if( str_out.at(0) == "BG_Colour_RGB" ){
					if( str_out.at(1) == "Background" ){
							//new approach on 2020-02-16
						HKEY h_hKey;
						if( RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Colors", 0, KEY_QUERY_VALUE, &h_hKey)== ERROR_SUCCESS ){
							if( GetStringRegKey( h_hKey, "Background", cfg_content.BG_Colour_RGB )!= ERROR_SUCCESS ){
								cfg_content.BG_Colour_RGB= "0 0 0";
							}
							printf("Loaded Background rgb = \"%s\"\n", cfg_content.BG_Colour_RGB.c_str() );
							RegCloseKey(h_hKey);
						}
					} else {
						cfg_content.BG_Colour_RGB= str_out.at(1);
					}
				} else if( str_out.at(0) == "imageFolder" ){
					bool removedPart= true;
					while(removedPart){
						removedPart= stringBegins( str_out.at(1), ".\\", false, &str_out.at(1) );
						if(!removedPart) removedPart= stringBegins( str_out.at(1), "./",	false, &str_out.at(1) );
						if(!removedPart) removedPart= stringBegins( str_out.at(1), "/",		false, &str_out.at(1) );
						if(!removedPart) removedPart= stringBegins( str_out.at(1), "\\",	false, &str_out.at(1) );
					}
					cfg_content.imageFolder= str_out.at(1);
				} else if( str_out.at(0) == "useSystemTime" ){
					cfg_content.useSystemTime= getBoolFromValue(str_out.at(1));
				} else if( str_out.at(0) == "forcedImageChoosing" ){
					cfg_content.forcedImageChoosing= getBoolFromValue(str_out.at(1));
				} else if( str_out.at(0) == "readjustTimeAfterSleep" ){
					cfg_content.readjustTimeAfterSleep= getBoolFromValue(str_out.at(1));
				} else if( str_out.at(0) == "saveLastImages" ){
					cfg_content.saveLastImages= getBoolFromValue(str_out.at(1));
				} else if( str_out.at(0) == "time" ){
					std::istringstream temp_sis(str_out.at(1));
					std::string str_rest;
					temp_sis>> temp_st;
					temp_sis>> str_rest;
					if( str_rest== "m" || str_rest== "min" ){
						temp_st*= 60;
					} else if( str_rest== "h" || str_rest== "hour" ){
						temp_st*= 3600;
					}
					if(temp_st<5) temp_st= 5;
					if(temp_st>86400){
						printf("error time can NOT be > 24h.. time set to 24h\n");
						temp_st= 86400;
					}
					cfg_content.time= temp_st;
				} else if( str_out.at(0) == "logFile" ){
					removeBeginingWhiteSpace(str_out.at(1));
					cfg_content.logFile= str_out.at(1);
				}
			}
		}
		cfgFstr.Close();
	} else {
		return GenerateNew(file);
	}
		//correct values
	if( cfg_content.useSystemTime && cfg_content.time%60 ){	//not supported
		printf("!!! config.time= %u does not work together with `config.useSystemTime` Please use n*60. !`config.useSystemTime` set to 0!\n", cfg_content.time);
		cfg_content.useSystemTime= false;
	}
	if(cfg_content.skipFoldersEnding.empty()) cfg_content.skipFoldersEnding= "/";
	if(!cfg_content.useSystemTime) cfg_content.readjustTimeAfterSleep= false;
return true;
}

bool configFile::GenerateNew(const std::string& file)
{
	std::ofstream cfgFstr;
	cfgFstr.open( file );
	if( !cfgFstr.is_open() ) return false;
	std::string temp_str="\
 @randomize show sequence, 1=random no repeat(requires `saveLastImages=1`), 2=with repeat\n\
random=0\n\
 @sort files, sorted CaseInsensitive alphabetically\n\
sort=1\n\
 @sort numbers, numerically\n\
sortDigitsAsNumbers=1\n\
 @sort dir content: 1= by filenames, 0= by full path\n\
DirSortByFileName=1\n\
\n\
 @image folder\n\
imageFolder=pic\n\
 @accepted ext of images, coverts: .png, .tiff, .webp to JPEG\n\
imageExt=.png .jpg .jpeg\n\
 @converts image to jpeg before showing it\n\
convertExt=.png .tif .tiff .webp\n\
 @copies UTF named image into ANSII named image before showing it(some windows desktops might not like UTF chars in image path)\n\
convertUTFNames=0\n\
 @to skip ALL folders leave empty, to skip NOTHING use '/'\n\
skipFoldersBeginning=old\n\
skipFoldersEnding=/\n\
 @dont show content of Hidden folders\n\
skipHiddenFolders=1\n\
\n\
 @converts .PNG to jpeg with BG Colour as set RGB( R, G, B ) or enter\"Background\" to get BGColour automatic from HKCU->Background Colour\n\
BG_Colour_RGB=Background\n\
 @when providing an ImageFileName Argument[1]. Always rescans to find it\n\
forcedImageChoosing=1\n\
 @system time instead of internal timer\n\
useSystemTime=1\n\
 @detect if PC exited Sleep and readjust timer(works only with useSystemTime)\n\
readjustTimeAfterSleep=1\n\
 @save shown images to file to start again from that spot\n\
saveLastImages=1\n\
 @time in seconds ( only n*60 works with `useSystemTime=true`)\n\
time=10 min\n\
\n\
 @save Logs into file, leave empty for no saving(you can also use Arg /log)\n\
logFile=\n\
";
	cfgFstr.write( temp_str.c_str(), temp_str.length() );

	cfgFstr.flush();
	cfgFstr.close();
	return true;
}


