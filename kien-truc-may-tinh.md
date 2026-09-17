# KIẾN TRÚC MÁY TÍNH — NỀN TẢNG VỮNG CHẮC

*Tài liệu tự học: từ bit và cổng logic đến CPU, bộ nhớ đệm và bộ nhớ ảo — hiểu máy tính thật sự chạy code của bạn như thế nào*

---

## 0. Cách dùng tài liệu này

### 0.1 Cấu trúc

| Phần | Chương | Nội dung |
|---|---|---|
| **I. Biểu diễn dữ liệu** | 1–4 | Hệ đếm, số nguyên và bù 2, số thực IEEE 754, ký tự và thứ tự byte |
| **II. Mạch số** | 5–7 | Đại số Boole, cổng logic, mạch cộng, bộ chọn kênh, flip-flop, thanh ghi |
| **III. Bộ xử lý** | 8–13 | Kiến trúc von Neumann, tập lệnh, **hợp ngữ RISC-V và x86-64**, hiệu năng, **pipeline**, kỹ thuật CPU hiện đại |
| **IV. Bộ nhớ** | 14–16 | Phân cấp bộ nhớ, **cache**, **bộ nhớ ảo** |
| **V. Hệ thống** | 17–19 | Vào/ra và lưu trữ, xử lý song song và GPU, chuyện gì xảy ra khi chạy một chương trình |
| **Phụ lục** | A–C | Công thức, hiểu lầm phổ biến, lộ trình và tài liệu |

Mỗi chương đi theo mạch: **ý tưởng → cơ chế → ví dụ cụ thể → liên hệ với code bạn viết → tự kiểm tra**. Cuối mỗi phần có **bài tập kèm đáp án**.

> ✅ **Về độ chính xác:**
> - Mọi phép tính (đổi hệ đếm, bù 2, IEEE 754, hiệu năng, pipeline, mô phỏng cache, dịch địa chỉ) đã được kiểm tra bằng Python.
> - Mã máy RISC-V trong tài liệu được **dịch ngược lại bằng công cụ capstone** để đối chiếu, và các chương trình hợp ngữ được **chạy thật trên trình giả lập unicorn**.
> - Các chương trình C đã được biên dịch bằng GCC 13 và chạy trên Linux x86-64; mã hợp ngữ x86-64 là output thật của GCC.
> - Riêng các số đo **thời gian** và **địa chỉ bộ nhớ** được ghi rõ là "ví dụ": trên máy bạn, con số sẽ khác nhưng xu hướng sẽ giống.

### 0.2 Vì sao lập trình viên cần học kiến trúc máy tính?

- Hiểu vì sao `0.1 + 0.2 != 0.3`, vì sao số nguyên bị tràn, vì sao `int` có giới hạn.
- Hiểu vì sao **duyệt mảng 2 chiều theo hàng** có thể nhanh hơn **gấp 10 lần** so với theo cột (Chương 15).
- Đọc được hợp ngữ khi gỡ lỗi, hiểu trình biên dịch đã làm gì với code của bạn.
- Viết được code tận dụng cache, SIMD, đa lõi — kỹ năng cốt lõi của lập trình game engine, cơ sở dữ liệu, AI hiệu năng cao.

---

# PHẦN I — BIỂU DIỄN DỮ LIỆU

## Chương 1. Hệ đếm và đơn vị

### 1.1 Ba hệ đếm của lập trình viên

| Hệ | Cơ số | Chữ số | Tiền tố trong C/C++ | Ví dụ (= 181) |
|---|---|---|---|---|
| Thập phân | 10 | 0–9 | (không) | `181` |
| Nhị phân | 2 | 0, 1 | `0b` (C23, C++14) | `0b10110101` |
| Thập lục phân (hex) | 16 | 0–9, A–F | `0x` | `0xB5` |
| Bát phân | 8 | 0–7 | `0` | `0265` |

> ⚠️ Trong C/C++, số bắt đầu bằng `0` là **bát phân**: `int x = 010;` cho `x = 8`, không phải 10.

**Giá trị của một số trong hệ cơ số $b$:**

$$d_{n-1} d_{n-2} \dots d_1 d_0 = d_{n-1} \cdot b^{n-1} + \dots + d_1 \cdot b + d_0$$

Ví dụ: $10110101_2 = 128 + 32 + 16 + 4 + 1 = 181$.

### 1.2 Chuyển đổi nhanh

**Thập phân → nhị phân:** chia liên tiếp cho 2, đọc số dư **từ dưới lên**:

```text
181 : 2 = 90 dư 1   ▲
 90 : 2 = 45 dư 0   │
 45 : 2 = 22 dư 1   │
 22 : 2 = 11 dư 0   │  đọc từ dưới lên
 11 : 2 =  5 dư 1   │
  5 : 2 =  2 dư 1   │
  2 : 2 =  1 dư 0   │
  1 : 2 =  0 dư 1   │
→ 181 = 10110101₂
```

**Nhị phân ↔ hex:** mỗi chữ số hex tương ứng **đúng 4 bit** — chỉ cần nhóm từ phải sang trái:

```text
1101 0111 1010₂
  D    7    A     → 0xD7A
```

| Hex | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Nhị phân | 0000 | 0001 | 0010 | 0011 | 0100 | 0101 | 0110 | 0111 | 1000 | 1001 | 1010 | 1011 | 1100 | 1101 | 1110 | 1111 |

Đây là lý do lập trình viên dùng hex: **gọn** hơn nhị phân nhưng vẫn **nhìn thấy được từng bit**. Một byte luôn là đúng 2 chữ số hex (`0x00` → `0xFF`).

### 1.3 Đơn vị dung lượng

| Ký hiệu | Giá trị |
|---|---|
| 1 **byte** | 8 bit |
| 1 **KB** (kilobyte, chuẩn SI) | 1 000 byte |
| 1 **KiB** (kibibyte, chuẩn IEC) | $2^{10}$ = 1 024 byte |
| 1 **MiB** | $2^{20}$ = 1 048 576 byte |
| 1 **GiB** | $2^{30}$ byte ≈ 1,07 × 10⁹ byte |

Trong kiến trúc máy tính (RAM, cache, bộ nhớ ảo), "KB, MB, GB" gần như luôn mang nghĩa **lũy thừa của 2**. Nhà sản xuất ổ cứng thì dùng lũy thừa của 10 — vì vậy ổ "1 TB" chỉ hiện khoảng 931 GiB trong hệ điều hành.

**Lũy thừa của 2 cần thuộc:** $2^{10} \approx 10^3$, $2^{16} = 65\,536$, $2^{20} \approx 10^6$, $2^{32} \approx 4{,}29 \times 10^9$.

**Mẹo:** $n$ bit biểu diễn được $2^n$ giá trị khác nhau; để đánh địa chỉ cho $N$ ô nhớ cần $\lceil \log_2 N \rceil$ bit.

---

## Chương 2. Số nguyên

### 2.1 Số không dấu (unsigned)

$n$ bit biểu diễn các số từ $0$ đến $2^n - 1$. Ví dụ 8 bit: 0 → 255.

Phép cộng vượt quá giới hạn sẽ **quay vòng** (lấy phần dư khi chia cho $2^n$): $250 + 10 = 260 \bmod 256 = 4$.

### 2.2 Số có dấu: bù 2 (two's complement)

Mọi CPU hiện đại biểu diễn số nguyên có dấu bằng **bù 2**: bit cao nhất mang **trọng số âm**.

$$\text{giá trị} = -b_{n-1} \cdot 2^{n-1} + b_{n-2} \cdot 2^{n-2} + \dots + b_0$$

| 8 bit | Giá trị |
|---|---|
| `0111 1111` | $+127$ (lớn nhất) |
| `0000 0001` | $+1$ |
| `0000 0000` | $0$ |
| `1111 1111` | $-1$ |
| `1111 1011` | $-5$ |
| `1000 0000` | $-128$ (nhỏ nhất) |

**Khoảng giá trị với $n$ bit:** $-2^{n-1}$ đến $2^{n-1} - 1$. Với `int32_t`: $-2\,147\,483\,648$ đến $2\,147\,483\,647$.

**Cách tìm số đối ($-x$):** **đảo tất cả các bit rồi cộng 1**.

```text
 +5  = 0000 0101
đảo  = 1111 1010
 +1  = 1111 1011   → đây là −5
```

**Vì sao dùng bù 2?**

1. Chỉ có **một** số 0 (các cách biểu diễn cũ có cả +0 và −0).
2. Phép **cộng** dùng **cùng một mạch** cho số có dấu và không dấu. Phép **trừ** chỉ là cộng với số đối: $a - b = a + (\sim b + 1)$.

**Ví dụ 2.1.** Tính $13 - 6$ trong 8 bit.

```text
   13 = 0000 1101
 + −6 = 1111 1010
 ────────────────
      1 0000 0111   → bỏ bit nhớ thứ 9 → 0000 0111 = 7 ✓
```

### 2.3 Cờ nhớ (C) và cờ tràn (V)

Sau mỗi phép cộng, CPU bật các **cờ trạng thái**:

| Cờ | Bật khi | Có ý nghĩa với |
|---|---|---|
| **C** (Carry) | Có nhớ ra khỏi bit cao nhất | Số **không dấu** — kết quả sai |
| **V** (Overflow) | Cộng hai số **cùng dấu** mà kết quả **khác dấu** | Số **có dấu** — kết quả sai |
| **Z** (Zero) | Kết quả bằng 0 | Cả hai |
| **N** (Negative) | Bit cao nhất của kết quả bằng 1 | Số có dấu |

Cùng một dãy bit, cùng một phép cộng — nhưng **đúng hay sai tùy vào cách bạn hiểu** dãy bit đó. Chương 6 sẽ dựng một bộ cộng từ cổng logic và in ra cả hai cờ này.

### 2.4 Mở rộng dấu và phép dịch bit

**Mở rộng dấu (sign extension):** khi chuyển số có dấu sang kiểu rộng hơn, **chép bit dấu** vào các bit mới:

```text
int8_t  −5 =           1111 1011
int16_t −5 = 1111 1111 1111 1011   (0xFFFB)   ← chép bit 1 sang trái
```

**Dịch bit:**

| Phép | Tác dụng | Ví dụ |
|---|---|---|
| `x << k` | Nhân với $2^k$ | `20 << 3 = 160` |
| Dịch phải **logic** (số không dấu) | Chia nguyên cho $2^k$, chèn 0 vào bên trái | |
| Dịch phải **số học** (số có dấu) | Chia cho $2^k$ (làm tròn xuống), chèn **bit dấu** vào bên trái | `-20 >> 2 = -5` |

Trình biên dịch thường thay phép nhân/chia cho hằng số lũy thừa 2 bằng phép dịch vì dịch rất rẻ.

### 2.5 Chương trình minh họa

```c
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// In từng byte của một vùng nhớ, theo đúng thứ tự trong RAM
static void dump(const char *label, const void *p, size_t n) {
    const unsigned char *b = p;
    printf("%-26s", label);
    for (size_t i = 0; i < n; i++) printf("%02x ", b[i]);
    printf("\n");
}

int main(void) {
    // 1) Thứ tự byte (endianness)
    uint32_t x = 0x12345678;
    dump("0x12345678 trong RAM:", &x, sizeof x);

    // 2) Số âm dạng bù 2
    int8_t neg = -5;
    int32_t neg32 = -5;
    dump("int8_t  -5:", &neg, sizeof neg);
    dump("int32_t -5:", &neg32, sizeof neg32);

    // 3) Tràn số không dấu: quay vòng
    uint8_t u = 250;
    u = (uint8_t)(u + 10);
    printf("uint8_t 250 + 10 = %u\n", u);

    // 4) Bit của số thực float (IEEE 754)
    float f = -6.75f;
    uint32_t bits;
    memcpy(&bits, &f, sizeof bits);           // cách hợp lệ để "nhìn" bit của float
    printf("-6.75f  = 0x%08" PRIX32 "\n", bits);

    f = 0.1f;
    memcpy(&bits, &f, sizeof bits);
    printf("0.1f    = 0x%08" PRIX32 " (thuc te luu %.17g)\n", bits, (double)f);

    // 5) 0.1 + 0.2 có bằng 0.3?
    double s = 0.1 + 0.2;
    printf("0.1 + 0.2 = %.17g, == 0.3? %s\n", s, s == 0.3 ? "co" : "khong");

    // 6) float không biểu diễn được mọi số nguyên lớn
    float big = 16777217.0f;                  // 2^24 + 1
    printf("16777217.0f luu thanh %.1f\n", (double)big);
    return 0;
}
```

**Kết quả:**

```text
0x12345678 trong RAM:     78 56 34 12
int8_t  -5:               fb
int32_t -5:               fb ff ff ff
uint8_t 250 + 10 = 4
-6.75f  = 0xC0D80000
0.1f    = 0x3DCCCCCD (thuc te luu 0.10000000149011612)
0.1 + 0.2 = 0.30000000000000004, == 0.3? khong
16777217.0f luu thanh 16777216.0
```

Chương trình này minh họa cho cả Chương 2, 3 và 4. Các dòng về `-5` cho thấy số âm được lưu dạng bù 2 (`fb` = `1111 1011`), và `int32_t -5` chính là `int8_t -5` được mở rộng dấu.

> ⚠️ **Trong C/C++, tràn số nguyên có dấu là hành vi không xác định (UB)**, dù phần cứng chỉ đơn giản là quay vòng. Trình biên dịch được phép giả định điều đó không bao giờ xảy ra khi tối ưu hóa — vì vậy đừng bao giờ dựa vào nó.

---

## Chương 3. Số thực dấu phẩy động (IEEE 754)

### 3.1 Ý tưởng: ký hiệu khoa học nhị phân

Giống như viết $-6{,}75 = -6{,}75 \times 10^0$ trong hệ thập phân, máy tính viết số thực dưới dạng:

$$x = (-1)^s \times 1{,}f \times 2^{e}$$

| Kiểu | Tổng số bit | Dấu $s$ | Số mũ (exponent) | Phần định trị (fraction $f$) | Độ lệch (bias) | Độ chính xác |
|---|---|---|---|---|---|---|
| `float` | 32 | 1 | 8 | 23 | 127 | ~7 chữ số thập phân |
| `double` | 64 | 1 | 11 | 52 | 1023 | ~15–16 chữ số thập phân |

Số mũ được lưu dưới dạng **có độ lệch**: giá trị lưu = $e + \text{bias}$. Nhờ vậy không cần bit dấu riêng cho số mũ.

### 3.2 Ví dụ: đổi −6,75 sang `float`

**Bước 1 — Đổi sang nhị phân:** $6 = 110_2$, $0{,}75 = 0{,}5 + 0{,}25 = 0{,}11_2$. Vậy $6{,}75 = 110{,}11_2$.

**Bước 2 — Chuẩn hóa về dạng $1{,}\dots$:** $110{,}11_2 = 1{,}1011_2 \times 2^2$.

**Bước 3 — Điền các trường:**

- Dấu: âm → $s = 1$
- Số mũ: $2 + 127 = 129 = 10000001_2$
- Phần định trị: bỏ số `1,` ở đầu, lấy phần sau dấu phẩy: `1011` rồi thêm 0 cho đủ 23 bit

```text
 s  số mũ (8)   phần định trị (23)
 1  10000001    10110000000000000000000

 1100 0000 1101 1000 0000 0000 0000 0000  =  0xC0D80000
```

Khớp với output của chương trình ở Chương 2.5 ✓.

**Chiều ngược lại** — giải mã `0x41200000`:

```text
0100 0001 0010 0000 ... → s = 0, số mũ = 10000010₂ = 130 → e = 3, f = 01₂
x = +1,01₂ × 2³ = 1010₂ = 10,0
```

### 3.3 Các giá trị đặc biệt

| Số mũ | Phần định trị | Ý nghĩa |
|---|---|---|
| Toàn 0 | Toàn 0 | $\pm 0$ (có cả +0 và −0!) |
| Toàn 0 | Khác 0 | Số rất nhỏ gần 0 (subnormal) |
| Toàn 1 | Toàn 0 | $\pm\infty$ — ví dụ `1.0 / 0.0` |
| Toàn 1 | Khác 0 | **NaN** (Not a Number) — ví dụ `0.0 / 0.0`, `sqrt(-1)` |

