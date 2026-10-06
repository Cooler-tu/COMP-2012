# 题目要求检查记录

检查日期：2026-10-06。依据是提供的作业要求和当前保存的六个实现文件及头文件。
这是一份检查报告，没有修改作业源码。以下测试通过不表示已证明所有输入均正确。

## 已确认的问题

### 1. [P1] 第一次入库会读取未初始化的指针（Task 2.2）

位置：`Shop.cpp:14`、`Shop.cpp:61`。

`new Product*[2]` 没有初始化元素。第一次入库时循环不执行，随后判断
`inventory[0] == nullptr`，读取了未初始化值。如果它恰巧非空，就不会保存新商品，
但数量仍增加。后续打印、移除或析构会使用无效指针。

验证：测试专用分配器将数组填为非零字节，第一次入库后的移除在 `Product.cpp:23`
触发 UBSan 无效指针诊断。普通样例恰好通过不能说明这处安全。
修正方向：空库存插入不能依赖未使用槽位原先的内容；或初始化数组，并保证扩容后同样成立。

### 2. [P1] 删除已与 Shop 脱离的评分会崩溃（Task 3.6）

位置：`Customer.cpp:141`。

`Customer::delete_rating()` 无条件调用 `now->shop->remove_rating(...)`。
若 Shop 已删除，或者已执行 `Shop::remove_rating()`，`now->shop` 就是空指针。
题目明确要求无论先前连接状态如何，都能删除评分。

验证：两条路径均在这行触发 UBSan 空指针调用。
修正方向：有 Shop 时才从 Shop 链表移除；无 Shop 时仍须正常从 Customer 链表移除并释放。

### 3. [P1] 无平台的成功订单会调用空 Platform（Task 4.1）

位置：`Shop.cpp:206`。

结算时无条件执行 `platform->change_balance(...)`。Test 38 的平台就是空指针。
当前 Test 38 提前被另一个错误拒绝，遮住了这一处。

验证：库存放一个要买的商品和一个额外商品，绕过末尾误判后，在这行触发 UBSan。
修正方向：平台余额、平台消费记录、VIP 查询只能在有平台时处理。

### 4. [P2] 一次移除可能移除多个相同 ID 商品（Task 2.2、4.1）

位置：`Product.cpp:22`。

`Shop::remove_product()` 找到商品后继续循环。数组左移之后，还可能遇到另一个匹配 ID。
库存有三个 ID 5 的商品时，调用一次后数量从 3 变为 1，而不是 2。
返回指针也会被后一次匹配覆盖，前面移除的商品可能泄漏。

验证：独立移除测试及重复 ID 订单测试均失败。
修正方向：一次调用只移除一个对象，并返回那个对象的指针。

### 5. [P2] 买到库存末尾的合法订单被拒绝（Task 4.1）

位置：`Shop.cpp:178`。

`pos == num_of_inventory` 被无条件认定为失败。订单全部匹配后，库存游标也可能刚好到末尾。
应区分“订单已经处理完”和“订单未处理完但库存已耗尽”。

验证：公开 Test 38 以及补测 `order_last_product` 均断言失败。

### 6. [P2] 订单被隐式要求按 ID 升序（Task 4.1）

位置：`Shop.cpp:168`、`Shop.cpp:193`。

题目没有保证订单有序，并要求商品按订单给出的顺序转移。当前 `pos` 只向前走。
库存包含 0、1、2、3 时，合法订单 `{2, 0}` 会被拒绝。

验证：`order_unsorted` 失败。
修正方向：支持任意订单顺序、重复 ID 和重复数量；不要通过排序订单破坏转移顺序。

### 7. [P2] Shop 和 Platform 的订单收入反了（Task 4.1）

位置：`Shop.cpp:185`、`Shop.cpp:189`、`Shop.cpp:205`。

有平台时 `rev` 实际计算的是平台抽成，后面却加给 Shop，把剩余金额加给 Platform。

