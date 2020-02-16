/***************************************************************************/
/* This command file "installs" this issue of EDM/2 on your machine.       */
/*                                                                         */
/* Change history:                                                         */
/*                                                                         */
/* October 1993 - Initial release. (Version 1.00)                          */
/* November 1993 - Completely rewritten by Gordon W. Zeglinski.            */
/*                 Modularization and minor modifications made by Larry    */
/*                 Salomon. (Version 2.00)                                 */
/* November 1993 - Use the built-in Directory() function instead of our    */
/*                 home-grown version (Version 2.01)                       */
/***************************************************************************/

/***************************************************************************/
/* Load the RexxUtil functions                                             */
/***************************************************************************/
Call RxFuncAdd 'SysLoadFuncs', 'REXXUTIL', 'SysLoadFuncs'
Call SysLoadFuncs

Say "EDM/2 Installation program  Version 2.01"
Say

"@ECHO OFF"
Parse Upper Arg VOLUME ISSUE

/***************************************************************************/
/* Check for help and for default issue installation                       */
/***************************************************************************/
If VOLUME="?" Then Do
   Say "Syntax:  INSTALL [volume issue]"
   Say
   Say "If not specified, this installs the current issue of EDM/2 in a"
   Say "folder on your machine.  Explicitly specifying the volume and"
   Say "issue is for reinstallation."
   Exit 1
End

If (VOLUME="") | (ISSUE="") Then Call SetVars

/***************************************************************************/
/* Because version 1.00 did not set the object id, we want to avoid        */
/* creating two folders with the same title, so set the folder title       */
/* to something different, state the new folder title in the diagnostic    */
/* message, and hope the users pick up on the change.                      */
/***************************************************************************/
FOLDERNAME="EDM/2 Issues"

Say "Attempting to install volume" VOLUME "issue" ISSUE "to folder '"|| ,
   FOLDERNAME||"'."

/***************************************************************************/
/* If the folder doesn't exist, create it.                                 */
/***************************************************************************/
If CheckFolder(FOLDERNAME)=0 Then Do
   Say "EDM/2 folder did not exist and could not be created.  Stop."
   Exit 2
End

/***************************************************************************/
/* Install the issue.  Note that we do not install the icon for the user.  */
/***************************************************************************/
If InstallIssue(VOLUME ISSUE)=0 Then Do
   Say "The issue could not be installed in the EDM/2 folder.  Stop."
   Exit 2
End

Say "Installation was successful."
Exit 0

CheckFolder:  Procedure
/***************************************************************************/
/* SysSetObjectData() relies on the fact that the folder was previously    */
/* created with an object id of "EDM/2_FOLDER".  If this call fails, then  */
/* we can assume that either 1) the folder doesn't exist or 2) that the    */
/* folder does exist but was created with the original INSTALL.CMD .       */
/***************************************************************************/
Parse Arg FOLDERNAME

If SysSetObjectData("<EDM/2_FOLDER>"," ")=0 Then Do
   Say "The EDM/2 folder was not found.  Attempting to create it."
   Return SysCreateObject("WPFolder", ,
                          FOLDERNAME, ,
                          "<WP_DESKTOP>", ,
                          "OBJECTID=<EDM/2_FOLDER>")
End

Return 1

InstallIssue:  Procedure
Parse Arg VOLUME ISSUE .

/***************************************************************************/
/* Determine the current directory.  Why the hell is there a SysMkDir()    */
/* and a SysRmDir() but no SysQueryCurrentDir() (or something like         */
/* that)???                                                                */
/***************************************************************************/
CURDIR=Directory()

Parse Upper Value CURDIR With CURDIR

ISSUEFILE=CURDIR||"\EDMI"||VOLUME||"-"||ISSUE||".INF"
ICONFILE=CURDIR||"\EDMI.ICO"

PRGOPTION="PROGTYPE=PM;EXENAME=VIEW.EXE;PARAMETERS="""||ISSUEFILE||""";"
PRGOPTION=PRGOPTION||"ICONFILE="||ICONFILE

Return SysCreateObject("WPProgram", ,
                       "Volume" VOLUME "issue" ISSUE, ,
                       "<EDM/2_FOLDER>", ,
                       PRGOPTION, ,
                       "REPLACE")

SetVars:
/*****************************************************************************/
/* This function sets the VOLUME and ISSUE variables used by the rest of the */
/* program and is generated by 'newissue.cmd'                                */
/*****************************************************************************/
VOLUME=2 
ISSUE=7 
Return 
