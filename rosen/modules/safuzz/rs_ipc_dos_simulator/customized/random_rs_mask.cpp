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

#include "customized/random_rs_mask.h"

#include <memory>

#include "SkStream.h"
#include "modules/svg/include/SkSVGDOM.h"

#include "common/safuzz_log.h"
#include "common/rs_vector2.h"
#include "customized/random_pixel_map.h"
#include "customized/random_rs_path.h"
#include "draw/brush.h"
#include "draw/path.h"
#include "random/random_data.h"
#include "random/random_engine.h"
#include "render/rs_mask.h"
#include "utils/point.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSMask> RandomRSMask::GetRandomRSMask()
{
    static constexpr int MASK_TYPE_INDEX_MAX = 4;
    auto type = static_cast<MaskType>(RandomEngine::GetRandomIndex(MASK_TYPE_INDEX_MAX));
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<RSMask> mask = nullptr;
    std::shared_ptr<RSPath> path = nullptr;
    switch (type) {
        case MaskType::PATH: {
            path = RandomRSPath::GetRandomRSPath();
            if (path == nullptr) {
                SAFUZZ_LOGE("RandomRSMask::GetRandomRSMask path is nullptr");
                return nullptr;
            }
            return RSMask::CreatePathMask(path->GetDrawingPath(), brush);
        }
        case MaskType::PIXEL_MAP: {
            return RSMask::CreatePixelMapMask(RandomPixelMap::GetRandomPixelMap());
        }
        case MaskType::GRADIENT: {
            return RSMask::CreateGradientMask(brush);
        }
        case MaskType::SVG: {
            return RSMask::CreateSVGMask(RandomData::GetRandomFloat(), RandomData::GetRandomFloat(),
                RandomData::GetRandomFloat(), RandomData::GetRandomFloat(), nullptr);
        }
        default: {
            mask = std::make_shared<RSMask>();
            mask->SetMaskType(MaskType::NONE);
            mask->SetMaskBrush(brush);
            mask->SetScaleX(RandomData::GetRandomFloat());
            mask->SetScaleY(RandomData::GetRandomFloat());
            mask->SetSvgX(RandomData::GetRandomFloat());
            mask->SetSvgY(RandomData::GetRandomFloat());
            mask->SetMaskPen(pen);
            return mask;
        }
    }
}
} // namespace Rosen
} // namespace OHOS
