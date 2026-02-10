#ifndef __GLOBAL_H_
#define __GLOBAL_H_

typedef enum {
	Loop_SCREEN0 = 0, 	//0
	Loop_SCREEN1,		//1
	Loop_CHOICE,		//2
	Loop_CAR,		//3
	Loop_QUAN,		//4
	Loop_BZ,		//5
} CarState;

typedef void (*LoopFunc)(void);

#endif
