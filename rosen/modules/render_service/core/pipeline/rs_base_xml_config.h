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

#ifndef RS_CORE_PIPELINE_BASE_XML_CONFIG_H
#define RS_CORE_PIPELINE_BASE_XML_CONFIG_H

#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace OHOS {
namespace Rosen {
class RSBaseXmlConfig {
public:
    enum class ValueType {
        UNDIFINED,
        MAP,
        BOOL,
        STRING,
        STRINGS,
        INTS,
        FLOATS,
        POSITIVE_FLOATS,
    };
    struct ConfigItem {
        std::map<std::string, ConfigItem>* property = nullptr;
        ValueType type = ValueType::UNDIFINED;
        union {
            std::map<std::string, ConfigItem>* mapValue = nullptr;
            bool boolValue;
            std::string stringValue;
            std::vector<int>* intsValue;
            std::vector<float>* floatsValue;
            std::vector<std::string>* stringsValue;
        };
        ConfigItem() {}
        ~ConfigItem()
        {
            Destroy();
        };
        void Destroy()
        {
            ClearValue();
            if (property) {
                delete property;
                property = nullptr;
            }
        }
        void ClearValue()
        {
            switch (type) {
                case ValueType::MAP:
                    delete mapValue;
                    mapValue = nullptr;
                    break;
                case ValueType::STRING:
                    stringValue.~basic_string();
                    break;
                case ValueType::INTS:
                    delete intsValue;
                    intsValue = nullptr;
                    break;
                case ValueType::FLOATS:
                    delete floatsValue;
                    floatsValue = nullptr;
                    break;
                case ValueType::STRINGS:
                    delete stringsValue;
                    stringsValue = nullptr;
                    break;
                default:
                    break;
            }
        }
        ConfigItem(const ConfigItem& value)
        {
            *this = value;
        }
        ConfigItem& operator=(const ConfigItem& value)
        {
            Destroy();
            switch (value.type) {
                case ValueType::MAP:
                    mapValue = new std::map<std::string, ConfigItem>(*value.mapValue);
                    break;
                case ValueType::BOOL:
                    boolValue = value.boolValue;
                    break;
                case ValueType::STRING:
                    new(&stringValue)std::string(value.stringValue);
                    break;
                case ValueType::INTS:
                    intsValue = new std::vector<int>(*value.intsValue);
                    break;
                case ValueType::FLOATS:
                    floatsValue = new std::vector<float>(*value.floatsValue);
                    break;
                case ValueType::STRINGS:
                    stringsValue = new std::vector<std::string>(*value.stringsValue);
                    break;
                default:
                    break;
            }
            type = value.type;
            if (value.property) {
                property = new std::map<std::string, ConfigItem>(*value.property);
            }
            return *this;
        }
        ConfigItem(ConfigItem&& value) noexcept
        {
            *this = std::move(value);
        }
        ConfigItem& operator=(ConfigItem&& value) noexcept
        {
            Destroy();
            switch (value.type) {
                case ValueType::MAP:
                    mapValue = value.mapValue;
                    value.mapValue = nullptr;
                    break;
                case ValueType::BOOL:
                    boolValue = value.boolValue;
                    break;
                case ValueType::STRING:
                    new(&stringValue)std::string(std::move(value.stringValue));
                    break;
                case ValueType::INTS:
                    intsValue = value.intsValue;
                    value.intsValue = nullptr;
                    break;
                case ValueType::FLOATS:
                    floatsValue = value.floatsValue;
                    value.floatsValue = nullptr;
                    break;
                case ValueType::STRINGS:
                    stringsValue = value.stringsValue;
                    value.stringsValue = nullptr;
                    break;
                default:
                    break;
            }
            type = value.type;
            property = value.property;
            value.type = ValueType::UNDIFINED;
            value.property = nullptr;
            return *this;
        }
        void SetProperty(const std::map<std::string, ConfigItem>& prop)
        {
            if (property) {
                delete property;
            }
            property = new std::map<std::string, ConfigItem>(prop);
        }
        // set map value
        void SetValue(const std::map<std::string, ConfigItem>& value)
        {
            ClearValue();
            type = ValueType::MAP;
            mapValue = new std::map<std::string, ConfigItem>(value);
        }
        // set bool value
        void SetValue(bool value)
        {
            ClearValue();
            type = ValueType::BOOL;
            boolValue = value;
        }
        // set string value
        void SetValue(const std::string& value)
        {
            ClearValue();
            type = ValueType::STRING;
            new(&stringValue)std::string(value);
        }
        // set ints value
        void SetValue(const std::vector<int>& value)
        {
            ClearValue();
            type = ValueType::INTS;
            intsValue = new std::vector<int>(value);
        }
        // set floats value
        void SetValue(const std::vector<float>& value)
        {
            ClearValue();
            type = ValueType::FLOATS;
            floatsValue = new std::vector<float>(value);
        }
        // set strings value
        void SetValue(const std::vector<std::string>& value)
        {
            ClearValue();
            type = ValueType::STRINGS;
            stringsValue = new std::vector<std::string>(value);
        }
        bool IsInts() const
        {
            return type == ValueType::INTS;
        }
        bool IsFloats() const
        {
            return type == ValueType::FLOATS;
        }
        bool IsString() const
        {
            return type == ValueType::STRING;
        }
        bool IsStrings() const
        {
            return type == ValueType::STRINGS;
        }
        bool IsBool() const
        {
            return type == ValueType::BOOL;
        }
        bool IsMap() const
        {
            return type == ValueType::MAP;
        }
        const ConfigItem& operator[](const std::string& key) const
        {
            if (type != ValueType::MAP) {
                return defaultItem;
            }
            if (mapValue->count(key) == 0) {
                return defaultItem;
            }
            return mapValue->at(key);
        }
        const ConfigItem& GetProp(const std::string& key) const
        {
            if (!property) {
                return defaultItem;
            }
            if (property->count(key) == 0) {
                return defaultItem;
            }
            return property->at(key);
        }
        static const ConfigItem defaultItem;
    };
    static std::recursive_mutex configItemMutex;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_CORE_PIPELINE_BASE_XML_CONFIG_H
