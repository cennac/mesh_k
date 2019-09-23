/*
 * Copyright (c) 2019, All rights reserved.
 *
 * File         : deviceProcIf.h
 * Status       : Current
 * Description  :
 *
 * Author       : haopeng
 * Contact      : 376915244@qq.com
 *
 * Revision     : 2019-07
 * Description  : Primary released
 *
 * ## Please log your description here for your modication ##
 *
 * Revision     :
 * Modifier     :
 * Description  :
 *
 * */

#define PROC_FILE_PATH "/proc/net/dev"
/* interface flags */
#define IFSTAT_LOOPBACK 1
#define IFSTAT_DOWN     2
#define IFSTAT_HASSTATS 4
#define IFSTAT_HASINDEX 8
#define IFSTAT_TOTAL  128
 
/*NIC list*/
struct ifstatData 
{
    char *name;
    int namelen;
    unsigned long obout, obin, bout, bin;
    int flags;
    struct ifstatData *next;
};
 
struct ifstatList 
{
    struct ifstatData *first;
    int flags;
};
 
/*device data*/
struct ifstatDriver
{
    char *name;
    int (*openDriver) (struct ifstatDriver *driver);
    int (*scanInterfaces) (struct ifstatDriver *driver,struct ifstatList *ifs);
    int (*getStats) (struct ifstatDriver *driver,struct ifstatList *ifs);
    void (*closeDriver) (struct ifstatDriver *driver);
    void *data;
};
 
struct procDriverData 
{
    char *file;
    int checked;
};

 struct ifstatRate
{
  double rxRate;
  double txRate;
  char*   ifname;
}ifstatRate_t;

