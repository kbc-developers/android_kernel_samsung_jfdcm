#!/bin/bash

export BUILD_TARGET=SAM
. sc04e.config

time ./_build-bootimg.sh $1
