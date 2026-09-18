# TỪ ĐIỂN CÚ PHÁP C VÀ C++ CHO LẬP TRÌNH HỆ THỐNG VÀ LẬP TRÌNH GAME

*Gặp một dòng code "khó hiểu" — tra ở đây để biết từng ký hiệu làm gì*

---

## 0. Cách dùng tài liệu này

### 0.1 Cấu trúc

| Phần | Chương | Nội dung |
|---|---|---|
| **Bảng giải mã nhanh** | 0.3 | Mỗi **ký hiệu** (`*`, `&`, `->`, `::`, `[[ ]]`, `...`) có thể mang những nghĩa nào |
| **I. C** | 1–7 | Tiền xử lý, kiểu và từ khóa, toán tử, **đọc khai báo phức tạp**, struct/union, hàm, **thành ngữ trong code hệ thống** |
| **II. C++** | 8–14 | Tham chiếu và `auto`, lớp, template, lambda và ép kiểu, thư viện chuẩn, đa luồng và coroutine, **cú pháp trong code game** |
| **III. Giải mã code thật** | 15–17 | Đọc từng dòng khó trong `net.h`, chat server C, và chat client C++ (từ tài liệu lập trình socket) |
| **Phụ lục** | A–E | Độ ưu tiên toán tử, chuỗi định dạng `printf`, khác biệt C/C++, công cụ, cách luyện đọc code |

Mỗi mục có **bảng tra** và **chương trình chạy được** kèm kết quả. Cuối mỗi phần có **bài luyện đọc nhanh**.

### 0.2 Về độ chính xác

> ✅ Mọi chương trình trong tài liệu đã được biên dịch bằng **GCC 13** với `-Wall -Wextra -pedantic`, chạy kèm AddressSanitizer/UBSan trên Linux, và phần "Kết quả" là output thật.
> - C dùng chế độ `-std=gnu17`; C++ dùng `-std=c++20` (riêng ví dụ `std::expected` dùng `-std=c++23`).
> - Ý nghĩa của các **khai báo phức tạp** không chỉ được giải thích bằng lời: chúng được kiểm tra bằng `_Static_assert` — nếu giải thích sai, chương trình **không biên dịch được**.
> - Các dòng code ở Phần III được **trích nguyên văn** từ mã nguồn đã chạy thật trong tài liệu lập trình socket.
> - Một số cú pháp là **phần mở rộng** của GCC/Clang (`__attribute__`, `__builtin_expect`, `__typeof__`) — đều được ghi chú. Phần SIMD dùng lệnh của CPU x86.

### 0.3 Bảng giải mã ký hiệu nhanh

Khi bối rối, hãy tra ký hiệu ở đây trước. Cùng một ký hiệu có thể mang **nhiều nghĩa khác nhau** tùy vị trí.

| Ký hiệu | Ngữ cảnh | Nghĩa | Ví dụ |
|---|---|---|---|
| `*` | Trong **khai báo** | "là con trỏ tới" | `int *p;` |
| | Trước **biểu thức** | **Giải tham chiếu**: lấy giá trị tại địa chỉ | `*p = 5;` |
| | Giữa hai giá trị | Phép nhân | `a * b` |
| `&` | Trước **biểu thức** | **Lấy địa chỉ** | `&x` |
| | Trong **khai báo** (C++) | "là **tham chiếu** tới" | `int& r = x;` |
| | Giữa hai giá trị | AND từng bit | `flags & MASK` |
| `&&` | Giữa hai điều kiện | AND logic | `a && b` |
| | Trong **khai báo** (C++) | **Tham chiếu rvalue** (nhận giá trị tạm) | `void f(std::string&& s);` |
| | `T&&` / `auto&&` với kiểu được suy luận | **Forwarding reference** (nhận mọi thứ) | `template<class T> void g(T&& x);` |
| `.` | Sau **đối tượng** | Truy cập thành viên | `player.hp` |
| `->` | Sau **con trỏ** | Truy cập thành viên qua con trỏ: `p->hp` ≡ `(*p).hp` | `ptr->hp` |
| | Sau `)` của hàm/lambda | **Kiểu trả về đặt sau** | `auto f() -> int` |
| `::` | | **Phạm vi**: thuộc namespace/lớp nào | `std::vector`, `Texture::created()` |
| | Đứng đầu | Phạm vi **toàn cục** (thường để gọi hàm C) | `::send(...)` |
| `< >` | Sau tên template | **Tham số template** | `std::vector<int>` |
| | Trong `#include` | Header của hệ thống/thư viện | `#include <stdio.h>` |
| `" "` | Trong `#include` | Header của dự án (tìm trong thư mục hiện tại trước) | `#include "net.h"` |
| `#` | Đầu dòng | Lệnh **tiền xử lý** | `#define`, `#ifdef` |
| | Trong macro | Biến tham số thành **chuỗi** | `#x` |
| `##` | Trong macro | **Dán** hai token | `get_##name` |
| `...` | Tham số hàm | **Số tham số thay đổi** | `int printf(const char*, ...);` |
| | Trong template | **Gói tham số** (pack) | `template<typename... Args>` |
| | Sau biểu thức | **Mở rộng** gói tham số | `f(args...)`, `(args + ...)` |
| `[ ]` | Khai báo / chỉ số | Mảng | `int a[10]; a[3]` |
| | Đầu biểu thức (C++) | **Danh sách bắt biến** của lambda | `[&](int x) { ... }` |
| `[[ ]]` | C++11+, C23 | **Thuộc tính** (gợi ý cho trình biên dịch) | `[[nodiscard]]` |
| `{ }` | Sau tên biến | **Khởi tạo** | `int x{5}; Vec2 v{1, 2};` |
| `( )` | Trước biểu thức | **Ép kiểu** kiểu C | `(int)3.7` |
| `~` | Trước tên lớp (C++) | **Hàm hủy** | `~Texture()` |
| | Trước biểu thức | Đảo tất cả các bit | `~MASK` |
| `!` | Trước biểu thức | Phủ định logic; `!!x` biến x thành 0 hoặc 1 | `!ok` |
| `? :` | | Toán tử điều kiện: `đk ? a : b` | `hp > 0 ? "song" : "chet"` |
| `:` | Sau tên trường | **Bit-field** (số bit) | `unsigned alive : 1;` |
| | Sau tên lớp | **Kế thừa** | `class Circle : public Shape` |
| | Sau `)` của constructor | **Danh sách khởi tạo** thành viên | `Foo() : x_(0) {}` |
| | Trong `for` (C++) | **Range-for**: "với mỗi phần tử trong" | `for (auto& e : v)` |
| | Sau `enum class X` | **Kiểu nền** | `enum class Layer : uint8_t` |
| | Sau nhãn | `case`, `default`, nhãn của `goto`, `public:` | `cleanup:` |
| `= 0` | Cuối khai báo hàm ảo | Hàm **thuần ảo** | `virtual void f() = 0;` |
| `= default` / `= delete` | Cuối khai báo hàm đặc biệt | Dùng bản mặc định / **cấm** dùng | `T(const T&) = delete;` |
| `<<` `>>` | Với số nguyên | **Dịch bit** | `1u << 3` |
| | Với luồng (`std::cout`) | Ghi / đọc | `std::cout << x` |
| `<=>` | C++20 | So sánh "3 chiều" | `auto operator<=>(const T&) const = default;` |
| `'` | Trong số (C++14, C23) | Dấu phân cách chữ số, bị bỏ qua | `1'000'000` |
| `\` | Cuối dòng | **Nối dòng** (cả trong chú thích!) | macro nhiều dòng |
| `0x` `0b` `0` | Đầu số | Hex / nhị phân / **bát phân** | `0xFF`, `0b1010`, `010` (= 8!) |
| `u` `l` `ll` `f` | Cuối số | Không dấu / long / long long / float | `1u`, `5ULL`, `0.5f` |
| `L"" u8"" R"()"` | Tiền tố chuỗi | Chuỗi rộng / UTF-8 / **chuỗi thô** (C++) | `R"(C:\path)"` |
| `_t` | Cuối tên kiểu | Quy ước cho **kiểu** | `size_t`, `int32_t`, `socket_t` |
| `__tên__` | | Tên dành cho **trình biên dịch/thư viện** | `__func__`, `__attribute__` |

---

# PHẦN I — C

## Chương 1. Tiền xử lý

Trước khi biên dịch, **bộ tiền xử lý** chạy qua file và thực hiện mọi dòng bắt đầu bằng `#`. Nó chỉ **thay thế văn bản** — không hiểu kiểu dữ liệu hay cú pháp C.

### 1.1 Bảng tra

| Cú pháp | Tác dụng |
|---|---|
| `#include <file.h>` / `#include "file.h"` | Chèn nội dung file |
| `#define NAME value` | Macro hằng: thay `NAME` bằng `value` |
| `#define F(x) ((x) * 2)` | Macro "hàm" — **bọc ngoặc** từng tham số và cả biểu thức |
| `#x` | Biến tham số `x` thành chuỗi `"x"` |
| `a##b` | Dán hai token thành một |
| `#define LOG(fmt, ...) ... __VA_ARGS__` | Macro nhận số tham số thay đổi |
| `#undef NAME` | Hủy định nghĩa |
| `#if` / `#elif` / `#else` / `#endif` | Biên dịch có điều kiện |
| `#ifdef X` ≡ `#if defined(X)`; `#ifndef X` | Có / không có định nghĩa `X` |
| `#ifndef H` `#define H` ... `#endif` | **Include guard** — chống include hai lần |
| `#pragma once` | Include guard ngắn gọn (không thuộc chuẩn nhưng mọi trình biên dịch lớn đều hỗ trợ) |
| `#pragma ...` | Lệnh riêng của trình biên dịch (ví dụ `#pragma pack`) |
| `#error "thong bao"` | Dừng biên dịch với thông báo lỗi |
| `_Static_assert(expr, "msg")` (C11); `static_assert` (macro trong `<assert.h>` từ C11, từ khóa từ C23 và trong C++) | Kiểm tra điều kiện **lúc biên dịch** |
| `__FILE__`, `__LINE__`, `__func__` | Tên file, số dòng, tên hàm hiện tại |
| `_WIN32`, `__linux__`, `__APPLE__` | Macro cho biết đang biên dịch cho nền tảng nào |
| `__cplusplus` | Có định nghĩa khi biên dịch bằng trình biên dịch C++ |
| `NDEBUG` | Khi được định nghĩa, `assert()` bị tắt (thường có trong bản release) |

### 1.2 Chương trình minh họa

```c
#include <stdio.h>

/* 1) Macro hằng và macro "hàm" — nhớ bọc ngoặc */
#define MAX_PLAYERS 8
#define SQUARE(x) ((x) * (x))
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/* 2) # biến tham số thành chuỗi; ## dán hai token thành một */
#define SHOW_INT(expr) printf("%s = %d\n", #expr, (expr))
#define MAKE_GETTER(field) int get_##field(void) { return field; }

/* 3) Macro nhận số tham số thay đổi */
#define LOG(fmt, ...) printf("[LOG] " fmt "\n", __VA_ARGS__)

/* 4) do { ... } while (0): để macro nhiều lệnh dùng an toàn sau if/else */
#define SWAP_INT(a, b) do { int tmp_ = (a); (a) = (b); (b) = tmp_; } while (0)

/* 5) X-macro: viết danh sách MỘT lần, sinh ra cả enum lẫn mảng tên */
#define WEAPON_LIST(X) X(SWORD) X(BOW) X(STAFF)
#define AS_ENUM(name) WEAPON_##name,
#define AS_STRING(name) #name,
enum Weapon { WEAPON_LIST(AS_ENUM) WEAPON_COUNT };
static const char *weapon_names[] = { WEAPON_LIST(AS_STRING) };

/* 6) Biên dịch có điều kiện theo nền tảng */
#if defined(_WIN32)
#  define PLATFORM "Windows"
#elif defined(__linux__)
#  define PLATFORM "Linux"
#elif defined(__APPLE__)
#  define PLATFORM "macOS"
#else
#  define PLATFORM "khac"
#endif

static int health = 75;
MAKE_GETTER(health)          /* sinh ra: int get_health(void) { return health; } */

_Static_assert(MAX_PLAYERS <= 16, "qua nhieu nguoi choi");   /* kiểm tra lúc biên dịch */

int main(void) {
    int scores[5] = {0};
    SHOW_INT(SQUARE(1 + 2));
    printf("ARRAY_SIZE(scores) = %zu\n", ARRAY_SIZE(scores));
    printf("get_health() = %d\n", get_health());
    LOG("nguoi choi %d co %d mau", 1, health);

    int a = 1, b = 2;
    if (a < b)
        SWAP_INT(a, b);      /* nhờ do-while(0), dấu ; ở đây hợp lệ và else bên dưới vẫn đúng */
    else
        printf("khong doi\n");
    printf("sau SWAP_INT: a = %d, b = %d\n", a, b);

    for (int i = 0; i < WEAPON_COUNT; i++) printf("vu khi %d: %s\n", i, weapon_names[i]);
    printf("ham hien tai: %s, dong: %d, nen tang: %s\n", __func__, __LINE__, PLATFORM);
#ifndef NDEBUG
    printf("ban DEBUG (NDEBUG chua duoc dinh nghia)\n");
#endif
    return 0;
}
```

**Kết quả:**

```text
SQUARE(1 + 2) = 9
ARRAY_SIZE(scores) = 5
get_health() = 75
[LOG] nguoi choi 1 co 75 mau
sau SWAP_INT: a = 2, b = 1
vu khi 0: SWORD
vu khi 1: BOW
vu khi 2: STAFF
ham hien tai: main, dong: 56, nen tang: Linux
ban DEBUG (NDEBUG chua duoc dinh nghia)
```

**Giải thích các mẫu khó hiểu:**

- **`do { ... } while (0)`**: không phải vòng lặp thật (chỉ chạy đúng một lần). Nó biến nhiều lệnh thành **một lệnh duy nhất** cần dấu `;` phía sau — nhờ đó `if (...) SWAP_INT(a, b); else ...` vẫn đúng cú pháp. Nếu chỉ dùng `{ ... }`, dấu `;` sau macro sẽ làm `else` bị lỗi.
- **X-macro** (`WEAPON_LIST(X)`): danh sách được viết **một lần**, rồi "áp" các macro khác nhau lên nó để sinh ra cả `enum` lẫn mảng tên. Thêm vũ khí mới chỉ cần sửa một chỗ — rất hay gặp trong engine game và trình giả lập.
- **`MAKE_GETTER(health)`** sinh ra cả một hàm. Khi đọc code dùng nhiều macro, hãy xem kết quả sau tiền xử lý bằng `gcc -E file.c`.

### 1.3 Bẫy: dấu `\` ở cuối chú thích

`\` cuối dòng **nối dòng kế tiếp** vào dòng hiện tại — kể cả khi nó nằm trong chú thích `//`:

```c
#include <stdio.h>
int main(void) {
    int x = 1; // chu thich ket thuc bang dau \
    x = 2;
    printf("x = %d\n", x);
    return 0;
}
```

**Kết quả:**

```text
x = 1
```

