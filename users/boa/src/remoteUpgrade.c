/*
 * Copyright (c) 2019, All rights reserved.
 *
 * File         : deviceProcIf.c
 * Status       : Current
 * Description  :
 *
 * Author       : haopeng
 * Contact      : 376915244@qq.com
 *
 * Revision     : 2019-08
 * Description  : Primary released
 *
 * ## Please log your description here for your modication ##
 *
 * Revision     :
 * Modifier     :
 * Description  :
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "remoteUpgrade.h"
#include "cJSON.h"
#include "md5.h"


static unsigned long parseDns(const char* host_name)
{
	struct hostent* host;
	struct in_addr addr;
	char ** pp;

	host = gethostbyname(host_name);
	if(NULL == host)
	{
		printf("autoupgradeDns: gethostbyname %s failed,try again\n", host_name);

		sleep(2) ;
		
		host = gethostbyname(host_name);
		
		if(NULL == host)
		{
			sleep(2) ;
			
			host = gethostbyname(host_name);
			
			if(NULL == host)
			{
				return -1;
			}
		}
	}
	pp = host->h_addr_list;
	
	if(*pp != NULL)
	{
		addr.s_addr = *((unsigned int *)*pp);
		pp++;
		
		printf("autoupgradeDns: gethostbyname %s sucess.\n", host_name);

		return addr.s_addr;
	}

	printf("autoupgradeDns: gethostbyname %s fail.\n", host_name);

	return 0;
}

static int httpTcpClientCreate(const char *host, int port){
    struct hostent *he;
    struct sockaddr_in serverAddr; 
    int socketFd;
 
    if((he = gethostbyname(host))==NULL){
        return -1;
    }
 
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_port = htons(port);
   serverAddr.sin_addr = *((struct in_addr *)he->h_addr);
 
    if((socketFd = socket(AF_INET,SOCK_STREAM,0))==-1){
        return -1;
    }
 
    if(connect(socketFd, (struct sockaddr *)&serverAddr,sizeof(struct sockaddr)) == -1){
        return -1;
    }
 
    return socketFd;
}
 
static void httpTcpClientClose(int socket){
    close(socket);
}
 
static int httpParseUrl(const char *url,char *host,char *file,int *port)
{
    char *ptr1,*ptr2;
    int len = 0;
    if(!url || !host || !file || !port){
        return -1;
    }
 
    ptr1 = (char *)url;
 
    if(!strncmp(ptr1,"http://",strlen("http://"))){
        ptr1 += strlen("http://");
    }else{
        return -1;
    }
 
    ptr2 = strchr(ptr1,'/');
    if(ptr2){
        len = strlen(ptr1) - strlen(ptr2);
        memcpy(host,ptr1,len);
        host[len] = '\0';
        if(*(ptr2 + 1)){
            memcpy(file,ptr2 + 1,strlen(ptr2) - 1 );
            file[strlen(ptr2) - 1] = '\0';
        }
    }else{
        memcpy(host,ptr1,strlen(ptr1));
        host[strlen(ptr1)] = '\0';
    }
    /*get host and ip*/
    ptr1 = strchr(host,':');
    if(ptr1){
        *ptr1++ = '\0';
        *port = atoi(ptr1);
    }else{
        *port = MY_HTTP_DEFAULT_PORT;
    }
 
    return 0;
}
 
 
static int httpTcpClientRecv(int socket,char *lpbuff){
    int recvnum = 0;
 
    recvnum = recv(socket, lpbuff,BUFFER_SIZE*4,0);
    printf("received response:\n%s \n",lpbuff);
 
    return recvnum;
}
 
static int httpTcpClientSend(int socket,char *buff,int size){
    int sent=0,tmpres=0;
 
    while(sent < size){
        tmpres = send(socket,buff+sent,size-sent,0);
        if(tmpres == -1){
            return -1;
        }
        sent += tmpres;
    }
    return sent;
}
 
