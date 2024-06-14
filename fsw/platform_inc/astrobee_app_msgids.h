/************************************************************************
**
**
** File: astrobee_app_msgids.h
**
** Purpose:
**  Define Canadarm App Message IDs
**
** Notes:
**
**
*************************************************************************/
#ifndef _astrobee_app_msgids_h_
#define _astrobee_app_msgids_h_

#include "cfe_msgids.h"

#define CANADARM_APP_CMD_MID     (CFE_PLATFORM_CMD_MID_BASE + 0x47)
#define CANADARM_APP_SEND_HK_MID (CFE_PLATFORM_CMD_MID_BASE + 0x48)
#define CANADARM_APP_ROBOT_STATE_MID (CFE_PLATFORM_CMD_MID_BASE + 0x49)

#define CANADARM_APP_HK_TLM_MID      (CFE_PLATFORM_TLM_MID_BASE + 0x46)
#define CANADARM_APP_ROBOT_CONTROL_MID   (CFE_PLATFORM_TLM_MID_BASE + 0x47)
#define CANADARM_APP_HR_CONTROL_MID  (CFE_PLATFORM_TLM_MID_BASE + 0x48)
#endif /* _astrobee_app_msgids_h_ */

/*********************************/
/* End of File Comment           */
/*********************************/
