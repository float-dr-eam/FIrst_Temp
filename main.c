#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "ADXL345.h"
#include "LED.h"


uint8_t ID;								//定义用于存放ID号的变量
float angleX, angleY, angleZ;
short angleX1, angleY1, angleZ1;
int main(void)
{
	OLED_Init();
	
	OLED_Clear();	
    LED_Init();
	ADXL345_Init(); // 初始化
	OLED_ShowString(1, 1, "ID:");		//显示静态字符串
	ID = ADXL345_GetID();				//获取MPU6050的ID号
	OLED_ShowHexNum(1, 4, ID, 2);		//OLED显示ID号
	
	OLED_ShowString(2,1,"X:");
	OLED_ShowString(3,1,"Y:");
	OLED_ShowString(4,1,"Z:");
	
	OLED_ShowString(2,16,"'");
	OLED_ShowString(3,16,"'");
	OLED_ShowString(4,16,"'");
	while(1)
	{
		ADXL345_Read_Average(&angleX1, &angleY1, &angleZ1,10);
		ADXL345_Get_Angle(&angleX, &angleY, &angleZ);
		OLED_ShowFloatNum(2, 3,(float) angleX1, 4);				//OLED显示数据
		OLED_ShowFloatNum(3, 3,(float) angleY1, 4);
		OLED_ShowFloatNum(4, 3,(float) angleZ1, 4);
        OLED_ShowFloatNum(2, 9,(float) angleX, 6);				//OLED显示数据
		OLED_ShowFloatNum(3, 9,(float) angleY, 6);
		OLED_ShowFloatNum(4, 9,(float) angleZ, 6);
		LED1_Turn();

	}
}

