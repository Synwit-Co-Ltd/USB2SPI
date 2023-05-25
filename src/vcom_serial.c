#include <stdio.h>
#include <string.h>
#include "ch32v30x.h"
#include "ch32v30x_usb.h"
#include "ch32v30x_usbhs_device.h"

#include "usb_desc.h"
#include "vcom_serial.h"


volatile VCOM Vcom;

VCOM_LINE_CODING LineCfg = {115200, 0, 0, 8};   // Baud rate, stop bits, parity bits, data bits


#define BUF_SZ  1024
uint8_t RXBuffer[BUF_SZ] __attribute__((aligned(4)));


void VCOM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    DMA_InitTypeDef  DMA_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  // PB12 => SPI2_NSS
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  // PB13 => SPI2_SCK
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;  // PB14 => SPI2_MISO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;  // PB15 => SPI2_MOSI
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);

    SPI_Cmd(SPI2, ENABLE);

    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI2->DATAR;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)RXBuffer;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_BufferSize = BUF_SZ;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel4, ENABLE);
}


void VCOM_LineCoding(VCOM_LINE_CODING * LineCfgx)
{
}


extern volatile uint32_t SysTick_ms;
void VCOM_TransferData(void)
{
    uint8_t *rxdata = RXBuffer;

    if(Vcom.in_ready)		// 可以向主机发送数据
    {
        if((DMA1->INTFR & (DMA1_FLAG_HT4 | DMA1_FLAG_TC4)) || (SysTick_ms > 10))
        {
            if(DMA1->INTFR & DMA1_FLAG_HT4)
            {
                rxdata = &RXBuffer[0];

                Vcom.in_bytes = BUF_SZ/2;

                DMA1->INTFCR = DMA1_FLAG_HT4;
            }
            else if(DMA1->INTFR & DMA1_FLAG_TC4)
            {
                rxdata = &RXBuffer[BUF_SZ/2];

                Vcom.in_bytes = BUF_SZ/2;

                DMA1->INTFCR = DMA1_FLAG_TC4;

                SysTick_ms = 0;
            }
            else
            {
                SysTick_ms = 0;

                uint32_t n_xfer = BUF_SZ - DMA_GetCurrDataCounter(DMA1_Channel4);
                if(n_xfer == 0)
                {
                    goto xfer_out;
                }
                if(n_xfer < BUF_SZ/2)
                {
                    rxdata = &RXBuffer[0];

                    Vcom.in_bytes = n_xfer;
                }
                else
                {
                    rxdata = &RXBuffer[BUF_SZ/2];

                    Vcom.in_bytes = n_xfer - BUF_SZ/2;
                }

                DMA_Cmd(DMA1_Channel4, DISABLE);
                DMA_SetCurrDataCounter(DMA1_Channel4, BUF_SZ);
                DMA_Cmd(DMA1_Channel4, ENABLE);
            }

            Vcom.in_ready = 0;

            USBHSD->UEP3_TX_DMA = (uint32_t)rxdata;
            USBHSD->UEP3_TX_LEN = Vcom.in_bytes;
            USBHSD->UEP3_TX_CTRL = (USBHSD->UEP3_TX_CTRL & ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_ACK;
        }
        else
        {
            /* Prepare a zero packet if previous packet size is CDC_BULK_IN_SZ and
               no more data to send at this moment to note Host the transfer has been done */
            if(Vcom.in_bytes == CDC_BULK_IN_SZ_HS)
			{
				Vcom.in_bytes = 0;
				
				USBHSD->UEP3_TX_LEN = 0;
				USBHSD->UEP3_TX_CTRL = (USBHSD->UEP3_TX_CTRL & ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_ACK;
			}
        }
    }

xfer_out:
	/* 从主机接收到数据，且 tx_buff 能够装下它们 */
    if(Vcom.out_ready)
    {
        Vcom.out_ready = 0;

        /* Ready for next BULK OUT */
        USBHSD->UEP3_RX_CTRL = (USBHSD->UEP3_RX_CTRL & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK;
    }
}
