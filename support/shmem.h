/*
--------------------------------------------------------------------------------
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
Boston, MA  02110-1301, USA.
--------------------------------------------------------------------------------
*/

// Copyright (c) 2019 John Seamons, ZL/KF6VO

#pragma once

#include "types.h"
#include "kiwi.h"
#include "str.h"
#include "printf.h"
#include "spi.h"
#include "spi_dev.h"
#include "rx_waterfall.h"

#include <signal.h>


// this is here instead of printf.h because of circular #include problems

#define N_LOG_MSG_LEN   256
#define N_LOG_SAVE      256

typedef struct {
    bool init;
	int idx, not_shown;
	char *arr[N_LOG_SAVE];
	char *endp;
	char mem[1];	// mem allocated starting here; must be last in struct
} log_save_t;

extern log_save_t *log_save_p;


#define N_SHMEM_STATUS 4
#define N_SHMEM_STATUS_STR  1024
#define N_SHMEM_SDR_HU_STATUS_STR  256

#ifndef linux
 #define SIGRTMIN 0
 #define SIGRTMAX 0
#endif

#define SIG_DEBUG       SIGUSR1
#define SIG_SETUP_TRAMP SIGUSR2

#define SIG_IPC_MIN     (SIGRTMIN + 0)
#define SIG_IPC_SPI     (SIGRTMIN + 0)
#define SIG_IPC_WF      (SIGRTMIN + 1)
#define SIG_BACKTRACE   (SIGRTMIN + 2)
#define SIG_MAX_USED    (2+1)

#define SIG2IPC(sig)    ((sig) - SIG_IPC_MIN)

typedef struct {
    funcPI_t func;
    int child_sig;
    int parent_pid, child_pid;
    int which_hiwat;
    #define N_SHMEM_WHICH 32
    u4_t request[N_SHMEM_WHICH], done[N_SHMEM_WHICH];
} shmem_ipc_t;

typedef struct {
    bool kiwi_exit;
    u4_t rv_u4_t[MAX_RX_CHANS];
    u4_t status_u4[N_SHMEM_STATUS][MAX_RX_CHANS];
    double status_f[N_SHMEM_STATUS][MAX_RX_CHANS];
	char status_str[N_SHMEM_STATUS_STR];
	char sdr_hu_status_str[N_SHMEM_SDR_HU_STATUS_STR];
	
    shmem_ipc_t ipc[SIG_MAX_USED];

    #ifdef SPI_SHMEM_DISABLE
    #else
        // shared with SPI async i/o helper process
        spi_shmem_t spi_shmem;
    #endif

    #ifdef WF_SHMEM_DISABLE
    #else
        // shared with waterfall offload process
        wf_shmem_t wf_shmem;
    #endif

    log_save_t log_save;    // must be last because of var length
} non_blocking_shmem_t;

extern non_blocking_shmem_t *shmem;

void shmem_init();
void sig_arm(int sig, funcPI_t handler, int flags=0);
void shmem_ipc_invoke(int child_sig, int which=0);
void shmem_ipc_setup(const char *id, int child_sig, funcPI_t func);
