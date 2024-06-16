/*
 * Copyright (C) 2024 Paranoid Android
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "FingerprintEngineNX769J"

#include <android/log.h>
#include <log/log.h>

#include <FingerprintEngine.h>
#include <Legacy2Aidl.h>

#include "fingerprint_device_nx769j.h"

namespace aidl::android::hardware::biometrics::fingerprint {

FingerprintEngine::FingerprintEngine(fingerprint_device_t* device)
    : mDevice(reinterpret_cast<fingerprint_device_gf95xx*>(device)),
      mAuthId(0), mChallengeId(0) {}

FingerprintEngine::~FingerprintEngine() {
    if (mDevice == nullptr) {
        ALOGE("No valid device");
        return;
    }
    int err;
    if (0 != (err = mDevice->common.close(reinterpret_cast<hw_device_t*>(mDevice)))) {
        ALOGE("Can't close fingerprint module, error: %d", err);
        return;
    }
    mDevice = nullptr;
}

void FingerprintEngine::setActiveGroup(int userId) {
    mUserId = userId;
    char path[256];
    snprintf(path, sizeof(path), "/data/vendor_de/%d/fpdata/", userId);
    mDevice->set_active_group(mDevice, mUserId, path);
}

void FingerprintEngine::generateChallengeImpl(ISessionCallback* /*cb*/) {
    ALOGI("generateChallengeImpl");

    mDevice->generate_challenge(mDevice);
}

void FingerprintEngine::revokeChallengeImpl(ISessionCallback* cb, int64_t challenge) {
    ALOGI("revokeChallengeImpl");

    mDevice->revoke_challenge(mDevice, challenge);
}

void FingerprintEngine::enrollImpl(ISessionCallback* cb, const keymaster::HardwareAuthToken& hat) {
    ALOGI("enrollImpl");

    hw_auth_token_t authToken;
    translate(hat, authToken);
    int error = mDevice->enroll(mDevice, &authToken, mUserId, 60);
    if (error) {
        ALOGE("enroll failed: %d", error);
        cb->onError(Error::UNABLE_TO_PROCESS, error);
    }
}

void FingerprintEngine::authenticateImpl(ISessionCallback* cb, int64_t operationId) {
    ALOGI("authenticateImpl");

    int error = mDevice->authenticate(mDevice, operationId, mUserId);
    if (error) {
        ALOGE("authenticate failed: %d", error);
        cb->onError(Error::UNABLE_TO_PROCESS, error);
    }
}

void FingerprintEngine::detectInteractionImpl(ISessionCallback* cb) {
    ALOGE("detectInteractionImpl: not supported");

    cb->onError(Error::UNABLE_TO_PROCESS, 0 /* vendorCode */);
}

void FingerprintEngine::enumerateEnrollmentsImpl(ISessionCallback* cb) {
    ALOGI("enumerateEnrollmentsImpl");

    int error = mDevice->enumerate(mDevice);
    if (error) {
        ALOGE("enumerate failed: %d", error);
    }
}

void FingerprintEngine::removeEnrollmentsImpl(ISessionCallback* cb, const std::vector<int32_t>& enrollmentIds) {
    ALOGI("removeEnrollmentsImpl, size: %zu", enrollmentIds.size());

    int error = mDevice->remove(mDevice, enrollmentIds.data(), enrollmentIds.size());
    if (error) {
        ALOGE("remove failed: %d", error);
    }
}

void FingerprintEngine::getAuthenticatorIdImpl(ISessionCallback* cb) {
    ALOGI("getAuthenticatorIdImpl");

    mDevice->get_authenticator_id(mDevice);
}

void FingerprintEngine::invalidateAuthenticatorIdImpl(ISessionCallback* cb) {
    ALOGI("invalidateAuthenticatorIdImpl: %ld", mAuthId);

    mDevice->invalidate_authenticator_id(mDevice);
}

void FingerprintEngine::onPointerDownImpl(int32_t /*pointerId*/, int32_t /*x*/, int32_t /*y*/, float /*minor*/, float /*major*/) {
    ALOGI("onPointerDownImpl");
    if (mDevice->sendCustomizedCommand) {
        mDevice->sendCustomizedCommand(mDevice, 10, 1, CUSTOMIZED_COMMAND, CUSTOMIZED_COMMAND_LEN);
    } else {
        ALOGW("onPointerDownImpl: sendCustomizedCommand is nullptr");
    }
}

