#!/bin/bash

mkdir build_topdir
cd build_topdir
top_dir=$PWD
export PATH=/usr/bin/:$PATH
export PATH=$top_dir/u-boot-xlnx/tools:$PATH

Xilinx_SDK_dir=

compiler_path=$Xilinx_SDK_dir/gnu/arm/lin
compiler_prefix=arm-xilinx-linux-gnueabi
hsi=$Xilinx_SDK_dir/bin/xsct
export CROSS_COMPILE=$compiler_path/bin/$compiler_prefix-

###############GET SRC FROM PROJ
cd $top_dir
rm -rf sd_card/*
tar -xvzf ../kmtz_linux.tar.gz
tar -xvzf configs.tar.gz
tar -xvzf vivado_project.tar.gz
tar -xvzf old_linux_files.tar.gz

### FSBL
cd $top_dir
mkdir fsbl
cd fsbl
echo "hsi open_hw_design $top_dir/zsys_wrapper.hdf; hsi generate_app -hw zsys_wrapper -os standalone -proc ps7_cortexa9_0 -app zynq_fsbl -compile -sw fsbl -dir ./fsbl" > hsi_fsbl.tcl
$hsi hsi_fsbl.tcl

### DTS
cd $top_dir
git clone --recursive https://github.com/Xilinx/device-tree-xlnx.git
mkdir device_tree_hsi
cd device_tree_hsi
echo "hsi open_hw_design $top_dir/zsys_wrapper.hdf;
hsi set_repo_path $top_dir/device-tree-xlnx;
hsi create_sw_design device-tree -os device_tree -proc ps7_cortexa9_0;
hsi set_property CONFIG.periph_type_overrides \"{BOARD te0726-03m}\" [hsi::get_os];
hsi generate_target -dir $top_dir/device_tree_hsi" > hsi_dtree.tcl
$hsi hsi_dtree.tcl

### UBOOT
cd $top_dir
git clone --recursive https://github.com/Xilinx/u-boot-xlnx.git
cd u-boot-xlnx
git checkout tags/xilinx-v2016.3
make zynq_zc702_defconfig
make
export PATH=$top_dir/u-boot-xlnx/tools/:$PATH

###############BUILDROOT
cd $top_dir
git clone --recursive https://github.com/buildroot/buildroot.git
cd buildroot
git checkout tags/2018.02

cp $top_dir/configs/buildroot_kmtz_defconfig configs/kmtz_defconfig

echo "# custom part start" >> configs/kmtz_defconfig
echo "BR2_TOOLCHAIN_EXTERNAL_PATH=\"$compiler_path\"" >> configs/kmtz_defconfig
echo "BR2_TOOLCHAIN_EXTERNAL_CUSTOM_PREFIX=\"$compiler_prefix\"" >> configs/kmtz_defconfig
echo "BR2_PACKAGE_FFMPEG=y" >> configs/kmtz_defconfig
echo "BR2_PACKAGE_WF111=y" >> configs/kmtz_defconfig
echo "BR2_PACKAGE_WIRELESS_TOOLS=y" >> configs/kmtz_defconfig
echo "BR2_PACKAGE_WPA_SUPPLICANT=y" >> configs/kmtz_defconfig
echo "BR2_LINUX_KERNEL_USE_CUSTOM_CONFIG=y" >> configs/kmtz_defconfig
echo "BR2_LINUX_KERNEL_CUSTOM_CONFIG_FILE=\"$top_dir/configs/lnx_kmtz_wf111_defconfig\"" >> configs/kmtz_defconfig
echo "# custom part end" >> configs/kmtz_defconfig

make kmtz_defconfig
make

### Magic with generated dts
cd $top_dir/device_tree_hsi
dtc -I dts -O dtb -o devicetree.dtb system-top.dts
dtc -I dtb -O dts -o custom.dts devicetree.dtb
sed -i -e 's|serial0 = "/amba/serial@e0000000"|serial0 = "/amba/serial@e0001000"|' custom.dts
sed -i -e 's|serial1 = "/amba/serial@e0001000"|serial1 = "/amba/serial@e0000000"|' custom.dts
sed -i -e 's|bootargs = "earlycon"|bootargs = "console=ttyPS0,115200 earlyprintk"|' custom.dts
rm devicetree.dtb
dtc -I dts -O dtb -o devicetree.dtb custom.dts

### CP SRC
cd $top_dir
mkdir sd_card
cp $top_dir/configs/ub_config.its sd_card/ub_config.its
cp $top_dir/buildroot/output/images/zImage sd_card/Image
cp $top_dir/buildroot/output/images/rootfs.cpio sd_card/ramdisk.cpio

### DTBs ###
cp $top_dir/device_tree_hsi/devicetree.dtb sd_card/system.dtb

cd sd_card
mkimage -f ub_config.its image.ub

### USEFUL COMMANDS
# dtc -I dtb -O dts -o devicetree.dts devicetree.dtb # make dts from dtb
# dtc -I dts -O dtb -o devicetree.dtb frombrt.dts    # opposite above command
# dumpimage -T flat_dt -p 1 -i image.ub system.dtb   # get #1 struct from .ub
# dumpimage -l image.ub # uimage.its
# /home/kmtz-linux/kmtz_linux/build_topdir/buildroot/output/build/linux-xlnx_rebase_v4.9_2017.3/scripts/extract-ikconfig image.ub > oldconfig.txt
