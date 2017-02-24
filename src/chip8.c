#include "chip8.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**
 * TODO: 
 * CXNN - implement rand. maybe init with pointers to functions???
 * SCREEN - ^
 */

static uint8_t chip8_screen_buffer[CHIP8_WIDTH][CHIP8_HEIGHT];

static uint8_t memory[4096] = {
    /*
      0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
    */
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    /*
      0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
      0x200-0xFFF - Program ROM and work RAM
    */
};

//CPU
static uint8_t V[16] = {0};
static uint16_t PC = 200;
static uint16_t I = 0;
static uint16_t stack[16] = {0};
static uint16_t stackP = 0;

//peripherals
volatile uint8_t delay_timer;
volatile uint8_t sound_timer;

//declare
void chip8_timer_ISR(void);
void chip8_draw_screen(uint8_t x, uint8_t y, uint8_t h, uint8_t *data);
void chip8_wait_key(void);
uint16_t chip8_get_opcode(uint16_t addr);

//opcode function declares
void chip8_opcode_none(uint16_t *opcode);

void chip8_opcode_0ALL(uint16_t *opcode);
void chip8_opcode_0NNN(uint16_t *opcode);
void chip8_opcode_00E0(uint16_t *opcode);
void chip8_opcode_00EE(uint16_t *opcode);

void chip8_opcode_1NNN(uint16_t *opcode);
void chip8_opcode_2NNN(uint16_t *opcode);
void chip8_opcode_3XNN(uint16_t *opcode);
void chip8_opcode_4XNN(uint16_t *opcode);
void chip8_opcode_5XY0(uint16_t *opcode);
void chip8_opcode_6XNN(uint16_t *opcode);
void chip8_opcode_7XNN(uint16_t *opcode);

void chip8_opcode_8ALL(uint16_t *opcode);
void chip8_opcode_8XY0(uint16_t *opcode);
void chip8_opcode_8XY1(uint16_t *opcode);
void chip8_opcode_8XY2(uint16_t *opcode);
void chip8_opcode_8XY3(uint16_t *opcode);
void chip8_opcode_8XY4(uint16_t *opcode);
void chip8_opcode_8XY5(uint16_t *opcode);
void chip8_opcode_8XY6(uint16_t *opcode);
void chip8_opcode_8XY7(uint16_t *opcode);
void chip8_opcode_8XYE(uint16_t *opcode);

void chip8_opcode_9XY0(uint16_t *opcode);
void chip8_opcode_ANNN(uint16_t *opcode);
void chip8_opcode_BNNN(uint16_t *opcode);
void chip8_opcode_CXNN(uint16_t *opcode);
void chip8_opcode_DXYN(uint16_t *opcode);

void chip8_opcode_EALL(uint16_t *opcode);
void chip8_opcode_EX9E(uint16_t *opcode);
void chip8_opcode_EXA1(uint16_t *opcode);

void chip8_opcode_FALL(uint16_t *opcode);
void chip8_opcode_FX07(uint16_t *opcode);
void chip8_opcode_FX0A(uint16_t *opcode);
void chip8_opcode_FX15(uint16_t *opcode);
void chip8_opcode_FX18(uint16_t *opcode);
void chip8_opcode_FX1E(uint16_t *opcode);
void chip8_opcode_FX29(uint16_t *opcode);
void chip8_opcode_FX33(uint16_t *opcode);
void chip8_opcode_FX55(uint16_t *opcode);
void chip8_opcode_FX65(uint16_t *opcode);


void (*opcode_fp_FXXX[])(uint16_t *) = {
    chip8_opcode_0ALL,
    chip8_opcode_1NNN,
    chip8_opcode_2NNN,
    chip8_opcode_3XNN,
    chip8_opcode_4XNN,
    chip8_opcode_5XY0,
    chip8_opcode_6XNN,
    chip8_opcode_7XNN,
    chip8_opcode_8ALL,
    chip8_opcode_9XY0,
    chip8_opcode_ANNN,
    chip8_opcode_BNNN,
    chip8_opcode_CXNN,
    chip8_opcode_DXYN,
    chip8_opcode_EALL,
    chip8_opcode_FALL
};

