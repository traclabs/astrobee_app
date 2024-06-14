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
** File: astrobee_app.c
**
** Purpose:
**   This file contains the source code for the ros App.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "astrobee_app_events.h"
#include "astrobee_app_version.h"
#include "astrobee_app.h"
#include "astrobee_app_table.h"

#include <string.h>

#include <math.h>

/*
** global data
*/
AstrobeeAppData_t AstrobeeAppData;
struct robot_state_st{
  AstrobeeAppJointState_t state; /**< Twist the robot is currently using **/
  bool is_robot_moving;
} lastRobotState;
bool updated_command;

void HighRateControLoop(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* AstrobeeAppMain() -- Application entry point and main process loop         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void AstrobeeAppMain(void)
{
    int32            status;
    CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(ASTROBEE_APP_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = AstrobeeAppInit();

    if (status != CFE_SUCCESS)
    {
        AstrobeeAppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Runloop
    */
    while (CFE_ES_RunLoop(&AstrobeeAppData.RunStatus) == true)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(ASTROBEE_APP_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&SBBufPtr, AstrobeeAppData.CommandPipe, CFE_SB_PEND_FOREVER);

        if (status == CFE_SUCCESS)
        {
            AstrobeeAppProcessCommandPacket(SBBufPtr);
        }
        else
        {
            CFE_EVS_SendEvent(ASTROBEE_APP_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Astrobee App: SB Pipe Read Error, App Will Exit");

            AstrobeeAppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(ASTROBEE_APP_PERF_ID);
    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(ASTROBEE_APP_PERF_ID);

    CFE_ES_ExitApp(AstrobeeAppData.RunStatus);

} /* End of AstrobeeAppMain() */

void fillJoints(AstrobeeAppJointState_t *_joints, float j0, float j1, float j2, float j3, float j4, float j5, float j6)
{
 _joints->joint_0 = j0;
 _joints->joint_1 = j1;
 _joints->joint_2 = j2;
 _joints->joint_3 = j3;
 _joints->joint_4 = j4;
 _joints->joint_5 = j5;
 _joints->joint_6 = j6;      
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* AstrobeeAppInit() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 AstrobeeAppInit(void)
{
    int32 status;

    AstrobeeAppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    AstrobeeAppData.CmdCounter = 0;
    AstrobeeAppData.ErrCounter = 0;
    AstrobeeAppData.square_counter = 0;
    AstrobeeAppData.hk_counter = 0;

    // updated
    updated_command = false;

    // Initialize telemetry data back to ground
    fillJoints(&AstrobeeAppData.HkTlm.Payload.state, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      
    AstrobeeAppData.HkTlm.Payload.is_robot_moving = false;

    /*
    ** Initialize app configuration data
    */
    AstrobeeAppData.PipeDepth = ASTROBEE_APP_PIPE_DEPTH;

    strncpy(AstrobeeAppData.PipeName, "ASTROBEE_APP_PIPE", sizeof(AstrobeeAppData.PipeName));
    AstrobeeAppData.PipeName[sizeof(AstrobeeAppData.PipeName) - 1] = 0;

    /*
    ** Initialize event filter table...
    */
    AstrobeeAppData.EventFilters[0].EventID = ASTROBEE_APP_STARTUP_INF_EID;
    AstrobeeAppData.EventFilters[0].Mask    = 0x0000;
    AstrobeeAppData.EventFilters[1].EventID = ASTROBEE_APP_COMMAND_ERR_EID;
    AstrobeeAppData.EventFilters[1].Mask    = 0x0000;
    AstrobeeAppData.EventFilters[2].EventID = ASTROBEE_APP_COMMANDNOP_INF_EID;
    AstrobeeAppData.EventFilters[2].Mask    = 0x0000;
    AstrobeeAppData.EventFilters[3].EventID = ASTROBEE_APP_COMMANDMODE_INF_EID;
    AstrobeeAppData.EventFilters[3].Mask    = 0x0000;
    AstrobeeAppData.EventFilters[4].EventID = ASTROBEE_APP_INVALID_MSGID_ERR_EID;
    AstrobeeAppData.EventFilters[4].Mask    = 0x0000;
    AstrobeeAppData.EventFilters[5].EventID = ASTROBEE_APP_LEN_ERR_EID;
    AstrobeeAppData.EventFilters[5].Mask    = 0x0000;
    AstrobeeAppData.EventFilters[6].EventID = ASTROBEE_APP_PIPE_ERR_EID;
    AstrobeeAppData.EventFilters[6].Mask    = 0x0000;

    status = CFE_EVS_Register(AstrobeeAppData.EventFilters, ASTROBEE_APP_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("AstrobeeApp: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_MSG_Init(&AstrobeeAppData.HkTlm.TlmHeader.Msg, CFE_SB_ValueToMsgId(ASTROBEE_APP_HK_TLM_MID), sizeof(AstrobeeAppData.HkTlm));
    CFE_MSG_Init(&AstrobeeAppData.FlightGoal.TlmHeader.Msg, CFE_SB_ValueToMsgId(ASTROBEE_APP_ROBOT_CONTROL_MID), sizeof(AstrobeeAppData.FlightGoal));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&AstrobeeAppData.CommandPipe, AstrobeeAppData.PipeDepth, AstrobeeAppData.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Astrobee App: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ASTROBEE_APP_SEND_HK_MID), AstrobeeAppData.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Astrobee App: Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ASTROBEE_APP_CMD_MID), AstrobeeAppData.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Astrobee App: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);

        return (status);
    }


    /*
    ** Subscribe to flight's robot state data
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ASTROBEE_APP_ROBOT_STATE_MID), AstrobeeAppData.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Astrobee App: Error Subscribing to Odom data, RC = 0x%08lX\n", (unsigned long)status);

        return (status);
    }

    
    /*
    ** Subscribe to High Rate wakeup for sending robot commands on the flight side
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ASTROBEE_APP_HR_CONTROL_MID), AstrobeeAppData.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Astrobee App: Error Subscribing to HR Wakeup Command, RC = 0x%08lX\n", (unsigned long)status);

        return (status);
    }

    CFE_EVS_SendEvent(ASTROBEE_APP_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "Astrobee App Initialized.%s",
                      ASTROBEE_APP_VERSION_STRING);

    return (CFE_SUCCESS);

} /* End of AstrobeeAppInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  AstrobeeAppProcessCommandPacket                                    */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the ros    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void AstrobeeAppProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case ASTROBEE_APP_CMD_MID:
            AstrobeeAppProcessGroundCommand(SBBufPtr);
            break;

        case ASTROBEE_APP_SEND_HK_MID:
            AstrobeeAppReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            break;

        case ASTROBEE_APP_ROBOT_STATE_MID:
            AstrobeeAppProcessRobotState(SBBufPtr);
            break;

        case ASTROBEE_APP_HR_CONTROL_MID:
            HighRateControLoop();
            break;
            
        default:
            CFE_EVS_SendEvent(ASTROBEE_APP_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "astrobee app: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End AstrobeeAppProcessCommandPacket */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* AstrobeeAppProcessGroundCommand() -- astrobee app ground commands          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void AstrobeeAppProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    OS_printf("AstrobeeAppProcessGroundCommand() -- we're getting a ground command...%d\n", CommandCode);

    /*
    ** Process "known" astrobee app ground commands
    */
    switch (CommandCode)
    {
        case ASTROBEE_APP_NOOP_CC:
            if (AstrobeeAppVerifyCmdLength(&SBBufPtr->Msg, sizeof(AstrobeeAppNoopCmd_t)))
            {
                AstrobeeAppNoop((AstrobeeAppNoopCmd_t *)SBBufPtr);
            }

            break;

        case ASTROBEE_APP_MOVE_CC:
            if (AstrobeeAppVerifyCmdLength(&SBBufPtr->Msg, sizeof(AstrobeeAppCmd_t)))
            {
                OS_printf("Updating robot command!!!!!!!!!!!!!!!");
                updateRobotCommand((AstrobeeAppCmd_t *)SBBufPtr);
            }

            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(ASTROBEE_APP_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }


    return;

} /* End of AstrobeeAppProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* AstrobeeAppProcessRobotState() -- astrobee app arm state                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void AstrobeeAppProcessRobotState(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    // Read
    if (AstrobeeAppVerifyCmdLength(&SBBufPtr->Msg, sizeof(AstrobeeAppRobotState_t)))
    {
       AstrobeeAppRobotState_t* state = (AstrobeeAppRobotState_t *)SBBufPtr;
       
       // Fill the lastState
       lastRobotState.state = state->state;
       lastRobotState.is_robot_moving = state->is_robot_moving;                     
    }


    return;

} /* End of AstrobeeAppProcessRobotState() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  AstrobeeAppReportHousekeeping                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 AstrobeeAppReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{    
    /*
    ** Get command execution counters...
    */
    AstrobeeAppData.HkTlm.Payload.CommandErrorCounter = AstrobeeAppData.ErrCounter*2;
    AstrobeeAppData.ErrCounter++;
    AstrobeeAppData.HkTlm.Payload.CommandCounter      = AstrobeeAppData.CmdCounter++;

    OS_printf("AstrobeeAppReportHousekeeping reporting: %d\n", AstrobeeAppData.HkTlm.Payload.CommandCounter);

 
    CFE_SB_TimeStampMsg(&AstrobeeAppData.HkTlm.TlmHeader.Msg);
    CFE_SB_TransmitMsg(&AstrobeeAppData.HkTlm.TlmHeader.Msg, true);

    return CFE_SUCCESS;

} /* End of AstrobeeAppReportHousekeeping() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* AstrobeeAppNoop -- ROS NOOP commands                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 AstrobeeAppNoop(const AstrobeeAppNoopCmd_t *Msg)
{
    CFE_EVS_SendEvent(ASTROBEE_APP_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "astrobee app: NOOP command %s",
                      ASTROBEE_APP_VERSION);

    return CFE_SUCCESS;
} /* End of AstrobeeAppNoop */


int32 updateRobotCommand(const AstrobeeAppCmd_t *Msg)
{
                OS_printf("Updating robot command inside......");

    fillJoints(&AstrobeeAppData.FlightGoal.goal, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   
   switch(Msg->pose_id)
   {
     // Open
     case 0:
     {
        fillJoints(&AstrobeeAppData.FlightGoal.goal, 0.0, 0.0, 0.0, -3.1416, 0.0, 0.0, 0.0);
     } break;
     // All zeros
     case 1:
     {} break;
     // Random
     case 2:
     {
        fillJoints(&AstrobeeAppData.FlightGoal.goal, 1.0, -1.5, 3.0, -3.2, 0.8, 0.5, -1.0);

     } break;       
   }

    updated_command = true;

    CFE_EVS_SendEvent(ASTROBEE_APP_COMMANDMODE_INF_EID, CFE_EVS_EventType_INFORMATION, "astrobee app: Received command %s",
                      ASTROBEE_APP_VERSION);

    return CFE_SUCCESS;
    
}

void HighRateControLoop(void) {
    
    // 1. Publish the twist to State in rosfsw (it is like sending a command to the robot)
    // (we should use another name, telemetry is not supposed to command anything)

    if (updated_command)    
    {
    CFE_SB_TimeStampMsg(&AstrobeeAppData.FlightGoal.TlmHeader.Msg);
    CFE_SB_TransmitMsg(&AstrobeeAppData.FlightGoal.TlmHeader.Msg, true);
    updated_command = false;    
    }

 
    
    // 2. Update the telemetry information to be sent back to the ground        
    struct robot_state_st *st = &lastRobotState;

    AstrobeeAppData.HkTlm.Payload.state = st->state;
    AstrobeeAppData.HkTlm.Payload.is_robot_moving  = st->is_robot_moving;

    // This data is sent when a Housekeeping request is received, 
    // (usually, at a low rate) so nothing sent here
    //memcpy(&st->joints, &AstrobeeAppData.HkTlm.Payload.state, sizeof(AstrobeeAppSSRMS_t) );
    
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* AstrobeeAppVerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool AstrobeeAppVerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    size_t            ActualLength = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode      = 0;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent(ASTROBEE_APP_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        AstrobeeAppData.ErrCounter++;
    }

    return (result);

} /* End of AstrobeeAppVerifyCmdLength() */
