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
** File: astrobee_app_msg.h
**
** Purpose:
**  Define Canadarm App Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _astrobee_app_msg_h_
#define _astrobee_app_msg_h_

/**
 * CanadarmApp command codes
 */
#define ASTROBEE_APP_NOOP_CC        0
#define ASTROBEE_APP_MOVE_CC   1

/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct 
{
   CFE_MSG_CommandHeader_t CmdHeader;
} CanadarmAppNoArgsCmd_t;


typedef struct
{
  float joint_0;
  float joint_1;
  float joint_2;
  float joint_3;
  float joint_4;
  float joint_5;
  float joint_6;              
} CanadarmAppJointState_t;

typedef struct
{
   CFE_MSG_CommandHeader_t CmdHeader;
   uint8 pose_id;
} CanadarmAppCmd_t;

/*
** The following commands all share the "NoArgs" format
**
** They are each given their own type name matching the command name, which
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef CanadarmAppNoArgsCmd_t CanadarmAppNoopCmd_t;

/*************************************************************************/

typedef struct
{
    uint8 CommandErrorCounter;
    uint8 CommandCounter;
    CanadarmAppJointState_t state;
    bool is_robot_moving;
} CanadarmAppHkTlmPayload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t  TlmHeader; /**< \brief Telemetry header */
    CanadarmAppHkTlmPayload_t Payload;   /**< \brief Telemetry payload */
} CanadarmAppHkTlm_t;


// These 2 messages are for communication with the robot on FSW side
typedef struct
{
    CFE_MSG_TelemetryHeader_t  TlmHeader; /**< \brief Telemetry header */
    CanadarmAppJointState_t goal; /**< Twist currently being applied **/

} CanadarmAppRobotCommand_t;

typedef struct
{
    CFE_MSG_CommandHeader_t  CmdHeader; /**< \brief Command header */
    CanadarmAppJointState_t state; /**< Twist the robot is currently using **/
    bool is_robot_moving;
} CanadarmAppRobotState_t;


#endif /* _astrobee_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/



















