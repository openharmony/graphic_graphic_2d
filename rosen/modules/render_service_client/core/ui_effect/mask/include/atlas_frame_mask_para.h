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
#ifndef UIEFFECT_ATLAS_FRAME_MASK_PARA_H
#define UIEFFECT_ATLAS_FRAME_MASK_PARA_H

#include <algorithm>

#include "common/rs_atlas_info.h"
#include "common/rs_macros.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/utils.h"

namespace OHOS {
namespace Rosen {
// mode is an enum (0=NONE, 1=INTERPOLATED), stored as int32_t, clamped to [0, 1]
constexpr int32_t ATLAS_FRAME_MASK_MODE_MIN = 0;
constexpr int32_t ATLAS_FRAME_MASK_MODE_MAX = 1;
constexpr int32_t ATLAS_FRAME_MASK_ROWS_MIN = 1;
constexpr int32_t ATLAS_FRAME_MASK_COLS_MIN = 1;
constexpr std::pair<float, float> ATLAS_FRAME_MASK_FRAME_WIDTH_LIMITS { 1.0f, 8192.0f };
constexpr std::pair<float, float> ATLAS_FRAME_MASK_FRAME_HEIGHT_LIMITS { 1.0f, 8192.0f };
constexpr std::pair<float, float> ATLAS_FRAME_MASK_PADDING_LIMITS { 0.0f, 64.0f };
constexpr int32_t ATLAS_FRAME_MASK_TOTAL_FRAME_MIN = 1;

class RSC_EXPORT AtlasFrameMaskPara : public MaskPara {
public:
    AtlasFrameMaskPara()
    {
        type_ = MaskPara::Type::ATLAS_FRAME_MASK;
    }
    ~AtlasFrameMaskPara() override = default;

    AtlasFrameMaskPara(const AtlasFrameMaskPara& other);

    void SetAtlasInfo(const AtlasInfo& atlasInfo)
    {
        atlasInfo_ = atlasInfo;
        atlasInfo_.mode = std::clamp(atlasInfo_.mode, ATLAS_FRAME_MASK_MODE_MIN, ATLAS_FRAME_MASK_MODE_MAX);
        atlasInfo_.rows = std::max(atlasInfo_.rows, ATLAS_FRAME_MASK_ROWS_MIN);
        atlasInfo_.cols = std::max(atlasInfo_.cols, ATLAS_FRAME_MASK_COLS_MIN);
        atlasInfo_.totalFrame = std::clamp(atlasInfo_.totalFrame,
            ATLAS_FRAME_MASK_TOTAL_FRAME_MIN, atlasInfo_.rows * atlasInfo_.cols);
        atlasInfo_.rows = std::min(atlasInfo_.rows, atlasInfo_.totalFrame);
        atlasInfo_.cols = std::min(atlasInfo_.cols, atlasInfo_.totalFrame);
        atlasInfo_.frameWidth = UIEffect::GetLimitedPara(atlasInfo_.frameWidth, ATLAS_FRAME_MASK_FRAME_WIDTH_LIMITS);
        atlasInfo_.frameHeight = UIEffect::GetLimitedPara(atlasInfo_.frameHeight, ATLAS_FRAME_MASK_FRAME_HEIGHT_LIMITS);
        atlasInfo_.padding = UIEffect::GetLimitedPara(atlasInfo_.padding, ATLAS_FRAME_MASK_PADDING_LIMITS);
        atlasInfo_.frameIndex = std::clamp(atlasInfo_.frameIndex, 0.0f,
            static_cast<float>(atlasInfo_.totalFrame - 1));
    }

    const AtlasInfo& GetAtlasInfo() const
    {
        return atlasInfo_;
    }

    bool Marshalling(Parcel& parcel) const override;

    static void RegisterUnmarshallingCallback();

    [[nodiscard]] static bool OnUnmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val);

    std::shared_ptr<MaskPara> Clone() const override;

private:
    AtlasInfo atlasInfo_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_ATLAS_FRAME_MASK_PARA_H