static char *httpParseResult(const char*recvBuf)
{
    char *ptmp = NULL; 
    char *response = NULL;
	char *recvBufPtr=NULL;
	char *dataHead,*dataTail;
	int count = 0;
	char httpHead[512];
	    printf("---->recvBuf:\n%s\n",recvBuf);
    ptmp = (char*)strstr(recvBuf,"HTTP/1.1");
    if(!ptmp){
        printf("http/1.1 not faind\n");
        return NULL;
    }
    if(atoi(ptmp + 9)!=200){
        printf("result:\n%s\n",recvBuf);
        return NULL;
    }
 
    ptmp = (char*)strstr(recvBuf,"\r\n\r\n");
    if(!ptmp){
        printf("ptmp is NULL\n");
        return NULL;
    }
    response = (char *)malloc(strlen(ptmp)+1);
    if(!response){
        printf("malloc failed \n");
        return NULL;
    }
    //strcpy(response,ptmp+4);
	response = strchr(ptmp+4,'{');
    return response;
}

 
char * httpGet(const char *url)
{
    char post[BUFFER_SIZE] = {0};
    int socketFd = -1;
    char recvBuf[BUFFER_SIZE*4] = {0};
    char *ptmp;
    char hostAddr[BUFFER_SIZE] = {'\0'};
    char file[BUFFER_SIZE] = {'\0'};
    int port = 0;
    int len=0;
	
    printf("--->httpGeturl=%s\n",url);
    if(!url){
        printf("      failed!\n");
        return NULL;
    }

    if(httpParseUrl(url,hostAddr,file,&port)){
        printf("httpParseUrl failed!\n");
        return NULL;
    }
    printf("hostAddr : %s\tfile:%s\t,%d\n",hostAddr,file,port);
 
    socketFd =  httpTcpClientCreate(hostAddr,port);
    if(socketFd < 0){
        printf("httpTcpclientCreate failed\n");
        return NULL;
    }
 
    sprintf(recvBuf,HTTP_GET,file,hostAddr,port);
 
    if(httpTcpClientSend(socketFd,recvBuf,strlen(recvBuf)) < 0){
        printf("httpCcpclientSend failed..\n");
        return NULL;
    }
	printf("send request:\n%s\n",recvBuf);
 
    if(httpTcpClientRecv(socketFd,recvBuf) <= 0){
        printf("httpTcpclientRecv failed\n");
        return NULL;
    }
    httpTcpClientClose(socketFd);
 
    return httpParseResult(recvBuf);
}

/*
 *parse Data as fellow:
 {"flag":true,"code":200,"message":"查询成功",
 "data":{"fid":"3e37bade62a44a618753e0081e756e75",
 "name":"测试2","type":0,"version":"1.0.0",
 "createtime":1566898148000,"content":"656564656",
 "downurl":"http://120.79.61.154:9001/firmware/download?fid=3e37bade62a44a618753e0081e756e75",
 "md5":"80AE29AFD0C97B2DF8F970567F09BE4B",
 "binurl":"/usr/uploads/bin/20192927012907.png","ssid":null}}
*/
remoteUpgrade_t getUpgradeFileInfo(remoteUpgrade_t fileInfo,char *recvData)
{
  cJSON *jsonData=NULL;
  cJSON *jasonObj=NULL;
  jsonData = cJSON_Parse(recvData);
  if (!jsonData)
  {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
  }
   if(jasonObj = cJSON_GetObjectItem(jsonData,"data"))
   {
 	strcpy(fileInfo.downloadUrl,cJSON_GetObjectItem(jasonObj,"downurl")->valuestring);
	strcpy(fileInfo.md5,cJSON_GetObjectItem(jasonObj,"md5")->valuestring);
	printf("-------->downurl=%s md5=%s\n",fileInfo.downloadUrl,fileInfo.md5);
   }
   return fileInfo;
}
int upgradeInit(remoteUpgrade_t fileInfo)
{
 memset(&fileInfo,0,sizeof(remoteUpgrade_t));
 return 0;
}

