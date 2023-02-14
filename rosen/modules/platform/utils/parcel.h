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

 /**
  * @file parcel.h
  *
  * @brief Provide classes for data container implemented in c_utils.
  *
  * Including class Parcel, Parcelable and related memory Allocator.
  */

#ifndef OHOS_UTILS_PARCEL_H
#define OHOS_UTILS_PARCEL_H

#include <string>
#include <vector>
#include "nocopyable.h"
#include "refbase.h"
#include "flat_obj.h"

namespace OHOS {

class Parcel;

/**
 * @brief Interface for class whose Instance can be written into a Parcel.
 *
 * @note If this object is remote, its position intends to
 * use in kernel data transaction.
 */
class Parcelable : public virtual RefBase {
public:
    virtual ~Parcelable() = default;

    Parcelable();
    /**
     * @brief Construct a new Parcelable object.
     *
     * @param asRemote Specifies whether this object is remote.
     */
    explicit Parcelable(bool asRemote);

    /**
     * @brief Write a parcelable object to the given parcel.
     *
     * @param parcel Target parcel to write to.
     * @return Return true being written on success or false if any error occur.
     * @note If this object is remote, its position will be saved in the parcel.
     * @note A static Unmarshalling function must also be implemented, so that
     * you can get data from the given parcel into this parcelable object.
     * See `static TestParcelable *Unmarshalling(Parcel &parcel)` as an example.
     */
    virtual bool Marshalling(Parcel &parcel) const = 0;

    /**
     * @brief Enum flag to describe behaviors of the Parcelable object.
     *
     * @var IPC Indicate the object can be used in IPC.
     * @var RPC Indicate the object can be used in RPC.
     * @var HOLD_OBJECT Indicate the object will ensure
     * alive during data transaction.
     *
     */
    enum BehaviorFlag { IPC = 0x01, RPC = 0x02, HOLD_OBJECT = 0x10 };

    /**
     * @brief Enable specified behavior.
     *
     * @param b Specific flag value.
     * @see BehaviorFlag.
     */
    inline void SetBehavior(BehaviorFlag b) const
    {
        behavior_ |= static_cast<uint8_t>(b);
    }

    /**
     * @brief Disable specified behavior.
     *
     * @param b Specific flag value. See BehaviorFlag.
     */
    inline void ClearBehavior(BehaviorFlag b) const
    {
        behavior_ &= static_cast<uint8_t>(~b);
    }

    /**
     * @brief Check whether specified behavior is enabled.
     *
     * @param b Specific flag value.
     * @see BehaviorFlag.
     */
    inline bool TestBehavior(BehaviorFlag b) const
    {
        return behavior_ & (static_cast<uint8_t>(b));
    }

public:
    bool asRemote_; // if the object is remote
    mutable uint8_t behavior_; // value of flag of behaviors
};

/**
 * @brief Interface for memory allocator of the data in Parcel.
 */
class Allocator {
public:
    virtual ~Allocator() = default;

    virtual void *Realloc(void *data, size_t newSize) = 0;

    virtual void *Alloc(size_t size) = 0;

    virtual void Dealloc(void *data) = 0;
};

/**
 * @brief Default implement of Allocator.
 *
 * @note Allocator defined by user for a parcel need be specified manually.
 */
class DefaultAllocator : public Allocator {
public:
    /**
     * @brief Use `malloc()` to allocate memory for a parcel.
     *
     * @param size Size of desired memory region.
     * @return Void-type pointer to the memory region.
     */
    void *Alloc(size_t size) override;

    /**
     * @brief Use `free()` to deallocate memory for a parcel.
     *
     * @param data Void-type pointer to the memory region.
     */
    void Dealloc(void *data) override;
private:
    /**
     * @brief Use `realloc()` to reallocate memory for a parcel.
     *
     * @param data Void-type pointer to the existed memory region.
     * @param newSize New desired size of memory region.
     * @return Void-type pointer to the new memory region.
     */
    void *Realloc(void *data, size_t newSize) override;
};

/**
 * @brief A data/message container.
 *
 * Contains interfaces for writing and reading data of various types,
 * including primitives, Parcelable objects etc.
 *
 * @note Usually used in IPC, RPC.
 */
class Parcel {
public:
    Parcel();

