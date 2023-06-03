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

#include "texgine_font.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::shared_ptr<SkFont> TexgineFont::GetFont() const
{
    return font_;
}

void TexgineFont::SetTypeface(const std::shared_ptr<TexgineTypeface> tf)
{
    if (tf) {
        font_->setTypeface(tf->GetTypeface());
    } else {
        font_->setTypeface(nullptr);
    }
}

void TexgineFont::SetSize(float textSize)
{
    font_->setSize(textSize);
}

float TexgineFont::GetMetrics(TexgineFontMetrics *metrics) const
{
    if (metrics == nullptr) {
        return 0;
    }

    return font_->getMetrics(metrics->GetFontMetrics().get());
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
