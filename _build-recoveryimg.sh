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

BUILD_SELECT="i"
if [ ! -n "$1" ]; then
  echo ""
  read -p "select build? [(i)mage/(k)ernel/(u)pdate default:image] " BUILD_SELECT
else
  BUILD_SELECT=$1
fi
# make start
if [ "$BUILD_SELECT" != 'image' -a "$BUILD_SELECT" != 'i' ]; then
	if [ "$BUILD_SELECT" = 'kernel' -o "$BUILD_SELECT" = 'k' ]; then
	  KERNEL_DEFCONFIG=kbc_sc04e_aosp_defconfig
	  echo ""
	  echo "=====> CLEANING..."
	  make clean
	  cp -f ./arch/arm/configs/$KERNEL_DEFCONFIG $OBJ_DIR/.config
	  make -C $PWD O=$OBJ_DIR oldconfig || exit -1
	fi

	echo ""
	echo "=====> BUILDING..."
	if [ -e make.log ]; then
	mv make.log make_old.log
	fi
	nice -n 10 make O=$OBJ_DIR -j12 2>&1 | tee make.log


	# check compile error
	COMPILE_ERROR=`grep 'error:' ./make.log`
	if [ "$COMPILE_ERROR" ]; then
	  echo ""
	  echo "=====> ERROR"
	  grep 'error:' ./make.log
	  exit -1
	fi

	# *.ko replace
#	echo ""
#	echo "=====> INSTALL KERNEL MODULES"
#	find -name '*.ko' -exec cp -av {} $RAMDISK_TMP_DIR/lib/modules/ \;

  cp $OBJ_DIR/arch/arm/boot/zImage ./release-tools/$TARGET_DEVICE/stock-img/recovery.img-kernel.gz
fi

# copy zImage -> kernel
cp ./release-tools/$TARGET_DEVICE/stock-img/recovery.img-kernel.gz $BIN_DIR/kernel

# create boot image
make_boot_image

if [ "$USE_LOKI" = 'y' ]; then
  make_loki_image
fi

#check image size
img_size=`wc -c $BIN_DIR/$IMAGE_NAME.img | awk '{print $1}'`
if [ $img_size -gt $IMG_MAX_SIZE ]; then
    echo "FATAL: $IMAGE_NAME image size over. image size = $img_size > $IMG_MAX_SIZE byte"
    rm $BIN_DIR/$IMAGE_NAME.img
    exit -1
fi

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
