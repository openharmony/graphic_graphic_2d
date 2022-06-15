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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_OCCLUSION_CHANGE_CALLBACK_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_OCCLUSION_CHANGE_CALLBACK_H

#include "transaction/rs_occlusion_data.h"

namespace OHOS {
namespace Rosen {
class RSIOcclusionChangeCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.occlusionchangecallback");
    RSIOcclusionChangeCallback() = default;
    virtual ~RSIOcclusionChangeCallback() noexcept = default;
    virtual void OnOcclusionVisibleChanged(std::shared_ptr<RSOcclusionData> occlusionData) = 0;

    enum {
        ON_OCCLUSION_VISIBLE_CHANGED,
    };
};
}
}

#endif