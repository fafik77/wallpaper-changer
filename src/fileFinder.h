#ifndef FILEFINDER_H
#define FILEFINDER_H


#include <iostream>
#include <ctime>	// time
#include <stdlib.h>	// qsort
#include <direct.h>	//cwd set, get
#include <random>

#include "configIO.h"


	///contains path and fileName, with custom sort
struct DirFileEnt{
	DirFileEnt(	const std::wstring& name_, const std::wstring& path_=L"" );
	DirFileEnt(){};
		///file name
	std::wstring name;
		///path to file
	std::wstring path;
	byte selected= 0;

		///return name
	std::wstring getName(bool afterNumber=true)const;
		///return path+name
	std::wstring getPathName(bool afterNumber=false)const;

	size_t getNameStartPos()const {
		if(path.size()) return path.size()+1;
		return path.size();
	}
};

	///vector(DirFileEnt)
typedef std::vector<DirFileEnt> vectorDF_entry;

	///vector pointers(DirFileEnt)
class vector_DFp: public std::vector<DirFileEnt*>
{
 public:

 	void del_front(size_t amount= 1);
 	void del_back();
		///will call del_front, or del_back, or ::erase
 	void delEl( size_t pos );
 	iterator at_pos( size_t pos );
 	size_t get_size() const;
 	void clear(){
		_M_erase_at_end(this->_M_impl._M_start);
		_pos_begin= 0;
 	}
 	iterator begin_f(){ return begin()+ _pos_begin; }
 protected:
	size_t _pos_begin= 0;
};
struct timeToTime
{
	int hour= 0;
	int min= 0;
	int sec= 0;
	int days= 0;

	timeToTime(time_t time_in, bool fillIfZero=false){
		days= time_in/ 86400;
		time_in= time_in% 86400;
		hour= time_in/ 3600;
		time_in= time_in% 3600;
		min= time_in/ 60;
		time_in= time_in% 60;
		sec= time_in;
	}
};
struct NumberHex32{
  //store
	bool inValid= false;
	unsigned char content[16]= {0};
  //functions
	bool operator < (const NumberHex32& other)const {return memcmp(this, &other, sizeof(NumberHex32) ) < 0;}
	bool operator > (const NumberHex32& other)const {return memcmp(this, &other, sizeof(NumberHex32) ) > 0;}
	bool operator == (const NumberHex32& other)const {return memcmp(this, &other, sizeof(NumberHex32) ) == 0;}

	unsigned char* begin() {return content;}
	const unsigned char* begin()const {return content;}
	unsigned char* end() {return content+ 16;}
	const unsigned char* end()const {return content+ 16;}
};

bool exists_file(const std::string& name);
bool exists_Wfile(LPCWSTR szPath);


	///@return char amount moved, -1 on error
size_t getDigitsAsNumber( const std::wstring& string1, const std::wstring& string2, long long* out_LL );
bool isFileNamedHex(const std::wstring& string1);
NumberHex32 getFileNamedHex_numHex32(const std::wstring& string1);
	///will return number 0-9A-Za-z, or -1(255) on fail
unsigned char get_numHex_fromChar(const wchar_t& char1, bool &O_failed);
bool compare_paths_bool(const DirFileEnt* DE_a, const DirFileEnt* DE_b );

	///@return 0 no, 1 true
bool stringEnds(const std::string& stringIn, const std::string& ends, bool CaseInsensit=true, std::string* rest=nullptr );
	///@return 0 no, 1 true
bool stringEnds(const std::string& stringIn, const vectorString& ends, bool CaseInsensit=true, std::string* rest=nullptr );

	///@return 0 no, 1 true
bool stringEnds(const std::wstring& stringIn, const std::string& ends, bool CaseInsensit=true, std::wstring* rest=nullptr );
	///@return 0 no, 1 true
bool stringEnds(const std::wstring& stringIn, const vectorString& ends, bool CaseInsensit=true, std::wstring* rest=nullptr );


