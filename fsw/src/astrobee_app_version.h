/************************************************************************
**
**
*************************************************************************/

/*! @file rover_app_version.h
 * @brief Purpose:
 *
 *  The RoverApp header file containing version information
 *
 */

#ifndef ASTROBEE_APP_VERSION_H
#define ASTROBEE_APP_VERSION_H

/* Development Build Macro Definitions */

#define ASTROBEE_APP_BUILD_NUMBER 2 /*!< Development Build: Number of commits since baseline */
#define ASTROBEE_APP_BUILD_BASELINE \
    "v1.2.0-rc1" /*!< Development Build: git tag that is the base for the current development */

/* Version Macro Definitions */

#define ASTROBEE_APP_MAJOR_VERSION 1  /*!< @brief ONLY APPLY for OFFICIAL releases. Major version number. */
#define ASTROBEE_APP_MINOR_VERSION 1  /*!< @brief ONLY APPLY for OFFICIAL releases. Minor version number. */
#define ASTROBEE_APP_REVISION      99 /*!< @brief ONLY APPLY for OFFICIAL releases. Revision version number. */
#define ASTROBEE_APP_MISSION_REV   0  /*!< @brief ONLY USED by MISSION Implementations. Mission revision */

#define ASTROBEE_APP_STR_HELPER(x) #x /*!< @brief Helper function to concatenate strings from integer macros */
#define ASTROBEE_APP_STR(x) \
    ASTROBEE_APP_STR_HELPER(x) /*!< @brief Helper function to concatenate strings from integer macros */

/*! @brief Development Build Version Number.
 * @details Baseline git tag + Number of commits since baseline. @n
 * See @ref cfsversions for format differences between development and release versions.
 */
#define ASTROBEE_APP_VERSION ASTROBEE_APP_BUILD_BASELINE "+dev" ASTROBEE_APP_STR(ASTROBEE_APP_BUILD_NUMBER)

/*! @brief Development Build Version String.
 * @details Reports the current development build's baseline, number, and name. Also includes a note about the latest
 * official version. @n See @ref cfsversions for format differences between development and release versions.
 */
#define ASTROBEE_APP_VERSION_STRING                       \
    " ASTROBEE APP DEVELOPMENT BUILD " ASTROBEE_APP_VERSION \
    ", Last Official Release: v1.1.0" /* For full support please use this version */

#endif /* ASTROBEE_APP_VERSION_H */

/************************/
/*  End of File Comment */
/************************/
