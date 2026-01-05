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

#ifndef RENDER_SERVICE_COMPOSER_CLIENT_CONNECTION_RS_COMPOSER_TO_RENDER_CONNECTION_H
#define RENDER_SERVICE_COMPOSER_CLIENT_CONNECTION_RS_COMPOSER_TO_RENDER_CONNECTION_H

#include "rs_composer_to_render_connection_stub.h"
#include "rs_render_composer_client.h"

namespace OHOS {
namespace Rosen {
class RSComposerToRenderConnection : public RSComposerToRenderConnectionStub {
public:
    RSComposerToRenderConnection() noexcept = default;
    ~RSComposerToRenderConnection() noexcept = default;
    int32_t ReleaseLayerBuffers(ReleaseLayerBuffersInfo& releaseLayerInfo) override;
    int32_t NotifyLppLayerToRender(uint64_t vsyncId, const std::set<uint64_t>& lppNodeIds) override;
    void RegisterReleaseLayerBuffersCB(ReleaseLayerBuffersCB callback) override;

private:
    ReleaseLayerBuffersCB releaseLayerBuffersCB_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_COMPOSER_CLIENT_CONNECTION_RS_COMPOSER_TO_RENDER_CONNECTION_H