int getFileRequestUrl(char url[])
{
  unsigned long hostIpAddr;
  char tmpBuf[16]={0};
  unsigned int ip=0;
  hostIpAddr=parseDns(UPGRADE_GILE_SERVER_HOST_NAME);
  ip=ntohl(hostIpAddr);
  sprintf(tmpBuf,IP_DOT_FORMATE,IP_SEGMENT(ip));
  sprintf(url,UPGRADE_FILE_SERVER_PATH,tmpBuf);
  return 0;
}

/*
#parameters:
 * filePath:file path need generate md5
 * md5:md5 value from remote server
#function:
 *compare md5,equal means file is ok,otherwise file is invalid,exit upgrade 
#return value:
 *0:file is ok     1:file invalid
*/
int checkFileMd5(const char *filePath, char md5[64])
{
	int ret=1;
	char md5Value[MD5_STR_LEN + 1];
	ret = computeUpgradeFileMd5(filePath, md5Value);
	if (0 == ret)
	{
		printf("[file - %s] md5 value:\n", filePath);
		printf("%s\n", md5Value);
		ret=strncmp(md5Value,md5,MD5_STR_LEN);
	}
	return ret;
} 

int computeUpgradeFileMd5(const char *filePath, char *md5Value)
{
	int i;
	int fd;
	int ret;
	unsigned char data[READ_DATA_SIZE];
	unsigned char md5_value[MD5_SIZE];
	MD5_CTX md5;

	fd = open(filePath, O_RDONLY);
	if (-1 == fd)
	{
		perror("open");
		return -1;
	}

	MD5Init(&md5);
	while (1)
	{
		ret = read(fd, data, READ_DATA_SIZE);
		if (-1 == ret)
		{
			perror("read");
			return -1;
		}
		MD5Update(&md5, data, ret); 
		if (0 == ret || ret < READ_DATA_SIZE)
		{
			break;
		}
	} 
	close(fd);
	MD5Final(&md5, md5_value); 
	for(i = 0; i < MD5_SIZE; i++)
	{
		snprintf(md5Value + i*2, 2+1, "%02x", md5_value[i]);
	}
	md5Value[MD5_STR_LEN] = '\0'; // add end
	return 0;
}

int getUpgradeFile(const char *url,const char *md5) {
    FILE *fp = NULL;
	char *ptr = NULL;
    char buf[128];
	char cmdBuf[128];
	int ret =1;
	memset(cmdBuf, 0, sizeof(cmdBuf));
	sprintf(cmdBuf,"cd /tmp ; wget -O fw.bin %s",url);
    if ((fp = popen(cmdBuf, "r")) != NULL) 
	{
        memset(buf, 0, sizeof(buf));
        while(fgets(buf, sizeof(buf)-1, fp)) 

		{         
            ptr = strstr(buf, "Network is unreachable");
            if (ptr) 
			{
                printf("Network is unreachable");
				ret =1;
                break;
            }

            memset(buf, 0, sizeof(buf));
        }

        pclose(fp);
    }
    if(access(LOCATION_FILE_PATH,R_OK)==0)
    {
      ret=checkFileMd5(LOCATION_FILE_PATH, md5);
    }      

    return ret;
}

void remoteUpgrade()
{
 char *recvDataPtr=NULL;
 char requestUrl[128]={0};
 remoteUpgrade_t remoteUpgradeInfo;
 upgradeInit(remoteUpgradeInfo);
 getFileRequestUrl(requestUrl);
 recvDataPtr=httpGet(requestUrl);
 if(NULL==recvDataPtr)
 {
   printf("httpGet return fail...");
 }
 else
 {
   printf("=====>receiveData:\n%s\n",recvDataPtr);
   remoteUpgradeInfo=getUpgradeFileInfo(remoteUpgradeInfo,recvDataPtr);
   printf("++++>downurl=%s md5=%s\n",remoteUpgradeInfo.downloadUrl,remoteUpgradeInfo.md5);
   //httpGet(remoteUpgradeInfo.downloadUrl);
   getUpgradeFile(remoteUpgradeInfo.downloadUrl,remoteUpgradeInfo.md5);
 }
 
}
