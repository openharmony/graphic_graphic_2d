/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rs_profiler_packet.h"

namespace OHOS::Rosen {

Packet::Packet(PacketType type)
{
    InitData(type);
}

bool Packet::IsBinary() const
{
    return (GetType() == BINARY);
}

bool Packet::IsCommand() const
{
    return (GetType() == COMMAND_ACKNOWLEDGED) || (GetType() == COMMAND);
}

char* Packet::Begin()
{
    return data_.data();
}

char* Packet::End()
{
    return data_.data() + data_.size();
}

Packet::PacketType Packet::GetType() const
{
    return *GetHeaderFieldPtr<PacketType, Header::TYPE>();
}

void Packet::SetType(Packet::PacketType type)
{
    *GetHeaderFieldPtr<PacketType, Header::TYPE>() = type;
}

uint32_t Packet::GetLength() const
{
    return *GetHeaderFieldPtr<uint32_t, Header::LENGTH>();
}

uint32_t Packet::GetPayloadLength() const
{
    return GetLength() - Packet::HEADER_SIZE;
}

std::vector<char> Packet::Release()
{
    auto res = std::move(data_);
    InitData(UNKNOWN);
    return res;
}

void Packet::SetLength(uint32_t length)
{
    *GetHeaderFieldPtr<uint32_t, Header::LENGTH>() = length;
}

void Packet::InitData(PacketType type)
{
    data_.resize(HEADER_SIZE);
    SetType(type);
    SetLength(HEADER_SIZE);
    readPointer_ = HEADER_SIZE;
    writePointer_ = HEADER_SIZE;
}

} // namespace OHOS::Rosen