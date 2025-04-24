/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "hwc_param.h"
#include "rs_uni_hwc_visitor.h"
#include "feature/hwc/rs_uni_hwc_compute_util.h"

namespace OHOS {
namespace Rosen {
RSUniHwcVisitor::RSUniHwcVisitor(RSUniRenderVisitor& visitor) : uniRenderVisitor_(visitor) {}
RSUniHwcVisitor::~RSUniHwcVisitor() {}

bool RSUniHwcVisitor::IsDisableHwcOnExpandScreen() const
{
    if (uniRenderVisitor_.curDisplayNode_ == nullptr) {
        RS_LOGE("curDisplayNode is null");
        return false;
    }

    // screenId > 0 means non-primary screen normally.
    return HWCParam::IsDisableHwcOnExpandScreen() &&
        uniRenderVisitor_.curDisplayNode_->GetScreenId() > 0;
}
} // namespace Rosen
} // namespace OHOS