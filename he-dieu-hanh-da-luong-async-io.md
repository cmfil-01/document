# HỆ ĐIỀU HÀNH, ĐA LUỒNG VÀ ASYNC I/O — NỀN TẢNG VỮNG CHẮC

*Tài liệu tự học: hiểu hệ điều hành làm gì bên dưới chương trình của bạn, viết code đa luồng đúng và an toàn, và xử lý hàng nghìn kết nối bằng I/O bất đồng bộ*

---

## 0. Cách dùng tài liệu này

### 0.1 Cấu trúc

| Phần | Chương | Nội dung |
|---|---|---|
| **I. Hệ điều hành** | 1–7 | Lời gọi hệ thống, **tiến trình** (`fork`/`exec`/`wait`), luồng, **lập lịch CPU**, quản lý bộ nhớ và **thay trang**, hệ thống file, giao tiếp liên tiến trình và tín hiệu |
| **II. Đa luồng** | 8–15 | `std::thread`, **race condition**, **deadlock**, condition variable, semaphore, **thread pool**, future/promise, lock-free, hiệu năng |
| **III. Async I/O** | 16–20 | Các mô hình I/O, **epoll**, event loop, async/await với **asyncio**, **coroutine C++20 với Asio**, chọn mô hình nào |
| **Phụ lục** | A–E | Bảng API POSIX / Windows / C++, lỗi hay gặp, công cụ, lộ trình, tài liệu |

Mỗi chương đi theo mạch: **ý tưởng → cơ chế → chương trình chạy được → lỗi hay gặp**. Cuối mỗi phần có **bài tập kèm lời giải**.

**Kiến thức cần có:** C/C++ cơ bản (con trỏ, lớp, lambda), khái niệm bộ nhớ ảo và socket ở mức cơ bản.

### 0.2 Môi trường

| Phần | Nền tảng | Biên dịch |
|---|---|---|
| **I** (C, API POSIX) | Linux, macOS, hoặc **WSL** trên Windows | `gcc -Wall -Wextra file.c -o file` |
| **II** (C++ chuẩn) | **Mọi hệ điều hành** | `g++ -std=c++20 -Wall -Wextra -pthread file.cpp -o file` (Visual Studio: bật C++20) |
| **III** — epoll | Chỉ **Linux** (hoặc WSL) | như Phần I |
| **III** — asyncio | Mọi hệ điều hành, Python 3.9+ | `python3 file.py` |
| **III** — Asio | Mọi hệ điều hành | `g++ -std=c++20 -I<asio>/include file.cpp -o file -pthread` |

> ✅ **Về độ chính xác:**
> - Mọi **chương trình hoàn chỉnh** trong tài liệu đã được biên dịch với `-Wall -Wextra -pedantic` và chạy thật trên Linux (kernel 6.18, GCC 13, Python 3.12), kèm AddressSanitizer/UBSan.
> - Mọi chương trình đa luồng còn được chạy dưới **ThreadSanitizer**. Không có cảnh báo nào, trừ các ví dụ **cố ý** chứa lỗi.
> - Các chương trình C++ chuẩn ở Phần II cũng được biên dịch cho **Windows** (MinGW-w64) và chạy thử qua Wine, cho kết quả giống hệt.
> - Kết quả mô phỏng lập lịch và thay trang được sinh bởi chính code Python trong tài liệu, và khớp với các ví dụ kinh điển trong giáo trình.
> - **Lưu ý quan trọng:** máy dùng để kiểm thử chỉ có **1 lõi CPU**. Những chỗ bị ảnh hưởng (ví dụ đo tăng tốc khi chạy song song) đều được ghi chú. Các số đo thời gian được ghi là "Ví dụ kết quả".

---

# PHẦN I — HỆ ĐIỀU HÀNH

## Chương 1. Hệ điều hành là gì?

### 1.1 Vai trò

**Hệ điều hành (HĐH)** là phần mềm đứng giữa chương trình và phần cứng, với ba nhiệm vụ chính:

1. **Trừu tượng hóa phần cứng:** chương trình chỉ cần "đọc file", "gửi dữ liệu mạng" — không cần biết ổ đĩa hay card mạng hoạt động ra sao.
2. **Quản lý và chia sẻ tài nguyên:** CPU, bộ nhớ, thiết bị được chia cho nhiều chương trình chạy "cùng lúc".
3. **Bảo vệ và cô lập:** chương trình này không được đọc bộ nhớ hay phá hoại chương trình khác.

### 1.2 Chế độ người dùng và chế độ nhân

CPU hiện đại có ít nhất hai **mức quyền**:

| | Chế độ người dùng (user mode) | Chế độ nhân (kernel mode) |
|---|---|---|
| Ai chạy | Chương trình của bạn | **Nhân (kernel)** của HĐH |
| Quyền | Bị giới hạn: không truy cập trực tiếp phần cứng, không đụng vào bộ nhớ của tiến trình khác | Toàn quyền |
| Lỗi nghiêm trọng | Chỉ chương trình đó bị dừng (ví dụ *segmentation fault*) | Cả máy có thể sập (màn hình xanh, *kernel panic*) |

Chương trình muốn làm việc cần quyền cao (đọc file, mở socket, tạo tiến trình) phải **nhờ nhân** thông qua **lời gọi hệ thống (system call)**:

```text
  Chương trình (user mode)                          Nhân (kernel mode)
  ┌──────────────────────┐
  │ printf("...")        │
  │   └─► write(1, ...)  │ ── lệnh đặc biệt (syscall) ──►  kiểm tra quyền,
  │                      │    CPU chuyển sang kernel mode   ghi ra thiết bị
  │   ◄── kết quả ───────│ ◄──────── quay về user mode ─────
  └──────────────────────┘
```

Mỗi lần chuyển qua lại như vậy **tốn thời gian** (lưu và khôi phục trạng thái CPU, kiểm tra quyền) — nên giảm số lời gọi hệ thống là một kỹ thuật tối ưu quan trọng.

### 1.3 Nhìn thấy lời gọi hệ thống

```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    /* Gọi hệ thống trực tiếp: ghi 20 byte vào file descriptor 1 (stdout) */
    const char *msg = "Xin chao tu write()\n";
    write(1, msg, strlen(msg));

    /* Hàm thư viện: printf định dạng chuỗi rồi (sớm hay muộn) cũng gọi write() */
    printf("PID cua toi la so duong: %s\n", getpid() > 0 ? "dung" : "sai");
    return 0;
}
```

**Kết quả:**

```text
Xin chao tu write()
PID cua toi la so duong: dung
```

Công cụ `strace` (Linux) cho thấy các lời gọi hệ thống mà chương trình thực hiện:

*Ví dụ kết quả* (đã lọc, chỉ giữ `write` và `getpid`; số PID thay đổi mỗi lần chạy):

```text
$ strace -e trace=write,getpid ./syscall_demo
write(1, "Xin chao tu write()\n", 20)   = 20
getpid()                                = 247
write(1, "PID cua toi la so duong: dung\n", 30) = 30
```

`printf` cuối cùng cũng chỉ là một lời gọi `write`.

### 1.4 Bộ đệm của thư viện C giảm số lời gọi hệ thống

Chương trình dưới đây ghi 1000 dòng vào file theo hai cách:

```c
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Cách dùng: ./buffering_demo stdio   hoặc   ./buffering_demo write */
int main(int argc, char *argv[]) {
    int use_stdio = argc > 1 && strcmp(argv[1], "stdio") == 0;
    if (use_stdio) {
        /* 1000 lần fprintf -> thư viện C gom vào bộ đệm, thỉnh thoảng mới gọi write() */
        FILE *f = fopen("out.txt", "w");
        for (int i = 0; i < 1000; i++) fprintf(f, "dong %d\n", i);
        fclose(f);
    } else {
        /* 1000 lần write() -> 1000 lần chuyển vào nhân */
        int fd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        char line[32];
        for (int i = 0; i < 1000; i++) {
            int n = snprintf(line, sizeof line, "dong %d\n", i);
            write(fd, line, (size_t)n);
        }
        close(fd);
    }
    unlink("out.txt");
    return 0;
}
```

Đếm số lời gọi `write` bằng `strace -c`:

*Ví dụ kết quả:*

```text
$ strace -c -e trace=write ./buffering_demo stdio
% time     seconds  usecs/call     calls    errors syscall
  0.00    0.000000           0         3           write

$ strace -c -e trace=write ./buffering_demo write
% time     seconds  usecs/call     calls    errors syscall
100.00    0.000787           0      1000           write
```

`fprintf` gom dữ liệu vào **bộ đệm trong bộ nhớ của chương trình** và chỉ gọi `write` khi bộ đệm đầy (hoặc khi `fclose`/`fflush`): **3 lần** thay vì **1000 lần**.

**Hệ quả cần nhớ:**

- `printf` ra terminal thường được đệm **theo dòng**; ra file hoặc qua pipe thì đệm **theo khối** (vài KB).
- Dữ liệu còn trong bộ đệm **chưa được ghi** — nếu chương trình bị "giết" đột ngột, nó sẽ mất. Dùng `fflush` khi cần chắc chắn.
- Bộ đệm này còn gây một lỗi kinh điển khi dùng `fork` (mục 2.4).

### 1.5 Kiến trúc nhân

| Kiến trúc | Ý tưởng | Ví dụ |
|---|---|---|
| **Nguyên khối (monolithic)** | Hầu hết dịch vụ (driver, hệ thống file, mạng) chạy **trong nhân** | Linux |
| **Vi nhân (microkernel)** | Nhân chỉ giữ phần tối thiểu (lập lịch, IPC, bộ nhớ); còn lại chạy như tiến trình người dùng | QNX, seL4, MINIX 3 |
| **Lai (hybrid)** | Kết hợp hai kiểu trên | Windows NT, macOS (XNU) |

Nguyên khối thường nhanh hơn (ít chuyển ngữ cảnh), vi nhân thường ổn định và an toàn hơn (một driver lỗi không làm sập cả nhân).

---

## Chương 2. Tiến trình

### 2.1 Tiến trình là gì?

**Chương trình** là file trên đĩa (thụ động). **Tiến trình (process)** là một chương trình **đang chạy**, gồm:

- **Không gian địa chỉ riêng:** mã lệnh, dữ liệu, heap, stack.
- **Trạng thái CPU:** giá trị các thanh ghi, con trỏ lệnh (PC).
- **Tài nguyên HĐH:** các file đang mở, socket, thư mục hiện hành, quyền hạn...

HĐH lưu tất cả thông tin này trong **khối điều khiển tiến trình (PCB)**. Mỗi tiến trình có một **mã số (PID)**.

### 2.2 Các trạng thái

```text
                     được chọn chạy
  ┌──────┐  sẵn sàng  ┌─────────┐ ─────────────► ┌─────────┐   kết thúc   ┌────────────┐
  │ Mới  │ ─────────► │ Sẵn sàng│                 │  Đang   │ ───────────► │ Kết thúc   │
  │(New) │            │ (Ready) │ ◄───────────── │  chạy   │              │(Terminated)│
  └──────┘            └─────────┘  hết lượt CPU   │(Running)│              └────────────┘
                           ▲       (bị ngắt)      └─────────┘
                           │                           │
                           │ I/O xong                  │ chờ I/O, chờ khóa,
                           │                           │ ngủ (sleep)
                           │      ┌───────────┐        │
                           └───── │   Chờ     │ ◄──────┘
                                  │ (Waiting/ │
                                  │  Blocked) │
                                  └───────────┘
```

Trên một lõi CPU, tại mỗi thời điểm chỉ có **một** tiến trình ở trạng thái *Đang chạy*. Cảm giác "nhiều chương trình chạy cùng lúc" đến từ việc HĐH **chuyển đổi rất nhanh** giữa chúng (Chương 4).

### 2.3 Tạo tiến trình trên Linux: `fork`, `exec`, `wait`

| Lời gọi | Tác dụng |
|---|---|
| `fork()` | **Nhân bản** tiến trình hiện tại. Trả về **0** trong tiến trình con, **PID của con** trong tiến trình cha, **−1** nếu lỗi |
| `exec...()` | **Thay** chương trình đang chạy trong tiến trình này bằng một chương trình khác (PID giữ nguyên). Không quay về nếu thành công |
| `waitpid()` | Cha **chờ** con kết thúc và lấy mã thoát |
| `_exit()` / `exit()` | Kết thúc tiến trình |

Shell chạy một lệnh chính là: `fork` → trong con: `exec` lệnh đó → trong cha: `wait`.

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int x = 10;
    printf("Truoc fork: x = %d\n", x);
    fflush(stdout);                       /* xả bộ đệm TRƯỚC khi fork (xem mục 2.4) */

    pid_t pid = fork();                   /* từ đây có HAI tiến trình cùng chạy tiếp */
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {                       /* ---- tiến trình CON ---- */
        x = 99;                           /* chỉ sửa bản sao của con */
        printf("[con] x = %d\n", x);
        fflush(stdout);
        /* Thay chương trình đang chạy bằng chương trình "echo" */
        execlp("echo", "echo", "[con] xin chao tu chuong trinh echo", (char *)NULL);
        perror("execlp");                 /* chỉ chạy tới đây nếu exec thất bại */
        _exit(127);
    }

    /* ---- tiến trình CHA ---- */
    int status;
    waitpid(pid, &status, 0);             /* chờ con kết thúc và "nhặt xác" */
    printf("[cha] con da ket thuc voi ma %d\n", WEXITSTATUS(status));
    printf("[cha] x van = %d (bo nho cua cha khong bi con sua)\n", x);

    /* Tiến trình con thứ hai kết thúc với mã 3 */
    pid = fork();
    if (pid == 0) _exit(3);
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) printf("[cha] con thu hai tra ve ma %d\n", WEXITSTATUS(status));
    return 0;
}
```

**Kết quả:**

```text
Truoc fork: x = 10
[con] x = 99
[con] xin chao tu chuong trinh echo
[cha] con da ket thuc voi ma 0
[cha] x van = 10 (bo nho cua cha khong bi con sua)
[cha] con thu hai tra ve ma 3
```

**Điểm cần hiểu:**

- Sau `fork`, con có **bản sao** bộ nhớ của cha. Con sửa `x = 99` không ảnh hưởng tới `x` của cha.
- Việc sao chép không tốn kém như ta tưởng: HĐH dùng kỹ thuật **copy-on-write** — hai tiến trình **dùng chung** các trang nhớ, và chỉ sao chép trang nào bị **ghi**.
- `execlp` thay toàn bộ chương trình, nên dòng `perror("execlp")` chỉ chạy khi `exec` thất bại.

### 2.4 Lỗi kinh điển: bộ đệm bị nhân đôi khi `fork`

```c
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    printf("A");            /* KHÔNG xuống dòng, KHÔNG fflush: "A" vẫn nằm trong bộ đệm */
    pid_t pid = fork();     /* bộ đệm bị sao chép sang con */
    if (pid == 0) {
        printf("C\n");      /* con in "A" (thừa hưởng) + "C" */
        return 0;
    }
    waitpid(pid, NULL, 0);
    printf("P\n");          /* cha in "A" + "P" */
    return 0;
}
```

**Kết quả:**

```text
AC
AP
```

Chữ `A` được in **hai lần**, dù chỉ có một lệnh `printf("A")`. Lý do: `A` vẫn nằm trong **bộ đệm stdio** (mục 1.4) lúc `fork`, và bộ đệm đó bị sao chép sang con. **Quy tắc:** luôn `fflush(stdout)` trước khi `fork`, và trong tiến trình con nên thoát bằng `_exit()` thay vì `exit()` nếu không muốn xả bộ đệm thừa hưởng từ cha.

### 2.5 Tiến trình zombie và mồ côi

- **Zombie:** con đã kết thúc nhưng cha **chưa `wait`**. HĐH phải giữ lại mục trong bảng tiến trình (để cha còn đọc được mã thoát). Nhiều zombie tích tụ sẽ làm cạn bảng tiến trình.
- **Mồ côi (orphan):** cha kết thúc trước con. Con được một tiến trình hệ thống (`init`/`systemd`, PID 1 hoặc một "subreaper") nhận làm con nuôi và `wait` giùm.

```c
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Đọc ký tự trạng thái của tiến trình từ /proc/<pid>/stat (chỉ có trên Linux) */
static char proc_state(pid_t pid) {
    char path[64], buf[512];
    snprintf(path, sizeof path, "/proc/%d/stat", (int)pid);
    FILE *f = fopen(path, "r");
    if (f == NULL) return '-';                     /* không còn tồn tại */
    if (fgets(buf, sizeof buf, f) == NULL) { fclose(f); return '?'; }
    fclose(f);
    char *p = strrchr(buf, ')');                   /* định dạng: pid (ten) TRANG_THAI ... */
    return p != NULL ? p[2] : '?';
}

