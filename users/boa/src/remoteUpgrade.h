 /*
  * Copyright (c) 2019, All rights reserved.
  *
  * File		 : deviceProcIf.c
  * Status		 : Current
  * Description  :
  *
  * Author		 : haopeng
  * Contact 	 : 376915244@qq.com
  *
  * Revision	 : 2019-08
  * Description  : Primary released
  *
  * ## Please log your description here for your modication ##
  *
  * Revision	 :
  * Modifier	 :
  * Description  :
  *
  * */

#ifndef _REMOTE_UPGRADE_H
#define _REMOTE_UPGRADE_H
#define MY_HTTP_DEFAULT_PORT 80
#define UPGRADE_FILE_SERVER_PATH "http://%s/firmware/latest"  //http://120.79.61.154/firmware/latest
#define UPGRADE_GILE_SERVER_HOST_NAME "mesh01.kingsignal.cn"  //mesh01.kingsignal.cn  mesh01.jxngroup.com
#define LOCATION_FILE_PATH "/tmp/fw.bin"
#define BUFFER_SIZE 1024
#define HTTP_GET "GET /%s HTTP/1.1\r\nHOST: %s:%d\r\nAccept: */*\r\n\r\n"
#define IP_DOT_FORMATE "%u.%u.%u.%u"
#define IP_SEGMENT(addr) \
 ((unsigned char *)&addr)[3], \
 ((unsigned char *)&addr)[2], \
 ((unsigned char *)&addr)[1], \
 ((unsigned char *)&addr)[0]


#define READ_DATA_SIZE	1024
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)

char * httpGet(const char *url);

typedef struct 
{
    char checkUpgradeUrl[128];  //check where to get upgrade file
    char downloadUrl[256];      //url to download upgrade file 
    char md5[64];
    uint32_t expired;
	int resultCode;
    int status;                
    int totalSize;
    int currentSize;
    
}remoteUpgrade_t;
int computeUpgradeFileMd5(const char *file_path, char *value);
#endif