**NaN khác mọi thứ, kể cả chính nó:** `x != x` là đúng khi `x` là NaN. Dùng `isnan(x)` để kiểm tra.

### 3.4 Những hệ quả mà lập trình viên phải biết

**1. Nhiều số thập phân "đơn giản" không biểu diễn chính xác được.** $0{,}1 = 0{,}000110011001100\dots_2$ (lặp vô hạn), nên bị làm tròn. Đó là lý do `0.1 + 0.2` ra `0.30000000000000004`.

→ **Không so sánh số thực bằng `==`**. Dùng sai số cho phép:

```c
if (fabs(a - b) < 1e-9) { /* coi như bằng nhau */ }
```

→ **Không dùng số thực cho tiền tệ.** Dùng số nguyên (đơn vị nhỏ nhất, ví dụ đồng hoặc xu) hoặc kiểu thập phân chuyên dụng.

**2. Khoảng cách giữa hai số liền kề tăng dần khi số lớn lên.** Với `float`:

| Giá trị | Khoảng cách đến số `float` kế tiếp |
|---|---|
| 1,0 | ≈ 0,00000012 |
| 100 000 | 0,0078125 (≈ 7,8 mm nếu đơn vị là mét) |
| 1 000 000 | 0,0625 (6,25 cm) |
| 10 000 000 | 1,0 |
| 16 777 216 ($2^{24}$) | 2,0 → `16777217.0f` bị làm tròn thành `16777216` |

> 🎮 **Liên hệ game:** Engine thường lưu vị trí bằng `float`. Trong thế giới mở rộng lớn, khi nhân vật ở cách gốc tọa độ khoảng 100 km, vị trí chỉ chính xác đến vài mm — đủ để camera và vật lý bị **rung giật**. Giải pháp phổ biến: **dời gốc tọa độ theo người chơi (floating origin)**, hoặc dùng `double` cho vị trí thế giới.

**3. Phép toán số thực không có tính kết hợp:** `(a + b) + c` có thể khác `a + (b + c)`. Trình biên dịch vì vậy **không** tự sắp xếp lại phép cộng số thực, trừ khi bạn cho phép bằng cờ như `-ffast-math`.

---

## Chương 4. Ký tự và thứ tự byte

### 4.1 ASCII và Unicode

**ASCII** dùng 7 bit cho 128 ký tự: chữ cái Latin không dấu, chữ số, dấu câu, ký tự điều khiển.

| Ký tự | Mã | Mẹo |
|---|---|---|
| `'0'` → `'9'` | 48 → 57 | `c - '0'` đổi ký tự số thành giá trị |
| `'A'` → `'Z'` | 65 → 90 | |
| `'a'` → `'z'` | 97 → 122 | Chữ thường = chữ hoa + 32 (khác nhau đúng 1 bit: `0x20`) |
| `'\n'` | 10 | Xuống dòng |
| `'\0'` | 0 | Kết thúc chuỗi C |

**Unicode** gán cho mỗi ký tự của mọi ngôn ngữ một **mã (code point)**, viết dạng `U+XXXX`. Ví dụ: `ệ` = `U+1EC7`, 😀 = `U+1F600`.

### 4.2 UTF-8 — cách lưu Unicode phổ biến nhất

UTF-8 dùng **1 đến 4 byte** cho mỗi ký tự, và **tương thích hoàn toàn với ASCII**:

| Khoảng code point | Số byte | Mẫu bit |
|---|---|---|
| U+0000 – U+007F | 1 | `0xxxxxxx` |
| U+0080 – U+07FF | 2 | `110xxxxx 10xxxxxx` |
| U+0800 – U+FFFF | 3 | `1110xxxx 10xxxxxx 10xxxxxx` |
| U+10000 – U+10FFFF | 4 | `11110xxx 10xxxxxx 10xxxxxx 10xxxxxx` |

**Ví dụ 4.1.** Chuỗi `"Việt"` trong UTF-8:

| Ký tự | Code point | Byte UTF-8 |
|---|---|---|
| V | U+0056 | `56` |
| i | U+0069 | `69` |
| ệ | U+1EC7 | `E1 BB 87` |
| t | U+0074 | `74` |

Tổng cộng **6 byte** cho 4 ký tự. Vì vậy `strlen("Việt")` trả về **6**, không phải 4 — một nguồn lỗi phổ biến khi xử lý tiếng Việt trong C/C++.

### 4.3 Thứ tự byte (endianness)

Một số nguyên nhiều byte, ví dụ `uint32_t x = 0x12345678`, được xếp vào bộ nhớ theo một trong hai cách:

```text
Địa chỉ:              0x100  0x101  0x102  0x103
Little-endian (x86, hầu hết ARM, RISC-V):  78     56     34     12    ← byte THẤP trước
Big-endian (thứ tự mạng, một số CPU cũ):   12     34     56     78    ← byte CAO trước
```

- Chương trình ở Chương 2.5 in ra `78 56 34 12` → máy đang chạy là **little-endian**.
- Thứ tự byte chỉ quan trọng khi **dữ liệu rời khỏi bộ nhớ của chương trình**: ghi file nhị phân, gửi qua mạng (mạng dùng big-endian — xem `htons`/`htonl`), hoặc đọc dữ liệu từ thiết bị khác.
- Nó **không** ảnh hưởng đến phép toán hay phép dịch bit trong chương trình: `x >> 24` luôn cho `0x12`.

---

## Bài tập Phần I

**Bài I.1.** Đổi 237 sang nhị phân và hex. Đổi `0x2F` sang thập phân.

**Bài I.2.** Biểu diễn −37 dưới dạng bù 2 với 8 bit (nhị phân và hex). Kiểu `int16_t` biểu diễn được khoảng giá trị nào?

**Bài I.3.** Tính `0x7F + 0x01` với 8 bit. Cho biết giá trị của cờ C và V, và giải thích kết quả khi hiểu là số có dấu.

**Bài I.4.** (a) Biểu diễn 0,15625 dưới dạng `float` (hex). (b) Giải mã `float` có giá trị `0xC1480000`.

**Bài I.5.** Biến `uint16_t v = 0xABCD` nằm tại địa chỉ `0x100` trên máy little-endian. Byte nào nằm ở `0x100`, byte nào ở `0x101`?

**Bài I.6.** Vì sao không nên viết `for (float x = 0.0f; x != 1.0f; x += 0.1f)`?

<details>
<summary><b>Đáp án Phần I</b> (bấm để mở)</summary>

**I.1.** $237 = 11101101_2 = \texttt{0xED}$. $\texttt{0x2F} = 2 \times 16 + 15 = 47$.

**I.2.** $37 = 00100101_2$ → đảo bit: $11011010$ → cộng 1: **$11011011_2$ = `0xDB`**. `int16_t`: từ $-32\,768$ đến $32\,767$.

**I.3.** `0x7F + 0x01 = 0x80`. Không có nhớ ra ngoài → **C = 0** (kết quả không dấu $127 + 1 = 128$ là đúng). Hai số dương cộng lại ra bit dấu 1 → **V = 1**: theo nghĩa có dấu, $127 + 1$ bị "quay" thành $-128$.

**I.4.** (a) $0{,}15625 = 0{,}00101_2 = 1{,}01_2 \times 2^{-3}$ → $s = 0$, số mũ $= -3 + 127 = 124 = 01111100_2$, $f = 01000\dots$ → `0011 1110 0010 0000 ...` = **`0x3E200000`**.
(b) `0xC1480000` = `1 10000010 1001000...` → $s = 1$, $e = 130 - 127 = 3$, giá trị $= -1{,}1001_2 \times 2^3 = -1100{,}1_2$ = **−12,5**.

**I.5.** Little-endian: byte thấp trước → `0x100` chứa **`CD`**, `0x101` chứa **`AB`**.

**I.6.** 0,1 không biểu diễn chính xác trong `float`; sau 10 lần cộng, `x` có thể không bao giờ **bằng đúng** 1,0 → vòng lặp vô hạn. Nên dùng biến đếm nguyên (`for (int i = 0; i < 10; i++) { float x = i * 0.1f; ... }`) hoặc so sánh bằng `<`.

</details>

---

# PHẦN II — MẠCH SỐ

## Chương 5. Đại số Boole và cổng logic

### 5.1 Các cổng cơ bản

Mọi thứ trong máy tính được xây từ **cổng logic** — mạch điện tử nhận vào các bit và cho ra một bit. Mỗi cổng được làm từ vài **transistor**, hoạt động như công tắc điều khiển bằng điện.

| Cổng | Ký hiệu | Biểu thức | Ra 1 khi | Toán tử trong C |
|---|---|---|---|---|
| **NOT** | $\overline{A}$ | $Y = \overline{A}$ | $A = 0$ | `~a` (bit), `!a` (logic) |
| **AND** | $A \cdot B$ | $Y = AB$ | Cả hai bằng 1 | `a & b` |
| **OR** | $A + B$ | $Y = A + B$ | Ít nhất một bằng 1 | `a \| b` |
| **XOR** | $A \oplus B$ | $Y = A \oplus B$ | Hai đầu vào **khác nhau** | `a ^ b` |
| **NAND** | $\overline{AB}$ | | Không phải cả hai bằng 1 | `~(a & b)` |
| **NOR** | $\overline{A + B}$ | | Cả hai bằng 0 | `~(a \| b)` |

**Bảng chân trị:**

| A | B | AND | OR | XOR | NAND | NOR |
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| 0 | 0 | 0 | 0 | 0 | 1 | 1 |
| 0 | 1 | 0 | 1 | 1 | 1 | 0 |
| 1 | 0 | 0 | 1 | 1 | 1 | 0 |
| 1 | 1 | 1 | 1 | 0 | 0 | 0 |

### 5.2 Các luật cần nhớ

| Luật | Dạng AND | Dạng OR |
|---|---|---|
| Đồng nhất | $A \cdot 1 = A$ | $A + 0 = A$ |
| Triệt tiêu | $A \cdot 0 = 0$ | $A + 1 = 1$ |
| Lũy đẳng | $A \cdot A = A$ | $A + A = A$ |
| Bù | $A \cdot \overline{A} = 0$ | $A + \overline{A} = 1$ |
| Phân phối | $A(B + C) = AB + AC$ | $A + BC = (A + B)(A + C)$ |
| Hấp thụ | $A(A + B) = A$ | $A + AB = A$ |
| **De Morgan** | $\overline{AB} = \overline{A} + \overline{B}$ | $\overline{A + B} = \overline{A} \cdot \overline{B}$ |

**Ví dụ 5.1.** Rút gọn $F = AB + A\overline{B}$.

$$F = A(B + \overline{B}) = A \cdot 1 = A$$

### 5.3 NAND là cổng "vạn năng"

Chỉ với cổng NAND, ta dựng được **mọi** mạch logic:

```text
NOT A   = A NAND A                         (1 cổng)
A AND B = NOT (A NAND B)                   (2 cổng)
A OR B  = (NOT A) NAND (NOT B)             (3 cổng — theo De Morgan)
```

Trong chế tạo chip, NAND và NOR rất rẻ về số transistor. Dự án **Nand2Tetris** (xem Phụ lục C) xây dựng cả một máy tính hoàn chỉnh chỉ từ cổng NAND.

### 5.4 Từ bảng chân trị đến mạch — và bìa Karnaugh

**Tổng các tích (SOP):** với mỗi dòng mà đầu ra bằng 1, viết một tích (AND) của các biến (biến bằng 0 thì lấy bù), rồi cộng (OR) tất cả lại.

**Ví dụ 5.2.** Hàm $F(A, B, C)$ bằng 1 tại các dòng (minterm) 1, 3, 5, 6, 7. Rút gọn bằng **bìa Karnaugh**:

```text
              BC
          00   01   11   10
       ┌────┬────┬────┬────┐
  A  0 │ 0  │ 1  │ 1  │ 0  │
       ├────┼────┼────┼────┤
     1 │ 0  │ 1  │ 1  │ 1  │
       └────┴────┴────┴────┘
             └──C──┘   (A·B: ô 11 và 10 ở hàng A=1)
```

Quy tắc: khoanh các nhóm ô **bằng 1** kề nhau, kích thước là **lũy thừa của 2** (1, 2, 4, 8...), càng lớn càng tốt. Cột `BC` được sắp theo **mã Gray** (00, 01, 11, 10 — hai ô kề nhau chỉ khác 1 bit).

- Nhóm 4 ô ở hai cột giữa (C = 1): cho số hạng $C$.
- Nhóm 2 ô ở hàng A = 1, cột 11 và 10 (A = 1, B = 1): cho số hạng $AB$.

$$F = C + AB$$

---

## Chương 6. Mạch tổ hợp

**Mạch tổ hợp** là mạch mà đầu ra **chỉ phụ thuộc vào đầu vào hiện tại** (không có "trí nhớ").

### 6.1 Bộ cộng bán phần và toàn phần

**Bộ cộng bán phần (half adder)** cộng 2 bit:

| A | B | Tổng $S$ | Nhớ $C$ |
|:-:|:-:|:-:|:-:|
| 0 | 0 | 0 | 0 |
| 0 | 1 | 1 | 0 |
| 1 | 0 | 1 | 0 |
| 1 | 1 | 0 | 1 |

$$S = A \oplus B, \qquad C = AB$$

**Bộ cộng toàn phần (full adder)** cộng 3 bit (thêm bit nhớ từ cột trước $C_{in}$):

$$S = A \oplus B \oplus C_{in}, \qquad C_{out} = AB + (A \oplus B) C_{in}$$

**Bộ cộng n bit kiểu gợn sóng (ripple-carry):** nối $n$ bộ cộng toàn phần, bit nhớ ra của cột này là bit nhớ vào của cột kế tiếp — y hệt cách cộng tay.

```text
   A3 B3        A2 B2        A1 B1        A0 B0
    │  │         │  │         │  │         │  │
  ┌─▼──▼─┐     ┌─▼──▼─┐     ┌─▼──▼─┐     ┌─▼──▼─┐
◄─┤  FA  │◄────┤  FA  │◄────┤  FA  │◄────┤  FA  │◄── 0
C4└──┬───┘ C3  └──┬───┘ C2  └──┬───┘ C1  └──┬───┘ C0
     S3           S2           S1           S0
```

**Nhược điểm:** bit nhớ phải "lan" qua **mọi** tầng, nên độ trễ tăng tuyến tính theo số bit. CPU thật dùng các bộ cộng nhanh hơn như **carry-lookahead**, tính trước bit nhớ cho nhiều cột cùng lúc.

### 6.2 Chương trình: dựng bộ cộng 8 bit từ cổng logic

Chương trình sau mô phỏng bộ cộng chỉ bằng các phép `&`, `|`, `^` trên **từng bit**, tính cả hai cờ C và V, rồi so sánh với phép `+` thật:

```c
#include <stdint.h>
#include <stdio.h>

// Bộ cộng toàn phần (full adder) — chỉ dùng các "cổng logic" AND, OR, XOR
static void full_adder(int a, int b, int cin, int *sum, int *cout) {
    int x = a ^ b;
    *sum = x ^ cin;
    *cout = (a & b) | (x & cin);
}

// Bộ cộng 8 bit kiểu "gợn sóng" (ripple-carry): nối 8 full adder
static uint8_t add8(uint8_t a, uint8_t b, int *carry, int *overflow) {
    uint8_t result = 0;
    int c = 0, c_into_msb = 0;
    for (int i = 0; i < 8; i++) {
        int s;
        if (i == 7) c_into_msb = c;           // nhớ vào bit dấu
        full_adder((a >> i) & 1, (b >> i) & 1, c, &s, &c);
        result |= (uint8_t)(s << i);
    }
    *carry = c;                               // cờ C: nhớ ra khỏi bit cao nhất
    *overflow = c_into_msb ^ c;               // cờ V: tràn số có dấu
    return result;
}

static void print_bits(uint8_t v) {
    for (int i = 7; i >= 0; i--) putchar((v >> i) & 1 ? '1' : '0');
}

static void demo(uint8_t a, uint8_t b) {
    int c, v;
    uint8_t r = add8(a, b, &c, &v);
    print_bits(a); printf(" + "); print_bits(b); printf(" = "); print_bits(r);
    printf("  | khong dau: %3u + %3u = %3u  C=%d | co dau: %4d + %4d = %4d  V=%d",
           a, b, r, c, (int8_t)a, (int8_t)b, (int8_t)r, v);
    printf("  %s\n", r == (uint8_t)(a + b) ? "(khop voi +)" : "(SAI!)");
}

int main(void) {
    demo(100, 50);    // tràn có dấu
    demo(240, 32);    // tràn không dấu
    demo(128, 255);   // cả hai
    demo(13, 250);    // 13 + (-6)
    return 0;
}
```

