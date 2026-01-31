/**
  ******************************************************************************
  * @file    i2c.c
  * @author  铁头山羊
  * @version V 1.0.0
  * @date    2024年9月3日
  * @brief   i2c驱动源文件
  ******************************************************************************
  */
#include "i2c.h"

//
// @简介：通过I2C向从机写入多个字节
//
// @参数 I2Cx：填写要操作的I2C的名称，可以是I2C1或I2C2
// @参数 Addr：填写从机的地址，左对齐 - A6 A5 A4 A3 A2 A1 A0 0
// @参数 pData：要发送的数据（数组）
// @参数 Size：要发送的数据的数量，以字节为单位
//
// @返回值：0 - 发送成功， -1 - 寻址失败， -2 - 数据被拒收
//
__weak int My_I2C_SendBytes(I2C_TypeDef *I2Cx, uint8_t Addr, const uint8_t *pData, uint16_t Size) {
	// #1. 等待总线空闲
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET);

	// #2. 发送起始位
	I2C_GenerateSTART(I2Cx, ENABLE);

	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB) == RESET);

	// #3. 寻址阶段
	I2C_ClearFlag(I2Cx, I2C_FLAG_AF);

	I2C_SendData(I2Cx, Addr & 0xfe);

	while (1) {
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == SET) {
			break;
		}
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
			I2C_GenerateSTOP(I2Cx, ENABLE);
			return -1; // 寻址失败
		}
	}

	// 清除ADDR
	I2C_ReadRegister(I2Cx, I2C_Register_SR1);
	I2C_ReadRegister(I2Cx, I2C_Register_SR2);

	// #4. 发送数据
	for (uint16_t i = 0; i < Size; i++) {
		while (1) {
			if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
				I2C_GenerateSTOP(I2Cx, ENABLE);
				return -2; // 数据被拒收
			}
			if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE) == SET) {
				break;
			}
		}

		I2C_SendData(I2Cx, pData[i]);
	}

	while (1) {
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
			I2C_GenerateSTOP(I2Cx, ENABLE);
			return -2; // 数据被拒收
		}

		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == SET) {
			break;
		}
	}

	// #5. 发送停止位
	I2C_GenerateSTOP(I2Cx, ENABLE);
	return 0; // 成功
}


//
// @简介：通过I2C从从机读多个字节
//
// @参数 I2Cx：填写要操作的I2C的名称，可以是I2C1或I2C2
// @参数 Addr：填写从机的地址，左对齐 - A6 A5 A4 A3 A2 A1 A0 0
// @参数 pBuffer：接收缓冲区（数组）
// @参数 Size：要读取的数据的数量，以字节为单位
//
// @返回值：0 - 发送成功， -1 - 寻址失败
//
__weak int My_I2C_ReceiveBytes(I2C_TypeDef *I2Cx, uint8_t Addr, uint8_t *pBuffer, uint16_t Size) {
	if (Size == 0)
		return 0;

	// #1. 等待总线空闲
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET);

	// #2. 发送起始位
	I2C_GenerateSTART(I2Cx, ENABLE);

	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB) == RESET);

	// #3. 寻址阶段
	I2C_ClearFlag(I2Cx, I2C_FLAG_AF);

	I2C_SendData(I2Cx, Addr | 0x01);

	while (1) {
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == SET) {
			break;
		}
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
			I2C_GenerateSTOP(I2Cx, ENABLE);
			return -1; // 寻址失败
		}
	}

	// #4. 数据读取
	if (Size == 1) {
		// 向ACK写0
		I2C_AcknowledgeConfig(I2Cx, DISABLE);

		// 清除ADDR
		__disable_irq();
		I2C_ReadRegister(I2Cx, I2C_Register_SR1);
		I2C_ReadRegister(I2Cx, I2C_Register_SR2);

		// 发送停止位
		I2C_GenerateSTOP(I2Cx, ENABLE);
		__enable_irq();
		// 等待RxNE置位
		while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);
		// 读取数据
		pBuffer[0] = I2C_ReceiveData(I2Cx);
	} else {
		// 向ACK写1
		I2C_AcknowledgeConfig(I2Cx, ENABLE);

		// 清除ADDR
		I2C_ReadRegister(I2Cx, I2C_Register_SR1);
		I2C_ReadRegister(I2Cx, I2C_Register_SR2);

		for (uint16_t i = 0; i < Size - 1; i++) {
			if (i == Size - 2) {
				__disable_irq();
			}
			// 等待RxNE置位
			while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);
			// 读取
			pBuffer[i] = I2C_ReceiveData(I2Cx);
		}

		// 向ACK写0
		I2C_AcknowledgeConfig(I2Cx, DISABLE);
		// 发送停止位
		I2C_GenerateSTOP(I2Cx, ENABLE);

		__enable_irq();

		// 等待RxNE置位
		while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);
		pBuffer[Size - 1] = I2C_ReceiveData(I2Cx);
	}

	return 0;
}

