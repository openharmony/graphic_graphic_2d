/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DVSYNC_PARAM_H
#define DVSYNC_PARAM_H

#include "feature_param.h"

namespace OHOS::Rosen {
class DVSyncParam : public FeatureParam {
public:
    DVSyncParam() = default;
    ~DVSyncParam() = default;

    static bool IsDVSyncEnable();
    static bool IsUiDVSyncEnable();
    static bool IsNativeDVSyncEnable();
    static bool IsAdaptiveDVSyncEnable();
    static uint32_t GetUiBufferCount();
    static uint32_t GetRsBufferCount();
    static uint32_t GetNativeBufferCount();
    static uint32_t GetWebBufferCount();
    static std::unordered_map<std::string, std::string> GetAdaptiveConfig();

protected:
    static void SetDVSyncEnable(bool isEnable);
    static void SetUiDVSyncEnable(bool isEnable);
    static void SetNativeDVSyncEnable(bool isEnable);
    static void SetAdaptiveDVSyncEnable(bool isEnable);
    static void SetRsBufferCount(int32_t cnt);
    static void SetUiBufferCount(int32_t cnt);
    static void SetNativeBufferCount(int32_t cnt);
    static void SetWebBufferCount(int32_t cnt);
    static void SetAdaptiveConfig(const std::string &name, const std::string &val);

private:
    inline static bool isRsDVSyncEnabled_ = false;
    inline static bool isUiDVSyncEnabled_ = false;
    inline static bool isNativeDVSyncEnabled_ = false;
    inline static bool isAdaptiveDVSyncEnabled_ = false;
    inline static uint32_t uiBufferCount_ = 3;
    inline static uint32_t rsBufferCount_ = 1;
    inline static uint32_t nativeBufferCount_ = 2;
    inline static uint32_t webBufferCount_ = 1;
    inline static std::unordered_map<std::string, std::string> adaptiveConfig_;

    friend class DVSyncParamParse;
};
} // namespace OHOS::Rosen
#endif // DVSYNC_PARAM_H