验证：普通客户订单 $100、抽成率 20%，应为 Shop $80、Platform $20，实际为 $20/$80。
VIP 订单原价 $129、折扣 0.5、抽成率 0.2，实际费用 $64、平台抽成 $6，
应为 Shop $58、Platform $6，实际为 $6/$58。Customer 扣费 $64 正确。

修正方向：Platform 收抽成，Shop 收实际费用减抽成。现有 VIP 两次截断的计算次序本身正确。

### 8. [P2] 成功交易没有更新平台消费记录（Task 4.1）

位置：`Shop.cpp:204`。

整个 `handle_order()` 没有调用 `update_customer_data()`。
客户实际购买后不会因此成为 VIP，平台打印的消费记录也缺失。

验证：门槛 $50，非 VIP 客户买 $60 商品后仍不是 VIP。
修正方向：只在成功交易时累加实际支付金额；本单的 VIP 状态要在更新前确定。

### 9. [P2] 脱离 Shop 后再次评分会创建新 Rating（Task 3.2）

位置：`Customer.cpp:68`。

用 `now->shop == shop` 查找已有评分，无法找到 `shop` 已设为空的旧评分。
重新评分会新建节点，Customer 链表留下同一 Shop 的两条评分，原 rating_ID 没有保留。

验证：评分 → Shop 移除评分 → Customer 再评分，返回的头节点不再是原对象。
修正方向：用保留的 `shop_ID` 识别旧评分，恢复它的 Shop 连接，更新同一对象并移动到两边链表头。

### 10. [P2] Platform 析构没有释放 CustomerData（Task 1.6）

位置：`Platform.cpp:16`。

析构只释放 Shop 和 shops 数组，没有遍历释放 `customer_data`。
客户数据功能虽然输出正确，留下未删除记录的平台仍会泄漏。

验证：建立两个消费记录后销毁平台，测试分配计数显示仍有两个对象未释放。
该验证是测试专用分配统计，不是 Linux LeakSanitizer 的评分结果。

### 11. [P2] 月底删除 Shop 后跳过下一家（Task 4.2）

位置：`Platform.cpp:156`、`Platform.cpp:165`、`Platform.cpp:170`。

删除会使 shops 左移，而循环仍执行 `i++`，跳过刚移到当前下标的 Shop。
无法付款和低评分删除两条路径都有问题。

验证：手续费 $10，三家余额依次 $5/$100/$100，应收到 $25，实际只收到 $15，
第二家余额仍是 $100。三家连续无法付款，以及两家连续低评分也未全部删除。
修正方向：删除后继续检查当前下标，只有保留当前 Shop 时才推进。

### 12. [P2] 月底同分排序缺少 Shop ID 的规则（Task 4.2）

位置：`Platform.cpp:174`。

只比较平均分，没有处理平均分相等时较小 shop_ID 在前的要求。
无评分 Shop 的平均分都是 -1，也适用这条规则。

验证：按 9、2、5 注册三家无评分 Shop，月底仍按 9、2、5 输出，应为 2、5、9。

## 逐项检查范围

