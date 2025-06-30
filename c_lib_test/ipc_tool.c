#include "ipc_tool.h"

static uint8_t bmRequestType = 0x40;
static uint8_t bmRequestType_r = 0xc0;
static uint8_t bRequest = 0x05;
static uint64_t timeout = 1000;
static uint16_t wIndex_pla = 0x0100;
static uint16_t wIndex_usb = 0x0000;

static libusb_device_handle *handle;
static libusb_context *ctx = NULL;
static libusb_device **devs = NULL;
hwsd_usb_st myusb;
volatile sig_atomic_t stop_thread = 0;
sem_t sem;
uint8_t share_buffer[16] = {0};

// patch code
uint8_t patch[320] = {
    0x04, 0xE0, 0xD8, 0xD3, 0x6C, 0xE4, 0xFF, 0xFF, 0xFE, 0xC5, 0xA0, 0x77,
    0x07, 0xB4, 0x00, 0x18, 0x00, 0x1A, 0x04, 0x10, 0x1C, 0xFA, 0xF6, 0xC1,
    0xCA, 0xA5, 0xF6, 0xC6, 0x3E, 0x40, 0x17, 0xF0, 0x7E, 0x23, 0x6E, 0x27,
    0x0C, 0x06, 0xEF, 0xC5, 0xA0, 0x9E, 0xF2, 0x27, 0xF2, 0x23, 0x02, 0x02,
    0x8A, 0xA5, 0xEF, 0x49, 0x3C, 0xF1, 0x61, 0x23, 0x61, 0x27, 0x04, 0x02,
    0x4A, 0xA5, 0x7B, 0xC3, 0x62, 0x9D, 0x02, 0x0A, 0x4A, 0xA5, 0x04, 0x02,
    0x00, 0x1C, 0x15, 0xE0, 0x6A, 0xE0, 0x05, 0xB0, 0x66, 0xE0, 0x05, 0xB4,
    0xD8, 0xC1, 0x20, 0x75, 0x0F, 0x15, 0xFA, 0xFA, 0x01, 0x05, 0x20, 0x9D,
    0x05, 0xB0, 0xB4, 0x39, 0x6C, 0x39, 0xFC, 0x39, 0x00, 0x1C, 0x0D, 0xE0,
    0x62, 0x75, 0x01, 0x05, 0x62, 0x9D, 0x0F, 0xE0, 0x26, 0x40, 0x53, 0xFA,
    0x00, 0x16, 0x51, 0xF0, 0xFC, 0x31, 0x5D, 0xC1, 0x39, 0x40, 0xE8, 0xF8,
    0x6C, 0x31, 0x60, 0x9D, 0x00, 0x14, 0x03, 0xF0, 0x00, 0x15, 0xEF, 0xF0,
    0x6C, 0x39, 0xFC, 0x39, 0x01, 0x19, 0x68, 0x99, 0x68, 0x71, 0x90, 0x49,
    0xFE, 0xF1, 0x6A, 0x71, 0x7C, 0xAE, 0x02, 0x04, 0x6C, 0x71, 0x7C, 0xAE,
    0x02, 0x04, 0xE5, 0xE7, 0x61, 0x23, 0x61, 0x27, 0x04, 0x02, 0x4A, 0xA5,
    0x40, 0xC3, 0x62, 0x9D, 0x02, 0x0A, 0x4A, 0xA5, 0x04, 0x02, 0x00, 0x1C,
    0x12, 0xE0, 0x05, 0xB4, 0xA0, 0xC1, 0x20, 0x75, 0x0F, 0x15, 0xC2, 0xFA,
    0x01, 0x05, 0x20, 0x9D, 0x05, 0xB0, 0xB4, 0x39, 0xFC, 0x39, 0x6C, 0x39,
    0x00, 0x1C, 0x0D, 0xE0, 0x62, 0x75, 0x01, 0x05, 0x62, 0x9D, 0x0F, 0xE0,
    0x26, 0x40, 0x1B, 0xFA, 0x00, 0x16, 0x19, 0xF0, 0xFC, 0x31, 0x25, 0xC1,
    0x39, 0x40, 0xE8, 0xF8, 0x6C, 0x31, 0x60, 0x9D, 0x00, 0x14, 0x03, 0xF0,
    0x00, 0x15, 0xEF, 0xF0, 0xFC, 0x39, 0x6C, 0x39, 0x7C, 0xA4, 0x64, 0x99,
    0x02, 0x04, 0x7C, 0xA4, 0x66, 0x99, 0x02, 0x04, 0x13, 0xC1, 0x68, 0x99,
    0x68, 0x71, 0x90, 0x49, 0xFE, 0xF1, 0xE5, 0xE7, 0x01, 0x00, 0x7A, 0xE7,
    0x07, 0xB0, 0x0D, 0xC5, 0xA0, 0x9F, 0x0A, 0xC0, 0x00, 0x71, 0x99, 0x48,
    0x00, 0x99, 0x02, 0xC1, 0x00, 0xB9, 0xC6, 0x51, 0xC0, 0xE9, 0x01, 0xF0,
    0xFF, 0xD9, 0x6C, 0xDC, 0x6C, 0xE4, 0,    0};

