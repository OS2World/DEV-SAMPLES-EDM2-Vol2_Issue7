#include "iniflobj.h"
#include <stdio.h>

ProfileFile::ProfileFile(PSZ name,HAB hb){

	hab=hb;
	FileName=name;
	hini=PrfOpenProfile(hab,FileName);
	Close=1;
}

ProfileFile::ProfileFile(HINI H){
	hini=H;
	Close=0;
}

ProfileFile::~ProfileFile(){
	if(Close)
		PrfCloseProfile(hini);
}


void ProfileFile::WriteString(PSZ App,PSZ Key, PSZ String){
	Error = PrfWriteProfileString(hini,App,Key, String);
}

void ProfileFile::WriteData(PSZ App, PSZ Key, void * Buff, ULONG BLen){
	Error= PrfWriteProfileData(hini,App,Key,Buff,BLen);
}

ULONG ProfileFile::GetDataSize(PSZ App,PSZ Key){
	ULONG Size;
	Error=PrfQueryProfileSize(hini,App,Key,&Size);

return Size;
}

void ProfileFile::GetData(PSZ App,PSZ Key,void * Buff, ULONG & BufMax){
	Error= PrfQueryProfileData(hini,App,Key,Buff,&BufMax);
}

ULONG ProfileFile::GetString(PSZ App,PSZ Key,PSZ Buff,ULONG BufMax,PSZ DefArg){

	return PrfQueryProfileString(hini,App,Key,DefArg,Buff,BufMax);
}

LONG ProfileFile::GetInt(PSZ App,PSZ Key,LONG DefArg){
	return PrfQueryProfileInt(hini,App,Key,DefArg);
}

void ProfileFile::WriteInt(PSZ App,PSZ Key,LONG Num){
	char Buff[200];

	sprintf(Buff,"%i",Num);
	WriteString(App,Key,Buff);
}


PRFPROFILE ProfileFile::Querry(){
	PRFPROFILE  Profile; /* Profile names
											structure */

	Error = PrfQueryProfile(hab,
				  &Profile);

return Profile;
}
