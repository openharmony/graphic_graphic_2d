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

#include "tools/token_utils.h"

#include "accesstoken_kit.h"
#ifdef SUPPORT_ACCESS_TOKEN
#include "nativetoken_kit.h"
#include "token_setproc.h"
#endif

#include "common/safuzz_log.h"
#include "configs/safuzz_config.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t PERM_NUM = 2;
const char* PERMS[PERM_NUM] = { "ohos.permission.DISTRIBUTED_DATASYNC", "ohos.permission.DISTRIBUTED_SOFTBUS_CENTER" };
const char* const APL_STR = "system_basic";
}

bool TokenUtils::ConfigureTokenInfo()
{
#ifdef SUPPORT_ACCESS_TOKEN
    NativeTokenInfoParams params = {
        .dcapsNum = 0,
        .permsNum = PERM_NUM,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = PERMS,
        .acls = nullptr,
        .processName = SAFUZZ_PROCESS_NAME,
        .aplStr = APL_STR,
    };
    uint64_t tokenId = GetAccessTokenId(&params);
    int accessTokenRet = SetSelfTokenID(tokenId);
    if (accessTokenRet != ACCESS_TOKEN_OK) {
        SAFUZZ_LOGE("TokenUtils::ConfigureTokenInfo SetSelfTokenID failed, ret is %d", accessTokenRet);
        return false;
    }
    int32_t accessTokenKitRet = OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    if (accessTokenKitRet != OHOS::Security::AccessToken::AccessTokenKitRet::RET_SUCCESS) {
        SAFUZZ_LOGE("TokenUtils::ConfigureTokenInfo ReloadNativeTokenInfo failed, ret is %" PRId32, accessTokenKitRet);
        return false;
    }
    return true;
#else
    return false;
#endif
}
} // namespace Rosen
} // namespace OHOS
