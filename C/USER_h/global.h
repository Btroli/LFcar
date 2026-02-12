#ifndef __GLOBAL_H_
#define __GLOBAL_H_

typedef enum {
	Loop_SCREEN0 = 0, 	//0
	Loop_SCREEN1,		//1
	Loop_SCREEN2,		//2
	Loop_CHOICE,		//3
	Loop_CAR,		//4
	Loop_QUAN,		//5
	Loop_BZ,		//6
} CarState;

typedef void (*LoopFunc)(void);

#endif
