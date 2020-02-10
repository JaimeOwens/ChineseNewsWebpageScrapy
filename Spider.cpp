//
// Created by lenovo on 2019/1/13.
//
#include "Spider.h"
#include "TextFilter.h"
#include "Saver.h"

/*=================================
 *Spider类
 *init:初始化：设置所需的url hash表，主机名和初始url
 *params:unsigned int size：hash表大小
 *		const char *hn:主机名
 *		string url:初始url
 *return:void
 *=================================*/
void Spider::init(unsigned int size, const char *hn, string url, string scope) {
    processed.create(size);//创建已处理的url的hash表
    unprocbf.create(size);//创建未处理的url的hash表
    hostname = hn;//设置主机名
    firsturl = url;//设置首个访问的url
    this->scope = scope;//设置url范围
	reconnect = false;
    cnt = 0;
}

void Spider::initFromFile(string filepath, unsigned int size) {
    map<string, string> configmap;
    ifstream infile;

    infile.open(filepath, ios::app);
    if (!infile.is_open())
        cout << "Open file failure" << endl;
    while (!infile.eof()) {
        string key, value;
        infile >> key >> value;//读入一行
        configmap[key] = value;//放入字符串键值对里
        configmap.insert(pair<string, string>(key, value));
    }
    infile.close();

    hnstr = configmap["hostname"];
    firsturl = configmap["firsturl"];
    scope = configmap["scope"];
    pages_path = configmap["pages_path"];
    hostname = hnstr.c_str();//hnstr也必须是类的成员变量，因为c_str不会给hostname重新分配空间，hostname还是只想原来hnstr的空间，所以hnstr不能在这个函数内定义，不能让这片空间被回收了

    processed.create(size);//创建已处理的url的hash表
    unprocbf.create(size);//创建未处理的url的hash表
	deamonize("spider");
}

/*=================================
 *Spider类
 *connectServer:连接主机
 *params:
 *return:void
 *=================================*/
void Spider::connectServer() {
    //根据域名取server的ip
    tag_connectstart:
	struct hostent *he;
    if ((he = gethostbyname(hostname)) == NULL) {
        cout << "hostname = '" << hostname << "'" << endl;
        cout << "firsturl = '" << firsturl << "'" << endl;
        cout << "scope = '" << scope << "'" << endl << endl;
        cout << "can not get host addr: " << hostname << endl;
        return;
    }

    //建立socket
    sockfdc = socket(he->h_addrtype, SOCK_STREAM, 0);
    if (sockfdc < 0) {
        cout << "fail to create socket!" << endl;
        return;
    }

    //构造服务器地址
    struct sockaddr_in server_addr;
    server_addr.sin_family = he->h_addrtype;
    server_addr.sin_port = htons(80);

    int i = 0;
    char addr_str[INET_ADDRSTRLEN];
    while (he->h_addr_list[i] != NULL && i < MAXIPNUM) {
        inet_ntop(he->h_addrtype, he->h_addr_list[i], addr_str, INET_ADDRSTRLEN);
        inet_pton(he->h_addrtype, addr_str, &server_addr.sin_addr);
        cout << "addr str = " << addr_str << endl;
        printf("addr_list[0]: %s\n", he->h_addr_list[0]);

        //链接服务器
        socklen_t alen = sizeof(server_addr);
        if ((connect(sockfdc, (sockaddr *) &server_addr, alen)) == -1) {
            cout << "fail to connect to server" << endl;
            //return;
            break;
        }
        cout << "---------success to connect to server---------" << endl;

        traverseUrls();//遍历url
        i++;
    }
    close(sockfdc);
	if(this->reconnect = true){
		reconnect = false;
		goto tag_connectstart;
	}
}

/*=================================
 *Spider类
 *traverseUrls:遍历url，取整个页面，取页面中所有的url，新的url进入队列
 *params:
 *return:void
 *=================================*/
