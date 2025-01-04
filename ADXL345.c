#include "stm32f10x.h"                  // Device header
#include "ADXL345.h"
#include "Delay.h"
#include <math.h>
#define MPU6050_WRITE_ADDRESS 0XD0
#define MPU6050_READ_ADDRESS  0XD1
short Data[3];
/**
  * 函    数：MPU6050等待事件
  * 参    数：同I2C_CheckEvent
  * 返 回 值：无
  */
void ADXL345_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000;									//给定超时计数时间
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)	//循环等待指定事件
	{
		Timeout --;										//等待时，计数值自减
		if (Timeout == 0)								//自减到0后，等待超时
		{
			/*超时的错误处理代码，可以添加到此处*/
			break;										//跳出等待，不等了
		}
	}
}
void ADXL345_WriteReg(uint8_t RegAddress,uint8_t Data)
{
	I2C_GenerateSTART(I2C2, ENABLE);										//硬件I2C生成起始条件
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//等待EV5
	
	I2C_Send7bitAddress(I2C2, slaveaddress, I2C_Direction_Transmitter);	//硬件I2C发送从机地址，方向为发送
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//等待EV6
	
	I2C_SendData(I2C2, RegAddress);											//硬件I2C发送寄存器地址
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);			//等待EV8
	
	I2C_SendData(I2C2, Data);												//硬件I2C发送数据
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);				//等待EV8_2
	
	I2C_GenerateSTOP(I2C2, ENABLE);											//硬件I2C生成终止条件
}

uint8_t ADXL345_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	I2C_GenerateSTART(I2C2, ENABLE);										//硬件I2C生成起始条件
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//等待EV5
	
	I2C_Send7bitAddress(I2C2, slaveaddress, I2C_Direction_Transmitter);	    //硬件I2C发送从机地址，方向为发送
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//等待EV6
	
	I2C_SendData(I2C2, RegAddress);											//硬件I2C发送寄存器地址
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);				//等待EV8_2
	
	I2C_GenerateSTART(I2C2, ENABLE);										//硬件I2C生成重复起始条件
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//等待EV5
	
	I2C_Send7bitAddress(I2C2, slaveaddress, I2C_Direction_Receiver);		//硬件I2C发送从机地址，方向为接收
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);		//等待EV6
	
	I2C_AcknowledgeConfig(I2C2, DISABLE);									//在接收最后一个字节之前提前将应答失能
	I2C_GenerateSTOP(I2C2, ENABLE);											//在接收最后一个字节之前提前申请停止条件
	
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);				//等待EV7
	Data = I2C_ReceiveData(I2C2);											//接收数据寄存器
	
	I2C_AcknowledgeConfig(I2C2, ENABLE);									//将应答恢复为使能，为了不影响后续可能产生的读取多字节操作
	
	return Data;
}


void ADXL345_ReReadReg(uint8_t RegAddress,short *DataBuf,uint8_t BufSize)
{
	uint8_t buf[6];
	uint8_t i;
	I2C_GenerateSTART(I2C2, ENABLE);										//硬件I2C生成起始条件
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//等待EV5
	
	I2C_Send7bitAddress(I2C2, slaveaddress, I2C_Direction_Transmitter);	    //硬件I2C发送从机地址，方向为发送
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//等待EV6
	
	I2C_SendData(I2C2, DATAX0);											//硬件I2C发送寄存器地址
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);				//等待EV8_2
	
	I2C_GenerateSTART(I2C2, ENABLE);										//硬件I2C生成重复起始条件
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//等待EV5
	
	I2C_Send7bitAddress(I2C2, slaveaddress, I2C_Direction_Receiver);		//硬件I2C发送从机地址，方向为接收
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);		//等待EV6
	
	for(i=0;i<6;i++)
	{
		if(i==5)
		{
			I2C_AcknowledgeConfig(I2C2, DISABLE);									//在接收最后一个字节之前提前将应答失能
			I2C_GenerateSTOP(I2C2, ENABLE);											//在接收最后一个字节之前提前申请停止条件
			ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);				//等待EV7
			buf[i] = I2C_ReceiveData(I2C2);											//接收数据寄存器
			I2C_AcknowledgeConfig(I2C2, ENABLE);									//将应答恢复为使能，为了不影响后续可能产生的读取多字节操作
		}
		else
		{
			ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);				//等待EV7
			buf[i] = I2C_ReceiveData(I2C2);											//接收数据寄存器
		}
 	}
	
	*DataBuf++ =(short)(((u16)buf[1]<<8)+buf[0]); // 合成数据    
	*DataBuf++ =(short)(((u16)buf[3]<<8)+buf[2]); 	    
	*DataBuf   = (short)(((u16)buf[5]<<8)+buf[4]); 
}


