#include "trace.h"

traceinfo_t * TI = (traceinfo_t *)TRACEINFO_ADDR;
tracelog_t * TL = (tracelog_t *)TRACELOG_ADDR;
uint32_t * CRC32 = (uint32_t *)TRACECRC_ADDR;
uint32_t Print_Write_Address = PRINTDATA_ADDR;
uint32_t Dump_Write_Address = DUMPBUF_ADDR;
uint8_t eventId = 0x00;


/**
 * ----------------------------------------------------------------------
 * Trace initialization function
 *-----------------------------------------------------------------------
 */
void MX_TRACE_Init(void)
{
	InitCC();
	ResetCC();
	TI->printlog_size = 0x00;
	TI->tracelog_size = 0x00;
	TI->dumpbuf_size = 0x00;
}
/**
 * ----------------------------------------------------------------------
 * Start trace recording function
 *-----------------------------------------------------------------------
 */
void SDK_TRACE_Start(void)
{
	EnableCC();
}
/**
 * ----------------------------------------------------------------------
 * Stop trace recording function
 *-----------------------------------------------------------------------
 */
void SDK_TRACE_Stop(void)
{
	DisableCC();
	__disable_irq();
	*CRC32 = crc32((unsigned char *)TRACEINFO_ADDR, sizeof(traceinfo_t) + (TI->tracelog_size), 0);
	SCB->AIRCR = 0x05FA0004;
}
/**
 * ----------------------------------------------------------------------
 * Function for writing an event to a trace buffer
 * @param id Event ID (For example: LED1)
 * @param value LED port state (0 or 1)
 *-----------------------------------------------------------------------
 */
void SDK_TRACE_Timestamp(uint8_t id, uint8_t value)
{
	uint32_t eCode = 0x00;
	eCode = (eCode << 8) + 0x01;
	eCode = (eCode << 8) + value;
	eCode = (eCode << 8) + id;
	TL[TI->tracelog_size/8].timecode = GetCC() / (HAL_RCC_GetHCLKFreq() / CLK_Prescaler);
	TL[TI->tracelog_size/8].eventcode = eCode;
	TI->tracelog_size+=8;
	eventId++;
}
/**
 * ----------------------------------------------------------------------
 * Function for writing a message to the trace buffer
 * @param data Pointer to array of chars
 *-----------------------------------------------------------------------
 */
void SDK_TRACE_Print(const char * data)
{
	uint8_t len = strlen(data);

	*(__IO uint32_t *)(Print_Write_Address+TI->printlog_size) = GetCC() / (HAL_RCC_GetHCLKFreq() / CLK_Prescaler);
	TI->printlog_size+=4;

	*(__IO uint8_t *)(Print_Write_Address+TI->printlog_size) = len;
	TI->printlog_size++;

	for (int i = 0; i < len; i++)
	{
		*(__IO uint8_t *)(Print_Write_Address+TI->printlog_size) = data[i];
		TI->printlog_size++;
	}
}
/**
 * ----------------------------------------------------------------------
 * Function for writing data to the trace buffer
 * @param data Pointer to data
 * @param size Size of data
 *-----------------------------------------------------------------------
 */
void SDK_TRACE_Dump(const char * data, uint16_t size)
{

	*(__IO uint32_t *)(Dump_Write_Address+TI->dumpbuf_size) = GetCC() / (HAL_RCC_GetHCLKFreq() / CLK_Prescaler);
	TI->dumpbuf_size+=4;

	*(__IO uint16_t *)(Dump_Write_Address+TI->dumpbuf_size) = size;
	TI->dumpbuf_size++;

	for (int i = 0; i < size; i++)
	{
		*(__IO uint8_t *)(Dump_Write_Address+TI->dumpbuf_size) = data[i];
		TI->dumpbuf_size++;
	}
}
