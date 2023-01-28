#include "at24c04_drv.h"
#include "iic_drv.h"
#include <stdio.h>

#define DEVICE_ADDR_WRITE		0xA0
#define DEVICE_ADDR_READ		0xA1
uint8_t WriteByte_AT24CXX(uint8_t data_addr,uint8_t data)
{
	//1、主机发起通信
	IIC_Start();
	//2、发送设备地址找到将要通信的从设备，并等待应答
	IIC_SendByte(DEVICE_ADDR_WRITE);
	if(IIC_WaitforACK() != IIC_ACK)
	{
		goto err_handle;
	}
	//3、发送数据写入地址，并等待应答
	IIC_SendByte(data_addr);
	if(IIC_WaitforACK() != IIC_ACK)
	{
		goto err_handle;
	}
	//4、发送要写入的数据，并等待应答
	IIC_SendByte(data);
	if(IIC_WaitforACK() != IIC_ACK)
	{
		goto err_handle;
	}
	//5、发送完成后停止
	IIC_Stop();
	return 0;
	err_handle:
		printf("WriteByte_AT24CXX error!!!\n");
		return 0xff;
}

uint8_t ReadByte_AT24CXX(uint8_t dataddr)
{	
	uint8_t retdata = 0;
	//1、开始信号
	IIC_Start();
	//2、找到你要通信的从设备地址（写模式），等待应答
	IIC_SendByte(DEVICE_ADDR_WRITE);
	if(IIC_WaitforACK() != IIC_ACK)
	{
		goto err_handle;
	}
	//3、写入你要读取的数据的地址，等待应答
	IIC_SendByte(dataddr);
	if(IIC_WaitforACK() != IIC_ACK)
	{
		goto err_handle;
	}
	//4、再发一个开始信号
	IIC_Start();
	//5、找到你要通信的从设备地址（读模式），等待应答
	IIC_SendByte(DEVICE_ADDR_READ);
	if(IIC_WaitforACK() != IIC_ACK)
	{
		goto err_handle;
	}
	//6、读取目标数据,并发送非应答信号
	retdata = IIC_RcvByte(IIC_NOACK);
	//8、停止
	IIC_Stop();

	return retdata;
	err_handle:
		printf("ReadByte_AT24CXX error!!!\n");
		return 0xff;
}

