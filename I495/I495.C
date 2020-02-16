#define INCL_GPIBITMAPS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINTIMER
#define INCL_WINWINDOWMGR
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sprite.h>
#include "rc.h"

#define CLS_CLIENT               "TestClass"

#define MAX_VEHICLES             10
#define TID_TRAFFIC              1
#define DELTA_X                  5L

#define SIT_CAR                  0x0001L
#define SIT_TRUCK                0x0002L
#define SIT_GOESLEFT             0x0004L
#define SIT_GOESRIGHT            0x0008L

#define SIS_UNUSED               0L
#define SIS_VISIBLE              1L
#define SIS_ENTERTOP             2L
#define SIS_EXITTOP              3L
#define SIS_ENTERBOTTOM          4L
#define SIS_EXITBOTTOM           5L

typedef struct _SPRITEINFO {
   HSPRITE hsSprite;
   SIZEL szlSize;
   ULONG ulType;
   ULONG ulState;
} SPRITEINFO, *PSPRITEINFO;

typedef struct _INSTDATA {
   ULONG ulSzStruct;
   HAB habAnchor;
   HWND hwndFrame;
   LONG lMod;
   HPLAYGROUND hpgPlay;
   SIZEL szlPlay;
   SPRITEINFO asiSprites[MAX_VEHICLES];
} INSTDATA, *PINSTDATA;

VOID cleanBitmaps(HBITMAP *phbmArray,ULONG ulMax)
{
   ULONG ulIndex;

   for (ulIndex=0; ulIndex<ulMax; ulIndex++) {
      if (phbmArray[ulIndex]!=NULLHANDLE) {
         GpiDeleteBitmap(phbmArray[ulIndex]);
      } /* endif */
   } /* endfor */
}

PSPRITEINFO findSprite(PSPRITEINFO psiArray,ULONG ulState,ULONG ulType)
{
   ULONG ulIndex;

   for (ulIndex=0; ulIndex<MAX_VEHICLES; ulIndex++) {
      if ((psiArray[ulIndex].ulState==ulState) &&
          ((psiArray[ulIndex].ulType & ulType)==ulType)) {
         return &psiArray[ulIndex];
      } /* endif */
   } /* endfor */

   return NULL;
}

