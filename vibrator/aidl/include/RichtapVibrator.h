// Copyright (C) 2024 Paranoid Android
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "aidl/vendor/aac/hardware/richtap/vibrator/BnRichtapVibrator.h"
#include <thread>


namespace aidl {
namespace vendor {
namespace aac {
namespace hardware {
namespace richtap {
namespace vibrator {

class RichtapVibrator : public BnRichtapVibrator {
public:
   RichtapVibrator();
   ~RichtapVibrator();

  ndk::ScopedAStatus init(const std::shared_ptr<::aidl::vendor::aac::hardware::richtap::vibrator::IRichtapCallback>& in_callback) override;
  ndk::ScopedAStatus setDynamicScale(int32_t in_scale, const std::shared_ptr<::aidl::vendor::aac::hardware::richtap::vibrator::IRichtapCallback>& in_callback) override;
  ndk::ScopedAStatus setF0(int32_t in_f0, const std::shared_ptr<::aidl::vendor::aac::hardware::richtap::vibrator::IRichtapCallback>& in_callback) override;
  ndk::ScopedAStatus stop(const std::shared_ptr<::aidl::vendor::aac::hardware::richtap::vibrator::IRichtapCallback>& in_callback) override;
  ndk::ScopedAStatus setAmplitude(int32_t in_amplitude, const std::shared_ptr<::aidl::vendor::aac::hardware::richtap::vibrator::IRichtapCallback>& in_callback) override;
  ndk::ScopedAStatus performHeParam(int32_t in_interval, int32_t in_amplitude, int32_t in_freq, const std::shared_ptr<::aidl::vendor::aac::hardware::richtap::vibrator::IRichtapCallback>& in_callback) override;
  ndk::ScopedAStatus off(const std::shared_ptr<::aidl::vendor::aac::hardware::richtap::vibrator::IRichtapCallback>& in_callback) override;
  ndk::ScopedAStatus on(int32_t in_duration, const std::shared_ptr<::aidl::vendor::aac::hardware::richtap::vibrator::IRichtapCallback>& in_callback) override;
  ndk::ScopedAStatus perform(int32_t in_effect, int8_t in_enable, const std::shared_ptr<::aidl::vendor::aac::hardware::richtap::vibrator::IRichtapCallback>& in_callback, int32_t* _aidl_return) override;
  ndk::ScopedAStatus performEnvelope(const std::vector<int32_t>& in_envInfo, bool in_fastFlag, const std::shared_ptr<::aidl::vendor::aac::hardware::richtap::vibrator::IRichtapCallback>& in_callback) override;
  ndk::ScopedAStatus performRtp(const ::ndk::ScopedFileDescriptor& in_file, const std::shared_ptr<::aidl::vendor::aac::hardware::richtap::vibrator::IRichtapCallback>& in_callback) override;
  ndk::ScopedAStatus performHe(int32_t in_looper, int32_t in_interval, int32_t in_amplitude, int32_t in_freq, const std::vector<int32_t>& in_data, const std::shared_ptr<::aidl::vendor::aac::hardware::richtap::vibrator::IRichtapCallback>& in_callback) override;
private:
};

}  // namespace vibrator
}  // namespace richtap
}  // namespace hardware
}  // namespace aac
}  // namespace vendor
}  // namespace aidl