Dòng `x = 2;` đã bị "nuốt" vào chú thích. GCC có cảnh báo `multi-line comment` (khi bật `-Wall`) — lý do nên luôn bật cảnh báo. *(Chính lỗi này đã xuất hiện khi soạn tài liệu này, trong một chú thích nói về dấu `\`!)*

---

## Chương 2. Kiểu dữ liệu, hằng số và từ khóa

### 2.1 Các kiểu hay gặp trong code hệ thống

| Kiểu | Header | Ý nghĩa | In bằng |
|---|---|---|---|
| `int8_t` … `int64_t`, `uint8_t` … `uint64_t` | `<stdint.h>` | Số nguyên **đúng** N bit | `PRId32`, `PRIu64` (`<inttypes.h>`) |
| `size_t` | `<stddef.h>` | Kích thước, chỉ số (không âm) | `%zu` |
| `ssize_t` | `<sys/types.h>` (POSIX) | Như `size_t` nhưng **có dấu**, để trả về −1 khi lỗi (`read`, `recv`) | `%zd` |
| `ptrdiff_t` | `<stddef.h>` | Hiệu hai con trỏ | `%td` |
| `intptr_t`, `uintptr_t` | `<stdint.h>` | Số nguyên đủ lớn để chứa **một địa chỉ** | `PRIxPTR` |
| `bool` | `<stdbool.h>` (C99; từ khóa trong C23) | Đúng/sai | `%d` |
| `off_t`, `pid_t`, `socklen_t` | POSIX | Vị trí trong file, mã tiến trình, độ dài địa chỉ socket | |
| `wchar_t`, `char16_t`, `char32_t`, `char8_t` | | Ký tự rộng / Unicode | |

### 2.2 Từ khóa bổ nghĩa

| Từ khóa | Ở đâu | Nghĩa |
|---|---|---|
| `const` | Biến | Không được sửa |
| `static` | Biến **cục bộ** | Sống suốt chương trình, chỉ khởi tạo **một lần** |
| | Biến/hàm **toàn cục** | Chỉ thấy được **trong file này** (liên kết nội bộ) |
| | Thành viên lớp (C++) | Thuộc về **lớp**, không thuộc từng đối tượng |
| `extern` | Khai báo | "Biến/hàm này được **định nghĩa ở file khác**" |
| `inline` | Hàm | Cho phép định nghĩa trong header; gợi ý chèn thân hàm tại chỗ gọi |
| `volatile` | Biến | Giá trị có thể thay đổi **ngoài tầm kiểm soát** của đoạn code (thanh ghi phần cứng, biến sửa trong trình xử lý tín hiệu) — trình biên dịch không được tối ưu hóa bỏ việc đọc lại |
| `restrict` (C99) | Con trỏ | Hứa rằng vùng nhớ này **chỉ** được truy cập qua con trỏ này → cho phép tối ưu mạnh hơn (`memcpy` dùng nó) |
| `register` | Biến | Lỗi thời, bỏ qua |
| `_Thread_local` (C11) / `thread_local` | Biến | Mỗi luồng một bản riêng |
| `_Alignas(n)` / `alignas` | Biến, kiểu | Căn lề theo bội số `n` |
| `_Noreturn` (C11) / `[[noreturn]]` | Hàm | Hàm không bao giờ quay về (`exit`, `abort`) |
| `typedef` | | Đặt **tên mới** cho một kiểu |
| `sizeof` | | Kích thước (byte) của kiểu/biến — tính **lúc biên dịch** |

### 2.3 Chương trình minh họa

```c
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static int counter(void) {
    static int n = 0;             /* static cục bộ: sống suốt chương trình, khởi tạo 1 lần */
    return ++n;
}

int main(void) {
    /* Hậu tố và tiền tố của hằng số */
    unsigned u = 10u;
    long long big = 9000000000LL;
    unsigned long long ubig = 18446744073709551615ULL;
    float f = 0.5f;               /* không có f -> double */
    int hex = 0xFF, oct = 010;
    char nl = '\n', letter = 'A', hex_char = '\x41';
    printf("u=%u big=%lld ubig=%llu f=%.1f hex=%d oct=%d nl=%d A=%c/%c\n",
           u, big, ubig, (double)f, hex, oct, nl, letter, hex_char);

    /* Kiểu có kích thước cố định và macro in tương ứng */
    int32_t i32 = -5;
    uint64_t u64 = 1ULL << 40;
    printf("i32=%" PRId32 " u64=%" PRIu64 "\n", i32, u64);

    /* Các kiểu "kích thước" */
    size_t sz = sizeof(int[10]);            /* kích thước không âm */
    ptrdiff_t diff = &"hello"[4] - &"hello"[1];
    printf("size_t=%zu ptrdiff_t=%td\n", sz, diff);

    /* Bẫy: so sánh có dấu với không dấu */
    int neg = -1;
    printf("-1 < 1u ? %s\n", (unsigned)neg < 1u ? "dung" : "SAI (vi -1 bi doi thanh so khong dau rat lon)");

    /* Bẫy: chia nguyên và phần dư với số âm (C99+: làm tròn về 0) */
    printf("-7 / 2 = %d, -7 %% 2 = %d\n", -7 / 2, -7 % 2);

    bool ok = true;
    const double PI = 3.14159;
    printf("ok=%d PI=%.2f\n", ok, PI);
    /* Bẫy: thứ tự tính các tham số của hàm KHÔNG được quy định */
    printf("counter: %d %d %d\n", counter(), counter(), counter());
    return 0;
}
```

**Kết quả:**

```text
u=10 big=9000000000 ubig=18446744073709551615 f=0.5 hex=255 oct=8 nl=10 A=A/A
i32=-5 u64=1099511627776
size_t=40 ptrdiff_t=3
-1 < 1u ? SAI (vi -1 bi doi thanh so khong dau rat lon)
-7 / 2 = -3, -7 % 2 = -1
ok=1 PI=3.14
counter: 3 2 1
```

**Ba bẫy trong kết quả trên:**

1. **So sánh có dấu với không dấu:** khi một vế là `unsigned`, vế kia bị chuyển sang `unsigned`: −1 trở thành 4 294 967 295. Bật `-Wextra` (có `-Wsign-compare`) để được cảnh báo.
2. **Chia số âm:** C làm tròn về 0: `-7 / 2 = -3`, `-7 % 2 = -1`.
3. **Thứ tự tính tham số:** dòng `printf("%d %d %d", counter(), counter(), counter())` in `3 2 1` trên GCC — vì chuẩn C (và C++) **không quy định** thứ tự tính các tham số của hàm. Đừng bao giờ viết code phụ thuộc vào thứ tự đó.

Ngoài ra: `010` là số **bát phân** (= 8), không phải 10.

---

## Chương 3. Toán tử

### 3.1 Bảng tra

| Nhóm | Toán tử | Ghi chú |
|---|---|---|
| Số học | `+ - * / %` | `/` giữa hai số nguyên là **chia nguyên** |
| Tăng/giảm | `++x` `x++` `--x` `x--` | Tiền tố: tăng rồi dùng; hậu tố: dùng rồi tăng |
| So sánh | `== != < > <= >=` | Kết quả là `0`/`1` (C), `false`/`true` (C++) |
| Logic | `&& \|\| !` | **Đoản mạch**: vế phải có thể không được tính |
| Bit | `& \| ^ ~ << >>` | Dùng với số **không dấu** cho an toàn |
| Gán | `= += -= *= /= %= &= \|= ^= <<= >>=` | `a += b` ≡ `a = a + b` |
| Điều kiện | `c ? a : b` | |
| Phẩy | `a, b` | Tính `a`, rồi `b`; kết quả là `b` |
| Con trỏ / thành viên | `* & -> . []` | |
| Kích thước | `sizeof x`, `sizeof(type)`, `_Alignof(type)` | Với biến có thể bỏ ngoặc |
| Ép kiểu | `(type)expr` | |

### 3.2 Chương trình minh họa

```c
#include <stdio.h>

static int calls = 0;
static int touch(int v) { calls++; return v; }

int main(void) {
    unsigned flags = 0;
    enum { F_VISIBLE = 1u << 0, F_SOLID = 1u << 1, F_ENEMY = 1u << 2 };
    flags |= F_VISIBLE | F_ENEMY;             /* bật */
    flags &= ~F_VISIBLE;                      /* tắt */
    flags ^= F_SOLID;                         /* đảo */
    printf("flags = %u, co ENEMY? %s\n", flags, (flags & F_ENEMY) ? "co" : "khong");

    /* Bẫy độ ưu tiên: == mạnh hơn & */
    printf("flags & F_VISIBLE == 0   -> %d (bi hieu la flags & (F_VISIBLE == 0))\n", flags & F_VISIBLE == 0);
    printf("(flags & F_VISIBLE) == 0 -> %d (dung y do: VISIBLE dang tat)\n", (flags & F_VISIBLE) == 0);

    /* Toán tử 3 ngôi và toán tử phẩy */
    int hp = 0;
    const char *state = hp > 0 ? "song" : "chet";
    int x = (touch(1), touch(2), 3);          /* tính lần lượt, lấy giá trị CUỐI */
    printf("state=%s x=%d calls=%d\n", state, x, calls);

    /* Đoản mạch: vế phải không được tính */
    calls = 0;
    if (0 && touch(1)) {}
    if (1 || touch(1)) {}
    printf("calls sau doan mach = %d\n", calls);

    /* sizeof mảng và con trỏ */
    int arr[10];
    int *p = arr;
    printf("sizeof arr = %zu, sizeof p = %zu, so phan tu = %zu\n", sizeof arr, sizeof p, sizeof arr / sizeof arr[0]);

    /* ++ tiền tố và hậu tố */
    int i = 5;
    int a = i++;
    int b = ++i;
    printf("a=%d b=%d i=%d\n", a, b, i);

    /* Dịch bit, lấy bit, đặt bit */
    unsigned v = 0xF0;
    printf("v>>4 = 0x%X, bit 7 = %u, v|1 = 0x%X\n", v >> 4, (v >> 7) & 1u, v | 1u);
    return 0;
}
```

**Kết quả:**

```text
flags = 6, co ENEMY? co
flags & F_VISIBLE == 0   -> 0 (bi hieu la flags & (F_VISIBLE == 0))
(flags & F_VISIBLE) == 0 -> 1 (dung y do: VISIBLE dang tat)
state=chet x=3 calls=2
calls sau doan mach = 0
sizeof arr = 40, sizeof p = 8, so phan tu = 10
a=5 b=7 i=7
v>>4 = 0xF, bit 7 = 1, v|1 = 0xF1
```

Khi biên dịch, GCC cảnh báo đúng dòng bẫy độ ưu tiên:

```text
warning: suggest parentheses around comparison in operand of '&' [-Wparentheses]
```

**Quy tắc:** toán tử bit (`& | ^`) có độ ưu tiên **thấp hơn** so sánh (`== <`). Luôn viết `(flags & MASK) == 0`. (Bảng độ ưu tiên đầy đủ ở Phụ lục A.)

---

## Chương 4. Con trỏ và cách đọc khai báo phức tạp

### 4.1 Quy tắc "phải – trái"

Đây là kỹ năng giải mã quan trọng nhất trong C:

1. Bắt đầu từ **tên biến**.
2. Đọc sang **phải**: gặp `[N]` → "là mảng N phần tử", gặp `(...)` → "là hàm nhận (...)".
3. Gặp `)` hoặc hết → quay sang **trái**: gặp `*` → "là con trỏ tới", gặp kiểu → "... kiểu đó".
4. Ra khỏi cặp ngoặc thì lặp lại bước 2.

**Ví dụ:** `int (*ops[2])(int, int)`

```text
ops                 → ops
ops[2]              → là mảng 2 phần tử
*ops[2]             → mỗi phần tử là con trỏ tới
(*ops[2])(int, int) → hàm nhận (int, int)
int ...             → trả về int
=> "ops là mảng 2 con trỏ tới hàm nhận (int, int) trả về int"
```

### 4.2 Bảng tra

| Khai báo | Đọc là |
|---|---|
| `int *p` | p là con trỏ tới int |
| `int *a[3]` | a là mảng 3 con trỏ tới int |
| `int (*p)[10]` | p là con trỏ tới **mảng** 10 int |
| `int (*fp)(int, int)` | fp là con trỏ tới **hàm** (int, int) trả về int |
| `int (*ops[2])(int, int)` | ops là mảng 2 con trỏ hàm |
| `int (*pick(char))(int, int)` | pick là **hàm** nhận char, trả về con trỏ tới hàm (int, int) → int |
| `void (*handler)(int)` | con trỏ tới hàm nhận int, không trả về gì (kiểu trình xử lý tín hiệu) |
| `char **argv` | con trỏ tới con trỏ tới char (mảng chuỗi) |
| `const int *p` ≡ `int const *p` | con trỏ tới int **hằng** — không sửa `*p`, được đổi `p` |
| `int *const p` | con trỏ **hằng** tới int — sửa được `*p`, không đổi `p` |
| `const int *const p` | cả hai đều hằng |
| `void *p` | con trỏ "không kiểu" — phải ép về kiểu thật trước khi dùng |
| `typedef int (*BinOp)(int, int)` | đặt tên `BinOp` cho kiểu con trỏ hàm → `BinOp pick(char)` dễ đọc hơn nhiều |

### 4.3 Chương trình — trình biên dịch tự xác nhận nghĩa

```c
#include <stdio.h>

/* Trình biên dịch (GCC/Clang) TỰ XÁC NHẬN ý nghĩa của từng khai báo:
   nếu hai kiểu không trùng nhau, chương trình KHÔNG biên dịch được. */
#define SAME_TYPE(a, b) _Static_assert(__builtin_types_compatible_p(a, b), "khac kieu")

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
static int table[10] = {1, 2, 3};
int *get_row(int i) { return &table[i]; }
int (*pick(char op))(int, int) { return op == '+' ? add : sub; }   /* hàm trả về con trỏ hàm */

typedef int (*BinOp)(int, int);           /* đặt tên cho kiểu con trỏ hàm -> dễ đọc hơn nhiều */
BinOp pick2(char op) { return op == '+' ? add : sub; }

int main(void) {
    int n = 0;
    int *p;                 /* p: con trỏ tới int */
    int *arr_of_ptr[3];     /* mảng 3 phần tử, mỗi phần tử là con trỏ tới int */
    int (*ptr_to_arr)[10];  /* con trỏ tới MẢNG 10 int */
    int (*fp)(int, int);    /* con trỏ tới HÀM nhận (int, int) trả về int */
    int (*ops[2])(int, int) = {add, sub};   /* mảng 2 con trỏ hàm */
    const int *pc;          /* con trỏ tới int HẰNG: không sửa *pc */
    int *const cp = &n;     /* con trỏ HẰNG tới int: không đổi cp */
    char **argvlike;        /* con trỏ tới con trỏ tới char */
    void (*handler)(int);   /* con trỏ tới hàm nhận int, không trả về gì (như trình xử lý tín hiệu) */

    SAME_TYPE(__typeof__(p), int *);
    SAME_TYPE(__typeof__(arr_of_ptr), int *[3]);
    SAME_TYPE(__typeof__(ptr_to_arr), int (*)[10]);
    SAME_TYPE(__typeof__(fp), int (*)(int, int));
    SAME_TYPE(__typeof__(ops), int (*[2])(int, int));
    SAME_TYPE(__typeof__(pick), int (*(char))(int, int));
    SAME_TYPE(__typeof__(pick), BinOp (char));
    SAME_TYPE(__typeof__(handler), void (*)(int));
    SAME_TYPE(__typeof__(argvlike), char **);
    SAME_TYPE(__typeof__(&table), int (*)[10]);
    SAME_TYPE(__typeof__(&get_row), int *(*)(int));

    p = &n;
    ptr_to_arr = &table;
    fp = add;
    pc = &n;
    *cp = 7;                                 /* được: sửa giá trị qua con trỏ hằng */
    (void)arr_of_ptr; (void)argvlike; (void)handler;

    printf("*p = %d, *pc = %d\n", *p, *pc);
    printf("(*ptr_to_arr)[2] = %d\n", (*ptr_to_arr)[2]);
    printf("fp(2, 3) = %d, ops[1](9, 4) = %d\n", fp(2, 3), ops[1](9, 4));
    printf("pick('+')(10, 5) = %d, pick2('-')(10, 5) = %d\n", pick('+')(10, 5), pick2('-')(10, 5));
    printf("*get_row(1) = %d\n", *get_row(1));

    /* Mảng "suy biến" thành con trỏ; &table là con trỏ tới CẢ mảng */
    printf("table == &table[0]: %s; (char*)(&table + 1) - (char*)&table = %td byte\n",
           (void *)table == (void *)&table[0] ? "dung" : "sai",
           (char *)(&table + 1) - (char *)&table);
    return 0;
}
```

**Kết quả:**

```text
*p = 7, *pc = 7
(*ptr_to_arr)[2] = 3
fp(2, 3) = 5, ops[1](9, 4) = 5
pick('+')(10, 5) = 15, pick2('-')(10, 5) = 5
*get_row(1) = 2
table == &table[0]: dung; (char*)(&table + 1) - (char*)&table = 40 byte
```

Macro `SAME_TYPE` dùng `__builtin_types_compatible_p` (phần mở rộng GCC/Clang): mọi dòng `SAME_TYPE(...)` ở trên đều biên dịch thành công, nghĩa là **cách đọc trong bảng 4.2 là chính xác**.

**Những điều cần nhớ về mảng và con trỏ:**

- Trong hầu hết biểu thức, tên mảng **"suy biến"** thành con trỏ tới phần tử đầu: `table` ≡ `&table[0]`.
- `&table` có **cùng địa chỉ** nhưng **khác kiểu**: nó là con trỏ tới **cả mảng**, nên `&table + 1` nhảy qua **toàn bộ** 40 byte.
- `a[i]` ≡ `*(a + i)`.
- Gọi hàm qua con trỏ có thể viết `fp(2, 3)` hoặc `(*fp)(2, 3)` — như nhau.

> 💡 Trang **cdecl.org** dịch khai báo C sang tiếng Anh và ngược lại — rất hữu ích khi gặp khai báo "rối".

---

## Chương 5. `struct`, `union`, `enum`

### 5.1 Bảng tra

| Cú pháp | Nghĩa |
|---|---|
| `struct Tag { ... };` | Định nghĩa struct; khi dùng phải viết `struct Tag x;` |
| `typedef struct { ... } Name;` | Struct ẩn danh + tên kiểu → viết `Name x;` |
| `typedef struct Node { struct Node *next; } Node;` | Cần **tên thẻ** (`Node`) khi struct tự trỏ tới chính nó |
| `struct Tag;` | **Khai báo trước** (kiểu chưa đầy đủ) — chỉ dùng được con trỏ; hay dùng để **che giấu** cài đặt (opaque handle) |
| `{1, 2}` | Khởi tạo theo thứ tự |
| `{.y = 5, .x = 3}` | **Designated initializer** — theo tên (C99) |
| `{[2] = 7}` | Khởi tạo mảng theo **chỉ số** |
| `{0}` | Mọi trường/phần tử bằng 0 |
| `(Vec2){3, 4}` | **Compound literal** — tạo giá trị struct tại chỗ (C99, **không** có trong C++ chuẩn) |
| `union { ... };` | Các trường **dùng chung** vùng nhớ; chỉ một trường có nghĩa tại một thời điểm |
| `union { float r; struct { float w, h; }; };` | Union/struct **ẩn danh** (C11): truy cập thẳng `s.r`, `s.w` |
| `unsigned alive : 1;` | **Bit-field**: trường chiếm đúng 1 bit |
| `char data[];` (trường cuối) | **Flexible array member**: độ dài quyết định lúc `malloc` |
| `enum { A, B = 10, C };` | Hằng nguyên: `A = 0`, `B = 10`, `C = 11` |
| `offsetof(Type, field)` | Vị trí (byte) của trường trong struct |
| `p->x` | ≡ `(*p).x` |

### 5.2 Chương trình minh họa

```c
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { float x, y; } Vec2;           /* typedef struct ẩn danh */

