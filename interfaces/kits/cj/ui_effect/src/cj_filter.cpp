/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "cj_filter.h"
#include "cj_ui_effect_log.h"
#include "filter/include/filter_blur_para.h"
#include "filter/include/filter_displacement_distort_para.h"

namespace OHOS {
namespace Rosen {
using namespace UIEffect;
CJFilter::CJFilter()
{
    m_FilterObj = std::make_shared<Filter>();
}

int32_t CJFilter::SetBlur(float blur)
{
    std::shared_ptr<FilterBlurPara> para = std::make_shared<FilterBlurPara>();
    para->SetRadius(blur);
    m_FilterObj->AddPara(para);
    FILTER_LOG_D("CJFilter set blur success.");
    return CJ_OK;
}
} // namespace Rosen
} // namespace OHOS