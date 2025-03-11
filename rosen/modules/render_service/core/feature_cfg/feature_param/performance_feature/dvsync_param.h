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

    bool IsDVSyncEnable() const;
    bool IsUiDVSyncEnable() const;
    bool IsNativeDVSyncEnable() const;
    bool IsAdaptiveDVSyncEnable() const;
    uint32_t GetUiBufferCount() const;
    uint32_t GetRsBufferCount() const;
    uint32_t GetNativeBufferCount() const;
    uint32_t GetWebBufferCount() const;
    std::unordered_map<std::string, std::string> GetAdaptiveConfig() const;

protected:
    void SetDVSyncEnable(bool isEnable);
    void SetUiDVSyncEnable(bool isEnable);
    void SetNativeDVSyncEnable(bool isEnable);
    void SetAdaptiveDVSyncEnable(bool isEnable);
    void SetRsBufferCount(int32_t cnt);
    void SetUiBufferCount(int32_t cnt);
    void SetNativeBufferCount(int32_t cnt);
    void SetWebBufferCount(int32_t cnt);
    void SetAdaptiveConfig(const std::string &name, const std::string &val);

private:
    bool isRsDVSyncEnabled_ = false;
    bool isUiDVSyncEnabled_ = false;
    bool isNativeDVSyncEnabled_ = false;
    bool isAdaptiveDVSyncEnabled_ = false;
    uint32_t uiBufferCount_ = 3;
    uint32_t rsBufferCount_ = 1;
    uint32_t nativeBufferCount_ = 2;
    uint32_t webBufferCount_ = 1;
    std::unordered_map<std::string, std::string> adaptiveConfig_;

    friend class DVSyncParamParse;
};
} // namespace OHOS::Rosen
#endif // DVSYNC_PARAM_H