int main(void) {
    pid_t pid = fork();
    if (pid == 0) {
        usleep(100 * 1000);                        /* con ngủ 100 ms */
        _exit(0);
    }
    usleep(20 * 1000);
    printf("con dang ngu        -> trang thai %c\n", proc_state(pid));
    usleep(200 * 1000);                            /* con đã kết thúc, cha chưa wait */
    printf("con xong, chua wait -> trang thai %c\n", proc_state(pid));
    waitpid(pid, NULL, 0);                         /* nhặt xác */
    printf("sau khi wait        -> trang thai %c\n", proc_state(pid));
    return 0;
}
```

**Kết quả** (ký tự trạng thái của Linux: `S` = đang ngủ, `Z` = zombie, `-` = không còn tồn tại):

```text
con dang ngu        -> trang thai S
con xong, chua wait -> trang thai Z
sau khi wait        -> trang thai -
```

**Quy tắc:** mọi tiến trình tạo con đều phải `wait` con của mình (hoặc xử lý tín hiệu `SIGCHLD`).

### 2.6 Trên Windows

Windows không có `fork`. Tạo tiến trình bằng **`CreateProcess`** — tương đương `fork` + `exec` trong một bước — và chờ bằng `WaitForSingleObject` + `GetExitCodeProcess`. (Bảng đối chiếu đầy đủ ở Phụ lục A.)

**Lệnh xem tiến trình:** Linux: `ps aux`, `ps -ef --forest`, `top`, `htop`; Windows: Task Manager, `tasklist`.

---

## Chương 3. Luồng (thread)

### 3.1 Luồng và tiến trình

**Luồng** là một **dòng thực thi** bên trong tiến trình. Một tiến trình có thể có nhiều luồng, **dùng chung** không gian địa chỉ.

| Thành phần | Riêng cho mỗi luồng | Dùng chung trong tiến trình |
|---|:-:|:-:|
| Thanh ghi CPU, con trỏ lệnh | ✅ | |
| **Stack** (biến cục bộ) | ✅ | |
| Biến `thread_local` | ✅ | |
| Heap, biến toàn cục, biến `static` | | ✅ |
| File, socket đang mở | | ✅ |
| Mã lệnh | | ✅ |

| | Nhiều tiến trình | Nhiều luồng |
|---|---|---|
| Chi phí tạo | Cao hơn | Thấp hơn |
| Chia sẻ dữ liệu | Khó (cần IPC — Chương 7) | Dễ (dùng chung bộ nhớ) |
| Cô lập lỗi | Tốt: một tiến trình hỏng không kéo theo tiến trình khác | Kém: một luồng gây lỗi bộ nhớ làm sập **cả tiến trình** |
| Nguy cơ lỗi đồng bộ | Thấp | **Cao** (Phần II) |
| Ví dụ | Trình duyệt Chrome (mỗi tab một tiến trình), web server kiểu Apache prefork | Game engine, cơ sở dữ liệu, IDE |

### 3.2 Luồng nhân và luồng người dùng

- **Luồng nhân (kernel thread):** HĐH biết và lập lịch từng luồng. `std::thread`, `pthread` trên Linux và thread của Windows đều thuộc loại này (mô hình 1:1).
- **Luồng người dùng / luồng xanh (green thread):** do thư viện hoặc runtime quản lý, HĐH không biết đến. Nhiều luồng người dùng chạy trên ít luồng nhân (mô hình M:N) — ví dụ goroutine của Go. **Coroutine** (Phần III) là họ hàng gần của ý tưởng này.

---

## Chương 4. Lập lịch CPU

### 4.1 Khái niệm

**Bộ lập lịch (scheduler)** quyết định tiến trình/luồng nào ở trạng thái *Sẵn sàng* được chạy tiếp, và chạy bao lâu.

- **Không ưu tiên trước (non-preemptive):** tiến trình chạy cho đến khi tự nhường (kết thúc hoặc chờ I/O).
- **Ưu tiên trước (preemptive):** HĐH có thể **ngắt** tiến trình đang chạy (nhờ ngắt đồng hồ) để chuyển sang tiến trình khác. Mọi HĐH hiện đại đều dùng cách này.
- **Chuyển ngữ cảnh (context switch):** lưu trạng thái tiến trình cũ, nạp trạng thái tiến trình mới. Mỗi lần chuyển tốn thời gian và làm "nguội" cache CPU — nên không thể chuyển quá thường xuyên.

**Các thước đo** (với mỗi tiến trình):

| Thước đo | Công thức | Ai quan tâm |
|---|---|---|
| **Thời gian hoàn thành** (turnaround) | lúc xong − lúc đến | Công việc chạy nền |
| **Thời gian chờ** (waiting) | hoàn thành − thời gian chạy | Hiệu quả chung |
| **Thời gian phản hồi** (response) | lần đầu được chạy − lúc đến | **Ứng dụng tương tác**, game |

### 4.2 Các thuật toán kinh điển

| Thuật toán | Ý tưởng | Ưu | Nhược |
|---|---|---|---|
| **FCFS** (đến trước phục vụ trước) | Hàng đợi FIFO | Đơn giản, công bằng theo thứ tự | Việc ngắn phải chờ việc dài (**hiệu ứng đoàn xe**) |
| **SJF** (việc ngắn nhất trước) | Chọn việc có thời gian chạy ngắn nhất | Thời gian chờ trung bình **tối ưu** (trong nhóm không ưu tiên trước) | Phải biết trước thời gian chạy; việc dài có thể **chờ mãi** |
| **SRTF** (thời gian còn lại ngắn nhất) | SJF có ưu tiên trước | Tốt hơn SJF | Như SJF, thêm chi phí chuyển ngữ cảnh |
| **Round Robin (RR)** | Mỗi việc chạy tối đa một **lượng thời gian (quantum)** rồi xuống cuối hàng | **Phản hồi nhanh**, công bằng | Thời gian chờ trung bình thường cao; hiệu năng phụ thuộc quantum |
| **Ưu tiên (priority)** | Chạy việc có độ ưu tiên cao nhất | Linh hoạt | **Đói (starvation)** → khắc phục bằng **lão hóa (aging)**: tăng dần ưu tiên cho việc chờ lâu |
| **Hàng đợi đa mức phản hồi (MLFQ)** | Nhiều hàng đợi ưu tiên; việc dùng hết quantum bị hạ mức | Tự thích nghi: việc tương tác ở mức cao, việc tính toán nặng ở mức thấp | Nhiều tham số cần tinh chỉnh |

**Trong thực tế:** Linux dùng bộ lập lịch CFS (từ năm 2007) nhằm chia CPU **công bằng** theo "thời gian chạy ảo", và từ kernel 6.6 chuyển sang EEVDF — cùng tinh thần công bằng nhưng xử lý độ trễ tốt hơn. Windows dùng lập lịch theo **độ ưu tiên** (32 mức) kết hợp nâng ưu tiên tạm thời cho luồng tương tác.

### 4.3 Ví dụ tính toán

Bốn tiến trình (đơn vị thời gian tùy ý):

| Tiến trình | Thời điểm đến | Thời gian chạy |
|---|---|---|
| P1 | 0 | 5 |
| P2 | 1 | 3 |
| P3 | 2 | 8 |
| P4 | 3 | 6 |

**FCFS:**

```text
| P1  | P2 |   P3    |  P4   |
0     5    8         16      22
```

Thời gian chờ: P1 = 0, P2 = 5 − 1 = 4, P3 = 8 − 2 = 6, P4 = 16 − 3 = 13 → **trung bình 5,75**.

**SJF (không ưu tiên trước):** lúc 0 chỉ có P1; lúc 5, chọn việc ngắn nhất trong P2 (3), P3 (8), P4 (6):

```text
| P1  | P2 |  P4   |   P3    |
0     5    8       14        22
```

Thời gian chờ: 0, 4, 12, 5 → **trung bình 5,25**.

**SRTF:** lúc 1, P2 (cần 3) ngắn hơn phần còn lại của P1 (còn 4) → P2 giành CPU:

```text
|P1| P2 | P1 |  P4   |   P3    |
0  1    4    8       14        22
```

Thời gian chờ: P1 = 8 − 0 − 5 = 3, P2 = 0, P3 = 12, P4 = 5 → **trung bình 5,00**.

**Round Robin, quantum = 2** (quy ước: tiến trình mới đến vào hàng đợi **trước** tiến trình vừa bị ngắt):

```text
|P1|P2|P3|P1|P4|P2|P3|P1|P4|P3|P4|P3|
0  2  4  6  8  10 11 13 14 16 18 20 22
```

Thời gian chờ trung bình **9,75** — cao nhất, nhưng thời gian **phản hồi** trung bình chỉ **2,0** (so với 5,75 của FCFS): mọi tiến trình đều sớm được chạy.

### 4.4 Mô phỏng bằng Python

```python
# Mô phỏng các thuật toán lập lịch CPU trên cùng một tập tiến trình
from collections import deque

procs = [("P1", 0, 5), ("P2", 1, 3), ("P3", 2, 8), ("P4", 3, 6)]   # (tên, thời điểm đến, thời gian chạy)

def report(name, timeline, finish, first_run):
    gantt = " ".join(f"{p}[{s}-{e}]" for p, s, e in timeline)
    wait = {p: finish[p] - a - b for p, a, b in procs}          # chờ = xong - đến - chạy
    tat = {p: finish[p] - a for p, a, b in procs}               # hoàn thành = xong - đến
    resp = {p: first_run[p] - a for p, a, b in procs}           # phản hồi = lần chạy đầu - đến
    n = len(procs)
    print(f"{name:5} {gantt}")
    print(f"      TB cho = {sum(wait.values()) / n:.2f}, TB hoan thanh = {sum(tat.values()) / n:.2f},"
          f" TB phan hoi = {sum(resp.values()) / n:.2f}")

def run(name, pick, preemptive, quantum=None):
    t, rem = 0, {p: b for p, a, b in procs}
    arrival = {p: a for p, a, b in procs}
    finish, first_run, timeline = {}, {}, []
    queue, arrived = deque(), set()

    def admit(now):                       # đưa tiến trình vừa đến vào hàng đợi (theo thứ tự đến)
        for p, a, b in procs:
            if a <= now and p not in arrived:
                arrived.add(p)
                queue.append(p)

    admit(t)
    while len(finish) < len(procs):
        if not queue:                     # CPU rảnh: nhảy tới lần đến kế tiếp
            t = min(a for p, a, b in procs if p not in arrived)
            admit(t)
            continue
        p = pick(queue, rem)
        queue.remove(p)
        first_run.setdefault(p, t)
        if quantum is not None:
            run_for = min(quantum, rem[p])
        elif preemptive:                  # chạy đến khi có tiến trình mới đến (có thể giành CPU)
            nxt = [a for q, a, b in procs if a > t]
            run_for = min(rem[p], (min(nxt) - t) if nxt else rem[p])
        else:
            run_for = rem[p]
        if timeline and timeline[-1][0] == p and timeline[-1][2] == t:
            timeline[-1] = (p, timeline[-1][1], t + run_for)      # gộp đoạn liền nhau
        else:
            timeline.append((p, t, t + run_for))
        t += run_for
        rem[p] -= run_for
        admit(t)                          # tiến trình mới đến vào hàng đợi TRƯỚC...
        if rem[p] == 0:
            finish[p] = t
        else:
            queue.append(p)               # ...tiến trình vừa bị ngắt quay lại cuối hàng
    report(name, timeline, finish, first_run)

fifo = lambda q, rem: q[0]                            # người đến trước
shortest = lambda q, rem: min(q, key=lambda p: rem[p])   # việc (còn lại) ngắn nhất

run("FCFS", fifo, preemptive=False)
run("SJF", shortest, preemptive=False)
run("SRTF", shortest, preemptive=True)
run("RR-2", fifo, preemptive=True, quantum=2)
```

**Kết quả:**

```text
FCFS  P1[0-5] P2[5-8] P3[8-16] P4[16-22]
      TB cho = 5.75, TB hoan thanh = 11.25, TB phan hoi = 5.75
SJF   P1[0-5] P2[5-8] P4[8-14] P3[14-22]
      TB cho = 5.25, TB hoan thanh = 10.75, TB phan hoi = 5.25
SRTF  P1[0-1] P2[1-4] P1[4-8] P4[8-14] P3[14-22]
      TB cho = 5.00, TB hoan thanh = 10.50, TB phan hoi = 4.25
RR-2  P1[0-2] P2[2-4] P3[4-6] P1[6-8] P4[8-10] P2[10-11] P3[11-13] P1[13-14] P4[14-16] P3[16-18] P4[18-20] P3[20-22]
      TB cho = 9.75, TB hoan thanh = 15.25, TB phan hoi = 2.00
```

**Nhận xét:** không có thuật toán "tốt nhất" cho mọi thước đo. SRTF tối ưu thời gian chờ nhưng cần biết trước thời gian chạy (điều không thể trong thực tế — HĐH phải **ước lượng** từ lịch sử); RR tối ưu phản hồi — quan trọng với giao diện và game.

---

## Chương 5. Quản lý bộ nhớ

### 5.1 Nhắc lại bộ nhớ ảo

Mỗi tiến trình có **không gian địa chỉ ảo** riêng, được chia thành các **trang** (thường 4 KiB); **bảng trang** ánh xạ trang ảo sang **khung** vật lý trong RAM. (Chi tiết về bảng trang, TLB và cách dịch địa chỉ có trong tài liệu kiến trúc máy tính.)

Khi truy cập một trang **không có trong RAM** → **lỗi trang (page fault)** → HĐH nạp trang đó vào. Nếu RAM đã đầy, HĐH phải chọn một trang để **đẩy ra** (xuống vùng swap, hoặc bỏ đi nếu trang đó có sẵn bản sao trên đĩa). Chọn trang nào là bài toán **thay trang**.

### 5.2 Các thuật toán thay trang

| Thuật toán | Đẩy ra trang... | Ghi chú |
|---|---|---|
| **FIFO** | Vào bộ nhớ **sớm nhất** | Đơn giản; có thể gặp **nghịch lý Belady** |
| **Tối ưu (Optimal)** | Sẽ được dùng lại **muộn nhất** trong tương lai | Không cài đặt được (phải biết trước tương lai) — dùng làm **thước đo** để so sánh |
| **LRU** | **Lâu nhất chưa được dùng** | Gần với tối ưu nhờ tính cục bộ; cài đặt chính xác khá tốn kém |
| **Clock (cơ hội thứ hai)** | Xấp xỉ LRU bằng **bit truy cập** | Được dùng rộng rãi trong thực tế |

**Nghịch lý Belady:** với FIFO, **tăng** số khung nhớ có thể làm **tăng** số lỗi trang. LRU và Optimal không bị nghịch lý này.

```python
# Mô phỏng các thuật toán thay trang: đếm số lỗi trang (page fault)
def fifo(refs, frames):
    mem, faults = [], 0
    for p in refs:
        if p not in mem:
            faults += 1
            if len(mem) == frames:
                mem.pop(0)                      # bỏ trang vào SỚM NHẤT
            mem.append(p)
    return faults

