

/*
Copyright (c) 2019, All rights reserved.

File         : thread.h
Description  : 

Contact      : fervour.cool@163.com

Revision     : 2019-07 
Description  : Primary released

*/




#ifndef __THREAD_H__
#define __THREAD_H__


typedef void * threadHandle_t;

threadHandle_t threadAddListeningFile(const char *name, int fd, void *data, void (*function)(void *));
void threadRemoveListeningFile(threadHandle_t handle);
threadHandle_t threadAddPollingFunction(const char *name, void *data, void (*function)(void *));
void threadRemovePollingFunction(threadHandle_t handle);
void threadSchedule(int slot);
void threadFreeAll(void);
void threadExit(void);


#endif /* __THREAD_H__ */

