/*
 * Copyright (C) 2024 Paranoid Android
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstring>

#include <hardware/fingerprint.h>
#include <hardware/hardware.h>

#define CUSTOMIZED_COMMAND "FingerService"
#define CUSTOMIZED_COMMAND_LEN strlen("FingerService")

struct fingerprint_device_gf95xx {
    hw_device_t common;
    fingerprint_notify_t notify;
    int (*set_notify)(fingerprint_device_gf95xx *dev, fingerprint_notify_t notify);
    int (*generate_challenge)(fingerprint_device_gf95xx *dev); // returns msg type 7
    int (*revoke_challenge)(fingerprint_device_gf95xx *dev, uint64_t challenge); // returns msg type 8
    int (*enroll)(fingerprint_device_gf95xx *dev, const hw_auth_token_t *hat, uint32_t gid, uint32_t timeout_sec);
    int (*get_authenticator_id)(fingerprint_device_gf95xx *dev); // returns msg type 9
    int (*invalidate_authenticator_id)(fingerprint_device_gf95xx *dev); // returns msg type 10
    int (*cancel)(fingerprint_device_gf95xx *dev);
    int (*enumerate)(fingerprint_device_gf95xx *dev);
    int (*remove)(fingerprint_device_gf95xx *dev, const int32_t *fids, uint32_t count);
    int (*set_active_group)(fingerprint_device_gf95xx *dev, uint32_t gid, const char *store_path);
    int (*authenticate)(fingerprint_device_gf95xx *dev, uint64_t operation_id, uint32_t gid);
    void *reserved1[2];
    int (*sendCustomizedCommand)(fingerprint_device_gf95xx *device, uint32_t cmd, uint32_t extras, const char *msg, ssize_t len);
    void *reserved2[2];
};

enum fingerprint_msg_type_nx769j {
    FINGERPRINT_NX769J_ERROR = FINGERPRINT_ERROR,
    FINGERPRINT_NX769J_ACQUIRED = FINGERPRINT_ACQUIRED,
    FINGERPRINT_NX769J_TEMPLATE_ENROLLING = FINGERPRINT_TEMPLATE_ENROLLING,
    FINGERPRINT_NX769J_TEMPLATE_REMOVED = FINGERPRINT_TEMPLATE_REMOVED,
    FINGERPRINT_NX769J_AUTHENTICATED = FINGERPRINT_AUTHENTICATED,
    FINGERPRINT_NX769J_TEMPLATE_ENUMERATING = FINGERPRINT_TEMPLATE_ENUMERATING,
    FINGERPRINT_NX769J_CHALLENGE_GENERATED = 7,
    FINGERPRINT_NX769J_CHALLENGE_REVOKED = 8,
    FINGERPRINT_NX769J_AUTHENTICATOR_ID_RETRIEVED = 9,
    FINGERPRINT_NX769J_AUTHENTICATOR_ID_INVALIDATED = 10,
};

typedef struct fingerprint_challenge_generated {
    uint64_t challenge;
} fingerprint_challenge_generated_t;

typedef struct fingerprint_challenge_revoked {
    uint64_t challenge;
} fingerprint_challenge_revoked_t;

typedef struct fingerprint_authenticator_id_retrieved {
    uint64_t authenticator_id;
} fingerprint_authenticator_id_retrieved_t;

typedef struct fingerprint_authenticator_id_invalidated {
    uint64_t authenticator_id;
} fingerprint_authenticator_id_invalidated_t;

struct fingerprint_msg_nx769j {
    fingerprint_msg_type_nx769j type;
    union {
        fingerprint_error_t error; // 1
        fingerprint_enroll_t enroll; // 2
        fingerprint_enumerated_t enumerated; // 3
        fingerprint_removed_t removed; //4
        fingerprint_acquired_t acquired; // 5
        fingerprint_authenticated_t authenticated; //6
        fingerprint_challenge_generated_t generated; // 7
        fingerprint_challenge_revoked_t revoked; // 8
        fingerprint_authenticator_id_retrieved_t retrieved; // 9
        fingerprint_authenticator_id_invalidated_t invalidated; // 10
    } data;
};
