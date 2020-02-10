//
// Created by lenovo on 2019/1/13.
//

#ifndef FINALBIGBOSS_SAVER_H
#define FINALBIGBOSS_SAVER_H

#include "Libraries.h"
#include "TextFilter.h"

#define ICONV_CONST const
//static const string UTF8 = "utf-8";
//static const string __UNICODE = "unicode";
//static const string GB2312 = "GB2312";
//static string const GBK = "GBK";
class Data{
public:
    string _hostname;
    string _content;
    string _title;
    string _info;
};

class Saver{
public:
    Data TextToData(TextFilter,Data,char *hostname);
    string ReadString(string);
    int mkpath(string);
    void saveContent(Data* &data,string path);
};
#endif //FINALBIGBOSS_SAVER_H