typedef struct Node {                          /* cần tên "Node" vì tự tham chiếu */
    int value;
    struct Node *next;
} Node;

typedef enum { SHAPE_CIRCLE, SHAPE_RECT = 10, SHAPE_TRI } ShapeKind;   /* giá trị: 0, 10, 11 */

typedef struct {
    ShapeKind kind;
    union {                                    /* union ẩn danh (C11): các trường dùng chung bộ nhớ */
        float radius;
        struct { float w, h; };                /* struct ẩn danh lồng trong union */
    };
} Shape;

typedef struct {
    unsigned alive  : 1;                       /* bit-field: chỉ chiếm 1 bit */
    unsigned team   : 3;                       /* 3 bit: giá trị 0..7 */
    unsigned level  : 12;
} PackedFlags;

typedef struct {
    uint16_t type;
    uint16_t length;
    char data[];                               /* flexible array member: độ dài quyết định lúc cấp phát */
} Packet;

typedef struct { char c; int i; } Padded;

struct Opaque;                                 /* khai báo trước: chỉ biết "có kiểu này" */
static int use_opaque(struct Opaque *h) { return h == NULL; }   /* dùng được con trỏ, không truy cập trường */

static float area(Shape s) {
    switch (s.kind) {
    case SHAPE_CIRCLE: return 3.14f * s.radius * s.radius;
    case SHAPE_RECT:   return s.w * s.h;
    default:           return 0.0f;
    }
}

static float length_sq(const Vec2 *v) { return v->x * v->x + v->y * v->y; }   /* p->x == (*p).x */

int main(void) {
    Vec2 a = {1.0f, 2.0f};                          /* khởi tạo theo thứ tự */
    Vec2 b = {.y = 5.0f, .x = 3.0f};                /* designated initializer: theo tên */
    Vec2 zero = {0};                                /* mọi trường = 0 */
    int grid[5] = {[2] = 7, [4] = 9};               /* theo chỉ số: {0, 0, 7, 0, 9} */
    printf("a=(%.0f,%.0f) b=(%.0f,%.0f) zero.x=%.0f grid=%d%d%d%d%d\n",
           (double)a.x, (double)a.y, (double)b.x, (double)b.y, (double)zero.x,
           grid[0], grid[1], grid[2], grid[3], grid[4]);

    /* Compound literal: tạo giá trị struct "tại chỗ" */
    printf("length_sq((Vec2){3, 4}) = %.0f\n", (double)length_sq(&(Vec2){3.0f, 4.0f}));

    Shape shapes[] = {{.kind = SHAPE_CIRCLE, .radius = 1.0f}, {.kind = SHAPE_RECT, .w = 2.0f, .h = 3.0f}};
    printf("dien tich: %.2f %.2f; SHAPE_TRI = %d\n", (double)area(shapes[0]), (double)area(shapes[1]), SHAPE_TRI);

    PackedFlags pf = {.alive = 1, .team = 5, .level = 300};
    unsigned nine = 9;
    pf.team = nine;                                 /* chỉ giữ 3 bit thấp: 9 = 0b1001 -> 1 */
    printf("bit-field: alive=%u team=%u level=%u sizeof=%zu\n", pf.alive, pf.team, pf.level, sizeof pf);

    const char *msg = "hello";
    Packet *pk = malloc(sizeof *pk + strlen(msg) + 1);   /* cấp phát header + phần dữ liệu */
    pk->type = 1;
    pk->length = (uint16_t)strlen(msg);
    memcpy(pk->data, msg, strlen(msg) + 1);
    printf("packet: type=%u len=%u data=%s sizeof(Packet)=%zu\n", pk->type, pk->length, pk->data, sizeof(Packet));
    free(pk);

    printf("Padded: sizeof=%zu, offsetof(i)=%zu\n", sizeof(Padded), offsetof(Padded, i));

    Node n2 = {2, NULL}, n1 = {1, &n2};
    for (Node *it = &n1; it != NULL; it = it->next) printf("node %d\n", it->value);
    printf("opaque NULL? %d\n", use_opaque(NULL));
    return 0;
}
```

**Kết quả:**

```text
a=(1,2) b=(3,5) zero.x=0 grid=00709
length_sq((Vec2){3, 4}) = 25
dien tich: 3.14 6.00; SHAPE_TRI = 11
bit-field: alive=1 team=1 level=300 sizeof=4
packet: type=1 len=5 data=hello sizeof(Packet)=4
Padded: sizeof=8, offsetof(i)=4
node 1
node 2
opaque NULL? 1
```

**Ghi chú:**

- `sizeof(Packet)` chỉ là **4 byte** — mảng `data[]` không chiếm chỗ; ta tự cấp phát thêm khi `malloc`. Mẫu này rất phổ biến trong gói tin mạng và định dạng file.
- `Padded` có `sizeof` là 8 dù chỉ chứa `char` + `int`: trình biên dịch chèn **3 byte đệm** để `int` bắt đầu ở vị trí chia hết cho 4.
- Bit-field chỉ giữ số bit đã khai báo: gán 9 vào trường 3 bit còn lại 1. Thứ tự bit và cách đóng gói bit-field **phụ thuộc trình biên dịch** — không dùng bit-field để ánh xạ trực tiếp định dạng file hay giao thức mạng.

---

## Chương 6. Hàm

### 6.1 Bảng tra

| Cú pháp | Nghĩa |
|---|---|
| `int f(void);` | Nguyên mẫu (prototype): hàm **không** nhận tham số. Trong C (trước C23), `int f();` nghĩa là "không rõ tham số" |
| `static int f(void) {...}` | Hàm chỉ dùng trong file này |
| `static inline int f(void) {...}` | Hàm nhỏ định nghĩa trong header |
| `int f(int n, ...)` | Số tham số thay đổi; đọc bằng `va_list`, `va_start`, `va_arg`, `va_end` (`<stdarg.h>`) |
| `void f(int *out)` | **Tham số đầu ra**: hàm ghi kết quả qua con trỏ |
| `int f(...)` trả về `0` / `-1` | Quy ước C/POSIX: 0 thành công, −1 lỗi, chi tiết trong `errno` |
| `void f(void (*cb)(int, void *), void *user)` | **Callback** + con trỏ "dữ liệu người dùng" |
| `__attribute__((format(printf, 2, 3)))` | (GCC/Clang) kiểm tra chuỗi định dạng như `printf` |
| `goto cleanup;` … `cleanup:` | Nhảy tới một điểm dọn dẹp chung |
| `_Noreturn void die(...)` | Hàm không bao giờ quay về |

### 6.2 Chương trình minh họa

```c
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Hàm có số tham số thay đổi: tham số cố định đầu tiên cho biết còn bao nhiêu */
static int sum_ints(int count, ...) {
    va_list ap;
    va_start(ap, count);
    int total = 0;
    for (int i = 0; i < count; i++) total += va_arg(ap, int);
    va_end(ap);
    return total;
}

/* Hàm kiểu printf; thuộc tính format giúp GCC/Clang kiểm tra chuỗi định dạng */
__attribute__((format(printf, 2, 3)))
static void log_msg(const char *level, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf("[%s] ", level);
    vprintf(fmt, ap);                 /* v-printf: nhận va_list */
    printf("\n");
    va_end(ap);
}

/* Trả về nhiều giá trị qua con trỏ (out-parameter); giá trị trả về là mã lỗi */
static int divide(int a, int b, int *quot, int *rem) {
    if (b == 0) return -1;
    *quot = a / b;
    *rem = a % b;
    return 0;
}

/* Callback: truyền hàm như dữ liệu */
static void for_each(const int *arr, size_t n, void (*fn)(int, void *), void *user) {
    for (size_t i = 0; i < n; i++) fn(arr[i], user);
}
static void accumulate(int v, void *user) { *(int *)user += v; }   /* void* -> ép về kiểu thật */

/* Mẫu "goto cleanup": một lối thoát duy nhất để giải phóng tài nguyên */
static int load_two(const char *p1, const char *p2) {
    int rc = -1;
    FILE *f1 = NULL, *f2 = NULL;
    char *buf = malloc(64);
    if (buf == NULL) goto cleanup;
    f1 = fopen(p1, "r");
    if (f1 == NULL) goto cleanup;
    f2 = fopen(p2, "r");
    if (f2 == NULL) goto cleanup;
    rc = 0;                           /* mọi thứ thành công */
cleanup:
    if (f2) fclose(f2);
    if (f1) fclose(f1);
    free(buf);
    return rc;
}

_Noreturn static void die(const char *why) {   /* hàm không bao giờ quay về */
    fprintf(stderr, "fatal: %s\n", why);
    exit(1);
}

int main(void) {
    printf("sum_ints(3, 10, 20, 30) = %d\n", sum_ints(3, 10, 20, 30));
    log_msg("INFO", "nguoi choi %s dat %d diem", "An", 42);

    int q, r;
    if (divide(17, 5, &q, &r) == 0) printf("17 / 5 = %d du %d\n", q, r);
    if (divide(1, 0, &q, &r) != 0) printf("chia cho 0 -> ma loi -1\n");

    int data[] = {1, 2, 3, 4}, total = 0;
    for_each(data, 4, accumulate, &total);
    printf("tong qua callback = %d\n", total);

    errno = 0;
    FILE *f = fopen("/khong/ton/tai", "r");
    if (f == NULL) printf("fopen that bai: errno = %d (%s)\n", errno, strerror(errno));

    printf("load_two -> %d\n", load_two("/khong/co/1", "/khong/co/2"));
    if (total != 10) die("tong sai");
    return 0;
}
```

**Kết quả:**

```text
sum_ints(3, 10, 20, 30) = 60
[INFO] nguoi choi An dat 42 diem
17 / 5 = 3 du 2
chia cho 0 -> ma loi -1
tong qua callback = 10
fopen that bai: errno = 2 (No such file or directory)
load_two -> -1
```

**Về `goto cleanup`:** trong C không có hàm hủy tự động, nên khi một hàm mở nhiều tài nguyên và có nhiều điểm thất bại, mẫu này gom việc dọn dẹp vào **một chỗ**. Đây là cách dùng `goto` được chấp nhận rộng rãi (nhân Linux dùng rất nhiều). Trong C++, RAII thay thế hoàn toàn mẫu này.

**Về `void *user`:** C không có closure/lambda, nên callback nhận thêm một con trỏ "dữ liệu tùy ý" để mang theo trạng thái — bên trong callback ép lại về kiểu thật: `*(int *)user += v;`.

---

## Chương 7. Thành ngữ hay gặp trong code hệ thống

### 7.1 Bảng tra

| Mẫu | Ý nghĩa |
|---|---|
| `(struct sockaddr *)&addr` | Lấy địa chỉ của `addr` rồi ép sang kiểu con trỏ "chung" mà API yêu cầu |
| `memset(&x, 0, sizeof x)` | Xóa toàn bộ `x` về 0 (`sizeof x` không cần ngoặc vì `x` là biến) |
| `malloc(n * sizeof *p)` | Cấp phát n phần tử đúng kiểu của `p` (không cần lặp lại tên kiểu) |
| `while ((n = recv(...)) > 0)` | **Gán rồi so sánh** trong điều kiện — ngoặc bên trong là bắt buộc |
| `if (fd < 0) { perror("open"); return -1; }` | Kiểm tra lỗi kiểu POSIX |
| `for (;;)` | Vòng lặp vô hạn |
| `(void)x;` | "Tôi biết `x` không được dùng" — tắt cảnh báo |
| `!!x` | Chuẩn hóa thành 0 hoặc 1 |
| `#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))` | Số phần tử của **mảng** (sai nếu truyền con trỏ!) |
| `container_of(ptr, type, member)` | Từ con trỏ tới **một trường**, suy ra con trỏ tới **cả struct** |
| `likely(x)` / `unlikely(x)` | Gợi ý nhánh (dựa trên `__builtin_expect` của GCC/Clang) |
| `__attribute__((packed))` / `#pragma pack(1)` | Bỏ byte đệm trong struct |
| `%.*s` | In đúng n ký tự (chuỗi không kết thúc bằng `'\0'`) |
| `(unsigned char *)&value` | Xem từng byte của một giá trị |
| `volatile sig_atomic_t flag` | Biến an toàn để sửa trong trình xử lý tín hiệu |
| `#ifdef _WIN32 ... #else ... #endif` | Code riêng cho từng nền tảng |
| `1u << n` | Mặt nạ bit thứ n |
| `x & (x - 1)` | Xóa bit 1 thấp nhất (bằng 0 ⇔ x là lũy thừa của 2, với x > 0) |

### 7.2 Chương trình minh họa

```c
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* 1) container_of: từ con trỏ tới MỘT TRƯỜNG, suy ra con trỏ tới CẢ struct (dùng nhiều trong nhân Linux) */
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))

struct ListLink { struct ListLink *next; };
struct Player {
    int id;
    struct ListLink link;         /* "móc" để xâu vào danh sách mà không cần cấp phát nút riêng */
    char name[16];
};

/* 2) likely/unlikely: gợi ý cho trình biên dịch nhánh nào hay xảy ra (phần mở rộng GCC/Clang) */
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/* 3) Struct "đóng gói" không đệm — khớp từng byte với định dạng file/giao thức */
struct __attribute__((packed)) WireHeader {
    uint8_t  version;
    uint32_t length;
};

/* 4) Tham số không dùng tới: (void)x để tắt cảnh báo */
static int handler(int sig, void *unused) {
    (void)unused;
    return sig * 2;
}

int main(void) {
    struct Player p = {.id = 7, .name = "Linh"};
    struct ListLink *l = &p.link;
    struct Player *back = container_of(l, struct Player, link);
    printf("container_of -> id=%d name=%s\n", back->id, back->name);

    int err = 0;
    if (unlikely(err != 0)) printf("loi hiem gap\n");
    else if (likely(err == 0)) printf("duong thuong gap\n");

    printf("sizeof WireHeader = %zu (khong co padding)\n", sizeof(struct WireHeader));
    printf("handler(3, NULL) = %d\n", handler(3, NULL));

    /* 5) %.*s: in đúng n ký tự của một chuỗi KHÔNG kết thúc bằng '\0' */
    const char buf[] = {'G', 'E', 'T', ' ', '/', 'x'};
    printf("method = %.*s\n", 3, buf);

    /* 6) Gán trong điều kiện: gán rồi kiểm tra — nhớ bọc ngoặc */
    const char *text = "a,b,c";
    const char *cur = text;
    const char *comma;
    int parts = 1;
    while ((comma = strchr(cur, ',')) != NULL) {
        parts++;
        cur = comma + 1;
    }
    printf("so phan = %d\n", parts);

    /* 7) Vòng lặp vô hạn kiểu C và thoát bằng break */
    int ticks = 0;
    for (;;) {
        if (++ticks == 3) break;
    }
    printf("ticks = %d\n", ticks);

    /* 8) Ép kiểu con trỏ để "nhìn" byte của một giá trị (char* được phép trỏ vào mọi đối tượng) */
    uint32_t value = 0x11223344;
    unsigned char *bytes = (unsigned char *)&value;
    printf("byte dau tien = 0x%02x (%s-endian)\n", bytes[0], bytes[0] == 0x44 ? "little" : "big");

    /* 9) Toán tử !! biến mọi giá trị khác 0 thành đúng 1 */
    printf("!!42 = %d, !!0 = %d\n", !!42, !!0);
    return 0;
}
```

