此版本：方框停车 几乎完美版，具有圈数的选项。

为什么Stime一直是零呢？
因为
	*SystemTime++ 等于 *(SystemTime++)
所以应该是这样的
	(*SystemTime)++
。修复了这个问题。

改变EvalueA,EvalueB,Stime的类型为volatile int16_t。 

一些细节。

