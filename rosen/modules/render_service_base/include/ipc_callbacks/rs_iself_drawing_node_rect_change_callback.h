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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK_H

#include <iremote_broker.h>
#include "transaction/rs_self_drawing_node_rect_data.h"

namespace OHOS {
namespace Rosen {
class RSISelfDrawingNodeRectChangeCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.selfdrawingnoderectchangecallback");
    RSISelfDrawingNodeRectChangeCallback() = default;
    virtual ~RSISelfDrawingNodeRectChangeCallback() noexcept = default;
    virtual void OnSelfDrawingNodeRectChange(std::shared_ptr<RSSelfDrawingNodeRectData> data) = 0;
};
}
}

#endif