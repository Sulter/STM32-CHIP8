#include <stdint.h>

uint8_t memory[4096] = {0};
uint8_t V[16] = {0};
uint16_t PC = 0;
uint16_t I = 0;
uint16_t stack[16] = {0};
uint16_t stackP = 0;


void chip8_init(uint8_t *game, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++) {
	memory[i + 0x200] = game[i];
    }
}

uint16_t chip8_get_opcode(uint16_t addr)
{
    return memory[addr+1]|(memory[addr] << 8);
}

void chip8_step(void)
{
    uint16_t opcode = chip8_get_opcode(PC++);
}


/******************OPCODES************************/
void chip8_opcode_none(uint16_t *opcode)
{
    (void)opcode;
}

//0NNN 	Call 		Calls RCA 1802 program at address NNN. Not necessary for most ROMs.

//00E0 	Display 	disp_clear() 	Clears the screen.

//00EE 	Flow 	return; 	Returns from a subroutine.
void chip8_opcode_00EE(uint16_t *opcode)
{
    (void)opcode;
    stackP--;
    PC = stack[stackP];
}

//1NNN 	Flow 	goto NNN; 	Jumps to address NNN.
void chip8_opcode_1NNN(uint16_t *opcode)
{
    PC = (*opcode & 0x0fff);
}

//2NNN 	Flow 	*(0xNNN)() 	Calls subroutine at NNN.
void chip8_opcode_2NNN(uint16_t *opcode)
{
    stack[stackP] = PC;
    stackP++;
    PC = (*opcode & 0x0fff);
}

//3XNN 	Cond 	if(Vx==NN) 	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
void chip8_opcode_3XNN(uint16_t *opcode)
{
    uint8_t NN = (*opcode & 0x00ff);
    uint8_t X = (*opcode & 0x0f00) >> 2;
    if(NN == V[X]) {
	PC++;
    }
}

//4XNN 	Cond 	if(Vx!=NN) 	Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
	


//5XY0 	Cond 	if(Vx==Vy) 	Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)

//6XNN 	Const 	Vx = NN 	Sets VX to NN.

//7XNN 	Const 	Vx += NN 	Adds NN to VX.

//8XY0 	Assign 	Vx=Vy 	Sets VX to the value of VY.

//8XY1 	BitOp 	Vx=Vx|Vy 	Sets VX to VX or VY. (Bitwise OR operation)

//8XY2 	BitOp 	Vx=Vx&Vy 	Sets VX to VX and VY. (Bitwise AND operation)

//8XY3 	BitOp 	Vx=Vx^Vy 	Sets VX to VX xor VY.

//8XY4 	Math 	Vx += Vy 	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.

//8XY5 	Math 	Vx -= Vy 	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.

//8XY6 	BitOp 	Vx >> 1 	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.[2]

//8XY7 	Math 	Vx=Vy-Vx 	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.

//8XYE 	BitOp 	Vx << 1 	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]

//9XY0 	Cond 	if(Vx!=Vy) 	Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)

//ANNN 	MEM 	I = NNN 	Sets I to the address NNN.

//BNNN 	Flow 	PC=V0+NNN 	Jumps to the address NNN plus V0.

//CXNN 	Rand 	Vx=rand()&NN 	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.

//DXYN 	Disp 	draw(Vx,Vy,N) 	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen

//EX9E 	KeyOp 	if(key()==Vx) 	Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)

//EXA1 	KeyOp 	if(key()!=Vx) 	Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)

//FX07 	Timer 	Vx = get_delay() 	Sets VX to the value of the delay timer.

//FX0A 	KeyOp 	Vx = get_key() 	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)

//FX15 	Timer 	delay_timer(Vx) 	Sets the delay timer to VX.

//FX18 	Sound 	sound_timer(Vx) 	Sets the sound timer to VX.

//FX1E 	MEM 	I +=Vx 	Adds VX to I.[3]

//FX29 	MEM 	I=sprite_addr[Vx] 	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.

//FX33 	BCD 	set_BCD(Vx);

//FX55 	MEM 	reg_dump(Vx,&I) 	Stores V0 to VX (including VX) in memory starting at address I.[4]

//FX65 	MEM 	reg_load(Vx,&I) 	Fills V0 to VX (including VX) with values from memory starting at address I.[4]