**Kết quả:**

```text
container_of -> id=7 name=Linh
duong thuong gap
sizeof WireHeader = 5 (khong co padding)
handler(3, NULL) = 6
method = GET
so phan = 3
ticks = 3
byte dau tien = 0x44 (little-endian)
!!42 = 1, !!0 = 0
```

**`container_of`** hoạt động bằng cách **lùi con trỏ** một khoảng đúng bằng vị trí của trường trong struct (`offsetof`). Nhân Linux dùng kỹ thuật này để xâu các đối tượng vào danh sách liên kết **mà không cần cấp phát nút riêng** ("intrusive list").

---

## Luyện đọc nhanh — Phần I

Đọc các khai báo sau thành lời (đáp án đã được trình biên dịch xác nhận bằng `_Static_assert`):

1. `char *(*q1)(const char *, int);`
2. `int *(*q2[5])(void);`
3. `double (*(*q3)(int))[3];`
4. `const char *const q4[] = {"a", "b"};`
5. `void (*q5(int sig, void (*handler)(int)))(int);`
6. Macro `#define MAX(a, b) a > b ? a : b` có vấn đề gì khi viết `2 * MAX(3, 4)`?

<details>
<summary><b>Đáp án</b> (bấm để mở)</summary>

1. `q1` là **con trỏ tới hàm** nhận `(const char *, int)` và trả về `char *`.
2. `q2` là **mảng 5 con trỏ tới hàm** không nhận tham số, trả về `int *`.
3. `q3` là **con trỏ tới hàm** nhận `int`, trả về **con trỏ tới mảng 3 `double`**.
4. `q4` là **mảng các con trỏ hằng** tới **ký tự hằng**: không sửa được chuỗi, cũng không trỏ phần tử sang chuỗi khác.
5. `q5` là **hàm** nhận `int` và một con trỏ hàm `void (*)(int)`, trả về một con trỏ hàm `void (*)(int)`. Đây chính là khai báo của hàm `signal()` chuẩn. Viết lại dễ đọc với `typedef void (*sighandler_t)(int);` → `sighandler_t q5(int sig, sighandler_t handler);`.
6. Khai triển thành `2 * 3 > 4 ? 3 : 4` → `(6 > 4) ? 3 : 4` = **3** (không phải 8). Phải bọc ngoặc: `#define MAX(a, b) ((a) > (b) ? (a) : (b))` — và vẫn còn vấn đề tham số bị tính **hai lần** (ví dụ `MAX(i++, j)`), nên ưu tiên dùng hàm `static inline`.

</details>

---

# PHẦN II — C++

## Chương 8. Namespace, tham chiếu và `auto`

### 8.1 Namespace

| Cú pháp | Nghĩa |
|---|---|
| `namespace game { ... }` | Gom tên vào một không gian riêng để tránh trùng tên |
| `namespace game::physics { ... }` | Namespace lồng nhau (C++17) |
| `game::physics::GRAVITY` | Truy cập tên trong namespace |
| `namespace phys = game::physics;` | Bí danh |
| `using std::cout;` | Đưa **một** tên vào phạm vi hiện tại |
| `using namespace std;` | Đưa **tất cả** tên vào — tránh dùng trong header (dễ trùng tên) |
| `namespace { ... }` | Namespace ẩn danh: chỉ thấy trong file này (thay cho `static` của C) |
| `::name` | Tên ở phạm vi toàn cục |

### 8.2 Tham chiếu và loại giá trị

| Cú pháp | Nghĩa |
|---|---|
| `T& r = x;` | Tham chiếu (bí danh) tới `x` — phải khởi tạo ngay, không thể "trỏ" sang biến khác |
| `const T& r` | Tham chiếu chỉ đọc; **gắn được với giá trị tạm** và kéo dài vòng đời của nó |
| `T&& r` | **Tham chiếu rvalue**: chỉ gắn với giá trị **tạm thời** (sắp bị hủy) → được phép "lấy" tài nguyên |
| `std::move(x)` | Ép `x` thành rvalue ("tôi không cần `x` nữa") — **bản thân nó không di chuyển gì** |
| `template<class T> void f(T&& x)` | **Forwarding reference**: nhận cả lvalue lẫn rvalue |
| `std::forward<T>(x)` | Chuyển tiếp `x` **giữ nguyên** là lvalue hay rvalue |

**lvalue** = có tên/địa chỉ, sống lâu (biến). **rvalue** = giá trị tạm (`x + 1`, `std::string("a")`, giá trị trả về theo giá trị).

### 8.3 `auto`, `decltype`, structured binding

| Cú pháp | Kiểu thu được (với `const int& cref = x;`) |
|---|---|
| `auto a = cref;` | `int` — `auto` **bỏ** `const` và `&` → luôn là **bản sao** |
| `auto& a = cref;` | `const int&` |
| `const auto& a = expr;` | tham chiếu chỉ đọc — cách an toàn để "xem" mà không sao chép |
| `auto&& a = x;` | `int&` (x là lvalue); `auto&& a = 10;` → `int&&` |
| `decltype(x)` | Kiểu **khai báo** của `x` |
| `decltype((x))` | Kiểu của **biểu thức** `(x)` → `int&` |
| `decltype(auto)` | Giữ nguyên kiểu chính xác (kể cả `&`) của giá trị trả về |
| `auto [a, b] = pair;` | **Structured binding**: tách pair/tuple/struct thành các biến |
| `if (auto it = m.find(k); it != m.end())` | `if` có câu lệnh khởi tạo (C++17); `it` chỉ sống trong `if`/`else` |
| `for (auto& e : v)` / `for (const auto& e : v)` / `for (auto e : v)` | Range-for: sửa được / chỉ đọc / **bản sao** |

### 8.4 Chương trình minh họa

```cpp
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace game::physics {                 // namespace lồng nhau (C++17)
inline constexpr double GRAVITY = 9.81;
}
namespace phys = game::physics;           // bí danh namespace

namespace {                               // namespace ẩn danh: chỉ dùng trong file này (thay cho static)
int helper() { return 42; }
}

// Ba phiên bản quá tải: phân biệt lvalue, const lvalue, rvalue
std::string kind(std::string&)       { return "lvalue"; }
std::string kind(const std::string&) { return "const lvalue"; }
std::string kind(std::string&&)      { return "rvalue"; }

// "Forwarding reference": T&& trong template giữ nguyên loại giá trị khi dùng std::forward
template <typename T>
std::string relay(T&& x) { return kind(std::forward<T>(x)); }

int main() {
    std::cout << "GRAVITY = " << phys::GRAVITY << ", helper() = " << helper() << '\n';

    std::string s = "hi";
    const std::string cs = "const";
    std::cout << kind(s) << ' ' << kind(cs) << ' ' << kind(std::string("tmp")) << ' '
              << kind(std::move(s)) << '\n';
    std::cout << "relay: " << relay(s) << ' ' << relay(cs) << ' ' << relay(std::string("tmp")) << '\n';

    // Tham chiếu hằng kéo dài vòng đời của giá trị tạm
    const std::string& r = std::string("song lau hon");
    std::cout << r << '\n';

    // auto: trình biên dịch tự suy kiểu — kiểm tra lúc biên dịch bằng static_assert
    int x = 5;
    const int& cref = x;
    auto a1 = cref;            // int (bỏ const và &)
    auto& a2 = cref;           // const int&
    auto&& a3 = x;             // int&   (x là lvalue)
    auto&& a4 = 10;            // int&&  (10 là rvalue)
    decltype(x) d1 = 0;        // int
    decltype((x)) d2 = x;      // int&  (có ngoặc: kiểu của BIỂU THỨC lvalue)
    static_assert(std::is_same_v<decltype(a1), int>);
    static_assert(std::is_same_v<decltype(a2), const int&>);
    static_assert(std::is_same_v<decltype(a3), int&>);
    static_assert(std::is_same_v<decltype(a4), int&&>);
    static_assert(std::is_same_v<decltype(d1), int>);
    static_assert(std::is_same_v<decltype(d2), int&>);
    (void)a1; (void)a2; (void)a3; (void)a4; (void)d1; (void)d2;

    // Structured bindings và if có câu lệnh khởi tạo
    std::map<std::string, int> scores{{"An", 90}, {"Binh", 75}};
    for (const auto& [name, score] : scores) std::cout << name << ": " << score << '\n';
    if (auto it = scores.find("Binh"); it != scores.end()) std::cout << "tim thay Binh = " << it->second << '\n';
    auto [pos, inserted] = scores.insert({"An", 0});
    std::cout << "chen An lan nua? " << std::boolalpha << inserted << " (giu " << pos->second << ")\n";

    // Range-for: bản sao, tham chiếu, tham chiếu hằng
    std::vector<int> v{1, 2, 3};
    for (int e : v) e *= 10;              // sửa BẢN SAO -> v không đổi
    for (int& e : v) e *= 10;             // sửa phần tử thật
    int sum = 0;
    for (const int& e : v) sum += e;
    std::cout << "sum = " << sum << '\n';
}
```

**Kết quả:**

```text
GRAVITY = 9.81, helper() = 42
lvalue const lvalue rvalue rvalue
relay: lvalue const lvalue rvalue
song lau hon
An: 90
Binh: 75
tim thay Binh = 75
chen An lan nua? false (giu 90)
sum = 60
```

Mọi kiểu suy luận ở trên đều được **kiểm chứng bằng `static_assert`** — chương trình biên dịch được nghĩa là bảng 8.3 đúng.

---

## Chương 9. Lớp

### 9.1 Bảng tra

| Cú pháp | Nghĩa |
|---|---|
| `class X { public: ... private: ... protected: ... };` | Kiểm soát truy cập (mặc định `class` là private, `struct` là public) |
| `X(int v) : v_(v) {}` | **Danh sách khởi tạo** — khởi tạo thành viên trước khi chạy thân hàm (theo **thứ tự khai báo**, không theo thứ tự viết) |
| `int x = 0;` trong lớp | Giá trị mặc định của thành viên |
| `explicit X(int)` | Cấm chuyển đổi ngầm: `X x = 5;` bị lỗi |
| `X(const X&)` / `X& operator=(const X&)` | Hàm tạo sao chép / gán sao chép |
| `X(X&&) noexcept` / `X& operator=(X&&) noexcept` | Hàm tạo di chuyển / gán di chuyển |
| `~X()` | Hàm hủy |
| `= default` | Dùng phiên bản trình biên dịch sinh ra |
| `= delete` | **Cấm** hàm này (thường để cấm sao chép) |
| `int get() const` | Hàm **không sửa** đối tượng; gọi được trên đối tượng `const` |
| `mutable int cache_;` | Được sửa kể cả trong hàm `const` |
| `static int count();` / `static inline int n = 0;` | Thành viên thuộc về **lớp** |
| `friend ...` | Cho hàm/lớp bên ngoài truy cập phần private |
| `this` | Con trỏ tới đối tượng hiện tại; `*this` là chính đối tượng |
| `T operator+(const T&) const` | Nạp chồng toán tử (thành viên) |
| `friend std::ostream& operator<<(std::ostream&, const T&)` | Cho phép `std::cout << obj` |
| `auto operator<=>(const T&) const = default;` | Sinh các phép so sánh (C++20) |
| `R operator()(Args)` | Đối tượng **gọi được như hàm** (functor) |
| `explicit operator bool() const` | Dùng được trong `if (obj)` nhưng không tự đổi thành số |
| `class D : public B` | Kế thừa công khai |
| `virtual void f();` | Hàm ảo — lời gọi qua con trỏ/tham chiếu lớp cha gọi đúng bản của lớp con |
| `virtual void f() = 0;` | Hàm **thuần ảo** → lớp **trừu tượng** |
| `void f() override;` | Khẳng định đang ghi đè (sai chữ ký sẽ báo lỗi) — **luôn viết** |
| `final` | Cấm ghi đè tiếp (hàm) / cấm kế thừa (lớp) |
| `virtual ~B() = default;` | Hàm hủy ảo — bắt buộc khi xóa đối tượng con qua con trỏ cha |
| `B::f()` | Gọi phiên bản của lớp cha |
| `using B::B;` | Kế thừa các hàm tạo của lớp cha |

### 9.2 Chương trình minh họa

```cpp
#include <compare>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct Vec2 {
    float x = 0, y = 0;                                      // giá trị mặc định cho thành viên
    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    auto operator<=>(const Vec2&) const = default;           // sinh ra <, <=, >, >= (C++20)
    bool operator==(const Vec2&) const = default;            // sinh ra ==, !=
};

class Texture {
public:
    explicit Texture(std::string name) : name_(std::move(name)), id_(next_id_++) {}   // danh sách khởi tạo
    Texture(const Texture&) = delete;                        // cấm sao chép
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) noexcept = default;                   // cho phép di chuyển
    Texture& operator=(Texture&&) noexcept = default;
    ~Texture() = default;

    const std::string& name() const { return name_; }        // hàm const: không sửa đối tượng
    int uses() const { return ++uses_; }                     // mutable cho phép sửa trong hàm const
    explicit operator bool() const { return !name_.empty(); }   // chuyển sang bool (tường minh)
    static int created() { return next_id_; }

    friend std::ostream& operator<<(std::ostream& os, const Texture& t) {   // hàm bạn: truy cập private
        return os << "Texture(" << t.name_ << ", id=" << t.id_ << ")";
    }

private:
    std::string name_;
    int id_;
    mutable int uses_ = 0;
    static inline int next_id_ = 1;                          // biến static khởi tạo ngay trong lớp (C++17)
};

class Shape {
public:
    virtual ~Shape() = default;                              // hàm hủy ảo: BẮT BUỘC khi dùng đa hình
    virtual double area() const = 0;                         // hàm thuần ảo -> lớp trừu tượng
    virtual std::string name() const { return "Shape"; }
};

class Circle : public Shape {
public:
    explicit Circle(double r) : r_(r) {}
    double area() const override { return 3.14159 * r_ * r_; }
    std::string name() const override { return "Circle"; }
protected:
    double r_;
};

class Ring final : public Circle {                           // final: không ai kế thừa Ring được nữa
public:
    Ring(double outer, double inner) : Circle(outer), inner_(inner) {}
    double area() const override { return Circle::area() - 3.14159 * inner_ * inner_; }  // gọi bản của lớp cha
    std::string name() const final { return "Ring"; }
private:
    double inner_;
};

struct Counter {
    int count = 0;
    int operator()(int step) { return count += step; }       // đối tượng "gọi được" như hàm
    int& operator[](int) { return count; }
};

struct Widget { int w, h; };                                 // aggregate

int main() {
    Vec2 a{1, 2}, b{3, 4};
    Vec2 c = a + b;
    c += {1, 1};
    std::cout << "c = (" << c.x << ", " << c.y << "), a < b: " << std::boolalpha << (a < b)
              << ", a == Vec2{1, 2}: " << (a == Vec2{1, 2}) << '\n';

    Texture t1("hero.png");
    // Texture t2 = t1;                                      // LỖI biên dịch: sao chép bị cấm
    Texture t2 = std::move(t1);                              // được: di chuyển
    // Texture t3 = "x";                                     // LỖI: constructor là explicit
    t2.uses();
    std::cout << t2 << ", uses = " << t2.uses() << ", t1 rong? " << !t1 << ", da tao " << Texture::created() - 1 << '\n';

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(1.0));
    shapes.push_back(std::make_unique<Ring>(2.0, 1.0));
    for (const auto& s : shapes) std::cout << s->name() << ": " << s->area() << '\n';

    Counter counter;
    counter(5);
    counter(3);
    counter[0] += 2;
    std::cout << "counter = " << counter.count << '\n';

    Widget w1{800, 600};                                     // aggregate init
    Widget w2{.w = 1920, .h = 1080};                         // designated init (C++20: đúng thứ tự khai báo)
    std::cout << w1.w << "x" << w1.h << ", " << w2.w << "x" << w2.h << '\n';

    // "Most vexing parse": dòng dưới KHÔNG tạo đối tượng mà khai báo một HÀM tên v
    // std::vector<int> v();
    std::vector<int> v{};                                    // dùng {} để chắc chắn là đối tượng
    std::cout << "v.size() = " << v.size() << '\n';
}
```

