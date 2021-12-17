/*
 * ECE 18-447, Spring 2013
 *
 * MIPS pipeline timing simulator
 *
 * Chris Fallin, 2012
 */

#ifndef _PIPE_H_
#define _PIPE_H_

#include "shell.h"

/* Pipeline ops (instances of this structure) are high-level representations of
 * the instructions that actually flow through the pipeline. This struct does
 * not correspond 1-to-1 with the control signals that would actually pass
 * through the pieline. Rather, it carries the orginal instruction, operand
 * information and values as they are collected, and destination information. */
typedef struct Pipe_Op {
    /* PC of this instruction */
    uint32_t pc;
    /* raw instruction */
    uint32_t instruction;
    /* decoded opcode and subopcode fields */
    int opcode, subop;

    /* immediate value, if any, for ALU immediates */
    uint32_t imm16, se_imm16;
    /* shift amount */
    int shamt;

    /* register source values */
    int reg_src1, reg_src2; /* 0 -- 31 if this inst has register source(s), or
                               -1 otherwise */
    uint32_t reg_src1_value, reg_src2_value; /* values of operands from source
                                                regs */

    /* memory access information */
    int is_mem;       /* is this a load/store? */
    uint32_t mem_addr; /* address if applicable */
    int mem_write; /* is this a write to memory? */
    uint32_t mem_value; /* value loaded from memory or to be written to memory */

    /* register destination information */
    int reg_dst; /* 0 -- 31 if this inst has a destination register, -1
                    otherwise */
    uint32_t reg_dst_value; /* value to write into dest reg. */
    int reg_dst_value_ready; /* destination value produced yet? */

    /* branch information */
    int is_branch;        /* is this a branch? */
    uint32_t branch_dest; /* branch destination (if taken) */
    int branch_cond;      /* is this a conditional branch? */
    int branch_taken;     /* branch taken? (set as soon as resolved: in decode
                             for unconditional, execute for conditional) */
    int is_link;          /* jump-and-link or branch-and-link inst? */
    int link_reg;         /* register to place link into? */
    
    uint32_t predictDirection;
    uint32_t predictDest;

} Pipe_Op;

/* The pipe state represents the current state of the pipeline. It holds a
 * pointer to the op that is currently at the input of each stage. As stages
 * execute, they remove the op from their input (set the pointer to NULL) and
 * place an op at their output. If the pointer that represents a stage's output
 * is not null when that stage executes, then this represents a pipeline stall,
 * and the stage must not overwrite its output (otherwise an instruction would
 * be lost).
 */

typedef struct Pipe_State {
    /* pipe op currently at the input of the given stage (NULL for none) */
    Pipe_Op *decode_op, *execute_op, *mem_op, *wb_op;

    /* register file state */
    uint32_t REGS[32];
    uint32_t HI, LO;

    /* program counter in fetch stage */
    uint32_t PC;

    /* information for PC update (branch recovery). Branches should use this
     * mechanism to redirect the fetch stage, and flush the ops that came after
     * the branch as necessary. */
    int branch_recover; /* set to '1' to load a new PC */
    uint32_t branch_dest; /* next fetch will be from this PC */
    int branch_flush; /* how many stages to flush during recover? (1 = fetch, 2 = fetch/decode, ...) */

    /* multiplier stall info */
    int multiplier_stall; /* number of remaining cycles until HI/LO are ready */

    /* place other information here as necessary */
    
    /* instruction cache stall info */
    int inst_stall;
    int inst_stall_valid;
    /* data cache stall info */
    int dataCache_stall;
    
    uint32_t predictDest;
    
   

} Pipe_State;

/* global variable -- pipeline state */
extern Pipe_State pipe;

/* called during simulator startup */
void pipe_init();

/* this function calls the others */
void pipe_cycle();

/* helper: pipe stages can call this to schedule a branch recovery */
/* flushes 'flush' stages (1 = execute only, 2 = fetch/decode, ...) and then
 * sets the fetch PC to the given destination. */
void pipe_recover(int flush, uint32_t dest);

/* each of these functions implements one stage of the pipeline */
void pipe_stage_fetch();
void pipe_stage_decode();
void pipe_stage_execute();
void pipe_stage_mem();
void pipe_stage_wb();

typedef struct cacheBlock{
    uint32_t tag,data,recency;
} cacheBlock;

typedef struct DataCacheBlock{
    uint32_t valid,tag,data,recency,dirty;
}DataCacheBlock;

typedef struct LRU{
    int block, valid;
} LRU;

typedef struct BTBinfo
{
    uint32_t addressTag,target;
    int valid, is_unconditional;
}BTBinfo;

/* For Instruction and data cache*/
uint32_t InstructionCache(uint32_t addr);
uint32_t DataCache(uint32_t addr);
void LRUupdate(int idx, int set, int is_inst);
void LRUpolicy(int tag, int set, int is_inst);
int findMaxidx(int *arr, int n);
/* For Branch prediction*/
uint32_t predictor(uint32_t PC);
void update_prediction(uint32_t PC, uint32_t target,int is_taken , int is_cond);

#endif