MRESULT EXPENTRY wndProc(HWND hwndWnd,
                         ULONG ulMsg,
                         MPARAM mpParm1,
                         MPARAM mpParm2)
{
   PINSTDATA pidData;

   pidData=WinQueryWindowPtr(hwndWnd,0);

   switch (ulMsg) {
   case WM_CREATE:
      {
         HPS hpsWnd;
         ULONG ulIndex;
         HBITMAP ahbmVehicles[MAX_VEHICLES];
         HBITMAP hbmBack;
         PSPRITEINFO psiCurrent;
         RECTL rclWnd;

         pidData=calloc(1,sizeof(INSTDATA));
         if (pidData==NULL) {
            WinAlarm(HWND_DESKTOP,WA_ERROR);
            return MRFROMSHORT(TRUE);
         } /* endif */

         pidData->ulSzStruct=sizeof(INSTDATA);

         WinSetWindowPtr(hwndWnd,0,pidData);

         pidData->habAnchor=WinQueryAnchorBlock(hwndWnd);
         pidData->hwndFrame=WinQueryWindow(hwndWnd,QW_PARENT);
         pidData->lMod=15;

         memset(ahbmVehicles,0,sizeof(ahbmVehicles));

         hpsWnd=WinGetPS(hwndWnd);

         for (ulIndex=BMP_RCL; ulIndex<=BMP_BTR; ulIndex++) {
            ahbmVehicles[ulIndex-BMP_RCL]=
               GpiLoadBitmap(hpsWnd,NULLHANDLE,ulIndex,0,0);

            if (ahbmVehicles[ulIndex-BMP_RCL]==NULLHANDLE) {
               break;
            } /* endif */
         } /* endfor */

         if (ulIndex!=BMP_BTR+1) {
            WinReleasePS(hpsWnd);
            cleanBitmaps(ahbmVehicles,MAX_VEHICLES);
            WinAlarm(HWND_DESKTOP,WA_ERROR);
            return MRFROMSHORT(TRUE);
         } /* endif */

         hbmBack=GpiLoadBitmap(hpsWnd,NULLHANDLE,BMP_ROAD,0,0);
         if (hbmBack==NULLHANDLE) {
            WinReleasePS(hpsWnd);
            cleanBitmaps(ahbmVehicles,MAX_VEHICLES);
            WinAlarm(HWND_DESKTOP,WA_ERROR);
            return MRFROMSHORT(TRUE);
         } /* endif */

         WinReleasePS(hpsWnd);

         SprCreatePlayground(pidData->habAnchor,&pidData->hpgPlay);
         if (pidData->hpgPlay==NULLHANDLE) {
            cleanBitmaps(ahbmVehicles,MAX_VEHICLES);
            GpiDeleteBitmap(hbmBack);
            WinAlarm(HWND_DESKTOP,WA_ERROR);
            return MRFROMSHORT(TRUE);
         } /* endif */

         SprSetPlaygroundBack(pidData->hpgPlay,hbmBack,NULL);
         SprQueryPlaygroundSize(pidData->hpgPlay,&pidData->szlPlay);

         for (ulIndex=0; ulIndex<MAX_VEHICLES; ulIndex++) {
            psiCurrent=&pidData->asiSprites[ulIndex];

            SprCreateSprite(pidData->habAnchor,
                            ahbmVehicles[ulIndex],
                            &psiCurrent->hsSprite);
            if (psiCurrent->hsSprite==NULLHANDLE) {
               break;
            } /* endif */

            SprAddSprite(pidData->hpgPlay,psiCurrent->hsSprite);

            SprQuerySpriteSize(psiCurrent->hsSprite,&psiCurrent->szlSize);

            psiCurrent->ulState=SIS_UNUSED;

            switch (BMP_RCL+ulIndex) {
            case BMP_RCL:
            case BMP_BCL:
            case BMP_GCL:
            case BMP_RCR:
            case BMP_BCR:
            case BMP_GCR:
               psiCurrent->ulType=SIT_CAR;
               break;
            case BMP_RTL:
            case BMP_BTL:
            case BMP_RTR:
            case BMP_BTR:
               psiCurrent->ulType=SIT_TRUCK;
               break;
            } /* endswitch */

            switch (BMP_RCL+ulIndex) {
            case BMP_RCL:
            case BMP_BCL:
            case BMP_GCL:
            case BMP_RTL:
            case BMP_BTL:
               psiCurrent->ulType|=SIT_GOESLEFT;
               break;
            case BMP_RCR:
            case BMP_BCR:
            case BMP_GCR:
            case BMP_RTR:
            case BMP_BTR:
               psiCurrent->ulType|=SIT_GOESRIGHT;
               break;
            } /* endswitch */
         } /* endfor */

         if (ulIndex!=MAX_VEHICLES) {
            for (ulIndex=0; ulIndex<MAX_VEHICLES; ulIndex++) {
               if (pidData->asiSprites[ulIndex].hsSprite!=NULLHANDLE) {
                  ahbmVehicles[ulIndex]=NULLHANDLE;
               } /* endif */
            } /* endif */

            cleanBitmaps(ahbmVehicles,MAX_VEHICLES);
            SprDestroyPlayground(pidData->hpgPlay);
            WinAlarm(HWND_DESKTOP,WA_ERROR);
            return MRFROMSHORT(TRUE);
         } /* endif */

         rclWnd.xLeft=0;
         rclWnd.yBottom=0;
         SprQueryPlaygroundSize(pidData->hpgPlay,(PSIZEL)&rclWnd.xRight);

         WinCalcFrameRect(pidData->hwndFrame,&rclWnd,FALSE);

         rclWnd.xRight-=rclWnd.xLeft;
         rclWnd.yTop-=rclWnd.yBottom;

         WinSetWindowPos(pidData->hwndFrame,
                         NULLHANDLE,
                         100,
                         100,
                         rclWnd.xRight,
                         rclWnd.yTop,
                         SWP_MOVE|SWP_SIZE|SWP_SHOW|SWP_ACTIVATE);

         srand(time(NULL));

         WinStartTimer(pidData->habAnchor,hwndWnd,TID_TRAFFIC,25);
      }
      break;
   case WM_DESTROY:
      WinStopTimer(pidData->habAnchor,hwndWnd,TID_TRAFFIC);
      SprDestroyPlayground(pidData->hpgPlay);
      free(pidData);
      break;
   case WM_BUTTON1DOWN:
      SprSetUpdateFlag(pidData->hpgPlay,FALSE);
      break;
   case WM_BUTTON1UP:
      {
         HPS hpsWnd;

         SprSetUpdateFlag(pidData->hpgPlay,TRUE);

         hpsWnd=WinGetPS(hwndWnd);
         SprDrawPlayground(hpsWnd,pidData->hpgPlay);
         WinReleasePS(hpsWnd);
      }
      break;
   case WM_COMMAND:
      switch (SHORT1FROMMP(mpParm1)) {
      case MI_MORECARS:
         pidData->lMod--;
         if (pidData->lMod<1) {
            pidData->lMod=1;
         } /* endif */
         break;
      case MI_LESSCARS:
         pidData->lMod++;
         if (pidData->lMod<30) {
            pidData->lMod=30;
         } /* endif */
         break;
      default:
         return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
      } /* endswitch */
      break;
   case WM_PAINT:
      {
         HPS hpsPaint;
         RECTL rclPaint;

         hpsPaint=WinBeginPaint(hwndWnd,NULLHANDLE,&rclPaint);
         WinFillRect(hpsPaint,&rclPaint,CLR_PALEGRAY);
         SprDrawPlayground(hpsPaint,pidData->hpgPlay);
         WinEndPaint(hpsPaint);
      }
      break;
   case WM_TIMER:
      switch (SHORT1FROMMP(mpParm1)) {
      case TID_TRAFFIC:
         {
            HPS hpsWnd;
            PSPRITEINFO psiSprite;
            POINTL ptlPos;
            ULONG ulIndex;

            hpsWnd=WinGetPS(hwndWnd);

            psiSprite=findSprite(pidData->asiSprites,SIS_ENTERTOP,0);
            if ((psiSprite==NULL) && (rand()%pidData->lMod==0)) {
               psiSprite=findSprite(pidData->asiSprites,
                                    SIS_UNUSED,
                                    SIT_GOESLEFT);
               if (psiSprite!=NULL) {
                  psiSprite->ulState=SIS_ENTERTOP;

                  ptlPos.x=pidData->szlPlay.cx;
                  ptlPos.y=pidData->szlPlay.cy/2+40;

                  SprSetSpritePosition(hpsWnd,psiSprite->hsSprite,&ptlPos);
                  SprSetSpriteVisibility(hpsWnd,psiSprite->hsSprite,TRUE);
               } /* endif */
            } /* endif */

            psiSprite=findSprite(pidData->asiSprites,SIS_ENTERBOTTOM,0);
            if ((psiSprite==NULL) && (rand()%pidData->lMod==0)) {
               psiSprite=findSprite(pidData->asiSprites,
                                    SIS_UNUSED,
                                    SIT_GOESRIGHT);
               if (psiSprite!=NULL) {
                  psiSprite->ulState=SIS_ENTERBOTTOM;

                  ptlPos.x=(-psiSprite->szlSize.cx);
                  ptlPos.y=pidData->szlPlay.cy/2-60;

                  SprSetSpritePosition(hpsWnd,psiSprite->hsSprite,&ptlPos);
                  SprSetSpriteVisibility(hpsWnd,psiSprite->hsSprite,TRUE);
               } /* endif */
            } /* endif */

            for (ulIndex=0; ulIndex<MAX_VEHICLES; ulIndex++) {
               psiSprite=&pidData->asiSprites[ulIndex];

               if (psiSprite->ulState==SIS_UNUSED) {
                  continue;
               } /* endif */

               SprQuerySpritePosition(psiSprite->hsSprite,&ptlPos);

               if ((psiSprite->ulType & SIT_GOESLEFT)!=0) {
                  ptlPos.x-=DELTA_X;

                  if (ptlPos.x+psiSprite->szlSize.cx<0) {
                     psiSprite->ulState=SIS_UNUSED;
                  } else
                  if (ptlPos.x<0) {
                     psiSprite->ulState=SIS_EXITTOP;
                  } else
                  if (ptlPos.x+psiSprite->szlSize.cx>pidData->szlPlay.cx) {
                     psiSprite->ulState=SIS_ENTERTOP;
                  } else {
                     psiSprite->ulState=SIS_VISIBLE;
                  } /* endif */
               } else {
                  ptlPos.x+=DELTA_X;

                  if (ptlPos.x>pidData->szlPlay.cx) {
                     psiSprite->ulState=SIS_UNUSED;
                  } else
                  if (ptlPos.x+psiSprite->szlSize.cx>pidData->szlPlay.cx) {
                     psiSprite->ulState=SIS_EXITBOTTOM;
                  } else
                  if (ptlPos.x<0) {
                     psiSprite->ulState=SIS_ENTERBOTTOM;
                  } else {
                     psiSprite->ulState=SIS_VISIBLE;
                  } /* endif */
               } /* endif */

               if (psiSprite->ulState!=SIS_UNUSED) {
                  SprSetSpritePosition(hpsWnd,psiSprite->hsSprite,&ptlPos);
               } else {
                  SprSetSpriteVisibility(hpsWnd,psiSprite->hsSprite,FALSE);
               } /* endif */
            } /* endfor */

            WinReleasePS(hpsWnd);
         }
         break;
      default:
         return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
      } /* endswitch */
      break;
   default:
      return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
   } /* endswitch */

   return MRFROMSHORT(FALSE);
}

