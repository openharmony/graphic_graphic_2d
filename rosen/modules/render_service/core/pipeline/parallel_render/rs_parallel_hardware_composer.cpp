/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_parallel_hardware_composer.h"
 
namespace OHOS {
namespace Rosen {

RSParallelSelfDrawingSurfaceShape::RSParallelSelfDrawingSurfaceShape(bool isRRect, RectF rect, Vector4f cornerRadius)
    : isRRect_(isRRect), rect_(rect), cornerRadius_(cornerRadius) {};

void RSParallelHardwareComposer::Init(uint32_t threadNum)
{
    std::map<unsigned int, Holes>().swap(surfaceAndHolesMap_);
    // to avoid crash caused by multi-thread, init surfaceAndHolesMap_ firstly.
    for (unsigned int i = 0; i < threadNum; i++) {
        surfaceAndHolesMap_[i] = Holes();
    }
}

void RSParallelHardwareComposer::ClearTransparentColor(std::shared_ptr<RSPaintFilterCanvas> canvas,
    unsigned int surfaceIndex)
{
    const auto &holes = surfaceAndHolesMap_[surfaceIndex];
    if (holes.size() == 0) {
        return;
    }
    for (const auto& hole : holes) {
        canvas->save();
        if (hole->IsRRect()) {
            canvas->clipRRect(hole->GetRRect(), true);
        } else {
            canvas->clipRect(hole->GetRect());
        }
        canvas->clear(SK_ColorTRANSPARENT);
        canvas->restore();
    }
}

void RSParallelHardwareComposer::AddTransparentColorArea(unsigned int surfaceIndex,
    std::unique_ptr<RSParallelSelfDrawingSurfaceShape> && shape)
{
    if (surfaceAndHolesMap_.count(surfaceIndex) > 0) {
        surfaceAndHolesMap_[surfaceIndex].push_back(std::move(shape));
    } else {
        RS_LOGE("Key doesn't exist, size:%zu, index:%u", surfaceAndHolesMap_.size(), surfaceIndex);
    }
}

} // namespace Rosen
} // namespace OHOS