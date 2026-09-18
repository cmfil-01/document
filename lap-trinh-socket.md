# LẬP TRÌNH SOCKET — NỀN TẢNG VỮNG CHẮC

*Tài liệu tự học: từ `socket()` đầu tiên đến chat server nhiều người dùng, web server tối giản và lớp C++ RAII — code chạy được trên cả Linux lẫn Windows*

---

## 0. Cách dùng tài liệu này

### 0.1 Cấu trúc

| Phần | Chương | Nội dung |
|---|---|---|
| **I. Nền tảng** | 1–3 | Socket là gì, lớp tương thích Linux/Windows, địa chỉ và phân giải tên |
| **II. TCP** | 4–6 | Server/client đầu tiên, **TCP là dòng byte và cách đóng khung thông điệp**, đóng kết nối và xử lý lỗi |
| **III. UDP** | 7–8 | Datagram, mất gói, timeout; socket không chặn và vòng lặp game |
| **IV. Nhiều client** | 9–11 | Các mô hình đồng thời, **chat server với `poll`**, epoll/IOCP và thư viện |
| **V. Tùy chọn và hiệu năng** | 12 | Timeout, **Nagle và `TCP_NODELAY`**, bộ đệm, dual-stack IPv4/IPv6 |
| **VI. Vận dụng** | 13–16 | Lớp C++ RAII, client chat đa luồng, web server tối giản, bảo mật, gỡ lỗi và kiểm thử |
| **Phụ lục** | A–D | Bảng API, checklist, lộ trình, tài liệu (bảng mã lỗi ở mục 6.5) |

Mỗi chương đi theo mạch: **ý tưởng → hàm và cơ chế → chương trình chạy được → lỗi hay gặp**. Cuối mỗi phần có **bài tập kèm lời giải**.

**Kiến thức cần có:** C cơ bản (con trỏ, struct, mảng); khái niệm mạng cơ bản — địa chỉ IP, cổng, sự khác nhau giữa TCP và UDP.

### 0.2 Môi trường

| Hệ điều hành | Biên dịch (C) | Biên dịch (C++) |
|---|---|---|
| **Linux / macOS / WSL** | `gcc -Wall -Wextra file.c -o file` | `g++ -std=c++17 -Wall -Wextra -pthread file.cpp -o file` |
| **Windows + MinGW** | `gcc -Wall -Wextra file.c -o file.exe -lws2_32` | `g++ -std=c++17 -Wall -Wextra file.cpp -o file.exe -lws2_32` |
| **Windows + Visual Studio** | Thêm `Ws2_32.lib` vào Linker → Input | Như bên trái |

- Mọi chương trình trong tài liệu đều `#include "net.h"` (Chương 2) — đặt file này **cùng thư mục** với file nguồn.
- Trên Linux, dùng chế độ mặc định của GCC (hoặc `-std=gnu17`). Chế độ `-std=c17` "nghiêm ngặt" sẽ ẩn một số hàm POSIX như `getaddrinfo`.
- Với chương trình server, trình duyệt hoặc tường lửa Windows có thể hỏi quyền cho phép mở cổng — hãy cho phép khi thử nghiệm.

> ✅ **Về độ chính xác:**
> - Mọi **chương trình hoàn chỉnh** trong tài liệu đã được biên dịch với `-Wall -Wextra -pedantic` và chạy thật trên **Linux**, kèm AddressSanitizer/UBSan. (Các đoạn mã minh họa ngắn không có `main`, và đoạn dual-stack ở mục 12.4, thì không được chạy — đều có ghi chú tại chỗ.)
> - Chúng cũng được biên dịch cho **Windows** bằng MinGW-w64 và chạy thử qua **Wine**. Server Windows đã phục vụ client Linux và ngược lại; bản ghi của chat server trên hai nền tảng giống nhau từng ký tự.
> - Wine là lớp tương thích chứ không phải Windows thật, nên **câu thông báo lỗi và thời gian đo** có thể khác. Những chỗ khác biệt đều được ghi chú, và mã lỗi Winsock trong tài liệu được lấy từ header chính thức.
> - Phần "Kết quả" là output thật; phần "Ví dụ kết quả" chứa số đo thời gian hoặc số cổng thay đổi theo từng lần chạy.

---

# PHẦN I — NỀN TẢNG

## Chương 1. Socket là gì?

### 1.1 Ý tưởng

**Socket** là một **đầu mút giao tiếp** mà hệ điều hành cung cấp cho chương trình. Chương trình chỉ việc "ghi vào" và "đọc ra" socket; mọi việc phức tạp — chia gói, định tuyến, gửi lại gói mất, sắp xếp thứ tự — do **nhân hệ điều hành** lo.

```text
   Chương trình A                                        Chương trình B
  ┌─────────────┐                                       ┌─────────────┐
  │ send()/recv()│                                       │ send()/recv()│
  └──────┬──────┘                                       └──────┬──────┘
 ────────┼──────────── ranh giới user / kernel ────────────────┼────────
  ┌──────▼──────┐                                       ┌──────▼──────┐
  │   Socket    │  bộ đệm gửi ──► TCP/UDP ──► IP ──►    │   Socket    │
  │ (trong nhân)│  bộ đệm nhận ◄── ... mạng ... ◄──     │ (trong nhân)│
  └─────────────┘                                       └─────────────┘
```

Hai điều cần nhớ ngay từ đầu:

1. `send()` thành công **không** có nghĩa là bên kia đã nhận — nó chỉ có nghĩa là dữ liệu đã được chép vào **bộ đệm gửi** của nhân.
2. `recv()` chỉ lấy những gì **đang có** trong bộ đệm nhận — có thể ít hơn bạn muốn.

Trên Linux/macOS, socket là một **file descriptor** (số nguyên), dùng chung cơ chế với file. Trên Windows, socket là kiểu `SOCKET` riêng.

Một kết nối TCP được xác định duy nhất bởi **bộ 5**: *(giao thức, IP nguồn, cổng nguồn, IP đích, cổng đích)*. Nhờ vậy, một server ở cổng 443 phục vụ được hàng nghìn client cùng lúc.

### 1.2 Vòng đời của socket TCP

```text
            SERVER                                        CLIENT
   socket()      tạo socket                          socket()
   bind()        gắn vào địa chỉ + cổng                  │
   listen()      chuyển sang chế độ lắng nghe            │
   accept()  ◄─────────── bắt tay 3 bước ──────────── connect()
      │ (trả về SOCKET MỚI dành riêng cho client này)    │
   recv()    ◄──────────────── dữ liệu ────────────── send()
   send()    ──────────────── dữ liệu ───────────────► recv()
   recv() = 0 ◄─────────────── FIN ────────────────── close() / shutdown()
   close()
```

### 1.3 Vòng đời của socket UDP

```text
            SERVER                                        CLIENT
   socket()                                          socket()
   bind()        gắn vào cổng cố định                    │  (không cần bind — OS tự chọn cổng)
   recvfrom() ◄──────────── datagram ─────────────── sendto()
   sendto()   ────────────── datagram ─────────────► recvfrom()
   (không có kết nối, không có accept, không có bắt tay)
```

### 1.4 Các lời gọi mặc định là **chặn** (blocking)

`accept()` chờ đến khi có client; `recv()` chờ đến khi có dữ liệu; `connect()` chờ đến khi bắt tay xong hoặc thất bại. Trong lúc chờ, **cả luồng đứng yên**. Làm sao phục vụ nhiều client, hay giữ cho vòng lặp game không bị đứng? Đó là nội dung của Phần III và IV.

---

## Chương 2. Lớp tương thích Linux / Windows (`net.h`)

### 2.1 Winsock khác POSIX ở đâu?

Winsock được thiết kế dựa trên socket của BSD, nên **phần lớn hàm giống hệt nhau**. Các khác biệt chính:

| Chủ đề | POSIX (Linux, macOS) | Winsock (Windows) |
|---|---|---|
| Khởi tạo | Không cần | **Bắt buộc** `WSAStartup()` trước, `WSACleanup()` sau |
| Header | `<sys/socket.h>`, `<netinet/in.h>`, `<arpa/inet.h>`, `<netdb.h>`, `<unistd.h>` | `<winsock2.h>`, `<ws2tcpip.h>` |
| Kiểu socket | `int` | `SOCKET` |
| Giá trị lỗi của `socket()`/`accept()` | `-1` | `INVALID_SOCKET` |
| Các hàm khác báo lỗi | trả về `-1` | trả về `SOCKET_ERROR` (cũng là −1) |
| Đóng socket | `close()` | `closesocket()` |
| Mã lỗi | `errno` | `WSAGetLastError()` |
| Kiểu trả về của `send`/`recv` | `ssize_t` | `int` |
| Chế độ không chặn | `fcntl(..., O_NONBLOCK)` | `ioctlsocket(..., FIONBIO, ...)` |
| `poll` | `poll()` | `WSAPoll()` |
| Timeout của `SO_RCVTIMEO` | `struct timeval` | `DWORD` (mili giây) |
| Ghi vào kết nối đã hỏng | Tín hiệu `SIGPIPE` **giết** tiến trình (nếu không xử lý) | Chỉ trả lỗi |

### 2.2 File `net.h`

Thay vì rải `#ifdef _WIN32` khắp nơi, ta gom các khác biệt vào một header. **Mọi chương trình trong tài liệu dùng file này.**

```c
/* file: net.h — lớp tương thích tối thiểu giữa POSIX socket (Linux, macOS) và Winsock (Windows) */
#ifndef NET_H
#define NET_H

#ifdef _WIN32
#  ifndef _WIN32_WINNT
#    define _WIN32_WINNT 0x0600               /* Vista trở lên: có inet_pton, inet_ntop, WSAPoll */
#  endif
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  include <windows.h>
typedef SOCKET socket_t;
typedef int    net_ssize_t;                   /* send/recv trả về int trên Windows */
#  define NET_INVALID         INVALID_SOCKET
#  define net_close(s)        closesocket(s)
#  define net_poll(fds, n, t) WSAPoll((fds), (ULONG)(n), (t))
#  define NET_SHUT_WR         SD_SEND
#else
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <sys/time.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <unistd.h>
#  include <fcntl.h>
#  include <poll.h>
#  include <errno.h>
#  include <signal.h>
#  include <time.h>
typedef int     socket_t;
typedef ssize_t net_ssize_t;
#  define NET_INVALID         (-1)
#  define net_close(s)        close(s)
#  define net_poll(fds, n, t) poll((fds), (nfds_t)(n), (t))
#  define NET_SHUT_WR         SHUT_WR
#endif

#include <stdio.h>
#include <string.h>

/* Gọi MỘT LẦN khi chương trình bắt đầu. Trả về 0 nếu thành công. */
static inline int net_init(void) {
#ifdef _WIN32
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 2), &wsa);
#else
    signal(SIGPIPE, SIG_IGN);   /* ghi vào kết nối đã hỏng: nhận lỗi EPIPE thay vì bị hệ điều hành "giết" */
    return 0;
#endif
}

static inline void net_cleanup(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

/* Mã lỗi của lời gọi socket vừa thất bại */
static inline int net_last_error(void) {
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

/* Chuỗi mô tả một mã lỗi */
static inline const char *net_strerror(int err) {
#ifdef _WIN32
    static char buf[256];
    DWORD n = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                             NULL, (DWORD)err, 0, buf, sizeof buf, NULL);
    while (n > 0 && (buf[n - 1] == '\r' || buf[n - 1] == '\n')) buf[--n] = '\0';
    return n > 0 ? buf : "unknown error";
#else
    return strerror(err);
#endif
}

/* In "<what>: <mô tả lỗi>" ra stderr, giống perror() */
static inline void net_perror(const char *what) {
    int err = net_last_error();
    fprintf(stderr, "%s: %s (ma %d)\n", what, net_strerror(err), err);
}

/* Lỗi "chưa có dữ liệu, thử lại sau" của socket không chặn */
static inline int net_would_block(int err) {
#ifdef _WIN32
    return err == WSAEWOULDBLOCK;
#else
    return err == EAGAIN || err == EWOULDBLOCK;
#endif
}

/* Lỗi "hết thời gian chờ" khi đặt SO_RCVTIMEO */
static inline int net_is_timeout(int err) {
#ifdef _WIN32
    return err == WSAETIMEDOUT;
#else
    return err == EAGAIN || err == EWOULDBLOCK;
#endif
}

/* Bật/tắt chế độ không chặn (non-blocking) */
static inline int net_set_nonblocking(socket_t s, int on) {
#ifdef _WIN32
    u_long mode = on ? 1 : 0;
    return ioctlsocket(s, FIONBIO, &mode);
#else
    int flags = fcntl(s, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(s, F_SETFL, on ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK));
#endif
}

/* recv() chờ tối đa `ms` mili giây rồi báo lỗi timeout */
static inline int net_set_recv_timeout(socket_t s, int ms) {
#ifdef _WIN32
    DWORD tv = (DWORD)ms;
#else
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
#endif
    return setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
}

/* Ngủ `ms` mili giây */
static inline void net_sleep_ms(int ms) {
#ifdef _WIN32
    Sleep((DWORD)ms);
#else
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
#endif
}

/* Đồng hồ đơn điệu, đơn vị mili giây (dùng để đo thời gian) */
static inline double net_now_ms(void) {
#ifdef _WIN32
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)f.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1e6;
#endif
}

#endif /* NET_H */
```

**Giải thích một số lựa chọn:**

- `static inline`: hàm được định nghĩa ngay trong header mà không gây lỗi "định nghĩa trùng" khi nhiều file cùng include, và trình biên dịch không cảnh báo nếu hàm không được dùng.
- `_WIN32_WINNT 0x0600`: yêu cầu API từ Windows Vista trở lên — cần cho `inet_pton`, `inet_ntop` và `WSAPoll`.
- `signal(SIGPIPE, SIG_IGN)`: trên Linux, ghi vào một kết nối mà bên kia đã đóng sẽ phát tín hiệu `SIGPIPE`, mặc định **giết chương trình ngay lập tức, không một dòng báo lỗi**. Bỏ qua tín hiệu này để nhận mã lỗi `EPIPE` như bình thường. (Cách khác trên Linux: truyền cờ `MSG_NOSIGNAL` cho từng lời gọi `send`.)
- Trong code, ta luôn ép kiểu con trỏ bộ đệm về `(const char *)` và độ dài về `int` khi gọi `send`/`recv`/`setsockopt` — Winsock yêu cầu vậy, còn POSIX chấp nhận được.

---

## Chương 3. Địa chỉ và phân giải tên

### 3.1 Họ cấu trúc `sockaddr`

API socket dùng **một con trỏ chung** `struct sockaddr *` cho mọi loại địa chỉ. Bên dưới là các cấu trúc cụ thể:

| Cấu trúc | Dùng cho | Trường quan trọng |
|---|---|---|
| `struct sockaddr_in` | IPv4 | `sin_family = AF_INET`, `sin_port`, `sin_addr` |
| `struct sockaddr_in6` | IPv6 | `sin6_family = AF_INET6`, `sin6_port`, `sin6_addr` |
| `struct sockaddr_storage` | **Bất kỳ** loại nào | Đủ lớn để chứa mọi loại địa chỉ — dùng khi nhận địa chỉ của người khác (`accept`, `recvfrom`) |
| `struct sockaddr` | "Kiểu chung" để truyền vào hàm | Chỉ dùng để ép kiểu con trỏ |

```c
struct sockaddr_in addr;
memset(&addr, 0, sizeof addr);             /* LUÔN xóa về 0 trước */
addr.sin_family = AF_INET;
addr.sin_port = htons(9000);               /* đổi sang thứ tự byte mạng! */
inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
connect(s, (struct sockaddr *)&addr, sizeof addr);   /* ép kiểu về sockaddr* */
```

### 3.2 Thứ tự byte mạng

CPU x86 và phần lớn ARM lưu số **little-endian** (byte thấp trước); giao thức mạng quy ước **big-endian** (byte cao trước).

| Hàm | Ý nghĩa |
|---|---|
| `htons` / `ntohs` | host ↔ network, số **16 bit** (cổng) |
| `htonl` / `ntohl` | host ↔ network, số **32 bit** (địa chỉ IPv4, số nguyên trong giao thức của bạn) |
| `inet_pton` | chuỗi `"192.168.1.10"` → dạng nhị phân |
| `inet_ntop` | dạng nhị phân → chuỗi |

> ⚠️ Quên `htons` là lỗi kinh điển: `sin_port = 8080` trên máy little-endian khiến chương trình thực ra dùng cổng `0x901F` = 36 895.

### 3.3 `getaddrinfo` — cách hiện đại để tạo địa chỉ

Thay vì tự điền `sockaddr_in`, hãy dùng `getaddrinfo`: nó phân giải tên miền (DNS), hỗ trợ **cả IPv4 lẫn IPv6**, và trả về **danh sách** địa chỉ đã điền sẵn.

```c
int getaddrinfo(const char *host, const char *port,
                const struct addrinfo *hints, struct addrinfo **result);
void freeaddrinfo(struct addrinfo *result);         /* bắt buộc gọi để giải phóng */
const char *gai_strerror(int errcode);              /* mô tả lỗi — KHÔNG dùng errno */
```

