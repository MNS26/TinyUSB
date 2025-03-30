#include "tusb_option.h"

#if (CFG_TUD_ENABLED && CFG_TUD_PTP)
#include "device/dcd.h"         // for faking dcd_event_xfer_complete
#include "device/usbd.h"
#include "device/usbd_pvt.h"

#include "ptp_device.h"

void ptp_init(void) {
}

bool ptp_deinit(void) {
  return true;
}

void ptp_reset(uint8_t rhport) {
}

uint16_t ptp_open(uint8_t rhport, tusb_desc_interface_t const * itf_desc, uint16_t max_len) {
  return 0;
}

bool ptp_control_xfer_cb (uint8_t rhport, uint8_t stage, tusb_control_request_t const * p_request) {
  return true;
}

bool ptp_xfer_cb (uint8_t rhport, uint8_t ep_addr, xfer_result_t event, uint32_t xferred_bytes) {
  return true;
}

#endif