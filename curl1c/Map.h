// CMap.h
#pragma once

#include "Common.h"
#include "../curl/include/curl/curl.h"

class CMap : public CContextImpl<CMap> {

  public:

    CMap(){
        DoMsgLine("constructor CMap");

    };
    virtual ~CMap(){
        DoMsgLine("destructor CMap");

    };


    BL_BEGIN_CONTEXT("Map", "Соответствие");



//Вставить (Insert)
//Количество (Count)
//Очистить (Clear)
//Получить (Get)
//Удалить (Delete)

	BL_PROC_WITH_DEFVAL(Insert, "Вставить", 1)
	{
		CString txt=ppParams[0]->GetString();
		if(!txt.IsEmpty())
			AfxMessageBox(txt);
		return TRUE;
	}
	BL_DEFVAL_FOR(DefTest)
	{
		if(nParam==0)
		{
			if(pValue)
				*pValue="Hello World!!!";
			return TRUE;
		}
		return FALSE;
	}


    BL_END_CONTEXT();

  private:
    CString ResourceAddress;
};
