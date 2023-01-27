target extended-remote :3333
monitor device Cortex-M4
monitor speed auto
file .pio/build/smaq3/firmware.elf
load
monitor reset