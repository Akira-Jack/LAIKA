#ifndef _FLASH_USER_H_
#define _FLASH_USER_H_

#include "define.h"

/*----------------------------------------------------------------------------
 **FMC 操作区域
-----------------------------------------------------------------------------*/
#define	FMC_APROM_START		(0x00UL)		
#define	FMC_APROM_END		(0xFFFFFFFFUL)				/*Max:64Kb*/

#define	FMC_DAT_START		(0x1C000000UL)
#define	FMC_DAT_END			(0x1C0003FFUL)				/*1Kb*/


/*----------------------------------------------------------------------------
 **FMC 操作命令
-----------------------------------------------------------------------------*/
#define	FMC_CMD_READ		(0x1U)				
#define FMC_CMD_WRITE		(0x2U)				
#define FMC_CMD_CRC			(0xDU)				/*CRC校验 CRC16-CCITT*/
#define FMC_CMD_PAGE_ERASE		(0x3U)				/*页擦除*/
#define FMC_CMD_ALL_ERASE		(0x6U)				/*整体擦除*/

/*----------------------------------------------------------------------------
 **FMC 解锁
-----------------------------------------------------------------------------*/
#define	FMC_WRITE_KEY	(0x55AA6699)

/*****************************************************************************
 ** \brief	
 **			
 ** \param [in] none
 ** \return  none
 ** \note	 
*****************************************************************************/

							
/*****************************************************************************/
/* Global type definitions ('typedef') */
/*****************************************************************************/


/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source) */
/*****************************************************************************/


/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source) */
/*****************************************************************************/

/*****************************************************************************
 ** \brief	
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source) */
/*****************************************************************************/
/*****************************************************************************
 ** \brief	FMC_EraseAll
 **			FMC 整体擦除
 ** \param [in] none 
 ** \return  none
 ** \note	适用于APROM
*****************************************************************************/
void FMC_EraseAll(void);

/*****************************************************************************
 ** \brief	FMC_ErasePage
 **			FMC 页擦除
 ** \param [in] PageAddr: 
 ** \return  none
 ** \note	
*****************************************************************************/
void FMC_ErasePage(uint32_t PageAddr);

/*****************************************************************************
 ** \brief	FMC_Read
 **			FMC 读
 ** \param [in] DataAddr：地址两位必须为0
 ** \return  32位数据
 ** \note	
*****************************************************************************/
uint32_t FMC_Read(uint32_t DataAddr);
/*****************************************************************************
 ** \brief	FMC_Write
 **			FMC 写
 ** \param [in] DataAddr：地址两位必须为0
 **				DataVlue: 32位数据
 ** \return  none
 ** \note	
*****************************************************************************/
void FMC_Write(uint32_t DataAddr, uint32_t DataVlue);

/*****************************************************************************
 ** \brief	FMC_CRC
 **			FMC CRC校验
 ** \param [in] CRCStartAddr:	  CRC校验起始地址
 **				CRCStopAddr:	  CRC校验结束地址
 ** \return  none
 ** \note	只适用于APROM区
*****************************************************************************/
uint16_t FMC_CRC(uint32_t CRCStartAddr, uint32_t CRCStopAddr);


#define		SECTOR_SIZE         512				//扇区大小
#define		SECTOR_AMOUNT       2				//扇区个数

#define USED_BYTE_QTY_IN_ONE_SECTOR     128		//最大存储字节(中微实际512)
//#define USED_BYTE_QTY_IN_ONE_SECTOR   256
//#define USED_BYTE_QTY_IN_ONE_SECTOR   512

#define FLASH_FEED_INFO_ADDR  	FMC_DAT_START + 4
#define FLASH_FUNC_ADDR  		FMC_DAT_START + SECTOR_SIZE


extern void IAP_erase_user(uint32_t PageAddr);						//删除扇区
extern void IAP_write_user(uint32_t Addr, uint8_t *EEDATA, u16 number);		//写入数据
extern void IAP_read_user(uint32_t Addr, uint8_t *DataAddress, u16 number);	//读取数据
extern void eeprom_data_init(u8 cmd);								//初始化flash
extern void user_write_flash(u32 begin_addr, u8 *dat, u16 counter);		//写连续数据
extern void protect_data_change(char error_num);					//根据保护数据更改喂食计划
extern char check_eeprom_data(void);									//检测备份喂食计划和喂食计划是否异常

extern void printf_feed_info(void);
#endif

