#ifndef __INIFILEOBJ_H__
#define __INIFILEOBJ_H__

#ifndef OS2_INCLUDED
	#define INCL_WINSHELLDATA
	#include <os2.h>
#endif


#ifndef _exp_
   #ifdef __DLL__
      #define _exp_ _Export
   #else
      #define _exp_
   #endif
#endif



class ProfileFile{
   APIRET  Error;
   HINI hini;
   HAB hab;
   PSZ FileName;
   char Close;

public:
   _exp_ ProfileFile(PSZ name,HAB hb);
   _exp_ ProfileFile(HINI H);
   _exp_ ~ProfileFile();

   APIRET GetError(){return Error;}
   HINI GetHandle(){return hini;}

   void _exp_ WriteString(PSZ App,PSZ Key, PSZ String);
   void _exp_ WriteData(PSZ App, PSZ Key, void * Buff, ULONG BLen);
   ULONG _exp_ GetDataSize(PSZ App,PSZ Key);
   void _exp_ GetData(PSZ App,PSZ Key,void * Buff, ULONG & BufMax);
   ULONG _exp_ GetString(PSZ App,PSZ Key,PSZ Buff,ULONG BufMax,PSZ DefArg=NULL);
   LONG _exp_ GetInt(PSZ App,PSZ Key,LONG DefArg=0);
   void _exp_ WriteInt(PSZ App,PSZ Key,LONG Num);
   PRFPROFILE _exp_ Querry();
};


class UserProfile:public ProfileFile{

public:
   UserProfile():ProfileFile(HINI_USERPROFILE){}
};


class SystemProfile:public ProfileFile{

public:
   SystemProfile():ProfileFile(HINI_SYSTEMPROFILE){}
};


#endif
