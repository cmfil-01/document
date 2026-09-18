# LẬP TRÌNH C CƠ BẢN & C++ NÂNG CAO

*Tài liệu tự học — hiểu chắc nền tảng, viết được code thật*

---

## 0. Cách dùng tài liệu này

### 0.1 Cấu trúc

| Phần | Nội dung | Mục tiêu |
|---|---|---|
| **Phần I – C cơ bản** (Chương 1–11) | Kiểu dữ liệu, con trỏ, bộ nhớ, struct, file | Hiểu **máy tính thực sự làm gì** với code của bạn |
| **Phần II – C++ nâng cao** (Chương 12–24) | OOP, RAII, move, smart pointer, template, STL, lambda, đa luồng | Viết code **an toàn, gọn, hiện đại** |
| **Phụ lục** | Lỗi kinh điển, công cụ, lộ trình | Tra cứu nhanh |

Vì sao học C trước? C++ được xây trên nền C. Khi đã hiểu con trỏ, stack/heap và việc tự quản lý bộ nhớ trong C, bạn sẽ hiểu **tại sao** C++ lại có `std::vector`, `std::unique_ptr` hay RAII — thay vì chỉ học thuộc cách dùng.

Mỗi chương đi theo mạch: **ý tưởng → cú pháp → chương trình hoàn chỉnh chạy được → lỗi hay gặp → tự kiểm tra**. Cuối mỗi phần có **bài tập lập trình kèm lời giải**.

> ✅ Mọi chương trình hoàn chỉnh trong tài liệu (có hàm `main`) đều đã được biên dịch với GCC 13 (bật `-Wall -Wextra -pedantic` và AddressSanitizer/UBSan), và phần **Kết quả** chính là output chạy thật trên Linux 64-bit.

### 0.2 Chuẩn bị môi trường

Bạn cần một trình biên dịch:

- **Linux:** `sudo apt install build-essential` (có sẵn `gcc`, `g++`)
- **Windows:** MSYS2 (MinGW-w64) hoặc Visual Studio (MSVC)
- **macOS:** `xcode-select --install` (có `clang`)

**Lệnh biên dịch khuyên dùng khi học:**

```bash
# C
gcc -std=c17 -Wall -Wextra -g main.c -o main

# C++
g++ -std=c++20 -Wall -Wextra -g main.cpp -o main

# Chạy
./main          # Linux/macOS
main.exe        # Windows
```

| Cờ | Ý nghĩa |
|---|---|
| `-std=c17` / `-std=c++20` | Chọn phiên bản ngôn ngữ |
| `-Wall -Wextra` | Bật gần như mọi cảnh báo — **luôn bật khi học** |
| `-g` | Thêm thông tin gỡ lỗi (để dùng `gdb`) |
| `-fsanitize=address,undefined` | Bắt lỗi bộ nhớ và hành vi không xác định lúc chạy (GCC/Clang) |

> 💡 **Mẹo:** Các chương trình mẫu in chữ **không dấu** để tránh lỗi hiển thị trên console Windows. Chú thích trong code vẫn viết tiếng Việt có dấu.

---

# PHẦN I — C CƠ BẢN

## Chương 1. Chương trình đầu tiên và quá trình biên dịch

### 1.1 Hello, C

```c
#include <stdio.h>          // thư viện nhập/xuất chuẩn (có printf)

int main(void) {            // điểm bắt đầu của mọi chương trình C
    printf("Xin chao, C!\n");
    return 0;               // 0 = kết thúc thành công
}
```

**Kết quả:**

```text
Xin chao, C!
```

Giải thích từng dòng:

- `#include <stdio.h>`: chèn nội dung file header `stdio.h` vào — trong đó khai báo hàm `printf`.
- `int main(void)`: hàm `main` trả về một số nguyên cho hệ điều hành; `void` nghĩa là không nhận tham số.
- `printf(...)`: in ra màn hình; `\n` là ký tự xuống dòng.
- `return 0;`: báo cho hệ điều hành biết chương trình chạy thành công.

### 1.2 Từ file `.c` đến file chạy được

Lệnh `gcc main.c -o main` thực ra làm 4 bước:

```text
main.c ──(1) Tiền xử lý──► main.i ──(2) Biên dịch──► main.s ──(3) Hợp dịch──► main.o ──(4) Liên kết──► main
         xử lý #include,            dịch sang               dịch sang              ghép với thư viện
         #define                    hợp ngữ                 mã máy                 (printf nằm ở libc)
```

Bạn có thể dừng ở từng bước để xem:

```bash
gcc -E main.c -o main.i     # chỉ tiền xử lý
gcc -S main.c -o main.s     # dừng sau khi dịch sang hợp ngữ
gcc -c main.c -o main.o     # dừng ở file object
gcc main.o -o main          # chỉ liên kết
```

Hiểu 4 bước này giúp bạn đọc được thông báo lỗi:

- Lỗi cú pháp, sai kiểu → lỗi ở bước **biên dịch** (compiler error).
- `undefined reference to 'foo'` → lỗi ở bước **liên kết** (linker error): hàm đã khai báo nhưng không tìm thấy phần định nghĩa.

### ✅ Tự kiểm tra

1. Quên `#include <stdio.h>` thì lỗi xảy ra ở bước nào?
2. Khai báo `int foo(void);` và gọi `foo()` nhưng không viết thân hàm — lỗi ở bước nào?

<details><summary>Đáp án</summary>

1. Bước biên dịch: trình biên dịch không biết `printf` là gì (thường báo "implicit declaration").
2. Bước liên kết: `undefined reference to 'foo'`.

</details>

---

## Chương 2. Kiểu dữ liệu, biến và hằng

### 2.1 Các kiểu cơ bản

| Kiểu | Kích thước thường gặp | Dùng cho |
|---|---|---|
| `char` | 1 byte | Ký tự, byte |
| `int` | 4 byte | Số nguyên thông thường |
| `long` | 8 byte (Linux 64-bit), **4 byte (Windows)** | Số nguyên lớn — kích thước không nhất quán! |
| `long long` | 8 byte | Số nguyên rất lớn |
| `float` | 4 byte | Số thực, độ chính xác ~7 chữ số |
| `double` | 8 byte | Số thực, độ chính xác ~15 chữ số — **mặc định nên dùng** |
| `bool` (cần `<stdbool.h>`) | 1 byte | Đúng/sai |

Mỗi kiểu nguyên có bản `unsigned` (không âm): `unsigned int` chứa được 0 đến 4 294 967 295.

**Khi cần kích thước chính xác** (lập trình mạng, file nhị phân, game), dùng `<stdint.h>`: `int8_t`, `uint8_t`, `int32_t`, `uint64_t`...

```c
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    printf("char    : %zu byte\n", sizeof(char));
    printf("int     : %zu byte\n", sizeof(int));
    printf("long    : %zu byte\n", sizeof(long));
    printf("double  : %zu byte\n", sizeof(double));
    printf("int32_t : %zu byte\n", sizeof(int32_t));
    printf("int *   : %zu byte\n", sizeof(int *));

    bool ok = true;
    printf("bool    : %zu byte, ok = %d\n", sizeof(bool), ok);
    return 0;
}
```

**Kết quả** (Linux 64-bit):

```text
char    : 1 byte
int     : 4 byte
long    : 8 byte
double  : 8 byte
int32_t : 4 byte
int *   : 8 byte
bool    : 1 byte, ok = 1
```

`sizeof` trả về kiểu `size_t` (số nguyên không âm), in bằng `%zu`.

### 2.2 Chuỗi định dạng `printf` / `scanf`

| Định dạng | Kiểu | Ghi chú |
|---|---|---|
| `%d` | `int` | |
| `%u` | `unsigned int` | |
| `%ld`, `%lld` | `long`, `long long` | |
| `%zu` | `size_t` | |
| `%f` | `double` (với `printf`) | `%.2f` = 2 chữ số thập phân |
| `%lf` | `double` (với `scanf`) | **Khác với `printf`** — hay nhầm |
| `%c` | `char` | |
| `%s` | chuỗi `char *` | |
| `%p` | con trỏ | |
| `%x` | số hex | |

> ⚠️ Dùng sai định dạng (ví dụ in `int` bằng `%f`) là **hành vi không xác định** — chương trình có thể in rác. Luôn bật `-Wall`, trình biên dịch sẽ cảnh báo.

### 2.3 Phép chia, ép kiểu và tràn số

```c
#include <stdio.h>

int main(void) {
    int a = 7, b = 2;
    printf("a / b = %d\n", a / b);                   // chia nguyên: bỏ phần thập phân
    printf("a %% b = %d\n", a % b);                  // chia lấy dư (%% để in dấu %)
    printf("(double)a / b = %.2f\n", (double)a / b); // ép kiểu trước khi chia

    char c = 'A';
    printf("%c = %d, c + 2 = %c\n", c, c, c + 2);    // char thực chất là số nguyên

    unsigned int u = 0;
    u = u - 1;                                       // unsigned "quay vòng"
    printf("0u - 1 = %u\n", u);
    return 0;
}
```

**Kết quả:**

```text
a / b = 3
a % b = 1
(double)a / b = 3.50
A = 65, c + 2 = C
0u - 1 = 4294967295
```

**Ghi nhớ:**

- `int / int` luôn cho kết quả nguyên. Muốn ra số thực, **ép một vế** sang `double` trước khi chia.
- Số **không dấu** tràn thì quay vòng (được định nghĩa rõ).
- Số **có dấu** tràn (ví dụ `INT_MAX + 1`) là **hành vi không xác định (UB)** — không được dựa vào nó.

### 2.4 Hằng số

```c
#define MAX_PLAYERS 4           // macro: tiền xử lý thay thế văn bản, không có kiểu
const double GRAVITY = 9.81;    // biến hằng: có kiểu, trình biên dịch kiểm tra được
```

Trong C, ưu tiên `const` khi có thể; `#define` hay dùng cho kích thước mảng hoặc hằng dùng trong `#if`.

### 2.5 Nhập dữ liệu với `scanf`

```c
#include <stdio.h>

int main(void) {
    int age;
    double height;
    printf("Nhap tuoi va chieu cao: ");
    if (scanf("%d %lf", &age, &height) != 2) {   // scanf trả về số biến đọc thành công
        printf("Du lieu khong hop le\n");
        return 1;
    }
    printf("Tuoi %d, cao %.2f m\n", age, height);
    return 0;
}
```

Chú ý dấu `&` trước tên biến: `scanf` cần **địa chỉ** của biến để ghi giá trị vào (sẽ hiểu rõ ở Chương 7). Luôn **kiểm tra giá trị trả về** của `scanf`.

### ✅ Tự kiểm tra

1. `printf("%d", 5 / 2 * 2);` in ra gì?
2. Vì sao nên dùng `int32_t` thay cho `long` khi ghi dữ liệu ra file nhị phân?

<details><summary>Đáp án</summary>

1. `4`, vì `5 / 2 = 2` (chia nguyên), rồi `2 * 2 = 4`.
2. `long` có thể là 4 hoặc 8 byte tùy hệ điều hành; `int32_t` luôn đúng 4 byte, nên file đọc được trên mọi máy.

</details>

---

## Chương 3. Toán tử và biểu thức

### 3.1 Các nhóm toán tử

| Nhóm | Toán tử |
|---|---|
| Số học | `+ - * / %` |
| So sánh | `== != < > <= >=` (kết quả là `1` hoặc `0`) |
| Logic | `&&` (và), `\|\|` (hoặc), `!` (phủ định) |
| Bit | `&` (AND), `\|` (OR), `^` (XOR), `~` (NOT), `<<`, `>>` (dịch bit) |
| Gán | `= += -= *= /= %= &= \|= ^= <<= >>=` |
| Tăng/giảm | `++ --` |
| Điều kiện | `điều_kiện ? a : b` |

### 3.2 `i++` và `++i`

```c
int i = 5;
int a = i++;   // a = 5, sau đó i = 6  (dùng giá trị cũ rồi mới tăng)
int b = ++i;   // i = 7, rồi b = 7     (tăng trước rồi mới dùng)
```

> ⚠️ Đừng viết `i = i++ + ++i;` — sửa một biến nhiều lần trong cùng biểu thức là **UB**.

### 3.3 Đoản mạch (short-circuit)

Với `A && B`: nếu `A` sai thì **không tính** `B`. Với `A || B`: nếu `A` đúng thì không tính `B`. Nhờ vậy ta viết được điều kiện an toàn:

```c
if (p != NULL && p->hp > 0) { ... }   // nếu p là NULL, p->hp không bao giờ được truy cập
```

### 3.4 Toán tử bit — công cụ quen thuộc trong game và hệ thống

Mỗi bit trong một số nguyên có thể dùng làm một "công tắc" (cờ):

```c
#include <stdio.h>

#define FLAG_VISIBLE (1u << 0)   // ...0001
#define FLAG_SOLID   (1u << 1)   // ...0010
#define FLAG_ENEMY   (1u << 2)   // ...0100

int main(void) {
    unsigned flags = 0;

    flags |= FLAG_VISIBLE | FLAG_ENEMY;          // BẬT hai cờ
    printf("flags = %u\n", flags);

    if (flags & FLAG_ENEMY)                      // KIỂM TRA một cờ
        printf("Doi tuong la ke dich\n");

    flags &= ~FLAG_VISIBLE;                      // TẮT một cờ
    printf("flags = %u\n", flags);

    flags ^= FLAG_SOLID;                         // ĐẢO một cờ
    printf("flags = %u\n", flags);

    printf("5 << 3 = %d, 40 >> 2 = %d\n", 5 << 3, 40 >> 2);   // nhân/chia cho lũy thừa của 2
    return 0;
}
```

**Kết quả:**

```text
flags = 5
Doi tuong la ke dich
flags = 4
flags = 6
5 << 3 = 40, 40 >> 2 = 10
```

| Việc cần làm | Cú pháp |
|---|---|
| Bật cờ | `flags \|= MASK` |
| Tắt cờ | `flags &= ~MASK` |
| Đảo cờ | `flags ^= MASK` |
| Kiểm tra cờ | `if (flags & MASK)` |

### 3.5 Bẫy độ ưu tiên toán tử

```c
if (flags & MASK == 0)     // SAI: == ưu tiên cao hơn &, thành flags & (MASK == 0)
if ((flags & MASK) == 0)   // ĐÚNG

if (x = 5)                 // SAI: gán chứ không phải so sánh, luôn đúng
if (x == 5)                // ĐÚNG
```

**Quy tắc vàng:** không chắc thứ tự thì **thêm ngoặc**.

### ✅ Tự kiểm tra

1. Giá trị của `0x0F & 0x3C` là bao nhiêu?
2. Viết biểu thức kiểm tra số nguyên `n` có phải số chẵn bằng toán tử bit.

<details><summary>Đáp án</summary>

1. `0000 1111 & 0011 1100 = 0000 1100 = 0x0C = 12`.
2. `(n & 1) == 0`.

</details>

---

## Chương 4. Cấu trúc điều khiển

```c
#include <stdio.h>

int main(void) {
    // for: biết trước số lần lặp
    int sum = 0;
    for (int i = 1; i <= 100; i++) sum += i;
    printf("Tong 1..100 = %d\n", sum);

    // while: lặp khi điều kiện còn đúng
    int n = 90210, digits = 0;
    while (n > 0) {
        n /= 10;
        digits++;
    }
    printf("So chu so = %d\n", digits);

    // do-while: thân vòng lặp chạy ÍT NHẤT MỘT LẦN
    int k = 10;
    do {
        printf("k = %d\n", k);
        k++;
    } while (k < 10);

    // switch: rẽ nhánh theo giá trị nguyên
    int day = 6;
    switch (day) {
    case 6:                       // không có break -> "rơi" xuống case 7
    case 7:
        printf("Cuoi tuan\n");
        break;
    default:
        printf("Ngay thuong\n");
        break;
    }

    // continue: bỏ qua phần còn lại của lần lặp; break: thoát vòng lặp
    for (int i = 0; i < 10; i++) {
        if (i % 2 == 0) continue;
        if (i == 7) break;
        printf("%d ", i);
    }
    printf("\n");
    return 0;
}
```

**Kết quả:**

```text
Tong 1..100 = 5050
So chu so = 5
k = 10
Cuoi tuan
1 3 5
```

**Lưu ý với `switch`:**

- Chỉ dùng được với giá trị **nguyên** (`int`, `char`, `enum`), không dùng với chuỗi.
- **Quên `break`** là lỗi rất phổ biến: code sẽ chạy tiếp sang `case` bên dưới. Nếu cố ý "rơi" như ví dụ trên, hãy để các `case` liền nhau hoặc ghi chú rõ.

**Chọn vòng lặp nào?**

| Tình huống | Vòng lặp |
|---|---|
| Biết trước số lần (duyệt mảng) | `for` |
| Lặp đến khi một điều kiện thay đổi | `while` |
| Phải chạy ít nhất một lần (menu, nhập lại dữ liệu) | `do-while` |

### ✅ Tự kiểm tra

Đoạn code sau in ra gì?

```c
for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) {
        if (j == i) break;
        printf("%d%d ", i, j);
    }
```

<details><summary>Đáp án</summary>

`10 20 21`. `break` chỉ thoát vòng lặp **trong cùng** (vòng `j`).

</details>

---

## Chương 5. Hàm

### 5.1 Khai báo, định nghĩa, gọi hàm