    /**
     * @brief Construct a new Parcel object with specified Allocator.
     *
     * @param allocator Specified Allocator.
     */
    explicit Parcel(Allocator *allocator);

    virtual ~Parcel();

    /**
     * @brief Get total size of existed data in this parcel.
     *
     * @return Value of the size in bytes.
     */
    size_t GetDataSize() const;

    /**
     * @brief Get the pointer to the beginning of data in this parcel.
     *
     * @return `uintptr_t`-type pointer.
     */
    uintptr_t GetData() const;

    /**
     * @brief Get position of every object written in this parcel.
     *
     * @return `binder_size_t`-type pointer to
     * the first slot of the position array.
     * @see flat_obj.h
     */
    binder_size_t GetObjectOffsets() const;

    /**
     * @brief Get total size of all of the current positions.
     *
     * @return Value of the size in bytes.
     */
    size_t GetOffsetsSize() const;

    /**
     * @brief Get total availiable bytes to write to this parcel.
     *
     * @return Amount of the availiable bytes.
     */
    size_t GetWritableBytes() const;

    /**
     * @brief Get total availiable bytes to read from this parcel.
     *
     * @return Amount of the availiable bytes.
     */
    size_t GetReadableBytes() const;

    /**
     * @brief Get total capacity of this parcel,
     * i.e. size of current data region in parcel.
     *
     * @return Value of the capacity in bytes.
     */
    size_t GetDataCapacity() const;

    /**
     * @brief Get maximum capacity of this parcel.
     *
     * @return Value of the capacity in bytes.
     */
    size_t GetMaxCapacity() const;

    /**
     * @brief Set capacity of this parcel,
     * i.e. size of current data region in parcel.
     *
     * @param newCapacity Desired new capacity.
     * @return Return True on success, or false if any error occur.
     * @note Corresponding Allocator will try to reallocate
     * the data region with new capacity.
     *
     */
    bool SetDataCapacity(size_t newCapacity);

    /**
     * @brief Set total size of existed data in this parcel.
     *
     * @param dataSize Desired value of size in bytes.
     * @return Return True on success, or false if any error occur.
     * @note Avoid using it independently, otherwise it may not
     * represents the correct size of existed data.
     */
    bool SetDataSize(size_t dataSize);

    /**
     * @brief Set maximux capacity of this parcel.
     *
     * @param maxCapacity Desired value of maximum capacity.
     * @return Return True on success, or false if any error occur.
     */
    bool SetMaxCapacity(size_t maxCapacity);

    // write primitives in alignment
    bool WriteBool(bool value);
    bool WriteInt8(int8_t value);
    bool WriteInt16(int16_t value);
    bool WriteInt32(int32_t value);
    bool WriteInt64(int64_t value);
    bool WriteUint8(uint8_t value);
    bool WriteUint16(uint16_t value);
    bool WriteUint32(uint32_t value);
    bool WriteUint64(uint64_t value);
    bool WriteFloat(float value);
    bool WriteDouble(double value);
    bool WritePointer(uintptr_t value);

    /**
     * @brief Write a data region(buffer) to this parcel.
     *
     * @param data Void-type pointer to the buffer.
     * @param size Size of the buffer to be written.
     * @return Return True on success, or false if any error occur.
     */
    bool WriteBuffer(const void *data, size_t size);

    /**
     * @brief Write a data region(buffer) to this parcel
     * in alignment and with terminator replaced.
     *
     * @param data Void-type pointer to the buffer.
     * @param size Size of the buffer to be written.
     * @param typeSize Size of the terminator.
     * @return Return True on success, or false if any error occur.
     * @note The last several bytes specified by `typeSize` of the aligned data
     * will be deemed as a terminator, and then will be replaced by
     * '0b00000000'.
     */
    bool WriteBufferAddTerminator(const void *data, size_t size, size_t typeSize);