| Trường của `hints` | Giá trị thường dùng |
|---|---|
| `ai_family` | `AF_UNSPEC` (cả hai), `AF_INET` (chỉ IPv4), `AF_INET6` |
| `ai_socktype` | `SOCK_STREAM` (TCP), `SOCK_DGRAM` (UDP) |
| `ai_flags` | `AI_PASSIVE` (địa chỉ để server **bind** khi `host = NULL`), `AI_NUMERICHOST` (không tra DNS) |

**Chương trình minh họa:**

```c
#include "net.h"

/* In mọi địa chỉ mà getaddrinfo trả về cho (host, port) */
static void resolve(const char *label, const char *host, const char *port, int flags) {
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;        /* chấp nhận cả IPv4 lẫn IPv6 */
    hints.ai_socktype = SOCK_STREAM;    /* TCP */
    hints.ai_flags = flags;

    int err = getaddrinfo(host, port, &hints, &res);
    if (err != 0) {
        printf("%-14s -> loi: %s\n", label, gai_strerror(err));
        return;
    }
    for (p = res; p != NULL; p = p->ai_next) {
        char ip[INET6_ADDRSTRLEN];
        unsigned port_num;
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *a = (struct sockaddr_in *)p->ai_addr;
            inet_ntop(AF_INET, &a->sin_addr, ip, sizeof ip);
            port_num = ntohs(a->sin_port);
        } else {
            struct sockaddr_in6 *a = (struct sockaddr_in6 *)p->ai_addr;
            inet_ntop(AF_INET6, &a->sin6_addr, ip, sizeof ip);
            port_num = ntohs(a->sin6_port);
        }
        printf("%-14s -> %s %-12s cong %u\n", label,
               p->ai_family == AF_INET ? "IPv4" : "IPv6", ip, port_num);
    }
    freeaddrinfo(res);   /* getaddrinfo cấp phát bộ nhớ -> phải giải phóng */
}

int main(void) {
    if (net_init() != 0) return 1;

    /* 1) Thứ tự byte: cổng phải được đổi sang thứ tự mạng (big-endian) */
    unsigned short net_port = htons(8080);                 /* 8080 = 0x1F90 */
    unsigned char *b = (unsigned char *)&net_port;
    printf("htons(8080) trong bo nho: %02x %02x\n", b[0], b[1]);

    /* 2) Chuỗi IP <-> nhị phân */
    struct in_addr bin;
    inet_pton(AF_INET, "192.168.1.10", &bin);
    unsigned char *ip = (unsigned char *)&bin;
    printf("192.168.1.10 dang nhi phan: %02x %02x %02x %02x\n", ip[0], ip[1], ip[2], ip[3]);

    /* 3) getaddrinfo — cách hiện đại để tạo địa chỉ, hỗ trợ cả IPv4 và IPv6 */
    resolve("192.168.1.10", "192.168.1.10", "80", AI_NUMERICHOST);
    resolve("2001:db8::1", "2001:db8::1", "443", AI_NUMERICHOST);
    resolve("abc (so)", "abc", "80", AI_NUMERICHOST);      /* không phải địa chỉ số -> lỗi */
    resolve("NULL + PASSIVE", NULL, "9000", AI_PASSIVE);   /* địa chỉ để server bind */

    net_cleanup();
    return 0;
}
```

**Kết quả** (Linux):

```text
htons(8080) trong bo nho: 1f 90
192.168.1.10 dang nhi phan: c0 a8 01 0a
192.168.1.10   -> IPv4 192.168.1.10 cong 80
2001:db8::1    -> IPv6 2001:db8::1  cong 443
abc (so)       -> loi: Name or service not known
NULL + PASSIVE -> IPv4 0.0.0.0      cong 9000
NULL + PASSIVE -> IPv6 ::           cong 9000
```

Bản Windows cho kết quả giống hệt, chỉ khác câu thông báo lỗi ở dòng `abc`.

**Nhận xét:**

- `0.0.0.0` (IPv4) và `::` (IPv6) nghĩa là **"mọi địa chỉ của máy này"** — server bind vào đây sẽ nhận kết nối từ mọi card mạng.
- Một tên như `localhost` có thể trả về **nhiều** địa chỉ (`::1` và `127.0.0.1`), theo thứ tự tùy hệ thống. Vì vậy client đúng chuẩn phải **thử lần lượt từng địa chỉ** cho đến khi kết nối được (xem `connect_tcp` ở Chương 4). Đây là nguyên nhân của một lỗi phổ biến: client kết nối `localhost`, thử `::1` trước, nhưng server chỉ nghe IPv4 → báo "Connection refused" dù server đang chạy.

---

## Bài tập Phần I

**Bài I.1.** Vì sao phải `memset` cấu trúc `sockaddr_in` về 0 trước khi điền?

**Bài I.2.** Một chương trình Windows gọi `socket()` và luôn nhận `INVALID_SOCKET`, `WSAGetLastError()` trả về **10093**. Nguyên nhân là gì?

**Bài I.3.** Viết lệnh `getaddrinfo` (các trường của `hints`) để: (a) server UDP bind vào mọi địa chỉ IPv4 ở cổng 5000; (b) client TCP kết nối `example.com` cổng 443, chấp nhận cả IPv4 và IPv6.

<details>
<summary><b>Lời giải Phần I</b> (bấm để mở)</summary>

**I.1.** Cấu trúc có thể có trường ẩn/đệm (ví dụ `sin_zero`, hay `sin_len` trên macOS); để giá trị rác trong đó có thể khiến `bind`/`connect` thất bại hoặc hoạt động không như mong đợi.

**I.2.** Mã 10093 là `WSANOTINITIALISED`: chưa gọi `WSAStartup()` (trong tài liệu: `net_init()`).

**I.3.** (a) `ai_family = AF_INET`, `ai_socktype = SOCK_DGRAM`, `ai_flags = AI_PASSIVE`, gọi `getaddrinfo(NULL, "5000", ...)`. (b) `ai_family = AF_UNSPEC`, `ai_socktype = SOCK_STREAM`, `ai_flags = 0`, gọi `getaddrinfo("example.com", "443", ...)` rồi thử kết nối lần lượt từng kết quả.

</details>

---

# PHẦN II — TCP

## Chương 4. Server và client TCP đầu tiên

### 4.1 Echo server

Server "echo" gửi trả lại mọi thứ nó nhận được — chương trình "Hello World" của lập trình mạng.

```c
/* file: tcp_echo_server.c — server "echo": gửi trả lại mọi thứ nhận được (xử lý lần lượt từng client)
   Cách dùng: ./tcp_echo_server [cong]   (mặc định 9000) */
#include "net.h"
#include <stdlib.h>

static int send_all(socket_t s, const char *buf, size_t len) {
    while (len > 0) {
        net_ssize_t n = send(s, buf, (int)len, 0);
        if (n <= 0) return -1;
        buf += n;
        len -= (size_t)n;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    const char *port = argc > 1 ? argv[1] : "9000";
    if (net_init() != 0) return 1;

    /* 1) Tìm địa chỉ để bind: host = NULL + AI_PASSIVE -> "mọi địa chỉ của máy này" */
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;           /* IPv4 cho đơn giản (xem mục dual-stack để hỗ trợ IPv6) */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int err = getaddrinfo(NULL, port, &hints, &res);
    if (err != 0) { fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err)); return 1; }

    /* 2) socket() */
    socket_t lst = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (lst == NET_INVALID) { net_perror("socket"); return 1; }

    /* Cho phép khởi động lại server ngay, không bị "Address already in use" do TIME_WAIT
       (trên Windows, SO_REUSEADDR có nghĩa khác — xem mục 6.4) */
#ifndef _WIN32
    int yes = 1;
    setsockopt(lst, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof yes);
#endif

    /* 3) bind() + 4) listen() */
    if (bind(lst, res->ai_addr, (socklen_t)res->ai_addrlen) != 0) { net_perror("bind"); return 1; }
    freeaddrinfo(res);
    if (listen(lst, SOMAXCONN) != 0) { net_perror("listen"); return 1; }
    printf("Echo server dang lang nghe o cong %s\n", port);
    fflush(stdout);

    for (;;) {
        /* 5) accept(): chờ một client, nhận về socket MỚI dành riêng cho client đó */
        struct sockaddr_storage peer;               /* đủ lớn cho cả IPv4 và IPv6 */
        socklen_t peer_len = sizeof peer;
        socket_t conn = accept(lst, (struct sockaddr *)&peer, &peer_len);
        if (conn == NET_INVALID) { net_perror("accept"); continue; }

        char ip[INET6_ADDRSTRLEN] = "?";
        struct sockaddr_in *p4 = (struct sockaddr_in *)&peer;
        inet_ntop(AF_INET, &p4->sin_addr, ip, sizeof ip);
        printf("[+] client %s\n", ip);
        fflush(stdout);

        /* 6) Vòng lặp nhận – gửi, đến khi client đóng (recv trả về 0) hoặc lỗi (< 0) */
        char buf[4096];
        net_ssize_t n;
        long total = 0;
        while ((n = recv(conn, buf, sizeof buf, 0)) > 0) {
            if (send_all(conn, buf, (size_t)n) != 0) break;
            total += n;
        }
        if (n < 0) net_perror("recv");
        printf("[-] client %s ngat ket noi, da echo %ld byte\n", ip, total);
        fflush(stdout);

        /* 7) close(): đóng socket của client này rồi quay lại chờ client khác */
        net_close(conn);
    }
}
```

### 4.2 Echo client

```c
/* file: tcp_echo_client.c — gửi một thông điệp tới echo server và in phản hồi
   Cách dùng: ./tcp_echo_client <host> <cong> <thong diep> */
#include "net.h"
#include <stdlib.h>

/* Thử lần lượt mọi địa chỉ của host cho đến khi connect được (hỗ trợ cả IPv4 và IPv6) */
static socket_t connect_tcp(const char *host, const char *port) {
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int err = getaddrinfo(host, port, &hints, &res);
    if (err != 0) { fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err)); return NET_INVALID; }

    socket_t s = NET_INVALID;
    for (p = res; p != NULL; p = p->ai_next) {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s == NET_INVALID) continue;
        if (connect(s, p->ai_addr, (socklen_t)p->ai_addrlen) == 0) break;   /* thành công */
        net_perror("connect");
        net_close(s);
        s = NET_INVALID;
    }
    freeaddrinfo(res);
    return s;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Cach dung: %s <host> <cong> <thong diep>\n", argv[0]);
        return 1;
    }
    if (net_init() != 0) return 1;

    socket_t s = connect_tcp(argv[1], argv[2]);
    if (s == NET_INVALID) return 1;

    const char *msg = argv[3];
    size_t len = strlen(msg);
    if (send(s, msg, (int)len, 0) != (net_ssize_t)len) { net_perror("send"); return 1; }

    /* Báo "tôi gửi xong" (đóng chiều gửi) — server sẽ nhận recv() == 0 sau khi đọc hết */
    shutdown(s, NET_SHUT_WR);

    /* Đọc đến khi server đóng kết nối — không giả định một lần recv là đủ */
    char buf[1024];
    size_t got = 0;
    net_ssize_t n;
    while (got < sizeof buf - 1 && (n = recv(s, buf + got, (int)(sizeof buf - 1 - got), 0)) > 0)
        got += (size_t)n;
    buf[got] = '\0';
    printf("Server tra loi: %s\n", buf);

    net_close(s);
    net_cleanup();
    return 0;
}
```

### 4.3 Chạy thử

```bash
# Terminal 1
gcc -Wall -Wextra tcp_echo_server.c -o tcp_echo_server
./tcp_echo_server 9000

# Terminal 2
gcc -Wall -Wextra tcp_echo_client.c -o tcp_echo_client
./tcp_echo_client 127.0.0.1 9000 "Xin chao TCP"
./tcp_echo_client localhost 9000 "Lan thu hai"
./tcp_echo_client 127.0.0.1 9555 "khong ai nghe"
```

**Kết quả ở Terminal 2:**

```text
Server tra loi: Xin chao TCP
Server tra loi: Lan thu hai
connect: Connection refused (ma 111)
```

**Kết quả ở Terminal 1:**

```text
Echo server dang lang nghe o cong 9000
[+] client 127.0.0.1
[-] client 127.0.0.1 ngat ket noi, da echo 12 byte
[+] client 127.0.0.1
[-] client 127.0.0.1 ngat ket noi, da echo 11 byte
```

Đã thử chéo nền tảng: server bản Windows phục vụ client Linux, và client bản Windows kết nối server Linux — đều cho kết quả đúng.

> 💡 **Không cần viết client để thử server:** dùng `nc 127.0.0.1 9000` (netcat, có sẵn trên Linux/macOS) hoặc `telnet 127.0.0.1 9000`, gõ vài dòng và xem server trả lời.

### 4.4 Từng lời gọi hàm làm gì?

| Lời gọi | Ý nghĩa | Lưu ý |
|---|---|---|
| `socket(family, type, proto)` | Tạo socket | `SOCK_STREAM` = TCP |
| `setsockopt(SO_REUSEADDR)` | Cho phép bind lại cổng vừa dùng | Xem mục 6.4 |
| `bind(s, addr, len)` | Gắn socket vào địa chỉ + cổng | Trên Linux, cổng < 1024 cần quyền quản trị |
| `listen(s, backlog)` | Chuyển sang lắng nghe | `backlog`: số kết nối đã bắt tay xong nhưng **chưa được `accept`** mà nhân giữ giùm |
| `accept(s, addr, len)` | Lấy **một** kết nối từ hàng đợi | Trả về **socket mới**; socket lắng nghe vẫn tiếp tục nghe |
| `connect(s, addr, len)` | Bắt tay 3 bước với server | Client thường không cần `bind` — OS tự chọn cổng tạm |
| `send` / `recv` | Gửi / nhận | Có thể xử lý **ít hơn** số byte yêu cầu (Chương 5) |
| `shutdown(s, SHUT_WR)` | Đóng **chiều gửi** | Bên kia nhận `recv() == 0` (Chương 6) |
| `close` / `closesocket` | Giải phóng socket | |

**Hạn chế của server này:** nó phục vụ **lần lượt từng client**. Trong lúc đang nói chuyện với client 1, client 2 kết nối vẫn được (nhân đã bắt tay và xếp vào hàng đợi `backlog`), nhưng **không được phục vụ** cho đến khi client 1 rời đi. Chương 9 giải quyết vấn đề này.

---

## Chương 5. TCP là dòng byte — và cách đóng khung thông điệp

### 5.1 Thí nghiệm

TCP chuyển một **dòng byte liên tục**, **không** có khái niệm "thông điệp". Hai lần `send` có thể đến trong một lần `recv`, và một lần `send` lớn có thể phải `recv` nhiều lần:

```c
#include "net.h"

/* Tạo hai socket TCP đã kết nối với nhau qua 127.0.0.1 (trong cùng một chương trình) */
static int make_tcp_pair(socket_t *client, socket_t *server) {
    socket_t lst = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;                                  /* cổng 0: hệ điều hành tự chọn */
    socklen_t len = sizeof addr;
    if (lst == NET_INVALID || bind(lst, (struct sockaddr *)&addr, sizeof addr) != 0 ||
        listen(lst, 1) != 0 || getsockname(lst, (struct sockaddr *)&addr, &len) != 0)
        return -1;
    *client = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(*client, (struct sockaddr *)&addr, sizeof addr) != 0) return -1;
    *server = accept(lst, NULL, NULL);
    net_close(lst);                                     /* không cần socket lắng nghe nữa */
    return *server == NET_INVALID ? -1 : 0;
}

int main(void) {
    if (net_init() != 0) return 1;
    socket_t cli, srv;
    if (make_tcp_pair(&cli, &srv) != 0) { net_perror("make_tcp_pair"); return 1; }

    /* Thí nghiệm 1: gửi 2 lần, nhận 1 lần */
    send(cli, "Hello", 5, 0);
    send(cli, "World", 5, 0);
    net_sleep_ms(100);                                  /* chờ dữ liệu tới hết */
    char buf[64];
    net_ssize_t n = recv(srv, buf, sizeof buf - 1, 0);
    buf[n > 0 ? n : 0] = '\0';
    printf("TN1: 2 lan send(5 byte) -> 1 lan recv duoc %d byte: \"%s\"\n", (int)n, buf);

    /* Thí nghiệm 2: gửi 1 lần 20000 byte, nhận bằng bộ đệm 4096 byte */
    static char big[20000];
    memset(big, 'x', sizeof big);
    n = send(cli, big, (int)sizeof big, 0);
    printf("TN2: 1 lan send gui %d byte -> cac lan recv:", (int)n);
    net_sleep_ms(100);
    net_close(cli);                                     /* đóng để recv() trả về 0 khi hết dữ liệu */
    char chunk[4096];
    long total = 0;
    while ((n = recv(srv, chunk, sizeof chunk, 0)) > 0) {
        printf(" %d", (int)n);
        total += n;
    }
    printf("\n     tong cong %ld byte, recv cuoi cung tra ve %d (ben kia da dong)\n", total, (int)n);

    net_close(srv);
    net_cleanup();
    return 0;
}
```

