/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "ge_visual_effect_impl.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

std::map<const std::string, std::function<void(GEVisualEffectImpl*)>> GEVisualEffectImpl::g_initialMap = {};

GEVisualEffectImpl::GEVisualEffectImpl(const std::string &name)
{
    auto iter = g_initialMap.find(name);
    if (iter != g_initialMap.end()) {
    }
    if (name == "KAWASE_BLUR") {
        filterType_ = FilterType::KAWASE_BLUR;
        kawaseParams_ = std::make_shared<GEKawaseBlurShaderFilterParams>();
    } else if (name == "GREY") {
        filterType_ = FilterType::GREY;
        greyParams_ = std::make_shared<GEGreyShaderFilterParams>();
    } else if (name == "AIBAR") {
        filterType_ = FilterType::AIBAR;
        aiBarParams_ = std::make_shared<GEAIBarShaderFilterParams>();
    } else if (name == "CHAINED_FILTER") {
        filterType_ = FilterType::CHAINED_FILTER;
    } else {
        filterType_ = GEVisualEffectImpl::FilterType::NONE;
    }
    filterVec_.clear();
}

GEVisualEffectImpl::~GEVisualEffectImpl()
{
    filterVec_.clear();
}

void GEVisualEffectImpl::SetParam(const std::string &tag, int32_t param)
{
    switch (filterType_) {
        case FilterType::KAWASE_BLUR: {
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string &tag, float param)
{
    switch (filterType_) {
        case FilterType::AIBAR: {
            if (aiBarParams_ == nullptr) {
                return;
            }

            break;
        }
        default:
            break;
    }
}

}  // namespace Drawing
}  // namespace Rosen
}  // namespace OHOS