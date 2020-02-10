//
// Created by lenovo on 2019/1/13.
//

#ifndef FINALBIGBOSS_FILTER_H
#define FINALBIGBOSS_FILTER_H

#include "Libraries.h"
class TextFilter{
private:
    string content;
    vector<string> lines;
    vector<string> urls;
    map<string,string> info;
public:
    void ParseHTML(char *);
    void DisplayVector();
    void DisplayMap();
    void ReplaceScriptandStyle(char *);
    void GetURLsandInfo(char *);
    char *ReplaceLabelandGetText(char *);
    void CalculateText();
    void Process(char *);
    string getContent();
    vector<string> getURLs();
    map<string,string> getInfos();
};
#endif //FINALBIGBOSS_FILTER_H
