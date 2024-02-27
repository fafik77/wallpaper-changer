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

int removeBeginingWhiteSpace(std::string &str);

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
	BYTE JPEG_Quality= 93;
	BYTE convertUTFNames= false;

	bool forcedImageChoosing= false;
	std::string logFile;
	BYTE useSystemTime= 0;
		///works only with useSystemTime
	BYTE readjustTimeAfterSleep= 0;
	BYTE saveLastImages= 0;
	size_t time= 0;

};
struct configArgsContent{
	std::string showLogTo;
	std::string showThisLogTo;
	HANDLE hOutPipedToFile;

	bool forcedImageChoosing= false;
};
struct struct_uuid{
 protected:
	BYTE data[16]= {0};
 public:
	~struct_uuid(){}
	struct_uuid(){}
	struct_uuid(const int& p1,const short& p2,const short& p3,const short& p4,const LONGLONG& p5);
	struct_uuid(const int& i4){ if(sizeof(i4)==16){ memcpy(this->data,&i4,16); } }
	struct_uuid(const int* i4){ memcpy(this->data,i4,16); }
	struct_uuid(const BYTE* i4){ memcpy(this->data,i4,16); }
	struct_uuid(const struct_uuid& other){ set_uuid(other); }
		///@return 1 on fail, 0 on success
	inline bool set_uuid(const struct_uuid& other){
		memcpy(this->data,other.data,16);
		return 0;
	}
		///@return 1 on fail, 0 on success
	bool set_uuid(const BYTE& bytes16){
		if(sizeof(bytes16)==16){
			memcpy(this->data,&bytes16,16);
			return 0;
		}
		return 1;
	}
		///warning you HAVE to give it 16Bytes of data
		///@return 1 on fail, 0 on success
	bool set_uuid(const BYTE* bytes16){
			memcpy(this->data,bytes16,16);
			return 0;
	}
		///warning you HAVE to give it 16Bytes of data
		///@return 1 on fail, 0 on success
	bool set_uuid(const int* bytes16){
			memcpy(this->data,bytes16,16);
			return 0;
	}
//		///warning! this is used for debugging, with rotated back-to-front values
		///@return true if same
	inline bool operator == (const struct_uuid& other){return !(memcmp(other.data,this->data,16));}
	operator const BYTE*() const {return data;}
	operator BYTE*() {return data;}
};

	///struct for handleCopyData()
struct copyData_sendStruct_main_primaryBin{ //24 byte total
		///the UUID of this program & request
	struct_uuid sign_UUID;
		///length in Bytes
	size_t length_of_CmdArgLine=0;
		///length in Bytes
	size_t length_of_pathFileCOut=0;
	copyData_sendStruct_main_primaryBin(){}
	~copyData_sendStruct_main_primaryBin(){}
	copyData_sendStruct_main_primaryBin(const struct_uuid& sign_UUID_): sign_UUID(sign_UUID_) {}
};
	///struct for handleCopyData()
struct copyData_sendStruct_main: public copyData_sendStruct_main_primaryBin{
	LPWSTR CmdArgLine= NULL;
	LPWSTR pathFileCOut= NULL;

	copyData_sendStruct_main(){}
	~copyData_sendStruct_main();
	copyData_sendStruct_main(const void* inBuffer, const size_t &inSize){setFromRawData(inBuffer,inSize);}
	copyData_sendStruct_main(const struct_uuid& a_uuid, LPWSTR CmdArgLine_, LPWSTR pathFileCOut_=NULL): copyData_sendStruct_main_primaryBin(a_uuid), CmdArgLine(CmdArgLine_), pathFileCOut(pathFileCOut_) {}
	size_t getSize(){
		size_t retVal= sizeof(copyData_sendStruct_main_primaryBin); //24 byte total
		if(CmdArgLine!=NULL) retVal+= (length_of_CmdArgLine=(wcslen(CmdArgLine) * sizeof(wchar_t) )) +1;
		if(pathFileCOut!=NULL) retVal+= (length_of_pathFileCOut=(wcslen(pathFileCOut) * sizeof(wchar_t) )) +1;
		return retVal;
	}
		///@param outBuffer should be: unused BYTE* type
	size_t getRawData(void** outBuffer, size_t &outSize);
		///this function sets data of this object from RawData in
		///@return 0= success, 1= size too short, 2= declared size in use does not match inSize given
	int setFromRawData(const void* inBuffer, const size_t &inSize);
		///@return 0= success, 1= error
	static bool tryPathOutFile(std::wstring &ioTryPath, const wchar_t* withPathFile, const wchar_t* withFileName=NULL);
 protected:
	bool b_owned_CmdArgLine= 0;
	bool b_owned_pathFileCOut= 0;
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
		///@return -1 error(changes to valueOnError), 0 false, 1 true
	static BYTE getBoolFromValue(const std::string& val, const BYTE valueOnError= 0);

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
