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

#include "rs_self_drawing_node_rect_change_callback_stub.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
int RSSelfDrawingNodeRectChangeCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSISelfDrawingNodeRectChangeCallback::GetDescriptor()) {
        ROSEN_LOGE("RSISelfDrawingNodeRectChangeCallback::OnRemoteRequest WriteInterfaceToken failed");
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(
            RSISelfDrawingNodeRectChangeCallbackInterfaceCode::ON_SELF_DRAWING_NODE_RECT_CHANGED): {
            std::shared_ptr<RSSelfDrawingNodeRectData> rectData(data.ReadParcelable<RSSelfDrawingNodeRectData>());
            OnSelfDrawingNodeRectChange(rectData);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }

    return ret;
}
} // namespace Rosen
} // namespace OHOS