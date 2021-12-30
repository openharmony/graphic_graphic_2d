/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <display_type.h>

#include "command/rs_base_node_command.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"

using namespace OHOS;
using namespace OHOS::Rosen;

int main()
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(1, 1, 1);
    auto renderClient = RSIRenderClient::CreateRenderServiceClient();
    RSTransactionProxy::GetInstance().SetRenderServiceClient(renderClient);
    RSTransactionProxy::GetInstance().AddCommand(command);
    RSTransactionProxy::GetInstance().FlushImplicitTransaction();
    // create surface
    RSSurfaceRenderNodeConfig config = {.id=0};
    std::shared_ptr<RSSurface> testSurface =
        std::static_pointer_cast<RSRenderServiceClient>(renderClient)->CreateNodeAndSurface(config);
    auto frame = testSurface->RequestFrame(1600, 1600);
    frame->SetDamageRegion(0, 0, 1600, 1600);
    testSurface->FlushFrame(frame);
    while(1);
    return 0;
}