def lru(refs, frames):
    mem, faults = [], 0
    for p in refs:
        if p in mem:
            mem.remove(p)                       # vừa dùng -> đưa về cuối (mới nhất)
        else:
            faults += 1
            if len(mem) == frames:
                mem.pop(0)                      # bỏ trang LÂU NHẤT CHƯA DÙNG
        mem.append(p)
    return faults

def optimal(refs, frames):
    mem, faults = [], 0
    for i, p in enumerate(refs):
        if p in mem:
            continue
        faults += 1
        if len(mem) == frames:
            future = refs[i + 1:]
            # bỏ trang sẽ được dùng lại MUỘN NHẤT (hoặc không bao giờ)
            victim = max(mem, key=lambda q: future.index(q) if q in future else len(future))
            mem.remove(victim)
        mem.append(p)
    return faults

refs = [7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1]
for name, algo in [("FIFO", fifo), ("LRU", lru), ("Optimal", optimal)]:
    print(f"{name:8} 3 khung: {algo(refs, 3):2d} loi trang")

belady = [1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5]
print(f"Belady: FIFO 3 khung = {fifo(belady, 3)} loi, 4 khung = {fifo(belady, 4)} loi")
```

**Kết quả:**

```text
FIFO     3 khung: 15 loi trang
LRU      3 khung: 12 loi trang
Optimal  3 khung:  9 loi trang
Belady: FIFO 3 khung = 9 loi, 4 khung = 10 loi
```

**Minh họa FIFO** với dãy truy cập ngắn `1, 2, 3, 4, 1, 2, 5` và 3 khung (`*` = lỗi trang):

```text
Truy cập :  1    2    3    4    1    2    5
Khung 1  :  1    1    1    4    4    4    5
Khung 2  :       2    2    2    1    1    1
Khung 3  :            3    3    3    2    2
Lỗi      :  *    *    *    *    *    *    *
```

(Trang 4 vào thay trang 1 — trang vào sớm nhất; rồi 1 thay 2, 2 thay 3, 5 thay 4.)

**Thrashing:** khi tổng bộ nhớ các tiến trình cần **vượt xa** RAM, HĐH liên tục đẩy trang ra rồi lại nạp vào — máy gần như đứng yên dù CPU rảnh. Giải pháp: giảm số tiến trình, thêm RAM, giảm lượng bộ nhớ sử dụng.

### 5.3 Copy-on-write và `mmap`

**Copy-on-write (COW):** sau `fork`, cha và con **dùng chung** trang nhớ ở chế độ chỉ đọc; khi một bên **ghi**, HĐH mới sao chép riêng trang đó. Nhờ vậy `fork` rất nhanh dù tiến trình dùng nhiều GB bộ nhớ.

**`mmap`** ánh xạ một vùng vào không gian địa chỉ:

- **Ánh xạ file:** đọc/ghi file như đọc/ghi mảng; HĐH tự nạp trang khi cần (nhanh với file lớn, truy cập ngẫu nhiên).
- **Bộ nhớ ẩn danh:** `malloc` dùng cách này cho các khối lớn.
- **Bộ nhớ dùng chung** giữa các tiến trình (`MAP_SHARED`):

```c
#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int normal = 0;                                   /* biến thường: mỗi tiến trình một bản */
    int *shared = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_ANONYMOUS, -1, 0);   /* vùng nhớ DÙNG CHUNG */
    if (shared == MAP_FAILED) { perror("mmap"); return 1; }
    *shared = 0;

    pid_t pid = fork();
    if (pid == 0) {
        normal = 42;
        *shared = 42;
        _exit(0);
    }
    waitpid(pid, NULL, 0);
    printf("bien thuong sau khi con sua: %d\n", normal);
    printf("vung nho chung sau khi con sua: %d\n", *shared);
    munmap(shared, sizeof(int));
    return 0;
}
```

**Kết quả:**

```text
bien thuong sau khi con sua: 0
vung nho chung sau khi con sua: 42
```

### 5.4 `malloc` lấy bộ nhớ từ đâu?

- Khối **nhỏ**: `malloc` quản lý một vùng heap lớn, xin thêm từ HĐH bằng `brk`/`sbrk` hoặc `mmap`, rồi tự chia nhỏ — hầu hết lời gọi `malloc` **không** cần vào nhân.
- Khối **lớn** (mặc định của glibc: từ khoảng 128 KiB): cấp phát trực tiếp bằng `mmap`, và trả lại HĐH ngay khi `free`.
- Linux mặc định **cấp phát lạc quan (overcommit)**: `malloc` thành công chưa có nghĩa là đã có RAM thật. Trang chỉ thật sự được cấp khi **ghi** lần đầu. Khi hết bộ nhớ thật, **OOM killer** sẽ chọn một tiến trình để kết thúc.

---

## Chương 6. Hệ thống file

### 6.1 Các khái niệm

- **Inode:** cấu trúc lưu **thông tin về file** (kích thước, quyền, thời gian, vị trí các khối dữ liệu) — nhưng **không** lưu tên file.
- **Thư mục:** một danh sách các cặp **(tên → số inode)**.
- **File descriptor (fd):** số nguyên chương trình dùng để chỉ một file đang mở. Mỗi tiến trình có **bảng fd** riêng; 0, 1, 2 là stdin, stdout, stderr. `open` luôn trả về **số nhỏ nhất còn trống**.
- **Vị trí đọc/ghi (offset):** mỗi file đang mở có một con trỏ vị trí, thay đổi sau mỗi lần `read`/`write`, và có thể đặt lại bằng `lseek`.

| | Liên kết cứng (hard link) | Liên kết mềm (symlink) |
|---|---|---|
| Bản chất | Thêm một **tên** trỏ vào **cùng inode** | Một file nhỏ chứa **đường dẫn** tới file khác |
| Xóa file gốc | Dữ liệu **vẫn còn** (inode chỉ bị xóa khi hết tên trỏ vào) | Symlink bị **gãy** |
| Trỏ tới thư mục / sang ổ đĩa khác | Không | Được |

```c
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    /* 0, 1, 2 đã được dùng cho stdin, stdout, stderr -> file mới nhận số nhỏ nhất còn trống */
    int fd = open("a.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    printf("file descriptor moi: %d\n", fd);

    write(fd, "Hello, file!", 12);
    lseek(fd, 7, SEEK_SET);                 /* dời "con trỏ vị trí" về byte thứ 7 */
    char buf[16] = {0};
    ssize_t n = read(fd, buf, sizeof buf - 1);
    printf("doc tu vi tri 7: \"%s\" (%zd byte)\n", buf, n);
    close(fd);

    /* Liên kết cứng (hard link): hai TÊN cùng trỏ vào MỘT inode */
    link("a.txt", "b.txt");
    /* Liên kết mềm (symlink): một file nhỏ chứa ĐƯỜNG DẪN tới file khác */
    symlink("a.txt", "c.txt");

    struct stat sa, sb, sc;
    stat("a.txt", &sa);
    stat("b.txt", &sb);
    lstat("c.txt", &sc);                    /* lstat: thông tin của chính symlink */
    printf("a.txt va b.txt cung inode: %s, so lien ket = %lu\n",
           sa.st_ino == sb.st_ino ? "co" : "khong", (unsigned long)sa.st_nlink);
    printf("c.txt la symlink: %s\n", S_ISLNK(sc.st_mode) ? "co" : "khong");

    unlink("a.txt");                        /* xóa TÊN a.txt */
    fd = open("b.txt", O_RDONLY);
    memset(buf, 0, sizeof buf);
    read(fd, buf, 5);
    close(fd);
    printf("sau khi xoa a.txt: b.txt van doc duoc \"%s\"\n", buf);
    printf("sau khi xoa a.txt: mo c.txt %s\n", open("c.txt", O_RDONLY) < 0 ? "that bai (symlink bi gay)" : "duoc");

    unlink("b.txt");
    unlink("c.txt");
    return 0;
}
```

**Kết quả:**

```text
file descriptor moi: 3
doc tu vi tri 7: "file!" (5 byte)
a.txt va b.txt cung inode: co, so lien ket = 2
c.txt la symlink: co
sau khi xoa a.txt: b.txt van doc duoc "Hello"
sau khi xoa a.txt: mo c.txt that bai (symlink bi gay)
```

### 6.2 Dữ liệu thật sự được ghi xuống đĩa khi nào?

Có **hai tầng bộ đệm**: bộ đệm stdio **trong chương trình** (mục 1.4), và **page cache trong nhân**. `write` thành công chỉ có nghĩa là dữ liệu đã vào page cache; nhân sẽ ghi xuống đĩa **sau đó**.

| Lời gọi | Đảm bảo |
|---|---|
| `fflush(f)` | Chuyển bộ đệm stdio vào nhân (gọi `write`) |
| `fsync(fd)` | Chờ đến khi dữ liệu **thực sự nằm trên đĩa** — chậm, nhưng cần thiết cho dữ liệu quan trọng (cơ sở dữ liệu, file lưu game) |

**Mẫu ghi file an toàn** (tránh hỏng file khi mất điện giữa chừng): ghi vào file tạm → `fsync` → `rename` file tạm thành tên thật (trên cùng hệ thống file, `rename` là thao tác nguyên tử).

### 6.3 Quyền và nhật ký

- **Quyền Unix:** `rwx` cho **chủ sở hữu / nhóm / người khác** — ví dụ `0644` = `rw-r--r--`.
- **Hệ thống file có nhật ký (journaling)** như ext4, NTFS, APFS: ghi **ý định thay đổi** vào nhật ký trước, nhờ vậy khôi phục nhanh và nhất quán sau sự cố.

---

## Chương 7. Giao tiếp liên tiến trình và tín hiệu

### 7.1 Các cơ chế IPC

| Cơ chế | Đặc điểm | Dùng khi |
|---|---|---|
| **Pipe** | Dòng byte **một chiều**, giữa các tiến trình có quan hệ cha–con | Nối lệnh shell (`a \| b`), cha giao việc cho con |
| **Named pipe (FIFO)** | Như pipe nhưng có **tên** trong hệ thống file | Hai tiến trình không liên quan |
| **Bộ nhớ dùng chung** | **Nhanh nhất** (không sao chép) nhưng phải tự đồng bộ | Dữ liệu lớn, hiệu năng cao |
| **Hàng đợi thông điệp** | Giữ ranh giới thông điệp | Giao tiếp có cấu trúc |
| **Socket** | Dùng được cả trên **cùng máy** (Unix domain socket) và **qua mạng** | Linh hoạt nhất |
| **Tín hiệu** | Thông báo **bất đồng bộ** rất ngắn (chỉ là một con số) | Yêu cầu dừng, thông báo sự kiện |

### 7.2 Pipe

`pipe(fds)` tạo hai fd: `fds[0]` để **đọc**, `fds[1]` để **ghi**. Kết hợp với `fork` và `dup2` (gán fd này sang số fd khác), ta tái tạo được `lệnh1 | lệnh2` của shell:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Làm giống lệnh shell:  (danh sách trái cây) | sort  */
int main(void) {
    int fds[2];
    if (pipe(fds) != 0) { perror("pipe"); return 1; }   /* fds[0]: đầu đọc, fds[1]: đầu ghi */

    pid_t pid = fork();
    if (pid == 0) {
        dup2(fds[0], STDIN_FILENO);     /* stdin của con := đầu đọc của ống */
        close(fds[0]);
        close(fds[1]);                  /* QUAN TRỌNG: đóng đầu ghi thừa, nếu không sort chờ mãi */
        setenv("LC_ALL", "C", 1);       /* thứ tự sắp xếp cố định */
        execlp("sort", "sort", (char *)NULL);
        perror("execlp");
        _exit(127);
    }

    close(fds[0]);                      /* cha chỉ ghi */
    const char *data = "chuoi\ntao\nbuoi\ncam\n";
    write(fds[1], data, strlen(data));
    close(fds[1]);                      /* đóng đầu ghi -> sort nhận EOF và bắt đầu in */
    waitpid(pid, NULL, 0);
    printf("(cha: sort da xong)\n");
    return 0;
}
```

**Kết quả:**

```text
buoi
cam
chuoi
tao
(cha: sort da xong)
```

> ⚠️ **Lỗi kinh điển:** quên đóng đầu ghi thừa trong tiến trình đọc. Khi đó, bên đọc **không bao giờ nhận được EOF** (vì vẫn còn một đầu ghi đang mở — chính là của nó) và **treo mãi**.

### 7.3 Tín hiệu (signal)

Tín hiệu là **ngắt mềm** gửi tới tiến trình: `SIGINT` (Ctrl+C), `SIGTERM` (yêu cầu kết thúc), `SIGKILL` (buộc kết thúc — **không thể** bắt), `SIGSEGV` (truy cập bộ nhớ sai), `SIGCHLD` (con kết thúc), `SIGPIPE` (ghi vào pipe/socket đã đóng)...

```c
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t got_usr1 = 0;   /* kiểu an toàn để sửa trong trình xử lý tín hiệu */
static volatile sig_atomic_t got_term = 0;

static void on_signal(int sig) {
    /* Trong trình xử lý tín hiệu CHỈ làm việc tối thiểu: đặt cờ.
       Không gọi printf, malloc... (không an toàn khi bị ngắt giữa chừng). */
    if (sig == SIGUSR1) got_usr1++;
    if (sig == SIGTERM) got_term = 1;
}

int main(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = on_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    kill(getpid(), SIGUSR1);          /* tự gửi tín hiệu cho mình (giống lệnh: kill -USR1 <pid>) */
    kill(getpid(), SIGUSR1);
    printf("Da nhan SIGUSR1 %d lan\n", (int)got_usr1);

    /* Vòng lặp chính kiểm tra cờ — cách "tắt server êm" phổ biến */
    int work_done = 0;
    while (!got_term) {
        work_done++;                              /* ... làm việc chính ... */
        if (work_done == 3) kill(getpid(), SIGTERM);   /* giả lập: có người gửi SIGTERM */
    }
    printf("Nhan SIGTERM sau %d luot lam viec -> don dep va thoat\n", work_done);
    return 0;
}
```

**Kết quả:**

```text
Da nhan SIGUSR1 2 lan
Nhan SIGTERM sau 3 luot lam viec -> don dep va thoat
```

**Quy tắc cho trình xử lý tín hiệu:** nó có thể chạy **xen vào bất kỳ đâu** trong chương trình — kể cả giữa một lời gọi `printf` hay `malloc`. Vì vậy chỉ được gọi các hàm **async-signal-safe** (như `write`, `_exit`) và chỉ nên **đặt cờ** kiểu `volatile sig_atomic_t`, để vòng lặp chính xử lý. Dùng `sigaction` thay cho `signal` cũ.

Trên Windows, tương đương gần nhất cho Ctrl+C là `SetConsoleCtrlHandler`. Trong C++ đa nền tảng, thư viện như Asio có `signal_set` (Chương 19).

---

## Bài tập Phần I

**Bài I.1.** Chương trình sau in ra bao nhiêu dòng `x`? Giải thích.

```c
fork();
fork();
fork();
printf("x\n");
```

**Bài I.2.** Với 4 tiến trình ở mục 4.3, vẽ biểu đồ Gantt và tính thời gian chờ trung bình với **Round Robin, quantum = 3**. So sánh với quantum = 2, và giải thích điều gì xảy ra khi quantum rất lớn (ví dụ 10).

**Bài I.3.** Với dãy truy cập `1, 2, 3, 4, 2, 1, 5, 6, 2, 1, 2, 3, 7, 6, 3, 2, 1, 2, 3, 6`, đếm số lỗi trang của FIFO, LRU và Optimal với 3 khung và 4 khung.

**Bài I.4.** Viết chương trình tính tổng 1..3 000 000 bằng **3 tiến trình con**, mỗi con tính một đoạn và gửi kết quả về cha **qua pipe**.

**Bài I.5.** Một server tạo tiến trình con cho mỗi yêu cầu nhưng không bao giờ gọi `wait`. Sau vài ngày, server không tạo được tiến trình mới nữa. Vì sao?

<details>
<summary><b>Lời giải Phần I</b> (bấm để mở)</summary>