**Kết quả:**

```text
c = (5, 7), a < b: true, a == Vec2{1, 2}: true
Texture(hero.png, id=1), uses = 2, t1 rong? true, da tao 1
Circle: 3.14159
Ring: 9.42477
counter = 10
800x600, 1920x1080
v.size() = 0
```

**"Most vexing parse":** `std::vector<int> v();` **không** tạo vector rỗng — C++ hiểu đó là **khai báo một hàm** tên `v` trả về `std::vector<int>`. Dùng `std::vector<int> v;` hoặc `std::vector<int> v{};`.

**Khởi tạo bằng `{}`** còn chặn chuyển đổi làm mất dữ liệu: `int x{3.7};` là **lỗi biên dịch**, trong khi `int x = 3.7;` âm thầm thành 3.

---

## Chương 10. Template

### 10.1 Bảng tra

| Cú pháp | Nghĩa |
|---|---|
| `template <typename T>` ≡ `template <class T>` | Khai báo template với tham số kiểu `T` |
| `template <typename T, std::size_t N>` | Tham số là **giá trị** hằng (như kích thước mảng) |
| `f<int>(x)` | Chỉ định rõ tham số template |
| `template <> R f<int>()` | **Chuyên biệt hóa** cho riêng `int` |
| `typename C::value_type` | Báo cho trình biên dịch: `C::value_type` là một **kiểu** |
| `obj.template get<int>()` | Báo rằng `get` là một **template** (khi `obj` phụ thuộc tham số template) |
| `template <typename... Args>` | Gói tham số (variadic) |
| `sizeof...(Args)` | Số phần tử trong gói |
| `(args + ... + 0)` | **Fold expression**: cộng dồn mọi phần tử (C++17) |
| `f(std::forward<Args>(args)...)` | Mở rộng gói, chuyển tiếp từng phần tử |
| `template <Number T>` | Tham số phải thỏa **concept** `Number` (C++20) |
| `requires cond` | Ràng buộc template bằng điều kiện |
| `auto f(Number auto x)` | Template viết tắt (C++20) |
| `concept Number = ...;` | Định nghĩa concept |
| `constexpr` | Có thể tính **lúc biên dịch** |
| `constinit` | Biến phải được khởi tạo lúc biên dịch (C++20) |
| `if constexpr (cond)` | Nhánh sai **không được biên dịch** |
| `Pair p{1, 2};` | **CTAD**: tự suy ra `Pair<int>` (C++17) |
| `template <typename T> Pair(T, T) -> Pair<T>;` | Hướng dẫn suy luận cho CTAD |
| `auto f() -> int` | Kiểu trả về đặt sau |
| `std::enable_if_t<...>` | Cách "cũ" (trước C++20) để ràng buộc template — gặp nhiều trong thư viện |

### 10.2 Chương trình minh họa

```cpp
#include <array>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

template <typename T>                         // hàm template
T max_of(T a, T b) { return a > b ? a : b; }

template <typename T, std::size_t N>          // tham số template là GIÁ TRỊ (N)
struct Buffer {
    std::array<T, N> data{};
    std::size_t capacity() const { return N; }
};

template <typename T>                         // chuyên biệt hóa toàn phần cho một kiểu
std::string type_name() { return "khong ro"; }
template <>
std::string type_name<int>() { return "int"; }

template <typename Container>
typename Container::value_type first(const Container& c) {   // "typename": báo cho compiler đây là KIỂU
    return *c.begin();
}

template <typename... Args>                   // variadic template: nhận số tham số bất kỳ
auto sum_all(Args... args) { return (args + ... + 0); }       // fold expression (C++17)

template <typename... Args>
std::size_t count_args(Args&&...) { return sizeof...(Args); }  // sizeof...: số tham số

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;   // concept (C++20)

template <Number T>                           // ràng buộc T phải là Number
T twice(T x) { return x * 2; }

auto half(Number auto x) { return x / 2; }    // "abbreviated template": tham số kiểu auto có ràng buộc

template <typename T>
    requires std::is_class_v<T>               // mệnh đề requires
std::string describe(const T&) { return "mot lop"; }

template <typename T>
std::string kind_of(const T&) {
    if (std::is_pointer_v<T>) return "con tro";               // if thường: CẢ HAI nhánh đều phải biên dịch được
    return "khong phai con tro";
}

template <typename T>
auto unwrap(T v) {
    if constexpr (std::is_pointer_v<T>) return *v;            // chỉ nhánh đúng mới được biên dịch
    else return v;
}

auto area(int w, int h) -> int { return w * h; }   // kiểu trả về đặt SAU (trailing return type)

constexpr int factorial(int n) { return n <= 1 ? 1 : n * factorial(n - 1); }
constexpr int FACT5 = factorial(5);           // tính lúc BIÊN DỊCH
static_assert(FACT5 == 120);

template <typename T>
struct Pair {
    T first, second;
};
template <typename T> Pair(T, T) -> Pair<T>;  // hướng dẫn suy luận (CTAD)

int main() {
    std::cout << max_of(3, 7) << ' ' << max_of<double>(2, 2.5) << '\n';   // tự suy / chỉ định rõ
    Buffer<float, 16> buf;
    std::cout << "capacity = " << buf.capacity() << ", type_name<int> = " << type_name<int>()
              << ", type_name<char> = " << type_name<char>() << '\n';
    std::vector<std::string> names{"An", "Binh"};
    std::cout << "first = " << first(names) << '\n';
    std::cout << "sum_all(1, 2, 3.5) = " << sum_all(1, 2, 3.5) << ", count_args = " << count_args(1, "a", 'c') << '\n';
    std::cout << "twice(21) = " << twice(21) << ", half(5.0) = " << half(5.0) << ", describe = " << describe(names) << '\n';
    // twice(std::string("x"));               // LỖI biên dịch: string không thỏa Number
    int n = 7;
    std::cout << kind_of(&n) << ", unwrap(&n) = " << unwrap(&n) << ", unwrap(3) = " << unwrap(3) << '\n';

    Pair p{1, 2};                             // CTAD: tự suy ra Pair<int>
    std::vector v{1.5, 2.5};                  // CTAD: std::vector<double>
    static_assert(std::is_same_v<decltype(p), Pair<int>>);
    static_assert(std::is_same_v<decltype(v), std::vector<double>>);
    std::cout << "area = " << area(3, 4) << ", FACT5 = " << FACT5 << ", p = " << p.first << "," << p.second << '\n';
}
```

**Kết quả:**

```text
7 2.5
capacity = 16, type_name<int> = int, type_name<char> = khong ro
first = An
sum_all(1, 2, 3.5) = 6.5, count_args = 3
twice(21) = 42, half(5.0) = 2.5, describe = mot lop
con tro, unwrap(&n) = 7, unwrap(3) = 3
area = 12, FACT5 = 120, p = 1,2
```

> 💡 Thông báo lỗi template thường rất dài. Hãy đọc **dòng lỗi đầu tiên** và tìm dòng `required from here` trỏ vào code **của bạn**. Concept (C++20) giúp thông báo ngắn và rõ hơn nhiều.

---

## Chương 11. Lambda, ép kiểu, `enum class` và thuộc tính

### 11.1 Giải phẫu một lambda

```text
[capture](params) mutable noexcept -> ReturnType { body }
   │        │        │        │            │          └ thân hàm
   │        │        │        │            └ kiểu trả về (tùy chọn)
   │        │        │        └ hứa không ném ngoại lệ (tùy chọn)
   │        │        └ cho phép sửa các bản sao đã bắt (tùy chọn)
   │        └ tham số (auto → generic lambda)
   └ danh sách bắt biến
```

| Capture | Nghĩa |
|---|---|
| `[]` | Không bắt gì |
| `[x]` / `[&x]` | Bắt `x` theo **giá trị** (sao chép lúc tạo) / theo **tham chiếu** |
| `[=]` / `[&]` | Bắt mọi biến được dùng theo giá trị / tham chiếu |
| `[=, &total]` | Mặc định theo giá trị, riêng `total` theo tham chiếu |
| `[this]` / `[*this]` | Bắt con trỏ đối tượng / **bản sao** đối tượng |
| `[n = 0]` | Tạo biến mới ngay trong lambda (init-capture) |
| `[p = std::move(ptr)]` | **Chuyển** một đối tượng vào lambda |
| `[&]() { ... }()` | Tạo và **gọi ngay** (IIFE) |

### 11.2 Bốn kiểu ép kiểu của C++

| Ép kiểu | Dùng khi | Kiểm tra |
|---|---|---|
| `static_cast<T>(x)` | Chuyển đổi "hợp lý": số ↔ số, `void*` → `T*`, enum ↔ số | Lúc biên dịch |
| `dynamic_cast<T*>(p)` | Ép xuống lớp con **an toàn** (lớp phải có hàm ảo) | Lúc chạy — trả về `nullptr` nếu sai kiểu |
| `const_cast<T&>(x)` | Bỏ `const` | Sửa đối tượng vốn là `const` là UB |
| `reinterpret_cast<T*>(p)` | Diễn giải lại bit (con trỏ ↔ con trỏ khác kiểu, con trỏ ↔ số) | Không — nguy hiểm nhất |
| `std::bit_cast<T>(x)` (C++20) | Xem bit của giá trị này như kiểu khác cùng kích thước | Lúc biên dịch, an toàn |
| `(T)x` | Ép kiểu kiểu C — thử lần lượt các loại trên | Khó biết nó thực sự làm gì → tránh trong C++ |

### 11.3 Thuộc tính và từ khóa đặc biệt

| Cú pháp | Nghĩa |
|---|---|
| `[[nodiscard]]` | Cảnh báo nếu bỏ qua giá trị trả về |
| `[[maybe_unused]]` | Không cảnh báo nếu không dùng |
| `[[fallthrough]]` | Cố ý "rơi" xuống `case` tiếp theo |
| `[[likely]]` / `[[unlikely]]` | Gợi ý nhánh (C++20) |
| `[[deprecated("...")]]` | Cảnh báo khi dùng |
| `[[noreturn]]` | Hàm không quay về |
| `noexcept` | Hứa không ném ngoại lệ (vi phạm → `std::terminate`) |
| `noexcept(expr)` | **Toán tử**: biểu thức có được hứa không ném không? |
| `enum class E : std::uint8_t { A, B };` | Enum có phạm vi (`E::A`), không tự đổi sang số, kiểu nền 1 byte |
| `alignof(T)` / `alignas(16)` | Yêu cầu căn lề / đặt căn lề |
| `nullptr` | Con trỏ rỗng có kiểu riêng (thay `NULL`) |

### 11.4 Chương trình minh họa

```cpp
#include <algorithm>
#include <bit>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

enum class Layer : std::uint8_t { Default = 1 << 0, Player = 1 << 1, Enemy = 1 << 2 };   // kiểu nền uint8_t

constexpr Layer operator|(Layer a, Layer b) {             // enum class không tự có | -> tự định nghĩa
    return static_cast<Layer>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}
constexpr bool has(Layer set, Layer bit) {
    return (static_cast<std::uint8_t>(set) & static_cast<std::uint8_t>(bit)) != 0;
}

struct Base { virtual ~Base() = default; };
struct Enemy : Base { int hp = 30; };
struct Pickup : Base {};

[[nodiscard]] int compute() { return 7; }                  // cảnh báo nếu bỏ qua kết quả
[[deprecated("dung compute()")]] int old_compute() { return 7; }

int classify(int v) {
    switch (v) {
    case 0:
        [[fallthrough]];                                   // cố ý "rơi" xuống case sau
    case 1:
        return 1;
    default:
        return 2;
    }
}

void maybe_throw() {}
void never_throw() noexcept {}

int main() {
    // ----- Lambda -----
    int base = 10;
    auto by_value = [base](int x) { return x + base; };
    auto by_ref = [&base](int x) { return x + base; };
    base = 100;
    std::cout << "by_value(1) = " << by_value(1) << ", by_ref(1) = " << by_ref(1) << '\n';

    auto counter = [n = 0]() mutable { return ++n; };      // init-capture + mutable
    counter();
    std::cout << "counter() = " << counter() << '\n';

    auto ptr = std::make_unique<int>(5);
    auto owner = [p = std::move(ptr)] { return *p * 2; };  // chuyển quyền sở hữu vào lambda
    std::cout << "owner() = " << owner() << ", ptr rong? " << std::boolalpha << (ptr == nullptr) << '\n';

    auto add = [](auto a, auto b) { return a + b; };       // generic lambda
    std::cout << "add(1, 2) = " << add(1, 2) << ", add(str) = " << add(std::string("a"), "b") << '\n';

    auto typed = [](double x) noexcept -> int { return static_cast<int>(x); };   // chỉ định kiểu trả về
    std::cout << "typed(3.9) = " << typed(3.9) << '\n';

    const int limit = [&] {                                // IIFE: tính một hằng phức tạp tại chỗ
        int l = 0;
        for (int i = 0; i < 5; ++i) l += i;
        return l;
    }();
    std::cout << "limit = " << limit << '\n';

    std::vector<int> nums{5, 3, 8, 1};
    std::sort(nums.begin(), nums.end(), [](int a, int b) { return a > b; });
    std::function<void(int)> printer = [](int x) { std::cout << x << ' '; };
    std::for_each(nums.begin(), nums.end(), printer);
    std::cout << '\n';

    // ----- Ép kiểu -----
    double d = 3.7;
    int i = static_cast<int>(d);                           // chuyển đổi "hợp lý" (cắt phần thập phân)
    std::vector<std::unique_ptr<Base>> objs;
    objs.push_back(std::make_unique<Enemy>());
    objs.push_back(std::make_unique<Pickup>());
    int enemies = 0;
    for (const auto& o : objs)
        if (auto* e = dynamic_cast<Enemy*>(o.get())) enemies += e->hp > 0;   // nullptr nếu sai kiểu
    const std::string label = "hang";
    std::string& editable = const_cast<std::string&>(label);   // bỏ const (NGUY HIỂM nếu thật sự sửa)
    (void)editable;
    float f = 1.0f;
    auto bits = std::bit_cast<std::uint32_t>(f);           // xem bit của float (C++20, an toàn)
    std::uint32_t value = 0x11223344;
    auto* raw = reinterpret_cast<unsigned char*>(&value);  // "nhìn" byte của đối tượng
    std::cout << "i = " << i << ", enemies = " << enemies << ", bits = 0x" << std::hex << bits
              << ", raw[0] = 0x" << static_cast<int>(raw[0]) << std::dec << '\n';

    Layer mask = Layer::Player | Layer::Enemy;
    std::cout << "has Enemy: " << has(mask, Layer::Enemy) << ", has Default: " << has(mask, Layer::Default)
              << ", underlying = " << static_cast<int>(static_cast<std::underlying_type_t<Layer>>(mask)) << '\n';   // C++23: std::to_underlying(mask)

    // ----- Thuộc tính và noexcept -----
    int r = compute();
    std::cout << "compute = " << r << ", classify(0) = " << classify(0) << ", classify(5) = " << classify(5) << '\n';
    std::cout << "noexcept(maybe_throw()) = " << noexcept(maybe_throw())
              << ", noexcept(never_throw()) = " << noexcept(never_throw()) << '\n';
    [[maybe_unused]] int debug_only = 42;                  // không cảnh báo dù không dùng
    std::cout << "alignof(double) = " << alignof(double) << ", sizeof(Layer) = " << sizeof(Layer) << '\n';
}
```

**Kết quả:**

```text
by_value(1) = 11, by_ref(1) = 101
counter() = 2
owner() = 10, ptr rong? true
add(1, 2) = 3, add(str) = ab
typed(3.9) = 3
limit = 10
8 5 3 1 
i = 3, enemies = 1, bits = 0x3f800000, raw[0] = 0x44
has Enemy: true, has Default: false, underlying = 6
compute = 7, classify(0) = 1, classify(5) = 2
noexcept(maybe_throw()) = false, noexcept(never_throw()) = true
alignof(double) = 8, sizeof(Layer) = 1
```