void FingerprintEngine::onPointerUpImpl(int32_t /*pointerId*/) {
    ALOGI("onPointerUpImpl");
    if (mDevice->sendCustomizedCommand) {
        mDevice->sendCustomizedCommand(mDevice, 10, 0, CUSTOMIZED_COMMAND, CUSTOMIZED_COMMAND_LEN);
    } else {
        ALOGW("onPointerDownImpl: sendCustomizedCommand is nullptr");
    }
}

void FingerprintEngine::onUiReadyImpl() {
    ALOGI("onUiReadyImpl: stub");
    // TODO: stub
}

ndk::ScopedAStatus FingerprintEngine::cancelImpl(ISessionCallback* cb) {
    ALOGI("cancelImpl");

    int ret = mDevice->cancel(mDevice);

    if (ret == 0) {
        cb->onError(Error::CANCELED, 0 /* vendorCode */);
        return ndk::ScopedAStatus::ok();
    } else {
        return ndk::ScopedAStatus::fromServiceSpecificError(ret);
    }
}

// Translate from errors returned by traditional HAL (see fingerprint.h) to
// AIDL-compliant Error
Error FingerprintEngine::VendorErrorFilter(int32_t error, int32_t* vendorCode) {
    *vendorCode = 0;

    switch (error) {
        case FINGERPRINT_ERROR_HW_UNAVAILABLE:
            return Error::HW_UNAVAILABLE;
        case FINGERPRINT_ERROR_UNABLE_TO_PROCESS:
            return Error::UNABLE_TO_PROCESS;
        case FINGERPRINT_ERROR_TIMEOUT:
            return Error::TIMEOUT;
        case FINGERPRINT_ERROR_NO_SPACE:
            return Error::NO_SPACE;
        case FINGERPRINT_ERROR_CANCELED:
            return Error::CANCELED;
        case FINGERPRINT_ERROR_UNABLE_TO_REMOVE:
            return Error::UNABLE_TO_REMOVE;
        case FINGERPRINT_ERROR_LOCKOUT: {
            *vendorCode = FINGERPRINT_ERROR_LOCKOUT;
            return Error::VENDOR;
        }
        default:
            if (error >= FINGERPRINT_ERROR_VENDOR_BASE) {
                // vendor specific code.
                *vendorCode = error - FINGERPRINT_ERROR_VENDOR_BASE;
                return Error::VENDOR;
            }
    }
    ALOGE("Unknown error from fingerprint vendor library: %d", error);
    return Error::UNABLE_TO_PROCESS;
}

// Translate acquired messages returned by traditional HAL (see fingerprint.h)
// to AIDL-compliant AcquiredInfo
AcquiredInfo FingerprintEngine::VendorAcquiredFilter(int32_t info, int32_t* vendorCode) {
    *vendorCode = 0;

    switch (info) {
        case FINGERPRINT_ACQUIRED_GOOD:
            return AcquiredInfo::GOOD;
        case FINGERPRINT_ACQUIRED_PARTIAL:
            return AcquiredInfo::PARTIAL;
        case FINGERPRINT_ACQUIRED_INSUFFICIENT:
            return AcquiredInfo::INSUFFICIENT;
        case FINGERPRINT_ACQUIRED_IMAGER_DIRTY:
            return AcquiredInfo::SENSOR_DIRTY;
        case FINGERPRINT_ACQUIRED_TOO_SLOW:
            return AcquiredInfo::TOO_SLOW;
        case FINGERPRINT_ACQUIRED_TOO_FAST:
            return AcquiredInfo::TOO_FAST;
        default:
            if (info >= FINGERPRINT_ACQUIRED_VENDOR_BASE) {
                // vendor specific code.
                *vendorCode = info - FINGERPRINT_ACQUIRED_VENDOR_BASE;
                return AcquiredInfo::VENDOR;
            }
    }
    ALOGE("Unknown acquiredmsg from fingerprint vendor library: %d", info);
    return AcquiredInfo::INSUFFICIENT;
}

