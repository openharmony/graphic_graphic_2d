使用说明
1）将ets目录下的文件夹，拷贝到目标工程的entry/src/main目录下进行替换，TS源工程可以参考
https://gitee.com/openharmony/applications_app_samples/tree/master/code/BasicFeature/Native/NdkTsDrawing
2）拷贝图片资源，将
https://gitee.com/openharmony/applications_app_samples/tree/master/code/BasicFeature/Native/NdkRenderNodeDrawing/entry/src/main/resources/rawfile
目录下的图片拷贝到目标工程的entry/src/main/resources/rawfile/文件夹下
3）因为bench工程需要用到4张图片，所以需要手动将上面拷贝的图片test.jpg重命名成test_1.jpg, test_2.jpg, test_3.jpg, test_4.jpg
或者可以本地准备4个图片，重命名成test_1.jpg, test_2.jpg, test_3.jpg, test_4.jpg
4）编译运行项目即可

### 工程目录

```
 ├──ets                           // ets代码区
 │  ├──entryability
 │  │  └──EntryAbility.ts         // 程序入口类
 |  ├──pages
 │  │  └──casefactory.ts          // 测试用例工厂类，用于用例名与对应代码的映射
 |  |  └──Index.ets               // 主界面代码入口
 |  |  └──myxnode.ts              // 测试框架代码
 |  |  └──testbase.ts             // 测试用例基类
 │  |──testcase                   // 页面文件
 │  |  └──interface             
 |  |     └──xxx.ts               // 性能和功能测试用例代码
 |  |──utils
 |     └──xxx.ts                  // 测试辅助类
 |
```