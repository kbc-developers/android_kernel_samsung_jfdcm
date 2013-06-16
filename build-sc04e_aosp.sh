#!/bin/bash

export BUILD_TARGET=AOSP
. sc04e.config

time ./_build-bootimg.sh $1
