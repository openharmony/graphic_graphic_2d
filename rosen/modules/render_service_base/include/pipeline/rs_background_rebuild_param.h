/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RS_BACKGROUND_REBUILD_PARAM_H
#define RS_BACKGROUND_REBUILD_PARAM_H

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSBackgroundRebuildParam {
public:
    static RSBackgroundRebuildParam& Instance();

    void SetBackgroundRebuildEnabled(bool isEnable);
    bool IsBackgroundRebuildEnabled() const;

private:
    RSBackgroundRebuildParam() = default;
    bool isBackgroundRebuildEnabled_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_BACKGROUND_REBUILD_PARAM_H