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

    static bool IsDirtyRegionEnable();

    static bool IsExpandScreenDirtyRegionEnable();

    static bool IsMirrorScreenDirtyRegionEnable();

    static bool IsAdvancedDirtyRegionEnable();

    static bool IsTileBasedAlignEnable();

    static int GetTileBasedAlignBits();

protected:
    static void SetDirtyRegionEnable(bool isEnable);

    static void SetExpandScreenDirtyRegionEnable(bool isEnable);

    static void SetMirrorScreenDirtyRegionEnable(bool isEnable);

    static void SetAdvancedDirtyRegionEnable(bool isEnable);

    static void SetTileBasedAlignEnable(bool isEnable);

    static void SetTileBasedAlignBits(int tileBasedAlignBits);

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