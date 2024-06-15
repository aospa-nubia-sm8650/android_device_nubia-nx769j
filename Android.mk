#
# Copyright (C) 2024 Paranoid Android
#
# SPDX-License-Identifier: Apache-2.0
#

LOCAL_PATH := $(call my-dir)

ifeq ( $(TARGET_DEVICE), nx769j )
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