INT main(VOID)
{
   HAB habAnchor;
   HMQ hmqQueue;
   ULONG ulFrame;
   HWND hwndFrame;
   HWND hwndClient;
   BOOL bLoop;
   QMSG qmMsg;

   habAnchor=WinInitialize(0);
   hmqQueue=WinCreateMsgQueue(habAnchor,0);

   WinRegisterClass(habAnchor,CLS_CLIENT,wndProc,CS_SIZEREDRAW,sizeof(PVOID));

   ulFrame=FCF_SYSMENU | FCF_TITLEBAR | FCF_SIZEBORDER | FCF_ACCELTABLE |
              FCF_SHELLPOSITION | FCF_TASKLIST | FCF_ICON;

   hwndFrame=WinCreateStdWindow(HWND_DESKTOP,
                                0,
                                &ulFrame,
                                CLS_CLIENT,
                                "I-495 Demonstration",
                                0,
                                NULLHANDLE,
                                RES_CLIENT,
                                &hwndClient);
   if (hwndFrame!=NULLHANDLE) {
      bLoop=WinGetMsg(habAnchor,&qmMsg,NULLHANDLE,0,0);
      while (bLoop) {
         WinDispatchMsg(habAnchor,&qmMsg);
         bLoop=WinGetMsg(habAnchor,&qmMsg,NULLHANDLE,0,0);
      } /* endwhile */

      WinDestroyWindow(hwndFrame);
   } /* endif */

   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);
   return 0;
}