//**************************新加***************************
//
// @简介：通过I2C从从机指定寄存器连续读取多个字节（Burst Read）
//
// @参数 I2Cx：填写要操作的I2C的名称，可以是I2C1或I2C2
// @参数 Addr：填写从机的地址，左对齐 - A6 A5 A4 A3 A2 A1 A0 0（如MPU6050填写0xd0）
// @参数 Reg：填写要读取的寄存器起始地址（如MPU6050的0x3b）
// @参数 pBuffer：接收缓冲区（数组）
// @参数 Size：要读取的数据的数量，以字节为单位
//
// @返回值：0 - 发送成功， -1 - 寻址失败， -2 - 数据/寄存器地址被拒收
//
__weak int My_I2C_ReadBurst(I2C_TypeDef *I2Cx, uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Size) {
	if (Size == 0)
		return 0;

	// ==================== 阶段 1：发送寄存器地址（写操作） ====================

	// #1. 等待总线空闲
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET);

	// #2. 发送起始位（START）
	I2C_GenerateSTART(I2Cx, ENABLE);
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB) == RESET);

	// #3. 发送写地址（Addr & 0xfe）并等待应答
	I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
	I2C_SendData(I2Cx, Addr & 0xFE);  // 确保是写方向（bit0 = 0）

	while (1) {
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == SET) {
			break;
		}
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
			I2C_GenerateSTOP(I2Cx, ENABLE);
			return -1; // 写寻址失败
		}
	}

	// 清除ADDR标志
	I2C_ReadRegister(I2Cx, I2C_Register_SR1);
	I2C_ReadRegister(I2Cx, I2C_Register_SR2);

	// #4. 发送寄存器地址（Reg）
	while (1) {
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
			I2C_GenerateSTOP(I2Cx, ENABLE);
			return -2; // 寄存器地址被拒收
		}
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE) == SET) {
			break;
		}
	}
	I2C_SendData(I2Cx, Reg);

	// 等待寄存器地址发送完成（BTF置位）
	while (1) {
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
			I2C_GenerateSTOP(I2Cx, ENABLE);
			return -2;
		}
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == SET) {
			break;
		}
	}

	// ==================== 阶段 2：Repeated START，进入读模式 ====================

	// #5. 发送重复起始位（Sr）
	I2C_GenerateSTART(I2Cx, ENABLE);
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB) == RESET);

	// #6. 发送读地址（Addr | 0x01）并等待应答
	I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
	I2C_SendData(I2Cx, Addr | 0x01);  // 读方向（bit0 = 1）

	while (1) {
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == SET) {
			break;
		}
		if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
			I2C_GenerateSTOP(I2Cx, ENABLE);
			return -1; // 读寻址失败
		}
	}

	// ==================== 阶段 3：连续读取数据 ====================

	if (Size == 1) {
		// 单字节读取：立即关闭ACK并准备STOP
		I2C_AcknowledgeConfig(I2Cx, DISABLE);

		// 清除ADDR（必须在关闭中断或原子操作中完成，防止收到数据后溢出）
		__disable_irq();
		I2C_ReadRegister(I2Cx, I2C_Register_SR1);
		I2C_ReadRegister(I2Cx, I2C_Register_SR2);

		// 发送停止位
		I2C_GenerateSTOP(I2Cx, ENABLE);
		__enable_irq();

		// 等待接收数据
		while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);
		pBuffer[0] = I2C_ReceiveData(I2Cx);
	} else {
		// 多字节读取：使能ACK
		I2C_AcknowledgeConfig(I2Cx, ENABLE);

		// 清除ADDR
		I2C_ReadRegister(I2Cx, I2C_Register_SR1);
		I2C_ReadRegister(I2Cx, I2C_Register_SR2);

		// 读取前 Size-1 个字节（都要发送ACK）
		for (uint16_t i = 0; i < Size - 1; i++) {
			// 倒数第二个字节前关闭中断，确保能及时处理最后一个字节
			if (i == Size - 2) {
				__disable_irq();
			}

			// 等待RxNE置位（收到数据）
			while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);
			pBuffer[i] = I2C_ReceiveData(I2Cx);
		}

		// 准备接收最后一个字节：关闭ACK并发送STOP
		I2C_AcknowledgeConfig(I2Cx, DISABLE);
		I2C_GenerateSTOP(I2Cx, ENABLE);
		__enable_irq();

		// 读取最后一个字节
		while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);
		pBuffer[Size - 1] = I2C_ReceiveData(I2Cx);
	}

	return 0; // 成功
}

