/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "parcel.h"
#include <climits>
#include "securec.h"
#include "utils_log.h"

namespace OHOS {

Parcelable::Parcelable() : Parcelable(false)
{}

Parcelable::Parcelable(bool asRemote)
{
}

Parcel::Parcel(Allocator *allocator)
{
}

Parcel::Parcel() : Parcel(new DefaultAllocator())
{}

Parcel::~Parcel()
{
}

size_t Parcel::GetWritableBytes() const
{
    return {};
}

size_t Parcel::GetReadableBytes() const
{
    return {};
}

size_t Parcel::CalcNewCapacity(size_t minNewCapacity)
{
    return {};
}

bool Parcel::EnsureWritableCapacity(size_t desireCapacity)
{
    return {};
}

size_t Parcel::GetDataSize() const
{
    return {};
}

uintptr_t Parcel::GetData() const
{
    return {};
}

binder_size_t Parcel::GetObjectOffsets() const
{
    return {};
}

size_t Parcel::GetOffsetsSize() const
{
    return {};
}

size_t Parcel::GetDataCapacity() const
{
    return {};
}

size_t Parcel::GetMaxCapacity() const
{
    return {};
}

bool Parcel::SetMaxCapacity(size_t maxCapacity)
{
    return {};
}

bool Parcel::SetAllocator(Allocator *allocator)
{
    return {};
}

bool Parcel::CheckOffsets()
{
    return {};
}

void Parcel::InjectOffsets(binder_size_t offsets, size_t offsetSize)
{
}

void Parcel::FlushBuffer()
{
}

bool Parcel::SetDataCapacity(size_t newCapacity)
{
    return {};
}

bool Parcel::SetDataSize(size_t dataSize)
{
    return {};
}

bool Parcel::WriteDataBytes(const void *data, size_t size)
{
    return {};
}

void Parcel::WritePadBytes(size_t padSize)
{
}

bool Parcel::WriteBuffer(const void *data, size_t size)
{
    return {};
}

bool Parcel::WriteBufferAddTerminator(const void *data, size_t size, size_t typeSize)
{
    return {};
}

bool Parcel::WriteUnpadBuffer(const void *data, size_t size)
{
    return {};
}

template <typename T>
bool Parcel::Write(T value)
{
    return {};
}

bool Parcel::WriteBool(bool value)
{
    return Write<int32_t>(static_cast<int32_t>(value));
}

bool Parcel::WriteBoolUnaligned(bool value)
{
    return Write<bool>(value);
}

bool Parcel::WriteInt8(int8_t value)
{
    return Write<int32_t>(static_cast<int32_t>(value));
}

bool Parcel::WriteInt8Unaligned(int8_t value)
{
    return Write<int8_t>(value);
}

bool Parcel::WriteInt16(int16_t value)
{
    return Write<int32_t>(static_cast<int32_t>(value));
}

bool Parcel::WriteInt16Unaligned(int16_t value)
{
    return Write<int16_t>(value);
}

bool Parcel::WriteInt32(int32_t value)
{
    return Write<int32_t>(value);
}

bool Parcel::WriteInt64(int64_t value)
{
    return Write<int64_t>(value);
}

bool Parcel::WriteUint8(uint8_t value)
{
    return Write<uint32_t>(static_cast<uint32_t>(value));
}

bool Parcel::WriteUint8Unaligned(uint8_t value)
{
    return Write<uint8_t>(value);
}

bool Parcel::WriteUint16(uint16_t value)
{
    return Write<uint32_t>(static_cast<uint32_t>(value));
}

bool Parcel::WriteUint16Unaligned(uint16_t value)
{
    return Write<uint16_t>(value);
}

bool Parcel::WriteUint32(uint32_t value)
{
    return Write<uint32_t>(value);
}

bool Parcel::WriteUint64(uint64_t value)
{
    return Write<uint64_t>(value);
}

bool Parcel::WriteFloat(float value)
{
    return Write<float>(value);
}

bool Parcel::WriteDouble(double value)
{
    return Write<double>(value);
}

bool Parcel::WritePointer(uintptr_t value)
{
    return Write<binder_uintptr_t>(value);
}

bool Parcel::WriteCString(const char *value)
{
    return {};
}

bool Parcel::WriteString(const std::string &value)
{
    return {};
}

bool Parcel::WriteString16(const std::u16string &value)
{
    return {};
}

bool Parcel::WriteString16WithLength(const char16_t *value, size_t len)
{
    return {};
}

bool Parcel::WriteString8WithLength(const char *value, size_t len)
{
    return {};
}

bool Parcel::EnsureObjectsCapacity()
{
    return {};
}

bool Parcel::WriteObjectOffset(binder_size_t offset)
{
    return {};
}

bool Parcel::WriteRemoteObject(const Parcelable *object)
{
    return {};
}

bool Parcel::WriteParcelable(const Parcelable *object)
{
    return {};
}

bool Parcel::WriteStrongParcelable(const sptr<Parcelable> &object)
{
    return {};
}

template <typename T>
bool Parcel::Read(T &value)
{
    return {};
}

template <typename T>
T Parcel::Read()
{
    return {};
}

bool Parcel::ParseFrom(uintptr_t data, size_t size)
{
    return {};
}

const uint8_t *Parcel::ReadBuffer(size_t length)
{
    return {};
}

const uint8_t *Parcel::ReadUnpadBuffer(size_t length)
{
    return {};
}

void Parcel::SkipBytes(size_t bytes)
{
}

size_t Parcel::GetReadPosition()
{
    return {};
}

bool Parcel::RewindRead(size_t newPosition)
{
    return {};
}

size_t Parcel::GetWritePosition()
{
    return {};
}

bool Parcel::RewindWrite(size_t newPosition)
{
    return {};
}

bool Parcel::ReadBool()
{
    int32_t temp = Read<int32_t>();
    return (temp != 0);
}

bool Parcel::ReadBoolUnaligned()
{
    return Read<bool>();
}

int8_t Parcel::ReadInt8()
{
    int32_t temp = Read<int32_t>();
    return static_cast<int8_t>(temp);
}

int16_t Parcel::ReadInt16()
{
    int32_t temp = Read<int32_t>();
    return static_cast<int16_t>(temp);
}

int32_t Parcel::ReadInt32()
{
    return Read<int32_t>();
}

int64_t Parcel::ReadInt64()
{
    return Read<int64_t>();
}

uint8_t Parcel::ReadUint8()
{
    uint32_t temp = Read<uint32_t>();
    return static_cast<uint8_t>(temp);
}

uint16_t Parcel::ReadUint16()
{
    uint32_t temp = Read<uint32_t>();
    return static_cast<uint16_t>(temp);
}

uint32_t Parcel::ReadUint32()
{
    return Read<uint32_t>();
}

uint64_t Parcel::ReadUint64()
{
    return Read<uint64_t>();
}

float Parcel::ReadFloat()
{
    return Read<float>();
}

double Parcel::ReadDouble()
{
    return Read<double>();
}

template <typename T>
bool Parcel::ReadPadded(T &value)
{
    return {};
}

bool Parcel::ReadBool(bool &value)
{
    return ReadPadded<bool>(value);
}

bool Parcel::ReadInt8(int8_t &value)
{
    return ReadPadded<int8_t>(value);
}

bool Parcel::ReadInt8Unaligned(int8_t &value)
{
    return Read<int8_t>(value);
}

bool Parcel::ReadInt16(int16_t &value)
{
    return ReadPadded<int16_t>(value);
}

bool Parcel::ReadInt16Unaligned(int16_t &value)
{
    return Read<int16_t>(value);
}

bool Parcel::ReadInt32(int32_t &value)
{
    return Read<int32_t>(value);
}

bool Parcel::ReadInt64(int64_t &value)
{
    return Read<int64_t>(value);
}

bool Parcel::ReadUint8(uint8_t &value)
{
    return ReadPadded<uint8_t>(value);
}

bool Parcel::ReadUint8Unaligned(uint8_t &value)
{
    return Read<uint8_t>(value);
}

bool Parcel::ReadUint16(uint16_t &value)
{
    return ReadPadded<uint16_t>(value);
}

bool Parcel::ReadUint16Unaligned(uint16_t &value)
{
    return Read<uint16_t>(value);
}

bool Parcel::ReadUint32(uint32_t &value)
{
    return Read<uint32_t>(value);
}

bool Parcel::ReadUint64(uint64_t &value)
{
    return Read<uint64_t>(value);
}

bool Parcel::ReadFloat(float &value)
{
    return Read<float>(value);
}

bool Parcel::ReadDouble(double &value)
{
    return Read<double>(value);
}

uintptr_t Parcel::ReadPointer()
{
    return Read<binder_uintptr_t>();
}

const char *Parcel::ReadCString()
{
    return {};
}

const std::string Parcel::ReadString()
{
    return {};
}

bool Parcel::ReadString(std::string &value)
{
    return {};
}

const std::u16string Parcel::ReadString16()
{
    return {};
}

bool Parcel::ReadString16(std::u16string &value)
{
    return {};
}

const std::u16string Parcel::ReadString16WithLength(int32_t &readLength)
{
    return {};
}

const std::string Parcel::ReadString8WithLength(int32_t &readLength)
{
    return {};
}

void *DefaultAllocator::Alloc(size_t size)
{
    return nullptr;
}

void DefaultAllocator::Dealloc(void *data)
{
    if (data != nullptr) {
        free(data);
    }
}

void *DefaultAllocator::Realloc(void *data, size_t newSize)
{
    return nullptr;
}

template <typename T1, typename T2>
bool Parcel::WriteVector(const std::vector<T1> &val, bool (Parcel::*Write)(T2))
{
    return {};
}

bool Parcel::WriteBoolVector(const std::vector<bool> &val)
{
    return WriteVector(val, &Parcel::WriteBool);
}

bool Parcel::WriteInt8Vector(const std::vector<int8_t> &val)
{
    return WriteVector(val, &Parcel::WriteInt8Unaligned);
}

bool Parcel::WriteInt16Vector(const std::vector<int16_t> &val)
{
    return WriteVector(val, &Parcel::WriteInt16);
}

bool Parcel::WriteInt32Vector(const std::vector<int32_t> &val)
{
    return WriteVector(val, &Parcel::WriteInt32);
}

bool Parcel::WriteInt64Vector(const std::vector<int64_t> &val)
{
    return WriteVector(val, &Parcel::WriteInt64);
}

bool Parcel::WriteUInt8Vector(const std::vector<uint8_t> &val)
{
    return WriteVector(val, &Parcel::WriteUint8Unaligned);
}

bool Parcel::WriteUInt16Vector(const std::vector<uint16_t> &val)
{
    return WriteVector(val, &Parcel::WriteUint16Unaligned);
}

bool Parcel::WriteUInt32Vector(const std::vector<uint32_t> &val)
{
    return WriteVector(val, &Parcel::WriteUint32);
}

bool Parcel::WriteUInt64Vector(const std::vector<uint64_t> &val)
{
    return WriteVector(val, &Parcel::WriteUint64);
}

bool Parcel::WriteFloatVector(const std::vector<float> &val)
{
    return WriteVector(val, &Parcel::WriteFloat);
}

bool Parcel::WriteDoubleVector(const std::vector<double> &val)
{
    return WriteVector(val, &Parcel::WriteDouble);
}

bool Parcel::WriteStringVector(const std::vector<std::string> &val)
{
    return WriteVector(val, &Parcel::WriteString);
}

bool Parcel::WriteString16Vector(const std::vector<std::u16string> &val)
{
    return WriteVector(val, &Parcel::WriteString16);
}

template <typename T>
bool Parcel::ReadVector(std::vector<T> *val, bool (Parcel::*Read)(T &))
{
    return {};
}

bool Parcel::ReadBoolVector(std::vector<bool> *val)
{
    return {};
}

bool Parcel::ReadInt8Vector(std::vector<int8_t> *val)
{
    return ReadVector(val, &Parcel::ReadInt8Unaligned);
}

bool Parcel::ReadInt16Vector(std::vector<int16_t> *val)
{
    return ReadVector(val, &Parcel::ReadInt16);
}

bool Parcel::ReadInt32Vector(std::vector<int32_t> *val)
{
    return ReadVector(val, &Parcel::ReadInt32);
}

bool Parcel::ReadInt64Vector(std::vector<int64_t> *val)
{
    return ReadVector(val, &Parcel::ReadInt64);
}

bool Parcel::ReadUInt8Vector(std::vector<uint8_t> *val)
{
    return ReadVector(val, &Parcel::ReadUint8Unaligned);
}

bool Parcel::ReadUInt16Vector(std::vector<uint16_t> *val)
{
    return ReadVector(val, &Parcel::ReadUint16Unaligned);
}

bool Parcel::ReadUInt32Vector(std::vector<uint32_t> *val)
{
    return ReadVector(val, &Parcel::ReadUint32);
}

bool Parcel::ReadUInt64Vector(std::vector<uint64_t> *val)
{
    return ReadVector(val, &Parcel::ReadUint64);
}

bool Parcel::ReadFloatVector(std::vector<float> *val)
{
    return ReadVector(val, &Parcel::ReadFloat);
}

bool Parcel::ReadDoubleVector(std::vector<double> *val)
{
    return ReadVector(val, &Parcel::ReadDouble);
}

bool Parcel::ReadStringVector(std::vector<std::string> *val)
{
    return {};
}

bool Parcel::ReadString16Vector(std::vector<std::u16string> *val)
{
    return {};
}
}  // namespace OHOS
