#include <pthread.h>

#include <windows.h>
#include <process.h>

int pthread_create(pthread_t * thread, pthread_attr_t * attr, unsigned int (__stdcall *start_routine)(void *), void * arg){
	HANDLE thread_handle;

	thread_handle = (HANDLE)_beginthreadex(NULL, 0, start_routine, arg, 0, thread);

	if(thread_handle == 0){
		return -1;
	}

	return 0;
}
