/*******************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
** File: astrobee_app.h
**
** Purpose:
**   This file is main hdr file for the ros application.
**
**
*******************************************************************************/

#ifndef _astrobee_app_h_
#define _astrobee_app_h_

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "astrobee_app_perfids.h"
#include "astrobee_app_msgids.h"
#include "astrobee_app_msg.h"

// #include "astrobee_app_msgids.h"

/***********************************************************************/
#define ASTROBEE_APP_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */
/************************************************************************
** Type Definitions
*************************************************************************/

/*
** Global Data
*/

typedef struct
{
    /*
    ** Command interface counters...
    */
    uint8 CmdCounter;
    uint8 ErrCounter;

    uint32 square_counter;
    uint32 hk_counter;

    //Housekeeping telemetry packet...
    AstrobeeAppHkTlm_t HkTlm;
    // Goal joint state sent to robot on flight side
    AstrobeeAppRobotCommand_t FlightGoal;
    
    // Run Status variable used in the main processing loop
    uint32 RunStatus;

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t CommandPipe;

    /*
    ** Initialization data (not reported in housekeeping)...
    */
    char   PipeName[CFE_MISSION_MAX_API_LEN];
    uint16 PipeDepth;

    CFE_EVS_BinFilter_t EventFilters[ASTROBEE_APP_EVENT_COUNTS];

} AstrobeeAppData_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (AstrobeeAppMain), these
**       functions are not called from any other source module.
*/
void  AstrobeeAppMain(void);

int32 AstrobeeAppInit(void);

void  AstrobeeAppProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);
void  AstrobeeAppProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);

int32 AstrobeeAppReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg);
void AstrobeeAppProcessRobotState(CFE_SB_Buffer_t *SBBufPtr);

int32 AstrobeeAppNoop(const AstrobeeAppNoopCmd_t *Msg);
int32 updateRobotCommand(const AstrobeeAppCmd_t *Msg);

bool AstrobeeAppVerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);
void fillJoints(AstrobeeAppJointState_t *_joints, float j0, float j1, float j2, float j3, float j4, float j5, float j6);

#endif /* _ASTROBEE_APP_h_ */