    /**
     * @brief Write a data region(buffer) to this parcel without padding.
     *
     * @param data Void-type pointer to the buffer.
     * @param size Size of the buffer to be written.
     * @return Return True on success, or false if any error occur.
     */
    bool WriteUnpadBuffer(const void *data, size_t size);

    /**
     * @brief Write a C-style string to this parcel.
     *
     * Default terminator `\0` of C-style string will also write.
     *
     * @param value A C-style string, i.e. char-type pointer.
     * @return Return True on success, or false if any error occur.
     */
    bool WriteCString(const char *value);

    /**
     * @brief Write a C++ string(`std::u16string`) to this parcel.
     *
     * The exact length of the string will write first, then the string itself
     * with an appended terminator `\0` will write.
     *
     * @param value An `std::string` object passed by reference.
     * @return Return True on success, or false if any error occur.
     */
    bool WriteString(const std::string &value);

    /**
     * @brief Write a C++ UTF-16 encoded string(`std::string`) to this parcel.
     *
     * The exact length of the string will write first, then the string itself
     * with an appended terminator `\0` will write.
     *
     * @param value An `std::u16string` object passed by reference.
     * @return Return True on success, or false if any error occur.
     */
    bool WriteString16(const std::u16string &value);

    /**
     * @brief Write a UTF-16 encoded string
     * with specified length to this parcel.
     *
     * An `std::u16string` object will be constructed by input `char16_t*`
     * pointer and the length `len` first. Then the input length and the string
     * data in `u16string` object with an appended terminator `\0` will write.
     *
     * @param value Pointer to the UTF-16 encoded string.
     * @param len Exact length of the input string.
     * @return Return True on success, or false if any error occur.
     */
    bool WriteString16WithLength(const char16_t *value, size_t len);

    /**
     * @brief Write a UTF-8 encoded string
     * with specified length to this parcel.
     *
     * The input length `len` and the string data
     * with an appended terminator `\0` will write.
     *
     * @param value Pointer to the UTF-8 encoded string.
     * @param len Exact length of the input string.
     * @return Return True on success, or false if any error occur.
     */
    bool WriteString8WithLength(const char *value, size_t len);

    /**
     * @brief Write a Parcelable object to this parcel.
     *
     * Remote object will be written by `WriteRemoteObject(const Parcelable *)`.
     * Non-remote object will be written by its own
     * `Marshalling(Parcel &parcel)`.
     *
     * @param object Pointer to the input Parcelable object.
     * @return Return True on success, or false if any error occur.
     * @note '0' of `Int32_t` will write if input pointer is `nullptr`.
     */
    bool WriteParcelable(const Parcelable *object);

    /**
     * @brief Write a Parcelable object to this parcel,
     * and enable its behavior of `HOLD_OBJECT`.
     *
     * @param object Smart pointer to the input parcelable object.
     * @return Return True on success, or false if any error occur.
     */
    bool WriteStrongParcelable(const sptr<Parcelable> &object);

    /**
     * @brief Write a remote object to this parcel.
     *
     * @param object Pointer to a remote object.
     * @return Return True on success, or false if any error occur.
     * @note If `HOLD_OBJECT` is enabled for the input object, it will stay
     * alive as long as this parcel is alive.
     *
     */
    bool WriteRemoteObject(const Parcelable *object);

    /**
     * @brief Write an object to this parcel.
     *
     * Use its own `Marshalling(Parcel &parcel)` when `nullptr` as input,
     * otherwise use `WriteRemoteObject(const Parcelable *)`.
     *
     * @tparam T Specific class type of the input object.
     * @param object Smart pointer to the input object.
     * @return Return True on success, or false if any error occur.
     */
    template<typename T>
    bool WriteObject(const sptr<T> &object);

    /**
     * @brief Parse input data by this parcel.
     *
     * @param data Pointer to input data.
     * @param size Size of input data(Bytes).
     * @return Return True on success, or false if any error occur.
     * @note Only read operation from this parcel is permitted after successful
     * calling this method.
     */
    bool ParseFrom(uintptr_t data, size_t size);