class imageDirExplorer{
 public:
 	imageDirExplorer(){
		cwd_update();
 	}
 	struct fileHKeeper_list;
 	struct fileHKeeper_item{
 		fileHKeeper_item(const std::string& name_, FILE* _fh_):
 			name(name_), _fh(_fh_){}
		~fileHKeeper_item(){
			if(_fh) fclose(_fh);
		}
		const std::string& get_name()const{ return name; }
		FILE* get_file(){ time= 0; return _fh; }
		int flush_file(){ if(_fh) return fflush(_fh); return -1; }
	 protected:
	 	friend class fileHKeeper_list;
 		BYTE time= 0;
 		std::string name;
 		FILE* _fh;
 	};
 	struct fileHKeeper_list{
 		~fileHKeeper_list(){ clear(); }
			///should never give nullptr
 		fileHKeeper_item* getOrAdd(const std::string& name, std::string modeOpenOveride="a+" );
 		void clear();
 		void deleteOldItems();
 		int removeFile( const std::string& name );
 		bool deleteEntry( const std::string& name, bool flushFile=true );
 		bool closeFile( const std::string& name, bool flushFile=true ) {return deleteEntry(name, flushFile); }
	 protected:
		std::vector<fileHKeeper_item*> tableItems;
 	};
 	void cwd_update(){ cwd_my= _wgetcwd( NULL, FILENAME_MAX*2 ); }
 	const std::wstring& get_pwd()const {return cwd_my;}
 	void log_pwd();
 	const std::wstring& getCurrImage() const {return _CurrImage_stringPath;}
 	const std::wstring& getPrevImage() const {return _PrevImage_stringPath;}

		///runs all variations of `RUNDLL32.EXE user32.dll`
 	void refreshDesktop();
		///sets the last image again as Wallpaper
 	void reshowWP();
		///logs when next display is to happen
 	void whenWPChange();
		///prints what is the current WP
 	void whatWPDisplayed();
		///print Exit msg
 	void printExitMsg();
		///write time
	size_t writeTime(const std::string& fileOut);

 	configFile mainConfig;
 	configArgsContent ArgsConfig;
 	vectorDF_entry DF_list;
 	vector_DFp DF_list_p;
		///specifies png to jpeg converter path
	std::wstring _ImageConverter_exe;
	std::wstring _ImageConverter_args;
	 ///path to this.exe location (with bs)
	std::wstring _OwnPath;

		///@return 0 no error, 1 error
	BYTE getImagesFromDir(){
		DF_list.clear();
		DF_list_p.clear();
		image_p= nullptr;
		image_i= 0;
		return getImagesFromDir(L"", DF_list);
	}

	void sortBy();
	void showImageList();
	void showFullImageList();
	void imageChange(DirFileEnt* overide= nullptr);
	bool imageChangeTo(std::wstring imgName);
		///new 2019-09-23
	BYTE WaitUntilTime();
	void free_singleLog();

		///all the things that need to be done to run it.
	void prepStart( bool LoadShownImg=true );
	void rescan();
	void SavedList_remove();
		///@return Contains true/false
	static bool stringContainsUTFchars(const std::wstring& wstrIn);

 protected:
 	std::wstring cwd_my;
		///stores the path to current image
 	std::wstring _CurrImage_stringPath;
 	std::wstring _CurrImage_stringPath_notOverriden;
		///stores the path to old image
	std::wstring _PrevImage_stringPath;
		///stores the path to current imageFile that is shown
 	std::wstring _CurrRegImage;
		///@return 0 no error, 1 error
	BYTE getImagesFromDir(const std::wstring& addPath , vectorDF_entry& out_vector);
	std::time_t ttime_pulse= 0;

	void SavedList_read();
	bool SavedList_getCurrentWP( const std::string& fileName );

		///returns pos in vector or -1 when npos
	size_t ImgInList_find( const std::wstring& strImagePath );
	void SavedList_add();
	std::wstring lastItemName;
		///will remove file after 1 Sleep
	std::wstring problematicFormat_ext;

	fileHKeeper_list list_writeUtfLine;
	size_t writeUtfLine( const std::wstring& strWrite, const std::string& file_out, std::string modeOpenOveride="a+", bool quickDiscard=false );
	enum writeToMultiple_enum{
		writeTo_console= 1,
		writeTo_files= 2,
		writeTo_all= writeTo_console| writeTo_files,
	};
		///will write to wprintf(), ArgsConfig.showLogTo, ArgsConfig.showThisLogTo
	void writeToMultiple(const std::wstring& strWrite, const writeToMultiple_enum writeToWhere= writeTo_all);


		///used for `config.random=1`
	size_t image_i= 0;
		///used for `config.random=1` to determine if 1st image was shown
	BYTE image_1Shown= 0;
	DirFileEnt* image_p= nullptr;
		///stores the entry to previous img
	DirFileEnt* image_p_old= nullptr;
};



#endif