**Kết quả** (Linux và Windows giống nhau):

```text
TN1: 2 lan send(5 byte) -> 1 lan recv duoc 10 byte: "HelloWorld"
TN2: 1 lan send gui 20000 byte -> cac lan recv: 4096 4096 4096 4096 3616
     tong cong 20000 byte, recv cuoi cung tra ve 0 (ben kia da dong)
```

- **TN1:** bên nhận **không thể biết** "Hello" và "World" là hai thông điệp riêng.
- **TN2:** một thông điệp 20 000 byte cần 5 lần `recv`. Qua mạng thật (không phải loopback), cách chia còn **thay đổi mỗi lần chạy**.

> ⚠️ **Lỗi phổ biến nhất khi mới học socket:** giả định "một lần `send` ↔ một lần `recv`". Chương trình chạy đúng trên máy mình (loopback nhanh) nhưng hỏng ngẫu nhiên khi chạy qua Internet.

### 5.2 Gửi đủ và nhận đủ

`send()` cũng có thể chỉ gửi **một phần** (khi bộ đệm gửi gần đầy, nhất là với socket không chặn). Hai hàm tiện ích cần có trong mọi chương trình TCP:

```c
/* Gửi đủ len byte */
static int send_all(socket_t s, const char *buf, size_t len);

/* Nhận ĐỦ len byte (dùng khi đã biết trước độ dài) */
static int recv_all(socket_t s, char *buf, size_t len);
```

(Cài đặt đầy đủ ở chương trình của mục 5.4.)

### 5.3 Đóng khung (framing): hai cách phổ biến

| Cách | Ví dụ | Ưu | Nhược |
|---|---|---|---|
| **Ký tự phân cách** | Mỗi thông điệp kết thúc bằng `\n` (HTTP header, SMTP, Redis) | Dễ đọc, dễ gỡ lỗi bằng `nc` | Nội dung không được chứa ký tự phân cách (phải "thoát"); phải quét từng byte |
| **Tiền tố độ dài** | `[4 byte độ dài][nội dung]` (nhiều giao thức nhị phân, game) | Nhanh, chứa được dữ liệu nhị phân tùy ý | Không đọc được bằng mắt |

### 5.4 Chương trình: tiền tố độ dài

```c
#include "net.h"
#include <stdint.h>
#include <stdlib.h>

#define MAX_MSG 1024   /* giới hạn độ dài thông điệp — KHÔNG BAO GIỜ tin độ dài do bên kia gửi */

/* Gửi đủ len byte (send có thể chỉ gửi được một phần) */
static int send_all(socket_t s, const char *buf, size_t len) {
    while (len > 0) {
        net_ssize_t n = send(s, buf, (int)len, 0);
        if (n <= 0) return -1;
        buf += n;
        len -= (size_t)n;
    }
    return 0;
}

/* Nhận ĐỦ len byte. Trả về 0 nếu thành công, 1 nếu bên kia đóng, -1 nếu lỗi */
static int recv_all(socket_t s, char *buf, size_t len) {
    while (len > 0) {
        net_ssize_t n = recv(s, buf, (int)len, 0);
        if (n == 0) return 1;
        if (n < 0) return -1;
        buf += n;
        len -= (size_t)n;
    }
    return 0;
}

/* Đóng khung: [độ dài 4 byte, big-endian][nội dung] */
static int send_msg(socket_t s, const char *text) {
    size_t len = strlen(text);
    unsigned char hdr[4] = {
        (unsigned char)(len >> 24), (unsigned char)(len >> 16),
        (unsigned char)(len >> 8),  (unsigned char)len,
    };
    if (send_all(s, (const char *)hdr, 4) != 0) return -1;
    return send_all(s, text, len);
}

static int recv_msg(socket_t s, char *out, size_t cap) {
    unsigned char hdr[4];
    int r = recv_all(s, (char *)hdr, 4);
    if (r != 0) return r;
    uint32_t len = ((uint32_t)hdr[0] << 24) | ((uint32_t)hdr[1] << 16) |
                   ((uint32_t)hdr[2] << 8) | (uint32_t)hdr[3];
    if (len >= cap) return -2;                 /* quá dài: từ chối thay vì tràn bộ đệm */
    r = recv_all(s, out, len);
    if (r != 0) return r;
    out[len] = '\0';
    return 0;
}

static int make_tcp_pair(socket_t *client, socket_t *server) {
    socket_t lst = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    socklen_t len = sizeof addr;
    if (lst == NET_INVALID || bind(lst, (struct sockaddr *)&addr, sizeof addr) != 0 ||
        listen(lst, 1) != 0 || getsockname(lst, (struct sockaddr *)&addr, &len) != 0)
        return -1;
    *client = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(*client, (struct sockaddr *)&addr, sizeof addr) != 0) return -1;
    *server = accept(lst, NULL, NULL);
    net_close(lst);
    return *server == NET_INVALID ? -1 : 0;
}

int main(void) {
    if (net_init() != 0) return 1;
    socket_t cli, srv;
    if (make_tcp_pair(&cli, &srv) != 0) { net_perror("make_tcp_pair"); return 1; }

    /* Bên gửi: 3 thông điệp liền nhau (TCP có thể gộp chúng lại) */
    send_msg(cli, "xin chao");
    send_msg(cli, "");                          /* thông điệp rỗng vẫn hợp lệ */
    send_msg(cli, "tin nhan thu ba");

    /* Một thông điệp bị cố tình chia làm hai lần gửi, cách nhau 200 ms */
    const char *slow = "\x00\x00\x00\x0B" "hello";   /* header báo 11 byte, mới gửi 5 */
    send_all(cli, slow, 9);
    net_sleep_ms(200);
    send_all(cli, " world", 6);

    /* Một header "độc hại" báo độ dài 4 GB */
    send_all(cli, "\xFF\xFF\xFF\xFF", 4);

    /* Bên nhận */
    char msg[MAX_MSG];
    for (int i = 1; i <= 5; i++) {
        int r = recv_msg(srv, msg, sizeof msg);
        if (r == 0) printf("thong diep %d: \"%s\" (%zu byte)\n", i, msg, strlen(msg));
        else if (r == -2) printf("thong diep %d: bi tu choi vi qua dai\n", i);
        else { printf("thong diep %d: ket noi loi/dong\n", i); break; }
    }

    net_close(cli);
    net_close(srv);
    net_cleanup();
    return 0;
}
```

**Kết quả** (Linux và Windows giống nhau):

```text
thong diep 1: "xin chao" (8 byte)
thong diep 2: "" (0 byte)
thong diep 3: "tin nhan thu ba" (15 byte)
thong diep 4: "hello world" (11 byte)
thong diep 5: bi tu choi vi qua dai
```

**Điểm đáng chú ý:**

- Thông điệp 4 bị cắt đôi và gửi cách nhau 200 ms, nhưng `recv_all` vẫn ghép đúng.
- Header "độc hại" báo 4 GB bị **từ chối** — nếu tin nó và `malloc` theo, server có thể cạn bộ nhớ. **Luôn đặt giới hạn độ dài.**
- Chuỗi `"\x00\x00\x00\x0B" "hello"` được viết thành **hai chuỗi liền nhau**, vì `\x0Bhe...` sẽ bị hiểu là một mã hex dài (`e` cũng là chữ số hex).

Cách **ký tự phân cách** được dùng trong chat server ở Chương 10 (và bài tập II.2).

### 5.5 Quy tắc gửi dữ liệu nhị phân

1. Dùng kiểu có **kích thước cố định** (`uint32_t`, `int16_t`), không dùng `int`, `long`.
2. Chuyển **thứ tự byte** rõ ràng (như `send_msg` ở trên, hoặc `htonl`).
3. **Không gửi nguyên struct** bằng `send(s, &my_struct, sizeof my_struct, 0)`: trình biên dịch có thể chèn byte đệm (padding) khác nhau giữa các nền tảng. Hãy ghi **từng trường** vào bộ đệm.
4. Với số thực, dùng dạng chuẩn (ví dụ `memcpy` sang `uint32_t` rồi đổi thứ tự byte — IEEE 754 giống nhau trên các CPU phổ biến), hoặc gửi dạng số nguyên đã **lượng tử hóa**.
5. Với giao thức phức tạp, cân nhắc thư viện: **Protocol Buffers**, **FlatBuffers**, **MessagePack**, hay đơn giản là **JSON** cho giao thức văn bản.

---

## Chương 6. Đóng kết nối và xử lý lỗi

### 6.1 `close` và `shutdown`

| Lời gọi | Tác dụng |
|---|---|
| `shutdown(s, SHUT_WR)` (Windows: `SD_SEND`) | Gửi **FIN**: "tôi gửi xong". Vẫn **nhận** được tiếp. Socket chưa bị giải phóng |
| `shutdown(s, SHUT_RDWR)` | Đóng cả hai chiều |
| `close(s)` / `closesocket(s)` | Giải phóng socket; nếu là tham chiếu cuối cùng thì đóng kết nối |

`shutdown(SHUT_WR)` hữu ích khi giao thức cần báo "hết dữ liệu" nhưng vẫn chờ phản hồi — như `tcp_echo_client` ở Chương 4.

### 6.2 `recv` trả về gì?

| Giá trị | Ý nghĩa | Nên làm |
|---|---|---|
| `> 0` | Số byte nhận được | Xử lý |
| `== 0` | Bên kia đã **đóng chiều gửi** (FIN) — kết thúc bình thường | Đóng socket |
| `< 0` | Lỗi — xem `net_last_error()` | Timeout/không chặn: thử lại; lỗi khác: đóng socket |

### 6.3 Thí nghiệm các lỗi thường gặp

```c
#include "net.h"

static void report(const char *what) {
    int err = net_last_error();
    printf("%-34s -> LOI %d: %s\n", what, err, net_strerror(err));
}

static int make_tcp_pair(socket_t *client, socket_t *server) {
    socket_t lst = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    socklen_t len = sizeof addr;
    if (lst == NET_INVALID || bind(lst, (struct sockaddr *)&addr, sizeof addr) != 0 ||
        listen(lst, 1) != 0 || getsockname(lst, (struct sockaddr *)&addr, &len) != 0)
        return -1;
    *client = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(*client, (struct sockaddr *)&addr, sizeof addr) != 0) return -1;
    *server = accept(lst, NULL, NULL);
    net_close(lst);
    return *server == NET_INVALID ? -1 : 0;
}

int main(void) {
    if (net_init() != 0) return 1;

    /* 1) Kết nối tới cổng không có chương trình nào lắng nghe */
    socket_t s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(1);                          /* gần như chắc chắn không có ai */
    if (connect(s, (struct sockaddr *)&addr, sizeof addr) != 0) report("connect toi cong dong");
    net_close(s);

    /* 2) Hai socket cùng bind một cổng */
    socket_t a = socket(AF_INET, SOCK_STREAM, 0);
    socket_t b = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_port = 0;
    socklen_t len = sizeof addr;
    bind(a, (struct sockaddr *)&addr, sizeof addr);
    listen(a, 1);
    getsockname(a, (struct sockaddr *)&addr, &len);    /* lấy cổng a đang dùng */
    if (bind(b, (struct sockaddr *)&addr, sizeof addr) != 0) report("bind cong dang duoc dung");
    net_close(a);
    net_close(b);

    /* 3) Bên kia đóng kết nối */
    socket_t cli, srv;
    if (make_tcp_pair(&cli, &srv) != 0) { net_perror("make_tcp_pair"); return 1; }
    net_close(srv);                                     /* server đóng -> gửi FIN */
    net_sleep_ms(100);
    char buf[16];
    net_ssize_t n = recv(cli, buf, sizeof buf, 0);
    printf("%-34s -> recv tra ve %d (FIN: ben kia da dong)\n", "recv sau khi ben kia dong", (int)n);

    n = send(cli, "a", 1, 0);                           /* vẫn "thành công": dữ liệu vào bộ đệm */
    printf("%-34s -> send tra ve %d\n", "send lan 1 sau khi bi dong", (int)n);
    net_sleep_ms(100);                                  /* bên kia trả lời bằng RST */
    n = send(cli, "b", 1, 0);
    if (n < 0) report("send lan 2 sau khi bi dong");
    net_close(cli);

    /* 4) Đóng socket khi vẫn còn dữ liệu CHƯA ĐỌC -> hệ điều hành gửi RST thay vì FIN */
    if (make_tcp_pair(&cli, &srv) != 0) { net_perror("make_tcp_pair"); return 1; }
    send(cli, "du lieu chua doc", 16, 0);
    net_sleep_ms(100);
    net_close(srv);                                     /* server đóng mà không đọc */
    net_sleep_ms(100);
    n = recv(cli, buf, sizeof buf, 0);
    if (n < 0) report("recv khi bi dong luc con du lieu");
    net_close(cli);

    net_cleanup();
    return 0;
}
```

**Kết quả** (Linux):

```text
connect toi cong dong              -> LOI 111: Connection refused
bind cong dang duoc dung           -> LOI 98: Address already in use
recv sau khi ben kia dong          -> recv tra ve 0 (FIN: ben kia da dong)
send lan 1 sau khi bi dong         -> send tra ve 1
send lan 2 sau khi bi dong         -> LOI 32: Broken pipe
recv khi bi dong luc con du lieu   -> LOI 104: Connection reset by peer
```

**Bản Windows** cho các mã lỗi tương ứng: **10061** (`WSAECONNREFUSED`), **10048** (`WSAEADDRINUSE`), và **10054** (`WSAECONNRESET`) ở cả hai dòng cuối.

**Đọc kết quả:**

1. **Connection refused:** máy đích trả lời bằng gói **RST** vì không có chương trình nào nghe cổng đó.
2. **Address already in use:** một cổng chỉ có một socket được bind (trừ khi dùng tùy chọn đặc biệt).
3. Bên kia đóng bình thường → `recv` trả về **0**.
4. `send` **lần đầu** sau khi bên kia đóng vẫn "thành công" — dữ liệu chỉ được chép vào bộ đệm. Bên kia (đã đóng) trả lời bằng **RST**, và lần `send` **tiếp theo** mới báo lỗi. **Bài học:** `send` thành công không chứng minh bên kia còn sống.
5. **Đóng socket khi vẫn còn dữ liệu chưa đọc**, nhân hệ điều hành gửi **RST** thay vì FIN, và bên kia nhận lỗi *Connection reset* thay vì `recv() == 0`. Hãy đọc hết dữ liệu (hoặc `shutdown` trước) nếu muốn đóng "êm".

### 6.4 TIME_WAIT và `SO_REUSEADDR`

Bên **đóng kết nối trước** sẽ giữ trạng thái **TIME_WAIT** một thời gian (cỡ vài chục giây đến vài phút) để xử lý các gói đến muộn. Xem bằng lệnh `ss` trên Linux:

*Ví dụ kết quả* (sau khi chạy `tcp_echo_client` — client đóng trước, cổng tạm của nó ở trạng thái TIME_WAIT):

```text
$ ss -tan state time-wait
Recv-Q Send-Q Local Address:Port  Peer Address:Port
0      0          127.0.0.1:53868    127.0.0.1:9500
```

Nếu **server** là bên đóng trước (thường gặp) rồi bạn tắt và bật lại server ngay, `bind` có thể báo **Address already in use**. Giải pháp trên Linux/macOS: đặt `SO_REUSEADDR` **trước** `bind` (như trong mọi server của tài liệu).

> ⚠️ **Trên Windows, `SO_REUSEADDR` có ý nghĩa khác:** nó cho phép một socket khác bind **chồng** lên cổng đang được dùng — có thể bị lợi dụng để "cướp" cổng. Trên Windows, server thường bind lại được cổng của mình mà không bị TIME_WAIT cản trở như Linux, nên các server trong tài liệu **không** đặt tùy chọn này trên Windows. Nếu muốn chắc chắn không ai bind chồng, dùng `SO_EXCLUSIVEADDRUSE`.

### 6.5 Bảng lỗi cần biết

