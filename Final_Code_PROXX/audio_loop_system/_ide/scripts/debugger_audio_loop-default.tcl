# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: C:\Users\eza10\Documents\flag\audio_loop_system\_ide\scripts\debugger_audio_loop-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source C:\Users\eza10\Documents\flag\audio_loop_system\_ide\scripts\debugger_audio_loop-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent Zed 210248469116" && level==0 && jtag_device_ctx=="jsn-Zed-210248469116-03727093-0"}
fpga -file C:/Users/eza10/Documents/flag/audio_loop/_ide/bitstream/complete_design_wrapper.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw C:/Users/eza10/Documents/flag/complete_design_wrapper/export/complete_design_wrapper/hw/complete_design_wrapper.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source C:/Users/eza10/Documents/flag/audio_loop/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow -data C:/Users/eza10/Documents/flag/audio_loop/src/wasted.data 0x308d014
dow -data C:/Users/eza10/Documents/flag/audio_loop/src/instructions.data 0x28a4010
dow -data C:/Users/eza10/Documents/flag/audio_loop/src/menu.data 0x20bb00c
dow -data C:/Users/eza10/Documents/flag/audio_loop/src/proper_size_menu.data 0x18d2008
dow -data C:/Users/eza10/Documents/flag/audio_loop/src/yay.data 0x3876018
targets -set -nocase -filter {name =~ "*A9*#0"}
dow C:/Users/eza10/Documents/flag/audio_loop/Debug/audio_loop.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