---

## Chương 12. Cú pháp của thư viện chuẩn

### 12.1 Bảng tra

| Cú pháp | Nghĩa |
|---|---|
| `std::unique_ptr<T>` / `std::make_unique<T>(args)` | Sở hữu độc quyền |
| `std::unique_ptr<T[]>` / `std::make_unique<T[]>(n)` | Sở hữu một **mảng** động; truy cập bằng `p[i]` |
| `std::shared_ptr<T>` / `std::make_shared<T>(...)` | Sở hữu chung (đếm tham chiếu) |
| `std::optional<T>`; `std::nullopt`; `*opt`; `opt->x`; `opt.value_or(v)`; `if (opt)` | Có thể có hoặc không có giá trị |
| `std::variant<A, B>`; `std::get<A>(v)`; `std::get_if<A>(&v)`; `std::visit(f, v)` | Một trong nhiều kiểu |
| `overloaded{ lambda1, lambda2 }` | Mẫu gom nhiều lambda để `visit` (không có sẵn trong thư viện — tự khai báo 2 dòng) |
| `std::tuple`; `std::tie(a, b, std::ignore) = t;` | Nhóm nhiều giá trị; gán vào biến có sẵn |
| `std::string_view` | "Nhìn" vào chuỗi mà không sao chép |
| `std::span<const int>` | "Nhìn" vào dãy phần tử liên tục (C++20) |
| `using namespace std::chrono_literals;` → `50ms`, `2s` | Hằng thời gian |
| `using namespace std::string_literals;` → `"abc"s` | Hằng `std::string` |
| `operator""_kg(long double)` → `2.5_kg` | Literal tự định nghĩa |
| `std::format("{:>6}", x)` | Định dạng chuỗi kiểu Python (C++20) |
| `v \| std::views::filter(f) \| std::views::transform(g)` | Ranges: xử lý dạng "đường ống", tính lười (C++20) |
| `std::ranges::sort(v)` | Thuật toán nhận thẳng container |
| `std::byte` | Byte thô (không phải số hay ký tự) |
| `R"(chuỗi "thô" \ không thoát)"` | Chuỗi thô: `\` và `"` giữ nguyên |
| `std::expected<T, E>`; `std::unexpected(e)`; `r.error()` | Kết quả **hoặc** lỗi (C++23) |

### 12.2 Chương trình minh họa

```cpp
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <format>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

using namespace std::chrono_literals;        // cho phép viết 50ms, 2s...
using namespace std::string_literals;        // cho phép viết "abc"s (std::string)

// Literal tự định nghĩa: 5.0_kg
constexpr long double operator""_kg(long double v) { return v * 1000; }   // đổi ra gam

// Mẫu "overloaded": gom nhiều lambda thành một đối tượng để dùng với std::visit
template <typename... Fs>
struct overloaded : Fs... { using Fs::operator()...; };

using Event = std::variant<int, std::string, double>;

std::optional<int> find_score(const std::map<std::string, int>& m, std::string_view name) {
    if (auto it = m.find(std::string(name)); it != m.end()) return it->second;
    return std::nullopt;                         // "không có giá trị"
}

std::tuple<int, int, int> rgb() { return {255, 128, 0}; }

int sum(std::span<const int> xs) {               // nhận mọi mảng liên tục mà không sao chép
    int s = 0;
    for (int x : xs) s += x;
    return s;
}

int main() {
    // Con trỏ thông minh
    auto one = std::make_unique<int>(7);
    auto many = std::make_unique<int[]>(3);      // mảng động: dùng [] để truy cập
    many[1] = 5;
    std::shared_ptr<std::string> shared = std::make_shared<std::string>("chia se");
    auto another = shared;
    std::cout << *one << ' ' << many[1] << ' ' << *shared << " use_count=" << shared.use_count() << '\n';

    // optional
    std::map<std::string, int> scores{{"An", 90}};
    auto s1 = find_score(scores, "An");
    auto s2 = find_score(scores, "Chi");
    std::cout << "An: " << (s1 ? std::to_string(*s1) : "khong co") << ", Chi: " << s2.value_or(-1)
              << ", has_value: " << std::boolalpha << s2.has_value() << '\n';

    // variant + visit
    std::vector<Event> events{42, "jump"s, 3.5};
    for (const auto& e : events)
        std::visit(overloaded{
                       [](int v) { std::cout << "int " << v << '\n'; },
                       [](const std::string& v) { std::cout << "string " << v << '\n'; },
                       [](double v) { std::cout << "double " << v << '\n'; },
                   },
                   e);
    if (auto* p = std::get_if<std::string>(&events[1])) std::cout << "get_if -> " << *p << '\n';

    // tuple, tie, structured binding
    auto [r, g, b] = rgb();
    int rr, gg;
    std::tie(rr, gg, std::ignore) = rgb();
    std::cout << "rgb = " << r << "," << g << "," << b << "; tie = " << rr << "," << gg << '\n';

    // string_view, span
    std::string_view sv = "Hello, world";
    int arr[] = {1, 2, 3};
    std::vector<int> vec{4, 5};
    std::cout << sv.substr(0, 5) << ", sum = " << sum(arr) + sum(vec) << '\n';
    const char* json = R"({"name": "An", "path": "C:\game"})";   // chuỗi thô: không cần viết \" hay \\ nữa
    std::cout << json << '\n';

    // chrono literals, user-defined literal
    auto timeout = 1500ms;
    std::cout << "timeout = " << timeout.count() << " ms = "
              << std::chrono::duration_cast<std::chrono::seconds>(timeout).count() << " s (lam tron xuong); 2.5_kg = "
              << static_cast<double>(2.5_kg) << " g\n";

    // format (C++20)
    std::cout << std::format("{:>6}|{:<6}|{:08.3f}|{:#x}\n", "HP", 100, 3.14159, 255);

    // ranges: đọc từ trái sang phải như một "đường ống"
    std::vector<int> data{1, 2, 3, 4, 5, 6};
    auto even_squares = data | std::views::filter([](int x) { return x % 2 == 0; })
                             | std::views::transform([](int x) { return x * x; });
    for (int x : even_squares) std::cout << x << ' ';
    std::cout << "| max = " << std::ranges::max(data) << '\n';

    // std::byte: kiểu "byte thô", không phải số hay ký tự
    std::byte flags{0b0000'1010};                // ' là dấu phân cách chữ số (C++14)
    flags |= std::byte{0b0000'0001};
    std::cout << "flags = " << std::to_integer<int>(flags) << '\n';
}
```

**Kết quả:**

```text
7 5 chia se use_count=2
An: 90, Chi: -1, has_value: false
int 42
string jump
double 3.5
get_if -> jump
rgb = 255,128,0; tie = 255,128
Hello, sum = 15
{"name": "An", "path": "C:\game"}
timeout = 1500 ms = 1 s (lam tron xuong); 2.5_kg = 2500 g
    HP|100   |0003.142|0xff
4 16 36 | max = 6
flags = 11
```

### 12.3 `std::expected` (C++23)

```cpp
#include <charconv>
#include <expected>
#include <iostream>
#include <string>
#include <string_view>

// C++23: std::expected<T, E> = "hoặc có kết quả T, hoặc có lỗi E"
std::expected<int, std::string> parse_port(std::string_view s) {
    int v = 0;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
    if (ec != std::errc{} || ptr != s.data() + s.size()) return std::unexpected("khong phai so: " + std::string(s));
    if (v < 1 || v > 65535) return std::unexpected("ngoai khoang: " + std::string(s));
    return v;
}

int main() {
    for (std::string_view s : {"8080", "abc", "70000"}) {
        auto r = parse_port(s);
        if (r) std::cout << "port " << *r << '\n';
        else std::cout << "loi: " << r.error() << '\n';
    }
    std::cout << "value_or: " << parse_port("x").value_or(9000) << '\n';
}
```

**Kết quả** (biên dịch với `-std=c++23`):

```text
port 8080
loi: khong phai so: abc
loi: ngoai khoang: 70000
value_or: 9000
```

---

## Chương 13. Cú pháp đa luồng và coroutine

### 13.1 Đa luồng

| Cú pháp | Nghĩa |
|---|---|
| `std::thread t(f, args...);` … `t.join();` | Tạo luồng; bắt buộc `join` hoặc `detach` |
| `std::jthread t([](std::stop_token st) {...});` | Tự join khi hủy, hỗ trợ yêu cầu dừng (C++20) |
| `std::ref(x)` | Truyền **tham chiếu** vào luồng (mặc định là sao chép) |
| `std::lock_guard lock(m);` | Khóa đến hết khối (CTAD: không cần viết `<std::mutex>`) |
| `std::unique_lock lock(m);` | Khóa có thể mở giữa chừng; dùng với condition variable |
| `std::scoped_lock lock(m1, m2);` | Khóa nhiều mutex không deadlock |
| `cv.wait(lock, [&] { return ready; });` | Ngủ đến khi điều kiện đúng |
| `std::atomic<int> n{0};` `n.fetch_add(1)` | Biến nguyên tử |
| `std::async(std::launch::async, f)` → `std::future<T>` → `.get()` | Chạy bất đồng bộ, lấy kết quả sau |
| `{ std::lock_guard lock(m); ... }` | Khối `{ }` riêng chỉ để **giới hạn thời gian giữ khóa** |

```cpp
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

int main() {
    std::mutex m;
    std::condition_variable cv;
    std::queue<int> jobs;
    bool done = false;
    std::atomic<int> processed{0};

    std::jthread worker([&](std::stop_token) {                 // C++20: tự join khi bị hủy
        for (;;) {
            std::unique_lock lock(m);                          // CTAD: không cần viết <std::mutex>
            cv.wait(lock, [&] { return !jobs.empty() || done; });
            if (jobs.empty()) return;
            jobs.pop();
            lock.unlock();                                     // mở khóa trước khi "làm việc"
            processed.fetch_add(1, std::memory_order_relaxed);
        }
    });

    for (int i = 0; i < 5; ++i) {
        {
            std::lock_guard lock(m);                           // khóa trong khối { }
            jobs.push(i);
        }
        cv.notify_one();
    }
    {
        std::scoped_lock lock(m);
        done = true;
    }
    cv.notify_all();
    worker.join();

    auto fut = std::async(std::launch::async, [] { std::this_thread::sleep_for(10ms); return 42; });
    std::cout << "processed = " << processed.load() << ", async = " << fut.get() << '\n';
}
```

**Kết quả:**

```text
processed = 5, async = 42
```

(Chương trình này cũng chạy sạch dưới ThreadSanitizer.)

### 13.2 Coroutine (C++20)

| Từ khóa | Nghĩa |
|---|---|
| `co_await expr` | **Tạm dừng** coroutine đến khi `expr` hoàn tất (luồng được giải phóng để làm việc khác) |
| `co_return value` | Kết thúc coroutine và trả về giá trị |
| `co_yield value` | Trả ra một giá trị rồi **tạm dừng** (dùng cho generator) |

Một hàm có chứa **bất kỳ** từ khóa nào ở trên là một coroutine. Kiểu trả về của nó (ví dụ `asio::awaitable<void>`, `std::generator<int>` của C++23) do **thư viện** cung cấp — bạn gần như không bao giờ tự viết phần máy móc bên trong (`promise_type`). Ví dụ rút gọn (bỏ phần `try`/`catch`) từ echo server Asio đã chạy thật:

```cpp
awaitable<void> echo(tcp::socket socket) {
    char data[4096];
    for (;;) {
        std::size_t n = co_await socket.async_read_some(asio::buffer(data), use_awaitable);
        co_await asio::async_write(socket, asio::buffer(data, n), use_awaitable);
    }
}
```

Đọc là: "lặp mãi: **chờ** đọc được một ít dữ liệu, rồi **chờ** ghi hết số dữ liệu đó trở lại" — mỗi lần chờ, luồng được dùng cho client khác. (Chi tiết ở tài liệu hệ điều hành – đa luồng – async I/O.)

---

## Chương 14. Cú pháp hay gặp trong code game

### 14.1 Bảng tra

| Cú pháp | Nghĩa trong game |
|---|---|
| `1.0f`, `0.016f` | Hằng `float` (không có `f` là `double` → phép tính bị đổi kiểu ngầm) |
| `Vec3 operator+(Vec3 o) const` | Toán học véc tơ viết tự nhiên: `pos + vel * dt` |
| `constexpr Vec3 UP{0, 1, 0};` | Hằng tính lúc biên dịch |
| `static_cast<int>(f)` | Chuyển float sang int (cắt phần thập phân) |
| `std::lerp`, `std::clamp` | Nội suy, kẹp giá trị |
| `(r << 24) \| (g << 16) \| ...` | Đóng gói màu / cờ vào một số nguyên |
| `enum class State { Idle, Chase };` + `switch` | Máy trạng thái của AI |
| `struct alignas(16) Data {...}` | Căn lề cho SIMD |
| `__m128`, `_mm_add_ps`, `_mm_load_ps` | Hàm intrinsic SIMD (SSE, x86) — ARM dùng NEON (`float32x4_t`, `vaddq_f32`) |
| `struct { std::vector<float> x, vx; }` | **Struct of Arrays** — thân thiện cache |
| `assert(cond && "thong bao")` | Kiểm tra ở bản debug; chuỗi chỉ để hiện thông báo (con trỏ chuỗi luôn "đúng") |
| `#ifdef NDEBUG` / `#if defined(_DEBUG)` | Code chỉ có ở bản release/debug |
| `[[likely]]`, `inline` | Gợi ý tối ưu cho vòng lặp nóng |
| `extern "C" { ... }` | Gọi thư viện viết bằng C (SDL, Lua, nhiều SDK) từ C++ |
| `union { struct { float x, y, z; }; float v[3]; };` | Truy cập cùng dữ liệu theo hai cách — **hợp lệ trong C**, nhưng đọc trường khác với trường vừa ghi là **UB trong C++** (dù nhiều engine vẫn dùng vì trình biên dịch hỗ trợ) |
| `UCLASS()`, `GENERATED_BODY()`, `UPROPERTY(...)` | **Macro** của Unreal Engine — được công cụ sinh mã của engine (UnrealHeaderTool) đọc, không phải cú pháp C++ đặc biệt |

### 14.2 Chương trình minh họa

```cpp
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
#include <immintrin.h>              // hàm "intrinsic" SIMD của x86 (SSE/AVX)

struct Vec3 {
    float x{}, y{}, z{};
    constexpr Vec3 operator+(Vec3 o) const { return {x + o.x, y + o.y, z + o.z}; }
    constexpr Vec3 operator-(Vec3 o) const { return {x - o.x, y - o.y, z - o.z}; }
    constexpr Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    constexpr float dot(Vec3 o) const { return x * o.x + y * o.y + z * o.z; }
    float length() const { return std::sqrt(dot(*this)); }
};
constexpr Vec3 operator*(float s, Vec3 v) { return v * s; }   // cho phép viết 2.0f * v

// Màu RGBA đóng gói trong 32 bit
constexpr std::uint32_t pack_rgba(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) {
    return (std::uint32_t{r} << 24) | (std::uint32_t{g} << 16) | (std::uint32_t{b} << 8) | a;
}
constexpr std::uint8_t green_of(std::uint32_t c) { return static_cast<std::uint8_t>((c >> 16) & 0xFF); }

// Bảng tra cứu tính sẵn lúc biên dịch
constexpr std::array<int, 5> make_xp_table() {
    std::array<int, 5> t{};
    for (int i = 0; i < 5; ++i) t[i] = 100 * (i + 1) * (i + 1);
    return t;
}
constexpr auto XP_TABLE = make_xp_table();

enum class State { Idle, Chase, Attack };

// Dữ liệu căn lề 16 byte để dùng với lệnh SIMD
struct alignas(16) Particles4 {
    float x[4];
    float vx[4];
};

int main() {
    Vec3 pos{0.0f, 10.0f, 0.0f}, vel{2.0f, 0.0f, 0.0f};
    const Vec3 gravity{0.0f, -9.8f, 0.0f};
    const float dt = 1.0f / 60.0f;                        // hậu tố f: số float (không có f là double)

    // Vòng lặp cố định bước thời gian (fixed timestep)
    for (int frame = 0; frame < 60; ++frame) {
        vel = vel + gravity * dt;                         // Euler bán ẩn: cập nhật vận tốc trước
        pos = pos + dt * vel;
    }
    std::cout << "sau 1 giay: pos.x = " << pos.x << ", pos.y ~ " << std::round(pos.y * 10) / 10 << '\n';

    float t = 0.25f;
    float smooth = std::lerp(0.0f, 100.0f, t);            // nội suy tuyến tính (C++20)
    int hp = std::clamp(150, 0, 100);                     // kẹp giá trị vào đoạn
    int percent = static_cast<int>(0.756f * 100);         // ép kiểu tường minh
    std::cout << "lerp = " << smooth << ", clamp = " << hp << ", percent = " << percent << '\n';

    std::uint32_t orange = pack_rgba(255, 165, 0, 255);
    std::cout << std::hex << "orange = 0x" << orange << std::dec
              << ", green = " << static_cast<int>(green_of(orange)) << ", XP_TABLE[3] = " << XP_TABLE[3] << '\n';

    State s = State::Chase;
    switch (s) {
    case State::Idle:   std::cout << "dung yen\n"; break;
    case State::Chase:  std::cout << "duoi theo\n"; break;
    case State::Attack: std::cout << "tan cong\n"; break;
    }

    // SIMD: cộng 4 cặp số trong MỘT lệnh máy
    Particles4 p{{1, 2, 3, 4}, {10, 20, 30, 40}};
    __m128 px = _mm_load_ps(p.x);                         // nạp 4 float (yêu cầu căn lề 16)
    __m128 pv = _mm_load_ps(p.vx);
    _mm_store_ps(p.x, _mm_add_ps(px, pv));                // x[i] += vx[i] cho cả 4 phần tử cùng lúc
    std::cout << "SIMD: " << p.x[0] << ' ' << p.x[1] << ' ' << p.x[2] << ' ' << p.x[3]
              << " (alignof = " << alignof(Particles4) << ")\n";

    // Struct of Arrays: mỗi thuộc tính một mảng liên tục (thân thiện cache)
    struct { std::vector<float> x, vx; } soa{{0, 1}, {5, 5}};
    for (std::size_t i = 0; i < soa.x.size(); ++i) [[likely]] soa.x[i] += soa.vx[i] * dt * 60;
    std::cout << "soa.x = " << soa.x[0] << ", " << soa.x[1] << '\n';

    assert(hp <= 100 && "mau khong duoc vuot 100");       // chỉ kiểm tra ở bản debug (khi không có NDEBUG)
    std::cout << "assert dang " << (
#ifdef NDEBUG
        "TAT"
#else
        "BAT"
#endif
    ) << '\n';
}
```

