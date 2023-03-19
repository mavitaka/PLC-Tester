/****************************************************************************/
#ifndef TRAME_H
#define TRAME_H
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <stdlib.h>
#include <QPlainTextEdit>

using namespace std;

class Transmission
{
public:

   // char* prod_trame(void);
    Transmission(char cmd,char* dat)
    {
       this->command=cmd;
       this->data=dat;


       // qDebug( "the integer is: %d",length);

    }

    char cheksum(char* data)
    {
        char *pString;
          uint16_t  cksum;
          uint8_t ck;
          pString = data;
          cksum = 0;

          while (*pString != '\0')
          cksum += (int)(*pString++) - '0';
          ck = cksum % 8;
          return((char)(ck));
        qDebug("the cheksum is: %d", ck);
    }

    char* prod_trame()
    {

    //char tab_trame[255];
    //for(int i=0;i<255;i++)
    //tab_trame[i]='\0';
   // char* trame=tab_trame;
char* trame=(char*)calloc((strlen(data)+4),sizeof(char));
//char j ='1';
//trame=(char*)calloc(sizeof(char)*(strlen(data)+1));
*(trame+0)=this->command;

//char ll= strlen(data)+3;
//*(trame+1)=ll;
strncpy(trame+1,data,strlen(data));
//*(trame+strlen(data)+1)= '\0';
//qDebug("the trame is: %s",trame);
//char ch=this->cheksum(trame);
//*(trame+strlen(data)+1)=ch;


        return (trame);
 free(trame);

  }

//char b_false;

public:
char command;
char* data;


};
/********************_______trame_r class__________*********************/
class trame_r
{
public:

    char command;
    char* data;
    int length;
    trame_r(char* buff)
    {
      free(data);
       // qDebug( "the data is: %s",buff);
        int size = strlen(buff);
        this->length=size;
       this->data=(char*)calloc(size,sizeof(char));
      // buff_copp=buff;
       command= *buff;
       strncpy(data,buff+1,size-1);


      // qDebug( "the command is: %c",command);
      // qDebug( "the data is: %s",data);
       //return(data);


    }




};


class trame_config
{
public:

    char command;
    char band_config;
    char modulation_config;
    char modulation_sheme;
    char map[6];
    char* num_carr;
    char* first_carr;
    char* last_carr;

   char* trame_conf(char com,char modul_config,char modul_sheme,char mapp[6])
    {
        char* tt;
        this->command=com;
        //this->band_config=b_conf;
        this->modulation_config=modul_config;
        this->modulation_sheme=modul_sheme;
        for(int i=0;i<6;i++)
        {this->map[i]=mapp[i];}
        char* trame;
        trame=(char*)malloc(9 * sizeof(char));
        *(trame+0) = command;
        *(trame+1)=modulation_config;
        *(trame+2)=modulation_sheme;
        for(int i=0;i<6;i++)
        {*(trame+i+3)=map[i];}

        qDebug( "the integer is: %s",trame);
        return (trame);

    }



};
















#endif
