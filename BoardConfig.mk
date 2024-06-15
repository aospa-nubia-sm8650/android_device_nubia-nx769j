#
# Copyright (C) 2024 Paranoid Android
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from nubia-sm8650-common
include device/nubia/sm8650-common/BoardConfigCommon.mk

DEVICE_PATH := device/nubia/nx769j

# DPI
TARGET_SCREEN_DENSITY := 480

# Init
TARGET_INIT_VENDOR_LIB := //$(DEVICE_PATH):libinit_nx769j
TARGET_RECOVERY_DEVICE_MODULES := libinit_nx769j


# OTA
TARGET_OTA_ASSERT_DEVICE := NX769J

# Properties
TARGET_ODM_PROP += $(DEVICE_PATH)/configs/properties/odm.prop
TARGET_SYSTEM_PROP += $(DEVICE_PATH)/configs/properties/system.prop
TARGET_VENDOR_PROP += $(DEVICE_PATH)/configs/properties/vendor.prop

# VINTF
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE += \
    $(DEVICE_PATH)/configs/vintf/framework_matrix_nubia.xml
