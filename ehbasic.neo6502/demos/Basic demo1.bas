1 REM DEMO
5 ? RND(100)
10 CLS
20 VDU %10000011
25 X = 0
30 PIXEL RND(0)*320,RND(0)*240
35 MOVE RND(0)*280,RND(0)*220
40 PRINT "Hello world"
50 COLOR RND(0)*7
60 IF RND(0)*10>6 GOTO 90
70 PIXEL RND(0)*320,RND(0)*240
80 GOTO 100
90 CIRCLE RND(0)*320,RND(0)*240,RND(0)*2+1,1
100 MOVE 160,120
110 DRAW RND(0)*320,RND(0)*240
120 PIXEL RND(0)*320,RND(0)*240
130 REFRESH
140 X=X+1
150 IF X=50 GOTO 10
160 GOTO 30