| POSIX | Mã Linux | Winsock | Mã | Ý nghĩa và cách xử lý |
|---|---|---|---|---|
| `ECONNREFUSED` | 111 | `WSAECONNREFUSED` | 10061 | Không ai nghe cổng đó → kiểm tra server, cổng, IPv4/IPv6 |
| `EADDRINUSE` | 98 | `WSAEADDRINUSE` | 10048 | Cổng đang bận → đổi cổng, tắt tiến trình cũ, `SO_REUSEADDR` (POSIX) |
| `ECONNRESET` | 104 | `WSAECONNRESET` | 10054 | Bên kia gửi RST → đóng socket |
| `EPIPE` | 32 | `WSAECONNRESET` / `WSAECONNABORTED` | 10054 / 10053 | Ghi vào kết nối đã hỏng → đóng socket |
| `ETIMEDOUT` | 110 | `WSAETIMEDOUT` | 10060 | Hết thời gian (kết nối hoặc `SO_RCVTIMEO` trên Windows) |
| `EAGAIN` / `EWOULDBLOCK` | 11 | `WSAEWOULDBLOCK` | 10035 | Socket không chặn chưa sẵn sàng → thử lại sau |
| `EINPROGRESS` | 115 | `WSAEWOULDBLOCK` | 10035 | `connect` không chặn đang tiến hành |
| `ENOTCONN` | 107 | `WSAENOTCONN` | 10057 | Socket chưa kết nối |
| `EHOSTUNREACH` | 113 | `WSAEHOSTUNREACH` | 10065 | Không có đường tới máy đích |
| `EMSGSIZE` | 90 | `WSAEMSGSIZE` | 10040 | Datagram UDP quá lớn so với bộ đệm |
| `EINTR` | 4 | `WSAEINTR` | 10004 | Lời gọi bị tín hiệu cắt ngang → thường chỉ cần gọi lại |
| — | — | `WSANOTINITIALISED` | 10093 | Quên `WSAStartup` |

(Mã số POSIX khác nhau giữa các hệ điều hành — macOS dùng số khác Linux — nên hãy so sánh bằng **tên hằng**, không bằng số.)

---

## Bài tập Phần II

**Bài II.1.** Viết **time server** TCP: mỗi client kết nối nhận một dòng chứa giờ hiện tại của máy chủ, rồi server đóng kết nối. Thử bằng `nc`.

**Bài II.2.** Viết hàm `recv_line(socket, bộ đệm, out, cap)` đọc **từng dòng** từ socket TCP, xử lý đúng trường hợp nhiều dòng đến cùng lúc và một dòng đến làm nhiều lần. Thử bằng một cặp socket trong cùng chương trình.

**Bài II.3.** Client gửi 3 thông điệp văn bản bằng 3 lần `send`, không có ký tự phân cách hay tiền tố độ dài. Chương trình "thỉnh thoảng" chạy sai khi triển khai thật. Giải thích và đề xuất 2 cách sửa.

**Bài II.4.** Server đọc tiền tố độ dài 4 byte, rồi `malloc` đúng kích thước đó để nhận nội dung. Chỉ ra lỗ hổng.

<details>
<summary><b>Lời giải Phần II</b> (bấm để mở)</summary>

**II.1.**

```c
/* file: time_server.c — mỗi client kết nối nhận một dòng giờ hiện tại rồi bị đóng kết nối */
#include "net.h"
#include <time.h>

int main(int argc, char *argv[]) {
    const char *port = argc > 1 ? argv[1] : "9013";
    if (net_init() != 0) return 1;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, port, &hints, &res) != 0) return 1;
    socket_t lst = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (lst == NET_INVALID || bind(lst, res->ai_addr, (socklen_t)res->ai_addrlen) != 0 ||
        listen(lst, SOMAXCONN) != 0) {
        net_perror("listen");
        return 1;
    }
    freeaddrinfo(res);
    for (;;) {
        socket_t c = accept(lst, NULL, NULL);
        if (c == NET_INVALID) continue;
        time_t now = time(NULL);
        char line[64];
        size_t len = strftime(line, sizeof line, "Gio may chu: %Y-%m-%d %H:%M:%S\n", localtime(&now));
        send(c, line, (int)len, 0);     /* dòng rất ngắn: một lần send là đủ trong thực tế,
                                           nhưng code nghiêm túc nên dùng send_all */
        net_close(c);
    }
}
```

*Ví dụ kết quả:*

```text
$ nc 127.0.0.1 9013
Gio may chu: 2026-09-17 04:18:10
```

**II.2.** Ý tưởng: giữ một **bộ đệm riêng cho mỗi kết nối**; nếu trong bộ đệm đã có `'\n'` thì trả dòng đó ra (và dời phần còn lại lên đầu), nếu chưa thì `recv` thêm.

```c
#include "net.h"

#define LINE_CAP 64

typedef struct {
    char data[LINE_CAP];
    size_t len;
} LineBuf;

/* Đọc một dòng (không gồm '\n') vào out.
   Trả về 1 nếu có dòng, 0 nếu bên kia đóng, -1 nếu lỗi hoặc dòng quá dài. */
static int recv_line(socket_t s, LineBuf *lb, char *out, size_t cap) {
    for (;;) {
        char *nl = memchr(lb->data, '\n', lb->len);
        if (nl != NULL) {                                   /* đã có đủ một dòng trong bộ đệm */
            size_t n = (size_t)(nl - lb->data);
            if (n >= cap) return -1;
            memcpy(out, lb->data, n);
            out[n] = '\0';
            lb->len -= n + 1;
            memmove(lb->data, nl + 1, lb->len);             /* giữ lại phần sau dòng */
            return 1;
        }
        if (lb->len == sizeof lb->data) return -1;          /* đầy mà chưa có '\n' */
        net_ssize_t r = recv(s, lb->data + lb->len, (int)(sizeof lb->data - lb->len), 0);
        if (r == 0) return 0;
        if (r < 0) return -1;
        lb->len += (size_t)r;
    }
}

static int make_tcp_pair(socket_t *client, socket_t *server) {
    socket_t lst = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    socklen_t len = sizeof addr;
    if (lst == NET_INVALID || bind(lst, (struct sockaddr *)&addr, sizeof addr) != 0 ||
        listen(lst, 1) != 0 || getsockname(lst, (struct sockaddr *)&addr, &len) != 0)
        return -1;
    *client = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(*client, (struct sockaddr *)&addr, sizeof addr) != 0) return -1;
    *server = accept(lst, NULL, NULL);
    net_close(lst);
    return *server == NET_INVALID ? -1 : 0;
}

int main(void) {
    if (net_init() != 0) return 1;
    socket_t cli, srv;
    if (make_tcp_pair(&cli, &srv) != 0) return 1;

    send(cli, "dong 1\ndong 2\nnua dau ", 22, 0);   /* 2 dòng đủ + nửa dòng */
    net_sleep_ms(100);
    send(cli, "nua sau\n", 8, 0);
    net_close(cli);                                  /* rồi đóng kết nối */

    LineBuf lb = {.len = 0};
    char line[LINE_CAP];
    int r;
    while ((r = recv_line(srv, &lb, line, sizeof line)) == 1)
        printf("dong: \"%s\"\n", line);
    printf("ket thuc: %s\n", r == 0 ? "ben kia da dong" : "loi");
    net_close(srv);
    net_cleanup();
    return 0;
}
```

**Kết quả** (Linux và Windows giống nhau):

```text
dong: "dong 1"
dong: "dong 2"
dong: "nua dau nua sau"
ket thuc: ben kia da dong
```

**II.3.** TCP là dòng byte, nên 3 thông điệp có thể bị **gộp** hoặc **cắt** tùy điều kiện mạng; trên loopback thì thường "may mắn" chạy đúng. Sửa: (1) kết thúc mỗi thông điệp bằng ký tự phân cách như `\n` và đọc theo dòng; (2) thêm tiền tố độ dài và dùng `recv_all`.

**II.4.** Kẻ tấn công gửi độ dài rất lớn (ví dụ 4 GB) → server cấp phát hết bộ nhớ hoặc bị treo chờ dữ liệu không bao giờ đến. Phải **kiểm tra độ dài với một giới hạn hợp lý** trước khi cấp phát, và đặt **timeout** cho việc nhận.

</details>

---

# PHẦN III — UDP

## Chương 7. UDP: datagram, mất gói và timeout

### 7.1 Đặc điểm khi lập trình

| Đặc điểm | Hệ quả khi viết code |
|---|---|
| **Không kết nối** | Không `listen`/`accept`; mỗi `recvfrom` cho biết **ai** gửi, trả lời bằng `sendto` về đúng địa chỉ đó |
| **Giữ ranh giới thông điệp** | Một `sendto` ↔ một `recvfrom` (nếu gói tới nơi) — **không cần đóng khung** |
| **Có thể mất, trùng, sai thứ tự** | Ứng dụng tự đánh **số thứ tự**, tự đặt **timeout**, tự quyết định có gửi lại không |
| **Kích thước giới hạn** | Nên giữ mỗi datagram dưới khoảng **1200 byte** để tránh bị phân mảnh trên đường đi |
| **Bộ đệm nhận nhỏ hơn datagram** | Phần thừa **bị cắt bỏ** (Windows báo lỗi `WSAEMSGSIZE`) → luôn dùng bộ đệm đủ lớn |

**`connect()` với UDP:** không bắt tay, chỉ **ghi nhớ địa chỉ đích**. Sau đó dùng được `send`/`recv`, và socket chỉ nhận gói từ đúng địa chỉ đó. Thêm một khác biệt: nếu đích trả về thông báo ICMP "port unreachable", lời gọi `recv` sau đó sẽ báo lỗi — trên Linux là `ECONNREFUSED` (đã kiểm tra); trên Windows, tài liệu của Microsoft mô tả lỗi này là `WSAECONNRESET`, kể cả với socket UDP **không** `connect`.

### 7.2 Chương trình: UDP ping có giả lập mất gói

**Server** — trả lời `ping N` bằng `pong N`, và cố tình bỏ mỗi gói thứ K:

```c
/* file: udp_ping_server.c — trả lời "ping N" bằng "pong N"; có thể giả lập mất gói
   Cách dùng: ./udp_ping_server [cong] [bo moi goi thu K]   (K = 0: không bỏ gói nào) */
#include "net.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
    const char *port = argc > 1 ? argv[1] : "9001";
    int drop_every = argc > 2 ? atoi(argv[2]) : 0;
    if (net_init() != 0) return 1;

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;       /* UDP */
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, port, &hints, &res) != 0) return 1;
    socket_t s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s == NET_INVALID || bind(s, res->ai_addr, (socklen_t)res->ai_addrlen) != 0) {
        net_perror("bind");
        return 1;
    }
    freeaddrinfo(res);
    printf("UDP ping server o cong %s, bo moi goi thu %d\n", port, drop_every);
    fflush(stdout);

    long count = 0;
    for (;;) {
        char buf[512];
        struct sockaddr_storage from;             /* địa chỉ người gửi — để biết trả lời cho ai */
        socklen_t from_len = sizeof from;
        net_ssize_t n = recvfrom(s, buf, sizeof buf - 1, 0, (struct sockaddr *)&from, &from_len);
        if (n < 0) { net_perror("recvfrom"); continue; }
        buf[n] = '\0';
        count++;

        int seq;
        if (sscanf(buf, "ping %d", &seq) != 1) continue;          /* bỏ qua gói không hợp lệ */
        if (drop_every > 0 && count % drop_every == 0) {
            printf("  (gia lap mat goi: bo ping %d)\n", seq);
            fflush(stdout);
            continue;
        }
        char reply[64];
        int len = snprintf(reply, sizeof reply, "pong %d", seq);
        sendto(s, reply, len, 0, (struct sockaddr *)&from, from_len);
    }
}
```

**Client** — gửi N gói, chờ mỗi gói tối đa 300 ms:

```c
/* file: udp_ping_client.c — gửi N gói ping, chờ pong có hạn thời gian, thống kê mất gói
   Cách dùng: ./udp_ping_client <host> <cong> <so goi> */
#include "net.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 4) { fprintf(stderr, "Cach dung: %s <host> <cong> <so goi>\n", argv[0]); return 1; }
    int total = atoi(argv[3]);
    if (net_init() != 0) return 1;

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    int err = getaddrinfo(argv[1], argv[2], &hints, &res);
    if (err != 0) { fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err)); return 1; }

    socket_t s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    /* connect() với UDP: chỉ ghi nhớ địa chỉ đích, KHÔNG bắt tay.
       Sau đó dùng send/recv, và chỉ nhận gói từ đúng địa chỉ này. */
    if (s == NET_INVALID || connect(s, res->ai_addr, (socklen_t)res->ai_addrlen) != 0) {
        net_perror("connect");
        return 1;
    }
    freeaddrinfo(res);
    net_set_recv_timeout(s, 300);                 /* chờ pong tối đa 300 ms */

    int received = 0;
    for (int seq = 1; seq <= total; seq++) {
        char msg[64], reply[64];
        int len = snprintf(msg, sizeof msg, "ping %d", seq);
        double t0 = net_now_ms();
        send(s, msg, len, 0);

        net_ssize_t n = recv(s, reply, sizeof reply - 1, 0);
        if (n < 0) {
            if (net_is_timeout(net_last_error())) printf("ping %d: het thoi gian cho\n", seq);
            else net_perror("recv");
            continue;
        }
        reply[n] = '\0';
        int rseq;
        if (sscanf(reply, "pong %d", &rseq) == 1 && rseq == seq) {
            received++;
            double rtt = net_now_ms() - t0;
            printf("ping %d: nhan \"%s\"%s\n", seq, reply, rtt < 50 ? " (RTT < 50 ms)" : "");
        } else {
            printf("ping %d: nhan goi khong khop \"%s\"\n", seq, reply);  /* gói cũ đến muộn */
        }
    }
    printf("Da gui %d, nhan %d, mat %d%%\n", total, received, (total - received) * 100 / total);
    net_close(s);
    net_cleanup();
    return 0;
}
```

**Chạy thử** (server bỏ mỗi gói thứ 3):

```bash
./udp_ping_server 9001 3          # Terminal 1
./udp_ping_client 127.0.0.1 9001 6   # Terminal 2
```

**Kết quả ở Terminal 2** (Linux và Windows giống nhau):

```text
ping 1: nhan "pong 1" (RTT < 50 ms)
ping 2: nhan "pong 2" (RTT < 50 ms)
ping 3: het thoi gian cho
ping 4: nhan "pong 4" (RTT < 50 ms)
ping 5: nhan "pong 5" (RTT < 50 ms)
ping 6: het thoi gian cho
Da gui 6, nhan 4, mat 33%
```

**Kết quả ở Terminal 1:**

```text
UDP ping server o cong 9001, bo moi goi thu 3
  (gia lap mat goi: bo ping 3)
  (gia lap mat goi: bo ping 6)
```

**Vì sao client kiểm tra số thứ tự?** Nếu pong của ping 3 đến **muộn** (sau khi client đã hết giờ chờ và gửi ping 4), lần `recv` tiếp theo sẽ nhận `pong 3` thay vì `pong 4`. Không kiểm tra số thứ tự, client sẽ tính sai. Đây là lý do mọi giao thức trên UDP đều có số thứ tự.

**Muốn tin cậy trên UDP?** Ý tưởng tối thiểu: đánh số mỗi gói → bên nhận gửi xác nhận (ACK) → bên gửi đặt timeout, không có ACK thì gửi lại → bên nhận bỏ gói trùng. Thực tế, hãy dùng thư viện đã được kiểm chứng (ENet, GameNetworkingSockets, QUIC) thay vì tự viết từ đầu.

---

## Chương 8. Socket không chặn và vòng lặp game

### 8.1 Chế độ không chặn

Với socket **không chặn** (non-blocking), các lời gọi **không bao giờ chờ**: nếu chưa có dữ liệu, `recv` trả về lỗi `EWOULDBLOCK` (Windows: `WSAEWOULDBLOCK`) ngay lập tức. Đây **không phải lỗi thật**, chỉ là "chưa có gì, quay lại sau".

| Lời gọi | Khi chưa sẵn sàng, socket không chặn sẽ... |
|---|---|
| `recv` / `recvfrom` | Trả lỗi "would block" |
| `send` | Gửi được một phần, hoặc trả lỗi "would block" nếu bộ đệm gửi đầy |
| `accept` | Trả lỗi "would block" nếu chưa có kết nối chờ |
| `connect` | Trả lỗi `EINPROGRESS` (Windows: `WSAEWOULDBLOCK`) — việc bắt tay tiếp tục ở nền |

### 8.2 Chương trình: xử lý gói tin trong vòng lặp game

Vòng lặp game phải chạy đều đặn (ví dụ 60 lần/giây) — không được đứng chờ mạng. Mẫu chuẩn: **mỗi tick, đọc hết các gói đang chờ, rồi làm việc khác.**

```c
#include "net.h"

int main(void) {
    if (net_init() != 0) return 1;

    /* Socket UDP gắn vào 127.0.0.1, cổng do hệ điều hành chọn */
    socket_t s = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in me;
    memset(&me, 0, sizeof me);
    me.sin_family = AF_INET;
    me.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    socklen_t len = sizeof me;
    if (s == NET_INVALID || bind(s, (struct sockaddr *)&me, sizeof me) != 0 ||
        getsockname(s, (struct sockaddr *)&me, &len) != 0) {
        net_perror("setup");
        return 1;
    }
    net_set_nonblocking(s, 1);    /* recvfrom sẽ KHÔNG chờ nếu chưa có dữ liệu */

    /* Mô phỏng vòng lặp game: mỗi "tick" xử lý hết gói tin đang chờ rồi làm việc khác */
    for (int tick = 1; tick <= 4; tick++) {
        if (tick == 2) {          /* giả lập: có 3 gói tin tới trước tick 2 */
            sendto(s, "move L", 6, 0, (struct sockaddr *)&me, sizeof me);
            sendto(s, "move R", 6, 0, (struct sockaddr *)&me, sizeof me);
            sendto(s, "jump", 4, 0, (struct sockaddr *)&me, sizeof me);
        }
        printf("tick %d:", tick);
        int count = 0;
        for (;;) {
            char buf[64];
            net_ssize_t n = recvfrom(s, buf, sizeof buf - 1, 0, NULL, NULL);
            if (n < 0) {
                int err = net_last_error();
                if (net_would_block(err)) break;       /* hết gói tin -> thoát, không chờ */
                net_perror("recvfrom");
                return 1;
            }
            buf[n] = '\0';
            printf(" [%s]", buf);
            count++;
        }
        printf("%s -> cap nhat the gioi, ve khung hinh...\n", count ? "" : " (khong co goi tin)");
    }
    net_close(s);
    net_cleanup();
    return 0;
}
```