**I.1.** **8 dòng.** Mỗi `fork` nhân đôi số tiến trình: 1 → 2 → 4 → 8, và cả 8 tiến trình đều chạy `printf`. (Đã kiểm tra bằng chương trình thật, đếm được đúng 8 dòng.)

**I.2.** Chạy bộ mô phỏng ở mục 4.4 với `quantum=3`:

```text
RR-3  P1[0-3] P2[3-6] P3[6-9] P4[9-12] P1[12-14] P3[14-17] P4[17-20] P3[20-22]
      TB cho = 8.50, TB hoan thanh = 14.00, TB phan hoi = 3.00
```

Quantum lớn hơn → ít chuyển ngữ cảnh hơn, thời gian chờ giảm (9,75 → 8,50) nhưng phản hồi chậm hơn (2,0 → 3,0). Khi quantum **lớn hơn mọi thời gian chạy** (ví dụ 10), RR **trở thành FCFS**: kết quả trùng với FCFS (thời gian chờ trung bình 5,75).

**I.3.**

| | FIFO | LRU | Optimal |
|---|---|---|---|
| 3 khung | 16 | 15 | 11 |
| 4 khung | 14 | 10 | 8 |

**I.4.**

```c
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

/* Tính tổng 1..3000000 bằng 3 tiến trình con; mỗi con gửi kết quả về cha qua pipe */
int main(void) {
    const long N = 3000000, PARTS = 3;
    int fds[PARTS][2];
    for (long p = 0; p < PARTS; p++) {
        pipe(fds[p]);
        if (fork() == 0) {
            close(fds[p][0]);                            /* con chỉ ghi */
            long from = p * (N / PARTS) + 1, to = (p + 1) * (N / PARTS);
            long long s = 0;
            for (long i = from; i <= to; i++) s += i;
            write(fds[p][1], &s, sizeof s);              /* gửi 8 byte kết quả (cùng máy nên không lo thứ tự byte) */
            close(fds[p][1]);
            _exit(0);
        }
        close(fds[p][1]);                                /* cha chỉ đọc */
    }
    long long total = 0;
    for (long p = 0; p < PARTS; p++) {
        long long part = 0;
        read(fds[p][0], &part, sizeof part);
        close(fds[p][0]);
        printf("tien trinh con %ld: %lld\n", p, part);
        total += part;
    }
    while (wait(NULL) > 0) {}
    printf("tong = %lld (dung = %lld)\n", total, (long long)N * (N + 1) / 2);
    return 0;
}
```

**Kết quả:**

```text
tien trinh con 0: 500000500000
tien trinh con 1: 1500000500000
tien trinh con 2: 2500000500000
tong = 4500001500000 (dung = 4500001500000)
```

**I.5.** Mọi tiến trình con đã kết thúc đều trở thành **zombie**, vẫn chiếm một mục trong bảng tiến trình. Khi số zombie chạm giới hạn số tiến trình của người dùng/hệ thống, `fork` báo lỗi. Sửa: gọi `waitpid` (có thể với `WNOHANG` trong vòng lặp chính), hoặc xử lý `SIGCHLD`.

</details>

---

# PHẦN II — ĐA LUỒNG (CONCURRENCY & MULTITHREADING)

## Chương 8. Đồng thời và song song

- **Đồng thời (concurrency):** nhiều việc **cùng tiến triển** trong một khoảng thời gian — có thể chỉ trên **một lõi**, bằng cách luân phiên.
- **Song song (parallelism):** nhiều việc **thực sự chạy cùng lúc** trên **nhiều lõi**.

```text
Đồng thời trên 1 lõi:   lõi 0: [A][B][A][C][B][A][C]...     (luân phiên)
Song song trên 3 lõi:   lõi 0: [AAAAAAAAAAAA]
                        lõi 1: [BBBBBBBBBBBB]
                        lõi 2: [CCCCCCCCCCCC]
```

| Loại công việc | Đặc điểm | Công cụ phù hợp |
|---|---|---|
| **CPU-bound** (tính toán nặng) | CPU luôn bận | **Nhiều luồng** trên nhiều lõi (tăng tốc tối đa khoảng bằng số lõi, bị giới hạn bởi định luật Amdahl) |
| **I/O-bound** (chờ mạng, đĩa) | Phần lớn thời gian là **chờ** | **Async I/O** (Phần III) hoặc luồng; tăng tốc nhờ "chờ song song", không cần nhiều lõi |

**Vì sao đa luồng khó?** Vì các luồng **dùng chung bộ nhớ**, còn HĐH có thể **ngắt một luồng ở bất kỳ lệnh máy nào**. Số cách xen kẽ giữa các luồng là khổng lồ, và lỗi thường chỉ xuất hiện ở một cách xen kẽ hiếm gặp.

---

## Chương 9. Luồng trong C++

### 9.1 `std::thread` và `std::jthread`

```cpp
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// Hàm chạy trong luồng: tính tổng 1..n, ghi kết quả qua tham chiếu
void sum_to(long n, long& out) {
    long s = 0;
    for (long i = 1; i <= n; ++i) s += i;
    out = s;
}

int main() {
    std::cout << "So luong phan cung: " << (std::thread::hardware_concurrency() > 0 ? "biet" : "khong ro") << '\n';

    // 1) Tạo luồng từ hàm; truyền tham chiếu phải dùng std::ref
    long r1 = 0, r2 = 0;
    std::thread t1(sum_to, 100, std::ref(r1));
    std::thread t2(sum_to, 1000, std::ref(r2));
    t1.join();                     // BẮT BUỘC join (hoặc detach) trước khi đối tượng thread bị hủy
    t2.join();
    std::cout << "sum_to(100) = " << r1 << ", sum_to(1000) = " << r2 << '\n';

    // 2) Tạo luồng từ lambda; mỗi luồng ghi vào ô RIÊNG của vector -> không cần khóa
    std::vector<std::string> results(4);
    std::vector<std::thread> workers;
    for (int i = 0; i < 4; ++i)
        workers.emplace_back([i, &results] { results[i] = "luong " + std::to_string(i) + " xong"; });
    for (auto& w : workers) w.join();
    for (const auto& s : results) std::cout << s << '\n';     // in theo thứ tự cố định

    // 3) std::jthread (C++20): tự join khi bị hủy, hỗ trợ yêu cầu dừng
    {
        std::jthread worker([](std::stop_token st) {
            int ticks = 0;
            while (!st.stop_requested()) {
                ++ticks;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            std::cout << "worker nhan yeu cau dung, da chay it nhat 1 tick: " << (ticks >= 1 ? "co" : "khong") << '\n';
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        worker.request_stop();
    }   // jthread tự join ở đây
    std::cout << "ket thuc main\n";
}
```

**Kết quả:**

```text
So luong phan cung: biet
sum_to(100) = 5050, sum_to(1000) = 500500
luong 0 xong
luong 1 xong
luong 2 xong
luong 3 xong
worker nhan yeu cau dung, da chay it nhat 1 tick: co
ket thuc main
```

**Những điều bắt buộc phải nhớ:**

1. Trước khi đối tượng `std::thread` bị hủy, phải gọi **`join()`** (chờ luồng xong) hoặc **`detach()`** (thả luồng chạy độc lập). Quên cả hai → chương trình bị dừng bằng `std::terminate`.
2. Tham số được **sao chép** vào luồng. Muốn truyền tham chiếu phải bọc bằng `std::ref`.
3. Lambda bắt biến **theo tham chiếu** (`[&]`) thì biến đó phải **sống lâu hơn** luồng.
4. **Ngoại lệ** không được bắt bên trong luồng sẽ kết thúc cả chương trình. Muốn chuyển ngoại lệ về luồng khác, dùng `std::future` (Chương 13).
5. `std::jthread` (C++20) **tự join** khi bị hủy và hỗ trợ **yêu cầu dừng** qua `std::stop_token` — nên ưu tiên dùng.
6. Thứ tự các luồng chạy là **không xác định**. Muốn kết quả có thứ tự, hãy cho mỗi luồng ghi vào **ô riêng** rồi in sau khi `join`, như ví dụ trên.

**`thread_local`:** biến có **một bản riêng cho mỗi luồng** — hữu ích cho bộ đệm tạm, bộ sinh số ngẫu nhiên...

---

## Chương 10. Race condition và vùng găng

### 10.1 Vì sao `++counter` không an toàn?

`++counter` trông như một lệnh, nhưng CPU thực hiện **ba bước**: đọc giá trị → cộng 1 → ghi lại. Hai luồng có thể xen kẽ:

```text
Luồng 1: đọc (5) ─────────── cộng → 6 ── ghi 6
Luồng 2:        đọc (5) ── cộng → 6 ───────────── ghi 6      ← mất một lần tăng!
```

**Race condition:** kết quả phụ thuộc vào **thứ tự xen kẽ** của các luồng. **Data race** (một loại cụ thể): hai luồng cùng truy cập một vùng nhớ, **ít nhất một luồng ghi**, không có đồng bộ — trong C++ đây là **hành vi không xác định (UB)**.

**Vùng găng (critical section):** đoạn code truy cập dữ liệu dùng chung, cần đảm bảo **tại mỗi thời điểm chỉ một luồng** được thực hiện (**loại trừ lẫn nhau** — mutual exclusion).

### 10.2 Thí nghiệm

```cpp
#include <atomic>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

const int THREADS = 4, N = 100000;
const int N_BIG = 20000000;    // vòng lặp dài hơn một "lượt CPU" -> luồng bị ngắt giữa chừng

long run(auto body) {
    std::vector<std::thread> ts;
    for (int t = 0; t < THREADS; ++t) ts.emplace_back(body);
    for (auto& th : ts) th.join();
    return 0;
}

int main(int argc, char* argv[]) {
    bool unsafe = argc > 1 && std::strcmp(argv[1], "unsafe") == 0;   // "./race unsafe" để xem data race

    if (unsafe) {
        long counter = 0;
        run([&] { for (int i = 0; i < N_BIG; ++i) ++counter; });        // DATA RACE!
        std::cout << "Khong khoa : " << counter << " (mong doi " << static_cast<long>(THREADS) * N_BIG << ")\n";
        return 0;
    }

    long counter = 0;
    std::mutex m;
    run([&] {
        for (int i = 0; i < N; ++i) {
            std::lock_guard<std::mutex> lock(m);                         // vùng găng được bảo vệ
            ++counter;
        }
    });
    std::cout << "Dung mutex : " << counter << '\n';

    std::atomic<long> acounter{0};
    run([&] { for (int i = 0; i < N; ++i) ++acounter; });               // phép tăng nguyên tử
    std::cout << "Dung atomic: " << acounter << '\n';
}
```

**Kết quả** (chế độ mặc định — có đồng bộ):

```text
Dung mutex : 400000
Dung atomic: 400000
```

**Chế độ `./race unsafe`** (không đồng bộ, 4 luồng × 20 triệu lần tăng, biên dịch với `-O0`):

*Ví dụ kết quả* (mỗi lần chạy một khác):

```text
Khong khoa : 41523516 (mong doi 80000000)
Khong khoa : 40547273 (mong doi 80000000)
Khong khoa : 35335452 (mong doi 80000000)
```

Gần **một nửa** số lần tăng bị mất.

> 🔍 **Phát hiện trong quá trình kiểm thử:** phiên bản đầu của thí nghiệm chỉ dùng 100 000 lần tăng mỗi luồng — và chạy **8 lần liền đều cho kết quả đúng**! Vì máy thử nghiệm có 1 lõi, mỗi luồng chạy xong vòng lặp ngắn trước khi bị HĐH ngắt, nên các luồng không bao giờ xen kẽ. Đây là bài học quan trọng nhất về đa luồng: **chạy thử đúng nhiều lần KHÔNG chứng minh code không có lỗi.** Trên máy nhiều lõi, hoặc khi tải thay đổi, lỗi sẽ xuất hiện.

### 10.3 ThreadSanitizer — bắt lỗi chắc chắn hơn chạy thử

Biên dịch với `-fsanitize=thread` (GCC, Clang), ThreadSanitizer theo dõi mọi truy cập bộ nhớ và báo data race **ngay cả khi lần chạy đó cho kết quả đúng**:

```bash
g++ -std=c++20 -pthread -fsanitize=thread -g -O1 race.cpp -o race_tsan
./race_tsan unsafe
```

*Kết quả* (rút gọn):

```text
WARNING: ThreadSanitizer: data race (pid=725)
  Read of size 8 at 0x7ffd0b6246d0 by thread T2:
    #0 operator() race.cpp:23
  ...
  Previous write of size 8 at ... by thread T1:
    #0 operator() race.cpp:23
  ...
SUMMARY: ThreadSanitizer: data race race.cpp:23 in operator()
```

Dòng 23 chính là `++counter` không được bảo vệ. Chế độ mặc định (dùng mutex/atomic) chạy dưới ThreadSanitizer **không có cảnh báo nào**.

> 💡 **Hãy chạy ThreadSanitizer trong quá trình phát triển mọi chương trình đa luồng.** (Không dùng chung với AddressSanitizer trong cùng một bản build. Nếu trình biên dịch bạn dùng — ví dụ MSVC — không hỗ trợ ThreadSanitizer, có thể kiểm tra code đa luồng bằng GCC/Clang trên Linux hoặc WSL.)

### 10.4 Công cụ đồng bộ trong C++

| Công cụ | Dùng khi |
|---|---|
| `std::mutex` | Khóa cơ bản |
| `std::lock_guard<std::mutex>` | Khóa trong **cả một khối** (RAII — tự mở khi ra khỏi khối) |
| `std::unique_lock<std::mutex>` | Cần mở/khóa lại giữa chừng, hoặc dùng với **condition variable** |
| `std::scoped_lock` (C++17) | Khóa **nhiều** mutex cùng lúc, không deadlock |
| `std::shared_mutex` + `std::shared_lock` | **Nhiều người đọc** hoặc **một người ghi** |
| `std::atomic<T>` | Biến đơn giản (bộ đếm, cờ) — không cần khóa |
| `std::call_once` / biến `static` cục bộ | Khởi tạo **đúng một lần** (an toàn luồng) |

**Không bao giờ** gọi `lock()`/`unlock()` thủ công — nếu có ngoại lệ hoặc `return` sớm ở giữa, mutex sẽ bị khóa mãi mãi.

**Giữ vùng găng càng nhỏ càng tốt:** không đọc file, gọi mạng, hay gọi callback lạ khi đang giữ khóa.

---

## Chương 11. Deadlock

### 11.1 Bốn điều kiện Coffman

Deadlock chỉ xảy ra khi **cả bốn** điều kiện cùng đúng:

1. **Loại trừ lẫn nhau:** tài nguyên chỉ một luồng dùng được mỗi lúc.
2. **Giữ và chờ:** luồng giữ một tài nguyên trong khi chờ tài nguyên khác.
3. **Không thể tước đoạt:** không lấy được tài nguyên từ luồng đang giữ.
4. **Chờ vòng tròn:** A chờ B, B chờ A (hoặc một vòng dài hơn).

Phá **một** điều kiện là đủ tránh deadlock. Cách thực tế nhất: phá **chờ vòng tròn** bằng cách **luôn khóa theo cùng một thứ tự**.

### 11.2 Thí nghiệm