**Kết quả:**

```text
01100100 + 00110010 = 10010110  | khong dau: 100 +  50 = 150  C=0 | co dau:  100 +   50 = -106  V=1  (khop voi +)
11110000 + 00100000 = 00010000  | khong dau: 240 +  32 =  16  C=1 | co dau:  -16 +   32 =   16  V=0  (khop voi +)
10000000 + 11111111 = 01111111  | khong dau: 128 + 255 = 127  C=1 | co dau: -128 +   -1 =  127  V=1  (khop voi +)
00001101 + 11111010 = 00000111  | khong dau:  13 + 250 =   7  C=1 | co dau:   13 +   -6 =    7  V=0  (khop voi +)
```

**Đọc kết quả:**

- Dòng 1: không dấu đúng ($C = 0$), có dấu sai ($V = 1$): hai số dương ra số âm.
- Dòng 2: không dấu sai ($C = 1$), có dấu đúng ($V = 0$): $-16 + 32 = 16$.
- Dòng 3: cả hai đều sai.
- Dòng 4: đây chính là phép **trừ** $13 - 6$ của Ví dụ 2.1. $C = 1$ ở đây không có nghĩa là sai với phép trừ có dấu.
- Cờ V được tính bằng **XOR của bit nhớ vào và bit nhớ ra khỏi bit dấu** — đúng cách phần cứng làm.

### 6.3 Bộ chọn kênh (MUX) và bộ giải mã (decoder)

**MUX 2→1:** tín hiệu chọn $S$ quyết định đầu vào nào được đưa ra — giống câu lệnh `S ? I1 : I0` trong C.

$$Y = \overline{S} \cdot I_0 + S \cdot I_1$$

MUX $2^k \to 1$ dùng $k$ bit chọn. MUX có mặt khắp nơi trong CPU: chọn toán hạng, chọn kết quả, chọn giá trị mới cho thanh ghi PC.

**Bộ giải mã (decoder) $k \to 2^k$:** nhận số nhị phân $k$ bit, bật **đúng một** trong $2^k$ đầu ra. Dùng để chọn ô nhớ theo địa chỉ, chọn thanh ghi cần ghi.

### 6.4 ALU — đơn vị số học và logic

**ALU** (Arithmetic Logic Unit) gom các phép toán (cộng, trừ, AND, OR, XOR, dịch, so sánh...) vào một khối. Một MUX ở đầu ra chọn kết quả theo **mã phép toán** do bộ điều khiển gửi tới:

```text
        A (32 bit)   B (32 bit)
            │            │
   ┌────────▼────────────▼────────┐
   │  [+]  [−]  [AND]  [OR]  [<]  │   tính song song mọi phép
   │    └────┴────┬────┴────┘     │
   │            MUX ◄───────────── mã phép toán (ALUop)
   └──────────────┬───────────────┘
                  ▼
            kết quả + cờ (Z, N, C, V)
```

---

## Chương 7. Mạch tuần tự — nơi máy tính "ghi nhớ"

**Mạch tuần tự** có đầu ra phụ thuộc vào cả đầu vào **và trạng thái đã lưu**. Đây là nền tảng của thanh ghi, bộ nhớ và mọi thứ có "trạng thái".

### 7.1 Chốt (latch) và flip-flop

**Chốt SR:** hai cổng NOR nối chéo, đầu ra của cổng này quay lại làm đầu vào cổng kia — vòng phản hồi này giữ được 1 bit.

| S (set) | R (reset) | Q sau đó |
|:-:|:-:|---|
| 0 | 0 | **Giữ nguyên** (đây là "trí nhớ") |
| 1 | 0 | 1 |
| 0 | 1 | 0 |
| 1 | 1 | Không hợp lệ |

**Chốt D:** khi tín hiệu cho phép `E = 1`, Q đi theo D; khi `E = 0`, Q giữ nguyên.

**Flip-flop D kích theo sườn (edge-triggered):** chỉ lấy giá trị D **tại thời điểm xung đồng hồ đi lên** (sườn lên). Giữa các sườn, D thay đổi thế nào cũng không ảnh hưởng.

```text
Clock      ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───
       ────┘   └───┘   └───┘   └───┘   └───┘
           ↑       ↑       ↑       ↑       ↑      ← sườn lên: flip-flop chụp D
D        ┌───────┐      ┌─────┐ ┌──────────────
       ──┘       └──────┘     └─┘
Q          ┌───────┐       ┌───────────────────
       ────┘       └───────┘
```

Để ý: tại các sườn lên, D lần lượt bằng 1, 0, 1, 1, 1 và Q nhận đúng các giá trị đó. Xung ngắn của D (xuống 0 rồi lên lại) nằm **giữa** hai sườn nên hoàn toàn bị bỏ qua. Đây là cách **mọi** trạng thái trong CPU được cập nhật: đồng loạt, theo nhịp đồng hồ.

### 7.2 Đồng hồ (clock)

- **Chu kỳ xung** $T$ và **tần số** $f = 1 / T$. CPU 3 GHz có chu kỳ $1 / (3 \times 10^9) \approx 333$ ps.
- Chu kỳ phải **đủ dài** để tín hiệu đi hết đường dài nhất của mạch tổ hợp giữa hai flip-flop (**đường tới hạn — critical path**). Đây là lý do mạch ngắn hơn → xung nhanh hơn, và là ý tưởng gốc của **pipeline** (Chương 12).

### 7.3 Thanh ghi, bộ đếm, máy trạng thái

- **Thanh ghi (register) n bit:** $n$ flip-flop D dùng chung đồng hồ. Thanh ghi 64 bit của CPU chính là 64 flip-flop.
- **Bộ đếm (counter):** thanh ghi + bộ cộng 1, đầu ra quay lại đầu vào. Bộ đếm $n$ bit đếm từ 0 đến $2^n - 1$ rồi quay về 0.
- **Máy trạng thái hữu hạn (FSM):** thanh ghi lưu **trạng thái hiện tại** + mạch tổ hợp tính **trạng thái kế tiếp** và đầu ra. Bộ điều khiển của CPU, đèn giao thông, hay AI của một NPC đơn giản đều có thể mô tả bằng FSM.

```text
          ┌──────────────────────────────┐
          │                              │
 input ──►│  Mạch tổ hợp                 │──► output
          │  (trạng thái kế tiếp)        │
          └──────────┬───────────────────┘
                     │
               ┌─────▼─────┐
     clock ───►│ Thanh ghi │── trạng thái hiện tại ──┐
               └───────────┘                         │
                     ▲                               │
                     └───────────────────────────────┘
```

### 7.4 Ô nhớ SRAM và DRAM

| | SRAM | DRAM |
|---|---|---|
| Cấu tạo mỗi bit | Khoảng **6 transistor** (hai cổng NOT nối vòng) | **1 transistor + 1 tụ điện** |
| Giữ dữ liệu | Chừng nào còn điện | Tụ rò điện → phải **làm tươi (refresh)** định kỳ (cỡ vài chục ms) |
| Tốc độ | Rất nhanh | Chậm hơn |
| Mật độ, giá | Thấp, đắt | Cao, rẻ |
| Dùng làm | **Cache** trong CPU | **RAM** chính |

Sự khác biệt này là lý do tồn tại **phân cấp bộ nhớ** (Chương 14).

---

## Bài tập Phần II

**Bài II.1.** Rút gọn: (a) $F = (A + B)(A + \overline{B})$; (b) $G = \overline{\overline{A} + \overline{B}}$.

**Bài II.2.** Dùng bìa Karnaugh rút gọn hàm $F(A, B, C)$ bằng 1 tại các minterm 0, 2, 4, 6.

**Bài II.3.** Cần bao nhiêu cổng NAND để tạo cổng NOT, AND, OR?

**Bài II.4.** Một bộ cộng ripple-carry 32 bit, mỗi bộ cộng toàn phần làm trễ tín hiệu nhớ 2 đơn vị thời gian cổng. Trường hợp xấu nhất, bit nhớ ra cuối cùng ổn định sau bao lâu? Còn với 64 bit?

**Bài II.5.** Một bộ đếm 3 bit bắt đầu từ 0. Sau 10 sườn lên của đồng hồ, giá trị của nó là bao nhiêu? Cần tối thiểu bao nhiêu flip-flop để đếm từ 0 đến 1000?

**Bài II.6.** Viết biểu thức của MUX 4→1 với hai bit chọn $S_1 S_0$ và bốn đầu vào $I_0 \dots I_3$.

<details>
<summary><b>Đáp án Phần II</b> (bấm để mở)</summary>

**II.1.** (a) $F = A + B\overline{B} = A + 0 = A$ (luật phân phối dạng OR). (b) Theo De Morgan: $G = \overline{\overline{A}} \cdot \overline{\overline{B}} = AB$.

**II.2.** Bốn minterm 000, 010, 100, 110 đều có $C = 0$ và phủ đủ mọi tổ hợp của A, B → **$F = \overline{C}$**.

**II.3.** NOT: 1 cổng. AND: 2 cổng. OR: 3 cổng.

**II.4.** 32 bit: $32 \times 2 = 64$ đơn vị. 64 bit: **128** đơn vị — độ trễ tăng gấp đôi, đó là lý do CPU 64 bit cần bộ cộng nhanh hơn (carry-lookahead).

**II.5.** $10 \bmod 8 = 2$ → giá trị **`010`**. Để đếm tới 1000 cần $\lceil \log_2 1001 \rceil = 10$ flip-flop ($2^{10} = 1024$).

**II.6.** $Y = \overline{S_1}\,\overline{S_0}\, I_0 + \overline{S_1} S_0 I_1 + S_1 \overline{S_0} I_2 + S_1 S_0 I_3$.

</details>

---

# PHẦN III — BỘ XỬ LÝ

## Chương 8. Kiến trúc von Neumann và chu trình thực thi lệnh

### 8.1 Mô hình von Neumann

Gần như mọi máy tính hiện nay theo mô hình do von Neumann mô tả năm 1945: **chương trình và dữ liệu cùng nằm trong bộ nhớ**, và CPU lần lượt lấy từng lệnh ra thực hiện.

```text
┌────────────────────────── CPU ───────────────────────────┐
│  ┌──────────────────┐        ┌─────────────────────────┐  │
│  │ Khối điều khiển  │        │ Đường dữ liệu (datapath) │  │
│  │ (Control Unit)   │───────►│  - Các thanh ghi         │  │
│  │ giải mã lệnh,    │ tín    │  - ALU                   │  │
│  │ phát tín hiệu    │ hiệu   │  - PC, IR                │  │
│  └──────────────────┘        └─────────────────────────┘  │
└────────────────────────────┬─────────────────────────────┘
                             │  BUS (địa chỉ, dữ liệu, điều khiển)
          ┌──────────────────┼──────────────────┐
   ┌──────▼──────┐    ┌──────▼──────┐    ┌──────▼──────┐
   │  Bộ nhớ     │    │  Vào/ra     │    │  Lưu trữ    │
   │ (lệnh +     │    │ (bàn phím,  │    │ (SSD, HDD)  │
   │  dữ liệu)   │    │  màn hình…) │    │             │
   └─────────────┘    └─────────────┘    └─────────────┘
```

**Kiến trúc Harvard** tách riêng bộ nhớ lệnh và bộ nhớ dữ liệu. Nhiều vi điều khiển dùng Harvard; CPU máy tính hiện đại là von Neumann ở cấp hệ thống nhưng tách **cache lệnh** và **cache dữ liệu** (L1i, L1d) — gọi là "Harvard cải tiến".

### 8.2 Các thanh ghi đặc biệt

| Thanh ghi | Vai trò |
|---|---|
| **PC** (Program Counter) | Địa chỉ của lệnh **sắp** thực hiện |
| **IR** (Instruction Register) | Lệnh **đang** thực hiện |
| **SP** (Stack Pointer) | Đỉnh stack |
| **Thanh ghi cờ** (flags / status) | Các cờ Z, N, C, V của phép toán vừa rồi (x86, ARM) |
| **Thanh ghi đa dụng** | Chứa dữ liệu đang tính toán (RISC-V có 32, x86-64 có 16) |

### 8.3 Chu trình Fetch – Decode – Execute

CPU lặp đi lặp lại **hàng tỉ lần mỗi giây**:

```text
      ┌──────────────────────────────────────────────┐
      ▼                                              │
 1. FETCH   : IR ← Bộ nhớ[PC]                        │
 2. DECODE  : phân tích IR: phép gì? thanh ghi nào?  │
 3. EXECUTE : ALU tính toán / tính địa chỉ           │
 4. MEMORY  : đọc/ghi bộ nhớ (nếu lệnh cần)          │
 5. WRITE   : ghi kết quả vào thanh ghi              │
 6. PC      ← PC + 4   (hoặc địa chỉ nhảy tới)  ─────┘
```

Toàn bộ "sự thông minh" của máy tính đến từ việc lặp vòng đơn giản này thật nhanh, trên các lệnh cực kỳ đơn giản.

---

## Chương 9. Kiến trúc tập lệnh (ISA)

### 9.1 ISA là gì?

**Kiến trúc tập lệnh (Instruction Set Architecture)** là **hợp đồng** giữa phần mềm và phần cứng: CPU hiểu những lệnh nào, có những thanh ghi nào, lệnh được mã hóa ra sao. Các CPU khác nhau (Intel, AMD) có thể thiết kế bên trong rất khác nhau nhưng cùng một ISA (x86-64), nên chạy được cùng một chương trình.

| ISA | Kiểu | Dùng ở đâu |
|---|---|---|
| **x86-64** | CISC | Hầu hết PC, laptop, máy chủ (Intel, AMD) |
| **ARM (AArch64)** | RISC | Điện thoại, máy tính bảng, nhiều laptop và máy chủ đời mới |
| **RISC-V** | RISC, **mở** (không mất phí bản quyền) | Vi điều khiển, nghiên cứu, giảng dạy, ngày càng nhiều chip thương mại |

### 9.2 CISC và RISC

| | CISC (x86) | RISC (ARM, RISC-V) |
|---|---|---|
| Số lệnh | Nhiều, phức tạp | Ít, đơn giản |
| Độ dài lệnh | Thay đổi (1–15 byte) | Cố định (thường 4 byte) |
| Truy cập bộ nhớ | Nhiều lệnh tính toán trực tiếp trên bộ nhớ | **Chỉ** `load`/`store` mới truy cập bộ nhớ |
| Giải mã | Phức tạp | Đơn giản, dễ pipeline |

Ranh giới ngày nay đã mờ: CPU x86 hiện đại **dịch** lệnh CISC thành các vi lệnh (micro-op) kiểu RISC bên trong.

### 9.3 Thanh ghi RISC-V (RV32/RV64)

RISC-V có 32 thanh ghi đa dụng `x0`–`x31`, mỗi thanh ghi có **tên theo quy ước sử dụng**:

| Thanh ghi | Tên | Vai trò | Hàm được gọi phải giữ nguyên? |
|---|---|---|:-:|
| `x0` | `zero` | **Luôn bằng 0** (ghi vào bị bỏ qua) | — |
| `x1` | `ra` | Địa chỉ trả về (return address) | |
| `x2` | `sp` | Con trỏ stack | ✅ |
| `x5`–`x7`, `x28`–`x31` | `t0`–`t6` | Biến tạm | ❌ |
| `x8`–`x9`, `x18`–`x27` | `s0`–`s11` | Biến cần giữ qua lời gọi hàm (`s0` còn là `fp`) | ✅ |
| `x10`–`x11` | `a0`–`a1` | Tham số hàm **và giá trị trả về** | ❌ |
| `x12`–`x17` | `a2`–`a7` | Tham số hàm | ❌ |

### 9.4 Định dạng lệnh RISC-V

