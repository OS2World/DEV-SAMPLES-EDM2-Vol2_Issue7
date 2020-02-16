/* REXX */
Parse Value MyProc(parms) With r1 r2 r3;
r1 = ParseReturned(r1);
r2 = ParseReturned(r2);
r3 = ParseReturned(r3);
Exit 0;

MyProc: Procedure;
 Parse Arg parms;

 Return Return(r1) Return(r2) Return(r3);

Return:
  Return '&vbar.' &vbar.&vbar. C2X(ARG(1));

ParseReturned:
  Return X2C(SUBSTR(ARG(1), 2));