    bool ReadBool();

    int8_t ReadInt8();

    int16_t ReadInt16();

    int32_t ReadInt32();

    int64_t ReadInt64();

    uint8_t ReadUint8();

    uint16_t ReadUint16();

    uint32_t ReadUint32();

    uint64_t ReadUint64();

    float ReadFloat();

    double ReadDouble();

    uintptr_t ReadPointer();

    bool ReadBool(bool &value);

    bool ReadInt8(int8_t &value);

    bool ReadInt16(int16_t &value);

    bool ReadInt32(int32_t &value);

    bool ReadInt64(int64_t &value);

    bool ReadUint8(uint8_t &value);

    bool ReadUint16(uint16_t &value);

    bool ReadUint32(uint32_t &value);

    bool ReadUint64(uint64_t &value);

    bool ReadFloat(float &value);

    bool ReadDouble(double &value);

    /**
     * @brief Read a block of data(buffer data) from this parcel.
     *
     * @param length Size of the buffer(Bytes).
     * @return A `uint8_t` pointer to the buffer.
     */
    const uint8_t *ReadBuffer(size_t length);

    /**
     * @brief Read a block of data(buffer data) without padding(alignment) from
     * this parcel.
     *
     * This method will read the effective data whose length specified by
     * `length` and discard bytes for padding.
     *
     * @param length Effective size of the buffer(Bytes).
     * @return A `uint8_t` pointer to the buffer.
     *
     */
    const uint8_t *ReadUnpadBuffer(size_t length);

    /**
     * @brief Skip the next several bytes specifed by `bytes` in read process.
     *
     * @param bytes Skipped number of bytes.
     */
    void SkipBytes(size_t bytes);

    /**
     * @brief Read a C-style string from this parcel.
     *
     * @return A C-style string, which is represented by a `char`-type pointer.
     */
    const char *ReadCString();

    /**
     * @brief Read a C++ string(`std::string`) object from this parcel.
     *
     * @return An `std::string` object.
     */
    const std::string ReadString();

    /**
     * @brief Read a C++ string(`std::string`) object from this parcel to input
     * object.
     *
     * @param value Target receiver `std::string` object.
     * @return Return True on success, or false if any error occur.
     */
    bool ReadString(std::string &value);

    /**
     * @brief Read a C++ UTF-16 encoded string(`std::string`) object from this
     * parcel.
     *
     * @return An `std::u16string` object.
     */
    const std::u16string ReadString16();

    /**
     * @brief Read a C++ UTF-16 string(`std::u16string`) object from this
     * parcel to input object.
     *
     * @param value Target receiver `std::string` object.
     * @return Return True on success, or false if any error occur.
     */
    bool ReadString16(std::u16string &value);

    /**
     * @brief Read a C++ UTF-16 string(`std::u16string`) object and its length
     * from this parcel.
     *
     * @param len `int32_t`-type variable passed by reference to receive the
     * length.
     * @return An output `std::u16string` object.
     */
    const std::u16string ReadString16WithLength(int32_t &len);

    /**
     * @brief Read a C++ string(`std::string`) object and its length from this
     * parcel.
     *
     * @param len `int32_t`-type variable passed by reference to receive the
     * length.
     * @return An output `std::u8string` object.
     */
    const std::string ReadString8WithLength(int32_t &len);

    /**
     * @brief Set the read cursor to specified position.
     *
     * @param newPosition Specified position represented by offsets relative to
     * the beginning of the data region.
     * @return Return True on success, or false if any error occur.
     */
    bool RewindRead(size_t newPosition);

    /**
     * @brief Set the write cursor to specified position.
     *
     * @param offsets Specified position represented by offsets(Bytes) relative
     * to the beginning of the data region.
     * @return Return True on success, or false if any error occur.
     */
    bool RewindWrite(size_t offsets);

    /**
     * @brief Get current position of read cursor.
     *
     * @return Position represented by offsets(Bytes) relative to the beginning
     * of the data region.
     */
    size_t GetReadPosition();

