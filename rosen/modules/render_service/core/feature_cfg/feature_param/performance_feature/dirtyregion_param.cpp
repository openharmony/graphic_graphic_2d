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

#include "dirtyregion_param.h"

namespace OHOS::Rosen {
bool DirtyRegionParam::IsDirtyRegionEnable()
{
    return isDirtyRegionEnable_;
}

void DirtyRegionParam::SetDirtyRegionEnable(bool isEnable)
{
    isDirtyRegionEnable_ = isEnable;
}

bool DirtyRegionParam::IsExpandScreenDirtyRegionEnable()
{
    return isExpandScreenDirtyRegionEnable_;
}

void DirtyRegionParam::SetExpandScreenDirtyRegionEnable(bool isEnable)
{
    isExpandScreenDirtyRegionEnable_ = isEnable;
}

bool DirtyRegionParam::IsMirrorScreenDirtyRegionEnable()
{
    return isMirrorScreenDirtyRegionEnable_;
}

void DirtyRegionParam::SetMirrorScreenDirtyRegionEnable(bool isEnable)
{
    isMirrorScreenDirtyRegionEnable_ = isEnable;
}

bool DirtyRegionParam::IsAdvancedDirtyRegionEnable()
{
    return isAdvancedDirtyRegionEnable_;
}

void DirtyRegionParam::SetAdvancedDirtyRegionEnable(bool isEnable)
{
    isAdvancedDirtyRegionEnable_ = isEnable;
}

bool DirtyRegionParam::IsComposeDirtyRegionEnableInPartialDisplay()
{
    return isComposeDirtyRegionEnableInPartialDisplay_;
}

void DirtyRegionParam::SetComposeDirtyRegionEnableInPartialDisplay(bool isEnable)
{
    isComposeDirtyRegionEnableInPartialDisplay_ = isEnable;
}

bool DirtyRegionParam::IsTileBasedAlignEnable()
{
    return isTileBasedAlignEnable_;
}

void DirtyRegionParam::SetTileBasedAlignEnable(bool isEnable)
{
    isTileBasedAlignEnable_ = isEnable;
}

int DirtyRegionParam::GetTileBasedAlignBits()
{
    return tileBasedAlignBits_;
}

void DirtyRegionParam::SetTileBasedAlignBits(int tileBasedAlignBits)
{
    tileBasedAlignBits_ = tileBasedAlignBits;
}
} // namespace OHOS::Rosen