Mọi lệnh dài **32 bit**; 7 bit thấp nhất (`opcode`) cho biết định dạng:

```text
         31        25 24    20 19    15 14  12 11     7 6      0
R-type  │  funct7    │  rs2   │  rs1   │funct3│   rd   │ opcode │  add, sub, and, or...
I-type  │      imm[11:0]      │  rs1   │funct3│   rd   │ opcode │  addi, lw, jalr...
S-type  │ imm[11:5]  │  rs2   │  rs1   │funct3│imm[4:0]│ opcode │  sw
B-type  │imm[12|10:5]│  rs2   │  rs1   │funct3│imm[4:1|11]│opcode│  beq, bne, blt, bge
U-type  │            imm[31:12]                │   rd   │ opcode │  lui, auipc
J-type  │      imm[20|10:1|11|19:12]           │   rd   │ opcode │  jal
```

- `rd`: thanh ghi đích; `rs1`, `rs2`: thanh ghi nguồn; `imm`: hằng số (immediate).
- Vị trí `rs1`, `rs2`, `rd` **cố định** trong mọi định dạng → CPU giải mã rất nhanh.

**Ví dụ 9.1.** Mã hóa `add x3, x1, x2` (R-type, `opcode = 0110011`, `funct3 = 000`, `funct7 = 0000000`):

```text
 funct7   rs2    rs1   funct3  rd     opcode
 0000000  00010  00001  000   00011  0110011

 0000 0000 0010 0000 1000 0001 1011 0011  =  0x002081B3
```

**Ví dụ 9.2.** Mã hóa `addi x1, x0, 5` (I-type, `opcode = 0010011`):

```text
 imm (12 bit)   rs1    funct3  rd     opcode
 000000000101   00000   000   00001  0010011

 0000 0000 0101 0000 0000 0000 1001 0011  =  0x00500093
```

**Ví dụ 9.3.** Mã hóa `sw x5, 8(x2)` — lưu `x5` vào bộ nhớ tại địa chỉ `x2 + 8` (S-type, `opcode = 0100011`, `funct3 = 010`). Hằng số $8 = 000000001000_2$ bị **tách đôi**: 7 bit cao ở đầu, 5 bit thấp ở giữa:

```text
 imm[11:5]  rs2    rs1   funct3  imm[4:0]  opcode
 0000000    00101  00010  010    01000     0100011

 0000 0000 0101 0001 0010 0100 0010 0011  =  0x00512423
```

| Lệnh | Mã máy (đã kiểm tra bằng capstone) |
|---|---|
| `addi x1, x0, 5` | `0x00500093` |
| `add x3, x1, x2` | `0x002081B3` |
| `sub x3, x1, x2` | `0x402081B3` (chỉ khác `funct7`) |
| `lw x6, 4(x10)` | `0x00452303` |
| `sw x5, 8(x2)` | `0x00512423` |

### 9.5 Các chế độ định địa chỉ

| Chế độ | Toán hạng lấy từ | Ví dụ RISC-V |
|---|---|---|
| Thanh ghi | Giá trị trong thanh ghi | `add x3, x1, x2` |
| Tức thời (immediate) | Hằng số nằm ngay trong lệnh | `addi x1, x1, 5` |
| Cơ sở + độ dời (base + offset) | Bộ nhớ tại `thanh ghi + hằng` | `lw x6, 4(x10)` |
| Tương đối PC | `PC + hằng` | `beq`, `jal` |

---

## Chương 10. Hợp ngữ

### 10.1 Các lệnh RISC-V thường dùng

| Nhóm | Lệnh | Ý nghĩa tương đương C |
|---|---|---|
| Số học | `add rd, rs1, rs2` | `rd = rs1 + rs2` |
| | `sub rd, rs1, rs2` | `rd = rs1 - rs2` |
| | `addi rd, rs1, imm` | `rd = rs1 + imm` |
| | `mul rd, rs1, rs2` | `rd = rs1 * rs2` (phần mở rộng M) |
| Logic / dịch | `and`, `or`, `xor`, `slli`, `srli`, `srai` | `&`, `\|`, `^`, `<<`, `>>` |
| Bộ nhớ | `lw rd, off(rs1)` | `rd = *(int *)(rs1 + off)` |
| | `sw rs2, off(rs1)` | `*(int *)(rs1 + off) = rs2` |
| Rẽ nhánh | `beq`, `bne`, `blt`, `bge` | `if (rs1 == / != / < / >= rs2) goto nhãn` |
| Nhảy | `jal rd, nhãn` | `rd = PC + 4; goto nhãn` (gọi hàm) |
| | `jalr rd, off(rs1)` | `rd = PC + 4; goto rs1 + off` |

**Lệnh giả (pseudo-instruction)** — trình hợp dịch tự đổi sang lệnh thật:

| Lệnh giả | Thực chất là |
|---|---|
| `li t0, 5` | `addi t0, zero, 5` |
| `mv a0, t0` | `addi a0, t0, 0` |
| `j nhãn` | `jal zero, nhãn` |
| `ret` | `jalr zero, 0(ra)` |

### 10.2 Ví dụ: tổng một mảng

Hàm C:

```c
int sum_array(const int *arr, int n) {
    int total = 0;
    for (int i = 0; i < n; i++) total += arr[i];
    return total;
}
```

Hợp ngữ RISC-V (RV32) viết tay:

```asm
# a0 = địa chỉ mảng, a1 = n. Trả về tổng trong a0.
sum_array:
    li   t0, 0            # total = 0
    li   t1, 0            # i = 0
loop:
    bge  t1, a1, done     # nếu i >= n thì thoát vòng lặp
    slli t2, t1, 2        # t2 = i * 4   (mỗi int chiếm 4 byte)
    add  t2, a0, t2       # t2 = địa chỉ của arr[i]
    lw   t3, 0(t2)        # t3 = arr[i]  (đọc bộ nhớ)
    add  t0, t0, t3       # total += arr[i]
    addi t1, t1, 1        # i++
    j    loop
done:
    mv   a0, t0           # giá trị trả về đặt vào a0
    ret
```

Mã máy tương ứng khi đặt hàm tại địa chỉ `0x1000`:

| Địa chỉ | Mã máy | Lệnh |
|---|---|---|
| `0x1000` | `00000293` | `li t0, 0` |
| `0x1004` | `00000313` | `li t1, 0` |
| `0x1008` | `00b35e63` | `bge t1, a1, done` (nhảy +28 byte) |
| `0x100c` | `00231393` | `slli t2, t1, 2` |
| `0x1010` | `007503b3` | `add t2, a0, t2` |
| `0x1014` | `0003ae03` | `lw t3, 0(t2)` |
| `0x1018` | `01c282b3` | `add t0, t0, t3` |
| `0x101c` | `00130313` | `addi t1, t1, 1` |
| `0x1020` | `fe9ff06f` | `j loop` (nhảy −24 byte) |
| `0x1024` | `00028513` | `mv a0, t0` |
| `0x1028` | `00008067` | `ret` |

Chạy trên trình giả lập với mảng `{3, 1, 4, 1, 5, 9}` cho kết quả **`a0 = 23`** ✓.

**Nhận xét:**

- `arr[i]` trong C thực chất là **"lấy địa chỉ + i × kích thước phần tử, rồi đọc bộ nhớ"**.
- Vòng lặp `for` chỉ là **một lệnh rẽ nhánh có điều kiện + một lệnh nhảy ngược**.
- Hằng số nhảy trong mã máy là **tương đối so với PC**: `fe9ff06f` mã hóa độ dời −24, tức từ `0x1020` quay về `0x1008`.

### 10.3 Gọi hàm và stack

**Quy ước gọi hàm (calling convention)** của RISC-V:

1. Bên gọi đặt tham số vào `a0`–`a7`, rồi `jal ra, hàm` (lưu địa chỉ quay về vào `ra`).
2. Hàm được gọi đặt kết quả vào `a0`, rồi `ret`.
3. Nếu hàm được gọi **lại gọi hàm khác**, nó phải **lưu `ra`** (và các giá trị cần giữ) lên **stack** trước, vì `jal` sẽ ghi đè `ra`.

**Ví dụ 10.1.** Hàm giai thừa đệ quy:

```c
int fact(int n) { return n < 2 ? 1 : n * fact(n - 1); }
```

```asm
fact:
    addi sp, sp, -16      # cấp 16 byte trên stack (stack mọc xuống dưới)
    sw   ra, 12(sp)       # lưu địa chỉ quay về
    sw   a0, 8(sp)        # lưu n (vì lời gọi đệ quy sẽ ghi đè a0)
    li   t0, 2
    blt  a0, t0, base     # nếu n < 2 thì trả về 1
    addi a0, a0, -1       # a0 = n - 1
    jal  ra, fact         # a0 = fact(n - 1)
    lw   t1, 8(sp)        # lấy lại n
    mul  a0, a0, t1       # a0 = n * fact(n - 1)
    j    end
base:
    li   a0, 1
end:
    lw   ra, 12(sp)       # khôi phục địa chỉ quay về
    addi sp, sp, 16       # trả lại vùng stack
    ret
```

Chạy thử trên trình giả lập: `fact(0) = 1`, `fact(1) = 1`, `fact(5) = 120`, `fact(10) = 3628800`, và `sp` luôn được **khôi phục đúng** giá trị ban đầu sau khi hàm trả về ✓.

**Stack khi đang tính `fact(3)`** (sâu nhất, trong lời gọi `fact(1)`):

```text
địa chỉ cao
 ┌──────────────────────┐ ◄── sp ban đầu
 │ ra (quay về bên gọi) │  khung của fact(3)
 │ n = 3                │
 │ 8 byte chưa dùng     │
 ├──────────────────────┤
 │ ra (quay về fact(3)) │  khung của fact(2)
 │ n = 2                │
 │ ...                  │
 ├──────────────────────┤
 │ ra (quay về fact(2)) │  khung của fact(1)
 │ n = 1                │
 │ ...                  │
 └──────────────────────┘ ◄── sp hiện tại
địa chỉ thấp
```

Mỗi khung chiếm 16 byte dù chỉ dùng 8, vì quy ước của RISC-V yêu cầu `sp` luôn là bội số của 16. Đây chính là "call stack" mà trình gỡ lỗi hiển thị cho bạn. **Tràn stack (stack overflow)** xảy ra khi có quá nhiều khung như thế này.

### 10.4 Đọc hợp ngữ x86-64 do GCC sinh ra

Với hàm:

```c
long sum_array(const int *a, long n) {
    long s = 0;
    for (long i = 0; i < n; i++)
        s += a[i];
    return s;
}
```

Lệnh `gcc -O1 -S -masm=intel -fno-asynchronous-unwind-tables -fcf-protection=none sum.c` cho ra (đã lược bớt vài dòng chỉ thị):

```asm
sum_array:
	test	rsi, rsi
	jle	.L4
	mov	rax, rdi
	lea	rsi, [rdi+rsi*4]
	mov	edx, 0
.L3:
	movsx	rcx, DWORD PTR [rax]
	add	rdx, rcx
	add	rax, 4
	cmp	rax, rsi
	jne	.L3
.L1:
	mov	rax, rdx
	ret
.L4:
	mov	edx, 0
	jmp	.L1
```

**Giải thích** (quy ước gọi hàm System V trên Linux/macOS: tham số lần lượt ở `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`; kết quả trả về ở `rax`):

| Lệnh | Ý nghĩa |
|---|---|
| `test rsi, rsi` / `jle .L4` | Nếu `n <= 0` thì nhảy tới `.L4` (trả về 0) |
| `mov rax, rdi` | `p = a` — con trỏ chạy |
| `lea rsi, [rdi+rsi*4]` | `end = a + n * 4` — `lea` là cách "mượn" cú pháp địa chỉ để làm phép tính nhanh |
| `mov edx, 0` | `s = 0` |
| `movsx rcx, DWORD PTR [rax]` | Đọc `int` 32 bit tại `p` và **mở rộng dấu** lên 64 bit |
| `add rdx, rcx` | `s += *p` |
| `add rax, 4` / `cmp rax, rsi` / `jne .L3` | `p++`; lặp khi `p != end` |
| `mov rax, rdx` / `ret` | Trả về `s` |

**Điều thú vị:** trình biên dịch đã **tự đổi vòng lặp dùng chỉ số `i` thành vòng lặp dùng con trỏ** — không còn phép nhân `i * 4` trong mỗi vòng. Đây là loại tối ưu bạn chỉ thấy được khi đọc hợp ngữ.

> 💡 **Công cụ:** **Compiler Explorer** (godbolt.org) cho phép gõ code C/C++ và xem ngay hợp ngữ do nhiều trình biên dịch sinh ra cho nhiều kiến trúc (x86-64, ARM, RISC-V), với các mức tối ưu khác nhau.
>
> Trên Windows, quy ước gọi hàm x64 của Microsoft khác: 4 tham số đầu nằm ở `rcx`, `rdx`, `r8`, `r9`.

---

## Chương 11. Đo và so sánh hiệu năng

### 11.1 Phương trình hiệu năng CPU

$$\boxed{\text{Thời gian CPU} = \text{IC} \times \text{CPI} \times T = \frac{\text{IC} \times \text{CPI}}{f}}$$

| Ký hiệu | Ý nghĩa | Phụ thuộc vào |
|---|---|---|
| **IC** (Instruction Count) | Số lệnh thực hiện | Thuật toán, ngôn ngữ, trình biên dịch, ISA |
| **CPI** (Cycles Per Instruction) | Số chu kỳ xung trung bình mỗi lệnh | Thiết kế CPU, loại lệnh |
| $T = 1/f$ | Thời gian một chu kỳ xung | Công nghệ chế tạo, thiết kế mạch |

**Ví dụ 11.1.** Một chương trình có $10^9$ lệnh.

- Máy A: CPI = 2,0; tần số 3 GHz → $\dfrac{10^9 \times 2}{3 \times 10^9} \approx 0{,}667$ s.
- Máy B: CPI = 1,2; tần số 2 GHz → $\dfrac{10^9 \times 1{,}2}{2 \times 10^9} = 0{,}6$ s.

Máy B **nhanh hơn 1,11 lần** dù có tần số thấp hơn. → **Tần số (GHz) một mình không nói lên tốc độ.**

**CPI trung bình** khi có nhiều loại lệnh: $\text{CPI} = \sum (\text{tỉ lệ lệnh loại } i) \times \text{CPI}_i$.

**Ví dụ 11.2.** 50% lệnh loại A (CPI 1), 30% loại B (CPI 2), 20% loại C (CPI 3):
$\text{CPI} = 0{,}5 \times 1 + 0{,}3 \times 2 + 0{,}2 \times 3 = 1{,}7$.

### 11.2 Định luật Amdahl

Nếu chỉ tăng tốc **một phần** chương trình, chiếm tỉ lệ $p$ thời gian, lên $s$ lần:

$$\boxed{\text{Tăng tốc tổng} = \frac{1}{(1 - p) + \dfrac{p}{s}}}$$

**Giới hạn:** khi $s \to \infty$, tăng tốc tối đa là $\dfrac{1}{1 - p}$.

**Ví dụ 11.3.**

- Phần chiếm 40% thời gian được tăng tốc 10 lần → $\dfrac{1}{0{,}6 + 0{,}04} \approx 1{,}56$ lần.
- 80% chương trình song song hóa được, chạy trên 4 lõi → $\dfrac{1}{0{,}2 + 0{,}2} = 2{,}5$ lần. Dù có vô hạn lõi, tối đa chỉ **5 lần**.

> 💡 **Bài học thực tế:** hãy **đo (profile)** trước khi tối ưu, và tập trung vào phần **chiếm nhiều thời gian nhất**. Tối ưu một hàm chỉ chiếm 5% thời gian thì dù nhanh gấp vô hạn, chương trình cũng chỉ nhanh hơn khoảng 5%.

### 11.3 Những thước đo dễ gây hiểu lầm

- **MIPS** (triệu lệnh mỗi giây): không so sánh được giữa các ISA khác nhau, vì một lệnh CISC có thể làm việc của nhiều lệnh RISC.
- **Tần số xung:** như Ví dụ 11.1.
- **Số lõi:** vô nghĩa nếu chương trình chỉ chạy một luồng.

