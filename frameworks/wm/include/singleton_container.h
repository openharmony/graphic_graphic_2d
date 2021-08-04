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

#ifndef FRAMEWORKS_WM_INCLUDE_SINGLETON_CONTAINER_H
#define FRAMEWORKS_WM_INCLUDE_SINGLETON_CONTAINER_H

#include <any>
#include <cstdint>
#include <map>
#include <set>
#include <string>

#include <refbase.h>

namespace OHOS {
class SingletonContainer : public RefBase {
public:
    static sptr<SingletonContainer> GetInstance();

    void AddSingleton(const std::string &name, const std::any &instance);
    void SetSingleton(const std::string &name, const std::any &instance);
    const std::any& GetSingleton(const std::string &name);
    const std::any& DependOn(const std::string &instance, const std::string &name);

    template<class T>
    static sptr<T> Get()
    {
        std::string nameT = __PRETTY_FUNCTION__;
        nameT = nameT.substr(nameT.find("T = "));
        nameT = nameT.substr(sizeof("T ="), nameT.length() - sizeof("T = "));

        using sptrT = sptr<T>;
        sptrT ret = nullptr;
        const std::any &instance = SingletonContainer::GetInstance()->GetSingleton(nameT);
        auto pRet = std::any_cast<sptrT>(&instance);
        if (pRet != nullptr) {
            ret = *pRet;
        }
        return ret;
    }

    template<class T>
    static void Set(const sptr<T> &ptr)
    {
        std::string nameT = __PRETTY_FUNCTION__;
        nameT = nameT.substr(nameT.find("T = "));
        nameT = nameT.substr(sizeof("T ="), nameT.length() - sizeof("T = "));

        SingletonContainer::GetInstance()->SetSingleton(nameT, ptr);
    }

private:
    SingletonContainer() = default;
    virtual ~SingletonContainer() override;
    static inline sptr<SingletonContainer> instance = nullptr;

    struct Singleton {
        std::any value;
        int32_t refCount;
    };
    std::map<std::string, int32_t> stringMap;
    std::map<int32_t, SingletonContainer::Singleton> singletonMap;
    std::map<int32_t, std::set<int32_t>> dependencySetMap;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_SINGLETON_CONTAINER_H