void (*opcode_fp_8ALL[])(uint16_t *) = {
    chip8_opcode_8XY0,
    chip8_opcode_8XY1,
    chip8_opcode_8XY2,
    chip8_opcode_8XY3,
    chip8_opcode_8XY4,
    chip8_opcode_8XY5,
    chip8_opcode_8XY6,
    chip8_opcode_8XY7,
    chip8_opcode_none,
    chip8_opcode_none,
    chip8_opcode_none,
    chip8_opcode_none,
    chip8_opcode_none,
    chip8_opcode_none,
    chip8_opcode_8XYE,
    chip8_opcode_none
};

/***************FUNCTIONS********************/
/**
 * Must be called @ 60Hz
 */
void chip8_timer_ISR(void)
{
    delay_timer--;
    sound_timer--;
}

void chip8_init(uint8_t *rom, uint16_t size)
{
    PC = 0x200;
    I = 0;
    for (uint16_t i = 0; i < size; i++) {
	memory[i + 0x200] = rom[i];
    }
}

uint16_t chip8_get_opcode(uint16_t addr)
{
    return memory[addr+1]|(memory[addr] << 8);
}

void chip8_step(void)
{
    uint16_t opcode = chip8_get_opcode(PC);
    PC += 2;
    opcode_fp_FXXX[(opcode & 0xf000) >> 12](&opcode);
    if(PC >= 0x200 + 10)
	PC = 0x200;
}

uint8_t** chip8_get_screen_buffer(void)
{
    return chip8_screen_buffer;
}

/*****************OPCODE CATCHERS*****************/
void chip8_opcode_0ALL(uint16_t *opcode)
{
    switch(*opcode) {
	case 0x00e0:
	    chip8_opcode_00E0(opcode);
	    break;
	case 0x00ee:
	    chip8_opcode_00EE(opcode);
	    break;
	default:
	    chip8_opcode_0NNN(opcode);
	    break;
    }
}

void chip8_opcode_8ALL(uint16_t *opcode)
{
    opcode_fp_8ALL[*opcode & 0x000F](opcode);
}

void chip8_opcode_EALL(uint16_t *opcode)
{
    switch(*opcode & 0x00ff) {
	case 0x009e:
	    chip8_opcode_EX9E(opcode);
	    break;
	case 0x00a1:
	    chip8_opcode_EXA1(opcode);
	    break;
	default:
	    chip8_opcode_none(opcode);
	    break;
    }
}

void chip8_opcode_FALL(uint16_t *opcode)
{
    switch(*opcode & 0x00ff) {
	case 0x0007:
	    chip8_opcode_FX07(opcode);
	    break;
	case 0x000A:
	    chip8_opcode_FX0A(opcode);
	    break;
	case 0x0015:
	    chip8_opcode_FX15(opcode);
	    break;
	case 0x0018:
	    chip8_opcode_FX18(opcode);
	    break;
	case 0x001e:
	    chip8_opcode_FX1E(opcode);
	    break;
	case 0x0029:
	    chip8_opcode_FX29(opcode);
	    break;
	case 0x0033:
	    chip8_opcode_FX33(opcode);
	    break;
	case 0x0055:
	    chip8_opcode_FX55(opcode);
	    break;
	case 0x0065:
	    chip8_opcode_FX65(opcode);
	    break;
	default:
	    chip8_opcode_none(opcode);
	    break;
    }
}
/******************OPCODES************************/
void chip8_opcode_none(uint16_t *opcode)
{
    (void)opcode;
}

//0NNN 	Call 		Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
void chip8_opcode_0NNN(uint16_t *opcode)
{
    (void)opcode;
}

