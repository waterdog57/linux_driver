#ifndef __IPC_TOOL__
#define __IPC_TOOL__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <libusb-1.0/libusb.h>

#define PLA_MCU ( 0x01 )
#define USB_MCU ( 0x00 )
#define BF_0 (1<<0)
#define BF_1 (1<<1)
#define BF_2 (1<<2)
#define BF_3 (1<<3)
#define BL_4 (1<<4)
#define BL_5 (1<<5)
#define BL_6 (1<<6)
#define BL_7 (1<<7)
#define BF_ALL ( BF_3|BF_2|BF_1|BF_0 )
#define BL_ALL ( BL_7|BL_6|BL_5|BL_4 )
#define B_ALL  ( BL_ALL|BF_ALL )

#define BUFFER_TEST    (1)
#define LOAD_PATCH     (1)
#define IPC_TEST       (1)
#define INTERRUPT_TEST (0)


#define IB_DATA_BUFF_ADDR (0x9000)
#define IB_DATA_MAXLEN    (2048)
#define IB_PAGE_SIZE      (512)

#define DESC_ADDR (0xd3d8)
#define DESC_POLL (0xdc6c)

#define USB_MCU_INT_EP (0x83)

typedef struct{
    // DW0
    uint32_t pg_sel:2;
    uint32_t sre_addr:14;
    uint32_t len:12;
    uint32_t reserve:3;
    uint32_t wr:1;
    // DW1
    uint32_t dst_addr:18;
    uint32_t reserve1:14;
}ipc_descriptor_st;

typedef struct{
    void (*ipc0_callback)(void);
    void (*ipc1_callback)(void);
    void (*ib2soc_callback)(void);
}hwsd_usb_st;

enum{
    IPC0 = 1 << 0,
    IPC1 = 1 << 1,
    IB2SOC = 1 << 2,
};


uint8_t hwusb_init(uint16_t vendor_id, uint16_t product_id);
uint8_t hwusb_exit(void);
void hwusb_register_callback( uint8_t num, void (*callback)(void) ) ;
uint8_t ib_write( uint8_t mcu, uint16_t address, uint8_t* buffer_address, uint32_t len,uint8_t byte_en);
uint8_t ib_read( uint8_t mcu, uint16_t address, uint8_t* buffer_address, uint32_t len,uint8_t byte_en);
uint8_t ipc_write( uint32_t address, uint8_t* buffer_address, uint32_t len,uint8_t byte_en);
uint8_t ipc_read( uint32_t address, uint8_t* buffer_address, uint32_t len,uint8_t byte_en);
void *usb_interrupt_thread(void *args);
void show_descriptor(void);

#endif