//snap_MultiSz.h
#ifndef SNAP_MULTISZ_INCLUDE
#define SNAP_MULTISZ_INCLUDE

BOOL isStrInMulti(LPCTSTR str, TCHAR * multi_sz);
void setMultiSz(TCHAR * target,TCHAR * source,int max_item_length,int max_total_length);
BOOL is_name_in_classlist(LPCTSTR str,LPCTSTR class_list);

#endif