```c
#include <stdio.h>

// Khai báo nguyên mẫu (prototype): cho trình biên dịch biết hàm tồn tại
int max2(int a, int b);
long long factorial(int n);
void try_change(int x);
int next_id(void);

int main(void) {
    printf("max2(3, 8) = %d\n", max2(3, 8));
    printf("10! = %lld\n", factorial(10));

    int v = 5;
    try_change(v);
    printf("v sau khi goi ham = %d\n", v);

    next_id();
    next_id();
    printf("next_id() lan 3 = %d\n", next_id());
    return 0;
}

// Định nghĩa hàm
int max2(int a, int b) {
    return a > b ? a : b;
}

long long factorial(int n) {
    if (n <= 1) return 1;              // điều kiện dừng — thiếu nó sẽ đệ quy vô hạn
    return n * factorial(n - 1);       // gọi đệ quy
}

void try_change(int x) {
    x = 100;                           // chỉ sửa BẢN SAO, biến gốc không đổi
    printf("trong ham: x = %d\n", x);
}

int next_id(void) {
    static int id = 0;                 // biến static: giữ giá trị giữa các lần gọi
    return ++id;
}
```

**Kết quả:**

```text
max2(3, 8) = 8
10! = 3628800
trong ham: x = 100
v sau khi goi ham = 5
next_id() lan 3 = 3
```

### 5.2 Truyền tham trị

**C luôn truyền tham trị**: hàm nhận **bản sao** của đối số. Đó là lý do `try_change` không sửa được `v`. Muốn hàm sửa được biến bên ngoài, phải truyền **địa chỉ** (con trỏ) — xem Chương 7.

### 5.3 Phạm vi và vòng đời của biến

| Loại biến | Khai báo ở đâu | Tồn tại bao lâu | Ai thấy được |
|---|---|---|---|
| Cục bộ | Trong hàm/khối `{}` | Đến khi ra khỏi khối | Chỉ khối đó |
| Cục bộ `static` | Trong hàm, có `static` | Suốt chương trình | Chỉ hàm đó |
| Toàn cục | Ngoài mọi hàm | Suốt chương trình | Mọi hàm (hạn chế dùng!) |

### 5.4 Đệ quy hoạt động thế nào?

Mỗi lần gọi hàm, một **khung (frame)** mới được đẩy lên **stack**, chứa tham số và biến cục bộ:

```text
factorial(3)                     stack (phát triển xuống dưới)
  └─ 3 * factorial(2)            ┌────────────────────┐
         └─ 2 * factorial(1)     │ factorial(3): n=3  │
                └─ return 1      │ factorial(2): n=2  │
         └─ return 2 * 1 = 2     │ factorial(1): n=1  │  ← đỉnh stack
  └─ return 3 * 2 = 6            └────────────────────┘
```

Đệ quy quá sâu (hàng trăm nghìn tầng) sẽ làm **tràn stack** (stack overflow). Với bài toán lớn, ưu tiên vòng lặp.

### ✅ Tự kiểm tra

Viết hàm đệ quy `int sum_digits(int n)` trả về tổng các chữ số của `n` (với `n >= 0`).

<details><summary>Đáp án</summary>

```c
int sum_digits(int n) {
    if (n < 10) return n;
    return n % 10 + sum_digits(n / 10);
}
```

</details>

---

## Chương 6. Mảng và chuỗi

### 6.1 Mảng

```c
#include <stdio.h>
#include <string.h>

// Mảng truyền vào hàm sẽ "suy biến" thành con trỏ -> PHẢI truyền kèm kích thước
double average(const int arr[], size_t n) {
    long sum = 0;
    for (size_t i = 0; i < n; i++) sum += arr[i];
    return n > 0 ? (double)sum / n : 0.0;
}

int main(void) {
    int scores[5] = {8, 6, 9, 7, 10};
    size_t n = sizeof(scores) / sizeof(scores[0]);   // chỉ đúng tại nơi khai báo mảng
    printf("n = %zu, trung binh = %.2f\n", n, average(scores, n));

    int grid[2][3] = {
        {1, 2, 3},
        {4, 5, 6},
    };
    printf("grid[1][2] = %d\n", grid[1][2]);

    // Chuỗi
    char name[16] = "Nam";
    char greet[32];
    printf("strlen = %zu, sizeof = %zu\n", strlen(name), sizeof(name));
    snprintf(greet, sizeof(greet), "Xin chao %s!", name);   // ghi an toàn, không tràn
    printf("%s\n", greet);

    if (strcmp(name, "Nam") == 0) printf("Hai chuoi bang nhau\n");
    return 0;
}
```

**Kết quả:**

```text
n = 5, trung binh = 8.00
grid[1][2] = 6
strlen = 3, sizeof = 16
Xin chao Nam!
Hai chuoi bang nhau
```

**Những điều bắt buộc phải nhớ về mảng:**

- Chỉ số bắt đầu từ **0**; mảng `n` phần tử có chỉ số `0` đến `n - 1`.
- C **không kiểm tra** chỉ số. Truy cập `scores[5]` không báo lỗi mà ghi đè lên vùng nhớ khác — đây là nguồn gốc của rất nhiều lỗ hổng bảo mật.
- Khi truyền vào hàm, mảng trở thành con trỏ, nên `sizeof(arr)` bên trong hàm là kích thước **con trỏ** (8 byte), không phải kích thước mảng.

### 6.2 Chuỗi trong C

Chuỗi C là **mảng `char` kết thúc bằng ký tự `'\0'`** (byte 0):

```text
char name[16] = "Nam";

 chỉ số:  0     1     2     3      4    ...   15
        ┌─────┬─────┬─────┬──────┬─────┬─────┬─────┐
        │ 'N' │ 'a' │ 'm' │ '\0' │  0  │ ... │  0  │
        └─────┴─────┴─────┴──────┴─────┴─────┴─────┘
 strlen(name) = 3   (đếm đến trước '\0')
 sizeof(name) = 16  (kích thước cả mảng)
```

| Hàm (`<string.h>`) | Công dụng | Lưu ý an toàn |
|---|---|---|
| `strlen(s)` | Độ dài chuỗi | |
| `strcmp(a, b)` | So sánh (trả về 0 nếu bằng) | **Không** dùng `a == b` — đó là so sánh địa chỉ |
| `strcpy(dst, src)` | Sao chép | Dễ tràn bộ đệm — ưu tiên `snprintf` |
| `strcat(dst, src)` | Nối chuỗi | Dễ tràn — ưu tiên `snprintf` |
| `snprintf(dst, size, fmt, ...)` | Ghi có định dạng | **An toàn**: không bao giờ ghi quá `size` byte |
| `fgets(buf, size, stdin)` | Đọc một dòng | An toàn; **tuyệt đối không dùng `gets`** |

### ✅ Tự kiểm tra

1. Mảng `char s[5]` chứa được chuỗi dài tối đa bao nhiêu ký tự?
2. Vì sao `if (s1 == s2)` không so sánh nội dung hai chuỗi?

<details><summary>Đáp án</summary>

1. 4 ký tự, vì cần 1 byte cho `'\0'`.
2. Tên mảng là địa chỉ phần tử đầu; `==` so sánh hai **địa chỉ**, không phải nội dung. Dùng `strcmp`.

</details>

---

## Chương 7. Con trỏ — trọng tâm của C

> Nếu chỉ được học kỹ một chương trong Phần I, hãy chọn chương này. Con trỏ là chìa khóa để hiểu mảng, chuỗi, bộ nhớ động, cấu trúc dữ liệu — và sau này là smart pointer, reference, iterator trong C++.

### 7.1 Con trỏ là gì?

Mỗi biến nằm ở một **địa chỉ** trong bộ nhớ. **Con trỏ** là một biến chứa **địa chỉ** của biến khác.

| Cú pháp | Ý nghĩa |
|---|---|
| `int *p;` | Khai báo `p` là con trỏ tới `int` |
| `&x` | Lấy **địa chỉ** của `x` |
| `*p` | **Giải tham chiếu**: truy cập giá trị tại địa chỉ `p` trỏ tới |
| `NULL` | Con trỏ "không trỏ vào đâu" |

```text
int x = 10;
int *p = &x;

   Tên     Địa chỉ (ví dụ)   Giá trị
 ┌──────┬─────────────────┬──────────────┐
 │  x   │   0x7ffc1000    │      10      │ ◄──┐
 ├──────┼─────────────────┼──────────────┤    │ p "trỏ tới" x
 │  p   │   0x7ffc1008    │  0x7ffc1000  │ ───┘
 └──────┴─────────────────┴──────────────┘
 *p  == 10        (giá trị tại địa chỉ p giữ)
 &x  == 0x7ffc1000
```

### 7.2 Con trỏ trong thực hành

```c
#include <stdio.h>

// Muốn hàm sửa được biến bên ngoài -> truyền địa chỉ
void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int main(void) {
    int x = 10;
    int *p = &x;
    printf("x = %d, *p = %d\n", x, *p);
    *p = 20;                              // sửa x thông qua p
    printf("x = %d\n", x);

    int a = 1, b = 2;
    swap(&a, &b);
    printf("a = %d, b = %d\n", a, b);

    // Con trỏ và mảng
    int arr[] = {10, 20, 30, 40};
    int *q = arr;                         // tên mảng -> địa chỉ phần tử đầu tiên
    printf("*(q + 2) = %d, q[3] = %d\n", *(q + 2), q[3]);
    printf("(arr + 3) - q = %td\n", (arr + 3) - q);

    // Con trỏ tới con trỏ
    int **pp = &p;
    **pp = 99;                            // pp -> p -> x
    printf("x = %d\n", x);
    return 0;
}
```

**Kết quả:**

```text
x = 10, *p = 10
x = 20
a = 2, b = 1
*(q + 2) = 30, q[3] = 40
(arr + 3) - q = 3
x = 99
```

### 7.3 Số học con trỏ

Với `int *q`, phép `q + 1` **không** cộng thêm 1 byte mà tiến tới **phần tử kế tiếp** (cộng thêm `sizeof(int)` byte).

$$\texttt{q[i]} \equiv \texttt{*(q + i)}$$

Đây là lý do mảng và con trỏ dùng được lẫn cho nhau trong nhiều trường hợp. Trừ hai con trỏ cùng mảng cho ra **số phần tử** giữa chúng (kiểu `ptrdiff_t`, in bằng `%td`).

### 7.4 `const` và con trỏ

Đọc khai báo **từ phải sang trái**:

| Khai báo | Đọc là | Sửa `*p`? | Đổi `p` sang chỗ khác? |
|---|---|:-:|:-:|
| `const int *p` | con trỏ tới int **hằng** | ❌ | ✅ |
| `int *const p` | con trỏ **hằng** tới int | ✅ | ❌ |
| `const int *const p` | con trỏ hằng tới int hằng | ❌ | ❌ |

**Thói quen tốt:** tham số con trỏ mà hàm chỉ đọc thì khai báo `const` (như `const int arr[]` ở Chương 6).

### 7.5 Con trỏ hàm và `void *`

Con trỏ có thể trỏ tới **hàm**. Kết hợp với `void *` (con trỏ "không kiểu"), ta viết được code tổng quát — ví dụ hàm sắp xếp `qsort` của thư viện chuẩn:

```c
#include <stdio.h>
#include <stdlib.h>

// qsort yêu cầu hàm so sánh nhận hai con trỏ void*
int cmp_asc(const void *a, const void *b) {
    int x = *(const int *)a;              // ép về đúng kiểu rồi mới đọc
    int y = *(const int *)b;
    return (x > y) - (x < y);             // trả về -1, 0, 1; tránh tràn số của "x - y"
}

int cmp_desc(const void *a, const void *b) {
    return cmp_asc(b, a);
}

void print_arr(const int *arr, size_t n) {
    for (size_t i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");
}

int main(void) {
    int data[] = {42, 7, 19, 3, 25};
    size_t n = sizeof data / sizeof data[0];

    int (*cmp)(const void *, const void *) = cmp_asc;   // biến con trỏ hàm
    qsort(data, n, sizeof data[0], cmp);
    print_arr(data, n);

    cmp = cmp_desc;                                      // đổi "chiến lược" so sánh
    qsort(data, n, sizeof data[0], cmp);
    print_arr(data, n);
    return 0;
}
```

**Kết quả:**

```text
3 7 19 25 42
42 25 19 7 3
```

Cú pháp `int (*cmp)(const void *, const void *)` đọc là: "`cmp` là con trỏ tới hàm nhận hai `const void *` và trả về `int`". Con trỏ hàm là tiền thân của **callback**, và sau này là **lambda** trong C++.

### 7.6 Các lỗi con trỏ nguy hiểm

| Lỗi | Ví dụ | Hậu quả |
|---|---|---|
| Con trỏ chưa khởi tạo | `int *p; *p = 5;` | Ghi vào địa chỉ ngẫu nhiên |
| Giải tham chiếu NULL | `int *p = NULL; *p = 5;` | Crash (segfault) |
| Trả về địa chỉ biến cục bộ | `int *f(void) { int x; return &x; }` | Con trỏ treo — `x` đã bị hủy |
| Vượt biên mảng | `arr[10]` với mảng 10 phần tử | Hỏng dữ liệu, lỗ hổng bảo mật |

**Quy tắc an toàn:** luôn khởi tạo con trỏ (bằng địa chỉ hợp lệ hoặc `NULL`), và kiểm tra `NULL` trước khi dùng nếu có khả năng.

### ✅ Tự kiểm tra

```c
int arr[] = {1, 2, 3, 4};
int *p = arr + 1;
printf("%d %d %d\n", *p, p[1], *(p - 1));
```

In ra gì?

<details><summary>Đáp án</summary>

`2 3 1`. `p` trỏ tới `arr[1]`, nên `p[1]` là `arr[2]`, `*(p - 1)` là `arr[0]`.

</details>

---

## Chương 8. Bộ nhớ động

### 8.1 Stack và Heap

| | Stack | Heap |
|---|---|---|
| Chứa gì | Biến cục bộ, tham số hàm | Vùng nhớ xin bằng `malloc` |
| Ai quản lý | **Tự động** (khi ra khỏi hàm) | **Lập trình viên** (phải `free`) |
| Kích thước | Nhỏ (thường vài MB) | Lớn (giới hạn bởi RAM) |
| Tốc độ cấp phát | Rất nhanh | Chậm hơn |
| Dùng khi | Dữ liệu nhỏ, sống ngắn | Kích thước chỉ biết lúc chạy, hoặc cần sống lâu hơn hàm tạo ra nó |

### 8.2 Bốn hàm cần biết (`<stdlib.h>`)

```c
int *a = malloc(n * sizeof *a);          // cấp phát n phần tử, KHÔNG khởi tạo (chứa rác)
int *b = calloc(n, sizeof *b);           // cấp phát n phần tử, khởi tạo toàn 0
int *c = realloc(a, 2 * n * sizeof *c);  // đổi kích thước (có thể dời sang chỗ mới)
free(b);                                  // trả lại bộ nhớ
```

- Viết `sizeof *a` thay vì `sizeof(int)`: nếu sau này đổi kiểu của `a`, code vẫn đúng.
- Cả ba hàm cấp phát đều trả về `NULL` khi thất bại — **phải kiểm tra**.
- Với `realloc`, gán kết quả vào **biến tạm** trước: nếu thất bại mà gán thẳng `a = realloc(a, ...)`, bạn sẽ mất con trỏ tới vùng nhớ cũ (rò rỉ bộ nhớ).

### 8.3 Ví dụ: mảng động tự mở rộng

Đây là cách `std::vector` của C++ hoạt động bên dưới:

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int    *data;
    size_t  size;       // số phần tử đang có
    size_t  capacity;   // số phần tử chứa được trước khi phải cấp phát lại
} IntVec;

void vec_init(IntVec *v) {
    v->data = NULL;
    v->size = 0;
    v->capacity = 0;
}

int vec_push(IntVec *v, int value) {
    if (v->size == v->capacity) {
        size_t new_cap = v->capacity ? v->capacity * 2 : 4;   // tăng gấp đôi
        int *tmp = realloc(v->data, new_cap * sizeof *tmp);
        if (tmp == NULL) return 0;                            // dữ liệu cũ vẫn còn nguyên
        v->data = tmp;
        v->capacity = new_cap;
    }
    v->data[v->size++] = value;
    return 1;
}

void vec_free(IntVec *v) {
    free(v->data);
    v->data = NULL;              // tránh con trỏ treo
    v->size = v->capacity = 0;
}

int main(void) {
    IntVec v;
    vec_init(&v);
    for (int i = 1; i <= 10; i++) {
        if (!vec_push(&v, i * i)) {
            fprintf(stderr, "Het bo nho\n");
            vec_free(&v);
            return 1;
        }
        if (v.size == v.capacity)
            printf("Day: size = %zu, capacity = %zu\n", v.size, v.capacity);
    }
    printf("Cuoi cung: size = %zu, capacity = %zu\n", v.size, v.capacity);
    for (size_t i = 0; i < v.size; i++) printf("%d ", v.data[i]);
    printf("\n");
    vec_free(&v);
    return 0;
}
```

**Kết quả:**

```text
Day: size = 4, capacity = 4
Day: size = 8, capacity = 8
Cuoi cung: size = 10, capacity = 16
1 4 9 16 25 36 49 64 81 100
```

Vì sao tăng **gấp đôi** mà không tăng thêm 1? Nếu mỗi lần chỉ tăng 1, mỗi lần thêm phần tử đều phải sao chép toàn bộ mảng — rất chậm. Tăng gấp đôi khiến chi phí trung bình mỗi lần thêm chỉ là $O(1)$.

### 8.4 Bốn lỗi bộ nhớ kinh điển

```c
// 1. Rò rỉ bộ nhớ (memory leak): quên free
int *p = malloc(100);
p = NULL;                  // vùng 100 byte không còn ai trỏ tới -> mất vĩnh viễn

// 2. Dùng sau khi giải phóng (use-after-free)
free(q);
q[0] = 5;                  // UB

// 3. Giải phóng hai lần (double free)
free(r);
free(r);                   // UB, thường crash

