#include "tusb_option.h"

#if (CFG_TUH_ENABLED && CFG_TUH_PTP)

#include "host/usbh.h"
#include "host/usbh_pvt.h"

#include "ptp_host.h"

static void process_set_config(tuh_xfer_t* xfer);


#endif