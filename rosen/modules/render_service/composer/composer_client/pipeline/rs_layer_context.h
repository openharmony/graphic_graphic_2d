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

#ifndef RENDER_SERVICE_COMPOSER_CLIENT_PIPELINE_RS_LAYER_CONTEXT_H
#define RENDER_SERVICE_COMPOSER_CLIENT_PIPELINE_RS_LAYER_CONTEXT_H

#include <functional>
#include <memory>
#include <mutex>

#include "rs_layer.h"
#include "rs_layer_transaction_handler.h"
#include "rs_render_to_composer_connection.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT RSLayerContext : public std::enable_shared_from_this<RSLayerContext> {
public:
    RSLayerContext();
    RSLayerContext(RSLayerContext&) = delete;
    RSLayerContext(RSLayerContext&&) = delete;
    RSLayerContext& operator=(RSLayerContext&) = delete;
    RSLayerContext& operator=(RSLayerContext&&) = delete;
    virtual ~RSLayerContext() = default;

    void SetRenderComposerClientConnection(const sptr<RSRenderToComposerConnection>& conn);
    void AddLayer(const std::shared_ptr<RSLayer> rsLayer);
    void ClearAllLayers();
    void CommitLayers();

private:
    std::shared_ptr<RSLayerTransactionHandler> GetRSLayerTransaction() const;
    std::shared_ptr<RSLayerTransactionHandler> rsLayerTransactionHandler_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_CLIENT_PIPELINE_RS_LAYER_CONTEXT_H