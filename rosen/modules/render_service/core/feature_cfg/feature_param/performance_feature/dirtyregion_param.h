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

#ifndef DIRTYREGION_PARAM_H
#define DIRTYREGION_PARAM_H

#include "feature_param.h"

namespace OHOS::Rosen {
class DirtyRegionParam : public FeatureParam {
public:
    DirtyRegionParam() = default;

    ~DirtyRegionParam() = default;

    satic bool IsDirtyRegionEnable();

    satic bool IsExpandScreenDirtyRegionEnable();

    satic bool IsMirrorScreenDirtyRegionEnable();

    satic bool IsAdvancedDirtyRegionEnable();

    satic bool IsTileBasedAlignEnable();

    satic int GetTileBasedAlignBits();

protected:
    satic void SetDirtyRegionEnable(bool isEnable);

    satic void SetExpandScreenDirtyRegionEnable(bool isEnable);

    satic void SetMirrorScreenDirtyRegionEnable(bool isEnable);

    satic void SetAdvancedDirtyRegionEnable(bool isEnable);

    satic void SetTileBasedAlignEnable(bool isEnable);

    satic void SetTileBasedAlignBits(int tileBasedAlignBits);

private:
    inline static bool isDirtyRegionEnable_ = true;
    inline static bool isExpandScreenDirtyRegionEnable_ = true;
    inline static bool isMirrorScreenDirtyRegionEnable_ = true;
    inline static bool isAdvancedDirtyRegionEnable_ = false;
    inline static bool isTileBasedAlignEnable_ = false;
    inline static int tileBasedAlignBits_ = 128;
    friend class DirtyRegionParamParse;
};
} // namespace OHOS::Rosen
#endif // DIRTYREGION_PARAM_H