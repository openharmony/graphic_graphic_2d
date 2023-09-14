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

#include "property/rs_property_drawable.h"

#include "property/rs_property_drawable_utilities.h"

namespace OHOS::Rosen {
RSPropertyDrawable::SaveRestorePair RSPropertyDrawable::GenerateSaveRestore(RSPaintFilterCanvas::SaveType type)
{
    if (type == RSPaintFilterCanvas::kNone) {
        return {};
    } else if (type == RSPaintFilterCanvas::kCanvas) {
        auto count = std::make_shared<int>(-1);
        return { std::make_unique<RSSaveDrawable>(count), std::make_unique<RSRestoreDrawable>(count) };
    } else {
        auto status = std::make_shared<RSPaintFilterCanvas::SaveStatus>();
        return { std::make_unique<RSCustomSaveDrawable>(status, type),
            std::make_unique<RSCustomRestoreDrawable>(status) };
    }
}

} // namespace OHOS::Rosen
