/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_MASK_H
#define UIEFFECT_MASK_H
#include <memory>
#include "mask_para.h"

namespace OHOS {
namespace Rosen {
class Mask final {
public:
    Mask() = default;
    ~Mask() = default;

    void SetMaskPara(const std::shared_ptr<MaskPara>& para)
    {
        para_ = para;
    }

    const MaskPara::Type& GetMaskType() const
    {
        return para_ ? para_->GetMaskParaType() : MaskPara::Type::NONE;
    }

    std::shared_ptr<MaskPara> GetMaskPara() const
    {
        return para_;
    }

private:
    std::shared_ptr<MaskPara> para_ = std::make_shared<MaskPara>();
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_MASK_H
