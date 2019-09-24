

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
//		BYTE isDigits_for= isDigits( c_a ) | (2*isDigits( c_b ));

		if( option_asNumbers ){
		  BYTE isDigits_for= isDigits( c_a ) | (2*isDigits( c_b ));
		  if( isDigits_for ){
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



size_t imageDirExplorer::sellectImgInList( const std::wstring& strImagePath )
{
	DirFileEnt temp_findEnt( strImagePath );
		///get pos of last \\ in path
	size_t PosLastSl= strImagePath.find_last_of( L"\\" );
	if( PosLastSl!= strImagePath.npos ){
		temp_findEnt.name= strImagePath.substr( PosLastSl+1 );
		temp_findEnt.path= strImagePath.substr( 0, PosLastSl );
	}

//	vector_DFp::iterator thatIt= std::lower_bound( DF_list_p.begin(), DF_list_p.end(), &temp_findEnt, compare_paths_bool );
//	if( thatIt!= DF_list_p.end() && (*thatIt)->getPathName()== temp_findEnt.getPathName() ){
//		size_t ret_valDelEl= ( thatIt- DF_list_p.begin() );
//
//		DF_list_p.erase( thatIt );
//		return ret_valDelEl;
//	} else {
//		return -1;
//	}

	vector_DFp::iterator thatIt= std::lower_bound( DF_list_p.begin_f(), DF_list_p.end(), &temp_findEnt, compare_paths_bool );
	if( thatIt!= DF_list_p.end() && (*thatIt)->getPathName()== temp_findEnt.getPathName() ){
		size_t ret_valDelEl= ( thatIt- DF_list_p.begin_f() );
//printf("ret_valDelEl = %i\n", ret_valDelEl);
//		DF_list_p.erase( thatIt );
		DF_list_p.delEl( ret_valDelEl );
		return ret_valDelEl;
	} else {
		wprintf( L"List has, no exist |%s|\n", strImagePath.c_str() );
		return -1;
	}


//	if( image_p && image_p->getPathName(false) == strImagePath ) return image_i;
//	size_t temp_u= 0;
//	size_t tempEnd= DF_list_p.get_size();
//
//	for( ; temp_u< tempEnd; ++temp_u){
////		DirFileEnt* currEnt= &*DF_list_p.at_pos( temp_u );
//		DirFileEnt* currEnt= *DF_list_p.at_pos( temp_u );
//		if( currEnt->getPathName(false) == strImagePath ){
//			lastItemName= currEnt->getPathName(false);
//			DF_list_p.delEl( temp_u );
//			return temp_u;
//		}
//	}
return -1;
}





