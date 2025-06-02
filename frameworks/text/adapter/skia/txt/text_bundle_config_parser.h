/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_MODULES_SPTEXT_BUNDLE_MANAGER_H
#define ROSEN_MODULES_SPTEXT_BUNDLE_MANAGER_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>

#ifdef ENABLE_OHOS_ENHANCE
#include "bundlemgr/bundle_mgr_interface.h"
#endif

namespace OHOS {
namespace Rosen {
namespace SPText {
constexpr uint32_t SINCE_API18_VERSION = 18;

class TextBundleConfigParser {
public:
    static TextBundleConfigParser& GetInstance()
    {
        static TextBundleConfigParser instance;
        return instance;
    }

    bool IsAdapterTextHeightEnabled() const;
    bool IsTargetApiVersion(size_t targetVersion) const;

private:
    TextBundleConfigParser()
    {
        InitBundleInfo();
    };

    ~TextBundleConfigParser() {};

    TextBundleConfigParser(const TextBundleConfigParser&) = delete;
    TextBundleConfigParser& operator=(const TextBundleConfigParser&) = delete;
    TextBundleConfigParser(TextBundleConfigParser&&) = delete;
    TextBundleConfigParser& operator=(TextBundleConfigParser&&) = delete;

    uint32_t bundleApiVersion_{0};
    bool adapterTextHeightEnable_{false};
    bool initStatus_{false};

    void InitBundleInfo();
    void InitTextBundleConfig();
    void InitTextBundleFailed();

#ifdef ENABLE_OHOS_ENHANCE
    bool IsMetaDataExistInModule(const std::string& metaData, const AppExecFwk::BundleInfo& bundleInfo);
    bool GetBundleInfo(AppExecFwk::BundleInfo& bundleInfo);
#endif
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_BUNDLE_MANAGER_H