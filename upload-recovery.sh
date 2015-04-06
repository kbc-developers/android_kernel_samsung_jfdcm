#!/bin/bash

adb "wait-for-device"

DATA_TMP=/data/local/tmp

BOOT_IMG=./out/SC04E/AOSP/bin/recovery.img
LOKI_FLASH=./release-tools/loki/loki_flash
adb push $BOOT_IMG $DATA_TMP
adb push $LOKI_FLASH $DATA_TMP

adb shell chmod 755 $DATA_TMP/loki_flash
adb shell su -c "$DATA_TMP/loki_flash recovery $DATA_TMP/recovery.img"
adb shell su -c "sync;sync;sync;sleep 2; reboot recovery"

