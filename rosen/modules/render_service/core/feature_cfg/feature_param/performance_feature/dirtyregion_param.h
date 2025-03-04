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

    bool IsDirtyRegionEnable() const;

    bool IsExpandScreenDirtyRegionEnable() const;

    bool IsMirrorScreenDirtyRegionEnable() const;

    bool IsAdvancedDirtyRegionEnable() const;

    bool IsTileBasedAlignEnable() const;

    int GetTileBasedAlignBits() const;

protected:
    void SetDirtyRegionEnable(bool isEnable);

    void SetExpandScreenDirtyRegionEnable(bool isEnable);

    void SetMirrorScreenDirtyRegionEnable(bool isEnable);

    void SetAdvancedDirtyRegionEnable(bool isEnable);

    void SetTileBasedAlignEnable(bool isEnable);

    void SetTileBasedAlignBits(int tileBasedAlignBits);

private:
    bool isDirtyRegionEnable_ = true;
    bool isExpandScreenDirtyRegionEnable_ = true;
    bool isMirrorScreenDirtyRegionEnable_ = true;
    bool isAdvancedDirtyRegionEnable_ = false;
    bool isTileBasedAlignEnable_ = false;
    int tileBasedAlignBits_ = 128;
    friend class DirtyRegionParamParse;
};
} // namespace OHOS::Rosen
#endif // DIRTYREGION_PARAM_H