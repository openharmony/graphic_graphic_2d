**Description:**

**Issue number:**

**Test & Result:**

**CodeCheck:**
<table>
    <tr>
        <th>类型</th><th>自检项</th><th>自检结果</th>
    </tr>
    <tr>
        <td rowspan="2">多线程相关</td><td>在类的成员变量中定义了vector/map/list等容器类型，且在多个成员函数中有操作时，需要加锁保护</td><td>自检结果：</td>
    </tr>
    <tr>
        <td>定义全局变量，在多个函数中都有操作时，需要加锁保护</td><td>自检结果：</td>
    </tr>
    <tr>
        <td rowspan="4">内存相关</td><td>调用外部接口时，确认是否对返回值做了判断，尤其外部接口返回了nullptr的情况，避免进程崩溃</td><td>自检结果：</td>
    </tr>
    <tr>
        <td>调用安全函数时，如memcpy_s等，是否检查其返回值</td><td>自检结果：</td>
    </tr>
    <tr>
        <td>检查函数中是否涉及了内存或资源申请（如文件句柄），注意每个异常退出流程，是否都已经将资源释放（推荐使用RAII）</td><td>自检结果：</td>
    </tr>
    </tr>
    <tr>
        <td>隐式内存分配场景：realpath、ReadParcelable序列化、cJSON相关函数时等，需主动释放或使用智能指针</td><td>自检结果：</td>
    </tr>
    <tr>
        <td rowspan="4">校验外部输入</td><td>使用nlohmann:json解析外部输入时，需判断参数类型是否符合预期</td><td>自检结果：</td>
    </tr>
    <tr>
        <td>所有外部输入均不可信，需判断外部输入是否直接作为内存分配的大小，数组下标、循环条件、SQL查询等</td><td>自检结果：</td>
    </tr>
    <tr>
        <td>外部输入的路径不可信，需使用realpath做标准化处理，并判断路径的合法性</td><td>自检结果：</td>
    </tr>
    <tr>
        <td>外部输入包括对外提供的接口，IPC的proxy/stub接口，序列化/反序列化接口等</td><td>自检结果：</td>
    </tr>
    </tr>
    <tr>
        <td rowspan="2">数学运算</td><td>代码中是否混合了加减乘除等运算，需检查是否可能导致整数溢出或符号翻转</td><td>自检结果：</td>
    </tr>
    <tr>
        <td>需检查代码是否有高精度数字转换为低精度的操作，如果必须，建议使用C++安全类型转换接口</td><td>自检结果：</td>
    </tr>
    </tr>
     <tr>
        <td rowspan="1">秘钥相关</td><td>如变量临时保存了口令、秘钥等，需要在使用完成后及时清空（内存memset掉）</td><td>自检结果：</td>
    </tr>
    <tr>
        <td rowspan="2">权限相关</td><td>作为系统服务对外提供了接口，是否做了权限保护和校验（如需要），只允许申请了权限的应用访问</td><td>自检结果：</td>
    </tr>
    <tr>
        <td>内核对外提供了设备节点，是否做了权限保护，只允许特定的系统服务访问</td><td>自检结果：</td>
    </tr>
    </tr>
    <tr>
        <td rowspan="3">内核操作</td><td>如有mmap操作，并使用remap_pfn_range进行地址映射时，校验起始地址是否是用户态输入且没有做合法性校验</td><td>自检结果：</td>
    </tr>
    <tr>
        <td>是否有copy_from_user，并对外部输入的数据做了长度校验，以防止缓冲区溢出</td><td>自检结果：</td>
    </tr>
    <tr>
        <td>是否有使用copy_to_user，并在返回到用户态时，对数据做了完整初始化，或使用memset清空后再赋值</td><td>自检结果：</td>
    </tr>
    </tr>
</table>