**Kết quả** (Linux và Windows giống nhau):

```text
tick 1: (khong co goi tin) -> cap nhat the gioi, ve khung hinh...
tick 2: [move L] [move R] [jump] -> cap nhat the gioi, ve khung hinh...
tick 3: (khong co goi tin) -> cap nhat the gioi, ve khung hinh...
tick 4: (khong co goi tin) -> cap nhat the gioi, ve khung hinh...
```

### 8.3 Khung sườn của một game server UDP

```c
/* Mô phỏng: server có thẩm quyền, cập nhật 30 lần mỗi giây (đoạn mã minh họa, không đầy đủ) */
const double TICK_MS = 1000.0 / 30;
double next_tick = net_now_ms();
for (;;) {
    /* 1) Đọc hết input của người chơi (socket không chặn) */
    while ((n = recvfrom(s, buf, sizeof buf, 0, (struct sockaddr *)&from, &len)) >= 0)
        handle_input(&from, buf, n);          /* kiểm tra hợp lệ, KHÔNG tin client */

    /* 2) Mô phỏng thế giới đúng một bước cố định */
    simulate(TICK_MS / 1000.0);

    /* 3) Gửi ảnh chụp trạng thái (snapshot) cho từng người chơi */
    for (each player) sendto(s, snapshot, snapshot_len, 0, player_addr, player_addr_len);

    /* 4) Ngủ đến tick kế tiếp */
    next_tick += TICK_MS;
    double wait = next_tick - net_now_ms();
    if (wait > 0) net_sleep_ms((int)wait);
}
```

Thay vì ngủ cố định, server thật thường dùng `poll` với **timeout bằng thời gian còn lại đến tick kế tiếp** — vừa thức dậy ngay khi có gói tin, vừa không bỏ lỡ tick.

---

# PHẦN IV — NHIỀU CLIENT

## Chương 9. Các mô hình xử lý đồng thời

| Mô hình | Cách làm | Ưu | Nhược | Hợp với |
|---|---|---|---|---|
| **Lần lượt (iterative)** | Phục vụ xong client này mới `accept` client khác | Đơn giản nhất | Một client chậm làm **tất cả** phải chờ | Học tập, công cụ nội bộ |
| **Mỗi client một luồng** | `accept` xong tạo luồng mới | Code tuần tự, dễ hiểu | Tốn bộ nhớ và chi phí chuyển ngữ cảnh khi có hàng nghìn client; phải đồng bộ dữ liệu chung | Vài chục – vài trăm client |
| **Vòng lặp sự kiện (`poll`/`epoll`/IOCP)** | **Một** luồng theo dõi nhiều socket, chỉ xử lý socket nào **sẵn sàng** | Phục vụ rất nhiều kết nối với ít tài nguyên | Code phải chia thành các bước nhỏ, lưu trạng thái cho từng kết nối | Chat, game server, web server |
| **Thread pool + sự kiện** | Vài luồng, mỗi luồng một vòng lặp sự kiện | Tận dụng đa lõi | Phức tạp nhất | Server hiệu năng cao |

**Thí nghiệm:** client 1 kết nối rồi "ngồi im"; client 2 kết nối và gửi `hi`.

```python
# file: concurrency_test.py — so sánh server xử lý lần lượt và server mỗi client một luồng
import socket, sys

port = int(sys.argv[1])
c1 = socket.create_connection(("127.0.0.1", port))   # client 1 kết nối rồi "ngồi im"
c2 = socket.create_connection(("127.0.0.1", port))   # client 2 kết nối (vào hàng đợi của hệ điều hành)
c2.settimeout(1.0)
c2.sendall(b"hi")
try:
    print("client 2 nhan duoc ngay:", c2.recv(100).decode())
except socket.timeout:
    print("client 2: KHONG co phan hoi sau 1 giay (server dang ban voi client 1)")
    c1.close()                                        # client 1 rời đi...
    print("client 1 roi di -> client 2 moi nhan duoc:", c2.recv(100).decode())
c1.close()
c2.close()
```

**Kết quả** với server lần lượt (`tcp_echo_server`, Chương 4):

```text
client 2: KHONG co phan hoi sau 1 giay (server dang ban voi client 1)
client 1 roi di -> client 2 moi nhan duoc: hi
```

**Kết quả** với server mỗi client một luồng (`echo_threads`, Chương 13):

```text
client 2 nhan duoc ngay: hi
```

---

## Chương 10. `poll` và chat server nhiều người dùng

### 10.1 Ý tưởng

`poll` nhận một **mảng** socket kèm sự kiện cần theo dõi, rồi **chờ đến khi ít nhất một socket sẵn sàng** (hoặc hết timeout):

```c
struct pollfd {
    socket_t fd;       /* socket cần theo dõi */
    short events;      /* muốn biết điều gì: POLLIN (đọc được), POLLOUT (ghi được) */
    short revents;     /* poll điền vào: điều gì ĐÃ xảy ra (POLLIN, POLLHUP, POLLERR...) */
};
int poll(struct pollfd *fds, nfds_t n, int timeout_ms);   /* Windows: WSAPoll */
```

| Socket | `POLLIN` có nghĩa là |
|---|---|
| Socket lắng nghe | Có kết nối mới → `accept` sẽ không chặn |
| Socket kết nối | Có dữ liệu → `recv` sẽ không chặn (hoặc bên kia đã đóng → `recv` trả 0) |

**`select`** là hàm cũ hơn cùng mục đích, nhưng bị giới hạn số socket (thường 1024 trên Linux) và khó dùng hơn — nên ưu tiên `poll`.

### 10.2 Chat server

**Giao thức:** mỗi dòng văn bản là một thông điệp. Lệnh `/nick <tên>` đổi tên, `/quit` thoát.

```c
/* file: chat_server.c — phòng chat nhiều người, MỘT luồng, dùng poll() (WSAPoll trên Windows)
   Giao thức: mỗi dòng văn bản kết thúc bằng '\n' là một thông điệp.
   Lệnh: /nick <ten>  đổi tên;  /quit  thoát.
   Cách dùng: ./chat_server [cong]   (mặc định 9000) */
#include "net.h"
#include <stdlib.h>

#define MAX_CLIENTS  32
#define MAX_LINE     512         /* dòng dài hơn giới hạn này -> ngắt kết nối (chống lạm dụng) */

typedef struct {
    socket_t fd;
    char name[32];
    char buf[MAX_LINE];          /* bộ đệm nhận: có thể đang chứa nửa dòng */
    size_t len;
} Client;

static Client clients[MAX_CLIENTS];
static int nclients = 0;
static int next_id = 1;

static void send_all(socket_t s, const char *data, size_t len) {
    while (len > 0) {
        net_ssize_t n = send(s, data, (int)len, 0);
        if (n <= 0) return;                    /* lỗi: client này sẽ bị phát hiện và xóa ở lần poll sau */
        data += n;
        len -= (size_t)n;
    }
}

/* Gửi cho mọi client, trừ client có chỉ số `except` (-1 = gửi cho tất cả) */
static void broadcast(int except, const char *msg) {
    for (int i = 0; i < nclients; i++)
        if (i != except) send_all(clients[i].fd, msg, strlen(msg));
    printf("%s", msg);                          /* ghi log ra màn hình server */
    fflush(stdout);
}

static void remove_client(int i, const char *reason) {
    char msg[128];
    snprintf(msg, sizeof msg, "* %s roi phong (%s)\n", clients[i].name, reason);
    net_close(clients[i].fd);
    clients[i] = clients[nclients - 1];        /* đưa phần tử cuối vào chỗ trống */
    nclients--;
    broadcast(-1, msg);
}

/* Xử lý một dòng hoàn chỉnh. Trả về 0 nếu client đã bị xóa. */
static int handle_line(int i, char *line) {
    size_t n = strlen(line);
    if (n > 0 && line[n - 1] == '\r') line[--n] = '\0';   /* chấp nhận cả "\r\n" */
    if (n == 0) return 1;

    char msg[MAX_LINE + 64];
    if (strncmp(line, "/nick ", 6) == 0 && line[6] != '\0') {
        char old[32];
        snprintf(old, sizeof old, "%s", clients[i].name);
        snprintf(clients[i].name, sizeof clients[i].name, "%.31s", line + 6);
        snprintf(msg, sizeof msg, "* %s doi ten thanh %s\n", old, clients[i].name);
        broadcast(-1, msg);
    } else if (strcmp(line, "/quit") == 0) {
        remove_client(i, "tu thoat");
        return 0;
    } else {
        snprintf(msg, sizeof msg, "[%s] %s\n", clients[i].name, line);
        broadcast(i, msg);                     /* không gửi lại cho người nói */
    }
    return 1;
}

/* Đọc dữ liệu mới của client i và tách thành các dòng */
static void handle_readable(int i) {
    Client *c = &clients[i];
    net_ssize_t n = recv(c->fd, c->buf + c->len, (int)(sizeof c->buf - c->len), 0);
    if (n <= 0) {
        remove_client(i, n == 0 ? "mat ket noi" : "loi");
        return;
    }
    c->len += (size_t)n;

    char *start = c->buf;
    char *nl;
    while ((nl = memchr(start, '\n', c->len - (size_t)(start - c->buf))) != NULL) {
        *nl = '\0';
        if (!handle_line(i, start)) return;   /* client đã bị xóa -> dừng */
        start = nl + 1;
    }
    /* Dời phần dòng còn dang dở về đầu bộ đệm */
    c->len -= (size_t)(start - c->buf);
    memmove(c->buf, start, c->len);
    if (c->len == sizeof c->buf) remove_client(i, "dong qua dai");
}

static socket_t open_listener(const char *port) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, port, &hints, &res) != 0) return NET_INVALID;
    socket_t s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
#ifndef _WIN32
    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof yes);
#endif
    if (s == NET_INVALID || bind(s, res->ai_addr, (socklen_t)res->ai_addrlen) != 0 ||
        listen(s, SOMAXCONN) != 0) {
        net_perror("listen");
        s = NET_INVALID;
    }
    freeaddrinfo(res);
    return s;
}

int main(int argc, char *argv[]) {
    const char *port = argc > 1 ? argv[1] : "9000";
    if (net_init() != 0) return 1;
    socket_t lst = open_listener(port);
    if (lst == NET_INVALID) return 1;
    printf("Chat server dang chay o cong %s\n", port);
    fflush(stdout);

    struct pollfd fds[MAX_CLIENTS + 1];
    for (;;) {
        /* 1) Lập danh sách socket cần theo dõi: [0] = socket lắng nghe, [1..] = các client */
        fds[0].fd = lst;
        fds[0].events = POLLIN;
        for (int i = 0; i < nclients; i++) {
            fds[i + 1].fd = clients[i].fd;
            fds[i + 1].events = POLLIN;
        }
        int count = nclients;

        /* 2) Chờ đến khi ÍT NHẤT MỘT socket có việc (-1 = chờ vô hạn) */
        if (net_poll(fds, count + 1, -1) < 0) { net_perror("poll"); break; }

        /* 3) Xử lý client — duyệt NGƯỢC để việc xóa (đổi chỗ với phần tử cuối) không làm lệch chỉ số */
        for (int i = count - 1; i >= 0; i--)
            if (fds[i + 1].revents & (POLLIN | POLLHUP | POLLERR)) handle_readable(i);

        /* 4) Socket lắng nghe "đọc được" nghĩa là có kết nối mới đang chờ accept */
        if (fds[0].revents & POLLIN) {
            socket_t fd = accept(lst, NULL, NULL);
            if (fd == NET_INVALID) continue;
            if (nclients == MAX_CLIENTS) {
                send_all(fd, "Phong day, thu lai sau\n", 23);
                net_close(fd);
                continue;
            }
            Client *c = &clients[nclients++];
            c->fd = fd;
            c->len = 0;
            snprintf(c->name, sizeof c->name, "khach%d", next_id++);
            char msg[128];
            snprintf(msg, sizeof msg, "Chao %s! Lenh: /nick <ten>, /quit\n", c->name);
            send_all(fd, msg, strlen(msg));
            snprintf(msg, sizeof msg, "* %s vao phong (%d nguoi)\n", c->name, nclients);
            broadcast(nclients - 1, msg);
        }
    }
    net_close(lst);
    net_cleanup();
    return 0;
}
```

**Những kỹ thuật quan trọng trong code:**

- **Mỗi client có bộ đệm riêng**, vì một dòng có thể đến làm nhiều lần (TCP là dòng byte).
- **Giới hạn độ dài dòng**: client gửi dữ liệu mãi mà không xuống dòng sẽ bị ngắt, thay vì làm tràn bộ nhớ.
- **Duyệt ngược** mảng client, và `accept` **sau khi** xử lý client, để việc xóa/thêm client không làm lệch chỉ số giữa mảng `fds` và mảng `clients`.
- Xử lý `POLLHUP` và `POLLERR` cùng với `POLLIN`: `recv` sẽ trả về 0 hoặc lỗi, và client được xóa.

### 10.3 Kiểm thử tự động

Thay vì mở nhiều cửa sổ `nc` và gõ tay, hãy viết một script mô phỏng nhiều người dùng. Script dưới đây còn thử cả trường hợp **một dòng bị gửi làm hai nửa** và **ngắt kết nối đột ngột**:

```python
# file: chat_test.py — kiểm thử tự động chat server: mô phỏng 3 người dùng
import socket, sys, time

HOST, PORT = "127.0.0.1", int(sys.argv[1]) if len(sys.argv) > 1 else 9000
users = []

def connect(name):
    s = socket.create_connection((HOST, PORT))
    s.settimeout(0.1)
    user = {"name": name, "sock": s, "log": []}
    users.append(user)
    step()
    return user

def step():
    time.sleep(0.2)                       # chờ server xử lý
    for u in users:                       # đọc hết dữ liệu đang chờ của mọi người dùng còn mở
        if u["sock"] is None:
            continue
        data = b""
        try:
            while chunk := u["sock"].recv(4096):
                data += chunk
        except socket.timeout:
            pass
        u["log"] += data.decode().splitlines()

def send(user, text):
    user["sock"].sendall(text.encode())
    step()

def close(user):
    user["sock"].close()
    user["sock"] = None
    step()

a = connect("A")
b = connect("B")
send(a, "/nick An\n")
send(b, "/nick Binh\n")
send(a, "Xin chao moi nguoi\n")
send(a, "Tin nhan bi ")                  # gửi NỬA dòng...
send(a, "chia doi\n")                    # ...rồi phần còn lại
c = connect("C")
send(c, "/quit\n")
send(b, "Tam biet\n")
close(b)                                 # đóng đột ngột, không gửi /quit
close(a)

for u in (a, b):
    print(f"--- Man hinh cua {u['name']} ---")
    print("\n".join(u["log"]))
```

**Kết quả** (`python3 chat_test.py 9000` khi server đang chạy):

```text
--- Man hinh cua A ---
Chao khach1! Lenh: /nick <ten>, /quit
* khach2 vao phong (2 nguoi)
* khach1 doi ten thanh An
* khach2 doi ten thanh Binh
* khach3 vao phong (3 nguoi)
* khach3 roi phong (tu thoat)
[Binh] Tam biet
* Binh roi phong (mat ket noi)
--- Man hinh cua B ---
Chao khach2! Lenh: /nick <ten>, /quit
* khach1 doi ten thanh An
* khach2 doi ten thanh Binh
[An] Xin chao moi nguoi
[An] Tin nhan bi chia doi
* khach3 vao phong (3 nguoi)
* khach3 roi phong (tu thoat)
```

**Log của server:**

```text
Chat server dang chay o cong 9000
* khach1 vao phong (1 nguoi)
* khach2 vao phong (2 nguoi)
* khach1 doi ten thanh An
* khach2 doi ten thanh Binh
[An] Xin chao moi nguoi
[An] Tin nhan bi chia doi
* khach3 vao phong (3 nguoi)
* khach3 roi phong (tu thoat)
[Binh] Tam biet
* Binh roi phong (mat ket noi)
* An roi phong (mat ket noi)
```

Chạy cùng script với server bản Windows cho **bản ghi giống hệt từng ký tự**.

