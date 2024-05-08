# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: C:\Users\eza10\Documents\flag\audio_loop_system\_ide\scripts\systemdebugger_audio_loop_system_standalone.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source C:\Users\eza10\Documents\flag\audio_loop_system\_ide\scripts\systemdebugger_audio_loop_system_standalone.tcl
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
dow C:/Users/eza10/Documents/flag/audio_loop/Debug/audio_loop.elf
targets -set -nocase -filter {name =~ "*A9*#1"}
dow C:/Users/eza10/Documents/flag/hello-2nd-core/Debug/hello-2nd-core.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
targets -set -nocase -filter {name =~ "*A9*#1"}
con