// 4. Tràn vùng nhớ heap
int *s = malloc(3 * sizeof *s);
s[3] = 1;                  // chỉ có s[0]..s[2]
```

**Cách phát hiện:** biên dịch với `-fsanitize=address` (AddressSanitizer), hoặc chạy bằng `valgrind ./main` trên Linux. Cả hai sẽ chỉ đúng dòng gây lỗi.

**Quy tắc sở hữu:** với mỗi `malloc`, hãy xác định rõ **ai chịu trách nhiệm `free`** và **khi nào**. Trong C++, quy tắc này được tự động hóa bằng RAII (Chương 14).

### ✅ Tự kiểm tra

Đoạn code sau có lỗi gì?

```c
char *make_greeting(void) {
    char buf[32];
    snprintf(buf, sizeof buf, "Hello");
    return buf;
}
```

<details><summary>Đáp án</summary>

Trả về địa chỉ của mảng cục bộ `buf` nằm trên stack; khi hàm kết thúc, `buf` bị hủy → con trỏ treo. Cách sửa: cấp phát bằng `malloc` (người gọi phải `free`), hoặc để người gọi truyền bộ đệm vào hàm.

</details>

---

## Chương 9. `struct`, `union`, `enum` và danh sách liên kết

### 9.1 Gom dữ liệu với `struct`, `enum`, `union`

```c
#include <stdio.h>

typedef enum { SHAPE_CIRCLE, SHAPE_RECT } ShapeKind;   // enum: đặt tên cho các hằng nguyên

typedef struct {
    ShapeKind kind;
    union {                          // union: các thành viên DÙNG CHUNG vùng nhớ
        double radius;               //   dùng khi kind == SHAPE_CIRCLE
        struct { double w, h; } rect; //  dùng khi kind == SHAPE_RECT
    };
} Shape;

double area(const Shape *s) {        // truyền con trỏ để không phải sao chép cả struct
    switch (s->kind) {               // s->kind  tương đương  (*s).kind
    case SHAPE_CIRCLE: return 3.14159 * s->radius * s->radius;
    case SHAPE_RECT:   return s->rect.w * s->rect.h;
    }
    return 0.0;
}

typedef struct { char c; int i; char d; } Padded;
typedef struct { int i; char c; char d; } Reordered;

int main(void) {
    Shape shapes[] = {
        { .kind = SHAPE_CIRCLE, .radius = 2.0 },          // khởi tạo theo tên trường
        { .kind = SHAPE_RECT,   .rect = { 3.0, 4.0 } },
    };
    for (size_t i = 0; i < 2; i++)
        printf("Hinh %zu: dien tich = %.2f\n", i, area(&shapes[i]));

    printf("sizeof(Padded) = %zu, sizeof(Reordered) = %zu\n",
           sizeof(Padded), sizeof(Reordered));
    return 0;
}
```

**Kết quả:**

```text
Hinh 0: dien tich = 12.57
Hinh 1: dien tich = 12.00
sizeof(Padded) = 12, sizeof(Reordered) = 8
```

**Giải thích:**

- `typedef` đặt tên ngắn gọn cho kiểu, để viết `Shape` thay vì `struct Shape`.
- **Truy cập thành viên:** dùng `.` với biến struct, dùng `->` với con trỏ tới struct.
- **`union` có "nhãn" (tagged union):** trường `kind` cho biết thành viên nào của union đang hợp lệ. Đọc sai thành viên là lỗi logic. (C++ có `std::variant` làm việc này an toàn hơn — Chương 22.)
- **Padding (căn lề):** CPU thích dữ liệu nằm ở địa chỉ chia hết cho kích thước của nó, nên trình biên dịch chèn byte trống:

```text
Padded:     [c][ pad pad pad ][ i i i i ][d][ pad pad pad ]   = 12 byte
Reordered:  [ i i i i ][c][d][ pad pad ]                      =  8 byte
```

Sắp xếp các trường **từ lớn đến nhỏ** giúp tiết kiệm bộ nhớ — điều đáng quan tâm khi có hàng triệu đối tượng trong game.

### 9.2 Danh sách liên kết đơn

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int value;
    struct Node *next;        // trỏ tới nút kế tiếp
} Node;

// Thêm vào đầu danh sách, trả về đầu mới
Node *push_front(Node *head, int value) {
    Node *n = malloc(sizeof *n);
    if (n == NULL) return head;
    n->value = value;
    n->next = head;
    return n;
}

// Xóa nút đầu tiên có giá trị value.
// Dùng con trỏ tới con trỏ để xử lý chung cả trường hợp xóa nút đầu.
void remove_value(Node **head, int value) {
    for (Node **pp = head; *pp != NULL; pp = &(*pp)->next) {
        if ((*pp)->value == value) {
            Node *victim = *pp;
            *pp = victim->next;   // "nối tắt" qua nút bị xóa
            free(victim);
            return;
        }
    }
}

void print_list(const Node *head) {
    for (const Node *p = head; p != NULL; p = p->next) printf("%d -> ", p->value);
    printf("NULL\n");
}

void free_list(Node *head) {
    while (head != NULL) {
        Node *next = head->next;  // lưu lại TRƯỚC khi free
        free(head);
        head = next;
    }
}

int main(void) {
    Node *list = NULL;
    for (int i = 1; i <= 4; i++) list = push_front(list, i * 10);
    print_list(list);

    remove_value(&list, 30);
    remove_value(&list, 40);      // xóa nút đầu: list phải được cập nhật -> truyền &list
    print_list(list);

    free_list(list);
    return 0;
}
```

**Kết quả:**

```text
40 -> 30 -> 20 -> 10 -> NULL
20 -> 10 -> NULL
```

**Mảng hay danh sách liên kết?**

| Thao tác | Mảng động | Danh sách liên kết |
|---|---|---|
| Truy cập phần tử thứ $i$ | $O(1)$ | $O(n)$ |
| Thêm/xóa ở đầu | $O(n)$ | $O(1)$ |
| Thân thiện với cache CPU | ✅ Rất tốt | ❌ Kém (nút nằm rải rác) |

Trên máy tính hiện đại, **mảng động thường nhanh hơn** trong đa số trường hợp nhờ cache. Hãy mặc định dùng mảng (và `std::vector` trong C++), chỉ chọn danh sách liên kết khi có lý do rõ ràng.

---

## Chương 10. Làm việc với file

```c
#include <stdio.h>

int main(void) {
    const char *path = "scores.txt";

    // Ghi file
    FILE *f = fopen(path, "w");                 // "w": ghi mới (xóa nội dung cũ)
    if (f == NULL) {
        perror("fopen");                        // in lý do lỗi của hệ điều hành
        return 1;
    }
    fprintf(f, "An 8\nBinh 6\nChi 9\n");
    fclose(f);

    // Đọc file
    f = fopen(path, "r");                       // "r": chỉ đọc
    if (f == NULL) {
        perror("fopen");
        return 1;
    }
    char name[32];
    int score, total = 0, count = 0;
    while (fscanf(f, "%31s %d", name, &score) == 2) {   // %31s: giới hạn độ dài, chống tràn
        printf("%-6s %d\n", name, score);               // %-6s: căn trái, rộng 6 ký tự
        total += score;
        count++;
    }
    fclose(f);

    if (count > 0) printf("Trung binh: %.2f\n", (double)total / count);
    remove(path);                               // xóa file tạm
    return 0;
}
```

**Kết quả:**

```text
An     8
Binh   6
Chi    9
Trung binh: 7.67
```

| Chế độ | Ý nghĩa |
|---|---|
| `"r"` | Đọc (file phải tồn tại) |
| `"w"` | Ghi mới (tạo file hoặc xóa nội dung cũ) |
| `"a"` | Ghi nối vào cuối |
| `"rb"`, `"wb"` | Như trên nhưng ở chế độ **nhị phân** (dùng với `fread`/`fwrite`) |

**Đọc từng dòng** (cách an toàn và linh hoạt nhất):

```c
char line[256];
while (fgets(line, sizeof line, f) != NULL) {
    // xử lý line (lưu ý: line có thể chứa '\n' ở cuối)
}
```

**Ghi/đọc nhị phân** (lưu nguyên struct, nhanh nhưng phụ thuộc kiến trúc máy):

```c
fwrite(&player, sizeof player, 1, f);   // ghi 1 struct
fread(&player, sizeof player, 1, f);    // đọc lại
```

---

## Chương 11. Tổ chức chương trình nhiều file và tiền xử lý

### 11.1 Tách header và source

Khi chương trình lớn dần, tách thành nhiều file: **header (`.h`)** chứa **khai báo**, **source (`.c`)** chứa **định nghĩa**.

```c
// file: intvec.h
#ifndef INTVEC_H          // include guard: tránh bị include hai lần
#define INTVEC_H

#include <stddef.h>

typedef struct {
    int *data;
    size_t size, capacity;
} IntVec;

void vec_init(IntVec *v);
int  vec_push(IntVec *v, int value);
void vec_free(IntVec *v);

#endif
```

```c
// file: intvec.c
#include "intvec.h"       // dấu "" cho header của mình, <> cho header hệ thống
#include <stdlib.h>

void vec_init(IntVec *v) { v->data = NULL; v->size = v->capacity = 0; }
/* ... vec_push, vec_free như Chương 8 ... */
```

```c
// file: main.c
#include "intvec.h"
#include <stdio.h>

int main(void) {
    IntVec v;
    vec_init(&v);
    vec_push(&v, 42);
    printf("%d\n", v.data[0]);
    vec_free(&v);
    return 0;
}
```

Biên dịch cả hai file source rồi liên kết:

```bash
gcc -std=c17 -Wall -Wextra main.c intvec.c -o app
```

Với dự án lớn hơn, dùng hệ thống build như **Make** hoặc **CMake** để không phải gõ lại lệnh và chỉ biên dịch lại những file thay đổi.

### 11.2 `static` và `extern` ở phạm vi file

| Từ khóa | Đặt trước biến/hàm toàn cục | Ý nghĩa |
|---|---|---|
| `static` | `static int helper(void)` | **Chỉ dùng trong file này** — nên dùng cho hàm phụ trợ |
| `extern` | `extern int g_count;` (trong `.h`) | Khai báo biến được **định nghĩa ở file khác** |

### 11.3 Macro và cái bẫy của nó

Macro chỉ là **thay thế văn bản**, không hiểu gì về toán học:

```c
#include <stdio.h>

#define SQUARE_BAD(x) x * x
#define SQUARE(x) ((x) * (x))

int main(void) {
    printf("SQUARE_BAD(1 + 2) = %d\n", SQUARE_BAD(1 + 2));   // thành 1 + 2 * 1 + 2
    printf("SQUARE(1 + 2)     = %d\n", SQUARE(1 + 2));       // thành ((1 + 2) * (1 + 2))
    return 0;
}
```

**Kết quả:**

```text
SQUARE_BAD(1 + 2) = 5
SQUARE(1 + 2)     = 9
```

**Quy tắc:** luôn bọc ngoặc quanh **từng tham số** và **toàn bộ macro**. Nhưng ngay cả khi bọc ngoặc, `SQUARE(i++)` vẫn tăng `i` hai lần. Vì vậy, khi có thể, hãy dùng **hàm** (hoặc `inline`, `constexpr` trong C++) thay vì macro.

### 11.4 Hành vi không xác định (Undefined Behavior) — danh sách cần nhớ

UB nghĩa là chuẩn C **không quy định** điều gì xảy ra: chương trình có thể chạy đúng, in rác, crash, hoặc chạy đúng trên máy bạn nhưng sai trên máy khác. Những UB hay gặp nhất:

1. Truy cập ngoài biên mảng.
2. Giải tham chiếu con trỏ `NULL` hoặc con trỏ treo.
3. Dùng biến chưa khởi tạo.
4. Tràn số nguyên **có dấu**.
5. Sửa một biến hai lần trong cùng biểu thức (`i = i++`).
6. `free` hai lần, hoặc `free` con trỏ không lấy từ `malloc`.
7. Sai chuỗi định dạng trong `printf`/`scanf`.
8. Sửa chuỗi hằng: `char *s = "abc"; s[0] = 'x';`

Công cụ phát hiện: `-Wall -Wextra` lúc biên dịch, `-fsanitize=address,undefined` lúc chạy.

---

## Bài tập Phần I

**Bài I.1.** Viết hàm `void reverse(char *s)` đảo ngược chuỗi **tại chỗ** chỉ bằng con trỏ (không dùng chỉ số `s[i]`).

**Bài I.2.** Viết chương trình đếm số lần xuất hiện của từng chữ cái (không phân biệt hoa thường) trong chuỗi `"Hello World"`, in các chữ cái có xuất hiện.

**Bài I.3.** Viết hàm `void min_max(const int *arr, size_t n, int *out_min, int *out_max)` trả về **cả hai** giá trị nhỏ nhất và lớn nhất qua tham số con trỏ.

**Bài I.4.** Cấp phát động một ma trận `rows × cols` bằng **một lần `malloc` duy nhất**, gán `m[i][j] = i * j`, in ra, rồi giải phóng.

**Bài I.5.** Cài đặt **ngăn xếp (stack)** bằng danh sách liên kết với các hàm `push`, `pop`, `is_empty`. Dùng nó để đảo ngược dãy `1 2 3 4 5`.

<details>
<summary><b>Lời giải Phần I</b> (bấm để mở)</summary>

**Bài I.1**

```c
#include <stdio.h>
#include <string.h>

void reverse(char *s) {
    char *left = s;
    char *right = s + strlen(s);        // trỏ vào '\0'
    if (right == s) return;             // chuỗi rỗng
    right--;                            // trỏ vào ký tự cuối
    while (left < right) {
        char tmp = *left;
        *left++ = *right;
        *right-- = tmp;
    }
}

int main(void) {
    char s[] = "Lap trinh C";           // mảng (sửa được), KHÔNG phải char *s = "..."
    reverse(s);
    printf("%s\n", s);
    return 0;
}
```

```text
C hnirt paL
```

**Bài I.2**

```c
#include <ctype.h>
#include <stdio.h>

int main(void) {
    const char *text = "Hello World";
    int count[26] = {0};
    for (const char *p = text; *p != '\0'; p++) {
        unsigned char ch = (unsigned char)*p;     // tránh UB khi truyền char âm vào ctype
        if (isalpha(ch)) count[tolower(ch) - 'a']++;
    }
    for (int i = 0; i < 26; i++)
        if (count[i] > 0) printf("%c: %d\n", 'a' + i, count[i]);
    return 0;
}
```

```text
d: 1
e: 1
h: 1
l: 3
o: 2
r: 1
w: 1
```

**Bài I.3**

```c
#include <stdio.h>

void min_max(const int *arr, size_t n, int *out_min, int *out_max) {
    if (n == 0) return;
    *out_min = *out_max = arr[0];
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < *out_min) *out_min = arr[i];
        if (arr[i] > *out_max) *out_max = arr[i];
    }
}

int main(void) {
    int a[] = {4, -2, 17, 8, 0};
    int lo = 0, hi = 0;
    min_max(a, sizeof a / sizeof a[0], &lo, &hi);
    printf("min = %d, max = %d\n", lo, hi);
    return 0;
}
```

```text
min = -2, max = 17
```

**Bài I.4**

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    size_t rows = 3, cols = 4;
    int *m = malloc(rows * cols * sizeof *m);   // một khối liên tục -> thân thiện cache
    if (m == NULL) return 1;

    for (size_t i = 0; i < rows; i++)
        for (size_t j = 0; j < cols; j++)
            m[i * cols + j] = (int)(i * j);     // "phẳng hóa" chỉ số 2 chiều

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) printf("%3d", m[i * cols + j]);
        printf("\n");
    }
    free(m);                                    // chỉ một lần free
    return 0;
}
```

```text
  0  0  0  0
  0  1  2  3
  0  2  4  6
```

**Bài I.5**

```c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct StackNode {
    int value;
    struct StackNode *next;
} StackNode;

bool push(StackNode **top, int value) {
    StackNode *n = malloc(sizeof *n);
    if (n == NULL) return false;
    n->value = value;
    n->next = *top;
    *top = n;
    return true;
}

bool is_empty(const StackNode *top) { return top == NULL; }

int pop(StackNode **top) {              // giả định stack không rỗng
    StackNode *n = *top;
    int value = n->value;
    *top = n->next;
    free(n);
    return value;
}

int main(void) {
    StackNode *st = NULL;
    for (int i = 1; i <= 5; i++) push(&st, i);
    while (!is_empty(st)) printf("%d ", pop(&st));   // pop hết -> không rò rỉ
    printf("\n");
    return 0;
}
```

```text
5 4 3 2 1
```

</details>

---

# PHẦN II — C++ NÂNG CAO

> Từ đây trở đi dùng chuẩn **C++20** (`g++ -std=c++20`). Mục tiêu của phần này không phải liệt kê mọi tính năng, mà là giúp bạn viết code C++ **an toàn, không rò rỉ bộ nhớ, dễ đọc** — đúng cách C++ hiện đại được dùng trong engine game, cơ sở dữ liệu hay thư viện AI.

## Chương 12. Từ C sang C++

### 12.1 Những thay đổi đầu tiên

```cpp
#include <iostream>
#include <string>
#include <vector>

namespace geo {                                   // namespace: tránh trùng tên
double area(double w, double h) { return w * h; }
double area(double r) { return 3.14159 * r * r; } // nạp chồng hàm (overloading)
}

void add_bonus(int &score, int bonus = 5) {       // tham chiếu + tham số mặc định
    score += bonus;
}