> 🔍 **Một hiện tượng phát hiện khi viết script:** phiên bản đầu của script **không đọc hết** dữ liệu của B trước khi đóng socket. Kết quả là server ghi `Binh roi phong (loi)` thay vì `(mat ket noi)` — vì đóng socket khi còn dữ liệu chưa đọc sẽ sinh **RST** (mục 6.3). Server vẫn xử lý đúng, nhưng đây là ví dụ thật cho thấy vì sao cần hiểu FIN và RST.

### 10.4 Hạn chế và cách nâng cấp

1. **`send` vẫn chặn.** Một client **không chịu đọc** sẽ làm bộ đệm gửi đầy, và `send_all` sẽ **treo cả server**. Cách sửa chuẩn: dùng socket không chặn, mỗi client có thêm **bộ đệm gửi** riêng; khi `send` báo "would block", giữ phần còn lại và đăng ký `POLLOUT` để gửi tiếp khi socket ghi được. Nếu bộ đệm gửi của client vượt ngưỡng → ngắt kết nối client đó.
2. **Không có timeout** cho client im lặng — có thể thêm thời điểm hoạt động cuối cho mỗi client và dùng timeout của `poll` để dọn dẹp định kỳ.
3. **Mỗi vòng lặp đều dựng lại mảng `fds`** — đơn giản nhưng tốn công khi có hàng nghìn kết nối; đó là lý do có `epoll`/`kqueue`/IOCP.
4. Trên Windows, một số phiên bản cũ của `WSAPoll` **không báo** khi `connect` không chặn thất bại — nếu cần theo dõi `connect`, hãy kiểm tra thêm bằng `getsockopt(SO_ERROR)` hoặc dùng `select`.

---

## Chương 11. `epoll`, `kqueue`, IOCP và thư viện

### 11.1 Vì sao cần API mới hơn `poll`?

Mỗi lần gọi, `poll` phải chép **toàn bộ** danh sách socket vào nhân và quét hết — chi phí tăng theo số kết nối. Các API hiện đại giữ danh sách **bên trong nhân** và chỉ trả về những socket **thật sự** có sự kiện:

| API | Hệ điều hành | Mô hình |
|---|---|---|
| `epoll` | Linux | **Sẵn sàng** (readiness): "socket này đọc được rồi, hãy gọi `recv`" |
| `kqueue` | macOS, BSD | Sẵn sàng |
| **IOCP** (I/O Completion Ports) | Windows | **Hoàn tất** (completion): "tôi đã nhận xong dữ liệu vào bộ đệm của bạn" |
| `io_uring` | Linux (mới) | Hoàn tất, hiệu năng rất cao |

**Kích hoạt theo mức và theo sườn (epoll):**

- **Theo mức (level-triggered, mặc định):** còn dữ liệu là còn được báo — giống `poll`, dễ dùng.
- **Theo sườn (edge-triggered, `EPOLLET`):** chỉ báo **một lần** khi có dữ liệu mới → phải đọc cho đến khi gặp "would block", nếu không sẽ không bao giờ được báo lại.

### 11.2 Nên dùng thư viện nào?

Viết trực tiếp trên `epoll`/IOCP cho cả hai nền tảng là rất tốn công. Trong dự án thực tế, hãy cân nhắc:

| Thư viện | Ngôn ngữ | Ghi chú |
|---|---|---|
| **Asio** (độc lập hoặc Boost.Asio) | C++ | Phổ biến nhất trong C++, bọc epoll/kqueue/IOCP, hỗ trợ coroutine |
| **libuv** | C | Nền tảng của Node.js |
| **libevent** | C | Lâu đời, ổn định |
| **ENet** | C | UDP tin cậy cho game, rất gọn |
| **GameNetworkingSockets** | C++ | Thư viện mạng game của Valve (UDP tin cậy, mã hóa) |

Hiểu socket thô như trong tài liệu này giúp bạn **dùng đúng** các thư viện trên và **gỡ lỗi** được khi có sự cố.

---

## Bài tập Phần III và IV

**Bài III.1.** Viết server UDP nhận một chuỗi và trả về số từ trong chuỗi. Thử bằng một client bất kỳ.

**Bài III.2.** Thêm lệnh `/who` vào chat server: trả về danh sách tên những người đang trong phòng, **chỉ gửi cho người hỏi**.

**Bài III.3.** Một client chat bị treo mạng (vẫn giữ kết nối nhưng không đọc dữ liệu). Sau một thời gian, **cả phòng chat** bị đứng. Giải thích.

**Bài III.4.** Vì sao với socket UDP không chặn, ta phải đọc **lặp** đến khi gặp "would block" thay vì chỉ đọc một gói mỗi tick?

<details>
<summary><b>Lời giải Phần III và IV</b> (bấm để mở)</summary>

**III.1.**

```c
/* file: wc_server.c — server UDP: nhận một chuỗi, trả về số từ trong chuỗi */
#include "net.h"
#include <ctype.h>

int main(int argc, char *argv[]) {
    const char *port = argc > 1 ? argv[1] : "9014";
    if (net_init() != 0) return 1;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, port, &hints, &res) != 0) return 1;
    socket_t s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s == NET_INVALID || bind(s, res->ai_addr, (socklen_t)res->ai_addrlen) != 0) {
        net_perror("bind");
        return 1;
    }
    freeaddrinfo(res);
    for (;;) {
        char buf[1024];
        struct sockaddr_storage from;
        socklen_t from_len = sizeof from;
        net_ssize_t n = recvfrom(s, buf, sizeof buf, 0, (struct sockaddr *)&from, &from_len);
        if (n < 0) continue;
        int words = 0, in_word = 0;
        for (net_ssize_t i = 0; i < n; i++) {        /* không cần '\0': dùng độ dài n */
            int space = isspace((unsigned char)buf[i]);
            if (!space && !in_word) words++;
            in_word = !space;
        }
        char reply[32];
        int len = snprintf(reply, sizeof reply, "%d tu", words);
        sendto(s, reply, len, 0, (struct sockaddr *)&from, from_len);
    }
}
```

*Thử bằng Python:*

```python
# file: wc_test.py — cần wc_server đang chạy ở cổng 9014
import socket
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.settimeout(1)
for m in ["xin chao the gioi", "   nhieu   khoang   trang  ", ""]:
    s.sendto(m.encode(), ("127.0.0.1", 9014))
    print(repr(m), "->", s.recv(100).decode())
```

```text
'xin chao the gioi' -> 4 tu
'   nhieu   khoang   trang  ' -> 3 tu
'' -> 0 tu
```

(Datagram rỗng vẫn là một thông điệp hợp lệ trong UDP.)

**III.2.** Thêm nhánh sau vào `handle_line`, ngay trước nhánh `/quit`:

```c
    } else if (strcmp(line, "/who") == 0) {
        char list[MAX_LINE + 64] = "* Trong phong:";
        for (int k = 0; k < nclients; k++) {
            strncat(list, " ", sizeof list - strlen(list) - 1);
            strncat(list, clients[k].name, sizeof list - strlen(list) - 1);
        }
        strncat(list, "\n", sizeof list - strlen(list) - 1);
        send_all(clients[i].fd, list, strlen(list));    /* chỉ gửi cho người hỏi */
```

Khi có `khach1` và một người đổi tên thành `Binh`, `khach1` gõ `/who` sẽ nhận:

```text
* Trong phong: khach1 Binh
```

**III.3.** Bộ đệm nhận của client treo đầy, rồi bộ đệm **gửi** của server cho client đó cũng đầy. Lời gọi `send_all` (chế độ chặn) trong `broadcast` **đứng chờ** mãi → vòng lặp `poll` duy nhất bị kẹt → mọi người khác cũng không được phục vụ. Sửa: socket không chặn + bộ đệm gửi riêng + ngắt kết nối client có hàng đợi quá lớn (mục 10.4).

**III.4.** Trong một tick có thể có **nhiều** gói đến. Nếu chỉ đọc một gói mỗi tick, gói tin sẽ dồn lại ngày càng nhiều: độ trễ tăng dần và cuối cùng bộ đệm nhận tràn, gói mới bị hệ điều hành **bỏ**.

</details>

---

# PHẦN V — TÙY CHỌN VÀ HIỆU NĂNG

## Chương 12. Timeout và các tùy chọn socket

Tùy chọn được đặt bằng `setsockopt(socket, level, tên, &giá_trị, kích_thước)`, thường **trước** khi dùng socket.

### 12.1 Timeout

| Nhu cầu | Cách làm |
|---|---|
| `recv` không chờ quá N giây | `SO_RCVTIMEO` (hàm `net_set_recv_timeout`) — hết giờ trả lỗi `EAGAIN` (Linux) / `WSAETIMEDOUT` (Windows) |
| `send` không chờ quá N giây | `SO_SNDTIMEO` |
| Chờ **nhiều** socket có giới hạn | Tham số `timeout` của `poll` |
| `connect` không chờ quá N giây | Kết nối ở chế độ không chặn + `poll` chờ `POLLOUT` + đọc `SO_ERROR` |

`connect` chặn mặc định có thể chờ **rất lâu** (hàng chục giây đến vài phút) nếu máy đích không trả lời. Chương trình dưới đây cài đặt `connect` có giới hạn thời gian, và tạo ra một tình huống "treo" thật bằng cách làm **đầy hàng đợi kết nối** của một server không bao giờ `accept`:

```c
#include "net.h"

/* connect() có giới hạn thời gian. Trả về 0 nếu kết nối được, -1 nếu lỗi/hết giờ
   (khi đó *err chứa mã lỗi). */
static int connect_with_timeout(socket_t s, const struct sockaddr *addr, socklen_t len,
                                int timeout_ms, int *err) {
    net_set_nonblocking(s, 1);
    if (connect(s, addr, len) == 0) {                  /* kết nối xong ngay (hiếm, thường với loopback) */
        net_set_nonblocking(s, 0);
        return 0;
    }
    *err = net_last_error();
#ifdef _WIN32
    if (*err != WSAEWOULDBLOCK) return -1;
#else
    if (*err != EINPROGRESS) return -1;
#endif
    struct pollfd p = {0};
    p.fd = s;
    p.events = POLLOUT;                                /* "ghi được" = bắt tay đã có kết quả */
    int r = net_poll(&p, 1, timeout_ms);
    if (r == 0) {
#ifdef _WIN32
        *err = WSAETIMEDOUT;
#else
        *err = ETIMEDOUT;
#endif
        return -1;
    }
    int so_error = 0;
    socklen_t optlen = sizeof so_error;                /* kết quả thật nằm trong SO_ERROR */
    getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)&so_error, &optlen);
    if (so_error != 0) {
        *err = so_error;
        return -1;
    }
    net_set_nonblocking(s, 0);
    return 0;
}

static void try_connect(const char *label, struct sockaddr_in *addr) {
    socket_t s = socket(AF_INET, SOCK_STREAM, 0);
    int err = 0;
    double t0 = net_now_ms();
    int r = connect_with_timeout(s, (struct sockaddr *)addr, sizeof *addr, 500, &err);
    double ms = net_now_ms() - t0;
    if (r == 0) printf("%-28s -> ket noi thanh cong\n", label);
    else printf("%-28s -> that bai sau %s: %s\n", label, ms >= 450 ? "~500 ms" : "< 100 ms",
                net_strerror(err));
    net_close(s);
}

int main(void) {
    if (net_init() != 0) return 1;

    /* Server "giả": lắng nghe nhưng KHÔNG BAO GIỜ accept, hàng đợi rất nhỏ */
    socket_t lst = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    socklen_t len = sizeof addr;
    bind(lst, (struct sockaddr *)&addr, sizeof addr);
    listen(lst, 1);
    getsockname(lst, (struct sockaddr *)&addr, &len);

    try_connect("server dang nghe", &addr);

    /* Lấp đầy hàng đợi kết nối của server */
    socket_t fill[8];
    for (int i = 0; i < 8; i++) {
        fill[i] = socket(AF_INET, SOCK_STREAM, 0);
        net_set_nonblocking(fill[i], 1);
        connect(fill[i], (struct sockaddr *)&addr, sizeof addr);
    }
    net_sleep_ms(200);
    try_connect("hang doi cua server da day", &addr);
    for (int i = 0; i < 8; i++) net_close(fill[i]);

    net_close(lst);
    try_connect("server da tat", &addr);

    net_cleanup();
    return 0;
}
```

**Kết quả** (Linux):

```text
server dang nghe             -> ket noi thanh cong
hang doi cua server da day   -> that bai sau ~500 ms: Connection timed out
server da tat                -> that bai sau < 100 ms: Connection refused
```

Bản Windows cho cùng kết quả (mã lỗi 10060 và 10061).

**Điểm mấu chốt:** khi `connect` không chặn, việc `poll` báo "ghi được" chỉ có nghĩa là **bắt tay đã có kết quả** — thành công hay thất bại phải đọc từ `getsockopt(SO_ERROR)`.

### 12.2 Nagle, ACK trễ và `TCP_NODELAY` — rất quan trọng với game

- **Thuật toán Nagle** (bật mặc định): nếu còn dữ liệu đã gửi mà chưa được xác nhận, TCP **giữ lại** các gói nhỏ tiếp theo để gộp chung — giảm số gói tí hon.
- **ACK trễ (delayed ACK)**: bên nhận không xác nhận ngay mà chờ một chút (Linux cỡ 40 ms; Windows mặc định có thể tới 200 ms) để gộp ACK vào dữ liệu trả lời.

Hai cơ chế này **"chờ nhau"** khi ứng dụng gửi yêu cầu bằng **nhiều lần `send` nhỏ** rồi chờ phản hồi:

```c
#include "net.h"

static int make_tcp_pair(socket_t *client, socket_t *server) {
    socket_t lst = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    socklen_t len = sizeof addr;
    if (lst == NET_INVALID || bind(lst, (struct sockaddr *)&addr, sizeof addr) != 0 ||
        listen(lst, 1) != 0 || getsockname(lst, (struct sockaddr *)&addr, &len) != 0)
        return -1;
    *client = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(*client, (struct sockaddr *)&addr, sizeof addr) != 0) return -1;
    *server = accept(lst, NULL, NULL);
    net_close(lst);
    return *server == NET_INVALID ? -1 : 0;
}

static void recv_exact(socket_t s, char *buf, int len) {
    while (len > 0) {
        net_ssize_t n = recv(s, buf, len, 0);
        if (n <= 0) return;
        buf += n;
        len -= (int)n;
    }
}

/* Đo thời gian trung bình của một lượt "yêu cầu – phản hồi",
   trong đó yêu cầu được gửi bằng HAI lần send nhỏ (header rồi body) */
static double measure(int nodelay) {
    socket_t cli, srv;
    if (make_tcp_pair(&cli, &srv) != 0) return -1;
    int on = nodelay;
    setsockopt(cli, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof on);

    const int rounds = 20;
    char buf[16];
    double total = 0;
    for (int i = 0; i < rounds; i++) {
        double t0 = net_now_ms();
        send(cli, "HD", 2, 0);              /* gói nhỏ thứ nhất: header */
        send(cli, "0123456789", 10, 0);     /* gói nhỏ thứ hai: body */
        recv_exact(srv, buf, 12);           /* "server" đọc đủ yêu cầu... */
        send(srv, "K", 1, 0);               /* ...rồi trả lời */
        recv_exact(cli, buf, 1);            /* "client" nhận phản hồi */
        total += net_now_ms() - t0;
    }
    net_close(cli);
    net_close(srv);
    return total / rounds;
}

int main(void) {
    if (net_init() != 0) return 1;
    printf("Nagle BAT  (mac dinh)  : trung binh %6.2f ms / luot\n", measure(0));
    printf("Nagle TAT  (TCP_NODELAY): trung binh %6.2f ms / luot\n", measure(1));
    net_cleanup();
    return 0;
}
```

**Ví dụ kết quả** (đo trên Linux; con số trên máy bạn sẽ khác):

```text
Nagle BAT  (mac dinh)  : trung binh  41.77 ms / luot
Nagle TAT  (TCP_NODELAY): trung binh   0.01 ms / luot
```

Chậm hơn **hàng nghìn lần** chỉ vì gửi header và body bằng hai lần `send`. Với game hay ứng dụng tương tác, đây là nguyên nhân kinh điển của độ trễ "bí ẩn" khoảng 40–200 ms.

**Cách xử lý (theo thứ tự ưu tiên):**

1. **Gom dữ liệu thành một lần `send`** (ghép header và body vào một bộ đệm) — tốt cho mọi trường hợp.
2. Với ứng dụng tương tác, gửi nhiều thông điệp nhỏ: bật **`TCP_NODELAY`**:

   ```c
   int on = 1;
   setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof on);
   ```

### 12.3 Các tùy chọn khác

