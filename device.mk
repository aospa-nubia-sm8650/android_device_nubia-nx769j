#
# Copyright (C) 2024 Paranoid Android
#
# SPDX-License-Identifier: Apache-2.0
#

# Get common SoC device configs
$(call inherit-product, device/nubia/sm8650-common/pineapple.mk)

# Get non-open-source specific aspects
$(call inherit-product, vendor/nubia/nx769j/nx769j-vendor.mk)

# Setup DLKM Kernel configs
$(call inherit-product, device/qcom/common/dlkm/kernel-platform.mk)


# Kernel (prebuilt)
KERNEL_PREBUILT_DIR := device/nubia/nx769j-kernel

# Audio configs
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/audio/audio_effects.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio/sku_pineapple/audio_effects.xml \
    $(LOCAL_PATH)/configs/audio/audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio/sku_pineapple/audio_policy_configuration.xml \
    $(LOCAL_PATH)/configs/audio/audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio/sku_pineapple/audio_policy_configuration.xml

# Shipping API Level
BOARD_API_LEVEL := 34
BOARD_SHIPPING_API_LEVEL := 34
PRODUCT_SHIPPING_API_LEVEL := 34

# Vibrator configs
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/vibrator/excluded-input-devices.xml:$(TARGET_COPY_OUT_VENDOR)/etc/excluded-input-devices.xml

# Soong namespaces
PRODUCT_SOONG_NAMESPACES += \
    $(LOCAL_PATH)