Thước đo đáng tin nhất là **thời gian chạy thực tế của chính khối lượng công việc bạn quan tâm**, hoặc các bộ benchmark chuẩn (như SPEC CPU) mô phỏng công việc thật.

---

## Chương 12. Pipeline

### 12.1 Ý tưởng: dây chuyền

Giặt đồ gồm 4 bước: giặt → sấy → gấp → cất. Làm **tuần tự** từng mẻ rất chậm; làm kiểu **dây chuyền** — mẻ 1 vào máy sấy thì mẻ 2 vào máy giặt ngay — thì **thông lượng** tăng gần 4 lần, dù thời gian cho **một** mẻ không đổi.

CPU RISC kinh điển chia việc thực hiện lệnh thành **5 tầng**, mỗi tầng ngăn cách bởi một thanh ghi pipeline:

| Tầng | Việc làm |
|---|---|
| **IF** (Instruction Fetch) | Đọc lệnh tại PC, tăng PC |
| **ID** (Instruction Decode) | Giải mã, đọc các thanh ghi nguồn |
| **EX** (Execute) | ALU tính toán / tính địa chỉ / so sánh rẽ nhánh |
| **MEM** (Memory) | Đọc/ghi bộ nhớ dữ liệu |
| **WB** (Write Back) | Ghi kết quả vào thanh ghi đích |

```text
Chu kỳ xung :   1     2     3     4     5     6     7     8
Lệnh 1      :  IF    ID    EX    MEM   WB
Lệnh 2      :        IF    ID    EX    MEM   WB
Lệnh 3      :              IF    ID    EX    MEM   WB
Lệnh 4      :                    IF    ID    EX    MEM   WB
```

Với $k$ tầng và $n$ lệnh, pipeline hoàn thành sau $k + n - 1$ chu kỳ.

### 12.2 Tính tăng tốc

**Ví dụ 12.1.** Thời gian các tầng lần lượt là 200, 150, 250, 300, 100 ps; mỗi thanh ghi pipeline thêm 20 ps.

- **Không pipeline (single-cycle):** chu kỳ = tổng = **1000 ps**.
- **Có pipeline:** chu kỳ = tầng **chậm nhất** + thanh ghi = 300 + 20 = **320 ps**.
- Tăng tốc (thông lượng) ≈ $1000 / 320 \approx$ **3,1 lần** — không đạt 5 lần vì các tầng **không cân bằng** và có chi phí thanh ghi.

Với $10^9$ lệnh: không pipeline mất $10^9 \times 1000 \text{ ps} = 1$ s; có pipeline mất khoảng $(5 + 10^9 - 1) \times 320 \text{ ps} \approx 0{,}32$ s.

> Pipeline tăng **thông lượng** (số lệnh hoàn thành mỗi giây), không làm **một lệnh riêng lẻ** nhanh hơn — thực tế mỗi lệnh còn chậm hơn một chút (5 × 320 = 1600 ps).

### 12.3 Xung đột (hazard)

Dây chuyền chỉ chạy trơn khi các lệnh không "vướng" nhau. Có ba loại xung đột:

**1. Xung đột dữ liệu (data hazard):** lệnh sau cần kết quả mà lệnh trước **chưa kịp ghi**.

```text
Chu kỳ            :  1    2    3    4    5    6
add x1, x2, x3    :  IF   ID   EX   MEM  WB          ← x1 chỉ được ghi ở chu kỳ 5
sub x4, x1, x5    :       IF   ID   EX   MEM  WB     ← nhưng cần x1 từ chu kỳ 3–4!
```

**Cách giải quyết — chuyển tiếp (forwarding):** kết quả của `add` đã có sẵn trong thanh ghi pipeline sau tầng EX (chu kỳ 3); ta **đưa thẳng** nó sang đầu vào ALU của `sub` ở chu kỳ 4, không chờ WB.

```text
add x1, x2, x3    :  IF   ID   EX ──┐MEM  WB
                                    │ (kết quả x1 nằm ở thanh ghi EX/MEM)
sub x4, x1, x5    :       IF   ID   ▼EX   MEM  WB
```

**Trường hợp đặc biệt — đọc bộ nhớ rồi dùng ngay:**

```text
lw  x1, 0(x2)     :  IF   ID   EX   MEM ─┐WB        ← x1 chỉ có SAU tầng MEM (cuối chu kỳ 4)
add x3, x1, x4    :       IF   ID   ──   ▼EX   MEM  WB   ← phải CHÈN 1 CHU KỲ TRỐNG (stall / bubble)
```

Trình biên dịch có thể **sắp xếp lại lệnh** để tránh kiểu xung đột này.

**2. Xung đột điều khiển (control hazard):** với lệnh rẽ nhánh, đến tầng EX mới biết có nhảy hay không — nhưng các lệnh phía sau **đã được nạp vào** pipeline. Nếu đoán sai, phải **hủy (flush)** chúng.

**Giải quyết:** **dự đoán rẽ nhánh** (Chương 13), rồi sửa khi đoán sai.

**Chi phí:** giả sử 20% lệnh là rẽ nhánh, đoán sai 10% số đó, mỗi lần sai mất 2 chu kỳ:

$$\text{CPI} = 1 + 0{,}2 \times 0{,}1 \times 2 = 1{,}04$$

**3. Xung đột tài nguyên (structural hazard):** hai tầng cần cùng một phần cứng tại cùng thời điểm. Tránh bằng thiết kế (ví dụ tách bộ nhớ lệnh và dữ liệu).

---

## Chương 13. Các kỹ thuật của CPU hiện đại

### 13.1 Dự đoán rẽ nhánh

CPU hiện đại có pipeline dài (khoảng 10–20 tầng), nên mỗi lần đoán sai rất tốn kém. Các bộ dự đoán dựa trên **lịch sử**:

**Bộ dự đoán 1 bit:** nhớ lần trước nhảy hay không, đoán lần này giống vậy.

**Bộ dự đoán 2 bit (bộ đếm bão hòa):** phải sai **hai lần liên tiếp** mới đổi dự đoán:

```text
          ──T──►          ──T──►          ──T──►
  [ 00 ]          [ 01 ]          [ 10 ]          [ 11 ]
          ◄──N──          ◄──N──          ◄──N──

  đoán:           đoán:           đoán:           đoán:
  KHÔNG nhảy      KHÔNG nhảy      NHẢY            NHẢY
  (chắc chắn)     (yếu)           (yếu)           (chắc chắn)

  Ở [00] mà gặp N thì giữ nguyên [00]; ở [11] mà gặp T thì giữ nguyên [11].
```

Quy tắc: trạng thái 0–1 đoán **không nhảy**, 2–3 đoán **nhảy**; mỗi lần nhảy thật thì **tăng 1** (tối đa 3), không nhảy thì **giảm 1** (tối thiểu 0).

**Ví dụ 13.1.** Một vòng lặp chạy 4 lần, được gọi 3 lần liên tiếp. Lệnh rẽ nhánh cuối vòng lặp có kết quả: `T T T N T T T N T T T N` (T = nhảy về đầu vòng, N = thoát). Cả hai bộ dự đoán ban đầu đều đoán "nhảy".

```text
Thực tế      : T T T N T T T N T T T N
1 bit (sai)  : . . . x x . . x x . . x     → 5 lần sai
2 bit (sai)  : . . . x . . . x . . . x     → 3 lần sai
```

Bộ 1 bit sai **hai lần** mỗi khi thoát vòng lặp (lần thoát, và lần đầu của lượt sau); bộ 2 bit chỉ sai **một lần**. CPU thật dùng các bộ dự đoán phức tạp hơn nhiều, đạt độ chính xác rất cao với phần lớn chương trình.

**Thí nghiệm:** cùng một hàm, chạy trên mảng **ngẫu nhiên** và mảng **đã sắp xếp**:

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 20000000

static double now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

