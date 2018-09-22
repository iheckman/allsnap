#include "stdafx.h"
#include "snap_lib_internal.h"
#include "snap_MultiSz.h"

void setMultiSz(TCHAR * p_target,TCHAR * p_source,int max_item_length,int max_total_length){
	int count = 0;

	if (!(*p_target)){
		p_target[0] = _T('\0');
		p_target[1] = _T('\0');
		return;
	}

	while ( *p_source && (count < max_total_length)){
		int coppied_length;
		TCHAR * just_coppied;

		just_coppied = lstrcpyn(p_target,p_source,max_item_length);

		coppied_length = lstrlen(just_coppied) + 1;
		
		p_target += coppied_length;
		count  += coppied_length;

		p_source += lstrlen(p_source) + 1;
	}

	*p_target = _T('\0'); //double null at the end

}

BOOL isStrInMulti(LPCTSTR str, TCHAR * multi_sz){
	for(multi_sz; *multi_sz; multi_sz += lstrlen(multi_sz)+1){
		if ((lstrcmp(str,multi_sz)==0)){
			return TRUE;
		}
	}
	return FALSE;
}

BOOL is_name_in_classlist(LPCTSTR str,LPCTSTR class_list){
  TCHAR * pch;
  TCHAR * next_token;
  TCHAR buffer[MAX_CLASSNAME_LIST_LENGTH + 2];
  lstrcpyn(buffer,class_list,MAX_CLASSNAME_LIST_LENGTH);
  buffer[MAX_CLASSNAME_LIST_LENGTH] = 0;

 
  pch = _tcstok_s(buffer,_T(";"),&next_token);
  while (pch != NULL)
  {
	if ((lstrcmp(str,pch)==0)){
		return TRUE;
	}
    pch = _tcstok_s(NULL,_T(";"),&next_token);
  }
  return FALSE;
}