### 使用说明

将cpp和ets目录下的文件夹，拷贝到目标工程的entry/src/main目录下进行对应替换

### 工程目录

```
 ├──ets                           // ets代码区
 │  ├──entryability
 │  │  └──EntryAbility.ts         // 程序入口类
 |  |──interface
 │  │  └──XComponentContext.ts    // XComponentContext
 |  ├──pages
 │  │  |──casefactory.ts          // 测试用例工厂类，用于用例名与对应代码的映射
 |  |  └──Index.ets               // 主界面代码入口
 |  |  └──myxnode.ts              // xnode测试框架ets层代码


 ├──cpp                           // cpp代码区
 │  ├──common                     // 日志打印相关
 |  |──dm
 │  │  └──xxx.cpp                 // 功能(dm)用例代码
 |  |──interface
 │  │  └──xxx.cpp                 // 单接口性能用例代码
 │  │──plugin                     // 生命周期管理模块
 |  |
 │  └──my_xcomponent.cpp          // xcomponent测试框架cpp层代码
 |  └──my_xnode.cpp               // xnode测试框架cpp层代码
 |  └──test_base.cpp              // 测试用例基类
 |  └──test_common.cpp            // 测试辅助类
 |  └──testcasefactory.cpp        // 测试用例工厂类，用于用例名与对应代码的映射
 |  
```