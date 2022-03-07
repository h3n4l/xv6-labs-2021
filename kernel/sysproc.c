/*
 * @Author: h3n4l
 * @Date: 2022-03-06 14:25:36
 * @LastEditors: h3n4l
 * @LastEditTime: 2022-03-06 22:40:49
 * @FilePath: /xv6-labs-2021/kernel/sysproc.c
 */
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;
  back_trace();
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_sigalarm(void)
{
  // Get two arguments of this syscall, there is prototype below:
  // int sigalarm(int ticks, void(*handler)());
  int xtick;
  uint64 callback_p;
  if(argint(0, &xtick) < 0){
    return -1;
  }
  if(xtick < 0){
    // What is negative value of xtick????
    return -1;
  }
  if(argaddr(1, &callback_p) < 0){
    return -1;
  }
  return sigalarm(xtick, callback_p);
}


uint64
sys_sigreturn(void)
{
  return sigreturn();
}