| Tùy chọn | Mức (`level`) | Công dụng | Ghi chú |
|---|---|---|---|
| `SO_REUSEADDR` | `SOL_SOCKET` | Bind lại cổng có kết nối ở TIME_WAIT | Nghĩa khác trên Windows (mục 6.4) |
| `SO_KEEPALIVE` | `SOL_SOCKET` | TCP định kỳ thăm dò kết nối im lặng | Mặc định thường sau **2 giờ** mới thăm dò → game nên tự gửi **heartbeat** ở tầng ứng dụng |
| `SO_RCVBUF`, `SO_SNDBUF` | `SOL_SOCKET` | Kích thước bộ đệm nhận/gửi | Hệ điều hành có thể làm tròn hoặc giới hạn giá trị |
| `SO_LINGER` | `SOL_SOCKET` | Hành vi khi `close` | `l_onoff = 1, l_linger = 0` → đóng bằng **RST** ngay; hiếm khi cần |
| `SO_BROADCAST` | `SOL_SOCKET` | Cho phép gửi UDP broadcast | Dùng để tìm server trong mạng LAN |
| `TCP_NODELAY` | `IPPROTO_TCP` | Tắt Nagle | Mục 12.2 |
| `IPV6_V6ONLY` | `IPPROTO_IPV6` | Socket IPv6 chỉ nhận IPv6 hay nhận cả IPv4 | Mục 12.4 |

### 12.4 Server nhận cả IPv4 và IPv6 (dual-stack)

Hai cách:

1. **Hai socket lắng nghe** — một `AF_INET`, một `AF_INET6` (đặt `IPV6_V6ONLY = 1`) — rồi `poll` cả hai.
2. **Một socket IPv6** với `IPV6_V6ONLY = 0`: kết nối IPv4 sẽ xuất hiện dưới dạng địa chỉ "IPv4-mapped", ví dụ `::ffff:127.0.0.1`.

```c
/* Đoạn mã minh họa cho cách 2 */
socket_t lst = socket(AF_INET6, SOCK_STREAM, 0);
int off = 0;
setsockopt(lst, IPPROTO_IPV6, IPV6_V6ONLY, (const char *)&off, sizeof off);   /* đặt TRƯỚC bind */
struct sockaddr_in6 addr;
memset(&addr, 0, sizeof addr);
addr.sin6_family = AF_INET6;
addr.sin6_addr = in6addr_any;           /* "::" */
addr.sin6_port = htons(9000);
bind(lst, (struct sockaddr *)&addr, sizeof addr);
listen(lst, SOMAXCONN);
```

**Luôn đặt `IPV6_V6ONLY` một cách tường minh**, vì giá trị mặc định khác nhau: Linux thường mặc định **0** (theo thiết lập hệ thống `net.ipv6.bindv6only`), còn Windows mặc định **1**.

> ⚠️ Đoạn mã này **chưa được chạy thử** trong quá trình soạn tài liệu, vì môi trường kiểm thử không hỗ trợ IPv6. Hãy thử trên máy của bạn: kết nối tới cả `127.0.0.1` và `::1` rồi in địa chỉ nhận được từ `accept`.

---

# PHẦN VI — VẬN DỤNG

## Chương 13. Lớp C++ RAII cho socket

### 13.1 Vì sao cần bọc socket?

Trong C, mỗi đường thoát sớm (lỗi, ngoại lệ) đều phải nhớ `close()` — rất dễ **rò rỉ socket**. Với C++, gắn vòng đời socket vào một đối tượng (**RAII**): hàm hủy tự đóng socket. Lớp này **không sao chép được** (tránh hai đối tượng cùng đóng một socket) nhưng **di chuyển được** (chuyển quyền sở hữu, ví dụ sang một luồng khác).

```cpp
// file: socket.hpp — lớp RAII bọc socket (dùng net.h), C++17
#pragma once
#include "net.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace net {

// Mã lỗi hệ thống -> std::error_code (có sẵn thông báo lỗi phù hợp với từng hệ điều hành)
inline std::error_code last_error() {
#ifdef _WIN32
    return {net_last_error(), std::system_category()};
#else
    return {net_last_error(), std::generic_category()};
#endif
}

[[noreturn]] inline void throw_last_error(const char* what) {
    throw std::system_error(last_error(), what);
}

// RAII cho WSAStartup/WSACleanup: tạo một đối tượng ở đầu main()
class Init {
public:
    Init() {
        if (net_init() != 0) throw_last_error("net_init");
    }
    ~Init() { net_cleanup(); }
    Init(const Init&) = delete;
    Init& operator=(const Init&) = delete;
};

// Sở hữu độc quyền một socket: không sao chép được, di chuyển được, tự đóng khi bị hủy
class Socket {
public:
    Socket() = default;
    explicit Socket(socket_t fd) noexcept : fd_(fd) {}
    ~Socket() { reset(); }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other) noexcept : fd_(std::exchange(other.fd_, NET_INVALID)) {}
    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            reset();
            fd_ = std::exchange(other.fd_, NET_INVALID);
        }
        return *this;
    }

    void reset() noexcept {
        if (fd_ != NET_INVALID) {
            net_close(fd_);
            fd_ = NET_INVALID;
        }
    }
    socket_t get() const noexcept { return fd_; }
    explicit operator bool() const noexcept { return fd_ != NET_INVALID; }

    // Gửi đủ mọi byte, ném ngoại lệ nếu lỗi
    void send_all(std::string_view data) const {
        while (!data.empty()) {
            net_ssize_t n = ::send(fd_, data.data(), static_cast<int>(data.size()), 0);
            if (n < 0) throw_last_error("send");
            data.remove_prefix(static_cast<std::size_t>(n));
        }
    }

    // Nhận tối đa len byte. Trả về 0 nghĩa là bên kia đã đóng kết nối.
    std::size_t recv_some(char* buf, std::size_t len) const {
        net_ssize_t n = ::recv(fd_, buf, static_cast<int>(len), 0);
        if (n < 0) throw_last_error("recv");
        return static_cast<std::size_t>(n);
    }

    Socket accept() const {
        socket_t c = ::accept(fd_, nullptr, nullptr);
        if (c == NET_INVALID) throw_last_error("accept");
        return Socket(c);
    }

    // Đóng chiều gửi (bên kia sẽ nhận recv() == 0)
    void shutdown_send() const noexcept { ::shutdown(fd_, NET_SHUT_WR); }

private:
    socket_t fd_ = NET_INVALID;
};

// Kết nối TCP tới host:port, thử lần lượt mọi địa chỉ (IPv4 và IPv6)
inline Socket connect_tcp(const std::string& host, const std::string& port) {
    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* res = nullptr;
    if (int err = ::getaddrinfo(host.c_str(), port.c_str(), &hints, &res); err != 0)
        throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(err));

    std::error_code last;
    for (addrinfo* p = res; p != nullptr; p = p->ai_next) {
        Socket s(::socket(p->ai_family, p->ai_socktype, p->ai_protocol));
        if (!s) { last = last_error(); continue; }
        if (::connect(s.get(), p->ai_addr, static_cast<socklen_t>(p->ai_addrlen)) == 0) {
            ::freeaddrinfo(res);
            return s;                         // thành công: trả quyền sở hữu ra ngoài
        }
        last = last_error();
    }                                         // các Socket thất bại tự đóng khi ra khỏi vòng lặp
    ::freeaddrinfo(res);
    throw std::system_error(last, "connect " + host + ":" + port);
}

// Mở socket lắng nghe TCP (IPv4) trên port
inline Socket listen_tcp(const std::string& port, int backlog = SOMAXCONN) {
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    addrinfo* res = nullptr;
    if (int err = ::getaddrinfo(nullptr, port.c_str(), &hints, &res); err != 0)
        throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(err));

    Socket s(::socket(res->ai_family, res->ai_socktype, res->ai_protocol));
    if (!s) {
        ::freeaddrinfo(res);
        throw_last_error("socket");
    }
#ifndef _WIN32
    int yes = 1;
    ::setsockopt(s.get(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
#endif
    int rc = ::bind(s.get(), res->ai_addr, static_cast<socklen_t>(res->ai_addrlen));
    ::freeaddrinfo(res);
    if (rc != 0) throw_last_error("bind");
    if (::listen(s.get(), backlog) != 0) throw_last_error("listen");
    return s;
}

}  // namespace net
```

**Điểm đáng chú ý:**

- Lỗi hệ thống được chuyển thành `std::system_error` kèm `std::error_code`. Trên Windows dùng `std::system_category()` để thông báo lỗi khớp với mã Winsock; trên POSIX dùng `std::generic_category()` với `errno`.
- Trong `connect_tcp`, các socket kết nối thất bại **tự đóng** khi ra khỏi vòng lặp — không cần một lời gọi `close` nào.
- `Init` đảm bảo `WSACleanup` luôn được gọi, kể cả khi `main` thoát bằng ngoại lệ.

### 13.2 Echo server đa luồng

```cpp
// file: echo_threads.cpp — echo server phục vụ NHIỀU client cùng lúc: mỗi client một luồng
// Cách dùng: ./echo_threads [cong]
#include "socket.hpp"

#include <iostream>
#include <mutex>
#include <string>
#include <thread>

std::mutex log_mutex;
void log_line(const std::string& text) {
    std::lock_guard<std::mutex> lock(log_mutex);      // nhiều luồng cùng in -> cần khóa
    std::cout << text << std::endl;
}

// Nhận socket THEO GIÁ TRỊ (move vào): luồng này sở hữu socket
void serve(net::Socket client, int id) {
    try {
        char buf[4096];
        std::size_t total = 0;
        while (std::size_t n = client.recv_some(buf, sizeof buf)) {
            client.send_all(std::string_view(buf, n));
            total += n;
        }
        log_line("client " + std::to_string(id) + " xong, echo " + std::to_string(total) + " byte");
    } catch (const std::system_error& e) {
        log_line("client " + std::to_string(id) + " loi: " + e.what());
    }
}   // client bị hủy ở đây -> socket tự đóng (RAII)

int main(int argc, char* argv[]) {
    try {
        net::Init init;
        net::Socket listener = net::listen_tcp(argc > 1 ? argv[1] : "9000");
        log_line("Echo server (da luong) dang chay");
        for (int id = 1;; ++id) {
            net::Socket client = listener.accept();
            log_line("client " + std::to_string(id) + " ket noi");
            std::thread(serve, std::move(client), id).detach();   // chuyển quyền sở hữu cho luồng mới
        }
    } catch (const std::exception& e) {
        std::cerr << "Loi: " << e.what() << '\n';
        return 1;
    }
}
```

Mỗi `std::thread` nhận **quyền sở hữu** socket qua `std::move`. Khi hàm `serve` kết thúc, socket tự đóng. (Kết quả so sánh với server lần lượt ở Chương 9.)

> ⚠️ `detach()` giữ ví dụ ngắn gọn, nhưng khiến luồng không được quản lý. Server thật nên giới hạn số luồng (hoặc dùng thread pool) và có cơ chế dừng các luồng khi tắt server.

### 13.3 Chat client hai luồng

Client cần **đồng thời** chờ bàn phím và chờ server. Trên Linux có thể `poll` cả `stdin` lẫn socket, nhưng `WSAPoll` của Windows **không** theo dõi được bàn phím. Cách đơn giản và chạy được trên cả hai nền tảng: **một luồng nhận, luồng chính đọc bàn phím.**

```cpp
// file: chat_client.cpp — client cho chat_server: một luồng NHẬN, luồng chính đọc BÀN PHÍM
// Cách dùng: ./chat_client <host> <cong>
#include "socket.hpp"

#include <iostream>
#include <string>
#include <thread>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Cach dung: " << argv[0] << " <host> <cong>\n";
        return 1;
    }
    try {
        net::Init init;
        net::Socket sock = net::connect_tcp(argv[1], argv[2]);

        // Luồng nhận: in mọi thứ server gửi tới cho đến khi server đóng kết nối
        std::thread receiver([&sock] {
            char buf[1024];
            try {
                while (std::size_t n = sock.recv_some(buf, sizeof buf))
                    std::cout.write(buf, static_cast<std::streamsize>(n)).flush();
                std::cout << "(server da dong ket noi)\n";
            } catch (const std::system_error& e) {
                std::cout << "(loi khi nhan: " << e.what() << ")\n";
            }
        });

        // Luồng chính: mỗi dòng gõ vào được gửi lên server
        std::string line;
        while (std::getline(std::cin, line)) {
            sock.send_all(line + "\n");
            if (line == "/quit") break;
        }
        sock.shutdown_send();     // báo "tôi gửi xong" -> server đóng -> luồng nhận nhận được 0 và kết thúc
        receiver.join();
    } catch (const std::exception& e) {
        std::cerr << "Loi: " << e.what() << '\n';
        return 1;
    }
}
```

**Một luồng gọi `send` và một luồng khác gọi `recv` trên cùng socket là an toàn**, trên cả POSIX lẫn Winsock. Nhưng **nhiều luồng cùng `send`** trên một socket TCP thì dữ liệu có thể xen kẽ lẫn nhau → cần mutex, hoặc chỉ cho một luồng gửi.

**Ví dụ phiên chat thật** — chat server ở Chương 10, hai người dùng chạy `chat_client` (ở đây nội dung bàn phím được đưa vào bằng script):

*Màn hình của Chi* (gõ `/nick Chi`, `Chao Binh!`, `/quit`):

```text
Chao khach1! Lenh: /nick <ten>, /quit
* khach1 doi ten thanh Chi
* khach2 vao phong (2 nguoi)
* khach2 doi ten thanh Binh
[Binh] Chao Chi, khoe khong?
(server da dong ket noi)
```

*Màn hình của Bình* (gõ `/nick Binh`, `Chao Chi, khoe khong?`, rồi thoát chương trình bằng Ctrl+D / hết dữ liệu vào):

```text
Chao khach2! Lenh: /nick <ten>, /quit
* khach2 doi ten thanh Binh
[Chi] Chao Binh!
* Chi roi phong (tu thoat)
(server da dong ket noi)
```

*Log của server:*

```text
Chat server dang chay o cong 9000
* khach1 vao phong (1 nguoi)
* khach1 doi ten thanh Chi
* khach2 vao phong (2 nguoi)
* khach2 doi ten thanh Binh
[Chi] Chao Binh!
[Binh] Chao Chi, khoe khong?
* Chi roi phong (tu thoat)
* Binh roi phong (mat ket noi)
```

Bản Windows của `chat_client` cũng đã được thử kết nối vào chat server chạy trên Linux. Khi server không chạy, client in thông báo lỗi từ ngoại lệ:

```text
Loi: connect 127.0.0.1:9599: Connection refused
```

---

## Chương 14. Web server tối giản

HTTP chỉ là **văn bản đi qua TCP** — ta tự viết được một web server nhỏ:

```c
/* file: http_server.c — web server tối giản: một yêu cầu mỗi kết nối
   Cách dùng: ./http_server [cong]   rồi mở http://localhost:8080/ trên trình duyệt */
#include "net.h"
#include <stdlib.h>

#define MAX_REQUEST 8192

/* Đọc đến khi gặp dòng trống "\r\n\r\n" (hết phần header), có giới hạn kích thước */
static int read_request(socket_t s, char *buf, size_t cap) {
    size_t len = 0;
    while (len < cap - 1) {
        net_ssize_t n = recv(s, buf + len, (int)(cap - 1 - len), 0);
        if (n <= 0) return -1;
        len += (size_t)n;
        buf[len] = '\0';
        if (strstr(buf, "\r\n\r\n") != NULL) return 0;
    }
    return -1;                                  /* header quá dài */
}

static void send_response(socket_t s, const char *status, const char *body) {
    char head[256];
    int n = snprintf(head, sizeof head,
                     "HTTP/1.1 %s\r\n"
                     "Content-Type: text/html; charset=utf-8\r\n"
                     "Content-Length: %zu\r\n"
                     "Connection: close\r\n"
                     "\r\n",
                     status, strlen(body));
    send(s, head, n, 0);
    send(s, body, (int)strlen(body), 0);
}

int main(int argc, char *argv[]) {
    const char *port = argc > 1 ? argv[1] : "8080";
    if (net_init() != 0) return 1;

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, port, &hints, &res) != 0) return 1;
    socket_t lst = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
#ifndef _WIN32
    int yes = 1;
    setsockopt(lst, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof yes);
#endif
    if (lst == NET_INVALID || bind(lst, res->ai_addr, (socklen_t)res->ai_addrlen) != 0 ||
        listen(lst, SOMAXCONN) != 0) {
        net_perror("listen");
        return 1;
    }
    freeaddrinfo(res);
    printf("HTTP server: http://localhost:%s/\n", port);
    fflush(stdout);

    for (;;) {
        socket_t c = accept(lst, NULL, NULL);
        if (c == NET_INVALID) continue;
        net_set_recv_timeout(c, 5000);          /* client chậm/không gửi gì: bỏ sau 5 giây */

        char req[MAX_REQUEST];
        char method[8] = "", path[256] = "";    /* khởi tạo: không bao giờ in biến chưa có giá trị */
        if (read_request(c, req, sizeof req) != 0 ||
            sscanf(req, "%7s %255s", method, path) != 2) {   /* giới hạn độ dài khi đọc chuỗi */
            send_response(c, "400 Bad Request", "<h1>400 Bad Request</h1>");
            printf("(yeu cau khong hop le)\n");
            fflush(stdout);
            net_close(c);
            continue;
        } else if (strcmp(method, "GET") != 0) {
            send_response(c, "405 Method Not Allowed", "<h1>405 Method Not Allowed</h1>");
        } else if (strcmp(path, "/") == 0) {
            send_response(c, "200 OK", "<h1>Xin chao tu socket!</h1>");
        } else {
            send_response(c, "404 Not Found", "<h1>404 Not Found</h1>");
        }
        printf("%s %s\n", method, path);
        fflush(stdout);
        net_close(c);
    }
}
```