**Kết quả:**

```text
sau 1 giay: pos.x = 2, pos.y ~ 5
lerp = 25, clamp = 100, percent = 75
orange = 0xffa500ff, green = 165, XP_TABLE[3] = 1600
duoi theo
SIMD: 11 22 33 44 (alignof = 16)
soa.x = 5, 6
assert dang BAT
```

**Để ý:** `static_cast<int>(0.756f * 100)` cho **75** chứ không phải 76 — ép sang số nguyên **cắt bỏ** phần thập phân. Muốn làm tròn, dùng `std::lround`.

### 14.3 `extern "C"` — vì sao cần?

C++ **"trang trí" tên hàm** (name mangling) để hỗ trợ nạp chồng: hàm `int ml_add(int, int)` trong file C++ được đặt tên máy là `_Z6ml_addii`, trong khi file C chỉ có tên `ml_add`. Header dùng chung cho C và C++ vì vậy có dạng:

```c
/* mathlib.h */
#ifndef MATHLIB_H
#define MATHLIB_H

#ifdef __cplusplus
extern "C" {            /* với C++: dùng quy ước đặt tên của C */
#endif

int ml_add(int a, int b);

#ifdef __cplusplus
}
#endif

#endif
```

**Kiểm chứng:** với header trên, chương trình C++ gọi `ml_add` từ file C biên dịch và chạy đúng (`ml_add(2, 3) = 5`). Nếu **bỏ** `extern "C"`, công cụ `nm` cho thấy file C++ tìm ký hiệu `_Z6ml_addii` còn file C chỉ có `ml_add`, và bước liên kết thất bại:

```text
undefined reference to `ml_add(int, int)'
```

---

## Luyện đọc nhanh — Phần II

Mỗi dòng sau làm gì? (Tất cả đã được kiểm tra biên dịch được.)

1. `auto&& x = f();` với `std::string f();`
2. `template <typename T> void g(T&& v);`
3. `auto lam = [=, &total](auto& e) mutable { total += e; };`
4. `std::unique_ptr<Node[]> nodes = std::make_unique<Node[]>(3);`
5. `explicit operator bool() const noexcept { ... }`
6. `std::vector<std::pair<int, std::string>>::const_iterator it = pairs.cbegin();`
7. `decltype(auto) first(std::vector<int>& v) { return v[0]; }`
8. `constexpr unsigned long long operator""_ms(unsigned long long v);` rồi `auto t = 250_ms;`
9. `template <typename T> requires std::is_integral_v<T> T h(T v);`

<details>
<summary><b>Đáp án</b> (bấm để mở)</summary>

1. `x` có kiểu `std::string&&` — tham chiếu tới giá trị tạm do `f()` trả về, và vòng đời của giá trị tạm được **kéo dài** theo `x`.
2. `g` nhận **mọi** loại giá trị (forwarding reference): với lvalue, `T` là `X&`; với rvalue, `T` là `X`.
3. Lambda generic (tham số `auto&`), bắt mọi biến theo **giá trị** trừ `total` theo **tham chiếu**; `mutable` cho phép sửa các bản sao bên trong.
4. `nodes` sở hữu một **mảng** 3 phần tử `Node`, tự `delete[]` khi bị hủy.
5. Cho phép viết `if (obj)` nhưng **không** cho phép `int n = obj;`; hàm không sửa đối tượng (`const`) và hứa không ném ngoại lệ (`noexcept`).
6. `it` là **iterator chỉ đọc** của vector chứa các cặp (int, string), trỏ vào phần tử đầu. (Thường viết gọn: `auto it = pairs.cbegin();`.)
7. Trả về **đúng kiểu** của `v[0]`, tức `int&` (tham chiếu) — với `auto` thay vì `decltype(auto)`, hàm sẽ trả về bản sao `int`.
8. Literal tự định nghĩa: `250_ms` gọi `operator""_ms(250)`.
9. Template chỉ chấp nhận kiểu **nguyên**; gọi `h(2.5)` là lỗi biên dịch.

</details>

---

# PHẦN III — GIẢI MÃ CODE THẬT

Các đoạn dưới đây được **trích nguyên văn** từ `net.h`, `chat_server.c`, `socket.hpp` và `chat_client.cpp` trong tài liệu lập trình socket (đã chạy thật trên Linux và Windows). Với mỗi đoạn, bảng bên dưới tách từng "mảnh" cú pháp và giải thích.

## Chương 15. Giải mã `net.h`

### 15.1 Chọn code theo nền tảng

```c
/* trích từ net.h */
#ifndef NET_H
#define NET_H

#ifdef _WIN32
#  ifndef _WIN32_WINNT
#    define _WIN32_WINNT 0x0600
#  endif
typedef SOCKET socket_t;
#  define net_close(s)        closesocket(s)
#  define net_poll(fds, n, t) WSAPoll((fds), (ULONG)(n), (t))
#else
typedef int     socket_t;
#  define net_close(s)        close(s)
```

| Mảnh | Nghĩa |
|---|---|
| `#ifndef NET_H` / `#define NET_H` / … `#endif` | Include guard: lần include thứ hai, `NET_H` đã được định nghĩa nên cả file bị bỏ qua |
| `#ifdef _WIN32` … `#else` | Trình biên dịch cho Windows luôn định nghĩa `_WIN32` (kể cả bản 64 bit) → chọn nhánh Winsock |
| `#  ifndef` | Khoảng trắng sau `#` được phép — chỉ để thụt lề cho dễ đọc các `#if` lồng nhau |
| `#    define _WIN32_WINNT 0x0600` | Báo cho header Windows: "hãy bật API từ phiên bản 6.0 (Vista) trở lên" |
| `typedef SOCKET socket_t;` | Đặt tên **chung** `socket_t` cho hai kiểu khác nhau (`SOCKET` trên Windows, `int` trên POSIX) → phần code còn lại chỉ dùng `socket_t` |
| `#  define net_close(s)  closesocket(s)` | Macro "hàm": `net_close(x)` được thay bằng `closesocket(x)` |
| `WSAPoll((fds), (ULONG)(n), (t))` | Bọc ngoặc từng tham số macro; `(ULONG)(n)` là **ép kiểu** vì `WSAPoll` nhận `ULONG` |

### 15.2 Hàm trong header

```c
/* trích từ net.h */
static inline int net_init(void) {
    signal(SIGPIPE, SIG_IGN);
    static char buf[256];
    while (n > 0 && (buf[n - 1] == '\r' || buf[n - 1] == '\n')) buf[--n] = '\0';
    return n > 0 ? buf : "unknown error";
    return fcntl(s, F_SETFL, on ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK));
    return setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
```

*(Các dòng lấy từ nhiều hàm khác nhau của `net.h`.)*

| Mảnh | Nghĩa |
|---|---|
| `static inline int net_init(void)` | `static`: mỗi file `.c` include header có bản riêng, không xung đột khi liên kết; `inline`: không cảnh báo nếu file đó không dùng hàm; `(void)`: không nhận tham số |
| `signal(SIGPIPE, SIG_IGN)` | Đặt hành vi cho tín hiệu `SIGPIPE` là "bỏ qua"; `SIG_IGN` là một **con trỏ hàm đặc biệt** |
| `static char buf[256];` (trong hàm) | Mảng **sống suốt chương trình** → trả về con trỏ tới nó là hợp lệ (nhưng không an toàn đa luồng) |
| `buf[n - 1] == '\r' \|\| buf[n - 1] == '\n'` | So sánh với **ký tự** (nháy đơn) CR và LF |
| `buf[--n] = '\0'` | **Giảm `n` trước**, rồi ghi ký tự kết thúc chuỗi vào vị trí mới — một lệnh làm hai việc |
| `return n > 0 ? buf : "unknown error";` | Toán tử 3 ngôi; cả hai nhánh đều là `const char *` |
| `on ? (flags \| O_NONBLOCK) : (flags & ~O_NONBLOCK)` | Bật cờ bằng `\|`, tắt cờ bằng `& ~` |
| `(const char *)&tv` | `&tv`: địa chỉ của `tv`; ép sang `const char *` vì Winsock khai báo tham số kiểu đó (POSIX nhận `const void *` nên cũng chấp nhận) |
| `sizeof tv` | Kích thước của **biến** — không cần ngoặc; tự đúng dù `tv` là `DWORD` hay `struct timeval` |
| `(long)(ms % 1000) * 1000000L` | Ép sang `long` **trước** khi nhân để tránh tràn `int`; hậu tố `L` làm hằng số có kiểu `long` |

---

## Chương 16. Giải mã `chat_server.c`

### 16.1 Kiểu dữ liệu

```c
/* trích từ chat_server.c */
#define MAX_CLIENTS  32
typedef struct {
    socket_t fd;
    char name[32];
    char buf[MAX_LINE];
    size_t len;
} Client;
static Client clients[MAX_CLIENTS];
static int nclients = 0;
```

| Mảnh | Nghĩa |
|---|---|
| `typedef struct { ... } Client;` | Struct ẩn danh được đặt tên kiểu `Client` → viết `Client c;` thay vì `struct ... c;` |
| `char name[32];` | Mảng **nằm ngay trong** struct (không phải con trỏ) — mỗi client có 32 byte riêng |
| `char buf[MAX_LINE];` | Kích thước lấy từ macro → được thay bằng `512` trước khi biên dịch |
| `static Client clients[MAX_CLIENTS];` | Mảng **toàn cục chỉ dùng trong file này**; biến toàn cục/`static` được khởi tạo bằng 0 |

### 16.2 Gửi và xóa phần tử

```c
/* trích từ chat_server.c */
static void send_all(socket_t s, const char *data, size_t len) {
        net_ssize_t n = send(s, data, (int)len, 0);
        data += n;
        len -= (size_t)n;
    clients[i] = clients[nclients - 1];
        snprintf(clients[i].name, sizeof clients[i].name, "%.31s", line + 6);
    if (n > 0 && line[n - 1] == '\r') line[--n] = '\0';
```

| Mảnh | Nghĩa |
|---|---|
| `const char *data` | Hàm **chỉ đọc** dữ liệu mà `data` trỏ tới (nhưng được dời con trỏ `data`) |
| `(int)len` | Ép `size_t` sang `int` vì `send` của Winsock nhận `int` |
| `data += n;` | **Số học con trỏ**: dời con trỏ tới sau `n` byte vừa gửi |
| `len -= (size_t)n;` | Ép `n` (có dấu) sang không dấu trước khi trừ — tránh cảnh báo so sánh/trộn dấu |
| `clients[i] = clients[nclients - 1];` | **Gán struct**: sao chép **toàn bộ** các trường (kể cả hai mảng) chỉ bằng một dấu `=` |
| `sizeof clients[i].name` | Kích thước của trường mảng = 32 |
| `"%.31s"` | In **tối đa 31 ký tự** (chừa chỗ cho `'\0'`) |
| `line + 6` | Con trỏ tới ký tự thứ 7 — tức phần **sau** `"/nick "` |
| `line[--n] = '\0'` | Bỏ ký tự `\r` ở cuối bằng cách thay nó bằng ký tự kết thúc chuỗi |

### 16.3 Tách dòng trong bộ đệm

```c
/* trích từ chat_server.c */
    Client *c = &clients[i];
    net_ssize_t n = recv(c->fd, c->buf + c->len, (int)(sizeof c->buf - c->len), 0);
    char *start = c->buf;
    while ((nl = memchr(start, '\n', c->len - (size_t)(start - c->buf))) != NULL) {
        *nl = '\0';
        start = nl + 1;
    c->len -= (size_t)(start - c->buf);
    memmove(c->buf, start, c->len);
```

| Mảnh | Nghĩa |
|---|---|
| `Client *c = &clients[i];` | `c` trỏ vào phần tử thứ `i` — sửa qua `c` là sửa thẳng phần tử trong mảng |
| `c->fd` | Trường `fd` của struct mà `c` trỏ tới (≡ `(*c).fd`) |
| `c->buf + c->len` | Mảng `buf` suy biến thành con trỏ, cộng thêm `len` → trỏ tới **chỗ trống đầu tiên** trong bộ đệm |
| `sizeof c->buf - c->len` | Số byte **còn trống**: `sizeof` áp dụng cho `c->buf` (512), rồi mới trừ |
| `char *start = c->buf;` | Con trỏ chạy, bắt đầu ở đầu bộ đệm |
| `start - c->buf` | **Hiệu hai con trỏ** = số byte đã xử lý |
| `while ((nl = memchr(...)) != NULL)` | Gán kết quả tìm `'\n'` vào `nl`, rồi so sánh với `NULL` — lặp khi còn tìm thấy |
| `*nl = '\0';` | Ghi đè `'\n'` bằng ký tự kết thúc → `start` giờ là một **chuỗi C hoàn chỉnh** |
| `start = nl + 1;` | Nhảy tới đầu dòng kế tiếp |
| `memmove(c->buf, start, c->len)` | Dời phần còn dang dở về đầu bộ đệm; `memmove` (không phải `memcpy`) vì hai vùng nhớ **có thể chồng nhau** |

### 16.4 Mở socket và vòng lặp `poll`

```c
/* trích từ chat_server.c */
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    if (getaddrinfo(NULL, port, &hints, &res) != 0) return NET_INVALID;
    if (s == NET_INVALID || bind(s, res->ai_addr, (socklen_t)res->ai_addrlen) != 0 ||
    struct pollfd fds[MAX_CLIENTS + 1];
    for (;;) {
            if (fds[i + 1].revents & (POLLIN | POLLHUP | POLLERR)) handle_readable(i);
            Client *c = &clients[nclients++];
            snprintf(c->name, sizeof c->name, "khach%d", next_id++);
```

