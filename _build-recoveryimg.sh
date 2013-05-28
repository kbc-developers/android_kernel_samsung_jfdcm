#!/bin/bash

KERNEL_DIR=$PWD
IMAGE_NAME=recovery

BIN_DIR=out/$TARGET_DEVICE/$BUILD_TARGET/bin
OBJ_DIR=out/$TARGET_DEVICE/$BUILD_TARGET/obj
mkdir -p $BIN_DIR
mkdir -p $OBJ_DIR

. build_func
. recovery_version

echo BUILD_RECOVERYVERSION $BUILD_RECOVERYVERSION

# set build env
BUILD_LOCALVERSION=$BUILD_RECOVERYVERSION

echo ""
echo "====================================================================="
echo "    BUILD START (RECOVERY VERSION $BUILD_LOCALVERSION)"
echo "====================================================================="

# copy RAMDISK
echo ""
echo "=====> COPY RAMDISK"
copy_ramdisk

echo ""
echo "=====> CREATE RELEASE IMAGE"
# clean release dir
if [ `find $BIN_DIR -type f | wc -l` -gt 0 ]; then
  rm -rf $BIN_DIR/*
fi
mkdir -p $BIN_DIR

# copy zImage -> kernel
cp ./release-tools/$TARGET_DEVICE/stock-img/recovery.img-kernel.gz $BIN_DIR/kernel

# create boot image
make_boot_image

# create odin image
cd $BIN_DIR
make_odin3_image

# create cwm image
make_cwm_image

cd $KERNEL_DIR

echo ""
echo "====================================================================="
echo "    BUILD COMPLETED"
echo "====================================================================="
exit 0
