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
#ifndef UIEFFECT_FILTER_MAP_COLOR_BY_BRIGHTNESS_PARA_H
#define UIEFFECT_FILTER_MAP_COLOR_BY_BRIGHTNESS_PARA_H

#include <vector>

#include "filter_para.h"

#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
class MapColorByBrightnessPara : public FilterPara {
public:
    MapColorByBrightnessPara()
    {
        type_ = FilterPara::ParaType::MAP_COLOR_BY_BRIGHTNESS;
    }
    ~MapColorByBrightnessPara() override = default;

    void SetColors(const std::vector<Vector4f>& colors)
    {
        colors_ = colors;
    }

    const std::vector<Vector4f>& GetColors() const
    {
        return colors_;
    }

    void SetPositions(const std::vector<float>& positions)
    {
        positions_ = positions;
    }

    const std::vector<float>& GetPositions() const
    {
        return positions_;
    }

private:
    std::vector<Vector4f> colors_;
    std::vector<float> positions_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_MAP_COLOR_BY_BRIGHTNESS_PARA_H
