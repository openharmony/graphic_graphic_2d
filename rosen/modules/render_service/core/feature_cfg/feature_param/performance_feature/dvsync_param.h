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
class DvsyncParam : public FeatureParam {
public:
    DvsyncParam() = default;
    ~DvsyncParam() = default;

    bool IsDvsyncEnable() const;
    bool IsUiDvsyncEnable() const;
    bool IsNativeDvsyncEnable() const;

    int32_t GetRsPreCount() const;
    int32_t GetAppPreCount() const;
    int32_t GetNativePreCount() const;

    void SetDvsyncEnable(bool isEnable);
    void SetUiDvsyncEnable(bool isEnable);
    void SetNativeDvsyncEnable(bool isEnable);
    void SetRsPreCount(int32_t cnt);
    void SetAppPreCount(int32_t cnt);
    void SetNativePreCount(int32_t cnt);

private:
    bool isDvsyncEnable_;
    bool isUiDvsyncEnable_;
    bool isNativeDvsyncEnable_;

    int32_t rsPreCnt_;
    int32_t appPreCnt_;
    int32_t nativePreCnt_;
};
} // namespace OHOS::Rosen
#endif // DVSYNC_PARAM_H