
DB 'Hi'

ORG 100H

ADI 03H
MOV B, A
;INR B
NOP


MOV A, B
NOP
ADD C
MVI H, 0x5A
STA 0x17C2
JMP LABEL
LABEL3: HLT
LABEL: NOP
ADI 03H
MOV D, A
JMP LABEL3