//#include <common.h>
#include <config.h>
#include <spl.h>
#include <mmc.h>

/**
 * spl_mmc_boot_partition() - Upstream U-Boot Core API Override.
 * @mmc: Pointer to the initialized MMC storage hardware context.
 *
 * This function handles the atomic fallback. Because the SPL retry loop 
 * executes entirely within RAM without resetting the CPU, our static counter 
 * dynamically routes the secondary storage read pass to Partition 3.
 */
int spl_mmc_boot_partition(const u32 boot_device) {
    /* Allocated permanently in SRAM; initialized to 0 ONLY on initial power-on */
    static int boot_attempt_pass = 0;
    
    /* Increment the execution pass index immediately on function entry */
    boot_attempt_pass++;

    /* Pass 1: Cold Boot / First Initialization */
    if (boot_attempt_pass == 1) {
        /* Direct the SPL file system driver to scan Partition 2 (Slot A) */
        return 2; 
    }

    /* Pass 2: The Fallback Execution Loop */
    /* This branch ONLY executes if Pass 1 threw a verification error and looped back */
    if (boot_attempt_pass == 2) {
        printf("\n===========================================================\n");
        printf("[CRITICAL FIRMWARE FAULT]: Slot A verification failed.\n");
        printf("[FAILOVER]: Routing storage block engine to Partition 3...\n");
        printf("===========================================================\n\n");
        
        /* Force the SPL file system driver to scan Partition 3 (Slot B) */
        return 3; 
    }

    /* Ultimate safety catch-all trap if everything fails */
    return 2;
}
