/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gaussian_blur_filter.h"

namespace OHOS {
namespace Rosen {
GaussianBlurFilter::GaussianBlurFilter()
{
    // Allocate the filters
    upSampleFilter_ = std::make_unique<ScaleFilter>();
    upSampleFilter_->SetScale(INVERSE_DOWNSAMPLE_FACTOR);
    downSampleFilter_ = std::make_unique<ScaleFilter>();
    downSampleFilter_->SetScale(DOWNSAMPLE_FACTOR);
    horizontalBlurFilter_ = std::make_unique<HorizontalBlurFilter>();
    verticalBlurFilter_ = std::make_unique<VerticalBlurFilter>();
}

GaussianBlurFilter::~GaussianBlurFilter()
{
    downSampleFilter_ = nullptr;
    upSampleFilter_ = nullptr;
    horizontalBlurFilter_ = nullptr;
    verticalBlurFilter_ = nullptr;
}

void GaussianBlurFilter::DoProcess(ProcessData& data)
{
    downSampleFilter_->Process(data);
    horizontalBlurFilter_->Process(data);
    verticalBlurFilter_->Process(data);
    upSampleFilter_->Process(data);
}

void GaussianBlurFilter::SetValue(const std::string& key, std::shared_ptr<void> value, int size)
{
    horizontalBlurFilter_->SetValue(key, value, size);
    verticalBlurFilter_->SetValue(key, value, size);
}

std::string GaussianBlurFilter::GetVertexShader()
{
    return std::string();
}

std::string GaussianBlurFilter::GetFragmentShader()
{
    return std::string();
}
} // namespcae Rosen
} // namespace OHOS