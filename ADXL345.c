#include "stm32f10x.h"                  // Device header
#include "ADXL345.h"
#include "Delay.h"
#include <math.h>
#define MPU6050_WRITE_ADDRESS 0XD0
#define MPU6050_READ_ADDRESS  0XD1
short Data[3];
/**
  * ��    ����MPU6050�ȴ��¼�
  * ��    ����ͬI2C_CheckEvent
  * �� �� ֵ����
  */
void ADXL345_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000;									//������ʱ����ʱ��
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)	//ѭ���ȴ�ָ���¼�
	{
		Timeout --;										//�ȴ�ʱ������ֵ�Լ�
		if (Timeout == 0)								//�Լ���0�󣬵ȴ���ʱ
		{
			/*��ʱ�Ĵ�������룬������ӵ��˴�*/
			break;										//�����ȴ���������
		}
	}
}
void ADXL345_WriteReg(uint8_t RegAddress,uint8_t Data)
{
	I2C_GenerateSTART(I2C2, ENABLE);										//Ӳ��I2C������ʼ����
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//�ȴ�EV5
	
	I2C_Send7bitAddress(I2C2, slaveaddress, I2C_Direction_Transmitter);	//Ӳ��I2C���ʹӻ���ַ������Ϊ����
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//�ȴ�EV6
	
	I2C_SendData(I2C2, RegAddress);											//Ӳ��I2C���ͼĴ�����ַ
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);			//�ȴ�EV8
	
	I2C_SendData(I2C2, Data);												//Ӳ��I2C��������
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);				//�ȴ�EV8_2
	
	I2C_GenerateSTOP(I2C2, ENABLE);											//Ӳ��I2C������ֹ����
}

uint8_t ADXL345_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	I2C_GenerateSTART(I2C2, ENABLE);										//Ӳ��I2C������ʼ����
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//�ȴ�EV5
	
	I2C_Send7bitAddress(I2C2, slaveaddress, I2C_Direction_Transmitter);	    //Ӳ��I2C���ʹӻ���ַ������Ϊ����
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//�ȴ�EV6
	
	I2C_SendData(I2C2, RegAddress);											//Ӳ��I2C���ͼĴ�����ַ
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);				//�ȴ�EV8_2
	
	I2C_GenerateSTART(I2C2, ENABLE);										//Ӳ��I2C�����ظ���ʼ����
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//�ȴ�EV5
	
	I2C_Send7bitAddress(I2C2, slaveaddress, I2C_Direction_Receiver);		//Ӳ��I2C���ʹӻ���ַ������Ϊ����
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);		//�ȴ�EV6
	
	I2C_AcknowledgeConfig(I2C2, DISABLE);									//�ڽ������һ���ֽ�֮ǰ��ǰ��Ӧ��ʧ��
	I2C_GenerateSTOP(I2C2, ENABLE);											//�ڽ������һ���ֽ�֮ǰ��ǰ����ֹͣ����
	
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);				//�ȴ�EV7
	Data = I2C_ReceiveData(I2C2);											//�������ݼĴ���
	
	I2C_AcknowledgeConfig(I2C2, ENABLE);									//��Ӧ��ָ�Ϊʹ�ܣ�Ϊ�˲�Ӱ��������ܲ����Ķ�ȡ���ֽڲ���
	
	return Data;
}


void ADXL345_ReReadReg(uint8_t RegAddress,short *DataBuf,uint8_t BufSize)
{
	uint8_t buf[6];
	uint8_t i;
	I2C_GenerateSTART(I2C2, ENABLE);										//Ӳ��I2C������ʼ����
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//�ȴ�EV5
	
	I2C_Send7bitAddress(I2C2, slaveaddress, I2C_Direction_Transmitter);	    //Ӳ��I2C���ʹӻ���ַ������Ϊ����
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//�ȴ�EV6
	
	I2C_SendData(I2C2, DATAX0);											//Ӳ��I2C���ͼĴ�����ַ
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);				//�ȴ�EV8_2
	
	I2C_GenerateSTART(I2C2, ENABLE);										//Ӳ��I2C�����ظ���ʼ����
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//�ȴ�EV5
	
	I2C_Send7bitAddress(I2C2, slaveaddress, I2C_Direction_Receiver);		//Ӳ��I2C���ʹӻ���ַ������Ϊ����
	ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);		//�ȴ�EV6
	
	for(i=0;i<6;i++)
	{
		if(i==5)
		{
			I2C_AcknowledgeConfig(I2C2, DISABLE);									//�ڽ������һ���ֽ�֮ǰ��ǰ��Ӧ��ʧ��
			I2C_GenerateSTOP(I2C2, ENABLE);											//�ڽ������һ���ֽ�֮ǰ��ǰ����ֹͣ����
			ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);				//�ȴ�EV7
			buf[i] = I2C_ReceiveData(I2C2);											//�������ݼĴ���
			I2C_AcknowledgeConfig(I2C2, ENABLE);									//��Ӧ��ָ�Ϊʹ�ܣ�Ϊ�˲�Ӱ��������ܲ����Ķ�ȡ���ֽڲ���
		}
		else
		{
			ADXL345_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);				//�ȴ�EV7
			buf[i] = I2C_ReceiveData(I2C2);											//�������ݼĴ���
		}
 	}
	
	*DataBuf++ =(short)(((u16)buf[1]<<8)+buf[0]); // �ϳ�����    
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

	/*����ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);		//����I2C2��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//����GPIOB��ʱ��
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//��PB10��PB11���ų�ʼ��Ϊ���ÿ�©���
	
	/*I2C��ʼ��*/
	I2C_InitTypeDef I2C_InitStructure;						//����ṹ�����
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;				//ģʽ��ѡ��ΪI2Cģʽ
	I2C_InitStructure.I2C_ClockSpeed = 50000;				//ʱ���ٶȣ�ѡ��Ϊ50KHz
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;		//ʱ��ռ�ձȣ�ѡ��Tlow/Thigh = 2
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;				//Ӧ��ѡ��ʹ��
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	//Ӧ���ַ��ѡ��7λ���ӻ�ģʽ�²���Ч
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;				//�����ַ���ӻ�ģʽ�²���Ч
	I2C_Init(I2C2, &I2C_InitStructure);						//���ṹ���������I2C_Init������I2C2
	
	/*I2Cʹ��*/
	I2C_Cmd(I2C2, ENABLE);									//ʹ��I2C2����ʼ����
	

	if(ADXL345_ReadReg(DEVICE_ID) == 0xE5)
	{
		ADXL345_WriteReg(DATA_FORMAT,0X0B); // �͵�ƽ�ж����,13λȫ�ֱ���,��������Ҷ���,16g���� 
		ADXL345_WriteReg(BW_RATE,0x0E);     // ��������ٶ�Ϊ100Hz
		ADXL345_WriteReg(POWER_CTL,0x08);   // ����ʹ��,����ģʽ,ʡ������
		ADXL345_WriteReg(INT_ENABLE,0x80);  // ��ʹ���ж�	
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
  * ��    ����ADXL345��ȡID��
  * ��    ������
  * �� �� ֵ��ADXL345��ID��
  */
uint8_t ADXL345_GetID(void)
{
	return ADXL345_ReadReg(DEVICE_ID);		//����ID�Ĵ�����ֵ
}


void ADXL345_Read_Average(short *x,short *y,short *z,u8 times)
{
	u8 i;
	short tx,ty,tz;
	*x=0;
	*y=0;
	*z=0;
	if(times)//��ȡ������Ϊ0
	{
		for(i=0;i<times;i++)//������ȡtimes��
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

