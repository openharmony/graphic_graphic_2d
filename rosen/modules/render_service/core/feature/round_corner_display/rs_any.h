/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RS_CORE_PIPELINE_RCD_ANY_H
#define RS_CORE_PIPELINE_RCD_ANY_H
#include <functional>
#include <mutex>
#include <thread>
#include <memory>
#include <iostream>

struct RsAny {
    RsAny(void) {}
    RsAny(const RsAny &that) : m_ptr(that.Clone()) {}
    RsAny(RsAny &&that) : m_ptr(std::move(that.m_ptr)) {}

    template<typename U, class = typename std::enable_if<
        !std::is_same<typename std::decay<U>::type, RsAny>::value, U>::type> RsAny(U &&value) : m_ptr(
        new Derived<typename std::decay<U>::type>(std::forward<U>(value))) {}

    bool IsNULL() const { return !bool(m_ptr); }

    template<class U>
    U& anyCast()
    {
        auto derived = reinterpret_cast<Derived<U>*>(m_ptr.get());
        return derived->m_value;
    }

    template<typename... Args>
    void Send(Args... args)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto f = anyCast<std::function<void(Args...)>>();
        f(args...);
    }

    RsAny& operator=(const RsAny &a)
    {
        if (m_ptr == a.m_ptr) {
            return *this;
        }
        m_ptr = a.Clone();
        return *this;
    }

private:
    std::mutex m_mutex;
    struct Base;
    using BaseUPtr = std::unique_ptr<Base>;
    struct Base {
        virtual ~Base() {}
        virtual BaseUPtr Clone() const = 0;
    };

    template<typename T>
    struct Derived : Base {
        template<typename U>
        Derived(U&& value) : m_value(std::forward<U>(value)) {}

        BaseUPtr Clone() const
        {
            return BaseUPtr(new Derived<T>(m_value));
        }

        T m_value;
    };

    BaseUPtr m_ptr;

public:
    BaseUPtr Clone() const
    {
        if (m_ptr != nullptr) {
            return m_ptr->Clone();
        }
        return nullptr;
    }
};
#endif