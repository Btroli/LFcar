修复细节：
unsigned int *SystemTime   ->   static volatile uint32_t *SystemTime

将pid0部分以及有关变量迁移至妙妙工具。

整合普通跑圈与方框跑圈，quan=0就是普通跑圈了。
车大改装后原pid值明显不适应。需要再调。

