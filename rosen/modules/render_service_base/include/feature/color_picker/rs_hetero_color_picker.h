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

#ifndef RS_HETERO_COLOR_PICKER_H
#define RS_HETERO_COLOR_PICKER_H

#include "feature/color_picker/rs_color_picker_manager.h"
#include "rs_trace.h"

#include "draw/canvas.h"
#include "draw/color.h"
#include "image/image.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
class RSHeteroColorPicker {
public:
    static RSHeteroColorPicker& Instance();
    RSHeteroColorPicker() = default;
    ~RSHeteroColorPicker() = default;

    bool GetColor(const std::function<void(Drawing::ColorQuad&)>& updateColor, RSPaintFilterCanvas& canvas,
        std::shared_ptr<Drawing::Image>& image);

private:
    bool ValidateInputs(const std::function<void(Drawing::ColorQuad&)>& updateColor,
        Drawing::Surface* surface, const std::shared_ptr<Drawing::Image>& image);
};

} // namespace Rosen
} // namespace OHOS
#endif // RS_HETERO_COLOR_PICKER_H