void Spider::traverseUrls() {
    unprocessed.push(firsturl);//初始url入队
	int empty_responce_cnt = 0;
    while (!unprocessed.empty()) {
        string url = unprocessed.front();//取队头
        unprocessed.pop();//队头出队
		cout << "====== processing url: " << url << " ======="<< endl;

		/*try{
	        char *urlc = new char[url.length() + 1];
		    url.copy(urlc, url.length(), 0);	//(目的char*,复制长度，起始位置)
		    *(urlc + url.length()) = '\0';		//结束符
		    //processed.add(urlc);//当前url加入到已处理的url中
			delete(urlc);
		}
		catch(const bad_alloc& e) {
			cout << "========= !lack of memory! ===========" << endl;
			continue;
		}*/

        string html;
        int len;
		len = getOnePage(url, html);//处理队头
		if(len==0){
			++empty_responce_cnt;
			if(empty_responce_cnt>3){
				firsturl = url;
				reconnect = true;
				return;
			}
			continue;
		}

        //savePage(html);
        //findUrls(html, unprocessed, processed);//新的url进队

		//处理正文及属性信息
		TextFilter tf;
		try{
			char *htmlc = new char[html.length() + 1];
		    html.copy(htmlc, html.length(), 0);//(目的char*,复制长度，起始位置)
		    *(htmlc + html.length()) = '\0';//结束符

			//cout << "trans html string to char * success" << endl;



			tf.ParseHTML(htmlc);
			//cout << "process html success" << endl;
			newUrlsEnque(tf.getURLs(), unprocessed, processed);//新的url进队
			delete(htmlc);//?待商议
			//存储
			Saver sv;
			Data dt,*dt_p;
			try{
				char * hostname2 = new char[strlen(hostname)];
				strcpy(hostname2,hostname);
				dt = sv.TextToData(tf, dt, hostname2);
				dt_p = &dt;
				sv.saveContent(dt_p,this->pages_path);
				delete(hostname2);
			}
			catch(const bad_alloc& e) {
				cout << "========= !lack of memory! ===========" << endl;
				savePage(html);
				continue;
			}
		}
		catch(const bad_alloc& e) {
			cout << "========= !lack of memory! ===========" << endl;
			savePage(html);
			continue;
		}
//		cout << "save html success" << endl;
    }
}

void Spider::newUrlsEnque(vector<string> urls,queue<string> &unprocessed, BloomFilter &processed){
	vector<string>::iterator ite = urls.begin();
	while(ite!=urls.end()){
		string url = *ite;
		cout << "url = " << url << endl;

        //验证是否已经处理了，未处理的放入未处理队列
		try{
	        char *urlc = new char[url.length() + 1];
		    url.copy(urlc, url.length(), 0);//(目的char*,复制长度，起始位置)
		    *(urlc + url.length()) = '\0';//结束符

			unsigned long int scoperesult = url.find(scope);
			if (processed.search(urlc) == 0 && scoperesult >= 0 && scoperesult<=url.length() ) {//&& unprocessed.size()<MAX_QUELEN
		        unprocessed.push(url);
		        processed.add(urlc);
		    	cout << "enqueue url = " << url << endl;
		    }
			delete(urlc);
		}
		catch(const bad_alloc& e) {
			cout << "========= !lack of memory! ===========" << endl;
			continue;
		}

		ite++;
	}
}


/*=================================
 *Spider类
 *getOnePage:发送http request头，接收responce
 *params:string url 请求的url
 *return:void
 *=================================*/
int Spider::getOnePage(string url, string &html) {
	//string html;
    //构造http头
    string head;
    constructHttpRequest(head, url);
    //cout << "--------------head-----------------" << endl << head<< endl;

    //send request to server
    ssize_t translen = send(sockfdc, head.c_str(), head.length(), 0);
    if (translen < head.length()) {
        cout << "fail to send http head to server" << hostname << endl;
        html = "";
        return 0;
    }
    //cout << "finish to send http head,waiting for response" << endl;

    //recv responce from server
    html = "";
    char buf[MAX_BUFLEN];
    buf[MAX_BUFLEN] = '\0';
    translen = 0;
    int recvtotallen = 0;
    int totallen = 0;
    cout << "------ start receiving from one page ------" << endl;

	struct timeval timeout = {0, 500};
    setsockopt(sockfdc, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));



    while (translen <= MAX_BUFLEN && recvtotallen <= totallen) {
        translen = recv(sockfdc, buf, MAX_BUFLEN, 0);
//        translen = recv(sockfdc, buf, MAX_BUFLEN, MSG_DONTWAIT);
        cout << endl;
        cout << "  ---- recv from server " << translen << " ----" << endl;
        //cout << "=====totallen = " << totallen << "=============" << endl;
        //cout << "=====recvtotallen = " << recvtotallen << "=========" << endl;
        if (translen <= 0) {
            cout << "  --- receive nothing from server ---" << hostname << endl;
            break;
        } else {
            html += buf;
			//cout << "  " << buf << endl<< endl;
            memset(buf, '\0', sizeof(buf) / sizeof(char));//清空buf
            if (recvtotallen == 0) {//http响应头
                totallen = getResponceLen(html);//处理响应头，取响应文本的长度
            }
            recvtotallen += translen;
		    cout << "  ---- cal receiving "<< recvtotallen << "------" << endl;
			cout << "  ---- total len = "<< totallen <<"-----"<< endl;
        }
    }
    cout << "---- stop receiving ------" << endl;
	cout << "---- total len = "<< totallen <<"-----"<< endl;
	cout << "---- recvtotallen = "<< recvtotallen <<" ----" << endl;
	//len = totallen;
	return totallen;
}