    /**
     * @brief Get current position of write cursor.
     *
     * @return Position represented by offsets(Bytes) relative to the beginning
     * of the data region.
     */
    size_t GetWritePosition();

    /**
     * @brief Read a Parcelable(and its subclass) object from this parcel.
     *
     * @tparam T Specific class type of the output object.
     * @return Object of specific class.
     * @note `nullptr` will return if a '0' is read.
     */
    template <typename T>
    T *ReadParcelable();

    /**
     * @brief Read a Parcelable object from this parcel, and manage it by a
     * smart pointer.
     *
     * @tparam T Specific class type of the output object.
     * @return Object managed by a smart pointer.
     * @note `nullptr` will return if a '0' is read.
     */
    template <typename T>
    sptr<T> ReadStrongParcelable();

    /**
     * @brief Check if it is valid to read an object from current cursor.
     *
     * @return Return true if valid, otherwise return false.
     */
    bool CheckOffsets();

    /**
     * @brief Read an object from this parcel.
     *
     * `CheckOffsets()` will call first to check whether it is valid to read an
     * object.
     *
     * @tparam T Specific class type of the output object.
     * @return A smart pointer to the object.
     * @note `nullptr` will return if `CheckOffsets()` fail.
     */
    template<typename T>
    sptr<T> ReadObject();

    /**
     * @brief Set the Allocator object of this parcel.
     *
     * New Allocator will reallocate the data region to which has been written.
     *
     * @param allocator Specified Allocator object.
     * @return Return True on success, or false if any error occur.
     */
    bool SetAllocator(Allocator *allocator);

    /**
     * @brief Record an array of offsets of the object.
     *
     * @param offsets A pointer to the input array.
     * @param offsetSize Size of the input array.
     * @note It will return directly if fail.
     */
    void InjectOffsets(binder_size_t offsets, size_t offsetSize);

    /**
     * @brief Deallocate the data region, and reset this parcel.
     */
    void FlushBuffer();

    /**
     * @brief Write an `std::vector` object to this parcel.
     *
     * @tparam T1 Specific class type for input vector.
     * @tparam T2 Specific class type for write method of this parcel.
     * @param val Input vector object passed by reference.
     * @param Write Specific `Parcel::Write(T2 value)` method.
     * @return Return True on success, or false if any error occur.
     */
    template <typename T1, typename T2>
    bool WriteVector(const std::vector<T1> &val, bool (Parcel::*Write)(T2));
    bool WriteBoolVector(const std::vector<bool> &val);
    bool WriteInt8Vector(const std::vector<int8_t> &val);
    bool WriteInt16Vector(const std::vector<int16_t> &val);
    bool WriteInt32Vector(const std::vector<int32_t> &val);
    bool WriteInt64Vector(const std::vector<int64_t> &val);
    bool WriteUInt8Vector(const std::vector<uint8_t> &val);
    bool WriteUInt16Vector(const std::vector<uint16_t> &val);
    bool WriteUInt32Vector(const std::vector<uint32_t> &val);
    bool WriteUInt64Vector(const std::vector<uint64_t> &val);
    bool WriteFloatVector(const std::vector<float> &val);
    bool WriteDoubleVector(const std::vector<double> &val);
    bool WriteStringVector(const std::vector<std::string> &val);
    bool WriteString16Vector(const std::vector<std::u16string> &val);

    /**
     * @brief Read an `std::vector` object from this parcel.
     *
     * @tparam T1 Specific class type for output vector.
     * @tparam T2 Specific class type for read method of this parcel.
     * @param val Pointer to the output vector object.
     * @param Write Specific `Parcel::Read(T2 value)` method.
     * @return Return True on success, or false if any error occur.
     */
    template <typename T>
    bool ReadVector(std::vector<T> *val, bool (Parcel::*Read)(T &));
    bool ReadBoolVector(std::vector<bool> *val);
    bool ReadInt8Vector(std::vector<int8_t> *val);
    bool ReadInt16Vector(std::vector<int16_t> *val);
    bool ReadInt32Vector(std::vector<int32_t> *val);
    bool ReadInt64Vector(std::vector<int64_t> *val);
    bool ReadUInt8Vector(std::vector<uint8_t> *val);
    bool ReadUInt16Vector(std::vector<uint16_t> *val);
    bool ReadUInt32Vector(std::vector<uint32_t> *val);
    bool ReadUInt64Vector(std::vector<uint64_t> *val);
    bool ReadFloatVector(std::vector<float> *val);
    bool ReadDoubleVector(std::vector<double> *val);
    bool ReadStringVector(std::vector<std::string> *val);
    bool ReadString16Vector(std::vector<std::u16string> *val);

