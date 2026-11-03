/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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


#ifndef SHADOW_UTIL_H
#define SHADOW_UTIL_H

namespace OHOS {
namespace Rosen {
namespace Drawing {

constexpr uint8_t SHADOW_FLAG_MASK = 0x0F;   // low 4 bits for ShadowFlags
constexpr uint8_t RECORD_CMD_BIT = 0x80;      // bit 7 for isRecordCmd

static inline uint8_t SetShadowIsRecordCmd(uint8_t shadowFlag, bool isRecordCmd)
{
    return isRecordCmd ? (shadowFlag | RECORD_CMD_BIT) : (shadowFlag & ~RECORD_CMD_BIT);
}

static inline bool GetShadowIsRecordCmd(const uint8_t shadowFlag)
{
    return (shadowFlag & RECORD_CMD_BIT);
}

static inline bool GetShadowIsRecordCmd(ShadowFlags shadowFlag)
{
    return GetShadowIsRecordCmd(static_cast<uint8_t>(shadowFlag));
}

static inline uint8_t SetShadowFlag(uint8_t shadowFlag)
{
    return (shadowFlag & RECORD_CMD_BIT) | (shadowFlag & SHADOW_FLAG_MASK);
}

static inline ShadowFlags GetShadowFlag(const uint8_t& shadowFlag)
{
    return static_cast<ShadowFlags>(shadowFlag & SHADOW_FLAG_MASK);
}

static inline ShadowFlags GetShadowFlag(ShadowFlags shadowFlag)
{
    return GetShadowFlag(static_cast<uint8_t>(shadowFlag));
}

} // namespace Drawing
} // namespace Rosen
} // OHOS

#endif // SHADOW_UTIL_H