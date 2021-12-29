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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_SINGLETON_H
#define RENDER_SERVICE_BASE_COMMON_RS_SINGLETON_H

namespace OHOS {
namespace Rosen {

#define RS_DECLARE_SINGLETON(MyClass)             \
private:                                          \
    friend RSSingleton<MyClass>;                  \
    MyClass &operator=(const MyClass &) = delete; \
    MyClass(const MyClass &) = delete;            \
    MyClass();                                    \
    ~MyClass();

template <typename T>
class RSSingleton {
public:
    static T& GetInstance()
    {
        return instance_;
    }

protected:
    RSSingleton() = default;
    virtual ~RSSingleton() = default;

private:
    RSSingleton(const RSSingleton&) = delete;
    RSSingleton& operator=(const RSSingleton&) = delete;
    RSSingleton(RSSingleton&&) = delete;
    RSSingleton& operator=(RSSingleton&&) = delete;

    static T instance_;
};

template <typename T>
T RSSingleton<T>::instance_;

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_COMMON_RS_SINGLETON_H