    // write raw primitive type(i.e. 1byte for bool, 4byte for `int16_t`, etc.)
    bool WriteBoolUnaligned(bool value);
    bool WriteInt8Unaligned(int8_t value);
    bool WriteInt16Unaligned(int16_t value);
    bool WriteUint8Unaligned(uint8_t value);
    bool WriteUint16Unaligned(uint16_t value);
    // read raw primitive type(i.e. 1byte for bool, 4byte for `int16_t`, etc.)
    bool ReadBoolUnaligned();
    bool ReadInt8Unaligned(int8_t &value);
    bool ReadInt16Unaligned(int16_t &value);
    bool ReadUint8Unaligned(uint8_t &value);
    bool ReadUint16Unaligned(uint16_t &value);

protected:
    /**
     * @brief Record position of the written object, which are represented by
     * offset from the beginning of the data region.
     *
     * @param offset Specific position.
     * @return Return True on success, or false if any error occur.
     */
    bool WriteObjectOffset(binder_size_t offset);

    /**
     * @brief Ensure number of the written objects is lower than the capacity of
     * objects.
     *
     * If it is full, the capacity will be expanded.
     *
     * @return Return True on success, or false if any error occur.
     */
    bool EnsureObjectsCapacity();

private:
    DISALLOW_COPY_AND_MOVE(Parcel);
    template <typename T>
    bool Write(T value);

    template <typename T>
    bool Read(T &value);

    template <typename T>
    T Read();

    template <typename T>
    bool ReadPadded(T &value);

    inline size_t GetPadSize(size_t size)
    {
        const size_t SIZE_OFFSET = 3;
        return (((size + SIZE_OFFSET) & (~SIZE_OFFSET)) - size);
    }

    size_t CalcNewCapacity(size_t minCapacity);

    bool WriteDataBytes(const void *data, size_t size);

    void WritePadBytes(size_t padded);

    bool EnsureWritableCapacity(size_t desireCapacity);

    bool WriteParcelableOffset(size_t offset);

private:
    uint8_t *data_;
    size_t readCursor_;
    size_t writeCursor_;
    size_t dataSize_;
    size_t dataCapacity_;
    size_t maxDataCapacity_;
    binder_size_t *objectOffsets_;
    size_t objectCursor_;
    size_t objectsCapacity_;
    Allocator *allocator_;
    std::vector<sptr<Parcelable>> objectHolder_;
    bool writable_ = true;
};

template <typename T>
bool Parcel::WriteObject(const sptr<T> &object)
{
    if (object == nullptr) {
        return T::Marshalling(*this, object);
    }
    return WriteRemoteObject(object.GetRefPtr());
}

template <typename T>
sptr<T> Parcel::ReadObject()
{
    if (!this->CheckOffsets()) {
        return nullptr;
    }
    sptr<T> res(T::Unmarshalling(*this));
    return res;
}

// Read data from the given parcel into this parcelable object.
template <typename T>
T *Parcel::ReadParcelable()
{
    int32_t size = this->ReadInt32();
    if (size == 0) {
        return nullptr;
    }
    return T::Unmarshalling(*this);
}

// Read data from the given parcel into this parcelable object, return sptr.
template <typename T>
sptr<T> Parcel::ReadStrongParcelable()
{
    int32_t size = this->ReadInt32();
    if (size == 0) {
        return nullptr;
    }
    sptr<T> res(T::Unmarshalling(*this));
    return res;
}

} // namespace OHOS
#endif
