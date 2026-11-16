# 作业要求复查记录（完整内存检查已完成）

日期：2026-10-07（香港时间）。检查对象为当前保存的六个实现文件。本次没有修改作业源码。

## 结论

当前版本通过 **44/44 公开测试**和 **34/34 要求补测**，包括本轮完成的 ASan、UBSan 和 LeakSanitizer 检查，未检出内存错误或泄漏。

## 实际结果

- 普通公开测试：44/44 通过，正常退出，输出与给定文件逐字节一致。
- ASan + UBSan + LeakSanitizer 同时开启的公开测试：**44/44 通过**；输出一致，退出码为 0，标准错误为空。
- ASan + UBSan + LeakSanitizer 同时开启的要求补测：**34/34 通过**；所有断言通过，退出码为 0，标准错误为空。
- macOS 原生 `leaks --atExit` 独立检查公开测试：**44/44 通过**，每项报告 `0 leaks for 0 total leaked bytes`。
- 上一轮 UBSan/测试专用分配计数补测：34/34 通过，场景结束后 scalar/array 分配计数差值全部为 0。

## 之前的超时原因与解决方式

旧 Apple clang 17 的 ASan 运行库在初始化阶段卡住，尚未进入作业程序的 main。将测试 1 等待时间延长至 90 秒仍未结束；一个仅打印一句话的最小程序也出现同样现象。

进程采样显示调用栈停在 ASan 的 `InitializeShadowMemory` → `get_dyld_hdr` → 内存分配 → 再次进入 ASan 初始化 → `StaticSpinMutex::LockSlow`。本地证据确认这次卡住发生在检测工具启动过程中。

在 `/tmp/pa1_memory/llvm22` 创建了配套的 clang 22.1.8 与 compiler-rt 22.1.8 临时环境，并用它重新编译当前源码。系统编译器、Makefile、六个实现文件均未更改。

未关闭检测，也未添加抑制规则；运行时显式设置：

```text
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1
```

还运行了两个独立的故意出错程序作为正向验证：堆数组越界被 ASan 检出，故意遗失的 101 字节分配被 LeakSanitizer 检出。这些测试程序只存在于 /tmp，不属于作业提交文件。

## 补测内容与范围

检查覆盖库存头插、降序和重复 ID 入库、数组扩容、空库存与不存在 ID 移除；订单输入拒绝、缺货及余额不足时状态不变、原始订单转移顺序、重复数量、库存末尾购买；无平台/普通/VIP 收入、舍入、整单统一 VIP 状态和消费记录；月底连续删除、低评分阈值、保留正确店铺与同分排序；评分更新、重排、脱离重连、双侧删除与交错析构；CustomerData 插入、更新、删除、VIP 边界与清理。

本轮 sanitizer 补测移除了之前的自定义 new/delete 分配器，没有清零或填充分配内容，没有依靠分配计数替代 sanitizer。最初用于非零填充的首次入库场景本轮作为普通首次入库场景运行；ASan/LSan 本身不用于证明所有未初始化读取均不存在。

未发现新增 include、auto/decltype、VLA 或额外全局/static 变量。普通编译仍有未使用变量及初始化列表顺序警告。测试通过说明已覆盖场景正确，不是对所有可能输入的形式化证明。

## 可复现文件

- 完整 sanitizer 驱动：`/tmp/pa1_memory/run_sanitizers.py`。
- 公开测试二进制：`/tmp/pa1_memory/shopping_asan22`。
- 无自定义分配器的补测源码：`/tmp/pa1_memory/requirements_no_hooks.cpp`。
- 补测二进制：`/tmp/pa1_memory/requirements_asan22`。
- 完整结果：`/tmp/pa1_memory/sanitizer_summary.json`。
- 逐项标准输出/错误：`/tmp/pa1_memory/public_XX.out`、`public_XX.err`、`extra_TEST.out`、`extra_TEST.err`。
- 原生泄漏驱动与汇总：`/tmp/pa1_memory/run_native_leaks.py`、`native_leaks_summary.json`。
- 工具启动卡住的进程采样：`/tmp/pa1_memory/asan_sample.txt`。
- 故意出错验证：`/tmp/pa1_memory/overflow_control.err`、`leak_control.err`。

检查对应源码 SHA-1（与上一轮最终保存版一致）：

```text
c3eec4fdfee5eb2713edf23759cb319a785b9ac9  Customer.cpp
032793d57c76e389f9c043d6dbecc1f3d7c2ccba  Shop.cpp
fd31b4426b9c19f1df5aaf1042587b504b43d4b8  Platform.cpp
f42068e5869edd0e66264fe1fbefbab3b14e320a  Product.cpp
d5aac4aee90ede37ba08979fbbc46b2e842e8b78  CustomerData.cpp
f59b48d22cadebeccc8957a217a901d35652ff63  Rating.cpp
```
