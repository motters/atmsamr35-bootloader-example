#pragma once

// Boot section
extern uint32_t __bootrom_start__;
extern uint32_t __bootrom_size__;

// Ap section
extern uint32_t __approm_start__;
extern uint32_t __approm_size__;

// Share memory address
extern int __sharedram_start__;
extern int __sharedram_size__;