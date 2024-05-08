The design directory comprises design sources containing "design_1_wrapper" as the top-level entity, zedboard_master.xdc for constraints, and design_1.vhd representing the main VHDL module. 
Additionally, the design_1.bd directory potentially holds block design files, while simulation sources encompass "design_1_wrapper" for simulation setup. 
This organized structure enables comprehensive development, from VHDL modeling to constraint definition and simulation, facilitating efficient design verification and implementation on the ZedBoard FPGA platform.

The structure goes as follows:
Vitis
- 0th core
```- Src```
      Color_codes.h
      Game_design.c
      Game_design.h
      Sprite_dev.cc
      Sprite_dev.h
      Usb.c
      Usb.h
      Usb_example.c
      Usb_example.h
      lscript.ld
  1st core
    src
      Bin
        Coin.bin
        test.bin
    Blob.s
    main.cc
    lscript.ld

Vivado
  Ip_repo
    Random_number_generator
    Zed_audio_ctrl
    Vga_controller_ip
  Last_hardware_update
  
Scripts
  jpgToHex.py
  wavToBin.py


NOTE: Import using Vitis' import functinality for a smooth run.
