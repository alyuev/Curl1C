// HTTPRequest.h
#pragma once

#include "Common.h"
#include "../curl/include/curl/curl.h"

class CHTTPRequest : public CContextImpl<CHTTPRequest> {

  public:

    CHTTPRequest(){
        DoMsgLine("constructor HTTPЗапрос");

    };
    virtual ~CHTTPRequest(){
        DoMsgLine("destructor HTTPЗапрос");

    };


    BL_BEGIN_CONTEXT("HTTPRequest", "HTTPЗапрос");

    BL_PROP_RW(ResourceAddress, "АдресРесурса") {
        value = ResourceAddress;
        return TRUE;
    }
    BL_PROP_WRITE(ResourceAddress) {
        ResourceAddress = value.GetString();
        return TRUE;
    }

    BL_PROP_RW(Headers, "Заголовки") {
        //value = Encoding;
        return TRUE;
    }
    BL_PROP_WRITE(Headers) {
        //Encoding = value.GetNumeric();
        return TRUE;
    }
    BL_END_CONTEXT();

  private:
    CString ResourceAddress;
};