static int cmp(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

// Cộng các phần tử >= 128 — có một lệnh rẽ nhánh trong vòng lặp
static long long sum_big(const int *v, int n) {
    long long s = 0;
    for (int i = 0; i < n; i++)
        if (v[i] >= 128) s += v[i];
    return s;
}

int main(void) {
    int *v = malloc(N * sizeof *v);
    if (v == NULL) return 1;
    srand(42);
    for (int i = 0; i < N; i++) v[i] = rand() % 256;

    double t = now();
    long long a = sum_big(v, N);             // dữ liệu ngẫu nhiên: nhánh khó đoán
    double t_random = now() - t;

    qsort(v, N, sizeof *v, cmp);
    t = now();
    long long b = sum_big(v, N);             // dữ liệu đã sắp xếp: nhánh rất dễ đoán
    double t_sorted = now() - t;

    printf("Ket qua giong nhau: %s\n", a == b ? "co" : "khong");
    printf("Mang ngau nhien : %.3f s\n", t_random);
    printf("Mang da sap xep : %.3f s\n", t_sorted);
    free(v);
    return 0;
}
```

**Ví dụ kết quả** khi biên dịch với `gcc -O0 branch.c -o branch` (đo trên máy thử nghiệm; con số trên máy bạn sẽ khác):

```text
Ket qua giong nhau: co
Mang ngau nhien : 0.121 s
Mang da sap xep : 0.040 s
```

Với mảng ngẫu nhiên, lệnh `if` đúng/sai hoàn toàn ngẫu nhiên → CPU đoán sai khoảng một nửa số lần. Với mảng đã sắp xếp, kết quả là "sai sai sai... đúng đúng đúng" → gần như luôn đoán đúng → nhanh hơn **khoảng 3 lần**, dù làm **đúng cùng một khối lượng** tính toán.

**Điều bất ngờ:** biên dịch với `-O1` hoặc `-O2`, hai con số gần như **bằng nhau** (trên máy thử nghiệm: 0,017 s và 0,016 s). Khi đọc hợp ngữ, ta thấy GCC đã thay lệnh `if` bằng lệnh **`cmovg`** (chuyển giá trị có điều kiện) — **không còn rẽ nhánh nào để đoán sai**. Ở mức `-O2`, GCC còn **véc tơ hóa** vòng lặp bằng các lệnh SIMD như `pcmpgtd` (so sánh nhiều số cùng lúc), `pand` và `paddq` — vừa không rẽ nhánh, vừa xử lý nhiều phần tử mỗi bước (Chương 13.3). Bài học: **trình biên dịch hiểu kiến trúc máy tính rất rõ** — hãy đo đạc trên bản build có tối ưu trước khi tự "tối ưu tay".

### 13.2 Siêu vô hướng và thực thi không theo thứ tự

- **Siêu vô hướng (superscalar):** CPU có **nhiều** đường thực thi song song, xử lý nhiều lệnh trong cùng một tầng.
- **Thực thi không theo thứ tự (out-of-order execution):** CPU thực hiện lệnh **ngay khi dữ liệu sẵn sàng**, không nhất thiết theo thứ tự trong chương trình, rồi **sắp xếp lại kết quả** cho đúng thứ tự trước khi "công bố" (commit).
- **Thực thi suy đoán (speculative execution):** thực hiện trước các lệnh sau một nhánh **theo dự đoán**; nếu đoán sai thì hủy kết quả.

> ⚠️ Năm 2018, các lỗ hổng **Spectre** và **Meltdown** cho thấy thực thi suy đoán có thể để lại **dấu vết trong cache** mà kẻ tấn công đo được, dù kết quả đã bị hủy. Đây là ví dụ nổi tiếng về việc kiến trúc máy tính ảnh hưởng trực tiếp đến bảo mật.

### 13.3 SIMD — một lệnh, nhiều dữ liệu

Các tập lệnh **SSE, AVX** (x86), **NEON** (ARM), **RVV** (RISC-V) có thanh ghi rộng (128, 256, 512 bit) để xử lý **nhiều số cùng lúc**:

```text
Thông thường (4 lệnh cộng):   a0+b0   a1+b1   a2+b2   a3+b3
SIMD (1 lệnh cộng):          [a0 a1 a2 a3] + [b0 b1 b2 b3] = [c0 c1 c2 c3]
```

Rất hiệu quả cho đồ họa, âm thanh, vật lý game, và phép nhân ma trận trong AI. Trình biên dịch thường **tự véc tơ hóa (auto-vectorize)** các vòng lặp đơn giản khi bật tối ưu.

### 13.4 Bức tường năng lượng và CPU đa lõi

Trước khoảng năm 2005, CPU nhanh lên chủ yếu nhờ **tăng tần số**. Nhưng công suất tiêu thụ tăng theo tần số, và chip chạm tới giới hạn tản nhiệt (**power wall**). Từ đó, ngành chuyển sang:

- **Đa lõi (multicore):** nhiều CPU nhỏ trên một chip — nhưng **phần mềm phải được viết để chạy song song** mới tận dụng được (xem định luật Amdahl).
- **Đa luồng đồng thời (SMT, như Hyper-Threading):** một lõi vật lý xuất hiện như hai lõi logic.
- **Lõi lai (hybrid):** kết hợp lõi hiệu năng cao và lõi tiết kiệm điện trên cùng chip.

---

## Bài tập Phần III

**Bài III.1.** Mã hóa sang mã máy (hex): (a) `addi x5, x6, -3`; (b) `sub x7, x5, x6`.

**Bài III.2.** Giải mã lệnh RISC-V `0x00A30333`.

**Bài III.3.** Chương trình có $2 \times 10^9$ lệnh, CPI = 1,5, chạy ở 2,5 GHz. Một thiết kế mới giảm CPI xuống 1,2 nhưng tần số chỉ còn 2,2 GHz. Thiết kế nào nhanh hơn, và nhanh hơn bao nhiêu lần?

**Bài III.4.** Trong một game, việc vẽ hình chiếm 60% thời gian mỗi khung hình. Nếu tăng tốc phần này 3 lần, khung hình nhanh lên bao nhiêu? Tối đa có thể nhanh lên bao nhiêu nếu chỉ tối ưu phần vẽ?

**Bài III.5.** Pipeline 5 tầng có thời gian 250, 350, 150, 300, 200 ps; mỗi thanh ghi pipeline thêm 20 ps. Tính chu kỳ xung khi không và có pipeline, và mức tăng tốc.

**Bài III.6.** 25% lệnh là rẽ nhánh, tỉ lệ đoán sai 5%, mỗi lần đoán sai mất 3 chu kỳ. Tính CPI hiệu dụng (CPI lý tưởng = 1).

**Bài III.7.** Chỉ ra xung đột dữ liệu và cách xử lý trong đoạn:

```asm
add x1, x2, x3
sub x4, x1, x5
and x6, x1, x7
```

**Bài III.8.** Viết hàm RISC-V `max2(a0, a1)` trả về số lớn hơn trong `a0`.

<details>
<summary><b>Đáp án Phần III</b> (bấm để mở)</summary>

**III.1.** (a) `imm = -3 = 111111111101₂`, `rs1 = x6 = 00110`, `funct3 = 000`, `rd = x5 = 00101`, `opcode = 0010011` → **`0xFFD30293`**. (b) `funct7 = 0100000`, `rs2 = x6`, `rs1 = x5`, `rd = x7`, `opcode = 0110011` → **`0x406283B3`**.

**III.2.** `0x00A30333` = `0000000 01010 00110 000 00110 0110011` → R-type, `funct7 = 0`, `funct3 = 0` → `add`; `rs2 = x10 (a0)`, `rs1 = x6 (t1)`, `rd = x6 (t1)` → **`add x6, x6, x10`** (tức `add t1, t1, a0`).

**III.3.** Cũ: $\dfrac{2 \times 10^9 \times 1{,}5}{2{,}5 \times 10^9} = 1{,}2$ s. Mới: $\dfrac{2 \times 10^9 \times 1{,}2}{2{,}2 \times 10^9} \approx 1{,}09$ s. Thiết kế mới nhanh hơn **1,1 lần**.

**III.4.** $\dfrac{1}{0{,}4 + 0{,}6/3} = \dfrac{1}{0{,}6} \approx 1{,}67$ lần. Tối đa: $\dfrac{1}{0{,}4} = 2{,}5$ lần.

**III.5.** Không pipeline: **1250 ps**. Có pipeline: 350 + 20 = **370 ps**. Tăng tốc ≈ $1250 / 370 \approx$ **3,38 lần**.

**III.6.** $1 + 0{,}25 \times 0{,}05 \times 3 = $ **1,0375**.

**III.7.** Cả `sub` và `and` đều dùng `x1` trước khi `add` kịp ghi ở tầng WB. Xử lý bằng **forwarding**: `sub` nhận `x1` từ thanh ghi EX/MEM, `and` nhận từ thanh ghi MEM/WB. Không cần chèn chu kỳ trống vì `add` không phải lệnh đọc bộ nhớ.

**III.8.** (Đã chạy thử trên trình giả lập với các cặp (7, 3), (3, 7), (−5, −2), (4, 4) — đều đúng.)

```asm
max2:
    blt  a0, a1, else     # nếu a0 < a1 thì kết quả là a1
    j    end              # ngược lại a0 đã là kết quả
else:
    mv   a0, a1
end:
    ret
```

Lưu ý: `blt` so sánh **có dấu** (dùng `bltu` cho số không dấu).

</details>

---

# PHẦN IV — BỘ NHỚ

## Chương 14. Phân cấp bộ nhớ

### 14.1 Vấn đề: CPU nhanh, bộ nhớ chậm

Qua nhiều thập kỷ, tốc độ CPU tăng nhanh hơn nhiều so với tốc độ RAM. Nếu mỗi lần đọc dữ liệu CPU đều phải chờ RAM, nó sẽ **ngồi chơi** phần lớn thời gian. Giải pháp: **phân cấp bộ nhớ** — nhiều tầng, tầng trên nhỏ nhưng nhanh, tầng dưới lớn nhưng chậm.

```text
            ▲ nhanh hơn, đắt hơn, nhỏ hơn
            │
        ┌───────┐
        │Thanh  │  vài chục thanh ghi, trong lõi CPU
        │ ghi   │
      ┌─┴───────┴─┐
      │ Cache L1  │  vài chục KB mỗi lõi (SRAM)
     ┌┴───────────┴┐
     │  Cache L2   │  vài trăm KB – vài MB mỗi lõi
    ┌┴─────────────┴┐
    │   Cache L3    │  vài MB – vài chục MB, dùng chung các lõi
   ┌┴───────────────┴┐
   │   RAM (DRAM)    │  vài GB – vài trăm GB
  ┌┴─────────────────┴┐
  │     SSD / HDD     │  hàng trăm GB – vài TB
  └───────────────────┘
            │
            ▼ chậm hơn, rẻ hơn, lớn hơn
```

**Độ trễ truy cập — cỡ độ lớn** (giá trị cụ thể thay đổi nhiều theo phần cứng):

| Tầng | Độ trễ điển hình | Nếu 1 ns được phóng to thành 1 giây |
|---|---|---|
| Cache L1 | ~1 ns | 1 giây |
| Cache L2 | vài ns | vài giây |
| Cache L3 | ~10 ns trở lên | vài chục giây |
| RAM | ~100 ns | gần 2 phút |
| SSD NVMe (đọc ngẫu nhiên) | hàng chục đến hàng trăm µs | vài giờ đến vài ngày |
| HDD (tìm kiếm) | vài ms | vài tháng |

Chênh lệch giữa L1 và RAM là khoảng **100 lần**. Đó là lý do cách bạn **sắp xếp dữ liệu** có thể quan trọng không kém thuật toán.

### 14.2 Nguyên lý cục bộ (locality)

Phân cấp bộ nhớ hiệu quả vì chương trình thường có hai thói quen:

| Loại | Ý nghĩa | Ví dụ |
|---|---|---|
| **Cục bộ thời gian (temporal)** | Dữ liệu **vừa dùng** sẽ sớm được dùng **lại** | Biến đếm vòng lặp, lệnh trong thân vòng lặp |
| **Cục bộ không gian (spatial)** | Dữ liệu **gần** chỗ vừa dùng sẽ sớm được dùng | Duyệt mảng tuần tự, các lệnh liên tiếp |

**Ví dụ đời thường:** bàn học (cache) chỉ để vài cuốn sách đang đọc; kệ sách trong phòng (RAM) chứa nhiều hơn; thư viện thành phố (ổ đĩa) có mọi thứ nhưng đi lại rất lâu. Khi lấy một cuốn từ kệ, bạn lấy luôn **cả tập** bên cạnh vì có thể sắp cần (cục bộ không gian).

---

## Chương 15. Bộ nhớ đệm (cache)

### 15.1 Khái niệm cơ bản

- Dữ liệu được chép giữa RAM và cache theo từng **khối (block)** kích thước cố định (thường 64 byte, gọi là *cache line*).
- **Trúng (hit):** dữ liệu cần có sẵn trong cache. **Trượt (miss):** không có → phải lấy từ tầng dưới rồi chép vào cache.
- **Tỉ lệ trúng** = số lần trúng / tổng số lần truy cập. **Tỉ lệ trượt** = 1 − tỉ lệ trúng.

### 15.2 Cache ánh xạ trực tiếp (direct-mapped)

Mỗi khối dữ liệu chỉ được đặt ở **đúng một** vị trí trong cache. Địa chỉ bộ nhớ được chia thành ba phần:

```text
┌──────────────────┬───────────────┬──────────────┐
│       TAG        │     INDEX     │    OFFSET    │
└──────────────────┴───────────────┴──────────────┘
  để kiểm tra đúng    chọn dòng nào   vị trí byte
  khối không          trong cache     trong khối
```

- **Offset:** $\log_2(\text{kích thước khối})$ bit.
- **Index:** $\log_2(\text{số dòng})$ bit.
- **Tag:** phần còn lại.

Mỗi dòng cache lưu: **bit hợp lệ (valid)** + **tag** + **dữ liệu của khối**.

```text
 Địa chỉ ──► [ tag | index | offset ]
                │      │
                │      └──► chọn dòng
                ▼
          ┌───┬───────┬──────────────────┐
          │ V │  Tag  │   Dữ liệu khối   │
          ├───┼───────┼──────────────────┤
 dòng i ► │ 1 │ 48D2  │ ................ │──► lấy byte theo offset
          └───┴───────┴──────────────────┘
                │
     so sánh ───┘  V == 1 và tag khớp  →  TRÚNG
```

**Ví dụ 15.1.** Địa chỉ 32 bit; cache ánh xạ trực tiếp có 1024 dòng, mỗi khối 16 byte. Tách địa chỉ `0x1234ABCD`.

*Lời giải.* Offset $= \log_2 16 = 4$ bit; index $= \log_2 1024 = 10$ bit; tag $= 32 - 10 - 4 = 18$ bit.

- Offset = 4 bit thấp nhất = `0xD`
- Index = 10 bit kế tiếp = `(0x1234ABCD >> 4) & 0x3FF` = `0x2BC` (dòng 700)
- Tag = 18 bit cao = `0x1234ABCD >> 14` = `0x48D2`

**Ví dụ 15.2 (cache thật).** Cache L1 dữ liệu 32 KiB, khối 64 byte, **8 đường** (xem 15.3), địa chỉ 48 bit. Số dòng $= 32\,768 / 64 = 512$; số tập $= 512 / 8 = 64$. Vậy offset 6 bit, index 6 bit, tag $48 - 12 = 36$ bit.

### 15.3 Cache kết hợp theo tập và kết hợp toàn phần

Cache ánh xạ trực tiếp đơn giản nhưng hay **xung đột**: hai khối cùng index sẽ đẩy nhau ra liên tục.

| Loại | Một khối có thể nằm ở | Ưu | Nhược |
|---|---|---|---|
| **Ánh xạ trực tiếp** | Đúng 1 dòng | Nhanh, rẻ | Nhiều xung đột |
| **Kết hợp theo tập N đường (N-way set associative)** | Bất kỳ dòng nào trong **1 tập** gồm N dòng | Cân bằng — **phổ biến nhất** | Phải so sánh N tag |
| **Kết hợp toàn phần (fully associative)** | Bất kỳ dòng nào | Ít trượt nhất | Đắt, chậm khi cache lớn |

Khi tập đã đầy, phải chọn khối để **thay thế**. Chiến lược phổ biến: **LRU** (Least Recently Used) — loại khối **lâu nhất chưa được dùng**.

**Ví dụ 15.3.** Cache có 4 khối. Dãy truy cập các **số hiệu khối**: `0, 8, 0, 6, 8`. So sánh ba cách tổ chức.

**Ánh xạ trực tiếp** (dòng = số hiệu mod 4):

| Truy cập | Dòng | Kết quả | Dòng 0 | Dòng 1 | Dòng 2 | Dòng 3 |
|---|---|---|---|---|---|---|
| 0 | 0 | Trượt | **0** | | | |
| 8 | 0 | Trượt | **8** | | | |
| 0 | 0 | Trượt | **0** | | | |
| 6 | 2 | Trượt | 0 | | **6** | |
| 8 | 0 | Trượt | **8** | | 6 | |

→ **5 lần trượt**. Khối 0 và 8 cùng rơi vào dòng 0 và đẩy nhau ra liên tục.

**Kết hợp 2 đường** (2 tập, tập = số hiệu mod 2, thay thế theo LRU):

| Truy cập | Tập | Kết quả | Tập 0 (sau khi truy cập) |
|---|---|---|---|
| 0 | 0 | Trượt | 0 |
| 8 | 0 | Trượt | 0, 8 |
| 0 | 0 | **Trúng** | 8, 0 *(0 vừa dùng)* |
| 6 | 0 | Trượt — loại 8 (lâu nhất chưa dùng) | 0, 6 |
| 8 | 0 | Trượt — loại 0 | 6, 8 |

→ **4 lần trượt**.

**Kết hợp toàn phần** (4 khối, còn chỗ trống nên không phải loại ai):

| Truy cập | 0 | 8 | 0 | 6 | 8 |
|---|---|---|---|---|---|
| Kết quả | Trượt | Trượt | **Trúng** | Trượt | **Trúng** |

→ **3 lần trượt**.

**Ba nguyên nhân trượt (3C):**

| Loại | Nguyên nhân | Giảm bằng cách |
|---|---|---|
| **Compulsory / Cold** (bắt buộc) | Lần đầu truy cập | Không tránh được (có thể nạp trước — prefetch) |
| **Capacity** (dung lượng) | Cache quá nhỏ | Cache lớn hơn |
| **Conflict** (xung đột) | Nhiều khối tranh cùng vị trí | Tăng độ kết hợp |

### 15.4 Thời gian truy cập trung bình (AMAT)

$$\boxed{\text{AMAT} = \text{thời gian trúng} + \text{tỉ lệ trượt} \times \text{chi phí trượt}}$$

**Ví dụ 15.4.** L1 truy cập 1 ns, tỉ lệ trượt 5%, mỗi lần trượt tốn thêm 20 ns:
$\text{AMAT} = 1 + 0{,}05 \times 20 = 2$ ns.

**Nhiều tầng cache:** chi phí trượt của tầng này chính là AMAT của tầng dưới.

**Ví dụ 15.5.** L1: 1 ns, trượt 5%. L2: 10 ns, trượt 20% (trong số các lần đến L2). RAM: 100 ns.

$$\text{AMAT} = 1 + 0{,}05 \times (10 + 0{,}2 \times 100) = 1 + 0{,}05 \times 30 = 2{,}5 \text{ ns}$$

Nhờ cache, thời gian trung bình **gần với tốc độ của L1** (2,5 ns) thay vì 100 ns của RAM.

**Ghi dữ liệu:** khi ghi, cache có thể ghi ngay xuống bộ nhớ chính (**write-through**) hoặc chỉ đánh dấu khối là "bẩn" (**dirty bit**) và ghi sau khi khối bị loại (**write-back**).

### 15.5 Viết code thân thiện với cache

**Thí nghiệm:** cộng mọi phần tử của ma trận 4096 × 4096, duyệt theo hàng và theo cột:

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 4096

static double now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(void) {
    // Ma trận N x N lưu liên tục theo HÀNG (row-major), như mảng 2 chiều trong C
    int *m = malloc((size_t)N * N * sizeof *m);
    if (m == NULL) return 1;
    for (size_t i = 0; i < (size_t)N * N; i++) m[i] = (int)(i % 7);

    long long sum = 0;
    double t = now();
    for (int i = 0; i < N; i++)              // duyệt theo hàng: đi tuần tự trong bộ nhớ
        for (int j = 0; j < N; j++)
            sum += m[(size_t)i * N + j];
    double row_time = now() - t;

    long long sum2 = 0;
    t = now();
    for (int j = 0; j < N; j++)              // duyệt theo cột: nhảy N phần tử mỗi bước
        for (int i = 0; i < N; i++)
            sum2 += m[(size_t)i * N + j];
    double col_time = now() - t;

    printf("Tong giong nhau: %s\n", sum == sum2 ? "co" : "khong");
    printf("Duyet theo hang: %.3f s\n", row_time);
    printf("Duyet theo cot : %.3f s\n", col_time);
    printf("Cham hon       : %.1f lan\n", col_time / row_time);
    free(m);
    return 0;
}
```

**Ví dụ kết quả** khi biên dịch với `gcc -O1 locality.c -o locality` (đo trên máy thử nghiệm; con số trên máy bạn sẽ khác):

```text
Tong giong nhau: co
Duyet theo hang: 0.011 s
Duyet theo cot : 0.115 s
Cham hon       : 10.0 lan
```

**Giải thích:** mảng hai chiều trong C được lưu **theo hàng** liên tiếp trong bộ nhớ. Duyệt theo hàng đọc các byte **liền kề** → mỗi lần nạp một khối 64 byte vào cache, 16 phần tử kế tiếp đều trúng; bộ nạp trước (prefetcher) của CPU cũng dễ dàng đoán được mẫu này. Duyệt theo cột nhảy **4096 phần tử** (16 KiB) mỗi bước → gần như lần nào cũng trượt.

**Quy tắc thực hành:**

1. Duyệt mảng nhiều chiều **theo đúng thứ tự lưu trữ** (C/C++: hàng trước, cột sau; Fortran/MATLAB thì ngược lại).
2. Ưu tiên **mảng liên tục** (`std::vector`) hơn **danh sách liên kết** (`std::list`) — các nút của danh sách nằm rải rác, mỗi bước là một lần trượt tiềm năng.
3. Giữ dữ liệu dùng cùng nhau **gần nhau**; giữ struct **nhỏ gọn** (sắp xếp trường để giảm padding).
4. Xử lý dữ liệu **theo lô**: làm một việc cho mọi phần tử, rồi mới sang việc tiếp theo.

> 🎮 **Liên hệ game — thiết kế hướng dữ liệu (data-oriented design):**
>
> ```cpp
> // Mảng các struct (AoS) — trực quan kiểu hướng đối tượng
> struct Particle { Vec3 pos; Vec3 vel; Color color; float life; char name[32]; };
> std::vector<Particle> particles;
>
> // Struct của các mảng (SoA) — thân thiện với cache
> struct Particles {
>     std::vector<Vec3>  pos;
>     std::vector<Vec3>  vel;
>     std::vector<Color> color;
>     std::vector<float> life;
> };
> ```
>
> Vòng lặp cập nhật vật lý chỉ cần `pos` và `vel`. Với AoS, mỗi khối cache nạp vào còn chứa `color`, `name`... không cần thiết, nên mỗi khối chứa được ít hạt hơn. Với SoA, cache chỉ chứa đúng dữ liệu cần, lại xếp liên tục nên dễ dàng **véc tơ hóa bằng SIMD**. Đây là tư tưởng nền tảng của kiến trúc **ECS** trong nhiều engine game hiện đại.

---

## Chương 16. Bộ nhớ ảo

### 16.1 Vì sao cần bộ nhớ ảo?

Mỗi chương trình (tiến trình) được **"tưởng"** rằng nó sở hữu một không gian địa chỉ riêng, rất lớn, bắt đầu từ 0. Hệ điều hành và phần cứng (**MMU** — Memory Management Unit) **dịch** địa chỉ ảo này sang địa chỉ vật lý trong RAM.

| Lợi ích | Giải thích |
|---|---|
| **Cô lập** | Tiến trình này không đọc/ghi được bộ nhớ của tiến trình khác |
| **Dùng nhiều hơn RAM thật** | Phần ít dùng được đẩy tạm xuống đĩa (swap) |
| **Đơn giản hóa** | Mọi chương trình đều có bố cục bộ nhớ giống nhau |
| **Chia sẻ** | Thư viện dùng chung (như libc) chỉ nằm một lần trong RAM, được "ánh xạ" vào nhiều tiến trình |
| **Bảo vệ** | Đánh dấu vùng chỉ đọc, không thực thi được... |

### 16.2 Trang và bảng trang

- Bộ nhớ ảo chia thành các **trang (page)**; bộ nhớ vật lý chia thành các **khung (frame)** cùng kích thước — thường **4 KiB**.
- Địa chỉ ảo = **số hiệu trang ảo (VPN)** + **offset**.
- **Bảng trang (page table)** của mỗi tiến trình ghi: trang ảo nào ↔ khung vật lý nào, kèm các bit: **hợp lệ (valid)**, **quyền (đọc/ghi/thực thi)**, **đã truy cập**, **đã sửa (dirty)**.

```text
 Địa chỉ ảo:     [        VPN        |  offset (12 bit) ]
                          │                  │
                          ▼                  │
                 ┌─────────────────┐         │
                 │   Bảng trang    │         │
                 │  VPN → PPN      │         │
                 └────────┬────────┘         │
                          ▼                  ▼
 Địa chỉ vật lý: [        PPN        |  offset (giữ nguyên) ]
```

**Ví dụ 16.1.** Địa chỉ ảo 32 bit, trang 4 KiB. Bảng trang cho biết trang ảo `0x00403` nằm ở khung vật lý `0x001F2`. Dịch địa chỉ ảo `0x00403A7C`.

*Lời giải.* Trang 4 KiB = $2^{12}$ byte → offset là **12 bit thấp** = 3 chữ số hex cuối.

- VPN = `0x00403`, offset = `0xA7C`
- Tra bảng: PPN = `0x001F2`
- Địa chỉ vật lý = PPN nối với offset = **`0x001F2A7C`**

### 16.3 Lỗi trang (page fault)

Khi truy cập một trang **không có trong RAM**, CPU phát sinh **lỗi trang** và chuyển quyền cho hệ điều hành:

1. Nếu trang hợp lệ nhưng đang nằm trên đĩa (hoặc chưa được nạp): hệ điều hành tìm một khung trống (có thể phải đẩy một trang khác xuống đĩa), nạp trang vào, cập nhật bảng trang, rồi **chạy lại** lệnh vừa lỗi.
2. Nếu địa chỉ không hợp lệ hoặc sai quyền: hệ điều hành **dừng chương trình** — chính là lỗi **Segmentation fault** (Linux) hay **Access violation** (Windows) mà bạn gặp khi dùng con trỏ sai.

Vì đọc đĩa chậm hơn RAM hàng nghìn đến hàng triệu lần, khi RAM không đủ và hệ thống liên tục đẩy trang ra vào (**thrashing**), máy sẽ chậm hẳn đi.

### 16.4 TLB — cache cho bảng trang

Mỗi lần truy cập bộ nhớ, nếu phải đọc bảng trang (cũng nằm trong RAM) thì sẽ tốn **gấp đôi** thời gian. **TLB** (Translation Lookaside Buffer) là một cache nhỏ, rất nhanh trong CPU, lưu các cặp VPN → PPN vừa dùng.

**Ví dụ 16.2.** Tra TLB mất 1 ns, truy cập RAM mất 100 ns, TLB trúng 98%, bảng trang một tầng:

$$0{,}98 \times (1 + 100) + 0{,}02 \times (1 + 100 + 100) = 103 \text{ ns}$$

Rất gần với 101 ns lý tưởng — nhờ cục bộ, TLB hiếm khi trượt.

### 16.5 Bảng trang nhiều tầng

Bảng trang một tầng cho không gian 32 bit, trang 4 KiB, mỗi mục 4 byte cần $2^{20} \times 4 = 4$ MiB **cho mỗi tiến trình**. Với không gian 48 bit và mục 8 byte, con số này là $2^{36} \times 8 = 512$ GiB — bất khả thi.

Giải pháp: **bảng trang nhiều tầng** (như cây), chỉ tạo những nhánh thực sự dùng. x86-64 dùng **4 tầng** với địa chỉ ảo 48 bit:

```text
 47      39 38      30 29      21 20      12 11          0
┌──────────┬──────────┬──────────┬──────────┬─────────────┐
│ tầng 4   │ tầng 3   │ tầng 2   │ tầng 1   │   offset    │
│  9 bit   │  9 bit   │  9 bit   │  9 bit   │   12 bit    │
└──────────┴──────────┴──────────┴──────────┴─────────────┘
```

(Các CPU mới hỗ trợ thêm tầng thứ 5 cho không gian 57 bit.) Hệ điều hành cũng hỗ trợ **trang lớn** (2 MiB, 1 GiB) để giảm số lần trượt TLB cho các chương trình dùng rất nhiều bộ nhớ.

### 16.6 Bố cục bộ nhớ của một tiến trình

```text
 địa chỉ cao ┌──────────────────────┐
             │ Kernel (không truy   │
             │ cập được từ user)    │
             ├──────────────────────┤
             │ Stack  ▼ mọc xuống   │  biến cục bộ, khung hàm
             │                      │
             │ ...  (thư viện dùng  │
             │      chung, mmap)    │
             │                      │
             │ Heap   ▲ mọc lên     │  malloc / new
             ├──────────────────────┤
             │ .bss                 │  biến toàn cục chưa khởi tạo
             │ .data                │  biến toàn cục đã khởi tạo
             │ .rodata              │  hằng số, chuỗi hằng
             │ .text                │  mã lệnh
 địa chỉ thấp└──────────────────────┘
```

**Thí nghiệm:**

```c
#include <stdio.h>
#include <stdlib.h>

int global_init = 42;          // vùng dữ liệu (.data)
int global_zero;               // vùng .bss

int main(void) {
    const char *msg = "hello";                  // chuỗi hằng (.rodata)
    int local = 1;                              // stack
    int *heap = malloc(sizeof *heap);           // heap
    if (heap == NULL) return 1;

    printf("rodata (chuoi hang) : %p\n", (void *)msg);
    printf("data  (global_init) : %p\n", (void *)&global_init);
    printf("bss   (global_zero) : %p\n", (void *)&global_zero);
    printf("heap  (malloc)      : %p\n", (void *)heap);
    printf("stack (local)       : %p\n", (void *)&local);
    free(heap);
    return 0;
}
```

**Ví dụ kết quả** (biên dịch với `gcc layout.c -o layout`; địa chỉ thay đổi **mỗi lần chạy** do cơ chế ngẫu nhiên hóa bố cục bộ nhớ **ASLR** — một biện pháp bảo mật):

```text
rodata (chuoi hang) : 0x55aae153c004
data  (global_init) : 0x55aae153e010
bss   (global_zero) : 0x55aae153e018
heap  (malloc)      : 0x55ab1f5572a0
stack (local)       : 0x7ffe26604bf4
```

Thứ tự tăng dần của địa chỉ khớp với sơ đồ: `.rodata` < `.data` < `.bss` < heap < stack.

---

## Bài tập Phần IV

**Bài IV.1.** Địa chỉ 32 bit, cache ánh xạ trực tiếp 256 dòng, khối 32 byte. Tính số bit tag/index/offset, và tách địa chỉ `0x0000ABCD`.

**Bài IV.2.** Cache 8 khối. Dãy số hiệu khối truy cập: `3, 11, 3, 19, 11, 3, 4, 12`. Đếm số lần trượt với: (a) ánh xạ trực tiếp; (b) kết hợp 2 đường (4 tập, LRU); (c) kết hợp toàn phần.

**Bài IV.3.** L1 truy cập 2 ns, tỉ lệ trượt 4%, chi phí trượt 80 ns. Tính AMAT.

**Bài IV.4.** Trang 4 KiB. Trang ảo `0x12` nằm ở khung `0x7`. Dịch địa chỉ ảo `0x12345`.

**Bài IV.5.** Không gian địa chỉ 32 bit, trang 8 KiB, mỗi mục bảng trang 4 byte. Bảng trang một tầng lớn bao nhiêu?

**Bài IV.6.** Vì sao đoạn code dưới đây chậm, và sửa thế nào?

```c
for (int j = 0; j < COLS; j++)
    for (int i = 0; i < ROWS; i++)
        img[i][j] = img[i][j] / 2;
```

<details>
<summary><b>Đáp án Phần IV</b> (bấm để mở)</summary>

**IV.1.** Offset $= \log_2 32 = 5$ bit; index $= \log_2 256 = 8$ bit; tag $= 32 - 13 = 19$ bit. Với `0x0000ABCD`: offset = `0xD`, index = `(0xABCD >> 5) & 0xFF` = `0x5E`, tag = `0xABCD >> 13` = `0x5`.

**IV.2.**

(a) Ánh xạ trực tiếp (dòng = số hiệu mod 8): 3, 11, 19 cùng rơi vào **dòng 3**; 4 và 12 cùng rơi vào **dòng 4** → lần nào cũng trượt: **8 lần trượt**.

(b) Kết hợp 2 đường (tập = số hiệu mod 4): 3, 11, 19 cùng tập 3; 4, 12 cùng tập 0.

| Truy cập | 3 | 11 | 3 | 19 | 11 | 3 | 4 | 12 |
|---|---|---|---|---|---|---|---|---|
| Kết quả | T | T | **Trúng** | T (loại 11) | T (loại 3) | T (loại 19) | T | T |

(T = trượt) → **7 lần trượt**.

(c) Kết hợp toàn phần (8 khối, không phải loại ai): chỉ trượt ở lần đầu của mỗi khối khác nhau — 3, 11, 19, 4, 12 → **5 lần trượt**.

**IV.3.** $2 + 0{,}04 \times 80 = $ **5,2 ns**.

**IV.4.** VPN = `0x12`, offset = `0x345` → địa chỉ vật lý **`0x7345`**.

**IV.5.** Offset 13 bit → $2^{19}$ trang → $2^{19} \times 4 = $ **2 MiB**.

**IV.6.** Vòng lặp duyệt **theo cột** trong khi mảng C lưu **theo hàng** → truy cập bộ nhớ nhảy cóc, trượt cache nhiều. Sửa: **đổi thứ tự hai vòng lặp** (vòng ngoài `i`, vòng trong `j`).

</details>

---

# PHẦN V — HỆ THỐNG

## Chương 17. Vào/ra và lưu trữ

### 17.1 CPU giao tiếp với thiết bị thế nào?

Mỗi thiết bị có một **bộ điều khiển (controller)** với các **thanh ghi** (trạng thái, lệnh, dữ liệu). CPU đọc/ghi các thanh ghi này qua:

- **Ánh xạ bộ nhớ (memory-mapped I/O):** thanh ghi thiết bị được gán vào **các địa chỉ bộ nhớ**; đọc/ghi địa chỉ đó chính là nói chuyện với thiết bị. Cách phổ biến nhất (RISC-V, ARM chỉ dùng cách này).
- **Cổng I/O riêng (port-mapped I/O):** dùng lệnh riêng (`in`, `out` trên x86).

### 17.2 Ba cách trao đổi dữ liệu

| Cách | Cơ chế | Ưu | Nhược | Ví dụ |
|---|---|---|---|---|
| **Hỏi vòng (polling)** | CPU liên tục kiểm tra "xong chưa?" | Đơn giản, độ trễ đoán trước được | Lãng phí CPU khi thiết bị ít việc | Vi điều khiển đơn giản; card mạng tốc độ rất cao |
| **Ngắt (interrupt)** | Thiết bị **báo** CPU khi có việc; CPU tạm dừng, chạy **trình phục vụ ngắt**, rồi quay lại | CPU rảnh tay làm việc khác | Mỗi ngắt có chi phí chuyển ngữ cảnh | Bàn phím, chuột, bộ định thời |
| **DMA** (Direct Memory Access) | Thiết bị **tự chép** khối dữ liệu lớn vào/ra RAM, chỉ báo CPU khi xong | CPU không phải chép từng byte | Phức tạp hơn | Ổ đĩa, card mạng, GPU |

**Quy trình xử lý ngắt:**

```text
Chương trình đang chạy ──► [thiết bị phát ngắt]
                              │
                              ▼
              CPU lưu PC và trạng thái hiện tại
                              │
                              ▼
              Nhảy tới trình phục vụ ngắt (ISR) theo bảng vector ngắt
                              │
                              ▼
              ISR xử lý (đọc dữ liệu, báo hệ điều hành...)
                              │
                              ▼
              Khôi phục trạng thái, quay lại chương trình
```

### 17.3 Bus

- **PCI Express (PCIe):** bus tốc độ cao nối CPU với GPU, SSD NVMe, card mạng. Gồm nhiều **làn (lane)** nối tiếp; khe x16 cho GPU, x4 cho SSD NVMe.
- **USB:** thiết bị ngoại vi.
- **SATA:** ổ đĩa đời cũ và SSD SATA (giới hạn khoảng 600 MB/s bởi chính giao tiếp SATA III).

### 17.4 Ổ cứng HDD và SSD

**HDD** lưu dữ liệu trên đĩa từ quay. Thời gian truy cập một khối:

$$\text{thời gian} = \text{tìm rãnh (seek)} + \text{chờ đĩa quay tới (rotational latency)} + \text{truyền dữ liệu}$$

**Ví dụ 17.1.** Đĩa quay 7200 vòng/phút. Độ trễ quay **trung bình** (nửa vòng):

$$\frac{1}{2} \times \frac{60}{7200} \text{ s} \approx 4{,}17 \text{ ms}$$

Cộng thêm vài ms tìm rãnh → mỗi lần đọc ngẫu nhiên mất cỡ **5–10 ms**. Đọc **tuần tự** thì nhanh hơn nhiều vì không phải di chuyển đầu đọc.

**SSD** lưu dữ liệu trên chip nhớ **flash**, không có bộ phận chuyển động:

| | HDD | SSD |
|---|---|---|
| Đọc ngẫu nhiên | Chậm (ms) | Nhanh (µs) |
| Chống sốc | Kém | Tốt |
| Tuổi thọ | Hỏng cơ khí | Mỗi ô nhớ chỉ ghi/xóa được **số lần giới hạn** → bộ điều khiển dùng **cân bằng hao mòn (wear leveling)** |
| Đặc thù | | Xóa theo **khối lớn**, ghi theo **trang** → cần lệnh **TRIM** và thu gom rác |
| Giá / GB | Rẻ | Đắt hơn |

**NVMe** là giao thức dành riêng cho SSD chạy trên PCIe, có nhiều hàng đợi song song — nhanh hơn nhiều so với SSD qua SATA.

**RAID** — kết hợp nhiều ổ đĩa:

| Loại | Cách làm | Mục tiêu |
|---|---|---|
| RAID 0 | Chia dữ liệu ra nhiều ổ (striping) | Tốc độ; hỏng 1 ổ là **mất hết** |
| RAID 1 | Sao chép y hệt sang ổ khác (mirroring) | An toàn |
| RAID 5 | Chia dữ liệu + **bit chẵn lẻ (parity)**, cần ≥ 3 ổ | Cân bằng; chịu được hỏng 1 ổ |

> ⚠️ RAID **không phải là sao lưu**: xóa nhầm hay mã độc mã hóa dữ liệu sẽ ảnh hưởng đến mọi ổ cùng lúc.

---

## Chương 18. Xử lý song song và GPU

### 18.1 Phân loại Flynn

| Loại | Lệnh | Dữ liệu | Ví dụ |
|---|---|---|---|
| **SISD** | 1 | 1 | CPU một lõi cổ điển |
| **SIMD** | 1 | Nhiều | Lệnh SSE/AVX/NEON; GPU |
| **MISD** | Nhiều | 1 | Hiếm gặp |
| **MIMD** | Nhiều | Nhiều | CPU đa lõi, cụm máy chủ |

### 18.2 Đa lõi và bộ nhớ dùng chung

Các lõi trong CPU **dùng chung RAM** (và cache L3). Lập trình đa luồng (như `std::thread` trong C++) gặp các vấn đề:

- **Data race:** nhiều luồng ghi cùng dữ liệu → cần mutex hoặc biến atomic.
- **Chi phí đồng bộ:** khóa (lock) làm các luồng phải chờ nhau.
- **Chia sẻ giả (false sharing)** — vấn đề thuần túy do **kiến trúc cache**:

```cpp
struct Counters {
    long a;   // luồng 1 liên tục tăng a
    long b;   // luồng 2 liên tục tăng b
};            // a và b nằm CHUNG một khối cache 64 byte
```

Hai luồng sửa hai biến **khác nhau**, không cần khóa — nhưng vì chung một khối cache, mỗi lần một lõi ghi, khối cache tương ứng trên lõi kia bị **vô hiệu hóa** và phải nạp lại. Kết quả: chương trình đa luồng có thể **chậm hơn** chạy một luồng.

**Cách sửa:** đặt các biến được ghi bởi các luồng khác nhau vào **các khối cache khác nhau**:

```cpp
struct Counters {
    alignas(64) long a;   // mỗi biến bắt đầu một khối 64 byte mới
    alignas(64) long b;
};
```

(C++17 có hằng `std::hardware_destructive_interference_size` cho con số này.) Cách tốt hơn nữa: mỗi luồng dùng **biến cục bộ riêng**, chỉ gộp kết quả ở cuối.

### 18.3 Định luật Amdahl cho song song

Với $p$ là tỉ lệ chạy song song được và $N$ lõi:

$$\text{Tăng tốc} = \frac{1}{(1 - p) + \dfrac{p}{N}}$$

**Ví dụ 18.1.** $p = 95\%$, $N = 8$: tăng tốc $\approx 5{,}9$ lần. Dù có vô hạn lõi, tối đa chỉ **20 lần**. Phần tuần tự nhỏ (5%) lại là "nút cổ chai".

**Định luật Gustafson** nhìn theo hướng lạc quan hơn: khi có nhiều lõi hơn, ta thường **giải bài toán lớn hơn** (độ phân giải cao hơn, nhiều nhân vật hơn), và khi đó phần song song chiếm tỉ trọng ngày càng lớn.

### 18.4 GPU

| | CPU | GPU |
|---|---|---|
| Số lõi | Vài đến vài chục lõi **mạnh** | Hàng nghìn lõi **đơn giản** |
| Tối ưu cho | Logic phức tạp, rẽ nhánh nhiều, độ trễ thấp | **Cùng một phép tính** trên **rất nhiều dữ liệu** |
| Bộ nhớ | RAM hệ thống | VRAM riêng, băng thông rất cao |
| Ví dụ | Logic game, AI của NPC, hệ điều hành | Tô màu hàng triệu điểm ảnh, nhân ma trận |

**Ứng dụng:**

- **Đồ họa:** mỗi điểm ảnh, mỗi đỉnh tam giác được xử lý độc lập bởi các chương trình nhỏ gọi là **shader**.
- **AI:** huấn luyện và chạy mạng nơ-ron chủ yếu là **nhân ma trận khổng lồ** — hoàn toàn phù hợp với GPU. Các nền tảng như CUDA, ROCm, hay API đồ họa hiện đại (Vulkan, DirectX 12, Metal) cho phép lập trình GPU.
- **Chi phí truyền dữ liệu:** chép dữ liệu giữa RAM và VRAM qua PCIe khá tốn kém → nên gửi dữ liệu **theo lô lớn**, hạn chế qua lại nhiều lần.

---

## Chương 19. Tổng hợp: chuyện gì xảy ra khi bạn chạy một chương trình?

Chương này nối **mọi thứ** trong tài liệu thành một câu chuyện. Bạn gõ `./game` trong terminal.

**① Nạp chương trình (Ch.16):** hệ điều hành tạo **tiến trình** mới với **không gian địa chỉ ảo** riêng, đọc file thực thi (ELF trên Linux, PE trên Windows) và **ánh xạ** các đoạn `.text`, `.rodata`, `.data` vào không gian đó — **chưa** thật sự chép gì vào RAM (**nạp theo yêu cầu — demand paging**). Thư viện dùng chung (libc...) cũng được ánh xạ vào.

**② Lệnh đầu tiên:** thanh ghi PC được đặt vào điểm bắt đầu của chương trình. CPU tra **TLB** để dịch địa chỉ → **trượt** → tra bảng trang → trang chưa có trong RAM → **lỗi trang** → hệ điều hành nạp trang mã lệnh từ SSD vào RAM → chạy lại lệnh.

**③ Chu trình lệnh (Ch.8, 12, 13):** CPU lấy lệnh từ **cache L1 lệnh** (lần đầu trượt, nạp từ RAM), giải mã thành các vi lệnh, **dự đoán rẽ nhánh**, thực thi **không theo thứ tự** trên nhiều đường song song trong pipeline dài.

**④ Dữ liệu (Ch.2, 3, 15):** các biến `int`, `float` nằm trong thanh ghi hoặc trong cache L1/L2/L3. Vòng lặp duyệt mảng được hưởng lợi từ **cục bộ không gian**; vòng lặp tính vật lý có thể được véc tơ hóa bằng **SIMD** (Ch.13).

**⑤ Gọi hệ thống:** khi cần đọc file, vẽ lên màn hình hay gửi dữ liệu mạng, chương trình thực hiện **system call** — CPU chuyển từ **chế độ người dùng (user mode)** sang **chế độ nhân (kernel mode)** có quyền cao hơn để hệ điều hành làm việc với thiết bị.

**⑥ Vào/ra (Ch.17):** đọc file ảnh từ SSD NVMe bằng **DMA**; khi xong, SSD **ngắt** CPU. Bàn phím, chuột gửi ngắt mỗi khi có thao tác.

**⑦ Đồ họa (Ch.18):** chương trình gửi dữ liệu mô hình và lệnh vẽ qua **PCIe** sang **GPU**; hàng nghìn lõi GPU tô màu song song hàng triệu điểm ảnh mỗi khung hình.

**⑧ Đa nhiệm:** bộ định thời **ngắt** CPU định kỳ (cỡ mili giây), hệ điều hành có thể chuyển sang chạy tiến trình khác — mỗi tiến trình vẫn "tưởng" mình độc chiếm máy nhờ bộ nhớ ảo.

**⑨ Kết thúc:** `main` trả về → system call `exit` → hệ điều hành thu hồi toàn bộ trang nhớ, đóng file, xóa tiến trình.

```text
  ./game
    │
    ▼
 [Hệ điều hành] ── tạo tiến trình, ánh xạ file thực thi vào bộ nhớ ảo
    │
    ▼
 [CPU] ── PC → TLB → bảng trang → (lỗi trang → nạp từ SSD) → cache L1 → giải mã → pipeline
    │           ▲                                                    │
    │           └─────────────── dữ liệu: L1 ↔ L2 ↔ L3 ↔ RAM ────────┘
    │
    ├── system call ──► kernel ──► driver ──► thiết bị (SSD, mạng) ──► DMA + ngắt
    │
    └── lệnh vẽ ──► PCIe ──► GPU (hàng nghìn lõi) ──► màn hình
```

---

## Bài tập Phần V

**Bài V.1.** Tính độ trễ quay trung bình của đĩa 5400 vòng/phút.

**Bài V.2.** Một chương trình có 90% khối lượng chạy song song được. Tăng tốc bao nhiêu trên 16 lõi? Tối đa bao nhiêu?

**Bài V.3.** Với bàn phím, nên dùng hỏi vòng hay ngắt? Với việc chép một file 10 GB từ SSD vào RAM, cách nào phù hợp?

**Bài V.4.** Hai luồng lần lượt tăng `stats.hits` và `stats.misses` (hai trường liền nhau của một struct) hàng trăm triệu lần. Chạy hai luồng lại chậm hơn một luồng. Giải thích và đề xuất hai cách sửa.

**Bài V.5.** Nhiệm vụ nào phù hợp với GPU hơn: (a) tìm đường cho 1 nhân vật trên bản đồ phức tạp; (b) làm mờ (blur) một bức ảnh 4K; (c) nhân hai ma trận 4096 × 4096; (d) phân tích cú pháp một file JSON?

<details>
<summary><b>Đáp án Phần V</b> (bấm để mở)</summary>

**V.1.** $\dfrac{1}{2} \times \dfrac{60}{5400}$ s ≈ **5,56 ms**.

**V.2.** $\dfrac{1}{0{,}1 + 0{,}9/16} = $ **6,4 lần**. Tối đa $\dfrac{1}{0{,}1} = $ **10 lần**.

**V.3.** Bàn phím: **ngắt** — phím được nhấn không thường xuyên, hỏi vòng sẽ lãng phí CPU. Chép file lớn: **DMA** — bộ điều khiển tự chép cả khối dữ liệu vào RAM, chỉ ngắt CPU khi hoàn tất.

**V.4.** **Chia sẻ giả (false sharing)**: hai trường nằm chung một khối cache 64 byte, nên mỗi lần một luồng ghi, khối cache của lõi kia bị vô hiệu hóa. Cách sửa: (1) tách hai trường sang hai khối cache khác nhau bằng `alignas(64)`; (2) mỗi luồng đếm vào **biến cục bộ riêng**, cuối cùng mới cộng vào struct chung.

**V.5.** Phù hợp với GPU: **(b)** và **(c)** — cùng một phép tính lặp lại trên rất nhiều dữ liệu độc lập. (a) và (d) có nhiều rẽ nhánh, xử lý tuần tự, phụ thuộc lẫn nhau → phù hợp với CPU.

</details>

---

# PHỤ LỤC

## Phụ lục A. Bảng công thức

| Chủ đề | Công thức |
|---|---|
| Số giá trị của $n$ bit | $2^n$ |
| Khoảng giá trị bù 2, $n$ bit | $-2^{n-1}$ đến $2^{n-1} - 1$ |
| Số đối trong bù 2 | $-x = \sim x + 1$ |
| Số thực IEEE 754 | $(-1)^s \times 1{,}f \times 2^{E - \text{bias}}$ (bias 127 cho `float`, 1023 cho `double`) |
| Thời gian CPU | $\text{IC} \times \text{CPI} \times T = \dfrac{\text{IC} \times \text{CPI}}{f}$ |
| CPI trung bình | $\sum \text{tỉ lệ}_i \times \text{CPI}_i$ |
| Định luật Amdahl | $\dfrac{1}{(1 - p) + p / s}$; tối đa $\dfrac{1}{1 - p}$ |
| Chu kỳ pipeline | tầng chậm nhất + độ trễ thanh ghi |
| Thời gian pipeline ($k$ tầng, $n$ lệnh) | $(k + n - 1) \times T$ |
| CPI khi có đoán sai rẽ nhánh | $1 + \text{tỉ lệ rẽ nhánh} \times \text{tỉ lệ sai} \times \text{số chu kỳ phạt}$ |
| Chia địa chỉ cache | offset = $\log_2(\text{khối})$, index = $\log_2(\text{số tập})$, tag = phần còn lại |
| AMAT | $\text{thời gian trúng} + \text{tỉ lệ trượt} \times \text{chi phí trượt}$ |
| Kích thước bảng trang một tầng | $2^{\text{bit địa chỉ} - \text{bit offset}} \times \text{kích thước mục}$ |
| Độ trễ quay trung bình của HDD | $\dfrac{1}{2} \times \dfrac{60}{\text{vòng/phút}}$ giây |

## Phụ lục B. Hiểu lầm phổ biến

1. **"CPU nhiều GHz hơn thì nhanh hơn."** Không chắc — còn phụ thuộc CPI và số lệnh (Ví dụ 11.1).
2. **"Nhiều lõi hơn thì chương trình nào cũng nhanh hơn."** Chỉ khi chương trình được viết để chạy song song, và vẫn bị giới hạn bởi Amdahl.
3. **"`float` lưu chính xác số thập phân."** Không — 0,1 không biểu diễn chính xác được.
4. **"Big-O giống nhau thì tốc độ như nhau."** Không — duyệt `std::list` và `std::vector` đều $O(n)$ nhưng có thể chênh nhau nhiều lần do cache.
5. **"Bộ nhớ ảo nghĩa là dùng ổ cứng làm RAM."** Swap chỉ là **một** công dụng; mục đích chính là cô lập và quản lý bộ nhớ.
6. **"Pipeline làm mỗi lệnh chạy nhanh hơn."** Không — nó tăng **thông lượng**; mỗi lệnh riêng lẻ còn chậm hơn chút.
7. **"Tràn số `int` thì quay vòng như phần cứng."** Trong C/C++, tràn số có dấu là **UB**.
8. **"Tối ưu tay luôn tốt hơn trình biên dịch."** Thường không — hãy đo trên bản build có tối ưu (Ví dụ ở 13.1).
9. **"x86 là CISC nên bên trong chạy lệnh phức tạp."** CPU x86 hiện đại dịch lệnh sang vi lệnh kiểu RISC.
10. **"RAID thay cho sao lưu."** Không.

## Phụ lục C. Lộ trình và tài liệu

### C.1 Lộ trình gợi ý (khoảng 8 tuần)

| Tuần | Nội dung | Mục tiêu tối thiểu |
|---|---|---|
| 1 | Chương 1–4 | Đổi hệ đếm, bù 2, IEEE 754 bằng tay |
| 2 | Chương 5–7 | Vẽ được bộ cộng, MUX; hiểu flip-flop và thanh ghi |
| 3–4 | Chương 8–10 | Viết và đọc được hợp ngữ RISC-V; đọc hiểu output của `gcc -S` cho hàm đơn giản |
| 5 | Chương 11–13 | Tính hiệu năng, Amdahl, pipeline; nhận diện xung đột |
| 6 | Chương 14–15 | Mô phỏng cache bằng tay; viết lại một vòng lặp cho thân thiện cache |
| 7 | Chương 16–18 | Dịch địa chỉ ảo; giải thích false sharing |
| 8 | Chương 19 + ôn tập | Kể lại toàn bộ hành trình chạy một chương trình |

### C.2 Công cụ thực hành

- **Logisim-evolution** hoặc **Digital**: vẽ và mô phỏng mạch số — tự dựng bộ cộng, thanh ghi, thậm chí cả một CPU.
- **RARS**, **Ripes** hoặc **Venus**: trình mô phỏng RISC-V; Ripes còn hiển thị trực quan pipeline và cache.
- **Compiler Explorer** (godbolt.org): xem hợp ngữ do trình biên dịch sinh ra.
- **perf** (Linux), **VTune** (Intel), **Visual Studio Profiler**: đo số lần trượt cache, đoán sai rẽ nhánh của chương trình thật.

### C.3 Tài liệu nên dùng song song

- **"Computer Organization and Design: The Hardware/Software Interface — RISC-V Edition"** (Patterson & Hennessy) — giáo trình nhập môn chuẩn, sát với nội dung tài liệu này.
- **"Computer Systems: A Programmer's Perspective"** (Bryant & O'Hallaron) — kiến trúc máy tính từ góc nhìn **lập trình viên C**, rất thực tế (biểu diễn dữ liệu, x86-64, cache, bộ nhớ ảo, liên kết).
- **"Digital Design and Computer Architecture"** (Harris & Harris) — mạnh về mạch số, dẫn tới thiết kế CPU hoàn chỉnh.
- **Nand2Tetris** (sách "The Elements of Computing Systems" và khóa học trực tuyến miễn phí) — tự xây máy tính từ cổng NAND đến hệ điều hành.
- **"Code: The Hidden Language of Computer Hardware and Software"** (Charles Petzold) — sách phổ thông, dễ đọc, giúp có cái nhìn trực quan.
- **"Computer Architecture: A Quantitative Approach"** (Hennessy & Patterson) — khi muốn đi sâu vào thiết kế CPU hiện đại.

### C.4 Bước tiếp theo theo hướng đi

- **Kỹ sư máy tính:** thiết kế mạch số bằng Verilog/SystemVerilog, lập trình FPGA, tự thiết kế một CPU RISC-V pipeline; hệ thống nhúng.
- **Kỹ sư game:** thiết kế hướng dữ liệu, SIMD, đa luồng trong engine, lập trình GPU (shader, compute shader), đo đạc hiệu năng.
- **Kỹ sư AI:** GPU và bộ tăng tốc chuyên dụng, tính toán song song, hiểu vì sao băng thông bộ nhớ thường là giới hạn khi chạy mô hình lớn.

---

*Chúc bạn học tốt! Kiến trúc máy tính trở nên "sống" nhất khi bạn tự **đo đạc**: hãy chạy các thí nghiệm trong tài liệu, đổi tham số, và quan sát máy tính của chính mình.*
