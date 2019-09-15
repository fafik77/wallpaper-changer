#include "configIO.h"


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
	std::ifstream cfgFstr;
	cfgFstr.open( file );
	if( cfgFstr.is_open() ){
		std::string line;
		while( std::getline(cfgFstr, line) ){
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
				} else if( str_out.at(0) == "skipFoldersBeginning" ){
					cfg_content.skipFoldersBeginning= str_out.at(1);
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
						} } Sleep(10);
						remove( "[]Background[]" );
					} else {
						cfg_content.BG_Colour_RGB= str_out.at(1);
					}
				} else if( str_out.at(0) == "imageFolder" ){
					cfg_content.imageFolder= str_out.at(1);
				} else if( str_out.at(0) == "useSystemTime" ){
					std::istringstream temp_sis(str_out.at(1));
					temp_sis>> temp_st;
					cfg_content.useSystemTime= temp_st;
				} else if( str_out.at(0) == "saveLastImages" ){
					std::istringstream temp_sis(str_out.at(1));
					temp_sis>> temp_st;
					cfg_content.saveLastImages= temp_st;
				} else if( str_out.at(0) == "time" ){
					std::istringstream temp_sis(str_out.at(1));
					std::string str_rest;
					temp_sis>> temp_st;
					temp_sis>> str_rest;
					if( str_rest == "m" || str_rest == "min" ){
						temp_st*= 60;
					} else if( str_rest == "h" || str_rest == "hour" ){
						temp_st*= 3600;
					}
					if(temp_st<5) temp_st= 5;
					cfg_content.time= temp_st;
					if( cfg_content.useSystemTime && temp_st%60 ){	//not supported
						printf("!!! config.time= %u does not work together with `config.useSystemTime` Please use n*60. !`config.useSystemTime` set to 0!\n", temp_st);
						cfg_content.useSystemTime= false;
					}
				}
			}
		}
		cfgFstr.close();
	} else {
		cfgFstr.close();
		return GenerateNew(file);
	}
	return true;
}

bool configFile::GenerateNew(const std::string& file)
{
	std::ofstream cfgFstr;
	cfgFstr.open( file );
	if( !cfgFstr.is_open() ) return false;
	std::string temp_str="\
 @randomize show sequence (best NOT to use `sort=1` with it), 1=random no repeat, 2=with repeat\n\
random=0\n\
 @sort files, sorted CaseInsensitive alphabetically\n\
sort=1\n\
 @sort numbers, numerically\n\
sortDigitsAsNumbers=1\n\
 @1= sort dir content by filenames, 0= sort by full path\n\
DirSortByFileName=1\n\
\n\
 @image folder\n\
imageFolder=./pic\n\
 @accepted ext of images, coverts: .png, .tiff, .webp to JPEG\n\
imageExt=.png .jpg .jpeg\n\
 @to skip ALL folders leave empty, to skip NOTHING use '/'\n\
skipFoldersBeginning=old\n\
 @dont show content of Hidden folders\n\
skipHiddenFolders=1\n\
\n\
 @converts .PNG to jpeg with BG Colour as set RGB( R, G, B ) or enter\"Background\" to get BGColour automatic\n\
BG_Colour_RGB=Background\n\
 @system time instead of internal timer\n\
useSystemTime=1\n\
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


