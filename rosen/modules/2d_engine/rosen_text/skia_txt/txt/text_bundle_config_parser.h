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

#ifdef OHOS_TEXT_ENABLE
#include "bundlemgr/bundle_mgr_interface.h"
#endif

namespace OHOS {
namespace Rosen {
namespace SPText {
class TextBundleConfigParser {
public:
    static bool IsAdapterTextHeightEnabled();
private:
#ifdef OHOS_TEXT_ENABLE
    static bool IsMetaDataExistInModule(const std::string& metaData);
    static sptr<AppExecFwk::IBundleMgr> GetSystemAbilityManager();
#endif
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_BUNDLE_MANAGER_H