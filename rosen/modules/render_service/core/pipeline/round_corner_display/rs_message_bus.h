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

#ifndef RS_CORE_PIPELINE_RCD_MESSAGE_BUS_H
#define RS_CORE_PIPELINE_RCD_MESSAGE_BUS_H

#include <iostream>
#include <map>
#include <functional>
#include <mutex>
#include <thread>
#include "rs_any.h"
#include "rs_notCopyable.h"

// @note declaration
template<typename... Args>
struct Impl;

// @note recursion end
template<typename T>
struct Impl<T> {
    static std::string name()
    {
        return std::to_string(sizeof(T));
    }
};

// @note recursion end
template<>
struct Impl<> {
    static std::string name()
    {
        return "";
    }
};

// @note recursion
template<typename T, typename... Args>
struct Impl<T, Args...> {
    static std::string name()
    {
        return std::to_string(sizeof(T)) + "," + Impl<Args...>::name();
    }
};

// @note recursion start
template<typename... Args>
std::string GetTypeName()
{
    return Impl<Args...>::name();
}

class RsMessageBus : RsNotCopyable {
public:
    // @brief register message processing functions based on topic and message type
    // @tparam ...Args  input parameters of funtion TMember
    // @tparam TObject  object type
    // @tparam TMember  function member type of object
    // @param strTopic  message topic
    // @param Object  object
    // @param Member  function member of the object
    template<typename... Args, typename TObject, typename TMember>
    void RegisterTopic(std::string strTopic, TObject *Object, TMember Member)
    {
        std::lock_guard<std::mutex> lock(m_map_mutex);
        std::function<void(Args...)> f = std::function<void(Args...)>([=](Args... arg) {
            (Object->*Member)(arg...);
        });
        m_map.emplace(GetKey<Args...>(strTopic), f);
    }
    // @brief Send the message to the bus, After receiving the message,
    // the messagebus will find and call the corresponding message processing function.
    // @tparam ...Args  parameter's type
    // @param strTopic  message topic
    // @param ...args  parameters
    template<typename... Args>
    void SendMsg(std::string strTopic, Args... args)
    {
        std::lock_guard<std::mutex> lock(m_map_mutex);
        auto range = m_map.equal_range(GetKey<Args...>(strTopic));
        std::multimap<std::string, RsAny>::iterator it;
        for (it = range.first; it != range.second; it++) {
            it->second.Send<Args...>(args...);
        }
    }
    // @brief  remove the topic
    // @tparam ...Args  Parameter's type
    // @param strTopic  message topic
    template<typename... Args>
    void RemoveTopic(std::string strTopic)
    {
        std::lock_guard<std::mutex> lock(m_map_mutex);
        auto it = m_map.find(GetKey<Args...>(strTopic));
        while (it != m_map.end()) {
            m_map.erase(it++);
        }
    }
private:
    // Calculate key values based on the Topic
    template<typename... Args>
    std::string GetKey(std::string &strTopic)
    {
        return strTopic + GetTypeName<Args...>();
    }
    std::multimap<std::string, RsAny> m_map;
    std::mutex m_map_mutex;
};
#endif