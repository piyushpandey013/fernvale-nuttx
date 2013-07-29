/*****************************************************************************
 * configs/sama5d3x-ek/src/nor_main.c
 *
 *   Copyright (C) 2013 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdio.h>
#include <debug.h>

#include "up_arch.h"
#include "sam_periphclks.h"
#include "chip/sam_hsmc.h"

#include "sama5d3x-ek.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define NOR_ENTRY ((nor_entry_t)SAM_EBICS0_VSECTION)

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef void (*nor_entry_t)(void);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: nor_main
 *
 * Description:
 *   nor_main is a tiny program that runs in ISRAM.  nor_main will enable
 *   NOR flash then jump to the program in NOR flash
 *
 ****************************************************************************/

int nor_main(int argc, char *argv)
{
  uint32_t regval;

  /* Make sure that the SMC peripheral is enabled (But of course it is... we
   * are executing from NOR FLASH now).
   */

  printf("Configuring NOR flash on CS0\n");
  sam_hsmc_enableclk();

  /* The SAMA5D3x-EK has 118MB of 16-bit NOR FLASH at CS0.  The NOR FLASH
   * has already been configured by the first level ROM bootloader... we
   * simply need to modify the timing here.
   */

  regval = HSMC_SETUP_NWE_SETUP(1) |  HSMC_SETUP_NCS_WRSETUP(0) |
           HSMC_SETUP_NRD_SETUP(2) | HSMC_SETUP_NCS_RDSETUP(0);
  putreg32(regval, SAM_HSMC_SETUP(HSMC_CS0));

  regval = HSMC_PULSE_NWE_PULSE(10) | HSMC_PULSE_NCS_WRPULSE(10) |
           HSMC_PULSE_NRD_PULSE(11) | HSMC_PULSE_NCS_RDPULSE(11);
  putreg32(regval, SAM_HSMC_PULSE(HSMC_CS0));

  regval = HSMC_CYCLE_NWE_CYCLE(11) | HSMC_CYCLE_NRD_CYCLE(14);
  putreg32(regval, SAM_HSMC_CYCLE(HSMC_CS0));

  regval = HSMC_TIMINGS_TCLR(0) | HSMC_TIMINGS_TADL(0) |
           HSMC_TIMINGS_TAR(0) | HSMC_TIMINGS_TRR(0) |
           HSMC_TIMINGS_TWB(0) | HSMC_TIMINGS_RBNSEL(0);
  putreg32(regval, SAM_HSMC_TIMINGS(HSMC_CS0));

  regval = HSMC_MODE_READMODE | HSMC_MODE_WRITEMODE |
           HSMC_MODE_EXNWMODE_DISABLED | HSMC_MODE_BIT_16 |
           HSMC_MODE_TDFCYCLES(1);
  putreg32(regval, SAM_HSMC_MODE(HSMC_CS0));

  /* Then jump into NOR flash */

#if 1
  printf("Waiting for GDB halt\n");
  fflush(stdout);
  for (;;);
#else
  printf("Jumping to NOR flash on CS0\n");
  fflush(stdout);
  usleep(500*1000);

  NOR_ENTRY();
#endif
  return 0; /* NOR_ENTRY() should not return */
}