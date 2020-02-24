#ifndef CONFIGIO_H
#define CONFIGIO_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm> // transform
#include <stdio.h>	//popen
#include <stdlib.h>

#include <windows.h>


const std::string emptyString;
typedef std::vector<std::string> vectorString;

int replaceAll_inPlace(std::wstring& str, const std::wstring& rep_from, const std::wstring& rep_to, size_t moveBy= -1);
std::wstring replaceAll(std::wstring str, const std::wstring& rep_from, const std::wstring& rep_to, size_t moveBy= -1);

bool stringLineSeparate2(const std::string& str_in, vectorString& out, const std::string& separator="=", bool separateAll=false);
	///@return 0 no, 1 true
bool stringBegins(const std::string& stringIn, const std::string& begins, bool CaseInsensit=true, std::string* rest=nullptr );
bool stringBegins(const std::wstring& stringIn, const std::wstring& begins, bool CaseInsensit=true, std::wstring* rest=nullptr );

LONG GetStringRegKey(HKEY hKey, const std::string &strValueName, std::string &strValue);
LONG GetWStringRegKey(HKEY hKey, const std::wstring &strValueName, std::wstring &strValue, const std::wstring &strDefaultValue= L"");
LONG SetWStringRegKeyValue(const HKEY under_hKey, const std::wstring& under_path, const std::wstring& under_name, const std::wstring& setValue );

//extern "C" FILE *popen(const char *command, const char *mode);
//extern "C" FILE *pclose(FILE* fp);

struct configFileContent{
	BYTE random= 0;
	BYTE sort= 1;
	BYTE sortDigitsAsNumbers= 1;
	BYTE DirSortByFileName= 0;
	std::string imageFolder= "background";
	std::string skipFoldersBeginning= "old";
	std::string skipFoldersEnding= "/";
	std::string BG_Colour_RGB= "0 0 0";
	BYTE skipHiddenFolders= 1;
	vectorString imageExt;
	vectorString _ImageExtProblematic;

	bool forcedImageChoosing= false;
	BYTE useSystemTime= 0;
		///works only with useSystemTime
	BYTE readjustTimeAfterSleep= 0;
	BYTE saveLastImages= 0;
	size_t time= 0;

};
struct configArgsContent{
	std::string showLogTo;
	std::string showThisLogTo;
	bool forcedImageChoosing= false;
};

	///reads UTF string from ANSI named file
class readUtfFile{
 public:
	readUtfFile(size_t bufferSize=4096);
	~readUtfFile();

		///normal method to readLine
	size_t readLine( std::wstring& out_Line );
		///returns string casted from readLine(wstring)
	size_t readLine( std::string& out_Line );
		///1 ok, 0 fail
	bool Open(const std::string& open_file= "BGchanger_List.cfg");
		///@return close error code
	int Close();

	void setBufferSize(size_t bufferSize=4096){ ReadOnce_size= bufferSize; }
 protected:
	size_t ReadOnce_size= 4096;
	std::wstring ReadOnce_left;
	FILE* io_file= nullptr;

		///appends string
	size_t readTo( std::wstring& out_read );
};

class configFile{
 public:
	configFile();
	~configFile();

		///GenerateNew if file does not exist
	bool Open(const std::string& file);
//		///close without saving
//	void Close();
//		///save changes
//	bool Flush();
	bool GenerateNew(const std::string& file);

	configFileContent cfg_content;
 protected:
	std::string open_file;
};



#endif
