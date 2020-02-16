#define INCL_DOSMISC
#include <os2.h>
#include <stdio.h>

#define MAX_MSG_LENGTH 1024

void main()
{
   APIRET  rc;

   UCHAR  **ppIvTable;
   ULONG    ulIvCount = 0;
   UCHAR    szErrorMesg[MAX_MSG_LENGTH];
   ULONG    ulMaxMsgLength = MAX_MSG_LENGTH - 1;
   ULONG    ulMessage;
   UCHAR    szFileName[]={"oso001.msg"};
   ULONG    ulActMsgLength;

   printf( "Give messagenr. : " ); /* Error number returned by a function e.g. DosRead */
   scanf( "%d", &ulMessage );

   rc = DosGetMessage( ppIvTable,
                       ulIvCount,
                       szErrorMesg,
                       ulMaxMsgLength,
                       ulMessage,
                       szFileName,
                       &ulActMsgLength );

   printf( "\nERROR MESSAGE :\n%s\n", szErrorMesg );
}