uint8_t polling_bit[4] = {0x0, 0x02, 0, 0};

uint8_t hwusb_init(uint16_t vendor_id, uint16_t product_id) {

  // just for local
  libusb_device *dev = NULL;
  struct libusb_device_descriptor desc;

  int r, i;
  ssize_t cnt;

  r = libusb_init(&ctx);
  if (r < 0) {
    fprintf(stderr, "Failed to initialize libusb: %s\n", libusb_error_name(r));
    return EXIT_FAILURE;
  }

  libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);

  cnt = libusb_get_device_list(ctx, &devs);
  if (cnt < 0) {
    fprintf(stderr, "Failed to get device list: %s\n",
            libusb_error_name((int)cnt));
    libusb_exit(ctx);
    return EXIT_FAILURE;
  }
  fprintf(stderr, "Found %zd USB devices\n", cnt);

  for (i = 0; (dev = devs[i]) != NULL; i++) {
    r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
      fprintf(stderr, "Failed to get device descriptor: %s\n",
              libusb_error_name(r));
      continue;
    }

    fprintf(stderr, "Device %d: VID=0x%04X, PID=0x%04X\n", i, desc.idVendor,
            desc.idProduct);

    if (desc.idVendor == vendor_id && desc.idProduct == product_id) {
      fprintf(stderr, "Found target device!\n");

      r = libusb_open(dev, &handle);
      if (r < 0) {
        fprintf(stderr, "Failed to open device: %s\n", libusb_error_name(r));
        continue;
      }

      fprintf(stderr, "Device opened successfully!\n");

      if (libusb_kernel_driver_active(handle, 0) == 1) {
        fprintf(stderr, "Kernel driver is active, detaching...\n");
        r = libusb_detach_kernel_driver(handle, 0);
        if (r < 0) {
          fprintf(stderr, "Failed to detach kernel driver: %s\n",
                  libusb_error_name(r));
        }
      }

      r = libusb_claim_interface(handle, 0);
      if (r < 0) {
        fprintf(stderr, "Failed to claim interface: %s\n",
                libusb_error_name(r));
        libusb_close(handle);
        handle = NULL;
        return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}

uint8_t hwusb_exit(void) {
  if (handle) {
    libusb_release_interface(handle, 0);
    int r = libusb_attach_kernel_driver(handle, 0);
    if (r < 0) {
      fprintf(stderr, "Failed to attach kernel driver: %s\n",
              libusb_error_name(r));
    }
    libusb_close(handle);
    handle = NULL;
  }
  if (devs) {
    libusb_free_device_list(devs, 1);
  }
  if (ctx) {
    libusb_exit(ctx);
  }

  return EXIT_SUCCESS;
}

void hwusb_register_callback(uint8_t num, void (*callback)(void)) {
  switch (num) {
  case 0:
    myusb.ipc0_callback = callback;
    break;
  case 1:
    myusb.ipc1_callback = callback;
    break;
  case 2:
    myusb.ib2soc_callback = callback;
    break;
  default:
    fprintf(stderr, "Invalid callback number: %d\n", num);
    break;
  }
}

uint8_t ib_write(uint8_t mcu, uint16_t address, uint8_t *buffer_address,
                 uint32_t len, uint8_t byte_en) {
  if (!handle)
    return 1;
  if (!buffer_address)
    return 1;
  if (len % 4) {
    fprintf(stderr, "Must be aligned to 4 bytes\n");
    return 1;
  }

  uint8_t r;
  uint32_t count;
  r = libusb_control_transfer(handle, bmRequestType, bRequest, address,
                              (mcu << 8) | byte_en, buffer_address, len,
                              timeout);
  if (r < 0) {
    fprintf(stderr, "control write error.\n");
  } else {
    // printf("send num : %d\n", r);
  }
}

uint8_t ib_read(uint8_t mcu, uint16_t address, uint8_t *buffer_address,
                uint32_t len, uint8_t byte_en) {
  if (!handle)
    return 1;
  if (!buffer_address)
    return 1;
  if (len % 4) {
    fprintf(stderr, "Must be aligned to 4 bytes\n");
    return 1;
  }

  uint8_t r;
  uint32_t count;
  r = libusb_control_transfer(handle, bmRequestType_r, bRequest, address,
                              (mcu << 8) | byte_en, buffer_address, len,
                              timeout);
  if (r < 0) {
    fprintf(stderr, "control write error.\n");
  } else {
    // printf("read num : %d\n", r);
  }
}

uint8_t ipc_write(uint32_t address, uint8_t *buffer_address, uint32_t len,
                  uint8_t byte_en) {
  if (!handle)
    return 1;
  if (!buffer_address)
    return 1;
  if (len % 4 || len == 0) {
    fprintf(stderr, "Must be aligned to 4 bytes\n");
    return 1;
  }

  uint32_t i, j, page, data_count = 0, data_len,
                       loop_count = (((len - 1) / IB_DATA_MAXLEN) + 1);
  uint8_t *buffer_address_temp = buffer_address, byte_en_temp = byte_en;

  ipc_descriptor_st ipc_desc[4] = {0};
  uint32_t dst_address = address;
  uint32_t count;
  uint8_t full_2k = 0;
  uint8_t *temp = (uint8_t *)ipc_desc;
  uint8_t cmd_temp[4] = {0};

  for (i = 0; i < loop_count; i++) {
    // data part
    if (data_count + IB_DATA_MAXLEN <= len) {
      data_len = IB_DATA_MAXLEN;
      byte_en_temp = B_ALL;
      full_2k = 1;
      data_count += IB_DATA_MAXLEN;
    } else {
      data_len = len - data_count;
      byte_en_temp = byte_en;
      full_2k = 0;
    }

    ib_write(USB_MCU, IB_DATA_BUFF_ADDR, buffer_address_temp, data_len,
             byte_en_temp);
    if (full_2k == 1) {
      buffer_address_temp += IB_DATA_MAXLEN;
    }

    // descriptor part
    if (full_2k == 1) {
      for (j = 0; j < 4; j++) {
        ipc_desc[j].pg_sel = j;
        ipc_desc[j].sre_addr = (0xd800 >> 2);
        ipc_desc[j].len = IB_PAGE_SIZE;
        ipc_desc[j].wr = 1;
        ipc_desc[j].dst_addr = dst_address;
        dst_address += IB_PAGE_SIZE;
      }
    } else {
      // printf("%d\n",data_len);
      page = (data_len - 1) / IB_PAGE_SIZE;
      for (j = 0; j < 4; j++) {
        if (j == page) {
          ipc_desc[j].pg_sel = j;
          ipc_desc[j].sre_addr = (0xd800 >> 2);
          ipc_desc[j].len = data_len - (page * IB_PAGE_SIZE);
          ipc_desc[j].wr = 1;
          ipc_desc[j].dst_addr = dst_address;
          dst_address += IB_PAGE_SIZE;
        } else if (j < page) {
          ipc_desc[j].pg_sel = j;
          ipc_desc[j].sre_addr = (0xd800 >> 2);
          ipc_desc[j].len = IB_PAGE_SIZE;
          ipc_desc[j].wr = 1;
          ipc_desc[j].dst_addr = dst_address;
          dst_address += IB_PAGE_SIZE;
        } else {
          memset(&ipc_desc[j], 0xff, sizeof(ipc_descriptor_st));
        }
      }
    }
    ib_write(PLA_MCU, DESC_ADDR, (uint8_t *)ipc_desc,
             sizeof(ipc_descriptor_st) * 4, B_ALL);

    // for(count = 0;count < 32;count++){
    //     if(count % 8 == 0)
    //         printf("\n");
    //     printf("%2x ",*(temp+count));
    // }
    // printf("\n\n");

    // polling bit
    ib_write(PLA_MCU, DESC_POLL, (uint8_t *)polling_bit, sizeof(polling_bit),
             B_ALL);
    // check polling bit done
    while (1) {
      ib_read(PLA_MCU, DESC_POLL, (uint8_t *)cmd_temp, sizeof(cmd_temp), B_ALL);
      printf("cmd_temp[] = %x %x %x %x\n", cmd_temp[0], cmd_temp[1],
             cmd_temp[2], cmd_temp[3]);
      if (cmd_temp[1] == 0)
        break;
    }
  }
  return 0;
}

uint8_t ipc_read(uint32_t address, uint8_t *buffer_address, uint32_t len,
                 uint8_t byte_en) {
  if (!handle)
    return 1;
  if (!buffer_address)
    return 1;
  if (len % 4 || len == 0) {
    fprintf(stderr, "Must be aligned to 4 bytes\n");
    return 1;
  }

  uint32_t i, j, page, data_count = 0, data_len,
                       loop_count = (((len - 1) / IB_DATA_MAXLEN) + 1);
  uint8_t *buffer_address_temp = buffer_address, byte_en_temp = byte_en;

  ipc_descriptor_st ipc_desc[4] = {0};
  uint32_t dst_address = address;
  uint32_t count;
  uint8_t full_2k = 0;
  uint8_t *temp = (uint8_t *)ipc_desc;
  uint8_t cmd_temp[4] = {0};

  for (i = 0; i < loop_count; i++) {
    // data part
    if (data_count + IB_DATA_MAXLEN <= len) {
      data_len = IB_DATA_MAXLEN;
      byte_en_temp = B_ALL;
      full_2k = 1;
      data_count += IB_DATA_MAXLEN;
    } else {
      data_len = len - data_count;
      byte_en_temp = byte_en;
      full_2k = 0;
    }

    // descriptor part
    if (full_2k == 1) {
      for (j = 0; j < 4; j++) {
        ipc_desc[j].pg_sel = j;
        ipc_desc[j].sre_addr = (0xd800 >> 2);
        ipc_desc[j].len = IB_PAGE_SIZE;
        ipc_desc[j].wr = 1;
        ipc_desc[j].dst_addr = dst_address;
        dst_address += IB_PAGE_SIZE;
      }
    } else {
      printf("%d\n", data_len);
      page = (data_len - 1) / IB_PAGE_SIZE;
      for (j = 0; j < 4; j++) {
        if (j == page) {
          ipc_desc[j].pg_sel = j;
          ipc_desc[j].sre_addr = (0xd800 >> 2);
          ipc_desc[j].len = data_len - (page * IB_PAGE_SIZE);
          ipc_desc[j].wr = 0;
          ipc_desc[j].dst_addr = dst_address;
          dst_address += IB_PAGE_SIZE;
        } else if (j < page) {
          ipc_desc[j].pg_sel = j;
          ipc_desc[j].sre_addr = (0xd800 >> 2);
          ipc_desc[j].len = IB_PAGE_SIZE;
          ipc_desc[j].wr = 0;
          ipc_desc[j].dst_addr = dst_address;
          dst_address += IB_PAGE_SIZE;
        } else {
          memset(&ipc_desc[j], 0xff, sizeof(ipc_descriptor_st));
        }
      }
    }
    ib_write(PLA_MCU, DESC_ADDR, (uint8_t *)ipc_desc,
             sizeof(ipc_descriptor_st) * 4, B_ALL);

    // for(count = 0;count < 32;count++){
    //     if(count % 8 == 0)
    //         printf("\n");
    //     printf("%2x ",*(temp+count));
    // }
    // printf("\n\n");

    // polling bit
    ib_write(PLA_MCU, DESC_POLL, (uint8_t *)polling_bit, sizeof(polling_bit),
             B_ALL);
    // check polling bit done
    while (1) {
      ib_read(PLA_MCU, DESC_POLL, (uint8_t *)cmd_temp, sizeof(cmd_temp), B_ALL);
      printf("cmd_temp[] = %x %x %x %x\n", cmd_temp[0], cmd_temp[1],
             cmd_temp[2], cmd_temp[3]);
      if (cmd_temp[1] == 0)
        break;
    }

    ib_read(USB_MCU, IB_DATA_BUFF_ADDR, buffer_address_temp, data_len,
            byte_en_temp);
    if (full_2k == 1) {
      buffer_address_temp += IB_DATA_MAXLEN;
    }
  }

  return 0;
}

void *usb_interrupt_thread(void *args) {

  int transferred;
  int r;
  sem_init(&sem, 0, 0);

  while (!stop_thread) {
    r = libusb_interrupt_transfer(handle, USB_MCU_INT_EP, share_buffer,
                                  sizeof(share_buffer), &transferred, 0);
    if (r < 0) {
      fprintf(stderr, "Interrupt transfer error: %s\n", libusb_error_name(r));
      continue;
    }
    if (transferred > 0) {
      sem_post(&sem);
      printf("Received interrupt data: ");
      for (int i = 0; i < transferred; i++) {
        printf("%02x ", share_buffer[i]);
      }
      printf("\n");
      if (share_buffer[2] & IPC0) {
        sem_post(&sem);
        myusb.ipc0_callback();
      } else if (share_buffer[2] & IPC1) {
        sem_post(&sem);
        myusb.ipc1_callback();
      } else if (share_buffer[2] & IB2SOC) {
        sem_post(&sem);
        myusb.ib2soc_callback();
      }
    }
  }
  pthread_exit(NULL);
}

void show_descriptor(void) {
  uint32_t count;
  uint8_t buffer_r[32] = {0};
  // read descriptor
  ib_read(PLA_MCU, DESC_ADDR, buffer_r, sizeof(buffer_r), B_ALL);
  for (count = 0; count < 32; count++) {
    if (count % 8 == 0)
      fprintf(stderr, "\n");
    fprintf(stderr, "%2x ", buffer_r[count]);
  }
  fprintf(stderr, "\n");
}