int main() {
    std::string name = "Nam";                     // chuỗi tự quản lý bộ nhớ
    name += " Nguyen";
    std::cout << "Ten: " << name << " (" << name.size() << " ky tu)\n";

    int score = 80;
    add_bonus(score);                             // không cần &score
    add_bonus(score, 10);
    std::cout << "Diem: " << score << '\n';

    std::cout << geo::area(3.0, 4.0) << ' ' << geo::area(1.0) << '\n';

    std::vector<int> nums = {3, 1, 4, 1, 5};      // mảng động tự quản lý bộ nhớ
    nums.push_back(9);
    int total = 0;
    for (int x : nums) total += x;                // range-based for
    std::cout << "Tong: " << total << '\n';

    auto *p = new int(42);                        // auto: tự suy luận kiểu
    std::cout << "*p = " << *p << '\n';
    delete p;                                     // (Chương 16 sẽ bỏ hẳn new/delete thủ công)
    p = nullptr;                                  // nullptr thay cho NULL
}
```

**Kết quả:**

```text
Ten: Nam Nguyen (10 ky tu)
Diem: 95
12 3.14159
Tong: 23
*p = 42
```

| C | C++ | Lợi ích |
|---|---|---|
| `printf` | `std::cout <<` | An toàn kiểu, không cần chuỗi định dạng |
| `char[]` + `strcpy` | `std::string` | Tự quản lý bộ nhớ, không tràn |
| `malloc` + `realloc` | `std::vector` | Tự mở rộng, tự giải phóng |
| `NULL` | `nullptr` | Có kiểu riêng, không nhầm với số 0 |
| Con trỏ làm tham số | **Tham chiếu** `int &` | Cú pháp gọn, không bao giờ "rỗng" |
| `#define` hằng | `const`, `constexpr` | Có kiểu, có phạm vi |

> `main()` trong C++ được phép không có `return 0;` — trình biên dịch tự hiểu là trả về 0.

### 12.2 Tham chiếu (reference)

Tham chiếu là **một tên khác** của một biến đã có:

```cpp
int x = 10;
int &r = x;     // r là bí danh của x
r = 20;         // x cũng thành 20
```

| | Con trỏ `int *p` | Tham chiếu `int &r` |
|---|---|---|
| Có thể rỗng? | Có (`nullptr`) | **Không** |
| Đổi sang biến khác? | Được | **Không** — gắn cố định khi khởi tạo |
| Phải khởi tạo ngay? | Không | **Có** |
| Cú pháp dùng | `*p`, `p->x` | Như biến thường |

**Quy tắc truyền tham số trong C++** (rất quan trọng, dùng suốt Phần II):

| Tình huống | Cách truyền | Ví dụ |
|---|---|---|
| Kiểu nhỏ (`int`, `double`, con trỏ) | Theo giá trị | `void f(int x)` |
| Kiểu lớn, **chỉ đọc** | Tham chiếu hằng | `void f(const std::string &s)` |
| Cần **sửa** đối số | Tham chiếu | `void f(std::vector<int> &v)` |
| Có thể "không có" đối số | Con trỏ | `void f(Node *n)` |

### 12.3 `auto`, `const`, `constexpr`

```cpp
auto a = 5;            // int
auto b = 2.5;          // double
auto s = std::string("hi");
const auto &ref = s;   // tham chiếu hằng tới s — auto bỏ & và const nên phải viết rõ

const int max_hp = 100;             // hằng lúc chạy
constexpr int tile_size = 32;       // hằng lúc BIÊN DỊCH (dùng được làm kích thước mảng, v.v.)
```

> ⚠️ `auto x = some_vector[0];` tạo **bản sao**. Muốn sửa phần tử gốc, viết `auto &x = some_vector[0];`.

---

## Chương 13. Lớp và đối tượng

### 13.1 Định nghĩa lớp

**Lớp (class)** gom **dữ liệu** và **hành vi** lại, đồng thời **che giấu** chi tiết bên trong (đóng gói).

```cpp
#include <iostream>
#include <string>
#include <utility>

class BankAccount {
public:                                                    // ai cũng dùng được
    BankAccount(std::string owner, double balance = 0.0)
        : owner_(std::move(owner)), balance_(balance) {    // danh sách khởi tạo thành viên
        ++count_;
    }
    ~BankAccount() { --count_; }                           // hàm hủy: chạy khi đối tượng bị hủy

    bool deposit(double amount) {
        if (amount <= 0) return false;
        balance_ += amount;
        return true;
    }
    bool withdraw(double amount) {
        if (amount <= 0 || amount > balance_) return false; // bảo vệ tính hợp lệ của dữ liệu
        balance_ -= amount;
        return true;
    }

    double balance() const { return balance_; }            // const: hàm KHÔNG sửa đối tượng
    const std::string &owner() const { return owner_; }
    static int count() { return count_; }                  // hàm static: thuộc về lớp

private:                                                   // chỉ bên trong lớp dùng được
    std::string owner_;
    double balance_;
    static inline int count_ = 0;                          // C++17: khởi tạo ngay trong lớp
};

int main() {
    BankAccount a("An", 100);
    a.deposit(50);
    if (!a.withdraw(500)) std::cout << "Khong du tien\n";
    std::cout << a.owner() << ": " << a.balance() << '\n';
    // a.balance_ = 1e9;   // LỖI biên dịch: balance_ là private

    {
        BankAccount b("Binh");
        std::cout << "So tai khoan: " << BankAccount::count() << '\n';
    }   // b bị hủy ở đây -> hàm hủy chạy
    std::cout << "So tai khoan: " << BankAccount::count() << '\n';
}
```

**Kết quả:**

```text
Khong du tien
An: 150
So tai khoan: 2
So tai khoan: 1
```

**Những điểm cần nắm:**

- **`public` / `private` / `protected`**: kiểm soát ai được truy cập. Mặc định của `class` là `private`, của `struct` là `public` — ngoài ra hai từ khóa này giống hệt nhau. Quy ước: dùng `struct` cho dữ liệu đơn giản, `class` khi có ràng buộc cần bảo vệ.
- **Danh sách khởi tạo** (`: owner_(...), balance_(...)`) khởi tạo thành viên **trực tiếp**, hiệu quả hơn gán trong thân hàm, và là cách **duy nhất** để khởi tạo thành viên `const` hoặc tham chiếu.
- **Hàm `const`**: có thể gọi trên đối tượng `const`. Hãy đánh dấu `const` cho mọi hàm không sửa đối tượng.
- **Hàm hủy** chạy **tự động** khi đối tượng ra khỏi phạm vi. Đây là nền móng của RAII (Chương 14).
- Hậu tố `_` cho thành viên private chỉ là quy ước đặt tên, giúp phân biệt với tham số.

> 🐛 Lớp `BankAccount` này có một lỗi tiềm ẩn: nếu viết `BankAccount c = a;` (sao chép), bộ đếm **không tăng** nhưng khi `c` bị hủy thì bộ đếm lại **giảm**. Chương 14 sẽ giải thích và sửa lỗi này.

### 13.2 Nạp chồng toán tử

Cho phép các kiểu tự định nghĩa dùng toán tử tự nhiên như kiểu có sẵn — cực kỳ phổ biến với véc tơ trong game:

```cpp
#include <cmath>
#include <iostream>

struct Vec2 {
    double x = 0, y = 0;

    Vec2 operator+(const Vec2 &o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2 &o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(double k) const { return {x * k, y * k}; }
    Vec2 &operator+=(const Vec2 &o) {       // toán tử gán trả về *this để viết nối được
        x += o.x;
        y += o.y;
        return *this;
    }
    bool operator==(const Vec2 &) const = default;   // C++20: so sánh từng thành viên
    double length() const { return std::sqrt(x * x + y * y); }
};

// Hàm tự do: cho phép viết  2.0 * v  (số đứng trước)
Vec2 operator*(double k, const Vec2 &v) { return v * k; }

// Cho phép  std::cout << v
std::ostream &operator<<(std::ostream &os, const Vec2 &v) {
    return os << '(' << v.x << ", " << v.y << ')';
}

int main() {
    Vec2 pos{0, 0};
    Vec2 vel{3, 4};
    double dt = 0.5;

    pos += vel * dt;        // di chuyển nửa giây
    pos += dt * vel;        // thêm nửa giây nữa
    std::cout << "pos = " << pos << ", |vel| = " << vel.length() << '\n';
    std::cout << std::boolalpha << (pos == Vec2{3, 4}) << '\n';
}
```

**Kết quả:**

```text
pos = (3, 4), |vel| = 5
true
```

**Nguyên tắc:** chỉ nạp chồng khi ý nghĩa **hiển nhiên** (cộng hai véc tơ). Đừng dùng `+` để làm việc không giống phép cộng.

### ✅ Tự kiểm tra

Vì sao đoạn sau lỗi biên dịch?

```cpp
void show(const BankAccount &acc) {
    acc.deposit(10);
}
```

<details><summary>Đáp án</summary>

`acc` là tham chiếu **hằng**, chỉ gọi được các hàm `const`. `deposit` sửa đối tượng nên không phải hàm `const`.

</details>

---

## Chương 14. RAII và Quy tắc 0/3/5

### 14.1 RAII — ý tưởng quan trọng nhất của C++

**RAII** (Resource Acquisition Is Initialization): **gắn vòng đời của tài nguyên với vòng đời của đối tượng**.

- **Hàm tạo** lấy tài nguyên (bộ nhớ, file, khóa mutex, kết nối mạng...).
- **Hàm hủy** trả tài nguyên.
- Vì hàm hủy **luôn** chạy khi đối tượng ra khỏi phạm vi — kể cả khi có `return` sớm hay ngoại lệ — tài nguyên **không bao giờ bị rò rỉ**.

```cpp
void c_style() {
    FILE *f = fopen("a.txt", "r");
    if (!f) return;
    if (loi_gi_do()) return;       // QUÊN fclose -> rò rỉ
    fclose(f);
}

void cpp_style() {
    std::ifstream f("a.txt");      // hàm tạo mở file
    if (!f) return;
    if (loi_gi_do()) return;       // hàm hủy tự đóng file
}                                  // hàm hủy tự đóng file
```

`std::string`, `std::vector`, `std::ifstream`, `std::unique_ptr`, `std::lock_guard`... đều là lớp RAII.

### 14.2 Vấn đề sao chép nông

Khi lớp **tự quản lý** tài nguyên bằng con trỏ thô, phép sao chép mặc định chỉ chép **giá trị con trỏ** (sao chép nông). Hai đối tượng cùng trỏ vào một vùng nhớ, và khi cả hai bị hủy → **giải phóng hai lần**:

```text
Buffer a(3);          a.data_ ──┐
Buffer b = a;         b.data_ ──┴──► [ 0 | 0 | 0 ]   (cùng một vùng nhớ!)
                      a bị hủy: delete[]  ✓
                      b bị hủy: delete[]  ✗  double free!
```

### 14.3 Quy tắc 3 và Quy tắc 5

> **Quy tắc 3:** Nếu lớp cần tự viết **một** trong ba hàm — **hàm hủy**, **hàm tạo sao chép**, **toán tử gán sao chép** — thì gần như chắc chắn cần viết **cả ba**.
>
> **Quy tắc 5:** Từ C++11, thêm **hàm tạo di chuyển** và **toán tử gán di chuyển** để hiệu quả hơn.

```cpp
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <utility>

class Buffer {
public:
    explicit Buffer(std::size_t n) : size_(n), data_(new int[n]{}) {
        std::cout << "  [ctor " << size_ << "]\n";
    }

    ~Buffer() { delete[] data_; }                                    // (1) hàm hủy

    Buffer(const Buffer &other)                                      // (2) sao chép SÂU
        : size_(other.size_), data_(new int[other.size_]) {
        std::copy(other.data_, other.data_ + size_, data_);
        std::cout << "  [copy ctor]\n";
    }

    Buffer(Buffer &&other) noexcept                                  // (3) di chuyển: "lấy" tài nguyên
        : size_(std::exchange(other.size_, 0)),
          data_(std::exchange(other.data_, nullptr)) {
        std::cout << "  [move ctor]\n";
    }

    // (4)+(5) Gán sao chép và gán di chuyển gộp làm một (thành ngữ copy-and-swap):
    // tham số nhận THEO GIÁ TRỊ -> được copy hoặc move sẵn từ phía người gọi
    Buffer &operator=(Buffer other) noexcept {
        swap(other);
        std::cout << "  [assign]\n";
        return *this;
    }                                     // other (giữ dữ liệu cũ) bị hủy tại đây

    void swap(Buffer &other) noexcept {
        std::swap(size_, other.size_);
        std::swap(data_, other.data_);
    }

    int &operator[](std::size_t i) { return data_[i]; }
    std::size_t size() const { return size_; }

private:
    std::size_t size_;
    int *data_;
};

int main() {
    std::cout << "Tao a:\n";
    Buffer a(3);
    a[0] = 7;

    std::cout << "Buffer b = a:\n";
    Buffer b = a;                       // sao chép
    b[0] = 99;
    std::cout << "a[0] = " << a[0] << ", b[0] = " << b[0] << '\n';

    std::cout << "Buffer c = std::move(a):\n";
    Buffer c = std::move(a);            // di chuyển
    std::cout << "a.size() = " << a.size() << ", c.size() = " << c.size() << '\n';

    std::cout << "b = c:\n";
    b = c;                              // copy vào tham số, rồi swap

    std::cout << "b = Buffer(5):\n";
    b = Buffer(5);                      // đối tượng tạm: không cần copy
    std::cout << "b.size() = " << b.size() << '\n';
}
```

**Kết quả:**

```text
Tao a:
  [ctor 3]
Buffer b = a:
  [copy ctor]
a[0] = 7, b[0] = 99
Buffer c = std::move(a):
  [move ctor]
a.size() = 0, c.size() = 3
b = c:
  [copy ctor]
  [assign]
b = Buffer(5):
  [ctor 5]
  [assign]
b.size() = 5
```

**Đọc kết quả:**

- `b[0] = 99` không làm đổi `a[0]` → sao chép sâu hoạt động đúng.
- Sau khi bị di chuyển, `a` rỗng (`size = 0`) nhưng vẫn **hợp lệ** để hủy.
- `b = Buffer(5)` không gọi copy hay move: từ C++17, đối tượng tạm được dựng **thẳng** vào tham số.

### 14.4 Quy tắc 0 — điều bạn nên làm trong thực tế

> **Quy tắc 0:** Đừng tự quản lý tài nguyên nếu không bắt buộc. Hãy dùng thành viên là các lớp RAII có sẵn, và **không viết hàm nào trong 5 hàm trên** — trình biên dịch sẽ tự sinh ra phiên bản đúng.

```cpp
class Buffer {
public:
    explicit Buffer(std::size_t n) : data_(n) {}
    int &operator[](std::size_t i) { return data_[i]; }
    std::size_t size() const { return data_.size(); }
private:
    std::vector<int> data_;     // vector tự lo copy, move, giải phóng
};
```

Tương đương hoàn toàn với phiên bản 60 dòng ở trên. Chỉ viết Quy tắc 5 khi bạn đang **xây dựng** một lớp RAII mới (như `std::vector` tự làm).

**Cấm sao chép** khi việc sao chép không có ý nghĩa (tài khoản ngân hàng, kết nối mạng, luồng...):

```cpp
class BankAccount {
public:
    BankAccount(const BankAccount &) = delete;             // sửa lỗi ở Chương 13
    BankAccount &operator=(const BankAccount &) = delete;
    // ...
};
```

---

## Chương 15. Ngữ nghĩa di chuyển (Move semantics)

### 15.1 lvalue và rvalue

- **lvalue**: biểu thức có **tên / địa chỉ cố định**, tồn tại lâu dài — ví dụ biến `x`.
- **rvalue**: giá trị **tạm thời**, sắp biến mất — ví dụ `x + 1`, `std::string("hi")`, giá trị trả về của hàm.

**Ý tưởng của move:** nếu một đối tượng sắp bị hủy, thay vì **sao chép** dữ liệu của nó (tốn kém), hãy **lấy luôn** tài nguyên của nó (rẻ — chỉ chép vài con trỏ).

**Tham chiếu rvalue** `T&&` chỉ gắn được với rvalue, nhờ đó ta viết hàm riêng cho trường hợp "được phép lấy":

```cpp
#include <iostream>
#include <string>
#include <utility>

void process(const std::string &s) { std::cout << "lvalue: " << s << '\n'; }
void process(std::string &&s)      { std::cout << "rvalue: " << s << '\n'; }

int main() {
    std::string name = "Nam";
    process(name);                  // biến có tên -> lvalue
    process(std::string("Tam"));    // đối tượng tạm -> rvalue
    process(name + "!");            // kết quả biểu thức -> rvalue
    process(std::move(name));       // std::move ép thành rvalue
}
```

**Kết quả:**

```text
lvalue: Nam
rvalue: Tam
rvalue: Nam!
rvalue: Nam
```

> 💡 **`std::move` không di chuyển gì cả!** Nó chỉ là phép **ép kiểu** sang rvalue, như một lời nói "tôi không cần đối tượng này nữa, ai muốn lấy thì lấy". Việc di chuyển thật chỉ xảy ra khi đối tượng được truyền vào hàm tạo/toán tử gán di chuyển. Ở ví dụ trên, `name` vẫn còn nguyên vì `process` chỉ in ra.

### 15.2 Move trong thực tế: `std::vector`

```cpp
#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct Tracker {
    std::string name;
    explicit Tracker(std::string n) : name(std::move(n)) {}
    Tracker(const Tracker &o) : name(o.name) { std::cout << "  copy " << name << '\n'; }
    Tracker(Tracker &&o) noexcept : name(std::move(o.name)) { std::cout << "  move " << name << '\n'; }
};

int main() {
    std::vector<Tracker> v;
    v.reserve(4);                       // cấp phát trước để kết quả dễ đọc
    Tracker t("A");

    std::cout << "push_back(t):\n";
    v.push_back(t);                     // t là lvalue -> COPY

    std::cout << "push_back(std::move(t)):\n";
    v.push_back(std::move(t));          // ép thành rvalue -> MOVE

    std::cout << "push_back(Tracker(\"B\")):\n";
    v.push_back(Tracker("B"));          // đối tượng tạm -> MOVE

    std::cout << "emplace_back(\"C\"):\n";
    v.emplace_back("C");                // dựng TẠI CHỖ trong vector -> không copy, không move

    std::cout << "v.size() = " << v.size() << '\n';
}
```

