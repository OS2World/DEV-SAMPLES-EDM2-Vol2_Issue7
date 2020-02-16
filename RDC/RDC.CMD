/* rdc.cmd -- Resource decompiler                        940403 */

say 'Operating System/2  Resource Decompiler'
say 'Version 2.00.007 Apr 03 1994'
say '(C) Copyright Martin Lafaix 1994'
say 'All rights reserved.'
say

arg param infile outfile x
fnum = 0; nl = '0d0a'x; includedlg=0

if param = '-H' | param = '' | x \= '' then
   do
      say 'Usage:  rdc [<options>] <.EXE input file> [<.RC output file>]'
      say '        -r              - Extract .res file'
      say '        -h              - Access Help'
      exit
   end

if param = '-R' & infile \= '' then
   do
      outfile = outname(infile,'res')
      if charin(infile,,2) = 'MZ' then base = 1+l2d(charin(infile,61,4))
      else base = 1
      type = charin(infile,base,2)
      if  type \= 'LX' & type \= 'NE' then
         do
            say 'Invalid input file header : 'infile
            exit
         end
      if type = 'NE' then
         do
            cseg = w2d(charin(infile,base+28,2))
            call skip 4
            segtab = readw()
            rsrctab = readw()
            call skip 12
            segshift = readw()
            rsrccnt = readw()
            tabsize = 4
            say 'Reading OS/2 v1.x .EXE file'
         end
      else
         do
            call charin infile,base+44,0
            pageshift = readl()
            call skip 16
            objtab = readl()
            call skip 4
            objmap = readl()
            call skip 4
            rsrctab = readl()
            rsrccnt = readl()
            call skip 40
            datapage = readl()
            tabsize = 14
            say 'Reading OS/2 v2.0 .EXE file'
         end
      do cnt = 0 to rsrccnt-1
         call charin infile,base+rsrctab+cnt*tabsize,0
         if type = 'NE' then call tab16in; else call tab32in
         call resout
      end /* do */
      say nl'Writing binary resources to 'outfile
   end
else
   do
      arg infile outfile
      outfile = outname(infile,'rc2')
      if charin(infile,,1)\= 'FF'x then
         do
            say 'Invalid RES input file : 'infile
            exit
         end
      call charin infile,1,0
      say 'Reading binary resources from .RES file'
      call emit '#include <os2.h>'nl||nl
      do while chars(infile) > 0
         call res2rc
      end /* do */
      if includedlg=1 then
         do 
            outf=outfile; outfile='';
            rcincl=outname(infile,'dlg')
            outfile=outf
            call emit nl'RCINCLUDE 'rcincl||nl
         end /* do */
      say nl'Writing extracted resources to 'outfile
   end 
exit

segin:   /* read segment map entry */
   call charin infile,base+segtab+(arg(1)-1)*8,0
   ssector = readw()
   cb = readw()
   sflags = readw()
   smin = readw()
   pos = 1+(2**segshift)*ssector
   flags = 0
   if bit(sflags,10) then flags = flags+64
   if bit(sflags,12) then flags = flags+16
   if bit(sflags,4) then flags = flags+4096
   if \ bit(sflags,11) then flags = flags+32
   return

tab16in: /* read resource table entry (16bits) */
   etype = readw()
   ename = readw()
   call segin cseg-rsrccnt+1+cnt
   return

objin:   /* read object map entry */
   call charin infile,base+objtab+(arg(1)-1)*24,8
   oflags = readl()
   opagemap = readl()
   omapsize = readl()
   opagedataoffset = l2d(charin(infile,base+objmap+(opagemap-1)*8,4))
   pos = 1+datapage+eoffset+(2**pageshift)*opagedataoffset
   flags = 0
   if bit(oflags,10) then flags = flags+64
   if bit(oflags,11) then flags = flags+16
   if bit(oflags,12) then flags = flags+4096
   if \ bit(oflags,15) then flags = flags+32
   return

tab32in: /* read resource table entry (32bits) */
   etype = readw()
   ename = readw()
   cb = readl()
   eobj = readw()
   eoffset = readl()
   call objin eobj
   return

resout:  /* write resource to outfile */
   call emit 'FF'x||d2w(etype)'FF'x||d2w(ename)d2w(flags)d2l(cb)
   call emit charin(infile,pos,cb)
   call charout ,'.'
   return

