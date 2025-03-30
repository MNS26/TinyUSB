#pragma once
#ifndef _TUSB_PTP_DEVICE_H_
#define _TUSB_PTP_DEVICE_H_

#include "common/tusb_common.h"
#include "ptp.h"

#ifdef __cplusplus
 extern "C" {
#endif

void     ptp_init            (void);
bool     ptp_deinit          (void);
void     ptp_reset           (uint8_t rhport);
uint16_t ptp_open            (uint8_t rhport, tusb_desc_interface_t const* itf_desc, uint16_t max_len);
bool     ptp_control_xfer_cb (uint8_t rhport, uint8_t stage, tusb_control_request_t const * p_request);
bool     ptp_xfer_cb         (uint8_t rhport, uint8_t ep_addr, xfer_result_t event, uint32_t xferred_bytes);
#ifdef __cplusplus
 }
#endif
#endif /* _TUSB_PTP_DEVICE_H_ */