bool FingerprintEngine::notifyImpl(ISessionCallback* cb, const fingerprint_msg_t* msg_generic, LockoutTracker& lockoutTracker) {
    const fingerprint_msg_nx769j* msg = reinterpret_cast<const fingerprint_msg_nx769j*>(msg_generic);
    //const uint64_t devId = reinterpret_cast<uint64_t>(mDevice);
    switch (msg->type) {
        case FINGERPRINT_NX769J_ERROR: {
            int32_t vendorCode = 0;
            Error result = VendorErrorFilter(msg->data.error, &vendorCode);
            ALOGD("onError(%d, %d)", result, vendorCode);
            cb->onError(result, vendorCode);
        } break;
        case FINGERPRINT_NX769J_ACQUIRED: {
            int32_t vendorCode = 0;
            AcquiredInfo result = VendorAcquiredFilter(msg->data.acquired.acquired_info, &vendorCode);
            ALOGD("onAcquired(%d, %d)", result, vendorCode);
            cb->onAcquired(result, vendorCode);
        } break;
        case FINGERPRINT_NX769J_TEMPLATE_ENROLLING: {
            ALOGD("onEnrollResult(fid=%d, gid=%d, rem=%d)", msg->data.enroll.finger.fid,
                  msg->data.enroll.finger.gid, msg->data.enroll.samples_remaining);
            cb->onEnrollmentProgress(msg->data.enroll.finger.fid,
                                      msg->data.enroll.samples_remaining);
        } break;
        case FINGERPRINT_NX769J_TEMPLATE_REMOVED: {
            ALOGD("onRemove(fid=%d, gid=%d, rem=%d)", msg->data.removed.finger.fid,
                  msg->data.removed.finger.gid, msg->data.removed.remaining_templates);
            std::vector<int> enrollments;
            enrollments.push_back(msg->data.removed.finger.fid);
            cb->onEnrollmentsRemoved(enrollments);
        } break;
        case FINGERPRINT_NX769J_AUTHENTICATED: {
            ALOGD("onAuthenticated(fid=%d, gid=%d)", msg->data.authenticated.finger.fid,
                msg->data.authenticated.finger.gid);
            if (msg->data.authenticated.finger.fid != 0) {
                const hw_auth_token_t hat = msg->data.authenticated.hat;
                HardwareAuthToken authToken;
                translate(hat, authToken);

                cb->onAuthenticationSucceeded(msg->data.authenticated.finger.fid, authToken);
                lockoutTracker.reset(true);
            } else {
                cb->onAuthenticationFailed();
                lockoutTracker.addFailedAttempt();
                return true;
            }
        } break;
        case FINGERPRINT_NX769J_TEMPLATE_ENUMERATING: {
            ALOGD("onEnumerate(fid=%d, gid=%d, rem=%d)", msg->data.enumerated.finger.fid,
                  msg->data.enumerated.finger.gid, msg->data.enumerated.remaining_templates);
            static std::vector<int> enrollments;
            enrollments.push_back(msg->data.enumerated.finger.fid);
            if (msg->data.enumerated.remaining_templates == 0) {
                cb->onEnrollmentsEnumerated(enrollments);
                enrollments.clear();
            }
        } break;
        case FINGERPRINT_NX769J_CHALLENGE_GENERATED: {
            mChallengeId = msg->data.generated.challenge;
            ALOGD("onChallengeGenerated: %ld", mChallengeId);
            cb->onChallengeGenerated(mChallengeId);
        } break;

        case FINGERPRINT_NX769J_CHALLENGE_REVOKED: {
            ALOGD("onChallengeRevoked: %ld", msg->data.revoked.challenge);
            cb->onChallengeRevoked(msg->data.revoked.challenge);
            if (mChallengeId == msg->data.revoked.challenge) {
                mChallengeId = 0;
            } else {
                ALOGE("onChallengeRevoked: wrong challenge revoked: %ld (current %ld)", msg->data.revoked.challenge, mChallengeId);
            }
        } break;

        case FINGERPRINT_NX769J_AUTHENTICATOR_ID_RETRIEVED: {
            mAuthId = msg->data.retrieved.authenticator_id;
            ALOGD("onAuthenticatorIdRetrieved: %ld", mAuthId);
            cb->onAuthenticatorIdRetrieved(mAuthId);
        } break;

        case FINGERPRINT_NX769J_AUTHENTICATOR_ID_INVALIDATED: {
            ALOGD("onAuthenticatorIdInvalidated: %ld", msg->data.invalidated.authenticator_id);
            cb->onAuthenticatorIdInvalidated(msg->data.invalidated.authenticator_id);
            if (mAuthId == msg->data.invalidated.authenticator_id) {
                mAuthId = 0;
            } else {
                ALOGE("onAuthenticatorIdInvalidated: wrong authentificator invalidated: %ld (current %ld)", msg->data.invalidated.authenticator_id, mAuthId);
            }
        } break;

        default:
            ALOGE("notifyImpl: Unknown message: %u", msg->type);
            abort();
            break;
    }

    return false;
}

}