//00E0 	Display 	disp_clear() 	Clears the screen.
void chip8_opcode_00E0(uint16_t *opcode)
{
    (void)opcode;
    memset(chip8_screen_buffer, 0, sizeof(chip8_screen_buffer[0][0]) * CHIP8_WIDTH * CHIP8_HEIGHT);
}

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
    uint8_t X = (*opcode & 0x0f00) >> 8;
    if(NN == V[X]) {
	PC += 2;
    }
}

//4XNN 	Cond 	if(Vx!=NN) 	Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
void chip8_opcode_4XNN(uint16_t *opcode)
{
    uint8_t NN = (*opcode & 0x00ff);
    uint8_t X = (*opcode & 0x0f00) >> 8;
    if(NN != V[X]) {
	PC += 2;
    }
}

//5XY0 	Cond 	if(Vx==Vy) 	Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
void chip8_opcode_5XY0(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    uint8_t Y = (*opcode & 0x00f0) >> 4;
    if(V[X] == V[Y]) {
	PC += 2;
    }
}

//6XNN 	Const 	Vx = NN 	Sets VX to NN.
void chip8_opcode_6XNN(uint16_t *opcode)
{
    uint8_t NN = (*opcode & 0x00ff);
    uint8_t X = (*opcode & 0x0f00) >> 8;
    V[X] = NN;
}

//7XNN 	Const 	Vx += NN 	Adds NN to VX.
void chip8_opcode_7XNN(uint16_t *opcode)
{
    uint8_t NN = (*opcode & 0x00ff);
    uint8_t X = (*opcode & 0x0f00) >> 8;
    V[X] += NN;
}

//8XY0 	Assign 	Vx=Vy 	Sets VX to the value of VY.
void chip8_opcode_8XY0(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    uint8_t Y = (*opcode & 0x00f0) >> 4;
    V[X] = V[Y];
}

//8XY1 	BitOp 	Vx=Vx|Vy 	Sets VX to VX or VY. (Bitwise OR operation)
void chip8_opcode_8XY1(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    uint8_t Y = (*opcode & 0x00f0) >> 4;
    V[X] |= V[Y];
}

//8XY2 	BitOp 	Vx=Vx&Vy 	Sets VX to VX and VY. (Bitwise AND operation)
void chip8_opcode_8XY2(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    uint8_t Y = (*opcode & 0x00f0) >> 4;
    V[X] &= V[Y];
}

//8XY3 	BitOp 	Vx=Vx^Vy 	Sets VX to VX xor VY.
void chip8_opcode_8XY3(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    uint8_t Y = (*opcode & 0x00f0) >> 4;
    V[X] ^= V[Y];
}

//8XY4 	Math 	Vx += Vy 	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
void chip8_opcode_8XY4(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    uint8_t Y = (*opcode & 0x00f0) >> 4;
    V[0xF] = (V[X] + V[Y]) >> 8;
    V[X] += V[Y];
}

//8XY5 	Math 	Vx -= Vy 	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
void chip8_opcode_8XY5(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    uint8_t Y = (*opcode & 0x00f0) >> 4;
    V[0xF] = V[X] < V[Y];
    V[X] -= V[Y];
}

//8XY6 	BitOp 	Vx >> 1 	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.[2]
void chip8_opcode_8XY6(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    V[0xF] = V[X] & 1;
    V[X] = V[X] >> 1;
}

//8XY7 	Math 	Vx=Vy-Vx 	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
void chip8_opcode_8XY7(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    uint8_t Y = (*opcode & 0x00f0) >> 4;
    V[0xF] = V[X] > V[Y];
    V[X] = V[Y] - V[X];
}

//8XYE 	BitOp 	Vx << 1 	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]
void chip8_opcode_8XYE(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    V[0xF] = V[X] >> 7;
    V[X] = V[X] << 1;
}