- Task 1.1：Product 普通构造和拷贝构造符合已检查要求。
- Task 1.2：Customer 初始属性、产品释放、评分析构连接符合已检查要求。
- Task 1.3：CustomerData 初始属性符合要求。
- Task 1.4：Shop 属性和无平台构造正确；库存首插存在问题 1，评分析构已通过混合生命周期补测。
- Task 1.5：Rating ID 递增、固定 ID 保存、分数截断、next 初始化符合已检查要求。
- Task 1.6：Platform 属性和 Shop 释放正确；CustomerData 释放存在问题 10。
- Task 2.1：空指针、重复指针拒绝，允许不同对象使用相同 ID，扩容和追加顺序已检查。
- Task 2.2：排序插入、扩容、来源 ID、不缩容量已检查；存在问题 1、4。
- Task 2.3：空指针/错误平台/重复注册拒绝，扩容，删除头/中/尾和不存在 ID 已检查。
- Task 2.4：空链表、头/中/尾插入删除、累加及负数消费已检查。
- Task 2.5：无记录、低于/等于/高于门槛及负消费已检查。
- Task 2.6：客户 ID 升序打印、总消费和 VIP 数量已检查。
- Task 3.1：空评分/错误 Shop ID 拒绝、已有头节点以及中尾节点移动已检查。
- Task 3.2：购买来源判断、空 Shop、分数截断、普通更新和 ID 保留已检查；脱离后更新存在问题 9。
- Task 3.3：两边输出参数、链表遍历和删除标记已检查。
- Task 3.4：负索引、空链表、头节点、多节点以及越界返回已检查。
- Task 3.5：无评分 -1、整数平均分、移除后平均分已检查。
- Task 3.6：Shop 移除头/中/尾，Customer 删除连接中的头/中/尾，以及交错析构已检查；存在问题 2。
- Task 4.1：输入拒绝、空库存、缺货/余额不足时状态不变已检查；存在问题 3–8。
- Task 4.2：部分余额收取、余额等于费用、3/4 条评分和平均分 14/15 边界已检查；存在问题 11、12。
- 编译限制：六个提交文件通过 `-std=c++17 -Werror=vla` 编译；未使用 auto/decltype，未发现新增全局/static 变量。

## 实际验证结果与限制

- 普通公开测试：43/44 通过；Test 38 在 `main.cpp:554` 断言失败。
- 独立要求补测：33 项中 16 项通过、17 项失败；失败对应上述 12 类问题。
- 补测启用了 UBSan，遇到未定义行为直接停止，并且每个测试有运行超时。
- 测试专用分配器默认清零新数组，以隔离问题 1；仅 `inventory_uninitialized` 用非零字节填充。
  因此其他补测的通过不能用于排除未初始化读取，也不能当作原程序已通过完整 ASan 检查。
- AddressSanitizer 测试版在当前环境 5 秒内没有完成，被限时终止；未完成整套 ASan/LeakSanitizer 检查。
- 不声称这些检查证明了所有输入都正确；上述问题都是对合法调用路径的实际复现或明确源码遗漏。
- 未修改六个实现文件、头文件、main.cpp 或给定测试。

## 复现补测

独立测试源码位于 `/tmp/pa1_requirement_checks.cpp`；输出位于 `/tmp/pa1_requirement_results.txt`。
这些是外部检查文件，不属于应提交的六个文件。测试专用全局变量和额外标准库只出现在检查程序里。

在项目目录重新编译：

```bash
g++ -std=c++17 -Werror=vla -g -O0 -fsanitize=undefined -fno-sanitize-recover=all -I . /tmp/pa1_requirement_checks.cpp Customer.cpp Shop.cpp Platform.cpp Product.cpp CustomerData.cpp Rating.cpp -o /tmp/pa1_requirement_checks
```

例如独立运行：

```bash
/tmp/pa1_requirement_checks delete_removed_rating
/tmp/pa1_requirement_checks remove_duplicate_ids
/tmp/pa1_requirement_checks order_regular_revenue
/tmp/pa1_requirement_checks month_skip_fee
```

检查记录对应的源码 SHA-1：

```text
491fdb424d0245ba127f89ffcea6d9a4bb557b54 Customer.cpp
2e2a717c85a61580fb12fbefd9b907b43a333895 Shop.cpp
8f252e792592ea29afc09398296abe1a2a5cc842 Platform.cpp
2919dae450dade8f219ad639ed060f0ec5522de1 Product.cpp
d5aac4aee90ede37ba08979fbbc46b2e842e8b78 CustomerData.cpp
f59b48d22cadebeccc8957a217a901d35652ff63 Rating.cpp
```
