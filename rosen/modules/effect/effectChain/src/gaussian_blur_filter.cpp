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
    downSampleFilter_ = new ScaleFilter();
    downSampleFilter_->SetScale(DOWNSAMPLE_FACTOR);
    upSampleFilter_ = new ScaleFilter();
    upSampleFilter_->SetScale(INVERSE_DOWNSAMPLE_FACTOR);
    horizontalBlurFilter_ = new HorizontalBlurFilter();
    verticalBlurFilter_ = new VerticalBlurFilter();
}

GaussianBlurFilter::~GaussianBlurFilter()
{
    delete downSampleFilter_;
    delete upSampleFilter_;
    delete horizontalBlurFilter_;
    delete verticalBlurFilter_;
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