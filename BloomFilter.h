//
// Created by lenovo on 2019/1/13.
//

#ifndef FINALBIGBOSS_BLOOMFILTER_H
#define FINALBIGBOSS_BLOOMFILTER_H

#include "Libraries.h"

#define GROUPNUM 23              // 哈希函数的个数 (错误率取 0.00001%）
#define VECTORSIZE 6709540     // 每个空间的大小

//填满字节的位域结构
typedef struct bloomsingle {
    unsigned char a : 1;
    unsigned char b : 1;
    unsigned char c : 1;
    unsigned char d : 1;
    unsigned char e : 1;
    unsigned char f : 1;
    unsigned char g : 1;
    unsigned char h : 1;
} bit, *pbit;

class BloomFilter {
private:
    pbit hash[GROUPNUM];  //定义一个长度为GROUPNUM=17的w位数组，数组的每个元素是一个pbit指针类型，也就是一个bit的地址
    unsigned int size;

    //超级哈希，参数seed确定具体哈希函数
    unsigned int MurmurHash2(const void *key, int len, unsigned int seed);

public:
    int getBitNumber(bit b, int pos);//获取bit结构中的pos位的值
    bit setBitNumber(bit b, int num, int pos);//将bit中pos位的值设为num
    void add(char *line);//将字符串line加入哈希表对应关键码位置中
    void create(unsigned int size);//创建一个大小为size的空间，返回携带空间指针的bf
    int search(char line[]);//在bf中查找字符串line，存在返回1，否则返回0

};


#endif //FINALBIGBOSS_BLOOMFILTER_H
