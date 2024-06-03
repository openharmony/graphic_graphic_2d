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
#include <set>
namespace OHOS::Rosen {

Packet::Packet(PacketType type, uint32_t reserve)
{
    data_.reserve(reserve);
    InitData(type);
}

bool Packet::IsBinary() const
{
    return (GetType() == BINARY);
}

bool Packet::IsCommand() const
{
    return GetType() == COMMAND;
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
    return static_cast<PacketType>(*reinterpret_cast<const uint8_t*>(data_.data() + HEADER_TYPE_OFFSET));
}

void Packet::SetType(Packet::PacketType type)
{
    *reinterpret_cast<uint8_t*>(data_.data() + HEADER_TYPE_OFFSET) = static_cast<uint8_t>(type);
}

uint32_t Packet::GetLength() const
{
    uint32_t length = 0;
    Utils::Move(&length, sizeof(length), data_.data() + HEADER_LENGTH_OFFSET, sizeof(length));
    return length;
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
    Utils::Move(data_.data() + HEADER_LENGTH_OFFSET, sizeof(length), &length, sizeof(length));
}

void Packet::InitData(PacketType type)
{
    data_.resize(HEADER_SIZE);
    SetType(type);
    SetLength(HEADER_SIZE);
    readPointer_ = HEADER_SIZE;
    writePointer_ = HEADER_SIZE;
}

[[maybe_unused]] bool Packet::Read(void* value, size_t size)
{
    if (!size) {
        return false;
    }
    if (readPointer_ + size > data_.size()) {
        return false;
    }
    if (!Utils::Move(value, size, data_.data() + readPointer_, size) != 0) {
        return false;
    }
    readPointer_ += size;
    return true;
}

[[maybe_unused]] bool Packet::Write(const void* value, size_t size)
{
    if (!size) {
        return false;
    }
    const size_t growSize = size - (data_.size() - writePointer_);
    if (writePointer_ + size > data_.size()) {
        data_.resize(data_.size() + growSize);
    }
    if (!Utils::Move(data_.data() + writePointer_, data_.size() - writePointer_, value, size)) {
        data_.resize(data_.size() - growSize);
        return false;
    }
    writePointer_ += size;
    SetLength(data_.size());
    return true;
}

} // namespace OHOS::Rosen