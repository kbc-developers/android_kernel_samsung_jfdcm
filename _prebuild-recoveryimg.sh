#!/bin/bash

KERNEL_DIR=$PWD
IMAGE_NAME=recovery

BIN_DIR=out/$TARGET_DEVICE/$BUILD_TARGET/bin
OBJ_DIR=out/$TARGET_DEVICE/$BUILD_TARGET/obj
mkdir -p $BIN_DIR
mkdir -p $OBJ_DIR

. build_func

RECOVERY_VERSION=recovery_version
if [ -f $RAMDISK_SRC_DIR/recovery_version ]; then
    RECOVERY_VERSION=$RAMDISK_SRC_DIR/recovery_version
fi
. $RECOVERY_VERSION

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
#REBUILD_IMAGE=./release-tools/$TARGET_DEVICE/stock-img/recovery.img-kernel.gz
PREBUILD_IMAGE=./release-tools/$TARGET_DEVICE/prebuild-img/zImage
echo "use image : ${PREBUILD_IMAGE}"
cp ${PREBUILD_IMAGE} $BIN_DIR/kernel

# create recovery image
make_recovery_image

#check image size
img_size=`wc -c $BIN_DIR/$IMAGE_NAME.img | awk '{print $1}'`
if [ $img_size -gt $IMG_MAX_SIZE ]; then
    echo "FATAL: $IMAGE_NAME image size over. image size = $img_size > $IMG_MAX_SIZE byte"
    rm $BIN_DIR/$IMAGE_NAME.img
    exit -1
fi

cd $BIN_DIR

# create odin image
make_odin3_image

# create cwm image
make_cwm_image

cd $KERNEL_DIR

echo ""
echo "====================================================================="
echo "    BUILD COMPLETED"
echo "====================================================================="
exit 0