```cpp
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std::chrono_literals;

std::mutex account_a, account_b;

// Thử khóa m trong tối đa `timeout`; trả về true nếu lấy được khóa
bool lock_with_timeout(std::mutex& m, std::chrono::milliseconds timeout) {
    auto deadline = std::chrono::steady_clock::now() + timeout;
    while (!m.try_lock()) {
        if (std::chrono::steady_clock::now() >= deadline) return false;
        std::this_thread::sleep_for(5ms);
    }
    return true;
}

// Khóa theo thứ tự (first, second). Nếu chờ khóa thứ hai quá 300 ms -> bỏ cuộc (dấu hiệu deadlock).
void transfer(std::mutex& first, std::mutex& second, bool& ok) {
    std::lock_guard<std::mutex> l1(first);
    std::this_thread::sleep_for(50ms);                 // "làm việc" -> tạo cơ hội cho luồng kia
    ok = lock_with_timeout(second, 300ms);
    if (ok) second.unlock();
}

int main() {
    // Tình huống 1: hai luồng khóa NGƯỢC thứ tự nhau
    bool ok1 = false, ok2 = false;
    {
        std::thread t1(transfer, std::ref(account_a), std::ref(account_b), std::ref(ok1));
        std::thread t2(transfer, std::ref(account_b), std::ref(account_a), std::ref(ok2));
        t1.join();
        t2.join();
    }
    std::cout << "Khoa nguoc thu tu: co luong phai bo cuoc sau 300 ms: " << (!ok1 || !ok2 ? "co" : "khong")
              << " -> khong co timeout thi ca hai se cho nhau mai mai (deadlock)\n";

    // Tình huống 2: std::scoped_lock khóa nhiều mutex cùng lúc, tự tránh deadlock
    std::mutex a, b;
    int balance_a = 100, balance_b = 100;
    auto move_money = [&](std::mutex& from_m, int& from, std::mutex& to_m, int& to, int amount) {
        for (int i = 0; i < 1000; ++i) {
            std::scoped_lock lock(from_m, to_m);        // thứ tự truyền vào không quan trọng
            from -= amount;
            to += amount;
        }
    };
    std::thread t3(move_money, std::ref(a), std::ref(balance_a), std::ref(b), std::ref(balance_b), 1);
    std::thread t4(move_money, std::ref(b), std::ref(balance_b), std::ref(a), std::ref(balance_a), 1);
    t3.join();
    t4.join();
    std::cout << "scoped_lock: A = " << balance_a << ", B = " << balance_b
              << ", tong = " << balance_a + balance_b << '\n';
}
```

**Kết quả:**

```text
Khoa nguoc thu tu: co luong phai bo cuoc sau 300 ms: co -> khong co timeout thi ca hai se cho nhau mai mai (deadlock)
scoped_lock: A = 100, B = 100, tong = 200
```

Hai luồng khóa `account_a` và `account_b` theo thứ tự **ngược nhau** → mỗi luồng giữ một khóa và chờ khóa kia. Nhờ thời gian chờ tối đa 300 ms, ít nhất một luồng bỏ cuộc và chương trình không bị treo — nhưng trong code thật (khóa không có timeout), chương trình sẽ **đứng vĩnh viễn**. Với `std::scoped_lock`, việc chuyển tiền qua lại 2000 lần diễn ra an toàn, tổng tiền được bảo toàn.

### 11.3 ThreadSanitizer phát hiện deadlock **tiềm ẩn**

```cpp
#include <iostream>
#include <mutex>
#include <thread>

std::mutex a, b;

int main() {
    // Hai luồng chạy LẦN LƯỢT nên chương trình KHÔNG bao giờ bị treo khi chạy thử...
    std::thread t1([] { std::lock_guard la(a); std::lock_guard lb(b); });   // khóa a rồi b
    t1.join();
    std::thread t2([] { std::lock_guard lb(b); std::lock_guard la(a); });   // khóa b rồi a
    t2.join();
    std::cout << "Chay xong, khong bi treo\n";
    // ...nhưng thứ tự khóa ngược nhau là một deadlock TIỀM ẨN — ThreadSanitizer vẫn phát hiện được
}
```

**Kết quả** (chạy thường — không bị treo):

```text
Chay xong, khong bi treo
```

*Kết quả dưới ThreadSanitizer* (rút gọn):

```text
WARNING: ThreadSanitizer: lock-order-inversion (potential deadlock)
  Cycle in lock order graph: M0 => M1 => M0
  Mutex M1 acquired here while holding mutex M0 in thread T1:
    ... lock_order.cpp:9
  ...
SUMMARY: ThreadSanitizer: lock-order-inversion (potential deadlock)
```

ThreadSanitizer ghi nhận **thứ tự khóa** và phát hiện vòng tròn — dù deadlock **chưa hề xảy ra** trong lần chạy này.

> ⚠️ **Công cụ cũng có giới hạn:** khi kiểm thử, ThreadSanitizer đi kèm GCC 13 báo sai *"unlock of an unlocked mutex"* cho một chương trình **một luồng** chỉ gọi `std::timed_mutex::try_lock_for` rồi `unlock` (nhiều khả năng do công cụ không theo dõi được lời gọi hệ thống mà thư viện dùng bên dưới). Vì vậy ví dụ ở mục 11.2 dùng `std::mutex` với `try_lock`. Khi gặp cảnh báo khó hiểu, hãy thử thu gọn thành một chương trình tối thiểu để phân biệt lỗi thật với báo động giả.

### 11.4 Cách phòng tránh

| Chiến lược | Cách làm |
|---|---|
| **Thứ tự khóa cố định** | Đánh số mọi mutex, luôn khóa theo thứ tự tăng dần (ví dụ theo ID tài khoản) |
| **Khóa nhiều mutex một lúc** | `std::scoped_lock(m1, m2)` |
| **Không gọi code lạ khi giữ khóa** | Callback, hàm ảo, hàm của thư viện khác có thể tự khóa mutex khác |
| **Timeout** | `try_lock` trong vòng lặp có hạn giờ; phát hiện và báo lỗi |
| **Giảm chia sẻ** | Mỗi luồng dữ liệu riêng, giao tiếp bằng **hàng đợi thông điệp** |

**Hai "họ hàng" của deadlock:**

- **Livelock:** các luồng không bị chặn nhưng liên tục **nhường nhau** mà không ai tiến lên được.
- **Starvation (đói):** một luồng **mãi không** được tài nguyên vì luồng khác luôn được ưu tiên.

**Bài toán kinh điển — Bữa ăn của các triết gia:** 5 triết gia ngồi quanh bàn, giữa mỗi hai người có một chiếc đũa; muốn ăn phải cầm cả hai chiếc. Nếu ai cũng cầm chiếc bên trái trước → deadlock. Cách giải đơn giản: đánh số đũa và luôn cầm chiếc **số nhỏ hơn** trước (thứ tự khóa cố định).

---

## Chương 12. Condition variable và semaphore

### 12.1 Chờ một điều kiện

Mutex bảo vệ dữ liệu, nhưng không giúp một luồng **chờ** đến khi dữ liệu thỏa điều kiện (ví dụ "hàng đợi có phần tử"). Vòng lặp kiểm tra liên tục (**busy waiting**) thì lãng phí CPU. **Condition variable** cho phép luồng **ngủ** đến khi được đánh thức.

```cpp
std::unique_lock<std::mutex> lock(m);
cv.wait(lock, [&] { return !queue.empty(); });   // 1) mở khóa + ngủ; 2) thức dậy: khóa lại + kiểm tra điều kiện
// ... đến đây: đang giữ khóa VÀ điều kiện đúng
```

**Luôn dùng dạng có điều kiện (predicate)**, vì:

1. **Đánh thức giả (spurious wakeup):** luồng có thể thức dậy dù không ai gọi `notify`.
2. **Mất tín hiệu:** nếu `notify` xảy ra **trước khi** luồng kịp `wait`, luồng sẽ ngủ mãi — trừ khi nó kiểm tra điều kiện trước.
3. **Tranh giành:** lúc luồng thức dậy, luồng khác có thể đã lấy mất phần tử.

**Điều kiện phải được thay đổi khi đang giữ cùng mutex** với lời gọi `wait`.

### 12.2 Hàng đợi producer–consumer có giới hạn

Đây là mẫu thiết kế quan trọng nhất trong lập trình đa luồng: **luồng sản xuất** đưa việc vào hàng đợi, **luồng tiêu thụ** lấy ra xử lý. Giới hạn sức chứa tạo ra **backpressure**: bên sản xuất quá nhanh sẽ tự động bị chậm lại.

```cpp
#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

// Hàng đợi an toàn luồng, có giới hạn sức chứa (mô hình producer–consumer)
template <typename T>
class BoundedQueue {
public:
    explicit BoundedQueue(std::size_t cap) : cap_(cap) {}

    // Chặn khi hàng đợi ĐẦY
    bool push(T value) {
        std::unique_lock<std::mutex> lock(m_);
        not_full_.wait(lock, [&] { return q_.size() < cap_ || closed_; });   // có điều kiện: chống "đánh thức giả"
        if (closed_) return false;
        q_.push_back(std::move(value));
        max_seen_ = std::max(max_seen_, q_.size());
        not_empty_.notify_one();
        return true;
    }

    // Chặn khi hàng đợi RỖNG; trả về nullopt khi đã đóng và hết dữ liệu
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(m_);
        not_empty_.wait(lock, [&] { return !q_.empty() || closed_; });
        if (q_.empty()) return std::nullopt;
        T v = std::move(q_.front());
        q_.pop_front();
        not_full_.notify_one();
        return v;
    }

    void close() {
        { std::lock_guard<std::mutex> lock(m_); closed_ = true; }
        not_empty_.notify_all();          // đánh thức MỌI luồng đang chờ để chúng thoát
        not_full_.notify_all();
    }

    std::size_t max_seen() const { std::lock_guard<std::mutex> lock(m_); return max_seen_; }

private:
    mutable std::mutex m_;
    std::condition_variable not_empty_, not_full_;
    std::deque<T> q_;
    std::size_t cap_, max_seen_ = 0;
    bool closed_ = false;
};

int main() {
    BoundedQueue<int> q(5);
    const int PRODUCERS = 3, CONSUMERS = 2, ITEMS = 1000;

    std::vector<std::thread> producers, consumers;
    std::vector<long> consumed_sum(CONSUMERS, 0), consumed_count(CONSUMERS, 0);

    for (int p = 0; p < PRODUCERS; ++p)
        producers.emplace_back([&q] { for (int i = 1; i <= ITEMS; ++i) q.push(i); });
    for (int c = 0; c < CONSUMERS; ++c)
        consumers.emplace_back([&, c] {
            while (auto item = q.pop()) {          // dừng khi hàng đợi đã đóng và rỗng
                consumed_sum[c] += *item;
                ++consumed_count[c];
            }
        });

    for (auto& t : producers) t.join();
    q.close();                                     // báo "không còn dữ liệu mới"
    for (auto& t : consumers) t.join();

    long total = 0, count = 0;
    for (int c = 0; c < CONSUMERS; ++c) { total += consumed_sum[c]; count += consumed_count[c]; }
    std::cout << "Da tieu thu " << count << " phan tu, tong = " << total
              << " (mong doi " << PRODUCERS * ITEMS << " va " << PRODUCERS * (ITEMS * (ITEMS + 1) / 2) << ")\n";
    std::cout << "Hang doi khong bao gio vuot suc chua: " << (q.max_seen() <= 5 ? "dung" : "sai") << '\n';
}
```

**Kết quả:**

```text
Da tieu thu 3000 phan tu, tong = 1501500 (mong doi 3000 va 1501500)
Hang doi khong bao gio vuot suc chua: dung
```

**Chi tiết quan trọng:**

- Hai condition variable riêng: `not_empty_` cho bên tiêu thụ, `not_full_` cho bên sản xuất.
- `close()` dùng `notify_all` để **mọi** luồng đang chờ đều thức dậy và thoát — thiếu bước này, các luồng tiêu thụ sẽ chờ mãi khi hết việc.
- Bên tiêu thụ chỉ dừng khi hàng đợi **đã đóng VÀ đã rỗng** — không làm mất phần tử nào.

### 12.3 Semaphore

**Semaphore đếm** giữ một số "vé". `acquire` lấy một vé (hết vé thì chờ), `release` trả vé. Dùng để **giới hạn số luồng** cùng làm một việc (số kết nối cơ sở dữ liệu, số lượt tải file song song...).

```cpp
#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <semaphore>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

std::counting_semaphore<2> slots(2);   // tối đa 2 "lượt tải" cùng lúc
std::atomic<int> active{0}, max_active{0};

void download(int id, std::vector<int>& done) {
    slots.acquire();                    // hết lượt -> chờ
    int now = ++active;
    int prev = max_active.load();
    while (now > prev && !max_active.compare_exchange_weak(prev, now)) {}   // cập nhật max bằng CAS
    std::this_thread::sleep_for(50ms);  // giả lập tải file
    --active;
    slots.release();                    // trả lượt
    done[id] = 1;
}

int main() {
    std::vector<int> done(6, 0);
    std::vector<std::jthread> ts;
    for (int i = 0; i < 6; ++i) ts.emplace_back(download, i, std::ref(done));
    ts.clear();                         // hủy các jthread -> join tất cả
    std::cout << "So luot tai xong: " << std::count(done.begin(), done.end(), 1) << "/6\n";
    std::cout << "So luot tai dong thoi toi da: " << max_active << '\n';
}
```

**Kết quả:**

```text
So luot tai xong: 6/6
So luot tai dong thoi toi da: 2
```

(Semaphore có 1 vé tương đương một mutex, nhưng khác ở chỗ **không** gắn với luồng sở hữu: luồng khác có thể `release`.)

### 12.4 Nhiều người đọc, một người ghi

Với dữ liệu **đọc nhiều, ghi ít** (bảng cấu hình, cache), `std::shared_mutex` cho phép **nhiều luồng đọc cùng lúc**:

```cpp
#include <map>
#include <shared_mutex>
#include <string>

class Config {
public:
    std::string get(const std::string& key) const {
        std::shared_lock lock(m_);                  // nhiều luồng đọc cùng lúc được
        auto it = data_.find(key);
        return it == data_.end() ? "" : it->second;
    }
    void set(const std::string& key, std::string value) {
        std::unique_lock lock(m_);                  // ghi: độc quyền
        data_[key] = std::move(value);
    }
private:
    mutable std::shared_mutex m_;
    std::map<std::string, std::string> data_;
};
```

Lưu ý: `shared_mutex` có chi phí cao hơn `mutex`; chỉ có lợi khi vùng đọc **đủ dài** và số luồng đọc **thật sự nhiều**. Hãy đo trước khi dùng.

---

## Chương 13. Thread pool, future, promise và `std::async`

### 13.1 Vì sao cần thread pool?

Tạo và hủy luồng cho **mỗi** việc nhỏ rất tốn kém; tạo **quá nhiều** luồng (nhiều hơn số lõi rất nhiều) thì chi phí chuyển ngữ cảnh lấn át lợi ích. **Thread pool** giữ một số luồng cố định, lấy việc từ **hàng đợi chung**.

### 13.2 Cài đặt

```cpp
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(std::size_t n) {
        for (std::size_t i = 0; i < n; ++i)
            workers_.emplace_back([this] { worker_loop(); });
    }
    ~ThreadPool() {
        { std::lock_guard<std::mutex> lock(m_); stopping_ = true; }
        cv_.notify_all();
        for (auto& w : workers_) w.join();       // chạy nốt các việc còn trong hàng đợi rồi mới dừng
    }
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Gửi một việc; nhận về std::future để lấy kết quả (hoặc ngoại lệ) sau
    template <typename F>
    auto submit(F f) -> std::future<std::invoke_result_t<F>> {
        using R = std::invoke_result_t<F>;
        auto task = std::make_shared<std::packaged_task<R()>>(std::move(f));
        std::future<R> fut = task->get_future();
        {
            std::lock_guard<std::mutex> lock(m_);
            if (stopping_) throw std::runtime_error("pool da dung");
            tasks_.emplace([task] { (*task)(); });
        }
        cv_.notify_one();
        return fut;
    }

private:
    void worker_loop() {
        for (;;) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(m_);
                cv_.wait(lock, [this] { return stopping_ || !tasks_.empty(); });
                if (stopping_ && tasks_.empty()) return;
                job = std::move(tasks_.front());
                tasks_.pop();
            }
            job();                                // chạy việc NGOÀI vùng khóa
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex m_;
    std::condition_variable cv_;
    bool stopping_ = false;
};

int main() {
    ThreadPool pool(4);

    // 1) Nhiều việc tính toán, gom kết quả qua future
    std::vector<std::future<long>> results;
    for (int i = 1; i <= 10; ++i)
        results.push_back(pool.submit([i] { return static_cast<long>(i) * i; }));
    long sum = 0;
    for (auto& f : results) sum += f.get();      // get() chờ đến khi việc đó xong
    std::cout << "Tong binh phuong 1..10 = " << sum << '\n';

    // 2) Ngoại lệ trong luồng được chuyển về nơi gọi get()
    auto bad = pool.submit([]() -> int { throw std::runtime_error("loi trong luong"); });
    try {
        bad.get();
    } catch (const std::exception& e) {
        std::cout << "Bat duoc ngoai le: " << e.what() << '\n';
    }

    // 3) std::async — cách nhanh nhất để chạy một việc bất đồng bộ
    auto fut = std::async(std::launch::async, [] { return std::string("ket qua tu std::async"); });
    std::cout << fut.get() << '\n';

    // 4) std::promise — một luồng "hứa" sẽ trả giá trị, luồng khác chờ
    std::promise<int> promise;
    std::future<int> answer = promise.get_future();
    std::thread producer([p = std::move(promise)]() mutable { p.set_value(42); });
    std::cout << "Gia tri tu promise: " << answer.get() << '\n';
    producer.join();
}
```

