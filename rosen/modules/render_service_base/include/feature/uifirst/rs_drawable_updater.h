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

#ifndef RS_DRAWABLE_UPDATER_H
#define RS_DRAWABLE_UPDATER_H

#include <limits>
#include "drawable/rs_drawable.h"
#include "drawable/rs_misc_drawable.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

template <typename U>
class RSDrawableHelper : public RSDrawable {
public:
    RSDrawableHelper() = default;
    ~RSDrawableHelper() override = default;
    std::shared_ptr<U> content_;
    RSPaintFilterCanvas::SaveType type_;
};

template <typename T, typename U>
static inline bool UpdateRSDrawable(std::shared_ptr<RSDrawable>& drawable, std::shared_ptr<U>& content)
{
    if constexpr (std::is_constructible<T, std::shared_ptr<U>>::value) {
        static const T BENCHMARK(nullptr);
        if (*(reinterpret_cast<const uintptr_t*>(&BENCHMARK)) !=
            *(reinterpret_cast<const uintptr_t*>(drawable.get()))) {
            return false;
        }
        drawable = std::make_shared<T>(content);
    } else {
        static const T BENCHMARK(nullptr, RSPaintFilterCanvas::SaveType::kNone);
        if (*(reinterpret_cast<const uintptr_t*>(&BENCHMARK)) !=
            *(reinterpret_cast<const uintptr_t*>(drawable.get()))) {
            return false;
        }
        auto type = reinterpret_cast<RSDrawableHelper<U>*>(drawable.get())->type_;
        drawable = std::make_shared<T>(content, type);
    }
    return true;
}

static void UpdateSaveRestoreDrawable(RSDrawable::DrawList& drawCmdList)
{
    int updated = 0;
    auto count = std::make_shared<uint32_t>(std::numeric_limits<uint32_t>::max());
    auto status = std::make_shared<RSPaintFilterCanvas::SaveStatus>();
    for (auto& drawable : drawCmdList) {
        if (drawable) {
            updated += (UpdateRSDrawable<DrawableV2::RSSaveDrawable>(drawable, count) ||
                        UpdateRSDrawable<DrawableV2::RSRestoreDrawable>(drawable, count) ||
                        UpdateRSDrawable<DrawableV2::RSCustomSaveDrawable>(drawable, status) ||
                        UpdateRSDrawable<DrawableV2::RSCustomRestoreDrawable>(drawable, status));
        }
    }
    RS_TRACE_NAME_FMT("UpdateSaveRestoreDrawable updated[%d]", updated);
}

} // namespace Rosen
} // namespace OHOS

#endif // RS_DRAWABLE_UPDATER_H