**Thử bằng `curl`** (hoặc mở `http://localhost:8080/` trên trình duyệt):

```text
$ curl -i http://127.0.0.1:8080/
HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8
Content-Length: 28
Connection: close

<h1>Xin chao tu socket!</h1>

$ curl -s -o /dev/null -w "%{http_code}\n" http://127.0.0.1:8080/abc
404

$ curl -s -o /dev/null -w "%{http_code}\n" -X POST http://127.0.0.1:8080/
405

$ printf 'hello\r\n\r\n' | nc 127.0.0.1 8080
HTTP/1.1 400 Bad Request
...
```

**Log của server:**

```text
HTTP server: http://localhost:8080/
GET /
GET /abc
POST /
(yeu cau khong hop le)
```

Bản Windows cho kết quả tương tự khi thử bằng `curl` từ Linux.

> 🐛 **Một lỗi thật được phát hiện khi kiểm thử:** phiên bản đầu của server khai báo `char method[8], path[256];` mà không khởi tạo. Khi `sscanf` thất bại, dòng log in ra nội dung **rác còn sót lại từ yêu cầu trước** (`hello /`). Đây là hành vi không xác định mà cả AddressSanitizer cũng không bắt được, vì vùng nhớ stack được tái sử dụng hợp lệ. Bài học: **khởi tạo mọi bộ đệm**, và **chỉ dùng kết quả của `sscanf` khi nó trả về đủ số trường**.

**Những gì server này còn thiếu** (lý do nên dùng thư viện cho web server thật): HTTPS, keep-alive, phân tích header đầy đủ, xử lý body (POST), mã hóa URL, chống duyệt thư mục (`../`), xử lý đồng thời.

---

## Chương 15. Bảo mật và độ bền

**Nguyên tắc số 1: mọi byte đến từ mạng đều có thể do kẻ xấu tạo ra.**

### 15.1 Checklist

- [ ] **Giới hạn độ dài** của mọi thứ đọc từ mạng: dòng, thông điệp, header, tên người dùng.
- [ ] **Không tin trường độ dài** do bên kia gửi (mục 5.4).
- [ ] Dùng `snprintf`, giới hạn độ rộng trong `sscanf` (`%255s`); **không** dùng `sprintf`, `strcpy`, `gets`.
- [ ] **Khởi tạo** mọi bộ đệm và biến.
- [ ] Đặt **timeout** cho kết nối im lặng (chống tấn công kiểu *Slowloris* — giữ kết nối mở và gửi cực chậm).
- [ ] **Giới hạn số kết nối** tổng và theo từng IP.
- [ ] Xử lý đúng `recv` trả về 0 và giá trị âm; không bao giờ bỏ qua giá trị trả về.
- [ ] Trong game: **không tin client** — kiểm tra mọi hành động phía server (tốc độ di chuyển, tầm bắn, cooldown).
- [ ] Khi phát triển, bind server vào `127.0.0.1` thay vì `0.0.0.0` nếu không cần máy khác truy cập.
- [ ] Chạy thử với **AddressSanitizer** và công cụ **fuzzing** (như libFuzzer, AFL++) cho phần phân tích dữ liệu.

### 15.2 Mã hóa (TLS)

Dữ liệu gửi qua socket thô là **văn bản trơn** — ai ở giữa đường cũng đọc và sửa được. Với dữ liệu nhạy cảm (mật khẩu, token), hãy dùng **TLS**:

- Thư viện: **OpenSSL**, **mbedTLS**, **wolfSSL**, hoặc **Schannel** (có sẵn trên Windows).
- Luồng làm việc: kết nối TCP như bình thường → "bọc" socket bằng phiên TLS → bắt tay TLS (kiểm tra chứng chỉ!) → dùng hàm đọc/ghi của thư viện thay cho `send`/`recv`.
- **Không bao giờ tự thiết kế thuật toán mã hóa.**

---

## Chương 16. Gỡ lỗi và kiểm thử

### 16.1 Bộ công cụ

| Mục đích | Công cụ |
|---|---|
| Đóng vai client/server thủ công | `nc` (netcat): `nc 127.0.0.1 9000` (TCP), `nc -u 127.0.0.1 9001` (UDP), `nc -l 9000` (lắng nghe) |
| Thử HTTP | `curl -v` |
| Xem socket đang mở | Linux: `ss -tanp`, `ss -uanp`; Windows: `netstat -ano` |
| Xem gói tin thật | **Wireshark**; `tcpdump -i lo port 9000` |
| Xem lời gọi hệ thống | Linux: `strace -e trace=network ./server` |
| Giả lập mạng xấu | Linux: `sudo tc qdisc add dev lo root netem delay 100ms loss 5%` (nhớ xóa sau khi thử: `sudo tc qdisc del dev lo root`); Windows: công cụ như Clumsy |

**Bộ lọc Wireshark hữu ích:** `tcp.port == 9000`, `udp.port == 9001`, `tcp.flags.reset == 1` (tìm gói RST), `tcp.analysis.retransmission` (gói bị gửi lại).

### 16.2 Quy trình khi "không kết nối được"

1. Server có đang chạy và **lắng nghe đúng cổng, đúng giao thức** không? (`ss -tlnp` / `netstat -ano`)
2. Server bind vào `127.0.0.1` (chỉ máy mình) hay `0.0.0.0` (mọi card mạng)?
3. Client kết nối **IPv4 hay IPv6**? Server có nghe loại đó không? (Nhớ lại vấn đề `localhost` ở Chương 3.)
4. **Tường lửa** trên máy server có chặn cổng không?
5. Kết nối qua Internet: đã **port forwarding** trên router chưa? Có bị **CGNAT** không?
6. Mã lỗi là gì? *Refused* → không ai nghe cổng; *timed out* → bị chặn, sai địa chỉ, hoặc máy không trả lời.

### 16.3 Kiểm thử

- **Kiểm thử tự động** các kịch bản nhiều client bằng script (như `chat_test.py`) — nhanh hơn và lặp lại được, so với gõ tay trên nhiều cửa sổ.
- Luôn thử các **trường hợp biên**: thông điệp rỗng, thông điệp bị cắt đôi, nhiều thông điệp dính nhau, thông điệp quá dài, client ngắt kết nối giữa chừng, client không bao giờ đọc.
- Thử trong điều kiện **mạng xấu** (độ trễ, mất gói) — loopback nhanh và hoàn hảo che giấu rất nhiều lỗi.
- Tách **logic giao thức** (phân tích và tạo thông điệp) khỏi **mã socket**, để kiểm thử logic bằng unit test mà không cần mạng.

---

## Bài tập Phần V và VI

**Bài V.1.** Dùng `socket.hpp`, viết chương trình `http_status <host> <cong> <duong_dan>` gửi yêu cầu HTTP GET và in **dòng trạng thái** của phản hồi. Thử với web server ở Chương 14.

**Bài V.2.** Một game gửi mỗi hành động của người chơi bằng 3 lần `send` nhỏ (loại, tọa độ x, tọa độ y) qua TCP. Người chơi phàn nàn về độ trễ khoảng 40–200 ms dù ping thấp. Giải thích và đưa ra hai cách sửa.

**Bài V.3.** Vì sao `echo_threads.cpp` phải truyền socket vào luồng bằng `std::move`? Nếu lớp `Socket` cho phép sao chép thì điều gì có thể xảy ra?

**Bài V.4.** Server nhận được rất nhiều kết nối mở ra nhưng không gửi dữ liệu nào. Sau một thời gian, người dùng thật không kết nối được nữa. Đây là kiểu tấn công gì, và server cần những biện pháp gì?

<details>
<summary><b>Lời giải Phần V và VI</b> (bấm để mở)</summary>

**V.1.**

```cpp
// file: http_status.cpp — in dòng trạng thái của một URL http:// (dùng socket.hpp)
// Cách dùng: ./http_status <host> <cong> <duong dan>
#include "socket.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Cach dung: " << argv[0] << " <host> <cong> <duong dan>\n";
        return 1;
    }
    try {
        net::Init init;
        net::Socket s = net::connect_tcp(argv[1], argv[2]);
        s.send_all(std::string("GET ") + argv[3] + " HTTP/1.1\r\nHost: " + argv[1] +
                   "\r\nConnection: close\r\n\r\n");
        std::string response;
        char buf[4096];
        while (std::size_t n = s.recv_some(buf, sizeof buf)) response.append(buf, n);
        std::cout << response.substr(0, response.find("\r\n")) << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Loi: " << e.what() << '\n';
        return 1;
    }
}
```

*Kết quả* (khi `http_server` chạy ở cổng 8080):

```text
$ ./http_status 127.0.0.1 8080 /
HTTP/1.1 200 OK
$ ./http_status localhost 8080 /khong-co
HTTP/1.1 404 Not Found
```

**V.2.** Nagle giữ lại các gói nhỏ phía sau cho đến khi gói đầu được xác nhận, còn bên nhận lại **trì hoãn ACK** → mỗi hành động bị trễ theo bộ đếm ACK trễ (mục 12.2). Sửa: (1) ghép 3 phần thành **một** bộ đệm và gửi một lần; (2) bật `TCP_NODELAY`. (Với dữ liệu thời gian thực như vị trí, cân nhắc chuyển sang UDP.)

**V.3.** `Socket` là kiểu **chỉ di chuyển**: `std::move` chuyển quyền sở hữu sang đối tượng trong luồng mới, còn biến `client` ở vòng lặp trở thành rỗng. Nếu sao chép được, hai đối tượng cùng giữ một socket: đối tượng ở vòng lặp bị hủy ngay khi sang vòng tiếp theo → **đóng socket** trong khi luồng kia vẫn đang dùng. Tệ hơn, số hiệu socket đó có thể được hệ điều hành cấp lại cho kết nối mới → luồng cũ đọc/ghi nhầm vào kết nối của người khác, rồi socket bị đóng hai lần.

**V.4.** Tấn công làm cạn tài nguyên kết nối (dạng *Slowloris* với HTTP): chiếm hết số kết nối/luồng/bộ đệm mà server cho phép. Biện pháp: timeout cho kết nối im lặng (cả lúc chờ yêu cầu đầu tiên), giới hạn số kết nối tổng và theo từng IP, dùng mô hình vòng lặp sự kiện thay vì mỗi kết nối một luồng, và đặt server sau một reverse proxy hoặc dịch vụ chống DDoS khi triển khai thật.

</details>

---

# PHỤ LỤC

## Phụ lục A. Bảng tra API

| Hàm | Công dụng | Trả về khi lỗi | Ghi chú |
|---|---|---|---|
| `socket(family, type, proto)` | Tạo socket | `-1` / `INVALID_SOCKET` | `SOCK_STREAM` = TCP, `SOCK_DGRAM` = UDP |
| `bind(s, addr, len)` | Gắn địa chỉ + cổng | `-1` | Server; cổng 0 = OS tự chọn |
| `listen(s, backlog)` | Lắng nghe | `-1` | Chỉ TCP |
| `accept(s, addr, len)` | Nhận kết nối | `-1` / `INVALID_SOCKET` | Trả về socket **mới** |
| `connect(s, addr, len)` | Kết nối (TCP) / ghi nhớ đích (UDP) | `-1` | |
| `send(s, buf, len, flags)` | Gửi | `-1` | Có thể gửi **ít hơn** `len` |
| `recv(s, buf, len, flags)` | Nhận | `-1` | `0` = bên kia đã đóng (TCP) |
| `sendto` / `recvfrom` | Gửi / nhận kèm địa chỉ | `-1` | Chủ yếu cho UDP |
| `shutdown(s, how)` | Đóng một chiều | `-1` | `SHUT_WR` / `SD_SEND` |
| `close(s)` / `closesocket(s)` | Giải phóng socket | `-1` | |
| `setsockopt` / `getsockopt` | Đặt / đọc tùy chọn | `-1` | Chương 12 |
| `getsockname(s, addr, len)` | Địa chỉ **của mình** | `-1` | Biết cổng OS đã chọn |
| `getpeername(s, addr, len)` | Địa chỉ **bên kia** | `-1` | |
| `getaddrinfo` / `freeaddrinfo` | Phân giải tên → địa chỉ | mã khác 0 | Dùng `gai_strerror`, không dùng `errno` |
| `inet_pton` / `inet_ntop` | Chuỗi ↔ nhị phân | `0`/`-1` hoặc `NULL` | |
| `htons` / `htonl` / `ntohs` / `ntohl` | Đổi thứ tự byte | — | |
| `poll` / `WSAPoll` | Chờ nhiều socket | `-1` | `0` = hết timeout |
| `fcntl` / `ioctlsocket` | Bật chế độ không chặn | `-1` | Hàm `net_set_nonblocking` |
| `WSAStartup` / `WSACleanup` | Khởi tạo / dọn Winsock | mã khác 0 | Chỉ Windows |

## Phụ lục B. Checklist cho một chương trình socket đúng chuẩn

1. Gọi `WSAStartup` trên Windows; bỏ qua `SIGPIPE` trên Linux/macOS.
2. Dùng `getaddrinfo`, thử **mọi** địa chỉ trả về, nhớ `freeaddrinfo`.
3. Kiểm tra giá trị trả về của **mọi** lời gọi.
4. Đổi thứ tự byte cho cổng và mọi số nguyên trong giao thức.
5. TCP: dùng `send_all`, và **đóng khung thông điệp** (phân cách hoặc tiền tố độ dài).
6. Phân biệt `recv() == 0` (đóng bình thường) với `recv() < 0` (lỗi).
7. Giới hạn kích thước và đặt timeout cho mọi dữ liệu đến.
8. Server: đặt `SO_REUSEADDR` trên POSIX; cân nhắc `SO_EXCLUSIVEADDRUSE` trên Windows.
9. Nhiều client: chọn mô hình phù hợp (luồng, `poll`, thư viện).
10. Tương tác độ trễ thấp: gom `send` hoặc bật `TCP_NODELAY`.
11. Đóng socket trên **mọi** đường thoát (C++: dùng RAII).
12. Kiểm thử với nhiều client, dữ liệu bị cắt/dính, ngắt kết nối đột ngột, mạng xấu.

## Phụ lục C. Lộ trình gợi ý (khoảng 6 tuần)

| Tuần | Nội dung | Mục tiêu tối thiểu |
|---|---|---|
| 1 | Chương 1–4 | Tự viết lại echo server/client **không nhìn tài liệu**, chạy được trên Linux và Windows |
| 2 | Chương 5–6 | Cài đặt đóng khung bằng tiền tố độ dài; giải thích được FIN và RST |
| 3 | Chương 7–8 | UDP ping có số thứ tự và timeout; vòng lặp không chặn |
| 4 | Chương 9–11 | Chat server với `poll`, kèm script kiểm thử |
| 5 | Chương 12–14 | Lớp RAII C++; client đa luồng; web server nhỏ |
| 6 | Chương 15–16 + dự án riêng | Một trò chơi nhiều người chơi đơn giản (ví dụ cờ caro qua mạng): nâng cấp chat server thành socket không chặn có bộ đệm gửi |

## Phụ lục D. Tài liệu nên dùng song song

- **Beej's Guide to Network Programming** (miễn phí) — hướng dẫn socket bằng C dễ đọc nhất, có ghi chú cho Windows.
- **"UNIX Network Programming, Vol. 1"** (W. Richard Stevens) — "kinh thánh" của lập trình socket.
- **"The Linux Programming Interface"** (Michael Kerrisk) — các chương về socket rất chi tiết.
- **"TCP/IP Illustrated, Vol. 1"** (Stevens) — hiểu TCP/UDP qua từng gói tin.
- **Tài liệu Winsock của Microsoft** (Microsoft Learn) — mô tả chính xác từng hàm và mã lỗi trên Windows.
- **Trang `man`** trên Linux: `man 7 socket`, `man 7 tcp`, `man 7 udp`, `man 7 ip`, `man 2 poll`, `man 7 epoll`.
- **Tài liệu Asio** — khi chuyển sang lập trình mạng bất đồng bộ bằng C++.
- **Cho lập trình mạng game:** blog "Gaffer On Games" (Glenn Fiedler) — loạt bài về UDP, độ tin cậy, đồng bộ trạng thái; loạt bài "Fast-Paced Multiplayer" (Gabriel Gambetta).

---

*Chúc bạn học tốt! Cách học socket nhanh nhất: **chạy thử mọi chương trình, rồi cố tình làm hỏng chúng** — cắt đôi thông điệp, rút dây mạng, tắt server giữa chừng — và quan sát chuyện gì xảy ra.*