**Kết quả:**

```text
Tong binh phuong 1..10 = 385
Bat duoc ngoai le: loi trong luong
ket qua tu std::async
Gia tri tu promise: 42
```

**Các thành phần:**

| Thành phần | Vai trò |
|---|---|
| `std::future<T>` | "Phiếu hẹn" để lấy kết quả **sau này**; `get()` chờ đến khi có kết quả (hoặc ném lại ngoại lệ) |
| `std::promise<T>` | Phía "hứa": đặt giá trị bằng `set_value` (hoặc `set_exception`) |
| `std::packaged_task<R()>` | Bọc một hàm; khi được gọi, tự đặt kết quả vào `future` tương ứng |
| `std::async(std::launch::async, f)` | Chạy `f` trên luồng khác, trả về `future` — cách nhanh nhất cho việc lẻ |

> ⚠️ **Bẫy của `std::async`:** `future` trả về từ `std::async` sẽ **chặn trong hàm hủy** cho đến khi việc xong. Viết `std::async(std::launch::async, f);` mà không giữ kết quả thì dòng đó sẽ **chờ `f` chạy xong** — tức là chạy tuần tự. Ngoài ra, nếu không chỉ định `std::launch::async`, việc có thể bị **trì hoãn** đến lúc gọi `get()`.

**Luồng của pool thực hiện việc NGOÀI vùng khóa** (xem `worker_loop`) — nếu không, các luồng sẽ phải xếp hàng chờ nhau và pool mất ý nghĩa.

---

## Chương 14. Lock-free và atomic

### 14.1 Compare-and-swap (CAS)

`std::atomic<T>` cung cấp các thao tác **nguyên tử** mà CPU hỗ trợ trực tiếp. Thao tác nền tảng là **CAS**: "nếu giá trị hiện tại vẫn là `expected` thì đặt thành `desired`, trả về `true`; ngược lại cập nhật `expected` bằng giá trị hiện tại và trả về `false`". Kết hợp với vòng lặp, ta cập nhật được dữ liệu **không cần khóa**:

```cpp
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

// Cập nhật giá trị lớn nhất KHÔNG dùng khóa, bằng compare-and-swap (CAS)
void update_max(std::atomic<int>& current_max, int value) {
    int prev = current_max.load();
    // Nếu giá trị hiện tại vẫn là prev thì thay bằng value; nếu không, prev được cập nhật và thử lại
    while (value > prev && !current_max.compare_exchange_weak(prev, value)) {
    }
}

int main() {
    std::atomic<int> best{0};
    std::vector<std::thread> ts;
    for (int t = 0; t < 4; ++t)
        ts.emplace_back([&best, t] {
            for (int i = 0; i < 100000; ++i) update_max(best, (i * 7 + t * 13) % 99991);
        });
    for (auto& th : ts) th.join();
    std::cout << "Gia tri lon nhat = " << best << '\n';
    std::cout << "atomic<int> la lock-free tren may nay: " << (best.is_always_lock_free ? "co" : "khong") << '\n';
}
```

**Kết quả:**

```text
Gia tri lon nhat = 99990
atomic<int> la lock-free tren may nay: co
```

(`compare_exchange_weak` có thể thất bại "giả" dù giá trị đúng bằng `expected` — nên luôn đặt trong vòng lặp.)

### 14.2 Khi nào nên — và không nên — dùng lock-free?

- **Nên:** bộ đếm, cờ, thống kê đơn giản (`std::atomic`).
- **Cẩn thận:** cấu trúc dữ liệu lock-free (stack, queue) rất **khó viết đúng**: vấn đề **ABA** (giá trị đổi từ A sang B rồi về A, CAS tưởng không có gì thay đổi), **thu hồi bộ nhớ** an toàn (hazard pointer, epoch)... Hãy dùng thư viện đã được kiểm chứng (ví dụ các hàng đợi lock-free của Boost, moodycamel::ConcurrentQueue) thay vì tự viết.
- Lock-free **không tự động nhanh hơn** mutex. Mutex hiện đại rất nhanh khi không có tranh chấp. **Hãy đo.**

---

## Chương 15. Hiệu năng đa luồng và các mẫu thực tế

### 15.1 Tính tổng song song

```cpp
#include <chrono>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

long long parallel_sum(const std::vector<int>& v, unsigned parts) {
    std::vector<long long> partial(parts, 0);         // mỗi luồng ghi ô riêng của mình
    std::vector<std::thread> ts;
    std::size_t chunk = v.size() / parts;
    for (unsigned p = 0; p < parts; ++p) {
        auto first = v.begin() + static_cast<long>(p * chunk);
        auto last = (p == parts - 1) ? v.end() : first + static_cast<long>(chunk);
        ts.emplace_back([first, last, &partial, p] { partial[p] = std::accumulate(first, last, 0LL); });
    }
    for (auto& t : ts) t.join();
    return std::accumulate(partial.begin(), partial.end(), 0LL);
}

int main() {
    std::vector<int> data(50'000'000, 1);
    unsigned cores = std::max(1u, std::thread::hardware_concurrency());
    for (unsigned parts : {1u, 2u, 4u, 8u}) {
        auto t0 = std::chrono::steady_clock::now();
        long long s = parallel_sum(data, parts);
        double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
        std::cout << parts << " luong: tong = " << s << ", " << ms << " ms\n";
    }
    std::cout << "So loi CPU cua may: " << cores << '\n';
}
```

*Ví dụ kết quả* (trên máy thử nghiệm **1 lõi**, biên dịch `-O2`):

```text
1 luong: tong = 50000000, 39.3454 ms
2 luong: tong = 50000000, 38.3316 ms
4 luong: tong = 50000000, 39.6758 ms
8 luong: tong = 50000000, 38.9718 ms
So loi CPU cua may: 1
```

Trên **một lõi**, thêm luồng **không** làm việc tính toán nhanh hơn — chúng chỉ luân phiên nhau. Trên máy **N lõi**, bạn sẽ thấy thời gian giảm dần khi tăng số luồng đến khoảng N, rồi **không giảm thêm** (thậm chí tăng nhẹ) khi vượt quá N. Việc cộng mảng còn bị giới hạn bởi **tốc độ đọc bộ nhớ**, nên tăng tốc thực tế thường thấp hơn N. Hãy chạy thử trên máy của bạn.

### 15.2 Những yếu tố ảnh hưởng hiệu năng

| Yếu tố | Giải thích | Cách xử lý |
|---|---|---|
| **Định luật Amdahl** | Phần tuần tự giới hạn tăng tốc | Giảm phần tuần tự (khởi tạo, gộp kết quả, vùng găng) |
| **Tranh chấp khóa** | Các luồng xếp hàng chờ cùng một mutex | Vùng găng nhỏ; dữ liệu riêng từng luồng; gộp kết quả ở cuối |
| **Chia sẻ giả (false sharing)** | Biến của các luồng khác nhau nằm chung một **khối cache** → cache liên tục bị vô hiệu | Mỗi luồng dùng biến cục bộ, hoặc đệm bằng `alignas(64)` |
| **Quá nhiều luồng** | Chuyển ngữ cảnh tốn kém, cache "nguội" | Số luồng tính toán ≈ số lõi (`hardware_concurrency()`) |
| **Việc quá nhỏ** | Chi phí giao việc lớn hơn bản thân công việc | Chia việc thành **lô** đủ lớn |

### 15.3 Mẫu trong game: tải tài nguyên không làm đứng game

Vòng lặp game chạy trên **luồng chính** và không được phép chờ. Việc chậm (tải file, giải nén, sinh bản đồ) được đẩy sang luồng khác; mỗi khung hình chỉ **kiểm tra** xem đã xong chưa:

```cpp
#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>

using namespace std::chrono_literals;

// Giả lập tải tài nguyên chậm (đọc đĩa, giải nén...)
std::string load_texture(const std::string& name) {
    std::this_thread::sleep_for(300ms);
    return name + " (1024x1024)";
}

int main() {
    // Bắt đầu tải ở luồng nền
    std::future<std::string> tex = std::async(std::launch::async, load_texture, "dragon.png");

    // Vòng lặp game tiếp tục chạy, mỗi khung hình KIỂM TRA (không chờ) việc tải
    int frames = 0;
    while (tex.wait_for(0ms) != std::future_status::ready) {
        ++frames;                                  // cập nhật, vẽ màn hình chờ...
        std::this_thread::sleep_for(16ms);         // ~60 khung hình / giây
    }
    std::cout << "Tai xong: " << tex.get() << '\n';
    std::cout << "Game van chay (nhieu khung hinh) trong luc tai: " << (frames >= 10 ? "co" : "khong") << '\n';
}
```

**Kết quả:**

```text
Tai xong: dragon.png (1024x1024)
Game van chay (nhieu khung hinh) trong luc tai: co
```

`wait_for(0ms)` kiểm tra trạng thái **mà không chờ**. Engine thật thường dùng **hệ thống job (job system)**: một thread pool với số luồng ≈ số lõi, chia công việc mỗi khung hình (vật lý, hoạt họa, culling) thành nhiều job nhỏ; một số API đồ họa và giao diện yêu cầu chỉ được gọi từ **luồng chính**.

---

## Bài tập Phần II

**Bài II.1.** Viết chương trình có hai luồng in **xen kẽ** `ping` và `pong`, mỗi luồng 3 lần, dùng `std::condition_variable`. Kết quả phải luôn là `ping pong ping pong ping pong`, dù luồng nào được khởi động trước.

**Bài II.2.** Tìm lỗi:

```cpp
std::mutex m;
std::queue<int> q;
void consumer() {
    std::unique_lock<std::mutex> lock(m);
    if (q.empty()) cv.wait(lock);
    int x = q.front();
    q.pop();
}
```

**Bài II.3.** Hàm `transfer(Account& from, Account& to, int amount)` khóa `from.m` rồi `to.m`. Hai luồng gọi đồng thời `transfer(a, b, 10)` và `transfer(b, a, 20)`. Chuyện gì có thể xảy ra? Đưa ra hai cách sửa.

**Bài II.4.** Một đồng nghiệp nói: "Tôi chạy chương trình đa luồng 1000 lần đều ra kết quả đúng, vậy là không có race condition." Bạn phản hồi thế nào?

**Bài II.5.** Vì sao dòng sau có thể khiến chương trình chạy **tuần tự** dù có vẻ như chạy song song?

```cpp
for (auto& file : files) std::async(std::launch::async, process, file);
```

<details>
<summary><b>Lời giải Phần II</b> (bấm để mở)</summary>

**II.1.**

```cpp
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

// Hai luồng in xen kẽ "ping" và "pong", mỗi luồng 3 lần
std::mutex m;
std::condition_variable cv;
bool ping_turn = true;          // trạng thái dùng chung: đến lượt ai

void player(bool is_ping) {
    for (int i = 0; i < 3; ++i) {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [&] { return ping_turn == is_ping; });   // chờ đến lượt mình
        std::cout << (is_ping ? "ping" : "pong") << '\n';
        ping_turn = !is_ping;                                   // chuyển lượt
        cv.notify_all();                                        // đánh thức luồng kia
    }
}

int main() {
    std::thread b(player, false);   // khởi động "pong" trước cũng không sao
    std::thread a(player, true);
    a.join();
    b.join();
}
```

**Kết quả:**

```text
ping
pong
ping
pong
ping
pong
```

(Đã kiểm tra dưới ThreadSanitizer — không có cảnh báo — và trên bản build Windows.)

**II.2.** Dùng `if` thay vì điều kiện trong `wait`: nếu bị **đánh thức giả**, hoặc luồng tiêu thụ khác đã lấy mất phần tử, `q.front()` được gọi trên hàng đợi **rỗng** → UB. Sửa: `cv.wait(lock, [&] { return !q.empty(); });`. (Đoạn code còn thiếu khai báo `cv`.)

**II.3.** Deadlock: luồng 1 giữ `a.m` chờ `b.m`, luồng 2 giữ `b.m` chờ `a.m`. Sửa: (1) `std::scoped_lock lock(from.m, to.m);`; (2) luôn khóa tài khoản có **ID nhỏ hơn** trước. (Cẩn thận thêm trường hợp `from` và `to` là **cùng** tài khoản — khóa một mutex hai lần là UB.)

**II.4.** Chạy thử đúng **không chứng minh** không có race: lỗi phụ thuộc vào thứ tự xen kẽ, có thể chỉ xuất hiện trên máy khác, số lõi khác, tải khác (mục 10.2 cho thấy một race chạy đúng 8/8 lần trên máy 1 lõi). Cần: phân tích code (dữ liệu dùng chung nào được ghi mà không đồng bộ?), chạy **ThreadSanitizer**, và chạy thử trên máy nhiều lõi dưới tải cao.

**II.5.** `std::future` trả về bị bỏ đi ngay → hàm hủy của nó **chờ** việc hoàn tất trước khi vòng lặp chạy tiếp → các file được xử lý **lần lượt**. Sửa: lưu các `future` vào một `std::vector` rồi `get()` sau vòng lặp, hoặc dùng thread pool.

</details>

---

# PHẦN III — ASYNC I/O

## Chương 16. Các mô hình I/O

### 16.1 Hai giai đoạn của một thao tác đọc

Mọi thao tác đọc (từ socket, file...) gồm hai giai đoạn: **(1) chờ dữ liệu sẵn sàng**, và **(2) chép dữ liệu từ nhân vào bộ đệm của chương trình**. Các mô hình I/O khác nhau ở chỗ chương trình phải chờ ở giai đoạn nào.

| Mô hình | Giai đoạn 1 (chờ dữ liệu) | Giai đoạn 2 (chép dữ liệu) | Ví dụ |
|---|---|---|---|
| **Chặn (blocking)** | Luồng **đứng chờ** | Luồng đứng chờ | `recv` mặc định |
| **Không chặn (non-blocking)** | Trả về ngay "chưa có", chương trình **hỏi lại** sau | Chờ (rất ngắn) | `O_NONBLOCK`, `EAGAIN` |
| **Ghép kênh (multiplexing)** | **Một** lời gọi chờ **nhiều** nguồn | Chờ | `select`, `poll`, **`epoll`**, `kqueue` |
| **Hướng tín hiệu** | HĐH gửi tín hiệu khi sẵn sàng | Chờ | `SIGIO` (hiếm dùng) |
| **Bất đồng bộ thật (completion)** | Không chờ | **Không chờ**: HĐH báo khi **đã chép xong** | **IOCP** (Windows), **io_uring** (Linux) |

Ba mô hình giữa được gọi là **mô hình sẵn sàng (readiness)**: HĐH báo "giờ đọc được rồi", chương trình tự gọi `recv`. Mô hình cuối là **mô hình hoàn tất (completion)**: chương trình đưa bộ đệm cho HĐH, HĐH báo "đã đọc xong vào bộ đệm của bạn".