**Kết quả:**

```text
push_back(t):
  copy A
push_back(std::move(t)):
  move A
push_back(Tracker("B")):
  move B
emplace_back("C"):
v.size() = 4
```

### 15.3 Các quy tắc thực hành

1. **Đối tượng sau khi bị move** ở trạng thái "hợp lệ nhưng không xác định". Chỉ nên hủy nó hoặc gán giá trị mới — đừng đọc nội dung.
2. **Không viết `return std::move(local);`** — trình biên dịch đã tự tối ưu việc trả về biến cục bộ (RVO/NRVO); thêm `std::move` còn có thể làm chậm hơn.

   ```cpp
   std::vector<int> make() {
       std::vector<int> v(1000);
       return v;               // ĐÚNG: không copy
   }
   ```

3. **Đánh dấu hàm tạo di chuyển là `noexcept`.** Khi `std::vector` cần cấp phát lại, nó chỉ dùng move nếu move được đảm bảo không ném ngoại lệ; nếu không, nó sẽ **copy** toàn bộ để giữ an toàn.
4. **Nhận tham số "để giữ lại" theo giá trị rồi move** — như hàm tạo `Tracker(std::string n) : name(std::move(n))`. Người gọi truyền lvalue thì tốn 1 copy, truyền rvalue thì chỉ tốn move.

---

## Chương 16. Con trỏ thông minh

### 16.1 Ba loại smart pointer

| Loại | Sở hữu | Dùng khi | Chi phí |
|---|---|---|---|
| `std::unique_ptr<T>` | **Độc quyền** — chỉ một chủ | **Mặc định** cho mọi đối tượng cấp phát động | Gần như bằng con trỏ thô |
| `std::shared_ptr<T>` | **Chia sẻ** — đếm số chủ | Nhiều nơi cùng sở hữu, không rõ ai hủy cuối cùng | Có bộ đếm tham chiếu (tốn hơn) |
| `std::weak_ptr<T>` | **Không sở hữu** — chỉ "quan sát" | Tham chiếu tới đối tượng do `shared_ptr` quản lý mà không giữ nó sống | |

**Quy tắc:** trong C++ hiện đại, **gần như không bao giờ viết `new`/`delete` trực tiếp**. Dùng `std::make_unique` / `std::make_shared`.

```cpp
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

struct Texture {
    std::string file;
    explicit Texture(std::string f) : file(std::move(f)) { std::cout << "  Load " << file << '\n'; }
    ~Texture() { std::cout << "  Free " << file << '\n'; }
};

struct FileCloser {                          // "deleter" tùy chỉnh cho FILE*
    void operator()(std::FILE *f) const {
        if (f) std::fclose(f);
    }
};

int main() {
    std::cout << std::boolalpha;

    std::cout << "1) unique_ptr:\n";
    {
        auto tex = std::make_unique<Texture>("hero.png");
        // auto copy = tex;                  // LỖI biên dịch: unique_ptr không sao chép được
        auto owner = std::move(tex);         // chuyển quyền sở hữu
        std::cout << "  tex rong? " << (tex == nullptr) << '\n';
    }                                        // owner ra khỏi phạm vi -> tự giải phóng

    std::cout << "2) shared_ptr:\n";
    std::shared_ptr<Texture> keep;
    {
        auto grass = std::make_shared<Texture>("grass.png");
        keep = grass;                                                  // thêm một chủ
        std::cout << "  use_count = " << grass.use_count() << '\n';
    }                                                                  // grass hủy, keep còn giữ
    std::cout << "  use_count = " << keep.use_count() << '\n';
    keep.reset();                                                      // chủ cuối cùng buông -> giải phóng

    std::cout << "3) weak_ptr:\n";
    std::weak_ptr<Texture> watcher;
    {
        auto sky = std::make_shared<Texture>("sky.png");
        watcher = sky;                                                 // không tăng use_count
        if (auto locked = watcher.lock())                              // thử "mượn" tạm thời
            std::cout << "  Van con: " << locked->file << '\n';
    }
    std::cout << "  expired = " << watcher.expired() << '\n';

    std::cout << "4) deleter tuy chinh:\n";
    {
        std::unique_ptr<std::FILE, FileCloser> f(std::fopen("log.txt", "w"));
        if (f) std::fputs("hello\n", f.get());   // .get() lấy con trỏ thô để truyền cho hàm C
    }                                            // fclose tự động
    std::remove("log.txt");
    std::cout << "Ket thuc\n";
}
```

**Kết quả:**

```text
1) unique_ptr:
  Load hero.png
  tex rong? true
  Free hero.png
2) shared_ptr:
  Load grass.png
  use_count = 2
  use_count = 1
  Free grass.png
3) weak_ptr:
  Load sky.png
  Van con: sky.png
  Free sky.png
  expired = true
4) deleter tuy chinh:
Ket thuc
```

### 16.2 Vòng tham chiếu và cách phá

Hai đối tượng giữ `shared_ptr` tới nhau sẽ **không bao giờ được giải phóng** (mỗi bên giữ bộ đếm của bên kia ≥ 1):

```cpp
struct Node {
    std::shared_ptr<Node> next;
    std::weak_ptr<Node>   prev;    // dùng weak_ptr cho chiều ngược lại để phá vòng
};
```

**Quy ước phổ biến:** quan hệ **cha → con** dùng `unique_ptr` hoặc `shared_ptr`; quan hệ **con → cha** dùng con trỏ thô (nếu chắc chắn cha sống lâu hơn) hoặc `weak_ptr`.

### 16.3 Con trỏ thô vẫn còn chỗ dùng

Con trỏ thô `T*` và tham chiếu `T&` vẫn hợp lệ khi **không sở hữu** — chỉ "mượn" để dùng:

```cpp
void render(const Texture &tex);     // mượn, không quan tâm ai sở hữu
void render(const Texture *tex);     // mượn, có thể là nullptr

auto tex = std::make_unique<Texture>("a.png");
render(*tex);
render(tex.get());
```

**Tóm tắt:** smart pointer thể hiện **quyền sở hữu**; con trỏ thô/tham chiếu thể hiện **sự mượn**.

---

## Chương 17. Kế thừa và đa hình

### 17.1 Ví dụ: hệ thống thực thể trong game

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Entity {                                     // lớp cơ sở trừu tượng
public:
    explicit Entity(std::string name) : name_(std::move(name)) {}
    virtual ~Entity() = default;                   // BẮT BUỘC khi dùng đa hình

    virtual void update() = 0;                     // hàm thuần ảo: lớp con PHẢI cài đặt
    virtual std::string kind() const { return "Entity"; }
    const std::string &name() const { return name_; }

protected:                                         // lớp con truy cập được
    std::string name_;
};

class Player : public Entity {
public:
    using Entity::Entity;                          // dùng lại hàm tạo của lớp cha
    void update() override { std::cout << name_ << " doc input tu ban phim\n"; }
    std::string kind() const override { return "Player"; }
};

class Enemy : public Entity {
public:
    Enemy(std::string name, int hp) : Entity(std::move(name)), hp_(hp) {}
    void update() override {
        std::cout << name_ << " (hp " << hp_ << ") duoi theo nguoi choi\n";
    }
    std::string kind() const override { return "Enemy"; }

private:
    int hp_;
};

class Boss final : public Enemy {                  // final: không cho kế thừa tiếp
public:
    explicit Boss(std::string name) : Enemy(std::move(name), 500) {}
    void update() override {
        Enemy::update();                           // gọi phiên bản của lớp cha
        std::cout << "  ...va tung chieu dac biet!\n";
    }
    std::string kind() const override { return "Boss"; }
};

void describe(const Entity &e) {                   // nhận MỌI loại Entity
    std::cout << e.name() << " la " << e.kind() << '\n';
}

int main() {
    std::vector<std::unique_ptr<Entity>> world;
    world.push_back(std::make_unique<Player>("Hero"));
    world.push_back(std::make_unique<Enemy>("Goblin", 30));
    world.push_back(std::make_unique<Boss>("Dragon"));

    for (const auto &e : world) e->update();       // mỗi đối tượng gọi đúng hàm của nó
    for (const auto &e : world) describe(*e);
}                                                  // unique_ptr tự hủy -> gọi đúng hàm hủy
```

**Kết quả:**

```text
Hero doc input tu ban phim
Goblin (hp 30) duoi theo nguoi choi
Dragon (hp 500) duoi theo nguoi choi
  ...va tung chieu dac biet!
Hero la Player
Goblin la Enemy
Dragon la Boss
```

### 17.2 Các từ khóa cần nắm

| Từ khóa | Ý nghĩa |
|---|---|
| `virtual` | Hàm có thể được lớp con ghi đè; lời gọi qua con trỏ/tham chiếu lớp cha sẽ gọi **phiên bản của lớp thật** |
| `= 0` | Hàm **thuần ảo**; lớp chứa nó là **lớp trừu tượng** (không tạo đối tượng trực tiếp được) |
| `override` | Khẳng định đang ghi đè — sai tên/tham số sẽ **báo lỗi biên dịch**. **Luôn viết.** |
| `final` | Cấm ghi đè tiếp (với hàm) hoặc cấm kế thừa (với lớp) |
| `protected` | Lớp con truy cập được, bên ngoài thì không |

### 17.3 Đa hình hoạt động thế nào? (vtable)

Mỗi lớp có hàm ảo sở hữu một **bảng hàm ảo (vtable)**; mỗi đối tượng chứa một con trỏ ẩn tới bảng của lớp mình:

```text
 Đối tượng Boss                 vtable của Boss
┌────────────────┐            ┌──────────────────────┐
│ vptr ──────────┼──────────► │ ~Boss()              │
│ name_ "Dragon" │            │ Boss::update()       │
│ hp_   500      │            │ Boss::kind()         │
└────────────────┘            └──────────────────────┘

e->update()  ==>  (*(e->vptr)[update])(e)   // tra bảng lúc CHẠY
```

Chi phí: thêm một con trỏ trong mỗi đối tượng và một lần tra bảng mỗi lời gọi — nhỏ, nhưng đáng kể nếu gọi hàng triệu lần mỗi khung hình.

### 17.4 Ba lỗi kinh điển

**1. Thiếu hàm hủy ảo:**

```cpp
struct Base { ~Base() {} };                       // KHÔNG virtual
struct Derived : Base { std::vector<int> data; };
std::unique_ptr<Base> p = std::make_unique<Derived>();
// Khi p bị hủy: chỉ ~Base() chạy -> data của Derived bị rò rỉ (UB)
```

**Quy tắc:** lớp nào có hàm `virtual` thì hàm hủy cũng phải `virtual`.

**2. Cắt đối tượng (object slicing):**

```cpp
Boss boss("Dragon");
Entity copy = boss;     // (nếu Entity không trừu tượng) chỉ chép PHẦN Entity, mất phần Boss
```

**Quy tắc:** làm việc với đối tượng đa hình qua **con trỏ hoặc tham chiếu**, không truyền theo giá trị.

**3. Gọi hàm ảo trong hàm tạo/hàm hủy:** lúc đó đối tượng chưa (hoặc không còn) là lớp con, nên phiên bản của lớp con **không** được gọi.

> 🎮 **Góc nhìn game engine:** Cây kế thừa sâu (`Entity → Character → Enemy → FlyingEnemy → ...`) nhanh chóng trở nên cứng nhắc. Nhiều engine hiện đại ưu tiên **thành phần (composition)** — ví dụ mô hình **ECS (Entity–Component–System)**, hoặc cách Unity gắn nhiều `Component` vào một `GameObject`. Nguyên tắc chung: *"ưu tiên kết hợp hơn kế thừa"*.

---

## Chương 18. Template — lập trình tổng quát

### 18.1 Hàm template, lớp template và concept

**Ý tưởng:** viết code **một lần** cho **nhiều kiểu**. Trình biên dịch sẽ tự sinh ra phiên bản riêng cho từng kiểu được dùng.

```cpp
#include <array>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// 1) Hàm template
template <typename T>
T max_of(const std::vector<T> &v) {
    if (v.empty()) throw std::invalid_argument("vector rong");
    T best = v[0];
    for (const T &x : v)
        if (best < x) best = x;          // yêu cầu T hỗ trợ toán tử <
    return best;
}

// 2) Lớp template: ngăn xếp kích thước cố định (N là tham số template kiểu số)
template <typename T, std::size_t N>
class FixedStack {
public:
    bool push(const T &value) {
        if (size_ == N) return false;
        data_[size_++] = value;
        return true;
    }
    T pop() { return data_[--size_]; }   // giả định không rỗng, để ví dụ ngắn gọn
    bool empty() const { return size_ == 0; }

private:
    std::array<T, N> data_{};
    std::size_t size_ = 0;
};

// 3) Concept (C++20): đặt ràng buộc rõ ràng cho T
template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template <Number T>
double average(const std::vector<T> &v) {
    double sum = 0;
    for (T x : v) sum += x;
    return v.empty() ? 0.0 : sum / v.size();
}

int main() {
    std::cout << max_of(std::vector<int>{3, 9, 2}) << '\n';
    std::cout << max_of(std::vector<std::string>{"tao", "chuoi", "xoai"}) << '\n';

    FixedStack<std::string, 3> st;
    st.push("a");
    st.push("b");
    st.push("c");
    std::cout << std::boolalpha << "push khi day: " << st.push("d") << '\n';
    while (!st.empty()) std::cout << st.pop() << ' ';
    std::cout << '\n';

    std::cout << average(std::vector<int>{1, 2, 4}) << '\n';
    // average(std::vector<std::string>{"x"});   // LỖI biên dịch: string không phải Number
}
```

**Kết quả:**

```text
9
xoai
push khi day: false
c b a
2.33333
```

### 18.2 Những điều cần biết

- **Suy luận kiểu:** ở `max_of(std::vector<int>{...})`, trình biên dịch tự hiểu `T = int`. Có thể chỉ định rõ: `max_of<int>(...)`.
- **Template phải đặt trong header (`.h`/`.hpp`)**, không tách định nghĩa sang `.cpp` như hàm thường — vì trình biên dịch cần thấy toàn bộ code để sinh phiên bản cho từng kiểu.
- **`typename` và `class`** trong `template <...>` có nghĩa như nhau.
- **Concept** giúp thông báo lỗi dễ đọc: thay vì hàng trăm dòng lỗi khó hiểu từ bên trong template, bạn nhận được "ràng buộc `Number` không thỏa mãn".
- Toàn bộ **STL** (`vector<T>`, `map<K, V>`, `sort`...) được viết bằng template — hiểu template giúp bạn đọc được thông báo lỗi và tài liệu của STL.

**Template chuyên biệt hóa** — xử lý riêng cho một kiểu cụ thể:

```cpp
template <typename T>
std::string type_name() { return "unknown"; }

template <>
std::string type_name<int>() { return "int"; }    // phiên bản riêng cho int
```

---

## Chương 19. Thư viện chuẩn (STL)

### 19.1 Chọn container

| Container | Cấu trúc | Truy cập | Thêm/xóa | Dùng khi |
|---|---|---|---|---|
| `vector<T>` | Mảng động | $O(1)$ | Cuối: $O(1)$, giữa: $O(n)$ | **Mặc định** — hãy chọn đầu tiên |
| `array<T, N>` | Mảng tĩnh | $O(1)$ | — | Kích thước cố định lúc biên dịch |
| `deque<T>` | Các khối mảng | $O(1)$ | Hai đầu: $O(1)$ | Hàng đợi hai đầu |
| `list<T>` | DSLK đôi | $O(n)$ | $O(1)$ nếu có iterator | Hiếm khi thực sự cần |
| `map<K, V>` | Cây đỏ-đen | $O(\log n)$ | $O(\log n)$ | Cần khóa **có thứ tự** |
| `unordered_map<K, V>` | Bảng băm | $O(1)$ trung bình | $O(1)$ trung bình | Tra cứu nhanh theo khóa |
| `set<T>` / `unordered_set<T>` | Như trên | | | Tập hợp không trùng lặp |
| `stack`, `queue`, `priority_queue` | Bộ điều hợp | | | Ngăn xếp, hàng đợi, hàng đợi ưu tiên (heap) |

### 19.2 Container, iterator và thuật toán

**Iterator** là "con trỏ tổng quát" duyệt qua container: `begin()` trỏ phần tử đầu, `end()` trỏ **sau** phần tử cuối. Các thuật toán trong `<algorithm>` làm việc trên cặp iterator `[begin, end)`.

```cpp
#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

