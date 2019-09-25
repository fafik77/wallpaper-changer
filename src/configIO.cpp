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
					std::istringstream temp_sis(str_out.at(1));
					temp_sis>> temp_st;
					cfg_content.sort= temp_st;
				} else if( str_out.at(0) == "sortDigitsAsNumbers" ){
					std::istringstream temp_sis(str_out.at(1));
					temp_sis>> temp_st;
					cfg_content.sortDigitsAsNumbers= temp_st;
				} else if( str_out.at(0) == "DirSortByFileName" ){
					std::istringstream temp_sis(str_out.at(1));
					temp_sis>> temp_st;
					cfg_content.DirSortByFileName= temp_st;
				} else if( str_out.at(0) == "imageExt" ){
					stringLineSeparate2(str_out.at(1), cfg_content.imageExt, " ", true);
				} else if( str_out.at(0) == "convertExt" ){
					stringLineSeparate2(str_out.at(1),  cfg_content._ImageExtProblematic, " ", true);
				} else if( str_out.at(0) == "skipFoldersBeginning" ){
					cfg_content.skipFoldersBeginning= str_out.at(1);
				}  else if( str_out.at(0) == "skipFoldersEnding" ){
					cfg_content.skipFoldersEnding= str_out.at(1);
				} else if( str_out.at(0) == "skipHiddenFolders" ){
					std::istringstream temp_sis(str_out.at(1));
					temp_sis>> temp_st;
					cfg_content.skipHiddenFolders= temp_st;
				} else if( str_out.at(0) == "BG_Colour_RGB" ){
					if( str_out.at(1) == "Background" ){
						ShellExecute(
							NULL,
							"open",
							"cmd",
							" /c reg query \"HKEY_CURRENT_USER\\Control Panel\\Colors\" /v Background > []Background[]",
							NULL,
							NULL
						);
						Sleep(200);
						{ std::ifstream temp_file_bgCol;
						temp_file_bgCol.open( "[]Background[]" );
						if(temp_file_bgCol.good() && temp_file_bgCol.is_open()){
							std::string str_line;
							while( !temp_file_bgCol.eof() ){
								getline( temp_file_bgCol, str_line );
								if( str_line.find( "Background" )!= str_line.npos ){
									std::string str_word;
									std::istringstream temp_sis( str_line );
									temp_sis>> str_word;
									temp_sis>> str_word;
									cfg_content.BG_Colour_RGB= str_line.substr( str_line.find_first_not_of( " \t", temp_sis.tellg() ) );
								}
							}
							temp_file_bgCol.close();
							temp_file_bgCol.clear();
						} }
						Sleep(20);
						remove( "[]Background[]" );
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
				}else if( str_out.at(0) == "useSystemTime" ){
					std::istringstream temp_sis(str_out.at(1));
					temp_sis>> temp_st;
					cfg_content.useSystemTime= temp_st;
				} else if( str_out.at(0) == "readjustTimeAfterSleep" ){
					std::istringstream temp_sis(str_out.at(1));
					temp_sis>> temp_st;
					cfg_content.readjustTimeAfterSleep= temp_st;
				} else if( str_out.at(0) == "saveLastImages" ){
					std::istringstream temp_sis(str_out.at(1));
					temp_sis>> temp_st;
					cfg_content.saveLastImages= temp_st;
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
";
	cfgFstr.write( temp_str.c_str(), temp_str.length() );

	cfgFstr.flush();
	cfgFstr.close();
	return true;
}


