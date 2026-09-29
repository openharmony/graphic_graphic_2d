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

#ifndef ROSEN_TEXT_REF_COUNT_HOLDER_H
#define ROSEN_TEXT_REF_COUNT_HOLDER_H

#include <atomic>
#include <memory>
#include <utility>

namespace OHOS {
namespace Rosen {
/**
 * @brief A generic reference-counted holder that wraps an exclusively owned object
 *        so it can be shared across module boundaries (e.g. C API and ArkUI)
 *        where only one stable raw pointer with manual AddRef/Release is available.
 *
 *        Header-only on purpose: it keeps no process-wide static state, so every
 *        DSO including it gets an independent, layout-identical instantiation and
 *        no cross-DSO symbol binding is introduced. The creator owns the initial
 *        reference (count == 1); the wrapped object is destroyed together with the
 *        holder when the count drops to zero.
 *
 *        T must be deletable through the pointer passed in (a virtual destructor
 *        is required when deleted via a base-class pointer).
 */
template<typename T>
class RefCountedHolder {
public:
    explicit RefCountedHolder(std::unique_ptr<T> obj) : obj_(std::move(obj)), refCount_(1) {}

    void AddRef() noexcept
    {
        refCount_.fetch_add(1, std::memory_order_relaxed);
    }

    void Release() noexcept
    {
        if (refCount_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete this;
        }
    }

    T* Get() const noexcept
    {
        return obj_.get();
    }

private:
    ~RefCountedHolder() = default; // only reachable through Release()

    std::unique_ptr<T> obj_;
    std::atomic<uint32_t> refCount_;
};

/**
 * @brief Optional RAII companion of RefCountedHolder for holders that manage
 *        their reference manually (AddRef on acquire, Release on drop).
 */
template<typename T>
class RefCountedPtr {
public:
    RefCountedPtr() = default;

    explicit RefCountedPtr(RefCountedHolder<T>* holder) : holder_(holder)
    {
        if (holder_ != nullptr) {
            holder_->AddRef();
        }
    }

    RefCountedPtr(const RefCountedPtr& other) : holder_(other.holder_)
    {
        if (holder_ != nullptr) {
            holder_->AddRef();
        }
    }

    RefCountedPtr(RefCountedPtr&& other) noexcept : holder_(other.holder_)
    {
        other.holder_ = nullptr;
    }

    RefCountedPtr& operator=(const RefCountedPtr& other)
    {
        if (this != &other) {
            RefCountedPtr tmp(other);
            Swap(tmp);
        }
        return *this;
    }

    RefCountedPtr& operator=(RefCountedPtr&& other) noexcept
    {
        if (this != &other) {
            Reset();
            holder_ = other.holder_;
            other.holder_ = nullptr;
        }
        return *this;
    }

    ~RefCountedPtr()
    {
        Reset();
    }

    void Reset()
    {
        if (holder_ != nullptr) {
            holder_->Release();
            holder_ = nullptr;
        }
    }

    T* Get() const noexcept
    {
        return holder_ != nullptr ? holder_->Get() : nullptr;
    }

    explicit operator bool() const noexcept
    {
        return holder_ != nullptr;
    }

private:
    void Swap(RefCountedPtr& other) noexcept
    {
        std::swap(holder_, other.holder_);
    }

    RefCountedHolder<T>* holder_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_TEXT_REF_COUNT_HOLDER_H