int main() {
    // 1) vector + thuật toán
    std::vector<int> v = {5, 3, 8, 1, 9, 2};
    std::sort(v.begin(), v.end());                              // 1 2 3 5 8 9

    auto it = std::find(v.begin(), v.end(), 8);
    if (it != v.end()) std::cout << "Tim thay 8 o vi tri " << (it - v.begin()) << '\n';

    auto evens = std::count_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    int sum = std::accumulate(v.begin(), v.end(), 0);
    std::cout << "So chan: " << evens << ", tong: " << sum << '\n';

    std::erase_if(v, [](int x) { return x < 3; });              // C++20: xóa theo điều kiện
    for (int x : v) std::cout << x << ' ';
    std::cout << '\n';

    // 2) unordered_map: đếm tần suất từ
    std::string text = "the cat and the dog and the bird";
    std::unordered_map<std::string, int> freq;
    std::istringstream iss(text);
    for (std::string w; iss >> w;) ++freq[w];                   // [] tự tạo khóa (giá trị 0) nếu chưa có

    // 3) Chép sang vector để sắp xếp theo số lần xuất hiện
    std::vector<std::pair<std::string, int>> items(freq.begin(), freq.end());
    std::sort(items.begin(), items.end(), [](const auto &a, const auto &b) {
        if (a.second != b.second) return a.second > b.second;   // nhiều hơn đứng trước
        return a.first < b.first;                               // bằng nhau thì theo ABC
    });
    for (const auto &[word, count] : items)                     // structured binding
        std::cout << word << ": " << count << '\n';

    // 4) map: khóa luôn được sắp xếp
    std::map<int, std::string> ranking = {{3, "Chi"}, {1, "An"}, {2, "Binh"}};
    for (const auto &[rank, name] : ranking) std::cout << rank << ". " << name << '\n';
    if (auto found = ranking.find(2); found != ranking.end())
        std::cout << "Hang 2: " << found->second << '\n';
}
```

**Kết quả:**

```text
Tim thay 8 o vi tri 4
So chan: 2, tong: 28
3 5 8 9
the: 3
and: 2
bird: 1
cat: 1
dog: 1
1. An
2. Binh
3. Chi
Hang 2: Binh
```

**Thuật toán hay dùng:**

| Thuật toán | Công dụng |
|---|---|
| `sort`, `stable_sort` | Sắp xếp (`stable_sort` giữ thứ tự các phần tử bằng nhau) |
| `find`, `find_if` | Tìm phần tử |
| `count`, `count_if` | Đếm |
| `any_of`, `all_of`, `none_of` | Kiểm tra điều kiện |
| `transform` | Biến đổi từng phần tử |
| `accumulate` (`<numeric>`) | Cộng dồn / gộp |
| `min_element`, `max_element` | Tìm nhỏ nhất / lớn nhất |
| `lower_bound`, `binary_search` | Tìm kiếm nhị phân trên dãy **đã sắp xếp** |
| `std::erase`, `std::erase_if` | Xóa phần tử (C++20) |

### 19.3 Bẫy: iterator bị vô hiệu hóa

```cpp
std::vector<int> v = {1, 2, 3};
int &first = v[0];
v.push_back(4);          // có thể cấp phát lại -> mọi tham chiếu/iterator cũ đều "treo"
std::cout << first;      // UB!

for (auto it = v.begin(); it != v.end(); ++it)
    if (*it == 2) v.erase(it);   // UB: it không còn hợp lệ sau erase
```

**Quy tắc:** không giữ tham chiếu, con trỏ hay iterator vào `vector` qua các thao tác làm thay đổi kích thước. Để xóa theo điều kiện, dùng `std::erase_if`.

### 19.4 `map::operator[]` và `at()`

```cpp
std::map<std::string, int> m;
int a = m["x"];      // nếu "x" chưa có: TỰ THÊM "x" với giá trị 0
int b = m.at("y");   // nếu "y" chưa có: ném ngoại lệ std::out_of_range
auto it = m.find("z");   // chỉ tìm, không thêm; so sánh it với m.end()
```

Dùng `[]` khi **muốn** thêm (đếm tần suất), dùng `find`/`contains` (C++20) khi chỉ muốn **kiểm tra**.

---

## Chương 20. Lambda và `std::function`

### 20.1 Cú pháp lambda

```text
[capture](tham_số) mutable -> kiểu_trả_về { thân hàm }
```

| Capture | Ý nghĩa |
|---|---|
| `[]` | Không dùng biến bên ngoài |
| `[x]` | Chép giá trị của `x` tại thời điểm tạo lambda |
| `[&x]` | Tham chiếu tới `x` |
| `[=]` | Chép mọi biến được dùng |
| `[&]` | Tham chiếu mọi biến được dùng |
| `[this]` | Truy cập thành viên của đối tượng hiện tại |
| `[n = 0]` | Tạo biến mới ngay trong capture (C++14) |

```cpp
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

// Hệ thống sự kiện đơn giản: lưu nhiều callback, gọi khi có sự kiện
class EventBus {
public:
    using Handler = std::function<void(const std::string &)>;
    void subscribe(Handler h) { handlers_.push_back(std::move(h)); }
    void emit(const std::string &msg) const {
        for (const auto &h : handlers_) h(msg);
    }

private:
    std::vector<Handler> handlers_;
};

int main() {
    // 1) Chụp theo giá trị và theo tham chiếu
    int bonus = 10;
    auto add_by_value = [bonus](int x) { return x + bonus; };
    auto add_by_ref = [&bonus](int x) { return x + bonus; };
    bonus = 100;
    std::cout << add_by_value(1) << ' ' << add_by_ref(1) << '\n';

    // 2) mutable: cho phép sửa bản sao nằm bên trong lambda
    auto counter = [n = 0]() mutable { return ++n; };
    counter();
    counter();
    std::cout << "counter: " << counter() << '\n';

    // 3) Lambda làm tiêu chí sắp xếp
    std::vector<std::string> names = {"Linh", "An", "Khanh", "Binh"};
    std::stable_sort(names.begin(), names.end(),
                     [](const std::string &a, const std::string &b) { return a.size() < b.size(); });
    for (const auto &n : names) std::cout << n << ' ';
    std::cout << '\n';

    // 4) std::function: lưu lambda để gọi về sau
    EventBus bus;
    int score = 0;
    bus.subscribe([](const std::string &m) { std::cout << "[log] " << m << '\n'; });
    bus.subscribe([&score](const std::string &m) {
        if (m == "coin") score += 5;
    });
    bus.emit("coin");
    bus.emit("coin");
    std::cout << "score = " << score << '\n';
}
```

**Kết quả:**

```text
11 101
counter: 3
An Linh Binh Khanh
[log] coin
[log] coin
score = 10
```

**Giải thích:**

- `add_by_value` chép `bonus = 10` **lúc tạo**, nên đổi `bonus` sau đó không ảnh hưởng.
- `stable_sort` giữ nguyên thứ tự ban đầu của `"Linh"` và `"Binh"` (cùng độ dài 4). Với `sort` thường, thứ tự của chúng không được đảm bảo.
- `std::function<void(const std::string &)>` chứa được **bất kỳ thứ gì gọi được** với chữ ký đó: lambda, con trỏ hàm, đối tượng hàm. Đổi lại, nó tốn chi phí hơn gọi lambda trực tiếp một chút.

### 20.2 Bẫy: tham chiếu treo trong lambda

```cpp
std::function<int()> make_counter() {
    int count = 0;
    return [&count]() { return ++count; };   // SAI: count bị hủy khi hàm kết thúc
}
// Sửa: return [count = 0]() mutable { return ++count; };
```

**Quy tắc:** nếu lambda **sống lâu hơn** phạm vi hiện tại (lưu vào biến thành viên, trả về, chạy trên luồng khác), đừng chụp biến cục bộ bằng tham chiếu.

---

## Chương 21. Xử lý lỗi: ngoại lệ và `std::optional`

```cpp
#include <charconv>
#include <cstddef>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

// Lớp ngoại lệ riêng, kế thừa từ lớp chuẩn
class ConfigError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Cách 1: NÉM NGOẠI LỆ khi gặp lỗi bất thường mà hàm không tự xử lý được
int parse_port(std::string_view text) {
    int value = 0;
    auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (ec != std::errc{} || ptr != text.data() + text.size())
        throw ConfigError("cong khong hop le: '" + std::string(text) + "'");
    if (value < 1 || value > 65535)
        throw std::out_of_range("cong ngoai khoang 1-65535: " + std::to_string(value));
    return value;
}

// Cách 2: std::optional khi "không có kết quả" là chuyện BÌNH THƯỜNG
std::optional<std::size_t> find_index(const std::vector<std::string> &v, std::string_view key) {
    for (std::size_t i = 0; i < v.size(); ++i)
        if (v[i] == key) return i;
    return std::nullopt;
}

int main() {
    for (std::string_view s : {"8080", "abc", "70000"}) {
        try {
            int port = parse_port(s);              // tính trước, in sau (xem ghi chú bên dưới)
            std::cout << "Port: " << port << '\n';
        } catch (const ConfigError &e) {          // bắt lớp CỤ THỂ trước
            std::cout << "ConfigError: " << e.what() << '\n';
        } catch (const std::exception &e) {        // rồi mới đến lớp TỔNG QUÁT
            std::cout << "Loi khac: " << e.what() << '\n';
        }
    }

    std::vector<std::string> items = {"sword", "shield", "potion"};
    if (auto idx = find_index(items, "shield"))    // optional chuyển được sang bool
        std::cout << "shield o vi tri " << *idx << '\n';
    std::cout << "bow o vi tri " << find_index(items, "bow").value_or(999) << '\n';
}
```

**Kết quả:**

```text
Port: 8080
ConfigError: cong khong hop le: 'abc'
Loi khac: cong ngoai khoang 1-65535: 70000
shield o vi tri 1
bow o vi tri 999
```

> 🐛 **Bẫy nhỏ nhưng thật:** nếu viết gộp `std::cout << "Port: " << parse_port(s);`, chữ `Port: ` sẽ được in ra **trước** khi `parse_port` ném ngoại lệ (vì C++17 đảm bảo `<<` được tính từ trái sang phải), và output sẽ thành `Port: ConfigError: ...`. Khi một bước có thể ném ngoại lệ, hãy tính nó **trước** rồi mới in.

### 21.1 Khi nào dùng cách nào?

| Cách | Dùng khi | Ví dụ |
|---|---|---|
| **Ngoại lệ** | Lỗi hiếm, bất thường, cần báo lên nhiều tầng | Hết bộ nhớ, file cấu hình hỏng |
| **`std::optional<T>`** | "Không có kết quả" là bình thường | Tìm kiếm không thấy |
| **Mã lỗi / `bool`** | Code cần hiệu năng cao hoặc môi trường tắt ngoại lệ | Vòng lặp game, hệ thống nhúng |
| **`std::expected<T, E>`** (C++23) | Muốn trả về **hoặc** kết quả **hoặc** thông tin lỗi | Phân tích cú pháp |
| **`assert`** | Lỗi **lập trình** — điều "không bao giờ được xảy ra" | Chỉ số âm do bug |

### 21.2 Quy tắc với ngoại lệ

- **Ném theo giá trị, bắt theo tham chiếu hằng:** `throw X(...)` và `catch (const X &e)`.
- **Bắt từ cụ thể đến tổng quát**, vì `catch` được thử theo thứ tự.
- **RAII giúp code an toàn với ngoại lệ:** khi ngoại lệ bay qua, hàm hủy của mọi đối tượng cục bộ vẫn chạy → không rò rỉ tài nguyên. Đây là lý do không nên dùng `new`/`delete` thủ công.
- **Hàm hủy và hàm move không được ném ngoại lệ** — đánh dấu `noexcept`.
- **Không dùng ngoại lệ để điều khiển luồng thông thường** (ví dụ thoát vòng lặp).

> 🎮 Nhiều engine game (ví dụ Unreal Engine) mặc định **tắt ngoại lệ** vì lý do hiệu năng và kích thước file. Trong môi trường đó, lỗi được xử lý bằng mã lỗi, `optional` hoặc `assert`.

---

## Chương 22. Các tiện ích C++ hiện đại

```cpp
#include <array>
#include <format>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

enum class Color { Red, Green, Blue };      // enum có phạm vi: không tự đổi sang int

constexpr int square(int x) { return x * x; }
constexpr int kTileCount = square(16);      // tính ngay lúc biên dịch

// span: "nhìn vào" một dải phần tử liên tiếp, không sở hữu, không sao chép
int sum(std::span<const int> values) {
    int s = 0;
    for (int v : values) s += v;
    return s;
}

// variant: giữ MỘT trong nhiều kiểu — phiên bản an toàn của union
using Command = std::variant<int, std::string>;

std::string describe(const Command &cmd) {
    if (const int *n = std::get_if<int>(&cmd)) return std::format("so {}", *n);
    return std::format("chuoi \"{}\"", std::get<std::string>(cmd));
}

[[nodiscard]] bool save_game() { return true; }    // cảnh báo nếu bỏ qua giá trị trả về

int main() {
    Color c = Color::Green;
    std::cout << "Green = " << static_cast<int>(c) << '\n';
    std::cout << "kTileCount = " << kTileCount << '\n';

    std::array<int, 3> arr = {1, 2, 3};
    std::vector<int> vec = {10, 20, 30, 40};
    int raw[] = {5, 5};
    std::cout << sum(arr) << ' ' << sum(vec) << ' ' << sum(raw) << '\n';   // một hàm cho mọi nguồn
    std::cout << sum(std::span(vec).subspan(1, 2)) << '\n';                // chỉ phần tử 1 và 2

    for (const Command &cmd : {Command{42}, Command{std::string("jump")}})
        std::cout << describe(cmd) << '\n';

    std::string_view sv = "Hello, world";
    std::cout << sv.substr(7) << '\n';                                    // không cấp phát bộ nhớ

    std::cout << std::format("[{:>6}] [{:<5}] [{:.3f}]\n", "HP", 100, 3.14159);

    if (!save_game()) return 1;
}
```

**Kết quả:**

```text
Green = 1
kTileCount = 256
6 100 10
50
so 42
chuoi "jump"
world
[    HP] [100  ] [3.142]
```

| Tính năng | Thay cho | Lợi ích |
|---|---|---|
| `enum class` | `enum` | Không "rò" tên ra ngoài, không tự đổi sang `int` |
| `constexpr` | `#define`, `const` | Tính toán lúc biên dịch, có kiểu |
| `std::span<T>` | Cặp `(T*, size_t)` | Một tham số, biết kích thước, dùng với mọi mảng liên tục |
| `std::string_view` | `const std::string &` / `const char *` | Nhận mọi loại chuỗi mà không sao chép |
| `std::variant` | `union` + `enum` nhãn | Luôn biết đang giữ kiểu nào, truy cập sai sẽ bị phát hiện |
| `std::format` (C++20) | `printf`, chuỗi `<<` dài | An toàn kiểu, cú pháp gọn (cần GCC 13+, Clang 17+ hoặc MSVC 2019+) |
| `[[nodiscard]]` | Ghi chú | Trình biên dịch cảnh báo khi quên kiểm tra kết quả |
| Structured binding | `.first`, `.second` | `auto [key, value] = ...` dễ đọc |

> ⚠️ **`string_view` và `span` không sở hữu dữ liệu.** Chúng trở thành "treo" nếu dữ liệu gốc bị hủy:
>
> ```cpp
> std::string_view bad() {
>     std::string s = "tam thoi";
>     return s;               // SAI: s bị hủy, view trỏ vào vùng nhớ đã giải phóng
> }
> ```
>
> Dùng chúng làm **tham số hàm** là an toàn nhất; cẩn thận khi lưu chúng lại hoặc trả về.

---

## Chương 23. Đa luồng cơ bản

### 23.1 Luồng, mutex, atomic, async

```cpp
#include <atomic>
#include <future>
#include <iostream>
#include <mutex>
#include <numeric>
#include <thread>
#include <vector>

int main() {
    // 1) Nhiều luồng cùng sửa một biến -> cần mutex
    long long counter = 0;
    std::mutex m;
    auto work = [&] {
        for (int i = 0; i < 100000; ++i) {
            std::lock_guard<std::mutex> lock(m);   // RAII: tự unlock khi ra khỏi khối
            ++counter;
        }
    };
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t) threads.emplace_back(work);
    for (auto &th : threads) th.join();            // BẮT BUỘC chờ luồng kết thúc
    std::cout << "counter = " << counter << '\n';

    // 2) atomic: gọn hơn cho biến đếm đơn giản
    std::atomic<int> hits{0};
    {
        std::jthread a([&] { for (int i = 0; i < 50000; ++i) ++hits; });
        std::jthread b([&] { for (int i = 0; i < 50000; ++i) ++hits; });
    }                                              // jthread (C++20) tự join khi bị hủy
    std::cout << "hits = " << hits << '\n';

    // 3) std::async: chạy một tác vụ nền và lấy kết quả qua future
    std::vector<int> data(1'000'000, 1);
    auto mid = data.begin() + data.size() / 2;
    auto left = std::async(std::launch::async,
                           [&] { return std::accumulate(data.begin(), mid, 0LL); });
    long long right = std::accumulate(mid, data.end(), 0LL);   // luồng chính làm nửa còn lại
    std::cout << "tong = " << left.get() + right << '\n';      // get() chờ kết quả
}
```

**Kết quả:**

```text
counter = 400000
hits = 100000
tong = 1000000
```

### 23.2 Data race — lỗi nguy hiểm nhất

Nếu bỏ `lock_guard` ở ví dụ trên, `counter` thường sẽ **nhỏ hơn 400000** và thay đổi mỗi lần chạy. Lý do: `++counter` gồm 3 bước (đọc → cộng → ghi); hai luồng có thể cùng đọc một giá trị cũ:

```text
Luồng 1: đọc 5 ──────── cộng → 6 ──── ghi 6
Luồng 2:      đọc 5 ─── cộng → 6 ───────────── ghi 6     (mất một lần tăng!)
```

**Data race** = hai luồng cùng truy cập một vùng nhớ, ít nhất một luồng **ghi**, và không có đồng bộ. Đây là **UB**.

**Phát hiện:** biên dịch với `-fsanitize=thread` (ThreadSanitizer).

### 23.3 Quy tắc an toàn

| Công cụ | Dùng khi |
|---|---|
| `std::atomic<T>` | Biến đơn giản (số đếm, cờ) |
| `std::mutex` + `std::lock_guard` | Bảo vệ một đoạn code / nhiều biến liên quan |
| `std::scoped_lock(m1, m2)` | Khóa **nhiều** mutex cùng lúc mà không bị deadlock |
| `std::condition_variable` | Luồng chờ đến khi có dữ liệu (mô hình producer–consumer) |
| `std::jthread` | Thay `std::thread` — tự `join`, không quên được |

