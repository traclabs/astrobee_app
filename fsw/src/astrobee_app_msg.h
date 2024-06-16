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
**  Define Astrobee App Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _astrobee_app_msg_h_
#define _astrobee_app_msg_h_

/**
 * AstrobeeApp command codes
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
} AstrobeeAppNoArgsCmd_t;


typedef struct
{
  float x;
  float y;
  float z;
  float qx;
  float qy;
  float qz;
  float qw;
  char frame[20];
} AstrobeeAppPose_t;

typedef struct
{
   CFE_MSG_CommandHeader_t CmdHeader;
   uint8 pose_id;
   char cmd_name[20];
   char cmd_id[20];
   char cmd_src[20];
   char cmd_origin[20];
   char subsys_name[20];
   // Args
   char frame[20];
   float x;
   float y;
   float z;
   float qx;
   float qy;
   float qz;
      
} AstrobeeAppCmd_t;

/*
** The following commands all share the "NoArgs" format
**
** They are each given their own type name matching the command name, which
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef AstrobeeAppNoArgsCmd_t AstrobeeAppNoopCmd_t;

/*************************************************************************/

typedef struct
{
    uint8 CommandErrorCounter;
    uint8 CommandCounter;
    AstrobeeAppPose_t state;
    bool is_robot_moving;
} AstrobeeAppHkTlmPayload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t  TlmHeader; /**< \brief Telemetry header */
    AstrobeeAppHkTlmPayload_t Payload;   /**< \brief Telemetry payload */
} AstrobeeAppHkTlm_t;


// These 2 messages are for communication with the robot on FSW side
typedef struct
{
    CFE_MSG_TelemetryHeader_t  TlmHeader; /**< \brief Telemetry header */
    AstrobeeAppCmd_t command; /**< Command being sent (undock or move) **/

} AstrobeeAppRobotCommand_t;

typedef struct
{
    CFE_MSG_CommandHeader_t  CmdHeader; /**< \brief Command header */
    AstrobeeAppPose_t state; /**< Current pose of the robot **/
    bool is_robot_moving;
} AstrobeeAppRobotState_t;


#endif /* _astrobee_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/



















