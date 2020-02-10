//
// Created by lenovo on 2019/1/13.
//

#ifndef FINALBIGBOSS_SPIDER_H
#define FINALBIGBOSS_SPIDER_H

#include "Libraries.h"
#include "BloomFilter.h"

#define MAX_BUFLEN 4096
#define MAXIPNUM 10
#define MAX_HTML_LEN 409600
#define MAX_QUELEN 500

extern void deamonize(const char* cmd);

typedef BloomFilter processed_t;//为了方便更改成其他结构，修改时只需改这里，不需改下面具体的代码

void splitString(const string &s, vector<string> &v, const string &c);

class Spider {
private:
    const char *hostname;//服务器hostname
    string hnstr;//用来存储从文件中读出来的string类型，因为const char*不能被改变，又因为在类内不能使其在声明时定义，所以用一个string来转，或者用全局变量/上层函数的变量来中转。hnstr也必须是类的成员变量，因为c_str不会给hostname重新分配空间，hostname还是只想原来hnstr的空间，所以hnstr不能在这个函数内定义，不能让这片空间被回收了
    string firsturl;//初始url
    int sockfdc;//本机与服务器链接的sockfd
    queue<string> unprocessed;//未处理的队列
    BloomFilter processed;//已处理的hash表
    BloomFilter unprocbf;//未处理的hash表
    int cnt;//计数
    string scope;//要存储的网页url限定
    bool reconnect;//标记是否要重新连接
    string pages_path;
public:
    void initFromFile(string filepath, unsigned int size);

    void init(unsigned int size, const char *hn, string url, string scope);

    void connectServer();

    void constructHttpRequest(string &head, string url);

    int getOnePage(string url, string &html);
    //string getOnePage(string url, int &len);
    //void getOnePage(string url, string &html);
    //string getOnePage(string url);

    int getResponceLen(string recvdata);

    void savePage(string &recvdata);

    void traverseUrls();

    void findUrls(string str, queue<string> &unprocessed, BloomFilter &processed);
    void newUrlsEnque(vector<string> urls,queue<string> &unprocessed, BloomFilter &processed);
};

#endif //FINALBIGBOSS_SPIDER_H