- **Không bao giờ gọi `lock()`/`unlock()` thủ công** — luôn dùng lớp RAII.
- **Giữ khóa càng ngắn càng tốt**; không gọi hàm chậm (I/O, mạng) khi đang giữ khóa.
- **Deadlock** xảy ra khi hai luồng khóa hai mutex theo **thứ tự ngược nhau**. Luôn khóa theo cùng một thứ tự, hoặc dùng `std::scoped_lock`.
- **Cách an toàn nhất là không chia sẻ:** chia dữ liệu cho mỗi luồng một phần riêng (như ví dụ `std::async`), chỉ gộp kết quả ở cuối.

---

## Chương 24. Dự án tổng hợp: hệ thống hạt (particle system)

Dự án nhỏ này kết hợp: `struct` + nạp chồng toán tử (Ch.13), `vector` + `erase_if` (Ch.19), lambda + `std::function` (Ch.20), `std::format` + `constexpr` (Ch.22) — và đây là thành phần có mặt trong hầu hết game engine (khói, lửa, vụ nổ).

```cpp
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <format>
#include <functional>
#include <iostream>
#include <numbers>
#include <utility>
#include <vector>

struct Vec2 {
    double x = 0, y = 0;
    Vec2 operator*(double k) const { return {x * k, y * k}; }
    Vec2 &operator+=(const Vec2 &o) {
        x += o.x;
        y += o.y;
        return *this;
    }
};

struct Particle {
    Vec2 pos;
    Vec2 vel;
    double life;   // thời gian sống còn lại (giây)
};

class ParticleSystem {
public:
    using DeathCallback = std::function<void(const Particle &)>;

    explicit ParticleSystem(Vec2 gravity) : gravity_(gravity) {}

    void on_death(DeathCallback cb) { on_death_ = std::move(cb); }

    // Bắn `count` hạt tỏa đều theo các hướng quanh `origin`
    void emit(Vec2 origin, int count, double speed) {
        for (int i = 0; i < count; ++i) {
            double angle = 2 * std::numbers::pi * (i + 0.5) / count;
            Vec2 vel{std::cos(angle) * speed, std::sin(angle) * speed};
            particles_.push_back({origin, vel, 1.0 + 0.5 * i});
        }
    }

    // Cập nhật một khung hình (dt giây)
    void update(double dt) {
        for (auto &p : particles_) {
            p.vel += gravity_ * dt;     // trọng lực thay đổi vận tốc
            p.pos += p.vel * dt;        // vận tốc thay đổi vị trí
            p.life -= dt;
        }
        std::erase_if(particles_, [this](const Particle &p) {
            if (p.life > 0) return false;
            if (on_death_) on_death_(p);   // báo cho bên đăng ký trước khi xóa
            return true;
        });
    }

    std::size_t alive() const { return particles_.size(); }

    double highest_y() const {
        auto it = std::max_element(particles_.begin(), particles_.end(),
                                   [](const Particle &a, const Particle &b) { return a.pos.y < b.pos.y; });
        return it == particles_.end() ? 0.0 : it->pos.y;
    }

private:
    Vec2 gravity_;
    std::vector<Particle> particles_;
    DeathCallback on_death_;
};

int main() {
    constexpr double kDt = 0.5;
    ParticleSystem ps({0, -10});

    int dead = 0;
    ps.on_death([&dead](const Particle &p) {
        ++dead;
        std::cout << std::format("  hat chet tai ({:.1f}, {:.1f})\n", p.pos.x, p.pos.y);
    });

    ps.emit({0, 0}, 4, 10.0);
    for (int frame = 1; frame <= 5; ++frame) {
        ps.update(kDt);
        std::cout << std::format("Frame {}: con {} hat", frame, ps.alive());
        if (ps.alive() > 0) std::cout << std::format(", cao nhat y = {:.2f}", ps.highest_y());
        std::cout << '\n';
    }
    std::cout << "Tong so hat da chet: " << dead << '\n';
}
```

**Kết quả:**

```text
Frame 1: con 4 hat, cao nhat y = 1.04
  hat chet tai (7.1, -0.4)
Frame 2: con 3 hat, cao nhat y = -0.43
  hat chet tai (-10.6, -4.4)
Frame 3: con 2 hat, cao nhat y = -25.61
  hat chet tai (-14.1, -39.1)
Frame 4: con 1 hat, cao nhat y = -39.14
  hat chet tai (17.7, -55.2)
Frame 5: con 0 hat
Tong so hat da chet: 4
```

Các dòng `hat chet tai ...` được in **trong lúc** gọi `update` của khung hình kế tiếp, nên chúng xuất hiện ngay trước dòng `Frame` tương ứng. Hạt nào có `life` nhỏ hơn thì chết trước.

**Gợi ý mở rộng** (bài tập tự làm):

1. Thêm màu và kích thước giảm dần theo `life`.
2. Tách `Vec2` ra header riêng và dùng chung cho các dự án khác.
3. Dùng thư viện đồ họa như **raylib** hoặc **SFML** để vẽ các hạt lên màn hình.
4. Tối ưu: thay vì `erase_if`, đánh dấu hạt chết và **tái sử dụng** (object pool) để tránh cấp phát liên tục.

---

## Bài tập Phần II

**Bài II.1.** Viết lớp template `Stack<T>` dựa trên `std::vector<T>`, có `push`, `pop`, `top`, `empty`, `size`. Gọi `pop`/`top` trên stack rỗng phải ném `std::out_of_range`.

**Bài II.2.** Cho danh sách sinh viên `{tên, điểm}`. In **3 sinh viên điểm cao nhất** (bằng điểm thì xếp theo tên) và **điểm trung bình**, chỉ dùng thuật toán STL và lambda.

**Bài II.3.** Cài đặt **cây nhị phân tìm kiếm** với `insert`, `contains`, in theo thứ tự giữa (in-order). Các nút được quản lý bằng `std::unique_ptr` — không có `new`/`delete` nào.

**Bài II.4.** Viết lớp RAII `ScopeLogger` in `vao <tên>` khi được tạo và `ra <tên>` khi bị hủy. Chứng minh rằng `ra` vẫn được in khi hàm thoát do ngoại lệ.

**Bài II.5.** Viết hàm template `clamp_all(std::vector<T> &v, const T &lo, const T &hi)` ép mọi phần tử vào đoạn `[lo, hi]`, với ràng buộc `T` so sánh được (dùng concept).

**Bài II.6.** Viết hàm `parallel_sum(data, n)` tính tổng một `vector<int>` bằng `n` luồng. Mỗi luồng tính một đoạn và ghi vào ô riêng của nó, **không dùng mutex**.

<details>
<summary><b>Lời giải Phần II</b> (bấm để mở)</summary>

**Bài II.1**

```cpp
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

template <typename T>
class Stack {
public:
    void push(T value) { data_.push_back(std::move(value)); }

    T pop() {
        if (data_.empty()) throw std::out_of_range("pop tren stack rong");
        T top = std::move(data_.back());
        data_.pop_back();
        return top;
    }

    const T &top() const {
        if (data_.empty()) throw std::out_of_range("top tren stack rong");
        return data_.back();
    }

    bool empty() const { return data_.empty(); }
    std::size_t size() const { return data_.size(); }

private:
    std::vector<T> data_;   // Quy tắc 0: không cần viết copy/move/hàm hủy
};

int main() {
    Stack<std::string> s;
    s.push("mot");
    s.push("hai");
    std::cout << "top = " << s.top() << ", size = " << s.size() << '\n';
    while (!s.empty()) std::cout << s.pop() << ' ';
    std::cout << '\n';

    try {
        s.pop();
    } catch (const std::out_of_range &e) {
        std::cout << "Loi: " << e.what() << '\n';
    }
}
```

```text
top = hai, size = 2
hai mot
Loi: pop tren stack rong
```

**Bài II.2**

```cpp
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

struct Student {
    std::string name;
    double score;
};

int main() {
    std::vector<Student> list = {
        {"An", 7.5}, {"Binh", 9.0}, {"Chi", 8.0}, {"Dung", 9.0}, {"Giang", 6.5},
    };

    std::sort(list.begin(), list.end(), [](const Student &a, const Student &b) {
        if (a.score != b.score) return a.score > b.score;
        return a.name < b.name;
    });

    std::size_t k = std::min<std::size_t>(3, list.size());
    for (std::size_t i = 0; i < k; ++i)
        std::cout << i + 1 << ". " << list[i].name << " - " << list[i].score << '\n';

    double total = std::accumulate(list.begin(), list.end(), 0.0,
                                   [](double acc, const Student &s) { return acc + s.score; });
    std::cout << "Trung binh: " << total / list.size() << '\n';
}
```

```text
1. Binh - 9
2. Dung - 9
3. Chi - 8
Trung binh: 8
```

Lưu ý: giá trị khởi đầu của `accumulate` là `0.0` (kiểu `double`). Nếu viết `0`, kết quả sẽ bị tính theo kiểu `int` và mất phần thập phân.

**Bài II.3**

```cpp
#include <iostream>
#include <memory>

class BST {
public:
    void insert(int value) { insert(root_, value); }

    bool contains(int value) const {
        const Node *cur = root_.get();
        while (cur != nullptr) {
            if (value == cur->value) return true;
            cur = value < cur->value ? cur->left.get() : cur->right.get();
        }
        return false;
    }

    void print_in_order() const {
        print(root_.get());
        std::cout << '\n';
    }

private:
    struct Node {
        int value;
        std::unique_ptr<Node> left, right;   // nút cha SỞ HỮU các nút con
        explicit Node(int v) : value(v) {}
    };

    // Nhận tham chiếu tới unique_ptr để có thể "gắn" nút mới vào đúng chỗ
    static void insert(std::unique_ptr<Node> &node, int value) {
        if (!node) {
            node = std::make_unique<Node>(value);
            return;
        }
        if (value < node->value) insert(node->left, value);
        else if (value > node->value) insert(node->right, value);   // bỏ qua giá trị trùng
    }

    static void print(const Node *node) {      // chỉ "mượn" -> dùng con trỏ thô
        if (node == nullptr) return;
        print(node->left.get());
        std::cout << node->value << ' ';
        print(node->right.get());
    }

    std::unique_ptr<Node> root_;   // hủy root_ -> cả cây tự giải phóng
};

int main() {
    BST tree;
    for (int x : {50, 30, 70, 20, 40, 60, 80, 30}) tree.insert(x);
    tree.print_in_order();
    std::cout << std::boolalpha << tree.contains(60) << ' ' << tree.contains(65) << '\n';
}
```

```text
20 30 40 50 60 70 80
true false
```

Lưu ý: với cây rất lệch (chèn dãy đã sắp xếp), việc hủy đệ quy qua hàng trăm nghìn tầng `unique_ptr` có thể gây tràn stack. Cây cân bằng hoặc hàm hủy dùng vòng lặp sẽ khắc phục được.

**Bài II.4**

```cpp
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

class ScopeLogger {
public:
    explicit ScopeLogger(std::string name) : name_(std::move(name)) {
        std::cout << "vao " << name_ << '\n';
    }
    ~ScopeLogger() { std::cout << "ra  " << name_ << '\n'; }

    ScopeLogger(const ScopeLogger &) = delete;             // sao chép không có ý nghĩa
    ScopeLogger &operator=(const ScopeLogger &) = delete;

private:
    std::string name_;
};

void load_level(int id) {
    ScopeLogger log("load_level");
    if (id < 0) throw std::invalid_argument("id am");
    std::cout << "  dang tai man " << id << '\n';
}

int main() {
    load_level(1);
    try {
        load_level(-1);
    } catch (const std::exception &e) {
        std::cout << "bat duoc: " << e.what() << '\n';
    }
}
```

```text
vao load_level
  dang tai man 1
ra  load_level
vao load_level
ra  load_level
bat duoc: id am
```

Dòng `ra load_level` thứ hai được in **trước** `bat duoc` — hàm hủy chạy ngay khi ngoại lệ rời khỏi `load_level`.

**Bài II.5**

```cpp
#include <algorithm>
#include <concepts>
#include <iostream>
#include <vector>

template <typename T>
    requires std::totally_ordered<T>
void clamp_all(std::vector<T> &v, const T &lo, const T &hi) {
    for (T &x : v) x = std::clamp(x, lo, hi);
}

int main() {
    std::vector<int> hp = {-20, 50, 130, 100};
    clamp_all(hp, 0, 100);
    for (int x : hp) std::cout << x << ' ';
    std::cout << '\n';

    std::vector<double> volume = {-0.5, 0.3, 1.7};
    clamp_all(volume, 0.0, 1.0);           // phải là 0.0, 1.0 — viết 0, 1 sẽ lỗi suy luận kiểu
    for (double x : volume) std::cout << x << ' ';
    std::cout << '\n';
}
```

```text
0 50 100 100
0 0.3 1
```

**Bài II.6**

```cpp
#include <cstddef>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

// Yêu cầu: 1 <= num_threads <= data.size()
long long parallel_sum(const std::vector<int> &data, std::size_t num_threads) {
    std::vector<long long> partial(num_threads, 0);   // mỗi luồng một ô riêng -> không có data race
    std::size_t chunk = data.size() / num_threads;
    {
        std::vector<std::jthread> workers;
        for (std::size_t t = 0; t < num_threads; ++t) {
            auto first = data.begin() + t * chunk;
            auto last = (t == num_threads - 1) ? data.end() : first + chunk;   // luồng cuối nhận phần dư
            workers.emplace_back([first, last, &partial, t] {
                partial[t] = std::accumulate(first, last, 0LL);
            });
        }
    }   // mọi jthread tự join ở đây
    return std::accumulate(partial.begin(), partial.end(), 0LL);
}

int main() {
    std::vector<int> data(1'000'001);
    std::iota(data.begin(), data.end(), 0);   // 0, 1, 2, ..., 1000000
    std::cout << parallel_sum(data, 4) << '\n';
    std::cout << parallel_sum(data, 7) << '\n';
}
```

```text
500000500000
500000500000
```

</details>

---

# PHỤ LỤC

## Phụ lục A. Lỗi kinh điển cần tránh

### Trong C

1. Quên `'\0'` khi tính kích thước chuỗi; dùng `strcpy`/`gets` gây tràn bộ đệm.
2. So sánh chuỗi bằng `==` thay vì `strcmp`.
3. Dùng `sizeof(arr)` bên trong hàm nhận mảng (chỉ ra kích thước con trỏ).
4. Quên `&` trong `scanf`; dùng `%f` thay vì `%lf` để đọc `double`.
5. Trả về địa chỉ biến cục bộ.
6. Quên `free` (rò rỉ), `free` hai lần, hoặc dùng sau khi `free`.
7. `a = realloc(a, ...)` — mất dữ liệu nếu `realloc` thất bại.
8. Quên `break` trong `switch`.
9. `if (x = 5)` thay vì `if (x == 5)`; `flags & MASK == 0` thiếu ngoặc.
10. Macro thiếu ngoặc.

### Trong C++

1. Dùng `new`/`delete` thủ công thay vì `std::make_unique` và container.
2. Lớp có hàm ảo nhưng hàm hủy **không** `virtual`.
3. Quên `override` → tưởng đã ghi đè nhưng thực ra tạo hàm mới.
4. Truyền đối tượng đa hình theo giá trị → cắt đối tượng.
5. Viết hàm hủy mà quên xử lý sao chép (vi phạm Quy tắc 3/5).
6. Đọc nội dung đối tượng sau khi đã `std::move`.
7. Giữ iterator/tham chiếu vào `vector` qua `push_back`/`erase`.
8. `map[key]` vô tình thêm khóa mới khi chỉ muốn kiểm tra.
9. Lambda chụp tham chiếu tới biến cục bộ nhưng sống lâu hơn biến đó.
10. `string_view`/`span` trỏ vào dữ liệu đã bị hủy.
11. `auto x = v[0];` tạo bản sao khi định sửa phần tử gốc.
12. Truy cập dữ liệu dùng chung giữa các luồng mà không đồng bộ.

---

## Phụ lục B. Công cụ làm việc

### B.1 Bộ cờ biên dịch

| Mục đích | Cờ |
|---|---|
| Học và debug | `-std=c++20 -Wall -Wextra -Wpedantic -g -O0` |
| Bắt lỗi bộ nhớ + UB | thêm `-fsanitize=address,undefined` |
| Bắt data race | `-fsanitize=thread` (không dùng chung với `address`) |
| Bản phát hành | `-O2` (hoặc `-O3`), bỏ `-g` nếu không cần |
| Cảnh báo chuyển kiểu | `-Wconversion -Wshadow` (hữu ích nhưng khá "ồn") |

### B.2 Gỡ lỗi với `gdb`

| Lệnh | Tác dụng |
|---|---|
| `gdb ./main` | Mở chương trình |
| `break main` / `b file.c:42` | Đặt điểm dừng |
| `run` | Chạy |
| `next` / `step` | Chạy dòng tiếp theo (bỏ qua / đi vào hàm) |
| `print x` / `p *ptr` | Xem giá trị |
| `backtrace` / `bt` | Xem chuỗi lời gọi hàm (cực hữu ích khi crash) |
| `continue` | Chạy tiếp đến điểm dừng sau |

Trên Windows, trình gỡ lỗi của Visual Studio hoặc VS Code (với extension C/C++) làm được những việc tương tự qua giao diện.

### B.3 Công cụ khác

- **Valgrind** (Linux): `valgrind --leak-check=full ./main` — tìm rò rỉ bộ nhớ.
- **clang-format**: tự động định dạng code theo một phong cách thống nhất.
- **clang-tidy**: phân tích tĩnh, gợi ý viết code theo C++ hiện đại.
- **Compiler Explorer** (godbolt.org): xem code biên dịch ra hợp ngữ như thế nào, thử nhiều trình biên dịch ngay trên trình duyệt.

