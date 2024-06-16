/************************************************************************
**
**
** File: astrobee_app_msgids.h
**
** Purpose:
**  Define Astrobee App Message IDs
**
** Notes:
**
**
*************************************************************************/
#ifndef _astrobee_app_msgids_h_
#define _astrobee_app_msgids_h_

#include "cfe_msgids.h"

#define ASTROBEE_APP_CMD_MID     (CFE_PLATFORM_CMD_MID_BASE + 0x47)
#define ASTROBEE_APP_SEND_HK_MID (CFE_PLATFORM_CMD_MID_BASE + 0x48)
#define ASTROBEE_APP_ROBOT_STATE_MID (CFE_PLATFORM_CMD_MID_BASE + 0x49)

#define ASTROBEE_APP_HK_TLM_MID      (CFE_PLATFORM_TLM_MID_BASE + 0x46)
#define ASTROBEE_APP_ROBOT_CONTROL_MID   (CFE_PLATFORM_TLM_MID_BASE + 0x47)
#define ASTROBEE_APP_HR_CONTROL_MID  (CFE_PLATFORM_TLM_MID_BASE + 0x48)
#endif /* _astrobee_app_msgids_h_ */

/*********************************/
/* End of File Comment           */
/*********************************/