res2rc:  /* convert .RES format to .RC */
   call skip 1
   rt = readw()
   call skip 1
   id = readw()
   opt = readw()
   cb = readl()
   select
      when rt = 1  then call emit 'POINTER 'id' 'option()' 'file('ptr')nl
      when rt = 2  then call emit 'BITMAP 'id' 'option()' 'file('bmp')nl
      when rt = 3  then call emit menuout('  ','MENU 'id' 'option()nl'BEGIN'nl)nl'END'nl
      when rt = 4  then includedlg=1
      when rt = 5  then call emit 'STRINGTABLE 'option()nl'BEGIN'strout()'END'nl
      when rt = 6  then call emit 'FONTDIR 'id||nl
      when rt = 7  then call emit 'FONT 'id' 'option()' 'file('fon')nl
      when rt = 8  then do; call emit 'ACCELTABLE 'id' 'option()nl'BEGIN'nl||keyout()'END'nl; end
      when rt = 9  then call emit 'RCDATA 'id||nl'BEGIN'nl'END'nl
      when rt = 10 then call emit 'MESSAGETABLE 'option()nl'BEGIN'strout()'END'nl
      when rt = 11 then do; call emit 'DLGINCLUDE 'id' 'charin(infile,,cb)nl; cb = 0; end
      when rt = 12 then call emit 'VKEYTBL 'id||nl
      when rt = 13 then call emit 'KEYTBL 'id||nl
      when rt = 14 then call emit 'CHARTBL 'id||nl
      when rt = 15 then call emit 'DISPLAYINFO 'id||nl
      when rt = 16 then call emit 'FKASHORT 'id||nl
      when rt = 17 then call emit 'FKALONG 'id||nl
      when rt = 18 then call emit 'HELPTABLE 'id||nl'BEGIN'htout()'END'nl
      when rt = 19 then call emit 'HELPSUBTABLE 'id||hstout()nl
      when rt = 20 then call emit 'FDDIR 'id||nl
      when rt = 21 then call emit 'FD 'id||nl
   otherwise
      call emit 'RESOURCE 'rt' 'id' 'option()' 'file('dat')
   end  /* select */
   call emit nl
   call skip cb
   call charout ,'.'
   return

emit:    /* write data to output file */
   return charout(outfile,arg(1))

option:  /* convert flags to option string */
   if bit(opt,10) then r = 'PRELOAD'; else r = 'LOADONCALL'
   if bit(opt,12) then r = r' MOVEABLE'
   if bit(opt, 4) then r = r' DISCARDABLE'
   if \ (bit(opt,4) | bit(opt,12)) then r = r' FIXED'
   if r = 'LOADONCALL MOVEABLE DISCARDABLE' then r = ''
   return r

file:    /* write cb bytes to res_xxx.arg(1) */
   r = 'res_'right(fnum,4,'0')'.'arg(1)
   call charout r,charin(infile,,cb)
   fnum = fnum+1; cb = 0
   call stream r,'c','close'
   return r

strout:  /* extract strings definitions */
   call skip 2
   id = (id-1)*16; cb = cb-2; r = nl
   do while cb > 0
      len = x2d(c2x(charin(infile,,1)))
      if len > 1 then r = r'  'left(id,8)'"'charin(infile,,len-1)'"'nl
      call skip 1
      id = id+1; cb = cb-len-1
   end /* do */
   return r

itemout: /* extract menu item definition */
   procedure expose nl cb infile outfile
   cb = cb-6; s = ''; a = ''; r = arg(1)'MENUITEM "'; x = '| MIS_'; y = '| MIA_'
   sty = readw()
   att = readw()
   iid = readw()
   if \ (bit(sty,13) | bit(sty,14)) then
      do
         c = charin(infile); cb = cb-1
         if c = 'FF'x & bit(sty,15) then do; r = r'#'readw(); cb = cb-2; end
         else do while c \= '00'x; r = r||c; c = charin(infile); cb = cb-1; end
      end
   if bit(sty,15) then s = s x'BITMAP'
   if bit(sty,14) then s = s x'SEPARATOR'
   if bit(sty,13) then s = s x'OWNERDRAW'
   if bit(sty,12) then s = s x'SUBMENU'
   if bit(sty,11) then s = s x'MULTMENU'
   if bit(sty,10) then s = s x'SYSCOMMAND'
   if bit(sty, 9) then s = s x'HELP'
   if bit(sty, 8) then s = s x'STATIC'
   if bit(sty, 7) then s = s x'BUTTONSEPARATOR'
   if bit(sty, 6) then s = s x'BREAK'
   if bit(sty, 5) then s = s x'BREAKSEPARATOR'
   if bit(sty, 4) then s = s x'GROUP'
   if bit(sty, 3) then s = s x'SINGLE'
   if bit(att,11) then a = a y'NODISMISS'
   if bit(att, 4) then a = a y'FRAMED'
   if bit(att, 3) then a = a y'CHECKED'
   if bit(att, 2) then a = a y'DISABLED'
   if bit(att, 1) then a = a y'HILITED'
   if a \= '' then a = ','substr(a,3)
   if s \= '' then s = ','substr(s,3); else if a \= '' then s = ','
   call emit r'", 'iid||s||a||nl
   if bit(sty,12) then do; call emit arg(1)'BEGIN'nl; call emit menuout(arg(1)'  ','')arg(1)'END'nl; end
   return