### B.4 CMake tối thiểu

Khi dự án có nhiều file, dùng CMake thay vì gõ lệnh `g++` thủ công:

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mygame src/main.cpp src/particle.cpp)
target_compile_options(mygame PRIVATE -Wall -Wextra)
```

```bash
cmake -S . -B build
cmake --build build
./build/mygame
```

---

## Phụ lục C. Lộ trình và tài liệu

### C.1 Lộ trình gợi ý (khoảng 8–10 tuần)

| Tuần | Nội dung | Mục tiêu tối thiểu |
|---|---|---|
| 1 | Chương 1–5 | Viết chương trình có hàm, vòng lặp, xử lý số |
| 2 | Chương 6–7 | **Tự tin với con trỏ**: vẽ được sơ đồ bộ nhớ cho mọi ví dụ |
| 3 | Chương 8–11 + Bài tập Phần I | Tự viết mảng động và danh sách liên kết không rò rỉ (kiểm tra bằng ASan) |
| 4 | Chương 12–13 | Viết lớp có đóng gói, nạp chồng toán tử |
| 5 | Chương 14–16 | Giải thích được Quy tắc 0/3/5; bỏ hẳn `new`/`delete` |
| 6 | Chương 17–18 | Thiết kế cây kế thừa nhỏ; viết template có concept |
| 7 | Chương 19–21 | Giải bài bằng STL + lambda thay vì vòng lặp thủ công |
| 8 | Chương 22–23 | Viết chương trình đa luồng sạch lỗi dưới ThreadSanitizer |
| 9–10 | Chương 24 + Bài tập Phần II + dự án riêng | Hoàn thành một dự án nhỏ có nhiều file, build bằng CMake |

### C.2 Tài liệu nên dùng song song

- **cppreference.com** — tài liệu tra cứu chuẩn cho cả C và C++. Nên mở thường trực.
- **learncpp.com** — giáo trình C++ miễn phí, rất chi tiết, phù hợp để học từng bước.
- **"The C Programming Language"** (Kernighan & Ritchie) — kinh điển về C, ngắn gọn.
- **"Modern C"** (Jens Gustedt) — C theo chuẩn hiện đại.
- **"A Tour of C++"** (Bjarne Stroustrup) — tổng quan C++ hiện đại từ chính tác giả ngôn ngữ.
- **"Effective Modern C++"** (Scott Meyers) — khi đã vững cơ bản, để hiểu sâu move, smart pointer, lambda.
- **C++ Core Guidelines** — bộ quy tắc viết C++ tốt do cộng đồng (dẫn dắt bởi Stroustrup và Sutter) duy trì.
- **CppCon — loạt bài "Back to Basics"** (YouTube) — các bài giảng nền tảng, dễ theo dõi.

### C.3 Bước tiếp theo theo hướng đi

- **Game:** thư viện **raylib** hoặc **SFML** để làm game 2D; sau đó **OpenGL** (trang LearnOpenGL) cho đồ họa 3D; sách **"Game Programming Patterns"** (Robert Nystrom, đọc miễn phí trên web) cho kiến trúc game.
- **Hệ thống / kỹ sư máy tính:** lập trình socket, gọi hệ thống (system call), cấu trúc dữ liệu hiệu năng cao, đo đạc hiệu năng (profiling).
- **AI:** phần lớn nghiên cứu dùng Python, nhưng các thư viện lõi (PyTorch, ONNX Runtime...) được viết bằng C++; nền tảng ở tài liệu này giúp bạn đọc hiểu và tối ưu phần đó khi cần.

---

*Chúc bạn học tốt! Hãy nhớ: đọc code không làm bạn giỏi lên — **gõ lại, sửa, làm hỏng và tự sửa** mới làm bạn giỏi lên.*
Trong C++, dấu `*` và `&` xuất hiện ở rất nhiều vị trí và **ý nghĩa thay đổi tùy ngữ cảnh**. Để học con trỏ và tham chiếu chắc chắn, bạn có thể nhớ theo nhóm sau.

# 1. `*` trong khai báo con trỏ

```cpp
int* p;
```

Nghĩa là:

> `p` là một con trỏ trỏ tới `int`.

Ví dụ:

```cpp
int x = 10;
int* p = &x;
```

Sơ đồ:

```text
x = 10
│
└── p ──> x
```

Có thể viết các kiểu sau:

```cpp
int* p;
int *p;
int * p;
```

Ba cách trên **hoàn toàn giống nhau**.

---

# 2. `&` trong khai báo tham chiếu

```cpp
int& ref = x;
```

Nghĩa là:

> `ref` là một tham chiếu đến `x`.

Ví dụ:

```cpp
int x = 10;
int& ref = x;

ref = 20;

cout << x; // 20
```

`ref` không phải một biến độc lập kiểu con trỏ; nó là một **bí danh (alias)** của `x`.

```text
x
↑
ref
```

---

# 3. `&` lấy địa chỉ của biến

Khi `&` đứng trước một biểu thức/biến:

```cpp
&x
```

nó có nghĩa:

> lấy địa chỉ của `x`.

Ví dụ:

```cpp
int x = 10;

cout << &x;
```

Kết quả sẽ là một địa chỉ bộ nhớ, chẳng hạn:

```text
0x7ff...
```

Đây chính là lý do ta thường thấy:

```cpp
int* p = &x;
```

Ở đây có **hai dấu khác ý nghĩa**:

```cpp
int* p = &x;
   ↑     ↑
 con trỏ  lấy địa chỉ
```

---

# 4. `*` giải tham chiếu (dereference)

Khi `*` đứng trước một **con trỏ**:

```cpp
*p
```

nó có nghĩa:

> truy cập vào giá trị tại địa chỉ mà `p` đang trỏ tới.

Ví dụ:

```cpp
int x = 10;
int* p = &x;

cout << *p;
```

Kết quả:

```text
10
```

Ta có:

```cpp
*p = 20;
```

thì:

```cpp
x == 20
```

Bởi vì `p` đang trỏ tới `x`.

---

# 5. Công thức cực kỳ quan trọng

Hãy nhớ 3 câu này:

```cpp
p      // địa chỉ đang chứa trong p
*p     // giá trị tại địa chỉ đó
&x     // địa chỉ của x
```

Ví dụ:

```cpp
int x = 100;
int* p = &x;
```

Ta có:

```cpp
x   = 100
&x  = địa chỉ của x
p   = địa chỉ của x
*p  = 100
```

Do đó:

```cpp
*p == x
```

và:

```cpp
p == &x
```

---

# 6. `*` trong tham số hàm

```cpp
void change(int* p)
{
    *p = 100;
}
```

Gọi:

```cpp
int x = 10;

change(&x);
```

Kết quả:

```cpp
x == 100
```

Ở đây:

```cpp
int* p
```

nghĩa là hàm nhận **một con trỏ**.

---

# 7. `&` trong tham số hàm — tham chiếu

```cpp
void change(int& x)
{
    x = 100;
}
```

Gọi:

```cpp
int a = 10;

change(a);
```

Kết quả:

```cpp
a == 100
```

So sánh:

```cpp
void change(int* p);
```

gọi:

```cpp
change(&a);
```

với:

```cpp
void change(int& x);
```

gọi:

```cpp
change(a);
```

Đây là một trong những điểm quan trọng nhất khi học C++.

---

# 8. Con trỏ tới con trỏ `**`

Bạn có thể có:

```cpp
int** pp;
```

Nghĩa là:

> `pp` là con trỏ tới một con trỏ `int`.

Ví dụ:

```cpp
int x = 10;

int* p = &x;
int** pp = &p;
```

Sơ đồ:

```text
pp
 │
 ▼
 p
 │
 ▼
 x = 10
```

Các giá trị:

```cpp
x      // 10
&x     // địa chỉ x

p      // địa chỉ x
*p     // 10
&p     // địa chỉ p

pp     // địa chỉ p
*pp    // p
**pp   // x = 10
```

---

# 9. Tham chiếu tới con trỏ

Đây là cú pháp rất hay gặp:

```cpp
int*& ref = p;
```

Nghĩa là:

> `ref` là tham chiếu tới một con trỏ `int`.

Ví dụ:

```cpp
int x = 10;
int y = 20;

int* p = &x;
int*& ref = p;

ref = &y;
```

Sau đó:

```cpp
cout << *p;
```

kết quả:

```text
20
```

Bởi vì `ref` chính là bí danh của `p`.

---

# 10. Con trỏ tới tham chiếu?

Bạn **không thể có con trỏ trực tiếp tới reference** theo kiểu:

```cpp
int&* p; // ❌ không hợp lệ
```

Reference không phải một object độc lập mà bạn có thể tạo pointer tới theo cách này.

Nhưng bạn có thể có:

```cpp
int*&
```

tức là:

> reference đến pointer.

---

# 11. Con trỏ hằng

```cpp
const int* p;
```

Hoặc:

```cpp
int const* p;
```

Nghĩa là:

> `p` có thể trỏ sang nơi khác, nhưng không được sửa giá trị thông qua `p`.

Ví dụ:

```cpp
int a = 10;
int b = 20;

const int* p = &a;

p = &b;      // ✅
*p = 30;     // ❌
```

---

# 12. Hằng con trỏ

```cpp
int* const p = &a;
```

Nghĩa:

> `p` không được trỏ sang địa chỉ khác, nhưng có thể sửa giá trị mà nó trỏ tới.

```cpp
int a = 10;
int b = 20;

int* const p = &a;

*p = 30;     // ✅
p = &b;      // ❌
```

---

# 13. Hằng con trỏ tới hằng

```cpp
const int* const p = &a;
```

Không thể:

```cpp
*p = 30;     // ❌
p = &b;      // ❌
```

---

# 14. `const` + reference

```cpp
const int& ref = x;
```

Nghĩa:

> `ref` tham chiếu tới `x` nhưng không được sửa `x` thông qua `ref`.

```cpp
int x = 10;

const int& ref = x;

cout << ref; // ✅

ref = 20;    // ❌
```

Một ứng dụng cực kỳ phổ biến:

```cpp
void print(const string& s)
{
    cout << s;
}
```

Không copy `string`, đồng thời không cho hàm sửa nó.

---

# 15. Reference to reference

Bạn có thể gặp:

```cpp
int&&
```

Đây là **rvalue reference**, không phải "reference tới reference".

Ví dụ:

```cpp
int&& x = 10;
```

Nó liên quan đến **rvalue**, move semantics và perfect forwarding.

---

# 16. `&&` trong C++

Dấu `&&` có nhiều ý nghĩa tùy ngữ cảnh.

### Toán tử logic

```cpp
if (a > 0 && b > 0)
{
}
```

Nghĩa:

```text
AND
```

### Rvalue reference

```cpp
int&& ref = 10;
```

Ở đây:

```cpp
&&
```

là rvalue reference.

---

# 17. `*` trong khai báo hàm

Ví dụ:

```cpp
int* getPointer();
```

Nghĩa là:

> Hàm trả về `int*`.

Ví dụ:

```cpp
int* getPointer()
{
    static int x = 10;
    return &x;
}
```

---

# 18. Function pointer

Đây là cú pháp `*` khá khó lúc mới học:

```cpp
int (*fp)(int, int);
```

Nghĩa là:

> `fp` là con trỏ tới một hàm nhận 2 `int` và trả về `int`.

Ví dụ:

```cpp
int add(int a, int b)
{
    return a + b;
}

int (*fp)(int, int) = add;

cout << fp(2, 3);
```

Kết quả:

```text
5
```

---

# 19. Pointer to array

Ví dụ:

```cpp
int (*p)[5];
```

Nghĩa:

> `p` là con trỏ tới một mảng gồm 5 `int`.

Trong khi:

```cpp
int* p[5];
```

lại nghĩa:

> `p` là một mảng gồm 5 con trỏ `int`.

Hai cú pháp này **hoàn toàn khác nhau**.

```cpp
int (*p)[5]; // pointer -> array
int* p[5];   // array -> pointers
```

---

# 20. Array of pointers

```cpp
int* arr[5];
```

Ví dụ:

```cpp
int a = 10;
int b = 20;

int* arr[2];

arr[0] = &a;
arr[1] = &b;
```

---

# 21. Pointer to pointer

```cpp
int** p;
```

Pointer tới pointer.

Có thể tiếp tục:

```cpp
int***
int****
int*****
```

Mỗi `*` thêm một tầng con trỏ.

Ví dụ:

```cpp
int x = 10;

int* p = &x;
int** pp = &p;
int*** ppp = &pp;
```

Ta có:

```cpp
***ppp
```

để lấy `x`.

---

# 22. `*` trong biểu thức nhân

Không phải lúc nào `*` cũng liên quan đến con trỏ.

```cpp
int a = 3;
int b = 4;

int c = a * b;
```

Ở đây:

```cpp
*
```

là phép nhân.

---

# 23. `&` trong phép AND bit

Tương tự, `&` cũng có thể là toán tử bitwise AND:

```cpp
int a = 6;
int b = 3;

int c = a & b;
```

Ở đây `&` **không phải reference và cũng không phải lấy địa chỉ**.

---

# 24. `*&`

Bạn có thể gặp:

```cpp
*&x
```

Hai toán tử thực hiện ngược nhau:

```cpp
*&x
```

hiểu là:

```text
x
→ &x : lấy địa chỉ
→ *  : truy cập địa chỉ đó
→ x
```

Do đó:

```cpp
*&x == x
```

---

# 25. `&*`

Tương tự:

```cpp
&*p
```

Nếu `p` là con trỏ hợp lệ:

```text
p
→ *p : lấy object mà p trỏ tới
→ &  : lấy địa chỉ object đó
→ p
```

Nên về mặt ý nghĩa:

```cpp
&*p == p
```

với điều kiện `p` hợp lệ.

---

# 26. `***p`, `**p`, `*p`

Ví dụ:

```cpp
int x = 10;

int* p = &x;
int** pp = &p;
int*** ppp = &pp;
```

Ta có:

```cpp
*p        // 10
*pp       // p
**pp      // 10
*ppp      // pp
**ppp     // p
***ppp    // 10
```

Cách nhớ:

```text
mỗi * = đi xuống 1 tầng
```

---

# 27. `&` trong tham chiếu hàm

```cpp
void foo(int& x)
```

`&` ở đây **không lấy địa chỉ**.

Nó biểu thị:

```text
x là reference
```

Trong khi:

```cpp
foo(&x);
```

`&` ở đây lại có nghĩa:

```text
lấy địa chỉ của x
```

Đây là lỗi người mới học rất dễ nhầm.

---

# 28. `*` và `&` trong một khai báo phức tạp

Ví dụ:

```cpp
int*& p;
```

Đọc từ tên biến ra ngoài:

```text
p
→ &
→ *
→ int
```

Nên:

```cpp
int*& p;
```

là:

> `p` là reference tới pointer tới `int`.

---

Ví dụ:

```cpp
int** p;
```

Đọc:

```text
p
→ *
→ *
→ int
```

= pointer → pointer → int.

---

Ví dụ:

```cpp
const int*& p;
```

= reference tới pointer tới const int.

---

# 29. Bảng tổng hợp các cú pháp quan trọng

| Cú pháp              | Ý nghĩa                     |
| -------------------- | --------------------------- |
| `int* p`             | `p` là pointer tới `int`    |
| `int& p`             | `p` là reference tới `int`  |
| `&x`                 | lấy địa chỉ `x`             |
| `*p`                 | dereference `p`             |
| `int** p`            | pointer tới pointer         |
| `int*& p`            | reference tới pointer       |
| `const int* p`       | pointer tới const int       |
| `int* const p`       | const pointer               |
| `const int* const p` | const pointer tới const int |
| `const int& p`       | const reference             |
| `int&& p`            | rvalue reference            |
| `int (*p)()`         | function pointer            |
| `int (*p)[5]`        | pointer tới mảng 5 `int`    |
| `int* p[5]`          | mảng 5 pointer              |
| `a * b`              | phép nhân                   |
| `a & b`              | bitwise AND                 |
| `a && b`             | logical AND                 |
| `*&x`                | lấy địa chỉ rồi dereference |
| `&*p`                | dereference rồi lấy địa chỉ |

---

# 30. Cách đọc mọi khai báo có `*` và `&`

Một kỹ thuật rất hữu ích:

> **Bắt đầu từ tên biến, đọc ra ngoài.**

Ví dụ:

```cpp
int*& p;
```

Bắt đầu từ `p`:

```text
p
↓
&
↓
*
↓
int
```

=> `p` là **reference tới pointer tới int**.

Ví dụ:

```cpp
int** pp;
```

```text
pp
↓
*
↓
*
↓
int
```

=> `pp` là **pointer tới pointer tới int**.

Ví dụ:

```cpp
int (*fp)(int);
```

Bắt đầu từ `fp`:

```text
fp
 ↓
(*fp)   → pointer
 ↓
(int)   → function nhận int
 ↓
int     → trả về int
```

=> `fp` là **function pointer**.

---

# 31. Hãy nhớ 4 thứ này trước tiên

Nếu bạn mới học con trỏ, chưa cần cố nhớ toàn bộ ngay. Trước tiên phải thành thạo:

```cpp
int x = 10;

int* p = &x;

x       // giá trị
&x      // địa chỉ
p       // địa chỉ
*p      // giá trị
```

Có thể xem như công thức:

```text
           & lấy địa chỉ
x  --------------------------> địa chỉ

x  <-------------------------- * 
           dereference
```

Và:

```cpp
int& ref = x;
```

thì:

```text
x  <-----> ref
```

`ref` là một tên khác của `x`.

Sau khi nắm chắc 4 biểu thức:

```cpp
x
&x
p
*p
```

thì `**`, `***`, `*&`, `&*`, `int*&`, `const int*`, `int* const` sẽ dễ hơn rất nhiều.