uint8_t ADXL345_Init(void)
{
	uint8_t i;
    short x_temp,y_temp,z_temp;
    short x_offset = 0;
    short y_offset = 0;
    short z_offset = 0;
    char x_calib = 0;
    char y_calib = 0;
    char z_calib = 0;

	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);		//开启I2C2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//将PB10和PB11引脚初始化为复用开漏输出
	
	/*I2C初始化*/
	I2C_InitTypeDef I2C_InitStructure;						//定义结构体变量
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;				//模式，选择为I2C模式
	I2C_InitStructure.I2C_ClockSpeed = 50000;				//时钟速度，选择为50KHz
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;		//时钟占空比，选择Tlow/Thigh = 2
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;				//应答，选择使能
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	//应答地址，选择7位，从机模式下才有效
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;				//自身地址，从机模式下才有效
	I2C_Init(I2C2, &I2C_InitStructure);						//将结构体变量交给I2C_Init，配置I2C2
	
	/*I2C使能*/
	I2C_Cmd(I2C2, ENABLE);									//使能I2C2，开始运行
	

	if(ADXL345_ReadReg(DEVICE_ID) == 0xE5)
	{
		ADXL345_WriteReg(DATA_FORMAT,0X0B); // 低电平中断输出,13位全分辨率,输出数据右对齐,16g量程 
		ADXL345_WriteReg(BW_RATE,0x0E);     // 数据输出速度为100Hz
		ADXL345_WriteReg(POWER_CTL,0x08);   // 链接使能,测量模式,省电特性
		ADXL345_WriteReg(INT_ENABLE,0x80);  // 不使用中断	
//      ADXL345_WriteReg(OFSX, 0xFD);
//		ADXL345_WriteReg(OFSY, 0X00);
//		ADXL345_WriteReg(OFSZ, 0X02);
		Delay_ms(100);
		for(i = 0; i < 10; i++)
		{
			ADXL345_Read_Average(&x_temp, &y_temp, &z_temp, 10);
			x_offset += x_temp;
			y_offset += y_temp;
			z_offset += z_temp;
		}
		x_offset /= 10;
		y_offset /= 10;
		z_offset /= 10;

		x_calib =- x_offset / 4;
		y_calib =- y_offset / 4;
		z_calib =- (z_offset - 256) / 4;
		ADXL345_WriteReg(OFSX, x_calib);
		ADXL345_WriteReg(OFSY, y_calib);
		ADXL345_WriteReg(OFSZ, z_calib);
		return 0;
	}
	return 1;
}

void ADXL345_GetData(short *x,short *y,short *z)
{
	short Data[3];
	ADXL345_ReReadReg(DATAX0,Data,sizeof(Data));
	*x=Data[0];
	*y=Data[1];
	*z=Data[2];
}

/**
  * 函    数：ADXL345获取ID号
  * 参    数：无
  * 返 回 值：ADXL345的ID号
  */
uint8_t ADXL345_GetID(void)
{
	return ADXL345_ReadReg(DEVICE_ID);		//返回ID寄存器的值
}


void ADXL345_Read_Average(short *x,short *y,short *z,u8 times)
{
	u8 i;
	short tx,ty,tz;
	*x=0;
	*y=0;
	*z=0;
	if(times)//读取次数不为0
	{
		for(i=0;i<times;i++)//连续读取times次
		{
			ADXL345_GetData(&tx,&ty,&tz);
			*x+=tx;
			*y+=ty;
			*z+=tz;

		}
		*x/=times;
		*y/=times;
		*z/=times;
	}
}

void ADXL345_Get_Angle(float *x_angle,float *y_angle,float *z_angle)
{
	short ax,ay,az;
	ADXL345_Read_Average(&ax,&ay,&az,10);
	*x_angle=atan(ax/sqrt((az*az+ay*ay)))*180/3.14;
	*y_angle=atan(ay/sqrt((ax*ax+az*az)))*180/3.14;
	*z_angle=atan(sqrt((ax*ax+ay*ay))/az)*180/3.14;
}