menuout: /* extract menus definitions */
   procedure expose nl cb infile outfile
   cb = cb-10;
   cbs = readw()
   typ = readw()
   cp = readw()
   off = readw()
   cnt = readw()
   if arg(2) \= '' then
      do
         if cp \= 850 then call emit 'CODEPAGE 'cp||nl
         call emit arg(2)
      end /* do */
   do cnt; call itemout arg(1); end
   return ''

keyout:  /* extract acceltable definitions */
   procedure expose nl cb infile outfile
   r = ''
   cnt = readw()
   cp = readw()
   cb = cb-4
   if cp \= 850 then call emit arg(1)'CODEPAGE 'cp||nl
   do cnt
      typ = readw()
      key = readw()
      if \ bit(typ,15) & key >= 32 & key <= 255 then key = '"'d2c(key)'"'; else key = '0x'd2x(key)
      cmd = readw()
      cb = cb-6; t = ''
      if bit(typ,16) then t = t', CHAR'
      if bit(typ,15) then t = t', VIRTUALKEY'
      if bit(typ,14) then t = t', SCANCODE'
      if bit(typ,13) then t = t', SHIFT'
      if bit(typ,12) then t = t', CONTROL'
      if bit(typ,11) then t = t', ALT'
      if bit(typ,10) then t = t', LONEKEY'
      if bit(typ, 8) then t = t', SYSCOMMAND'
      if bit(typ, 7) then t = t', HELP'
      r = r'  'left(key',',8)left(cmd',',8)substr(t,3)nl
   end /* do */
   return ''

htout:   /* extract helptable definitions */
   r = nl
   i = readw()
   do while i \= 0
      r = r'  HELPITEM 'i', 'readw()
      call skip 2
      r = r', 'readw()nl; cb = cb-8
      i = readw()
   end /* do */
   cb = cb-2
   return r

hstout:  /* extract helpsubtable definitions */
   sis = readw()
   if sis \= 2 then r = nl'SUBITEMSIZE 'sis; else r = ''
   r = r||nl'BEGIN'nl; cb = cb-2
   i = readw()
   do while i \= 0
      r = r||'  HELPSUBITEM 'i
      do sis-1; r = r', 'readw(); end
      cb = cb-2*sis; r = r||nl
      i = readw();
   end /* do */
   cb = cb-2
   return r'END'

outname: /* return name made from infile and extension */
   if outfile = '' then
      if lastpos('.',arg(1)) > lastpos('\',arg(1)) then
         outfile = left(arg(1),lastpos('.',arg(1)))arg(2)
      else
         outfile = arg(1)'.'arg(2)
   return outfile

readw:   /* read one word from infile */
   return w2d(charin(infile,,2))

readl:   /* read one long from infile */
   return l2d(charin(infile,,4))

skip:    /* skip arg(1) chars */
   return charin(infile,,arg(1))

bit:     /* return bit arg(2) of arg(1) */
   return substr(x2b(d2x(arg(1),4)), arg(2),1)

w2d:     /* littleendian word to decimal */
   w = c2x(arg(1))
   return x2d(substr(w,3,2)substr(w,1,2))

d2w:     /* decimal to littleendian word */
   w = d2x(arg(1),4)
   return x2c(substr(w,3,2)substr(w,1,2))

l2d:     /* littleendian long to decimal */
   l = c2x(arg(1))
   return x2d(substr(l,7,2)substr(l,5,2)substr(l,3,2)substr(l,1,2))

d2l:     /* decimal to littleindian long */
   l = d2x(arg(1),8)
   return x2c(substr(l,7,2)substr(l,5,2)substr(l,3,2)substr(l,1,2))
