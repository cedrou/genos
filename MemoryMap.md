## Physical memory ##

|  **start**   |    **end**   |   **size**   |                                        |
|:-------------|:-------------|:-------------|:---------------------------------------|
| 0x00000000 | 0x00100000 | 0x00100000 | BIOS                                   |
| 0x00100000 | 0x00101000 | 0x00001000 | Loader (kldr.exe)                      |
| 0x00102000 | 0x00109000 | 0x00007000 | Kernel (kernel.exe)                    |
| 0x00109000 | 0x0012C000 | 0x00023000 | PDB (kernel.pdb)                       |
| 0x0012C000 | 0x0013C000 | 0X00010000 | Kernel stack                           |
| 0x0013C000 | 0x0013D000 | 0x00001000 | Frame manager bitset (1 frame / 128 MiB of available memory)  |
| 0x0013D000 | 0x0013E000 | 0x00001000 | Kernel page directory                  |
| 0x0013E000 | 0x0013F000 | 0x00001000 | Kernel page table 0 (identity)         |
| 0x0013F000 | 0x00140000 | 0x00001000 | Kernel page table 756 (3GiB kernel)    |
|    ....    |    ....    |    ....    | Available                              |

## Kernel virtual memory ##

|  **start**   |    **end**   |   **size**   |                                        |
|:-------------|:-------------|:-------------|:---------------------------------------|
| 0x00000000 | 0x00100000 | 0x00100000 | BIOS                                   |
| 0x00100000 | 0x00101000 | 0x00001000 | Loader (kldr.exe)                      |
|    ....    |    ....    |    ....    | Available                              |
| 0xC0000000 | 0xC0008000 | 0x00008000 | Kernel (kernel.exe)                    |
| 0xC0008000 | 0xC0031000 | 0x00029000 | PDB (kernel.pdb)                       |
| 0xC0031000 | 0xC0032000 | 0X00001000 | Kernel stack page guard                |
| 0xC0032000 | 0xC0042000 | 0X00010000 | Kernel stack                           |
| 0xC0042000 | 0xC0043000 | 0x00001000 | Frame manager bitset                   |
|    ....    |    ....    |    ....    | Available                              |
| 0xFFC00000 | 0xFFFFFFFF | 0x00400000 | Kernel page directory                  |

## Process virtual memory ##

TODO