/*=================================
 *Spider类
 *findUrls:用正则表达式寻找href后的url
 *params:string str 整篇文章的html
 *		queue<string> &unprocessed 未处理的url队列unprocessed
 *		BloomFilter &processed 已处理的url的hash表
 *return:void
 *=================================*/
void Spider::findUrls(string str, queue<string> &unprocessed, BloomFilter &processed) {
    string endmark, url;
    int startpos, endpos;

	regex reg("(https?:)?//[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|\u4e00-\u9fa5]+");
/*regex reg(
            "<a\\s*(href|HREF)\\s*[=]\\s*(\u0022|\u0027)(https?://)?[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|\u4e00-\u9fa5]+(\u0022|\u0027)");*/

    sregex_iterator ite(str.begin(), str.end(), reg);
    sregex_iterator end;

    for (; ite != end; ++ite) {//遍历用正则找到的字符串列表href="url"或href='url'
        url = ite->str();

		startpos = url.find("'", 0) + 1;//url起始位置为引号后一位
        if (startpos <= 0) {//没找到单引号
            startpos = url.find("\"") + 1;//找双引号
            endmark = "\"";//则结束符也是双引号
        } else endmark = "'";//找到单引则结束符也是单引
        endpos = url.find(endmark, startpos);//从引号后一位开始找下一个结束符
        url = url.substr(startpos, endpos - startpos);//引号后一位到引号前一位都是url


        //验证是否已经处理了，未处理的放入未处理队列
		try{
	        char *urlc = new char[url.length() + 1];
		    url.copy(urlc, url.length(), 0);//(目的char*,复制长度，起始位置)
		    *(urlc + url.length()) = '\0';//结束符

			unsigned long int scoperesult = url.find(scope);

		    if (processed.search(urlc) == 0 && unprocbf.search(urlc) == 0 && scoperesult >= 0 && scoperesult<=url.length()) {

		        unprocessed.push(url);

		        unprocbf.add(urlc);
		    	cout << "enqueue url = " << url << endl;
		    }
			delete(urlc);
		}
		catch(const bad_alloc& e) {
			cout << "========= !lack of memory! ===========" << endl;
			continue;
		}
    }
	cout << "end enque" << endl;
}

/*=================================
 *Spider类
 *savePage:存储一篇完整的网页
 *params:string &recvdata 网页内容字符串
 *return:void
 *=================================*/
void Spider::savePage(string &recvdata) {
    string filename;

    filename += "pages/testfile" + to_string(cnt) + ".content";

//    filename += "D:\\UnixExperiencesProgramming\\finalbigboss\\pages\\testfile" + to_string(cnt) + ".content";
    cnt++;
    cout<<filename<<endl;
    ofstream outfile(filename.c_str());//创建输入流
    if (!outfile) {
        cout << "fail to create a file ";
        return;
    }
    outfile << recvdata;
    outfile.close();
}


/*=================================
 *Spider类
 *constructHttpRequest:构造http请求头
 *params:string &head 可修改的空字符串
 *		string url 请求的url
 *return:void
 *=================================*/
void Spider::constructHttpRequest(string &head, string url) {
    //一般头部
    head = "GET ";
    head.append(url);//url
    head.append(" HTTP/1.1\r\n");
    head.append("Host: ");
    head.append(hostname);//请求的域名
    head.append("\r\n");
    head.append("Accept: text/content,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n");
    head.append(
            "User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.120 Safari/537.36\r\n");
    head.append("Accept-Language: zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n");
    head.append("Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7");
    head.append("Accept-Encoding: gzip,deflate\r\n");
    head.append("\r\n");//表明请求头结束了

}
/*=================================
 *Spider类
 *getResponceLen:提取responce头中返回的总字符数
 *params:string recvdata:响应头字符串
 *return:int:字符数量
 *=================================*/
int Spider::getResponceLen(string recvdata) {
    int datalen = 0;
	int findflag = 0;
    //split string by \n
    vector<string> strvec;
    splitString(recvdata, strvec, "\r\n");
    for (int i = 0; i < strvec.size(); i++) {
        size_t fi = strvec[i].find("Length", 0);
        if (fi == strvec[i].npos)
            continue;
		findflag = 1;
        vector<string> lenvec;
        splitString(strvec[i], lenvec, ":");
        datalen = atoi(lenvec[1].c_str());
    }
	if(datalen == 0) return MAX_HTML_LEN;
    return datalen;
}