### 16.2 Vì sao cần async I/O?

Một server phục vụ **10 000 kết nối** (bài toán **C10K**) theo kiểu "mỗi kết nối một luồng" cần 10 000 luồng — mỗi luồng có stack riêng (thường vài trăm KB đến vài MB bộ nhớ ảo), cùng chi phí chuyển ngữ cảnh khổng lồ, trong khi **hầu hết thời gian các luồng chỉ ngồi chờ**. Với async I/O, **một** luồng (hoặc vài luồng) có thể quản lý hàng chục nghìn kết nối, vì chỉ xử lý kết nối nào **đang có việc**.

---

## Chương 17. `epoll` — event loop trên Linux

### 17.1 API

| Lời gọi | Ý nghĩa |
|---|---|
| `epoll_create1(0)` | Tạo một đối tượng epoll trong nhân |
| `epoll_ctl(ep, EPOLL_CTL_ADD / MOD / DEL, fd, &event)` | Đăng ký / sửa / hủy theo dõi một fd — **chỉ một lần**, không phải mỗi vòng lặp như `poll` |
| `epoll_wait(ep, events, max, timeout_ms)` | Chờ, trả về **chỉ** những fd có sự kiện |

Nhờ danh sách theo dõi nằm sẵn trong nhân, chi phí mỗi lần `epoll_wait` chỉ phụ thuộc vào số fd **có sự kiện**, không phụ thuộc tổng số fd đang theo dõi.

### 17.2 Kích hoạt theo mức và theo sườn

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

/* Đếm số lần epoll_wait báo "đọc được" khi dữ liệu đến nhưng ta chỉ đọc MỘT PHẦN */
static void run(const char *name, unsigned extra_flag) {
    int sv[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);          /* hai socket nối sẵn với nhau */
    int ep = epoll_create1(0);
    struct epoll_event ev = {.events = EPOLLIN | extra_flag, .data.fd = sv[0]};
    epoll_ctl(ep, EPOLL_CTL_ADD, sv[0], &ev);

    write(sv[1], "0123456789", 10);                  /* 10 byte đến */
    printf("%-22s:", name);
    for (int round = 1; round <= 4; round++) {
        struct epoll_event out;
        int n = epoll_wait(ep, &out, 1, 0);          /* timeout 0: chỉ kiểm tra, không chờ */
        if (n == 1) {
            char c;
            read(sv[0], &c, 1);                      /* chỉ đọc 1 byte mỗi lần được báo */
            printf(" [lan %d: bao, doc '%c']", round, c);
        } else {
            printf(" [lan %d: im lang]", round);
        }
    }
    printf("\n");
    close(ep);
    close(sv[0]);
    close(sv[1]);
}

int main(void) {
    run("Theo muc (mac dinh)", 0);
    run("Theo suon (EPOLLET)", EPOLLET);
    return 0;
}
```

**Kết quả:**

```text
Theo muc (mac dinh)   : [lan 1: bao, doc '0'] [lan 2: bao, doc '1'] [lan 3: bao, doc '2'] [lan 4: bao, doc '3']
Theo suon (EPOLLET)   : [lan 1: bao, doc '0'] [lan 2: im lang] [lan 3: im lang] [lan 4: im lang]
```

| Chế độ | Hành vi | Hệ quả khi lập trình |
|---|---|---|
| **Theo mức** (mặc định) | Còn dữ liệu là **còn được báo** | An toàn, giống `poll`; đọc một phần cũng không sao |
| **Theo sườn** (`EPOLLET`) | Chỉ báo **khi có dữ liệu MỚI đến** | **Bắt buộc** dùng socket không chặn và đọc **cho đến khi gặp `EAGAIN`**; nếu không, dữ liệu còn lại bị "bỏ quên" |

Theo sườn giảm số lần được báo (hữu ích khi nhiều luồng cùng chờ một epoll), nhưng dễ sai hơn. **Người mới nên dùng theo mức.**

### 17.3 Echo server một luồng dùng epoll

```c
/* file: epoll_echo.c — echo server MỘT LUỒNG dùng epoll (chỉ Linux)
   Cách dùng: ./epoll_echo [cong] */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_EVENTS 64

static void set_nonblocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

int main(int argc, char *argv[]) {
    int port = argc > 1 ? atoi(argv[1]) : 9000;

    int lst = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(lst, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((unsigned short)port);
    if (bind(lst, (struct sockaddr *)&addr, sizeof addr) != 0 || listen(lst, SOMAXCONN) != 0) {
        perror("bind/listen");
        return 1;
    }
    set_nonblocking(lst);

    /* 1) Tạo đối tượng epoll trong nhân, đăng ký socket lắng nghe MỘT LẦN */
    int ep = epoll_create1(0);
    struct epoll_event ev = {.events = EPOLLIN, .data.fd = lst};
    epoll_ctl(ep, EPOLL_CTL_ADD, lst, &ev);
    printf("epoll echo server o cong %d\n", port);
    fflush(stdout);

    long served = 0;
    struct epoll_event events[MAX_EVENTS];
    for (;;) {
        /* 2) Chờ: nhân chỉ trả về những socket THỰC SỰ có sự kiện */
        int n = epoll_wait(ep, events, MAX_EVENTS, -1);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }
        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;
            if (fd == lst) {
                /* 3) Có thể có NHIỀU kết nối đang chờ -> accept đến khi hết (EAGAIN) */
                for (;;) {
                    int c = accept(lst, NULL, NULL);
                    if (c < 0) break;
                    set_nonblocking(c);
                    struct epoll_event cev = {.events = EPOLLIN, .data.fd = c};
                    epoll_ctl(ep, EPOLL_CTL_ADD, c, &cev);
                }
                continue;
            }
            /* 4) Client có dữ liệu (hoặc đã đóng) */
            char buf[4096];
            ssize_t r = recv(fd, buf, sizeof buf, 0);
            if (r > 0) {
                /* Để ví dụ ngắn gọn: coi như gửi được hết một lần. Server thật phải
                   giữ phần chưa gửi và đăng ký EPOLLOUT (như mục 10.4 tài liệu socket). */
                send(fd, buf, (size_t)r, MSG_NOSIGNAL);
            } else if (r == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
                epoll_ctl(ep, EPOLL_CTL_DEL, fd, NULL);   /* hủy đăng ký rồi đóng */
                close(fd);
                if (++served % 100 == 0) {
                    printf("da phuc vu xong %ld client\n", served);
                    fflush(stdout);
                }
            }
        }
    }
    close(ep);
    close(lst);
    return 0;
}
```

**Kiểm thử:** mở **500 kết nối cùng lúc**, gửi dữ liệu trên tất cả rồi mới đọc phản hồi:

```python
# file: many_clients.py — mở N kết nối CÙNG LÚC tới echo server, gửi và kiểm tra phản hồi
import socket, sys

port, n = int(sys.argv[1]), int(sys.argv[2])
socks = [socket.create_connection(("127.0.0.1", port)) for _ in range(n)]   # tất cả cùng mở
for i, s in enumerate(socks):
    s.sendall(f"client {i}\n".encode())
ok = 0
for i, s in enumerate(socks):
    s.settimeout(2)
    data = b""
    while not data.endswith(b"\n"):
        data += s.recv(100)
    ok += data.decode() == f"client {i}\n"
for s in socks:
    s.close()
print(f"{ok}/{n} client nhan dung phan hoi, voi {n} ket noi mo dong thoi")
```

**Kết quả** (chạy script hai lần khi server đang chạy ở cổng 9000):

```text
500/500 client nhan dung phan hoi, voi 500 ket noi mo dong thoi
500/500 client nhan dung phan hoi, voi 500 ket noi mo dong thoi
```

**Log của server:**

```text
epoll echo server o cong 9000
da phuc vu xong 100 client
da phuc vu xong 200 client
...
da phuc vu xong 1000 client
```

Một luồng, không khóa, không tạo luồng mới — vẫn phục vụ đồng thời 500 kết nối. Số kết nối tối đa bị giới hạn bởi **số fd được phép mở** (`ulimit -n`).

### 17.4 Các API tương đương

| Nền tảng | API | Ghi chú |
|---|---|---|
| Linux | `epoll`, `io_uring` | `io_uring` (kernel 5.1+) theo mô hình hoàn tất, dùng hàng đợi vòng chia sẻ giữa chương trình và nhân để giảm số lời gọi hệ thống |
| macOS, BSD | `kqueue` | Mô hình sẵn sàng, theo dõi được cả timer, tín hiệu, tiến trình |
| Windows | **IOCP**; `WSAPoll` | IOCP theo mô hình hoàn tất |

Viết code trực tiếp cho cả ba là rất tốn công — đó là lý do có các thư viện như **libuv**, **libevent** và **Asio** (Chương 19).

---

## Chương 18. Lập trình bất đồng bộ: callback, future và async/await

### 18.1 Ba phong cách

Dùng trực tiếp event loop buộc code phải chia thành nhiều mảnh nhỏ, mỗi mảnh xử lý một sự kiện. Các phong cách lập trình bất đồng bộ giúp code dễ viết hơn:

| Phong cách | Ví dụ (giả mã) | Nhược điểm |
|---|---|---|
| **Callback** | `read(sock, [](data) { write(sock, data, [](){ ... }); });` | Lồng nhau sâu ("callback hell"), khó xử lý lỗi |
| **Future / promise** | `read(sock).then(write).then(...)` | Dễ đọc hơn, nhưng vòng lặp và rẽ nhánh vẫn khó viết |
| **async/await (coroutine)** | `data = await read(sock); await write(sock, data);` | Trông như code tuần tự, **dễ đọc nhất** |

**Coroutine** là hàm có thể **tạm dừng** ở điểm `await` và **tiếp tục** sau đó, trong khi luồng được giải phóng để làm việc khác. Nhiều coroutine chạy **xen kẽ trên cùng một luồng** — đồng thời mà không cần song song.

```text
Event loop (1 luồng):
  coroutine A: ──chạy──► await đọc mạng  ·········  (dữ liệu đến) ──chạy──► await ...
  coroutine B:               ──chạy──► await timer ··· (hết giờ) ──chạy──► xong
  coroutine C:                              ──chạy──► await đọc file ··············
                ▲ tại mỗi thời điểm chỉ MỘT coroutine chạy; "···" là đang chờ, không tốn CPU
```

### 18.2 asyncio (Python) — minh họa rõ nhất

```python
import asyncio
import time

async def download(name, seconds):
    print(f"  bat dau {name}")
    await asyncio.sleep(seconds)      # "chờ I/O": nhường event loop cho việc khác
    print(f"  xong {name}")
    return f"{name}: {seconds}s"

async def bad_download(name, seconds):
    time.sleep(seconds)               # SAI: hàm chặn -> cả event loop đứng yên
    return f"{name}: {seconds}s"

async def main():
    t0 = time.perf_counter()
    results = await asyncio.gather(download("A", 0.3), download("B", 0.2), download("C", 0.1))
    print("Ket qua:", results)
    print(f"Tong thoi gian (dung await): {time.perf_counter() - t0:.1f}s  <- xap xi viec LAU NHAT")

    t0 = time.perf_counter()
    await asyncio.gather(bad_download("A", 0.3), bad_download("B", 0.2), bad_download("C", 0.1))
    print(f"Tong thoi gian (dung time.sleep): {time.perf_counter() - t0:.1f}s  <- bang TONG cac viec")

    t0 = time.perf_counter()
    await asyncio.gather(*(asyncio.to_thread(time.sleep, s) for s in (0.3, 0.2, 0.1)))
    print(f"Tong thoi gian (chuyen viec chan sang luong): {time.perf_counter() - t0:.1f}s")

asyncio.run(main())
```

**Kết quả:**

```text
  bat dau A
  bat dau B
  bat dau C
  xong C
  xong B
  xong A
Ket qua: ['A: 0.3s', 'B: 0.2s', 'C: 0.1s']
Tong thoi gian (dung await): 0.3s  <- xap xi viec LAU NHAT
Tong thoi gian (dung time.sleep): 0.6s  <- bang TONG cac viec
Tong thoi gian (chuyen viec chan sang luong): 0.3s
```

**Ba bài học:**

1. Với `await asyncio.sleep`, ba việc **chờ cùng lúc** → tổng thời gian ≈ việc **lâu nhất** (0,3 s), không phải tổng (0,6 s).
2. Gọi hàm **chặn** (`time.sleep`, đọc file lớn, tính toán nặng, thư viện mạng kiểu cũ) bên trong coroutine sẽ **làm đứng cả event loop** — mọi coroutine khác phải chờ. Đây là lỗi phổ biến nhất khi lập trình async.
3. Khi buộc phải gọi hàm chặn, hãy **đẩy nó sang luồng khác** (`asyncio.to_thread`, `run_in_executor`).

### 18.3 Echo server bằng asyncio

```python
# file: asyncio_echo.py — echo server bằng asyncio: MỘT luồng, mỗi client một coroutine
import asyncio
import sys

async def handle(reader, writer):
    while data := await reader.read(4096):   # await: nhường event loop trong lúc chờ dữ liệu
        writer.write(data)
        await writer.drain()                  # chờ nếu bộ đệm gửi đầy (tự động "backpressure")
    writer.close()
    await writer.wait_closed()

async def main():
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 9000
    server = await asyncio.start_server(handle, "0.0.0.0", port)
    print(f"asyncio echo server o cong {port}", flush=True)
    async with server:
        await server.serve_forever()

asyncio.run(main())
```

Chạy `python3 asyncio_echo.py 9000` rồi dùng lại script `many_clients.py`:

**Kết quả:**

```text
500/500 client nhan dung phan hoi, voi 500 ket noi mo dong thoi
```

So với phiên bản epoll ở Chương 17: cùng mô hình (một luồng, event loop), nhưng code ngắn và đọc như code tuần tự. `writer.drain()` còn tự xử lý trường hợp bộ đệm gửi đầy — việc mà server epoll đơn giản ở trên đã bỏ qua.

---

## Chương 19. Coroutine C++20 với Asio

### 19.1 Chuẩn bị

**Asio** là thư viện mạng và I/O bất đồng bộ phổ biến nhất của C++. Nó bọc `epoll` / `kqueue` / IOCP dưới một API chung, chạy trên mọi nền tảng. Bản **độc lập** (không cần Boost) chỉ gồm các header:

```bash
git clone --depth 1 https://github.com/chriskohlhoff/asio.git
# thư mục header: asio/asio/include
g++ -std=c++20 -I asio/asio/include file.cpp -o file -pthread
```

(Có thể cài qua trình quản lý gói như vcpkg, Conan, hoặc `apt install libasio-dev`. Các ví dụ dưới đây đã được kiểm thử với Asio 1.38.2. Trên Windows với MinGW, cần liên kết thêm `-lws2_32 -lmswsock` và nên khai báo phiên bản Windows mục tiêu, ví dụ `-D_WIN32_WINNT=0x0A00` cho Windows 10, để tránh cảnh báo của Asio.)

### 19.2 Nhiều tác vụ xen kẽ trên một luồng

```cpp
// Ba "tác vụ" chạy xen kẽ trên MỘT luồng nhờ coroutine và timer bất đồng bộ
#include <asio.hpp>

#include <chrono>
#include <iostream>
#include <string>

using namespace std::chrono_literals;

asio::awaitable<void> task(std::string name, std::chrono::milliseconds step, int repeats) {
    asio::steady_timer timer(co_await asio::this_coro::executor);
    for (int i = 1; i <= repeats; ++i) {
        timer.expires_after(step);
        co_await timer.async_wait(asio::use_awaitable);    // chờ KHÔNG chặn luồng
        std::cout << "  " << name << " buoc " << i << '\n';
    }
}