//9XY0 	Cond 	if(Vx!=Vy) 	Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
void chip8_opcode_9XY0(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    uint8_t Y = (*opcode & 0x00f0) >> 4;
    if(V[X] != V[Y]) {
	PC += 2;
    }
}
//ANNN 	MEM 	I = NNN 	Sets I to the address NNN.
void chip8_opcode_ANNN(uint16_t *opcode)
{
    uint8_t NNN = (*opcode & 0x0fff);
    I = NNN;
}

//BNNN 	Flow 	PC=V0+NNN 	Jumps to the address NNN plus V0.
void chip8_opcode_BNNN(uint16_t *opcode)
{
    uint8_t NNN = (*opcode & 0x0fff);
    PC = V[0] + NNN;
}

//CXNN 	Rand 	Vx=rand()&NN 	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
void chip8_opcode_CXNN(uint16_t *opcode)
{
    uint8_t NN = (*opcode & 0x00ff);
    uint8_t X = (*opcode & 0x0f00) >> 8;
    uint8_t rand = 151;
    V[X] = rand & NN;
}

//DXYN 	Disp 	draw(Vx,Vy,N) 	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
void chip8_opcode_DXYN(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    uint8_t Y = (*opcode & 0x00f0) >> 4;
    uint8_t height = *opcode & 0x000f;
    uint16_t currentI = I;
    V[0xf] = 0;

    for(uint16_t currentY = V[Y]; currentY < V[Y] + height; currentY++) {
	uint8_t drawB = memory[currentI++];
	for(uint8_t bit = 0; bit < 8; bit++) {
	    uint8_t pixel = (drawB >> (7 - bit)) & 1;
	    uint8_t currentX = V[X] + bit;

	    //if a pixel goes from 1->0, set the VF to 1.
	    if(chip8_screen_buffer[currentX][currentY] == 1 && pixel == 1) {
		V[0xf] = 1;
	    }
	    chip8_screen_buffer[currentX][currentY] ^= pixel;
	}
    }
}

//EX9E 	KeyOp 	if(key()==Vx) 	Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
void chip8_opcode_EX9E(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
}

//EXA1 	KeyOp 	if(key()!=Vx) 	Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
void chip8_opcode_EXA1(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
}

//FX07 	Timer 	Vx = get_delay() 	Sets VX to the value of the delay timer.
void chip8_opcode_FX07(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    V[X] = delay_timer;
}

//FX0A 	KeyOp 	Vx = get_key() 	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
void chip8_opcode_FX0A(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    for(;;);
}

//FX15 	Timer 	delay_timer(Vx) 	Sets the delay timer to VX.
void chip8_opcode_FX15(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    delay_timer = V[X];
}

//FX18 	Sound 	sound_timer(Vx) 	Sets the sound timer to VX.
void chip8_opcode_FX18(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    sound_timer = V[X];
}

//FX1E 	MEM 	I +=Vx 	Adds VX to I.[3]
void chip8_opcode_FX1E(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    I += V[X];
}

//FX29 	MEM 	I=sprite_addr[Vx] 	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
void chip8_opcode_FX29(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    I = memory[V[X] * 5];
}

//FX33 	BCD 	set_BCD(Vx);
void chip8_opcode_FX33(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
}

//FX55 	MEM 	reg_dump(Vx,&I) 	Stores V0 to VX (including VX) in memory starting at address I.[4]
void chip8_opcode_FX55(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    for(uint8_t i = 0; i <= X; i++) {
	memory[I+i] = V[i];
    }
}

//FX65 	MEM 	reg_load(Vx,&I) 	Fills V0 to VX (including VX) with values from memory starting at address I.[4]
void chip8_opcode_FX65(uint16_t *opcode)
{
    uint8_t X = (*opcode & 0x0f00) >> 8;
    for(uint8_t i = 0; i <= X; i++) {
	V[i] = memory[I+i];
    }
}
