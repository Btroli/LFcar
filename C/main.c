#include "AllHeader.h"

int main(void) {

	InitAll();

	while (1)
		loop_functions[LoopMode]();

}

