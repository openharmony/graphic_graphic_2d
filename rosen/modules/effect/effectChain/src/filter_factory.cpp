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

#include "ec_log.h"
#include "saturation_filter.h"
#include "brightness_filter.h"
#include "contrast_filter.h"
#include "scale_filter.h"
#include "gaussian_blur_filter.h"
#include "input.h"
#include "output.h"
#include "filter_factory.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<Filter> FilterFactory::GetFilter(cJSON* type)
{
    std::string filterType = type->valuestring;
    if (!algoFilterSet_.count(filterType)) {
        LOGE("The filter type of %{public}s is not registered.", filterType.c_str());
        return nullptr;
    }

    if (filterType == "Input") {
        std::shared_ptr<Input> inputFilter = std::make_shared<Input>();
        LOGD("The filter type of %{public}s is created.", filterType.c_str());
        std::shared_ptr<Filter> filter = std::static_pointer_cast<Filter>(inputFilter);
        return filter;
    } else if (filterType == "Output") {
        std::shared_ptr<Output> outputFilter = std::make_shared<Output>();
        LOGD("The filter type of %{public}s is created.", filterType.c_str());
        std::shared_ptr<Filter> filter = std::static_pointer_cast<Filter>(outputFilter);
        return filter;
    } else if (filterType == "Saturation") {
        std::shared_ptr<SaturationFilter> saturationFilter = std::make_shared<SaturationFilter>();
        LOGD("The filter type of %{public}s is created.", filterType.c_str());
        std::shared_ptr<Filter> filter = std::static_pointer_cast<Filter>(saturationFilter);
        return filter;
    } else if (filterType == "Brightness") {
        std::shared_ptr<BrightnessFilter> brightnessFilter = std::make_shared<BrightnessFilter>();
        LOGD("The filter type of %{public}s is created.", filterType.c_str());
        std::shared_ptr<Filter> filter = std::static_pointer_cast<Filter>(brightnessFilter);
        return filter;
    } else if (filterType == "Contrast") {
        std::shared_ptr<ContrastFilter> contrastFilter = std::make_shared<ContrastFilter>();
        LOGD("The filter type of %{public}s is created.", filterType.c_str());
        std::shared_ptr<Filter> filter = std::static_pointer_cast<Filter>(contrastFilter);
        return filter;
    } else if (filterType == "GaussianBlur") {
        std::shared_ptr<GaussianBlurFilter> gaussianBlurFilter = std::make_shared<GaussianBlurFilter>();
        LOGD("The filter type of %{public}s is created.", filterType.c_str());
        std::shared_ptr<Filter> filter = std::static_pointer_cast<Filter>(gaussianBlurFilter);
        return filter;
    } else if (filterType == "HorizontalBlur") {
        std::shared_ptr<HorizontalBlurFilter> horizontalBlurFilter = std::make_shared<HorizontalBlurFilter>();
        LOGD("The filter type of %{public}s is created.", filterType.c_str());
        std::shared_ptr<Filter> filter = std::static_pointer_cast<Filter>(horizontalBlurFilter);
        return filter;
    } else if (filterType == "VerticalBlur") {
        std::shared_ptr<VerticalBlurFilter> verticalBlurFilter = std::make_shared<VerticalBlurFilter>();
        LOGD("The filter type of %{public}s is created.", filterType.c_str());
        std::shared_ptr<Filter> filter = std::static_pointer_cast<Filter>(verticalBlurFilter);
        return filter;
    } else if (filterType == "Scale") {
        std::shared_ptr<ScaleFilter> scaleFilter = std::make_shared<ScaleFilter>();
        LOGD("The filter type of %{public}s is created.", filterType.c_str());
        std::shared_ptr<Filter> filter = std::static_pointer_cast<Filter>(scaleFilter);
        return filter;
    }
    return nullptr;
}
} // namespcae Rosen
} // namespace OHOS
