/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "pipeline/main_thread/rs_surface_permission.h"
#include <ipc_skeleton.h>
#include <accesstoken_kit.h>
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::string MEDIA_SERVICE_PROCESS_NAME = "media_service";
}

bool RSSurfacePermission::CheckLppCaller()
{
    Security::AccessToken::AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    Security::AccessToken::AccessTokenID mediaServiceToken =
        Security::AccessToken::AccessTokenKit::GetNativeTokenId(MEDIA_SERVICE_PROCESS_NAME);
    if (tokenId != mediaServiceToken) {
        RS_LOGE("RSSurfacePermission::CheckLppCaller failed, calling process is not media_service");
        return false;
    }
    return true;
}

} // namespace Rosen
} // namespace OHOS