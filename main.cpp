#include "TextFilter.h"
#include "Saver.h"
#include "Spider.h"

int main(int argc, char **argv) {
    /*char *filepath = (char*)"pages/ifeng1.txt";
    TextFilter tf;

    ifstream in;
    in.open(filepath);
    if (!in) {
        cout << "Error!" << endl;
        return;
    }
    in.seekg(0, ios::end);
    int length = in.tellg();
    in.seekg(0, ios::beg);
    char *buffer;
    buffer = new char[length];
    in.read(buffer, length);
    in.close();
    tf.ParseHTML(buffer);*/

//    tf.Process(filepath);
    /*Saver sv;
    Data dt,*dt_p;
    char hostname[30] = "news.ifeng.com";
    dt = sv.TextToData(tf, dt, hostname);
    dt_p = &dt;
    sv.saveContent(dt_p);*/
    //string configfile = "D:\\UnixExperiencesProgramming\\finalbigboss\\host.conf";
	string configfile = "host.conf";
    unsigned int size = VECTORSIZE;
    Spider spider;
    spider.initFromFile(configfile,size);
    spider.connectServer();
    return 0;
}

