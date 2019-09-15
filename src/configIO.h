#ifndef CONFIGIO_H
#define CONFIGIO_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <windows.h>


const std::string emptyString;
typedef std::vector<std::string> vectorString;


bool stringLineSeparate2(const std::string& str_in, vectorString& out, const std::string& separator="=", bool separateAll=false);

struct configFileContent{
	BYTE random= 0;
	BYTE sort= 1;
	BYTE sortDigitsAsNumbers= 1;
	BYTE DirSortByFileName= 0;
	std::string imageFolder= "background";
	std::string skipFoldersBeginning= "old";
	std::string BG_Colour_RGB= "0 0 0";
	BYTE skipHiddenFolders= 1;
	vectorString imageExt;

	BYTE useSystemTime= 0;
	BYTE saveLastImages= 0;
	size_t time= 0;

};
struct configArgsContent{
	bool showOnly= false;
	std::string showLogTo;
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