int main() {
    asio::io_context io;
    asio::co_spawn(io, task("A (moi 30 ms)", 30ms, 3), asio::detached);
    asio::co_spawn(io, task("B (moi 50 ms)", 50ms, 2), asio::detached);
    auto t0 = std::chrono::steady_clock::now();
    io.run();                                              // trả về khi mọi tác vụ xong
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count();
    std::cout << "Tong thoi gian gan bang viec dai nhat (~100 ms): " << (ms >= 95 && ms < 150 ? "dung" : "sai") << '\n';
}
```

**Kết quả:**

```text
  A (moi 30 ms) buoc 1
  B (moi 50 ms) buoc 1
  A (moi 30 ms) buoc 2
  A (moi 30 ms) buoc 3
  B (moi 50 ms) buoc 2
Tong thoi gian gan bang viec dai nhat (~100 ms): dung
```

A thức dậy ở 30, 60, 90 ms; B ở 50, 100 ms — thứ tự in khớp với dòng thời gian, và tổng thời gian khoảng 100 ms (việc dài nhất), dù chỉ có **một luồng**.

| Khái niệm Asio | Ý nghĩa |
|---|---|
| `asio::io_context` | Event loop; `run()` xử lý sự kiện cho đến khi hết việc (hoặc bị `stop()`) |
| `asio::awaitable<T>` | Kiểu trả về của một coroutine Asio |
| `co_await ... use_awaitable` | Chờ một thao tác bất đồng bộ mà không chặn luồng |
| `asio::co_spawn(ctx, coroutine, asio::detached)` | Khởi chạy một coroutine, không chờ kết quả |

### 19.3 Echo server bằng coroutine

```cpp
// file: asio_echo.cpp — echo server với Asio + coroutine C++20: code trông tuần tự nhưng không chặn
// Biên dịch: g++ -std=c++20 -I<thu_muc_asio>/include asio_echo.cpp -o asio_echo -pthread
// Cách dùng: ./asio_echo [cong]
#include <asio.hpp>

#include <cstdlib>
#include <iostream>

using asio::ip::tcp;
using asio::awaitable;
using asio::use_awaitable;

// Một coroutine cho mỗi client. Mỗi "co_await" là một điểm tạm dừng:
// coroutine nhường luồng cho việc khác và được tiếp tục khi I/O hoàn tất.
awaitable<void> echo(tcp::socket socket) {
    try {
        char data[4096];
        for (;;) {
            std::size_t n = co_await socket.async_read_some(asio::buffer(data), use_awaitable);
            co_await asio::async_write(socket, asio::buffer(data, n), use_awaitable);
        }
    } catch (const std::exception&) {
        // client đóng kết nối (asio::error::eof) hoặc lỗi mạng -> kết thúc coroutine
    }
}

awaitable<void> listener(unsigned short port) {
    auto executor = co_await asio::this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), port});
    for (;;) {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
        asio::co_spawn(executor, echo(std::move(socket)), asio::detached);   // "khởi chạy và không chờ"
    }
}

int main(int argc, char* argv[]) {
    unsigned short port = argc > 1 ? static_cast<unsigned short>(std::atoi(argv[1])) : 9000;
    try {
        asio::io_context io;                                   // event loop
        asio::signal_set signals(io, SIGINT, SIGTERM);         // Ctrl+C -> dừng êm
        signals.async_wait([&](auto, auto) { io.stop(); });
        asio::co_spawn(io, listener(port), asio::detached);
        std::cout << "Asio echo server o cong " << port << std::endl;
        io.run();                                              // chạy event loop trên luồng này
        std::cout << "Da dung server" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Loi: " << e.what() << '\n';
        return 1;
    }
}
```

**Kiểm thử** với `many_clients.py`, rồi nhấn Ctrl+C (gửi `SIGINT`):

**Kết quả của script:**

```text
500/500 client nhan dung phan hoi, voi 500 ket noi mo dong thoi
```

**Output của server:**

```text
Asio echo server o cong 9000
Da dung server
```

Mỗi client là một coroutine viết như code **tuần tự** — đọc rồi ghi trong vòng lặp — nhưng không có luồng nào bị chặn. Lỗi mạng và việc client đóng kết nối được báo bằng **ngoại lệ**, và coroutine tự kết thúc. `signal_set` giúp tắt server êm trên cả Linux lẫn Windows.

**Mở rộng ra nhiều lõi:** gọi `io.run()` trên **nhiều luồng** cùng lúc. Khi đó, các handler có thể chạy song song, nên dữ liệu dùng chung phải được bảo vệ — Asio cung cấp **strand** để đảm bảo các handler của cùng một đối tượng không chạy đồng thời.

---

## Chương 20. Chọn mô hình nào?

| Tình huống | Lựa chọn phù hợp |
|---|---|
| Tính toán nặng (xử lý ảnh, vật lý, AI) | **Nhiều luồng** (thread pool / job system), số luồng ≈ số lõi |
| Rất nhiều kết nối mạng, mỗi kết nối ít việc | **Async I/O** (epoll/IOCP qua thư viện; asyncio; Asio) |
| Vài chục kết nối, logic phức tạp | **Mỗi kết nối một luồng** vẫn đơn giản và đủ tốt |
| Cần cô lập lỗi, chạy code không tin cậy | **Nhiều tiến trình** |
| Giao diện, game | **Luồng chính** cho vòng lặp/giao diện + việc nặng đẩy sang luồng khác |
| Server hiệu năng cao | **Kết hợp**: vài luồng, mỗi luồng một event loop; việc tính toán nặng chuyển sang thread pool riêng |

**Quy tắc vàng:**

1. **Đừng chặn event loop.** Việc tính toán nặng hoặc lời gọi chặn phải được chuyển sang thread pool.
2. **Chia sẻ càng ít càng tốt.** Truyền dữ liệu bằng thông điệp/hàng đợi thay vì dùng chung biến.
3. **Đo trước khi tối ưu.** Mỗi mô hình có chi phí riêng; hãy dùng profiler.
4. **Kiểm thử bằng công cụ** (ThreadSanitizer, kiểm thử tải), không chỉ bằng vài lần chạy thử.

---

## Bài tập Phần III

**Bài III.1.** Dùng asyncio, "tải" 5 trang (mỗi trang mất 0,1 s) nhưng **tối đa 2 trang cùng lúc**. Tổng thời gian khoảng bao nhiêu?

**Bài III.2.** Một server epoll dùng chế độ **theo sườn**, nhưng mỗi lần được báo chỉ gọi `recv` **một lần** với bộ đệm 1 KB. Khi client gửi một thông điệp 10 KB, server chỉ nhận được phần đầu rồi "treo". Giải thích.

**Bài III.3.** Trong một web server viết bằng asyncio, một endpoint tính toán mất 2 giây CPU. Khi có người gọi endpoint đó, mọi request khác cũng bị chậm 2 giây. Vì sao, và sửa thế nào?

**Bài III.4.** So sánh chi phí phục vụ 10 000 kết nối nhàn rỗi (chỉ thỉnh thoảng gửi dữ liệu) bằng mô hình "mỗi kết nối một luồng" và mô hình event loop.

<details>
<summary><b>Lời giải Phần III</b> (bấm để mở)</summary>

**III.1.** Dùng `asyncio.Semaphore(2)`:

```python
import asyncio
import time

async def download(i, sem):
    async with sem:                     # tối đa 2 tác vụ vào đây cùng lúc
        await asyncio.sleep(0.1)
        return i

async def main():
    sem = asyncio.Semaphore(2)
    t0 = time.perf_counter()
    results = await asyncio.gather(*(download(i, sem) for i in range(5)))
    print("Ket qua:", results)
    print(f"Thoi gian: {time.perf_counter() - t0:.1f}s (5 viec x 0.1s, toi da 2 cung luc -> 3 dot)")

asyncio.run(main())
```

**Kết quả:**

```text
Ket qua: [0, 1, 2, 3, 4]
Thoi gian: 0.3s (5 viec x 0.1s, toi da 2 cung luc -> 3 dot)
```

5 việc, mỗi đợt tối đa 2 → 3 đợt × 0,1 s ≈ **0,3 s**.

**III.2.** Ở chế độ theo sườn, epoll chỉ báo **khi có dữ liệu mới đến**. Sau lần đọc 1 KB đầu tiên, 9 KB còn lại vẫn nằm trong bộ đệm nhưng **không có sự kiện mới** → server không bao giờ được báo lại (xem thí nghiệm ở mục 17.2). Sửa: dùng socket không chặn và đọc **lặp** cho đến khi `recv` trả về `EAGAIN` — hoặc chuyển về chế độ theo mức.

**III.3.** Tính toán CPU chạy **trên luồng của event loop** → event loop không thể xử lý gì khác trong 2 giây. Sửa: chuyển phần tính toán sang luồng hoặc tiến trình khác (`await asyncio.to_thread(...)` hoặc `loop.run_in_executor(ProcessPoolExecutor(), ...)` — với tính toán nặng thuần Python, dùng **tiến trình** để tránh giới hạn GIL của CPython).

**III.4.** **Mỗi kết nối một luồng:** 10 000 luồng, mỗi luồng cần stack riêng (bộ nhớ ảo cỡ vài MB mỗi luồng theo mặc định trên Linux) và một mục trong bộ lập lịch; nhiều luồng thức dậy/ngủ gây chuyển ngữ cảnh liên tục. **Event loop:** một luồng; mỗi kết nối chỉ tốn một fd và một cấu trúc trạng thái nhỏ (vài KB bộ đệm); chỉ những kết nối có dữ liệu mới được xử lý.

</details>

---

# PHỤ LỤC

## Phụ lục A. Bảng đối chiếu API

| Việc | POSIX (Linux/macOS) | Windows | C++ chuẩn / thư viện |
|---|---|---|---|
| Tạo tiến trình | `fork` + `exec`, `posix_spawn` | `CreateProcess` | — (Boost.Process) |
| Chờ tiến trình | `waitpid` | `WaitForSingleObject` + `GetExitCodeProcess` | — |
| Pipe | `pipe`, `mkfifo` | `CreatePipe`, named pipe | — |
| Bộ nhớ dùng chung | `mmap`, `shm_open` | `CreateFileMapping` + `MapViewOfFile` | — |
| Tín hiệu / Ctrl+C | `sigaction` | `SetConsoleCtrlHandler` | `asio::signal_set` |
| Mở / đọc / ghi file | `open`, `read`, `write` | `CreateFile`, `ReadFile`, `WriteFile` | `std::fstream`, `std::filesystem` |
| Tạo luồng | `pthread_create` | `CreateThread`, `_beginthreadex` | `std::thread`, `std::jthread` |
| Mutex | `pthread_mutex_t` | `CRITICAL_SECTION`, `SRWLOCK` | `std::mutex` |
| Condition variable | `pthread_cond_t` | `CONDITION_VARIABLE` | `std::condition_variable` |
| Semaphore | `sem_t` | `CreateSemaphore` | `std::counting_semaphore` |
| Thao tác nguyên tử | `stdatomic.h` (C11) | `Interlocked...` | `std::atomic` |
| Chờ nhiều nguồn I/O | `poll`, `epoll`, `kqueue` | `WSAPoll`, IOCP | Asio, libuv |

## Phụ lục B. Lỗi hay gặp

**Hệ điều hành:**

1. Quên `wait` → tiến trình zombie tích tụ.
2. Quên `fflush` trước `fork` → output bị nhân đôi.
3. Quên đóng đầu pipe thừa → bên đọc không bao giờ nhận EOF.
4. Gọi `printf`, `malloc` trong trình xử lý tín hiệu.
5. Nghĩ rằng `write` thành công nghĩa là dữ liệu đã nằm trên đĩa (cần `fsync`).

**Đa luồng:**

6. Truy cập dữ liệu dùng chung không đồng bộ (data race) — và tin rằng "chạy thử đúng là đúng".
7. `lock()`/`unlock()` thủ công thay vì RAII.
8. Khóa nhiều mutex theo thứ tự khác nhau → deadlock.
9. `cv.wait` không có điều kiện; thay đổi điều kiện mà không giữ mutex.
10. Quên `join` → `std::terminate`; lambda bắt tham chiếu tới biến đã bị hủy.
11. Tạo quá nhiều luồng; vùng găng quá lớn; làm I/O khi đang giữ khóa.
12. Bỏ qua `std::future` trả về từ `std::async`.

**Async I/O:**

13. Gọi hàm chặn hoặc tính toán nặng trong event loop.
14. Dùng epoll theo sườn mà không đọc đến `EAGAIN`.
15. Quên xử lý trường hợp `send` chỉ gửi được một phần trong server không chặn.
16. Quên `await` (trong Python, coroutine sẽ không chạy và có cảnh báo *"coroutine was never awaited"*).

## Phụ lục C. Công cụ

| Mục đích | Công cụ |
|---|---|
| Xem tiến trình, luồng, CPU, bộ nhớ | `ps`, `top`, `htop` (phím `H` để xem luồng); Windows: Task Manager, Process Explorer |
| Xem lời gọi hệ thống | `strace` (Linux), `dtruss` (macOS), Process Monitor (Windows) |
| Thông tin tiến trình | `/proc/<pid>/` (status, maps, fd, stat) trên Linux |
| Gỡ lỗi đa luồng | `gdb`: `info threads`, `thread <n>`, `thread apply all bt` (rất hữu ích khi chương trình **bị treo**) |
| Bắt data race, deadlock tiềm ẩn | **ThreadSanitizer** (`-fsanitize=thread`); Valgrind Helgrind/DRD |
| Đo hiệu năng | `perf`, Intel VTune, Visual Studio Profiler, Tracy (cho game) |
| Kiểm thử tải server | `wrk`, `ab`, script tự viết (như `many_clients.py`) |

## Phụ lục D. Lộ trình gợi ý (khoảng 8 tuần)

| Tuần | Nội dung | Mục tiêu tối thiểu |
|---|---|---|
| 1 | Chương 1–3 | Viết được mini shell: đọc lệnh, `fork`, `exec`, `wait` |
| 2 | Chương 4–5 | Tính tay lập lịch và thay trang; chạy và sửa hai bộ mô phỏng |
| 3 | Chương 6–7 | Nối hai lệnh bằng pipe; xử lý Ctrl+C để thoát êm |
| 4 | Chương 8–11 | Viết lại các ví dụ race/deadlock, chạy dưới ThreadSanitizer |
| 5 | Chương 12–13 | Tự cài đặt hàng đợi an toàn luồng và thread pool **không nhìn tài liệu** |
| 6 | Chương 14–15 | Song song hóa một bài toán thật (ví dụ xử lý ảnh), đo trên máy nhiều lõi |
| 7 | Chương 16–18 | Server epoll hoàn chỉnh (có bộ đệm gửi); chat server bằng asyncio |
| 8 | Chương 19–20 | Chat server bằng Asio + coroutine; so sánh với các phiên bản trước |

## Phụ lục E. Tài liệu nên dùng song song

- **"Operating Systems: Three Easy Pieces" (OSTEP)** — Remzi & Andrea Arpaci-Dusseau, **miễn phí** trên mạng; giáo trình HĐH dễ đọc nhất, chia đúng ba mảng: ảo hóa, đồng thời, lưu trữ bền vững.
- **"Operating System Concepts"** (Silberschatz, Galvin, Gagne) — giáo trình kinh điển ở nhiều trường đại học.
- **"Modern Operating Systems"** (Andrew Tanenbaum).
- **"The Linux Programming Interface"** (Michael Kerrisk) — tham khảo đầy đủ về tiến trình, tín hiệu, IPC, epoll trên Linux.
- **"C++ Concurrency in Action"** (Anthony Williams) — sách chuẩn về đa luồng trong C++.
- **"The Art of Multiprocessor Programming"** (Herlihy, Shavit) — khi muốn đi sâu vào lock-free.
- **Tài liệu Asio** (think-async.com) và **tài liệu asyncio** (docs.python.org).
- **xv6** (MIT, miễn phí) — một HĐH nhỏ gọn để đọc mã nguồn và làm bài tập nhân.

---

*Chúc bạn học tốt! Hai lời khuyên quan trọng nhất: với hệ điều hành — **dùng `strace` và `/proc` để nhìn tận mắt**; với đa luồng — **đừng tin kết quả chạy thử, hãy tin ThreadSanitizer**.*