| Mảnh | Nghĩa |
|---|---|
| `struct addrinfo hints, *res;` | Khai báo **hai** biến: `hints` là struct, `res` là **con trỏ** tới struct (dấu `*` chỉ gắn với `res`!) |
| `memset(&hints, 0, sizeof hints)` | Xóa `hints` về 0 trước khi điền các trường cần thiết |
| `&res` | Truyền **địa chỉ của con trỏ** để `getaddrinfo` ghi kết quả vào `res` (tham số kiểu `struct addrinfo **`) |
| `a \|\| b \|\| c` | Đoản mạch: nếu `socket` đã lỗi (`s == NET_INVALID`), `bind` và `listen` **không được gọi** |
| `res->ai_addr` | Trường `ai_addr` (kiểu `struct sockaddr *`) của struct mà `res` trỏ tới |
| `struct pollfd fds[MAX_CLIENTS + 1];` | Mảng 33 phần tử: 1 cho socket lắng nghe + 32 cho client |
| `for (;;)` | Vòng lặp vô hạn |
| `revents & (POLLIN \| POLLHUP \| POLLERR)` | Gộp ba cờ bằng `\|`, rồi kiểm tra xem **có cờ nào** trong số đó được bật không |
| `&clients[nclients++]` | Lấy địa chỉ phần tử ở vị trí `nclients` **hiện tại**, **sau đó** tăng `nclients` (hậu tố `++`) |
| `next_id++` | Dùng giá trị hiện tại cho tên, rồi tăng bộ đếm |

---

## Chương 17. Giải mã `socket.hpp` và `chat_client.cpp`

### 17.1 Hàm và ngoại lệ

```cpp
// trích từ socket.hpp
namespace net {
inline std::error_code last_error() {
    return {net_last_error(), std::system_category()};
[[noreturn]] inline void throw_last_error(const char* what) {
    throw std::system_error(last_error(), what);
```

| Mảnh | Nghĩa |
|---|---|
| `namespace net {` | Mọi tên bên trong có tiền tố `net::` (ví dụ `net::Socket`) |
| `inline` (hàm trong header C++) | Cho phép định nghĩa hàm trong header được include ở nhiều file mà không lỗi "định nghĩa trùng" |
| `std::error_code` | Kiểu chuẩn đại diện cho "mã lỗi + loại mã lỗi" |
| `return {a, b};` | Tạo đối tượng kiểu trả về từ `{a, b}` — gọi constructor `error_code(int, const error_category&)` |
| `std::system_category()` | Loại lỗi của hệ điều hành — biết cách chuyển mã lỗi Windows thành thông báo |
| `[[noreturn]]` | Hàm luôn **ném ngoại lệ**, không bao giờ trả về bình thường |
| `throw std::system_error(...)` | Ném ngoại lệ chứa mã lỗi và mô tả |

### 17.2 Lớp `Socket`

```cpp
// trích từ socket.hpp
class Socket {
    Socket() = default;
    explicit Socket(socket_t fd) noexcept : fd_(fd) {}
    ~Socket() { reset(); }
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other) noexcept : fd_(std::exchange(other.fd_, NET_INVALID)) {}
    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
        return *this;
    socket_t get() const noexcept { return fd_; }
    explicit operator bool() const noexcept { return fd_ != NET_INVALID; }
    void send_all(std::string_view data) const {
            net_ssize_t n = ::send(fd_, data.data(), static_cast<int>(data.size()), 0);
            data.remove_prefix(static_cast<std::size_t>(n));
    socket_t fd_ = NET_INVALID;
```

| Mảnh | Nghĩa |
|---|---|
| `Socket() = default;` | Constructor mặc định do trình biên dịch sinh (dùng giá trị mặc định `fd_ = NET_INVALID`) |
| `explicit Socket(socket_t fd)` | Tạo từ một socket thô; `explicit` cấm viết `Socket s = 5;` |
| `noexcept` | Hứa không ném ngoại lệ |
| `: fd_(fd)` | Danh sách khởi tạo: gán `fd` cho thành viên `fd_` |
| `~Socket() { reset(); }` | Hàm hủy: tự đóng socket khi đối tượng bị hủy (**RAII**) |
| `Socket(const Socket&) = delete;` | **Cấm sao chép** — tránh hai đối tượng cùng đóng một socket |
| `Socket(Socket&& other)` | Constructor **di chuyển**: nhận một `Socket` sắp bị bỏ đi |
| `std::exchange(other.fd_, NET_INVALID)` | Gán `NET_INVALID` vào `other.fd_` và **trả về giá trị cũ** → "lấy" socket từ `other` và để `other` rỗng, trong một biểu thức |
| `Socket& operator=(Socket&& other)` | Toán tử gán di chuyển; trả về `Socket&` để viết nối được `a = b = c` |
| `if (this != &other)` | Chống **tự gán** (`s = std::move(s)`) |
| `return *this;` | `this` là con trỏ → `*this` là chính đối tượng |
| `socket_t get() const` | Hàm không sửa đối tượng |
| `explicit operator bool() const` | Cho phép `if (!s)` — xem socket có hợp lệ không |
| `std::string_view data` | Nhận **mọi** loại chuỗi mà không sao chép |
| `::send(...)` | `::` đứng đầu: gọi hàm `send` **toàn cục** (của hệ điều hành), tránh nhầm với thành viên `send_all` hay tên khác trong namespace |
| `data.data()` / `data.size()` | Con trỏ tới ký tự đầu / độ dài |
| `static_cast<int>(...)` | Ép kiểu tường minh kiểu C++ |
| `data.remove_prefix(n)` | "Cắt" `n` ký tự đầu khỏi view (không sửa chuỗi gốc) |
| `socket_t fd_ = NET_INVALID;` | Giá trị mặc định của thành viên; hậu tố `_` là **quy ước đặt tên** cho thành viên private |

### 17.3 Hàm `connect_tcp`

```cpp
// trích từ socket.hpp
inline Socket connect_tcp(const std::string& host, const std::string& port) {
    addrinfo hints{};
    addrinfo* res = nullptr;
    if (int err = ::getaddrinfo(host.c_str(), port.c_str(), &hints, &res); err != 0)
        throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(err));
    for (addrinfo* p = res; p != nullptr; p = p->ai_next) {
        Socket s(::socket(p->ai_family, p->ai_socktype, p->ai_protocol));
        if (!s) { last = last_error(); continue; }
            return s;
```

| Mảnh | Nghĩa |
|---|---|
| `const std::string& host` | Nhận chuỗi theo **tham chiếu hằng** — không sao chép, không sửa |
| `addrinfo hints{};` | Khởi tạo **mọi trường bằng 0** (thay cho `memset` của C). Trong C++ không cần viết `struct addrinfo` |
| `addrinfo* res = nullptr;` | Con trỏ khởi tạo rỗng |
| `if (int err = ...; err != 0)` | `if` có câu lệnh khởi tạo: `err` chỉ tồn tại trong `if` |
| `host.c_str()` | Lấy `const char*` từ `std::string` để truyền cho hàm C |
| `std::string("getaddrinfo: ") + gai_strerror(err)` | Phải có ít nhất một vế là `std::string` thì `+` mới là **nối chuỗi** (hai `const char*` không cộng được) |
| `for (addrinfo* p = res; p != nullptr; p = p->ai_next)` | Duyệt **danh sách liên kết** kết quả |
| `Socket s(::socket(...));` | Tạo đối tượng `Socket` sở hữu socket vừa mở |
| `if (!s)` | Gọi `explicit operator bool` — socket không hợp lệ? |
| `continue;` | Bỏ qua phần còn lại, sang địa chỉ tiếp theo — và `s` bị hủy (tự đóng) |
| `return s;` | Trả về một biến cục bộ **chỉ di chuyển được**: trình biên dịch tự **di chuyển** (hoặc bỏ qua việc sao chép), không cần `std::move` |

### 17.4 `chat_client.cpp`

```cpp
// trích từ chat_client.cpp
        net::Init init;
        net::Socket sock = net::connect_tcp(argv[1], argv[2]);
        std::thread receiver([&sock] {
                while (std::size_t n = sock.recv_some(buf, sizeof buf))
                    std::cout.write(buf, static_cast<std::streamsize>(n)).flush();
            } catch (const std::system_error& e) {
        while (std::getline(std::cin, line)) {
            sock.send_all(line + "\n");
        receiver.join();
    } catch (const std::exception& e) {
```

| Mảnh | Nghĩa |
|---|---|
| `net::Init init;` | Tạo đối tượng chỉ để **chạy constructor** (`WSAStartup`) và **hàm hủy** (`WSACleanup`) khi `main` kết thúc — không dùng biến `init` ở đâu khác |
| `net::Socket sock = net::connect_tcp(...)` | Nhận quyền sở hữu socket từ giá trị trả về (di chuyển) |
| `argv[1]` | Tham số dòng lệnh thứ nhất (`char*`), tự chuyển thành `std::string` khi truyền vào hàm |
| `std::thread receiver([&sock] { ... });` | Tạo luồng chạy **lambda**; lambda bắt `sock` theo **tham chiếu** (an toàn vì `sock` sống lâu hơn luồng — luồng được `join` trước khi `sock` bị hủy) |
| `while (std::size_t n = sock.recv_some(...))` | Khai báo biến **ngay trong điều kiện** `while`; lặp khi `n != 0` (0 = server đóng kết nối) |
| `sizeof buf` | Kích thước mảng `buf` (1024) |
| `std::cout.write(buf, n)` | Ghi đúng `n` byte (dữ liệu không kết thúc bằng `'\0'`) |
| `static_cast<std::streamsize>(n)` | `write` nhận kiểu `std::streamsize` (có dấu) |
| `.flush()` | **Nối lời gọi**: `write` trả về chính `std::cout`, rồi gọi tiếp `flush` để in ngay |
| `catch (const std::system_error& e)` | Bắt ngoại lệ theo **tham chiếu hằng** (tránh sao chép và tránh "cắt" đối tượng) |
| `while (std::getline(std::cin, line))` | Đọc từng dòng; `getline` trả về luồng, luồng chuyển thành `false` khi hết dữ liệu |
| `line + "\n"` | Nối `std::string` với chuỗi C → `std::string` mới |
| `receiver.join()` | Chờ luồng nhận kết thúc |
| `catch (const std::exception& e)` | Bắt mọi ngoại lệ chuẩn — `std::system_error` và `std::runtime_error` đều kế thừa từ `std::exception` |

---

# PHỤ LỤC

## Phụ lục A. Độ ưu tiên toán tử (từ cao xuống thấp, rút gọn)

| Mức | Toán tử | Kết hợp |
|---|---|---|
| 1 | `::` (C++) | trái → phải |
| 2 | `a++` `a--` `f()` `a[i]` `.` `->` | trái → phải |
| 3 | `++a` `--a` `+a` `-a` `!` `~` `(type)` `*p` `&x` `sizeof` `new` `delete` | **phải → trái** |
| 4 | `.*` `->*` (C++) | trái → phải |
| 5 | `*` `/` `%` | trái → phải |
| 6 | `+` `-` | trái → phải |
| 7 | `<<` `>>` | trái → phải |
| 8 | `<=>` (C++20) | trái → phải |
| 9 | `<` `<=` `>` `>=` | trái → phải |
| 10 | `==` `!=` | trái → phải |
| 11 | `&` | trái → phải |
| 12 | `^` | trái → phải |
| 13 | `\|` | trái → phải |
| 14 | `&&` | trái → phải |
| 15 | `\|\|` | trái → phải |
| 16 | `?:` `=` `+=` `-=` … `throw` | **phải → trái** |
| 17 | `,` | trái → phải |

**Những bẫy hay gặp:**

- `*p++` = `*(p++)`: lấy giá trị tại `p`, **rồi** tăng **con trỏ** (không phải tăng giá trị).
- `(*p)++`: tăng **giá trị** tại `p`.
- `flags & MASK == 0` = `flags & (MASK == 0)`.
- `a << 1 + 1` = `a << 2`.
- `std::cout << a & b` = `(std::cout << a) & b` → lỗi biên dịch; viết `std::cout << (a & b)`.

**Quy tắc thực tế:** nếu phải suy nghĩ về độ ưu tiên, hãy **thêm ngoặc**.

## Phụ lục B. Chuỗi định dạng `printf`

| Định dạng | Kiểu | Định dạng | Kiểu |
|---|---|---|---|
| `%d` `%i` | `int` | `%u` | `unsigned` |
| `%ld` `%lld` | `long`, `long long` | `%lu` `%llu` | bản không dấu |
| `%zu` / `%zd` | `size_t` / `ssize_t` | `%td` | `ptrdiff_t` |
| `%f` `%.2f` | `double` (float tự đổi sang double) | `%e` `%g` | dạng khoa học / tự chọn |
| `%c` | ký tự | `%s` `%.*s` | chuỗi / chuỗi có độ dài |
| `%p` | con trỏ (`void *`) | `%x` `%X` `%#x` `%08x` | hex / hex có `0x` / đệm 0 |
| `%%` | ký tự `%` | `PRId32`, `PRIu64` | cho `int32_t`, `uint64_t`: `printf("%" PRIu64, v)` |

Độ rộng và căn lề: `%5d` (rộng 5, căn phải), `%-10s` (căn trái), `%05.1f` (đệm 0).

> ⚠️ Với `scanf`, `double` dùng `%lf` (khác `printf`). Luôn giới hạn độ dài chuỗi: `%31s`.

## Phụ lục C. Khác biệt đáng nhớ giữa C và C++

| Chủ đề | C | C++ |
|---|---|---|
| `void *` → `int *` | Tự chuyển: `int *p = malloc(...)` | **Phải ép**: `static_cast<int*>(malloc(...))` (và nên dùng `new`/container) |
| `struct Tag` | Phải viết `struct Tag x;` (trừ khi `typedef`) | Viết `Tag x;` |
| `int f();` | Không rõ tham số (trước C23) | Không nhận tham số |
| Compound literal `(Vec2){1, 2}` | Có (C99) | Không có trong chuẩn (GCC hỗ trợ như phần mở rộng) → dùng `Vec2{1, 2}` |
| Designated initializer | Tự do thứ tự, có cả `[i] = ...` | C++20: phải **đúng thứ tự** khai báo, không có dạng mảng |
| `'a'` | Kiểu `int` | Kiểu `char` |
| `bool`, `true`, `false` | `<stdbool.h>` (từ khóa từ C23) | Từ khóa |
| `NULL` | Macro | Dùng `nullptr` |
| Union đổi trường để "xem bit" | Được phép | UB — dùng `std::bit_cast` hoặc `memcpy` |
| Tên hàm trong file object | Giữ nguyên | Bị "trang trí" (cần `extern "C"` để gọi qua lại) |
| Flexible array member `char data[];` | Có (C99) | Không có trong chuẩn |
| `restrict` | Có | Không có (GCC/Clang có `__restrict`) |

## Phụ lục D. Công cụ giải mã

| Công cụ | Dùng để |
|---|---|
| **cppreference.com** | Tra cứu chính xác mọi từ khóa, hàm thư viện C và C++ |
| **cdecl.org** | Dịch khai báo C phức tạp sang lời |
| **C++ Insights** (cppinsights.io) | Xem trình biên dịch "viết lại" code C++ của bạn thế nào (range-for, lambda, `auto`, template...) |
| **Compiler Explorer** (godbolt.org) | Thử code nhanh trên nhiều trình biên dịch |
| `gcc -E file.c` | Xem code **sau khi** tiền xử lý (macro đã được thay thế) |
| `-Wall -Wextra -pedantic` | Để trình biên dịch chỉ ra cú pháp đáng ngờ |
| `static_assert(std::is_same_v<decltype(x), T>)` | Tự kiểm tra kiểu mà `auto`/template suy ra |
| IDE (VS Code, CLion, Visual Studio) | Rê chuột để xem kiểu thật của biến; "Go to definition" để xem macro/hàm được định nghĩa thế nào |

## Phụ lục E. Cách luyện đọc code lạ

1. **Tìm từ khóa và ký hiệu** trong bảng 0.3 trước — xác định mỗi ký hiệu đang mang nghĩa nào.
2. **Tách dòng dài** thành từng mảnh như Phần III; với khai báo, dùng quy tắc phải – trái (Chương 4).
3. **Gặp tên lạ** (`NET_INVALID`, `net_poll`) → "Go to definition": rất có thể đó là **macro** hoặc **typedef**.
4. **Gặp template khó** → hỏi: "tham số template là gì? kiểu thật là gì?" — thử ghi rõ kiểu ra.
5. **Chép đoạn code ra một file nhỏ**, thêm `printf`/`std::cout` hoặc `static_assert` để kiểm chứng suy đoán của mình — đúng như cách tài liệu này được kiểm tra.
6. Đọc lại các chương trình trong các tài liệu trước (socket, hệ điều hành, C/C++): giờ bạn đã có thể tự "giải mã" từng dòng.

---

*Chúc bạn học tốt! Cú pháp chỉ khó khi gặp lần đầu — sau vài lần tra bảng và tự viết lại, những dòng như `while ((nl = memchr(...)) != NULL)` hay `std::exchange(other.fd_, NET_INVALID)` sẽ trở nên tự nhiên.*
