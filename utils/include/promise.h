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

#ifndef UTILS_INCLUDE_PROMISE_H
#define UTILS_INCLUDE_PROMISE_H

#include <condition_variable>
#include <thread>

#include <refbase.h>

namespace OHOS {
template<class T>
class Promise : public RefBase {
public:
    Promise() = default;
    virtual ~Promise() = default;

    Promise(const T& t)
    {
        value = t;
        resolved = true;
    }

    virtual T Await()
    {
        if (resolved == false) {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this]() { return resolved == true; });
        }
        return value;
    }

    virtual void Then(std::function<void(const T &t)> func)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (resolved == false) {
            this->func = func;
        } else {
            func(value);
        }
    }

    virtual bool Resolve(const T &t)
    {
        if (resolved == false) {
            std::unique_lock<std::mutex> lock(mutex);
            if (resolved == false) {
                value = t;
                resolved = true;
                cv.notify_all();
                if (func != nullptr) {
                    func(value);
                }
                return true;
            }
        }
        return false;
    }

private:
    bool resolved = false;
    std::mutex mutex;
    std::condition_variable cv;
    std::function<void(const T &t)> func = nullptr;
    T value;
};
} // namespace OHOS

#endif // UTILS_INCLUDE_PROMISE_H
