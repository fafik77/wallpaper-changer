#ifndef FILEFINDER_H
#define FILEFINDER_H


#include <algorithm> // transform
#include <ctime>	// time

#include <stdlib.h>	// qsort
//#include <cstdio>	// printf
#include "configIO.h"
#include <direct.h>	//cwd set, get


	///contains path and fileName, with custom sort
struct DirFileEnt{
	DirFileEnt(	const std::wstring& name_, const std::wstring& path_=L"" );
	DirFileEnt();
		///file name
	std::wstring name;
	bool is_number= 0;
		///path to file
	std::wstring path;
	int sortPos= 0;
	byte selected= 0;

		///return name
	std::wstring getName(bool afterNumber=true)const;
		///return path+name
	std::wstring getPathName(bool afterNumber=false)const;

};

	///vector(DirFileEnt)
typedef std::vector<DirFileEnt> vectorDF_entry;

	///vector pointers(DirFileEnt)
//class vector_DFp: public std::vector<vectorDF_entry::iterator>
class vector_DFp: public std::vector<DirFileEnt*>
{
 public:

 	void del_front();
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


bool exists_file(const std::string& name);
bool exists_Wfile(LPCWSTR szPath);


bool isDigits( wchar_t char_1, wchar_t char_2 );
	///@return char amount moved, -1 on error
size_t getDigitsAsNumber( const std::wstring& string1, const std::wstring& string2, long long* out_LL );
bool compare_paths_bool(const DirFileEnt* DE_a, const DirFileEnt* DE_b );

	///@return 0 no, 1 true
bool stringEnds(const std::string& stringIn, const std::string& ends, bool CaseInsensit=true, std::string* rest=nullptr );
	///@return 0 no, 1 true
bool stringEnds(const std::string& stringIn, const vectorString& ends, bool CaseInsensit=true, std::string* rest=nullptr );

	///@return 0 no, 1 true
bool stringEnds(const std::wstring& stringIn, const std::string& ends, bool CaseInsensit=true, std::wstring* rest=nullptr );
	///@return 0 no, 1 true
bool stringEnds(const std::wstring& stringIn, const vectorString& ends, bool CaseInsensit=true, std::wstring* rest=nullptr );


	///@return 0 no, 1 true
bool stringBegins(const std::string& stringIn, const std::string& begins, bool CaseInsensit=true, std::string* rest=nullptr );
bool stringBegins(const std::wstring& stringIn, const std::wstring& begins, bool CaseInsensit=true, std::wstring* rest=nullptr );
const time_t dhms_time= (60*60*24);


class imageDirExplorer{
 public:
 	configFile mainConfig;
 	configArgsContent ArgsConfig;
 	vectorDF_entry DF_list;
 	vector_DFp DF_list_p;
		///specifies png to jpeg converter path
	std::wstring _ImageConverter_exe;
	std::wstring _ImageConverter_args;
	std::wstring _OwnPath;
	vectorString _ImageExtProblematic;

		///@return 0 no error, 1 error
	BYTE getImagesFromDir(){
		DF_list.clear();
		DF_list_p.clear();
		image_p= nullptr;
		image_i= 0;
		return getImagesFromDir(L"", DF_list);
	}

	void sortBy();
	void iterateImages();
	void showImageList();

		///all the things that need to be done to run it.
	void prepStart();
	void SavedList_remove();

 protected:
		///@return 0 no error, 1 error
	BYTE getImagesFromDir(const std::wstring& addPath , vectorDF_entry& out_vector);
	BYTE WaitTime();
	void imageChange();
	void SavedList_read();
	bool SavedList_getCurrentWP( const std::string& fileName );

		///returns pos in vector or -1 when npos
	size_t ImgInList_find( const std::wstring& strImagePath );
	void SavedList_add();
	std::wstring lastItemName;

	size_t writeUtfLine( const std::wstring& strWrite, const std::string& file_out, std::string modeOpenOveride="a+" );

	class readUtfFile{
	 public:
		readUtfFile();
		~readUtfFile();

		size_t readLine( std::wstring& out_Line );
			///1 ok, 0 fail
		bool Open(const std::string& open_file= "BGchanger_List.cfg");
			///@return close error code
		int Close();
	 protected:
		size_t ReadOnce_size= 4096;
		std::wstring ReadOnce_left;
		FILE* io_file= nullptr;

			///appends string
		size_t readTo( std::wstring& out_read );
	};

		///used for `config.random=1`
	size_t image_i= 0;
		///used for `config.random=1` to determine if 1st image was shown
	BYTE image_1Shown= 0;
	DirFileEnt* image_p= nullptr;
};



#endif
