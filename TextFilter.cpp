//
// Created by lenovo on 2019/1/13.
//

#include "TextFilter.h"

//工具函数，查看vector
void TextFilter::DisplayVector() {
    vector<string>::iterator it;
    for (it = this->lines.begin(); it != this->lines.end(); *it++) {
        cout << *it << endl;
    }
    return;
}

//工具函数，查看map
void TextFilter::DisplayMap() {
    map<string, string>::iterator it;
    for (it = this->info.begin(); it != this->info.end(); *it++) {
        cout << (*it).first << ' ' << (*it).second << endl;
    }
    return;
}

//去除html里面的<script>和<style>
void TextFilter::ReplaceScriptandStyle(char *html) {
    bool flag = false;
    char script_begin[8] = "<script";
    char script_end[10] = "</script>";
    char style_begin[7] = "<style";
    char style_end[9] = "</style>";
    char comment_begin[5] = "<!--";
    char comment_end[4] = "-->";
    char space[6] = "&nbsp";
    char *p = html;
    string prohtml;
    while (*(p + 1) != '\0') {
        if (!flag) {
            if ((strncmp(p, script_begin, 7) == 0) || strncmp(p, style_begin, 6) == 0
                || strncmp(p, comment_begin, 4) == 0) {
                flag = true;
            } else {
                if (strncmp(p, space, 5) == 0) {
                    p = p + 5;
                }
                char t = *p;
                prohtml += t;
            }
        }
        if (flag) {
            if (strncmp(p, script_end, 9) == 0) {
                flag = false;
                p = p + 9;
            } else if (strncmp(p, style_end, 8) == 0) {
                flag = false;
                p = p + 8;
            } else if (strncmp(p, comment_end, 3) == 0) {
                flag = false;
                p = p + 3;
            }
        }
        p++;
    }
    this->content = prohtml;
}

//获取网页里的URL和其它信息
void TextFilter::GetURLsandInfo(char *html) {
    char *p = html;
    char http[8] = "http://";
    char meta[6] = "<meta";
    char name[6] = "name=";
    char httpequip[12] = "http-equiv=";
    char content[9] = "content=";
    char property[10] = "property=";
    char title[8] = "<title>";
    string url_temp;
    string time_temp;
    string title_temp;
    string author_temp;
    string charset_temp;
    string key;
    string value;
    while (*(p + 1) != '\0') {
        if (strncmp(p, title, 7) == 0) {
            p = p + 7;
            while (*p != '<' && *(p + 1) != '\0') {
                value += *p;
                p++;
            }
            this->info["title"] = value;
			cout << "title = " << this->info["title"] << endl;
            value.clear();
        }
        if ((strncmp(p, http, 7) == 0)) {
            while (*p != '\"' && *p != '\'' && *(p + 1) != '\0') {
                url_temp += *p;
                p++;
            }
            cout <<"url_temp = " << url_temp << endl;
            this->urls.push_back(url_temp);
            url_temp.clear();
        }
        if (strncmp(p, meta, 5) == 0) {
            p = p + 5;
            bool flag = false;
            while (*p != '>' && *(p + 1) != '\0') {
                //cout << *p;
                if (strncmp(p, name, 5) == 0) {
                    p = p + 6;
                    flag = true;
                    while (*p != '\"' && *p != '"' && *(p + 1) != '\0') {
                        key += *p;
                        p++;
                    }
                }
                if (strncmp(p, httpequip, 11) == 0) {
                    p = p + 12;
                    flag = true;
                    while (*p != '\"' && *p != '"' && *(p + 1) != '\0') {
                        key += *p;
                        p++;
                    }
                }
                if (strncmp(p, property, 9) == 0) {
                    p = p + 10;
                    flag = true;
                    while (*p != '\"' && *p != '"' && *(p + 1) != '\0') {
                        key += *p;
                        p++;
                    }
                }
                if (strncmp(p, content, 8) == 0 && flag) {
                    p = p + 9;
                    while (*p != '\"' && *p != '"' && *(p + 1) != '\0') {
                        value += *p;
                        p++;
                    }
                }
                p++;
            }
            if (flag) {
                this->info[key] = value;
				//cout << "key = " << key << " value = " << value << endl;
                key.clear();
                value.clear();
            }
        }
        p++;
    }

/*	cout << endl << endl << "traverse map " << endl;
	map<string,string>::iterator itr = this->info.begin();
	while(itr != this->info.end()){
		cout << "key = " << itr->first << endl;
		cout << "value = " << itr->second << endl << endl;
		itr++;
	}
	cout << "end of traverse map" << endl;*/
    return;
}

//去除标签获取文字
char *TextFilter::ReplaceLabelandGetText(char *html) {
    char *p = html;
    bool flag = false;
    bool flag2 = false;
    string prohtml;
    string line_temp;
    while (*(p + 1) != '\0') {
        if (!flag) {
            if (*p == '<') {
                flag = true;
            } else {
                char t = *p;
                if (*p != '\n' && *p != ' ' && *p != '\r') {
                    line_temp += t;
                    flag2 = true;
                }
                if ((*p == '\n' || *p == '\r') && flag2) {
                    this->lines.push_back(line_temp);
                    line_temp.clear();
                    flag2 = false;
                }
                prohtml += t;
            }
        }
        if (flag) {
            if (*p == '>') {
                flag = false;
            }
        }
        p++;
    }
    return (char *) prohtml.c_str();
}

//计算正文位置并获取正文
void TextFilter::CalculateText() {
	cout << "CalculateText..." << endl;
    vector<string>::iterator it;
    int cnt;
    int block_width = 3;
    int min_length = 1000;
    int max_length = -1;
    this->content.clear();
	//this->content.reserve(500);
    for (it = this->lines.begin(), cnt = 0; it != this->lines.end(); *it++, cnt++) {
        int length = 0;
        length = (int) (*it).length();
        min_length = length < min_length ? length : min_length;
        max_length = length > max_length ? length : max_length;
    }
    int threshold = (int) (max_length + min_length) / 1.8;
    int i;
    for (i = 0; i < cnt - 1; i++) {
        if (this->lines[i].length() > threshold) {
            while (this->lines[i].length() > threshold) {
				try{
					if(this->content.length()+this->lines[i].length()>this->content.max_size()) break;
		            this->content += this->lines[i];
					cout << "content = " << content << endl << endl;
				}
				catch(const length_error& e){
					break;
				}
                i++;
            }
        }
    }
    return;
}

//完整解析html
void TextFilter::ParseHTML(char *html) {
	cout << "ParseHTML html...." << endl;
    this->ReplaceScriptandStyle(html);
	cout << "ReplaceScriptandStyle success...." << endl;
    char *temphtml = (char *) (this->content.c_str());
    this->GetURLsandInfo(temphtml);
	cout << "GetURLsandInfo success...." << endl;
    this->content = (string) this->ReplaceLabelandGetText(temphtml);
	cout << "ReplaceLabelandGetText success...." << endl;
    this->CalculateText();
	cout << "CalculateText success...." << endl;
    return;
}

//目前入口：从已有文件中读取html
void TextFilter::Process(char *filepath) {
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
	delete(buffer);
    this->ParseHTML(buffer);
    return;
}

//类私有成员接口
string TextFilter::getContent() {
    return this->content;
}

//类私有成员接口
vector<string> TextFilter::getURLs() {
    return this->urls;
}

//类私有成员接口
map<string, string> TextFilter::getInfos() {
    return this->info;
}
