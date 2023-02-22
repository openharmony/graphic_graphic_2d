/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_FUNCTIONAL_DELEGATE_H
#define ROSEN_RENDER_SERVICE_BASE_RS_FUNCTIONAL_DELEGATE_H

#include <functional>
#include <memory>

#include "common/rs_common_def.h"
#include "delegate/rs_delegate.h"
#include "common/rs_macros.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSFunctionalDelegate : public RSDelegate {
public:
    static std::shared_ptr<RSFunctionalDelegate> Create()
    {
        return std::make_shared<RSFunctionalDelegate>();
    }

    void SetRepaintCallback(std::function<void()> repaintCallback);
    void Repaint() override;

private:
    std::function<void()> repaintCallback_ = nullptr;
};
} // namespace OHOS::Rosen

#endif // ROSEN_RENDER_SERVICE_BASE_RS_FUNCTIONAL_DELEGATE_H
