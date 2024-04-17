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

#include "ge_visual_effect.h"

#include "ge_log.h"
#include "ge_visual_effect_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

GEVisualEffect::GEVisualEffect(const std::string& name, DrawingPaintType type)
    : visualEffectName_(name), type_(type), visualEffectImpl_(std::make_unique<GEVisualEffectImpl>(name))
{}

GEVisualEffect::~GEVisualEffect() {}

void GEVisualEffect::SetParam(const std::string& tag, int32_t param)
{
    (void)type_;
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, int64_t param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, float param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, double param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const char* const param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const Drawing::Matrix param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, const std::vector<std::pair<float, float>> param)
{
    visualEffectImpl_->SetParam(tag, param);
}

void GEVisualEffect::SetParam(const std::string& tag, bool param)
{
    visualEffectImpl_->SetParam(tag, param);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
