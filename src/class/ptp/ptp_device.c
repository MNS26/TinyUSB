#include "tusb_option.h"

#if (CFG_TUD_ENABLED && CFG_TUD_PTP)
#include "device/dcd.h"         // for faking dcd_event_xfer_complete
#include "device/usbd.h"
#include "device/usbd_pvt.h"

#include "ptp_device.h"

// Level where CFG_TUSB_DEBUG must be at least for this driver is logged
#ifndef CFG_TUD_MSC_LOG_LEVEL
  #define CFG_TUD_MSC_LOG_LEVEL   CFG_TUD_LOG_LEVEL
#endif

#define TU_LOG_DRV(...)   TU_LOG(CFG_TUD_MSC_LOG_LEVEL, __VA_ARGS__)

enum {
  PTP_STAGE_CMD  = 0,
  PTP_STAGE_DATA,
  PTP_STAGE_STATUS,
  PTP_STAGE_STATUS_SENT,
  PTP_STAGE_NEED_RESET,
};

typedef struct {

  uint8_t itf_num;
  uint8_t ep_notif;
  uint8_t ep_in;
  uint8_t ep_out;

  // Bulk Only Transfer (BOT) protovol
  uint8_t stage;

  uint32_t total_length; //bytes to be transferred
  uint32_t transferred_length; // number of bytes tansferred so far

} ptpd_interface_t;

static ptpd_interface_t _ptp_itf;

CFG_TUD_MEM_SECTION static struct {
  TUD_EPBUF_DEF(buf, CFG_TUD_PTP_EP_BUFSIZE);
} _ptpd_epbuf;








void ptp_init(void) {
  tu_memclr(&_ptp_itf,sizeof(ptpd_interface_t));
}

bool ptp_deinit(void) {
  return true;
}

void ptp_reset(uint8_t rhport) {
  (void)rhport;
  tu_memclr(&_ptp_itf, sizeof(ptpd_interface_t));
}

uint16_t ptp_open(uint8_t rhport, tusb_desc_interface_t const * itf_desc, uint16_t max_len) {
  // Check if we are PTP and BOT protocol
  TU_VERIFY(TUSB_CLASS_IMAGE         == itf_desc->bInterfaceClass &&
            PTP_SUBCLASS_STILL_IMAGE == itf_desc->bInterfaceSubClass &&
            PTP_PROTOCOL_BOT         == itf_desc->bInterfaceProtocol, 0);

  ptpd_interface_t* p_ptp = &_ptp_itf;
  p_ptp->itf_num = itf_desc->bInterfaceNumber;

  uint16_t drv_len = sizeof(tusb_desc_interface_t) + (itf_desc->bNumEndpoints * sizeof(tusb_desc_endpoint_t));
  TU_ASSERT(max_len>= drv_len, 0); // Max length must be at least 1 interface + 3 endpoints

  const uint8_t* p_desc = tu_desc_next(itf_desc);
  const tusb_desc_endpoint_t* desc_ep = {0};
  desc_ep = (const tusb_desc_endpoint_t*) p_desc;
    
  // OPen data endpoint pair
  TU_ASSERT(usbd_open_edpt_pair(rhport, p_desc, 2, TUSB_XFER_BULK, &p_ptp->ep_out, &p_ptp->ep_in), 0);

  // do twice since EP 1 & 2 are data in/out
  p_desc = tu_desc_next(p_desc);
  p_desc = tu_desc_next(p_desc);

  // Open notification endpoint
  desc_ep = (const tusb_desc_endpoint_t*) p_desc;
  p_ptp->ep_notif= desc_ep->bEndpointAddress;

  TU_ASSERT(usbd_edpt_open(rhport, desc_ep), 0);
    
  // IDK if there needs to be more here or not
  return drv_len;
}

static void proc_bot_reset(ptpd_interface_t* p_ptp) {
  p_ptp->stage              = PTP_STAGE_CMD;
  p_ptp->total_length       = 0;
  p_ptp->transferred_length = 0;
}


bool ptp_control_xfer_cb (uint8_t rhport, uint8_t stage, tusb_control_request_t const * request) {
  if (stage != CONTROL_STAGE_SETUP) {
    return true;
  }

  ptpd_interface_t* p_ptp = &_ptp_itf;

  if (TUSB_REQ_TYPE_STANDARD     == request->bmRequestType_bit.type      &&
      TUSB_REQ_RCPT_ENDPOINT     == request->bmRequestType_bit.recipient &&
      TUSB_REQ_CLEAR_FEATURE     == request->bRequest                    &&
      TUSB_REQ_FEATURE_EDPT_HALT == request->wValue ) {
    uint8_t const ep_addr = tu_u16_low(request->wIndex);

    if (p_ptp->stage == PTP_STAGE_NEED_RESET) {

      usbd_edpt_stall(rhport, ep_addr);
    } else {
      if (ep_addr == p_ptp->ep_in) {
        if (p_ptp->stage == PTP_STAGE_STATUS) {
          TU_ASSERT("TODO: IMPLEMENT\n");
        }

      } else if (ep_addr == p_ptp->ep_out) {
        if (p_ptp->stage == PTP_STAGE_CMD) {
          if (usbd_edpt_ready(rhport, p_ptp->ep_out)) {
            TU_ASSERT("TODO: IMPLEMENT\n");
          }
        }
      }
    }
    return true;
  }

  // Hadle class req only now
  TU_VERIFY(request->bmRequestType_bit.type == TUSB_REQ_TYPE_CLASS);

  switch (request->bRequest)
  {
  case PTP_REQ_DEVICE_RESET:
    TU_LOG_DRV("  MSC BOT Reset\r\n");
    TU_VERIFY(request->wValue == 0 && request->wLength == 0);
    proc_bot_reset(p_ptp);//reset driver state
    tud_control_status(rhport, request);
  break;
  case PTP_REQ_CANCEL:
    TU_ASSERT("TODO: IMPLEMENT PTP_REQ_CANCEL\n");
    return false;
  break;
  case PTP_REQ_GET_DEVICE_STATUS:
    TU_ASSERT("TODO: IMPLEMENT PTP_REQ_GET_DEVICE_STATUS\n");
    return false;
  break;
  case PTP_REQ_GET_EXTENDED_EVENT_DATA:
    TU_ASSERT("TODO: IMPLEMENT PTP_REQ_GET_EXTENDED_EVENT_DATA\n");
    return false;
  break;
  default:
    return false;
  }
  return true;
}
bool ptp_xfer_cb (uint8_t rhport, uint8_t ep_addr, xfer_result_t event, uint32_t xferred_bytes) {
  TU_ASSERT("TODO: IMPLEMENT %s\n", __func__);
  return false;
}

#endif