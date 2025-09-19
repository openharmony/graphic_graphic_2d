**Description:**

**Issue number:**

**Test & Result:**

**CodeCheck:**
<table>
  <thead>
    <tr>
      <th>类型</th>
      <th>自检项</th>
      <th>自检结果</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td rowspan="2"><strong>多线程</strong></td>
      <td>在类的成员变量中定义了vector/map/list等容器类型，且在多个成员函数中有操作时，需要加锁保护</td>
      <td>自检结果：</td>
    </tr>
    <tr>
      <td>定义全局变量，在多个函数中都有操作时，需要加锁保护</td>
      <td>自检结果：</td>
    </tr>
    <tr>
      <td rowspan="4"><strong>内存操作</strong></td>
        <td>调用外部接口时，确认是否对返回值做了判空判断，尤其外部接口返回了nullptr的情况，避免进程崩溃</td>
        <td>自检结果：</td>
    </tr>
    <tr>
        <td>内存操作优先使用安全函数，并检查其返回值</td>
        <td>自检结果：</td>
    </tr>
    <tr>
        <td>注意每个异常退出流程，是否都已经将资源释放（推荐使用RAII）</td>
        <td>自检结果：</td>
    </tr>
    <tr>
        <td>隐式内存分配场景：realpath、ReadParcelable序列化、cJSON相关函数时等，需主动释放或使用智能指针</td>
        <td>自检结果：</td>
    </tr>
    <tr>
      <td rowspan="3"><strong>外部输入</strong></td>
        <td>所有外部输入均不可信，需判断外部输入是否直接作为内存分配的大小，数组下标、循环条件、SQL查询等</td>
        <td>自检结果：</td>
    </tr>
    <tr>
        <td>注意外部字符串数据有无尾0</td>
        <td>自检结果：</td>
    </tr>
    <tr>
        <td>外部输入的路径不可信，需使用realpath做标准化处理，并判断路径的合法性</td>
        <td>自检结果：</td>
    </tr>
    <tr>
      <td><strong>敏感信息</strong></td>
        <td>注意日志中打印敏感信息需匿名化</td>
        <td>自检结果：</td>
    </tr>
    <tr>
      <td><strong>数学运算</strong></td>
        <td>代码中是否混合了加减乘除等运算，需检查是否可能导致整数溢出或符号翻转</td>
        <td>自检结果：</td>
    </tr>
    <tr>
      <td><strong>初始化</strong></td>
        <td>类成员、局部变量使用前需初始化</td>
        <td>自检结果：</td>
    </tr>
    <tr>
      <td><strong>权限管理</strong></td>
        <td>作为系统服务对外提供了接口（或RSCmd），是否做了权限保护和校验，只允许申请了权限的应用访问</td>
        <td>自检结果：</td>
    </tr>
  </tbody>
</table>

### L0新增用例自检结果
- [ ] 是，有新增L0用例，且完成自检
- [ ] 否
