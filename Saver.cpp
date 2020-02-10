//
// Created by lenovo on 2019/1/13.
//
#include "Libraries.h"
#include "Saver.h"

Data Saver::TextToData(TextFilter tf, Data dt, char *hostname) {
    dt._hostname = hostname;
    map<string, string> infos = tf.getInfos();
    dt._title = infos["title"];
    map<string, string>::iterator it;
    string infotemp;
    for (it = infos.begin(); it != infos.end(); *it++) {
        infotemp = (*it).first + ' ' + (*it).second + "\n";
    }
    dt._info = infotemp;
    dt._content = tf.getContent();
//    cout<<dt._title<<endl;
//    cout<<dt._info<<endl;
//    cout<<dt._content<<endl;
    return dt;
}

void Saver::saveContent(Data *&data,string path) {
    //string _dir = "pages/" + string(data->_hostname) + "-" + string(data->_title);
    string _dir = path + string(data->_hostname);
    //string _dir = string(data->_hostname) + "/" + string(data->_title);
recreate:
    if (access(_dir.c_str(), R_OK | W_OK | F_OK) == 0){
        cout<<_dir<<endl;
        ofstream openfile(_dir+"/"+string(data->_title), ios::app);
	cout << "title = " << data->_title << endl;
        if(openfile){
            openfile << "{"
//                 << "title:" << this->ReadString(string(data->_title)) << ","
//                 << "infomations:" << this->ReadString(string(data->_info)) << ","
//                 << "content:" << this->ReadString(string(data->_content))
//                 << "}";
                   << "title:" << data->_title << endl
                   << "infomations:" << data->_info << endl
                   << "content:" << data->_content
                   << "}";
            openfile.close();
        } else {
            cout<<"Error"<<endl;
            //this->mkpath(_dir.c_str());
        }
    }else{
    	this->mkpath(_dir.c_str());
	goto recreate;
    }
}

int Saver::mkpath(string s) {
    mode_t mode = 0755;
    size_t pre = 0, pos;
    string dir;
    int mdret;

    if (s[s.size() - 1] != '/') {
        // force trailing / so we can handle severything in loop
        s += '/';
    }

    while ((pos = s.find_first_of('/', pre)) != string::npos) {
        dir = s.substr(0, pos++);
        pre = pos;
        if (dir.size() == 0)
            continue; // if leading / first time is 0 length
        if ((mdret = ::mkdir(dir.c_str(), mode)) && errno != EEXIST) {
            return mdret;
        }
    }
    return mdret;
}

string Saver::ReadString(string buf) {
    //TODO
    return buf;
}
