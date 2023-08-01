; NAME: Get Random Item No Message
; GAMES: MP3_USA
; EXECUTION: Direct

; Custom Edit of Gwonam's event to not display messages.

ADDIU SP SP -48 ; Including 4 byte pad
SW RA 40(SP)
SW S2 36(SP)
SW S1 32(SP)
SW S0 28(SP)

; BEGIN DETERMINE PLAYER CODE
; This code block makes sure that the current player is the one
; getting the item. Written by PartyPlanner64.

JAL GetCurrentPlayerIndex
NOP

ADDU S0 R0 R0

LUI S1 hi(p1_item1)
ADDIU S1 S1 lo(p1_item1)

BEQ S0 V0 determineslot
NOP
ADDIU S0 S0 1

LUI S1 hi(p2_item1)
ADDIU S1 S1 lo(p2_item1)

BEQ S0 V0 determineslot
NOP
ADDIU S0 S0 1

LUI S1 hi(p3_item1)
ADDIU S1 S1 lo(p3_item1)

BEQ S0 V0 determineslot
NOP
ADDIU S0 S0 1

LUI S1 hi(p4_item1)
ADDIU S1 S1 lo(p4_item1)

; END DETERMINE PLAYER CODE

; BEGIN ANALYZE INVENTORY CODE
; This code block checks if the current player can fit the item.
; If not, we proceed to the next block. Otherwise, we jump ahead
; to "giveitem".

determineslot:
ADDIU S2 R0 0xFF
LBU S0 0(S1)
BEQ S0 S2 giveitem ; First slot empty (0xFF)
NOP
ADDIU S1 S1 1
LBU S0 0(S1)
BEQ S0 S2 giveitem
NOP
ADDIU S1 S1 1
LBU S0 0(S1)
BEQ S0 S2 giveitem
NOP

; END ANALYZE INVENTORY CODE

; BEGIN SHOW "CANNOT GIVE ITEM" MESSAGE CODE
; This code block displays a message specified below under
; "cannotGiveItemMsg", if the current player has a full invetory.
; You can remove this block if you want, but remember
; to remove the cannotGiveItemMsg block as well, including "align 16".

SW R0 16(SP) ; A4
SW R0 20(SP) ; A5
SW R0 24(SP) ; A6
ADDI A0 R0 -1 ; Character image (-1 for none)
LUI A1 hi(cannotGiveItemMsg)
ADDIU A1 A1 lo(cannotGiveItemMsg)
ADDU A2 R0 R0
JAL 0x800EC8EC ; ShowMessage
ADDU A3 R0 R0

JAL 0x800EC9DC
NOP
JAL 0x800EC6C8
NOP
JAL 0x800EC6EC
NOP

; END SHOW "CANNOT GIVE ITEM" MESSAGE CODE

J exit
NOP

giveitem:

;BEGIN DETERMINE ITEM CODE

; This code block generates a random 8-bit integer (0-255) and
; iteratively checks how big it is. Depending on its size, one of
; five different items can be received. By changing the size and 
; number of the intervals, any number of items can be included in the 
; list with separate and specific odds of getting them.

JAL 0x8000B16C ; Generates an integer.
NOP

ADDIU A0 R0 0x34 ; Is the number less than 52? (20% chance)
SLT S0 V0 A0
NOP

BNE S0 R0 getitemone ; If so, get item one.
NOP

; If we got here, it is not less than 52.

ADDIU A0 R0 0x67 ; Is it less than 103? (40% chance)
SLT S0 V0 A0
NOP

BNE S0 R0 getitemtwo ; If so, get item two.
NOP

; If we got here, it is not less than 103.

ADDIU A0 R0 0x9A ; Is it less than 154? (60% chance)
SLT S0 V0 A0
NOP

BNE S0 R0 getitemthree ; If so, get item three.
NOP

; If we got here, it is not less than 154.

ADDIU A0 R0 0xCD ; Is it less than 205? (80% chance)
SLT S0 V0 A0
NOP

BNE S0 R0 getitemfour ; If so, get item four.
NOP

; If we got here, it is not less than 205. Since we have a 20% chance
; of reaching this point, let's just get item five.

J getitemfive
NOP

; END DETERMINE ITEM CODE

; BEGIN GET ITEM CODE
; This code block gives the player one of the items chosen in
; the previous block. The item values can be changed to any value
; in this table:
; 0x00 = Mushroom
; 0x01 = Skeleton Key
; 0x02 = Poison Mushroom
; 0x03 = Reverse Mushroom
; 0x04 = Cellular Shopper
; 0x05 = Warp Block
; 0x06 = Plunder Chest
; 0x07 = Bowser Phone
; 0x08 = Dueling Glove
; 0x09 = Lucky Lamp
; 0x0A = Golden Mushroom
; 0x0B = Boo Bell
; 0x0C = Boo Repellant
; 0x0D = Bowser Suit
; 0x0E = Magic Lamp
; 0x0F = Koopa Card
; 0x10 = Barter Box
; 0x11 = Lucky Coin
; 0x12 = Wacky Watch

getitemone:
ADDIU S2 R0 0x00 ; Mushroom value
SB S2 0(S1)
J bye
getitemtwo:
ADDIU S2 R0 0x01 ; Skeleton Key value
SB S2 0(S1)
J bye
getitemthree:
ADDIU S2 R0 0x06; Plunder Chest value
SB S2 0(S1)
J bye
getitemfour:
ADDIU S2 R0 0x08 ; Dueling Glove value
SB S2 0(S1)
J bye
getitemfive:
ADDIU S2 R0 0x0B ; Boo Bell value
SB S2 0(S1)

; END GET ITEM CODE

bye:

exit:
LW RA 40(SP)
LW S2 36(SP)
LW S1 32(SP)
LW S0 28(SP)
JR RA
ADDIU SP SP 48

.align 16
cannotGiveItemMsg:
.ascii "You have too many items"
.byte 0x85 ; period
.byte 0xFF,0 ; FF=Pause