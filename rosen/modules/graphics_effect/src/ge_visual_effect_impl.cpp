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

std::map<const std::string, std::function<void(GEVisualEffectImpl*)>> GEVisualEffectImpl::g_initialMap = {
    { "KWASE_BLUR",
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::KAWASE_BLUR);
            impl->MakeKawaseParams();
        } },
    { "GREY",
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::GREY);
            impl->MakeGreyParams();
        } },
    { "AIBAR",
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::AIBAR);
            impl->MakeAIBarParams();
        } }
};

GEVisualEffectImpl::GEVisualEffectImpl(const std::string& name)
{
    auto iter = g_initialMap.find(name);
    if (iter != g_initialMap.end()) {
        iter->second(this);
    }
}

GEVisualEffectImpl::~GEVisualEffectImpl() {}

void GEVisualEffectImpl::SetParam(const std::string& tag, int32_t param)
{
    switch (filterType_) {
        case FilterType::KAWASE_BLUR: {
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, int64_t param)
{
    switch (filterType_) {
        case FilterType::KAWASE_BLUR: {
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, float param)
{
    switch (filterType_) {
        case FilterType::AIBAR: {
            if (aiBarParams_ == nullptr) {
                return;
            }

            if (tag == "AIBAR_LOW") {
                aiBarParams_->aiBarLow = param;
            }

            if (tag == "AIBAR_HIGH") {
                aiBarParams_->aiBarHigh = param;
            }

            if (tag == "AIBAR_THRESHOLD") {
                aiBarParams_->aiBarThreshold = param;
            }

            if (tag == "AIBAR_OPACITY") {
                aiBarParams_->aiBarOpacity = param;
            }

            if (tag == "AIBAR_SATURATION") {
                aiBarParams_->aiBarSaturation = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, double param) {}

void GEVisualEffectImpl::SetParam(const std::string& tag, const char* const param) {}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
