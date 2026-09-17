# GIẢI TÍCH — NỀN TẢNG VỮNG CHẮC

*Tài liệu tự học bám theo mạch **Giải tích 1 → 2 → 3** như ở Đại học Bách khoa Hà Nội, kèm phần vận dụng cho lập trình game và AI*

---

## 0. Cách dùng tài liệu này

### 0.1 Cấu trúc

| Phần | Tương ứng | Chương | Nội dung |
|---|---|---|---|
| **I** | Giải tích 1 | 1–8 | Giới hạn, liên tục, đạo hàm, Taylor, khảo sát hàm, tích phân, **hàm nhiều biến** |
| **II** | Giải tích 2 | 9–12 | Ứng dụng hình học, **tích phân bội**, tích phân đường, tích phân mặt, lý thuyết trường |
| **III** | Giải tích 3 | 13–16 | **Chuỗi số**, chuỗi lũy thừa và Fourier, **phương trình vi phân**, biến đổi Laplace |
| **IV** | Vận dụng | 17–18 | Phương pháp số; giải tích trong game và AI (có code chạy được) |
| **Phụ lục** | | A–E | Bảng công thức, lỗi hay gặp, kiểm tra bài bằng máy, lộ trình |

Mỗi chương đi theo mạch: **ý tưởng trực quan → định nghĩa, công thức → ví dụ giải chi tiết → dạng bài hay gặp**. Cuối mỗi phần có **bài tập kèm đáp án**.

> **Lưu ý:** Đây là tài liệu tự học, không thay thế bài giảng và đề cương chính thức của Khoa Toán – Tin. Nội dung các học phần có thể khác nhau đôi chút giữa các nhóm ngành (ví dụ MI1111, MI1112, MI1113), nên hãy đối chiếu với đề cương lớp bạn.

> ✅ **Về độ chính xác:** mọi giới hạn, đạo hàm, nguyên hàm, tích phân, tổng chuỗi, nghiệm phương trình vi phân và biến đổi Laplace trong tài liệu đều đã được kiểm tra lại bằng thư viện **SymPy**. Các chương trình Python và C++ ở Phần IV đã được chạy thật, và phần "Kết quả" là output thật.

### 0.2 Ký hiệu

| Ký hiệu | Ý nghĩa | Ghi chú |
|---|---|---|
| $\tan x$, $\cot x$ | Tang, cotang | Nhiều sách Việt Nam viết $\operatorname{tg} x$, $\operatorname{cotg} x$ |
| $\arctan x$ | Hàm ngược của $\tan$ | Hay viết $\operatorname{arctg} x$ |
| $\ln x$ | Logarit cơ số $e$ | |
| $f'(x)$, $\dfrac{df}{dx}$ | Đạo hàm | |
| $f'_x$, $\dfrac{\partial f}{\partial x}$ | Đạo hàm riêng theo $x$ | |
| $\alpha(x) \sim \beta(x)$ | Hai vô cùng bé **tương đương** | |
| $o(x^n)$ | Vô cùng bé bậc cao hơn $x^n$ | |
| $\nabla f$ | Gradient của $f$ | |

### 0.3 Cách học hiệu quả

1. **Hiểu ý nghĩa trước, công thức sau.** Đạo hàm là **tốc độ thay đổi**, tích phân là **tổng tích lũy**. Nếu chỉ nhớ công thức, bạn sẽ quên rất nhanh.
2. **Che lời giải, tự làm lại.** Giải tích học bằng tay.
3. **Kiểm tra bằng máy sau khi làm** (Phụ lục C), không phải trước.
4. **Nối với lập trình:** mỗi khi học một khái niệm, hãy hỏi "trong game/AI nó dùng để làm gì?". Phần IV trả lời câu hỏi đó.

---

# PHẦN I — GIẢI TÍCH 1

## Chương 1. Giới hạn

### 1.1 Ý tưởng

$\lim\limits_{x \to a} f(x) = L$ nghĩa là: khi $x$ tiến **càng gần** $a$ (nhưng không cần bằng $a$), $f(x)$ tiến **càng gần** $L$.

**Định nghĩa chính xác (ε – δ):**

$$\lim_{x \to a} f(x) = L \iff \forall \varepsilon > 0,\ \exists \delta > 0:\ 0 < |x - a| < \delta \Rightarrow |f(x) - L| < \varepsilon$$

Đọc là: "muốn $f(x)$ gần $L$ bao nhiêu cũng được (sai số $\varepsilon$), chỉ cần chọn $x$ đủ gần $a$ (trong khoảng $\delta$)".

- **Giới hạn một phía:** $\lim\limits_{x \to a^-}$ (từ bên trái), $\lim\limits_{x \to a^+}$ (từ bên phải). Giới hạn tồn tại **khi và chỉ khi** hai giới hạn một phía tồn tại và **bằng nhau**.
- **Giới hạn của dãy:** $\lim\limits_{n \to \infty} u_n = L$ — cùng ý tưởng với $n$ là số tự nhiên.

### 1.2 Các giới hạn cơ bản cần thuộc

$$\lim_{x \to 0} \frac{\sin x}{x} = 1 \qquad \lim_{x \to \infty} \left(1 + \frac{1}{x}\right)^x = e \qquad \lim_{x \to 0} (1 + x)^{1/x} = e$$

$$\lim_{x \to 0} \frac{\ln(1 + x)}{x} = 1 \qquad \lim_{x \to 0} \frac{e^x - 1}{x} = 1 \qquad \lim_{x \to 0} \frac{(1 + x)^\alpha - 1}{x} = \alpha$$

**So sánh tốc độ tiến ra vô cùng** (khi $x \to +\infty$): 

$$\ln x \ll x^\alpha \ll a^x \quad (\alpha > 0,\ a > 1)$$

Ví dụ: $\lim\limits_{x \to +\infty} \dfrac{x^2}{e^x} = 0$.

### 1.3 Khử các dạng vô định

Các dạng **vô định**: $\dfrac{0}{0}$, $\dfrac{\infty}{\infty}$, $0 \cdot \infty$, $\infty - \infty$, $1^\infty$, $0^0$, $\infty^0$.

**Ví dụ 1.1.** $\lim\limits_{x \to 2} \dfrac{x^2 - 4}{x - 2} = \lim\limits_{x \to 2} (x + 2) = 4$ (phân tích nhân tử để khử dạng $\frac{0}{0}$).

**Ví dụ 1.2.** $\lim\limits_{x \to \infty} \dfrac{3x^2 + x}{2x^2 - 5} = \lim\limits_{x \to \infty} \dfrac{3 + 1/x}{2 - 5/x^2} = \dfrac{3}{2}$ (chia cả tử và mẫu cho lũy thừa cao nhất).

**Dạng $1^\infty$** — công thức rất hay dùng: nếu $u(x) \to 0$ và $v(x) \to \infty$ thì

$$\boxed{\lim (1 + u)^v = e^{\lim u \cdot v}}$$

**Ví dụ 1.3.** $\lim\limits_{x \to \infty} \left(1 + \dfrac{2}{x}\right)^{3x} = e^{\lim \frac{2}{x} \cdot 3x} = e^6$.

### 1.4 Vô cùng bé tương đương — công cụ mạnh nhất để tính giới hạn

$\alpha(x)$ là **vô cùng bé (VCB)** khi $x \to a$ nếu $\alpha(x) \to 0$. Hai VCB **tương đương** ($\alpha \sim \beta$) nếu $\lim \dfrac{\alpha}{\beta} = 1$.

**Bảng VCB tương đương khi $x \to 0$** (thuộc lòng):

| | |
|---|---|
| $\sin x \sim x$ | $\tan x \sim x$ |
| $\arcsin x \sim x$ | $\arctan x \sim x$ |
| $1 - \cos x \sim \dfrac{x^2}{2}$ | $\ln(1 + x) \sim x$ |
| $e^x - 1 \sim x$ | $a^x - 1 \sim x \ln a$ |
| $(1 + x)^\alpha - 1 \sim \alpha x$ | $\sqrt{1 + x} - 1 \sim \dfrac{x}{2}$ |

Bảng vẫn đúng khi thay $x$ bằng bất kỳ biểu thức $u(x) \to 0$: ví dụ $\sin(3x) \sim 3x$, $\ln(1 + x^2) \sim x^2$.

**Quy tắc thay thế:** trong **tích** và **thương**, được thay một VCB bằng VCB tương đương.

**Ví dụ 1.4.** $\lim\limits_{x \to 0} \dfrac{\ln(1 + 3x)}{\sin 2x} = \lim\limits_{x \to 0} \dfrac{3x}{2x} = \dfrac{3}{2}$.

**Ví dụ 1.5.** $\lim\limits_{x \to 0} \dfrac{e^{2x} - 1}{\tan x} = \lim\limits_{x \to 0} \dfrac{2x}{x} = 2$.

> ⚠️ **Lỗi kinh điển: KHÔNG được thay VCB tương đương trong tổng/hiệu.**
>
> $\lim\limits_{x \to 0} \dfrac{\tan x - \sin x}{x^3}$: nếu thay $\tan x \sim x$, $\sin x \sim x$, tử số thành $x - x = 0$ và kết quả "bằng 0" — **SAI**.
>
> Cách đúng: $\tan x - \sin x = \tan x (1 - \cos x) \sim x \cdot \dfrac{x^2}{2}$ (đã chuyển thành **tích**), nên giới hạn bằng $\dfrac{1}{2}$.

---

## Chương 2. Hàm số liên tục

### 2.1 Định nghĩa

$f$ **liên tục tại** $x_0$ nếu $\lim\limits_{x \to x_0} f(x) = f(x_0)$. Ba điều kiện ngầm: $f(x_0)$ xác định, giới hạn tồn tại, và hai giá trị bằng nhau.

Hình ảnh trực quan: vẽ đồ thị **không nhấc bút** khi đi qua $x_0$.

**Ví dụ 2.1.** Tìm $a$ để hàm sau liên tục tại 0:

$$f(x) = \begin{cases} \dfrac{\sin x}{x} & x \ne 0 \\ a & x = 0 \end{cases}$$

*Lời giải.* $\lim\limits_{x \to 0} \dfrac{\sin x}{x} = 1$, nên cần $a = 1$.

### 2.2 Phân loại điểm gián đoạn

| Loại | Đặc điểm | Ví dụ |
|---|---|---|
| **Loại 1 — bỏ được** | Hai giới hạn một phía hữu hạn và **bằng nhau**, nhưng khác $f(x_0)$ hoặc $f(x_0)$ không xác định | $\dfrac{x^2 - 1}{x - 1}$ tại $x = 1$ |
| **Loại 1 — bước nhảy** | Hai giới hạn một phía hữu hạn nhưng **khác nhau** | $\dfrac{\lvert x\rvert}{x}$ tại $x = 0$ (trái $-1$, phải $1$) |
| **Loại 2** | Ít nhất một giới hạn một phía **vô hạn hoặc không tồn tại** | $\dfrac{1}{x}$ tại 0; $\sin\dfrac{1}{x}$ tại 0 |

### 2.3 Các định lý về hàm liên tục trên đoạn $[a, b]$

1. **Định lý Weierstrass:** $f$ đạt **giá trị lớn nhất và nhỏ nhất** trên đoạn.
2. **Định lý giá trị trung gian (Bolzano–Cauchy):** nếu $f(a)$ và $f(b)$ **trái dấu** thì phương trình $f(x) = 0$ có nghiệm trong $(a, b)$.

> 💻 Định lý 2 là cơ sở của **phương pháp chia đôi (bisection)** — thuật toán tìm nghiệm đơn giản và chắc chắn hội tụ: chia đôi đoạn, giữ nửa có hai đầu trái dấu, lặp lại.

---

## Chương 3. Đạo hàm và vi phân

### 3.1 Định nghĩa và ý nghĩa

$$f'(x_0) = \lim_{h \to 0} \frac{f(x_0 + h) - f(x_0)}{h}$$

| Góc nhìn | Ý nghĩa của $f'(x_0)$ |
|---|---|
| **Hình học** | Hệ số góc của **tiếp tuyến** tại $x_0$ |
| **Vật lý** | **Vận tốc tức thời** (nếu $f$ là vị trí theo thời gian) |
| **Tổng quát** | **Tốc độ thay đổi** của $f$ khi $x$ thay đổi |

**Phương trình tiếp tuyến** tại $x_0$: $y = f(x_0) + f'(x_0)(x - x_0)$.

**Ví dụ 3.1.** Đạo hàm của $f(x) = x^2$ bằng định nghĩa:

$$f'(x) = \lim_{h \to 0} \frac{(x + h)^2 - x^2}{h} = \lim_{h \to 0} \frac{2xh + h^2}{h} = \lim_{h \to 0} (2x + h) = 2x$$

**Liên tục không kéo theo khả vi.** Hàm $f(x) = |x|$ liên tục tại 0 nhưng đạo hàm trái bằng $-1$, đạo hàm phải bằng $1$ → **không có đạo hàm** tại 0 (đồ thị có "góc nhọn"). Chiều ngược lại thì đúng: **khả vi ⇒ liên tục**.

### 3.2 Bảng đạo hàm cơ bản

| $f(x)$ | $f'(x)$ | $f(x)$ | $f'(x)$ |
|---|---|---|---|
| $x^\alpha$ | $\alpha x^{\alpha - 1}$ | $\sin x$ | $\cos x$ |
| $e^x$ | $e^x$ | $\cos x$ | $-\sin x$ |
| $a^x$ | $a^x \ln a$ | $\tan x$ | $\dfrac{1}{\cos^2 x}$ |
| $\ln x$ | $\dfrac{1}{x}$ | $\cot x$ | $-\dfrac{1}{\sin^2 x}$ |
| $\log_a x$ | $\dfrac{1}{x \ln a}$ | $\arcsin x$ | $\dfrac{1}{\sqrt{1 - x^2}}$ |
| $\sqrt{x}$ | $\dfrac{1}{2\sqrt{x}}$ | $\arctan x$ | $\dfrac{1}{1 + x^2}$ |

### 3.3 Quy tắc tính đạo hàm

$$(u \pm v)' = u' \pm v' \qquad (uv)' = u'v + uv' \qquad \left(\frac{u}{v}\right)' = \frac{u'v - uv'}{v^2}$$

**Quy tắc dây chuyền (đạo hàm hàm hợp)** — quan trọng nhất, và là nền tảng của **lan truyền ngược (backpropagation)** trong AI:

$$\boxed{\big(f(g(x))\big)' = f'(g(x)) \cdot g'(x)}$$

**Ví dụ 3.2.**

- $(x^2 \sin x)' = 2x \sin x + x^2 \cos x$ (quy tắc tích)
- $\left(e^{x^2}\right)' = e^{x^2} \cdot 2x$ (dây chuyền)
- $\big(\ln(\cos x)\big)' = \dfrac{1}{\cos x} \cdot (-\sin x) = -\tan x$ (dây chuyền)

**Đạo hàm hàm mũ–lũy thừa** $y = u^v$: lấy logarit hai vế rồi đạo hàm.

**Ví dụ 3.3.** $y = x^x$ ($x > 0$). Ta có $\ln y = x \ln x$. Đạo hàm hai vế theo $x$:

$$\frac{y'}{y} = \ln x + 1 \Rightarrow y' = x^x(\ln x + 1)$$

### 3.4 Đạo hàm hàm ẩn và hàm cho theo tham số

**Hàm ẩn:** đạo hàm hai vế phương trình theo $x$, coi $y$ là hàm của $x$.

**Ví dụ 3.4.** $x^2 + y^2 = 25$. Đạo hàm: $2x + 2y \cdot y' = 0 \Rightarrow y' = -\dfrac{x}{y}$. Tại điểm $(3, 4)$: $y' = -\dfrac{3}{4}$.

**Hàm tham số** $x = x(t)$, $y = y(t)$:

$$\frac{dy}{dx} = \frac{y'(t)}{x'(t)}$$

**Ví dụ 3.5.** $x = t^2$, $y = t^3$ → $\dfrac{dy}{dx} = \dfrac{3t^2}{2t} = \dfrac{3t}{2}$.

### 3.5 Đạo hàm cấp cao

Một số công thức đạo hàm cấp $n$ hay dùng:

$$(e^{ax})^{(n)} = a^n e^{ax} \qquad (\sin x)^{(n)} = \sin\left(x + \frac{n\pi}{2}\right) \qquad \left(\frac{1}{1 + x}\right)^{(n)} = \frac{(-1)^n\, n!}{(1 + x)^{n + 1}}$$

**Công thức Leibniz** cho đạo hàm cấp cao của tích: $(uv)^{(n)} = \sum\limits_{k=0}^{n} \binom{n}{k} u^{(k)} v^{(n-k)}$.

### 3.6 Vi phân và xấp xỉ tuyến tính

**Vi phân:** $df = f'(x)\, dx$. Khi $\Delta x$ nhỏ:

$$\boxed{f(x_0 + \Delta x) \approx f(x_0) + f'(x_0)\, \Delta x}$$

Tức là **gần một điểm, hàm số trông giống tiếp tuyến của nó**.

**Ví dụ 3.6.** Tính gần đúng $\sqrt{4{,}02}$. Chọn $f(x) = \sqrt{x}$, $x_0 = 4$, $\Delta x = 0{,}02$:

$$\sqrt{4{,}02} \approx 2 + \frac{1}{2\sqrt{4}} \cdot 0{,}02 = 2{,}005$$

Giá trị thật: $2{,}004993\ldots$ — sai số chỉ khoảng $6 \times 10^{-6}$.

---

## Chương 4. Các định lý giá trị trung bình, L'Hôpital và Taylor

### 4.1 Định lý Rolle và Lagrange

**Rolle:** $f$ liên tục trên $[a, b]$, khả vi trên $(a, b)$, $f(a) = f(b)$ ⇒ có $c \in (a, b)$ với $f'(c) = 0$.

**Lagrange (giá trị trung bình):** $f$ liên tục trên $[a, b]$, khả vi trên $(a, b)$ ⇒ có $c \in (a, b)$ với

$$f'(c) = \frac{f(b) - f(a)}{b - a}$$

**Ý nghĩa:** nếu bạn đi 120 km trong 2 giờ (trung bình 60 km/h), thì **chắc chắn có lúc** đồng hồ tốc độ chỉ **đúng 60 km/h**.

Hệ quả hay dùng: nếu $f'(x) = 0$ trên cả một khoảng thì $f$ là **hằng số** trên khoảng đó.

### 4.2 Quy tắc L'Hôpital

Với dạng $\dfrac{0}{0}$ hoặc $\dfrac{\infty}{\infty}$, nếu giới hạn vế phải tồn tại:

$$\lim \frac{f(x)}{g(x)} = \lim \frac{f'(x)}{g'(x)}$$

(Đạo hàm **riêng** tử và **riêng** mẫu — không phải đạo hàm thương.)

**Ví dụ 4.1.**

$$\lim_{x \to 0} \frac{x - \sin x}{x^3} \overset{L}{=} \lim_{x \to 0} \frac{1 - \cos x}{3x^2} = \lim_{x \to 0} \frac{x^2/2}{3x^2} = \frac{1}{6}$$

(bước thứ hai dùng VCB tương đương cho gọn).

**Ví dụ 4.2 (dạng $0 \cdot \infty$).** Chuyển về dạng phân số trước:

$$\lim_{x \to 0^+} x \ln x = \lim_{x \to 0^+} \frac{\ln x}{1/x} \overset{L}{=} \lim_{x \to 0^+} \frac{1/x}{-1/x^2} = \lim_{x \to 0^+} (-x) = 0$$

### 4.3 Công thức Taylor

**Ý tưởng:** xấp xỉ một hàm phức tạp gần $x_0$ bằng một **đa thức** có cùng giá trị và cùng các đạo hàm đến cấp $n$ tại $x_0$.

$$f(x) = \sum_{k=0}^{n} \frac{f^{(k)}(x_0)}{k!}(x - x_0)^k + R_n(x)$$

- **Phần dư dạng Peano:** $R_n(x) = o\big((x - x_0)^n\big)$ — dùng để **tính giới hạn**.
- **Phần dư dạng Lagrange:** $R_n(x) = \dfrac{f^{(n+1)}(c)}{(n+1)!}(x - x_0)^{n+1}$ với $c$ nằm giữa $x_0$ và $x$ — dùng để **đánh giá sai số**.

Khi $x_0 = 0$, gọi là **công thức Maclaurin**.

**Bảng khai triển Maclaurin (thuộc lòng):**

$$e^x = 1 + x + \frac{x^2}{2!} + \frac{x^3}{3!} + \dots + \frac{x^n}{n!} + o(x^n)$$

$$\sin x = x - \frac{x^3}{3!} + \frac{x^5}{5!} - \dots \qquad \cos x = 1 - \frac{x^2}{2!} + \frac{x^4}{4!} - \dots$$

$$\ln(1 + x) = x - \frac{x^2}{2} + \frac{x^3}{3} - \dots \qquad \frac{1}{1 - x} = 1 + x + x^2 + x^3 + \dots$$

$$\arctan x = x - \frac{x^3}{3} + \frac{x^5}{5} - \dots \qquad (1 + x)^\alpha = 1 + \alpha x + \frac{\alpha(\alpha - 1)}{2!}x^2 + \dots$$

**Ví dụ 4.3 (tính giới hạn bằng Taylor).**

$$\lim_{x \to 0} \frac{e^x - 1 - x}{x^2} = \lim_{x \to 0} \frac{\left(1 + x + \frac{x^2}{2} + o(x^2)\right) - 1 - x}{x^2} = \frac{1}{2}$$

Và Ví dụ 4.1 làm lại bằng Taylor: $x - \sin x = x - \left(x - \frac{x^3}{6} + o(x^3)\right) = \frac{x^3}{6} + o(x^3)$, nên giới hạn bằng $\frac{1}{6}$.

> 💡 **Taylor giải quyết được cả bài "thay VCB trong tổng"** ở Chương 1: $\tan x - \sin x = \left(x + \frac{x^3}{3}\right) - \left(x - \frac{x^3}{6}\right) + o(x^3) = \frac{x^3}{2} + o(x^3)$.

**Ví dụ 4.4 (đánh giá sai số).** Xấp xỉ $\sin(0{,}1) \approx 0{,}1 - \dfrac{0{,}1^3}{6} = 0{,}0998333\ldots$

Số hạng kế tiếp trong khai triển là $\dfrac{x^5}{5!}$, nên sai số không vượt quá $\dfrac{0{,}1^5}{120} \approx 8{,}3 \times 10^{-8}$. Giá trị thật $0{,}0998334166\ldots$ — sai số thực tế đúng cỡ $8{,}3 \times 10^{-8}$.

**Thử nghiệm:** xấp xỉ $e$ bằng đa thức Taylor của $e^x$ tại $x = 1$:

```python
import math

# Xấp xỉ e = e^1 bằng đa thức Taylor bậc n: 1 + 1 + 1/2! + ... + 1/n!
for n in [1, 2, 4, 6, 8, 10]:
    approx = sum(1 / math.factorial(k) for k in range(n + 1))
    print(f"bac {n:2d}: {approx:.10f}   sai so = {abs(math.e - approx):.2e}")
```

**Kết quả:**

```text
bac  1: 2.0000000000   sai so = 7.18e-01
bac  2: 2.5000000000   sai so = 2.18e-01
bac  4: 2.7083333333   sai so = 9.95e-03
bac  6: 2.7180555556   sai so = 2.26e-04
bac  8: 2.7182787698   sai so = 3.06e-06
bac 10: 2.7182818011   sai so = 2.73e-08
```

Sai số giảm **cực nhanh** khi tăng bậc — đó là lý do máy tính và trình biên dịch dùng khai triển Taylor để tính $\sin$, $\cos$, $e^x$.

---

## Chương 5. Khảo sát hàm số và bài toán tối ưu

### 5.1 Các công cụ

| Mục tiêu | Công cụ |
|---|---|
| **Đơn điệu** | $f' > 0$: tăng; $f' < 0$: giảm |
| **Điểm tới hạn** | $f'(x) = 0$ hoặc $f'$ không tồn tại |
| **Cực trị — dấu hiệu 1** | $f'$ đổi dấu từ $+$ sang $-$: **cực đại**; từ $-$ sang $+$: **cực tiểu** |
| **Cực trị — dấu hiệu 2** | $f'(x_0) = 0$ và $f''(x_0) < 0$: cực đại; $f''(x_0) > 0$: cực tiểu |
| **Lồi / lõm** | $f'' > 0$: **lồi** (cong lên, hình chữ U); $f'' < 0$: **lõm** (cong xuống) |
| **Điểm uốn** | $f''$ đổi dấu |
| **Tiệm cận đứng** | $\lim\limits_{x \to x_0} f(x) = \pm\infty$ |
| **Tiệm cận ngang** | $\lim\limits_{x \to \pm\infty} f(x) = b$ |
| **Tiệm cận xiên** $y = ax + b$ | $a = \lim\limits_{x \to \infty} \dfrac{f(x)}{x}$, $b = \lim\limits_{x \to \infty} \big(f(x) - ax\big)$ |

> Quy ước "lồi/lõm" khác nhau giữa các sách. Tài liệu này dùng: **lồi** khi $f'' > 0$ — cũng là quy ước của tối ưu hóa và AI ("hàm lồi" có một cực tiểu toàn cục duy nhất nếu lồi chặt). Hãy kiểm tra quy ước trong giáo trình của bạn.

**Ví dụ 5.1.** Khảo sát $f(x) = x^3 - 3x^2 + 2$.

- $f'(x) = 3x^2 - 6x = 3x(x - 2) = 0 \iff x = 0$ hoặc $x = 2$.
- $f''(x) = 6x - 6$. Tại $x = 0$: $f'' = -6 < 0$ → **cực đại** $f(0) = 2$. Tại $x = 2$: $f'' = 6 > 0$ → **cực tiểu** $f(2) = -2$.
- $f'' = 0 \iff x = 1$, và $f''$ đổi dấu → **điểm uốn** $(1, 0)$.

| $x$ | $-\infty$ | | $0$ | | $1$ | | $2$ | | $+\infty$ |
|---|---|---|---|---|---|---|---|---|---|
| $f'$ | | $+$ | $0$ | $-$ | | $-$ | $0$ | $+$ | |
| $f$ | $-\infty$ | ↗ | **2** | ↘ | 0 | ↘ | **−2** | ↗ | $+\infty$ |

**Ví dụ 5.2 (tiệm cận).** $g(x) = \dfrac{x^2 + 1}{x - 1}$.

- Tiệm cận đứng: $x = 1$.
- Chia đa thức: $g(x) = x + 1 + \dfrac{2}{x - 1}$. Khi $x \to \infty$, phần $\frac{2}{x-1} \to 0$ → **tiệm cận xiên** $y = x + 1$.

### 5.2 Bài toán tối ưu

**Quy trình:**

1. Đặt biến, viết **hàm mục tiêu** cần cực đại/cực tiểu.
2. Xác định **miền giá trị** của biến.
3. Tìm điểm tới hạn trong miền; so sánh giá trị tại điểm tới hạn **và tại hai đầu mút**.

**Ví dụ 5.3.** Từ tấm bìa vuông cạnh 12 cm, cắt 4 hình vuông cạnh $x$ ở 4 góc rồi gấp thành hộp không nắp. Tìm $x$ để thể tích lớn nhất.

*Lời giải.* $V(x) = x(12 - 2x)^2$ với $0 < x < 6$.

$V'(x) = (12 - 2x)^2 - 4x(12 - 2x) = (12 - 2x)(12 - 6x) = 0 \iff x = 2$ (loại $x = 6$ vì ngoài miền).

$V$ tăng trên $(0, 2)$, giảm trên $(2, 6)$ → thể tích lớn nhất $V(2) = 2 \cdot 8^2 = 128 \text{ cm}^3$.

---

## Chương 6. Tích phân bất định (nguyên hàm)

### 6.1 Định nghĩa và bảng nguyên hàm

$F$ là **nguyên hàm** của $f$ nếu $F'(x) = f(x)$. Tập mọi nguyên hàm: $\displaystyle\int f(x)\,dx = F(x) + C$.

**Mẹo kiểm tra:** luôn có thể tự kiểm tra kết quả bằng cách **đạo hàm lại**.

| $\displaystyle\int f(x)\,dx$ | Kết quả | $\displaystyle\int f(x)\,dx$ | Kết quả |
|---|---|---|---|
| $\displaystyle\int x^\alpha dx$ ($\alpha \ne -1$) | $\dfrac{x^{\alpha+1}}{\alpha + 1}$ | $\displaystyle\int \sin x\,dx$ | $-\cos x$ |
| $\displaystyle\int \frac{dx}{x}$ | $\ln\lvert x\rvert$ | $\displaystyle\int \cos x\,dx$ | $\sin x$ |
| $\displaystyle\int e^x dx$ | $e^x$ | $\displaystyle\int \frac{dx}{\cos^2 x}$ | $\tan x$ |
| $\displaystyle\int a^x dx$ | $\dfrac{a^x}{\ln a}$ | $\displaystyle\int \frac{dx}{\sin^2 x}$ | $-\cot x$ |
| $\displaystyle\int \frac{dx}{1 + x^2}$ | $\arctan x$ | $\displaystyle\int \frac{dx}{\sqrt{1 - x^2}}$ | $\arcsin x$ |
| $\displaystyle\int \frac{dx}{x^2 + a^2}$ | $\dfrac{1}{a}\arctan\dfrac{x}{a}$ | $\displaystyle\int \frac{dx}{x^2 - a^2}$ | $\dfrac{1}{2a}\ln\left\lvert\dfrac{x - a}{x + a}\right\rvert$ |

(Mọi kết quả đều cộng thêm hằng số $C$.)

### 6.2 Phương pháp đổi biến

Nếu biểu thức có dạng $f(u(x)) \cdot u'(x)$, đặt $u = u(x)$, $du = u'(x)\,dx$.

**Ví dụ 6.1.** $\displaystyle\int \frac{2x}{1 + x^2}dx$. Đặt $u = 1 + x^2$, $du = 2x\,dx$:

$$\int \frac{du}{u} = \ln|u| + C = \ln(1 + x^2) + C$$

**Mẹo nhận dạng:** thấy một biểu thức **và đạo hàm của nó** (sai khác hằng số) cùng xuất hiện → đổi biến.

### 6.3 Phương pháp tích phân từng phần

$$\boxed{\int u\,dv = uv - \int v\,du}$$

**Chọn $u$ theo thứ tự ưu tiên "LIATE":** **L**ogarit → **I**nverse (hàm lượng giác ngược) → **A**lgebraic (đa thức) → **T**rigonometric (lượng giác) → **E**xponential (mũ). Hàm đứng trước được chọn làm $u$.

**Ví dụ 6.2.** $\displaystyle\int x e^x dx$: đặt $u = x$, $dv = e^x dx$ → $du = dx$, $v = e^x$:

$$\int x e^x dx = x e^x - \int e^x dx = (x - 1)e^x + C$$

**Ví dụ 6.3.** $\displaystyle\int x \ln x\,dx$: đặt $u = \ln x$, $dv = x\,dx$ → $du = \dfrac{dx}{x}$, $v = \dfrac{x^2}{2}$:

$$\int x \ln x\,dx = \frac{x^2}{2}\ln x - \int \frac{x}{2}dx = \frac{x^2}{2}\ln x - \frac{x^2}{4} + C$$

**Ví dụ 6.4 (tích phân "quay vòng").** $I = \displaystyle\int e^x \sin x\,dx$. Từng phần hai lần:

$$I = e^x \sin x - \int e^x \cos x\,dx = e^x \sin x - \left(e^x \cos x + \int e^x \sin x\,dx\right) = e^x(\sin x - \cos x) - I$$

Chuyển vế: $2I = e^x(\sin x - \cos x)$, nên $I = \dfrac{e^x(\sin x - \cos x)}{2} + C$.

### 6.4 Tích phân hàm hữu tỉ

**Quy trình:** (1) nếu bậc tử ≥ bậc mẫu, chia đa thức; (2) phân tích mẫu thành nhân tử; (3) tách thành **phân thức đơn giản**; (4) tích phân từng phần tử.

**Ví dụ 6.5.** $\displaystyle\int \frac{dx}{x^2 - 3x + 2}$. Ta có $x^2 - 3x + 2 = (x - 1)(x - 2)$ và

$$\frac{1}{(x - 1)(x - 2)} = \frac{A}{x - 2} + \frac{B}{x - 1}$$

Quy đồng: $1 = A(x - 1) + B(x - 2)$. Cho $x = 2$: $A = 1$. Cho $x = 1$: $B = -1$. Vậy

$$\int \frac{dx}{x^2 - 3x + 2} = \ln|x - 2| - \ln|x - 1| + C = \ln\left|\frac{x - 2}{x - 1}\right| + C$$

**Mẫu bậc hai vô nghiệm → đưa về dạng $\arctan$** bằng cách thêm bớt để có bình phương:

**Ví dụ 6.6.** $\displaystyle\int \frac{dx}{x^2 + 4x + 5} = \int \frac{dx}{(x + 2)^2 + 1} = \arctan(x + 2) + C$.

### 6.5 Tích phân hàm lượng giác

- **Hạ bậc:** $\sin^2 x = \dfrac{1 - \cos 2x}{2}$, $\cos^2 x = \dfrac{1 + \cos 2x}{2}$.

  **Ví dụ 6.7.** $\displaystyle\int \sin^2 x\,dx = \frac{x}{2} - \frac{\sin 2x}{4} + C$.

- **Lũy thừa lẻ** của $\sin$ hoặc $\cos$: tách một thừa số rồi đổi biến. Ví dụ $\int \sin^3 x\,dx = \int (1 - \cos^2 x)\sin x\,dx$, đặt $u = \cos x$.
- **Hàm hữu tỉ của $\sin x$, $\cos x$:** đặt $t = \tan\dfrac{x}{2}$, khi đó $\sin x = \dfrac{2t}{1 + t^2}$, $\cos x = \dfrac{1 - t^2}{1 + t^2}$, $dx = \dfrac{2\,dt}{1 + t^2}$ — luôn đưa được về hàm hữu tỉ của $t$ (dù có thể dài).

> ⚠️ Không phải hàm nào cũng có nguyên hàm biểu diễn được bằng hàm sơ cấp. Ví dụ $\int e^{-x^2}dx$ hay $\int \frac{\sin x}{x}dx$. Khi đó dùng chuỗi hoặc **phương pháp số** (Chương 17).

---

## Chương 7. Tích phân xác định và ứng dụng

### 7.1 Ý tưởng: tổng tích lũy

Chia $[a, b]$ thành $n$ đoạn nhỏ, lấy tổng diện tích các hình chữ nhật mỏng (**tổng Riemann**), rồi cho số đoạn tiến ra vô cùng:

$$\int_a^b f(x)\,dx = \lim_{n \to \infty} \sum_{i=1}^{n} f(\xi_i)\,\Delta x_i$$

Ý nghĩa: **diện tích có dấu** dưới đồ thị; **tổng tích lũy** của một đại lượng thay đổi liên tục (quãng đường = tích phân của vận tốc).

### 7.2 Công thức Newton–Leibniz

$$\boxed{\int_a^b f(x)\,dx = F(b) - F(a)} \qquad \text{với } F' = f$$

Đây là **định lý cơ bản của giải tích**: tích phân và đạo hàm là hai phép toán **ngược nhau**. Dạng thứ hai của định lý: $\dfrac{d}{dx}\displaystyle\int_a^x f(t)\,dt = f(x)$.

**Ví dụ 7.1.** $\displaystyle\int_0^\pi \sin x\,dx = [-\cos x]_0^\pi = 1 + 1 = 2$.

**Ví dụ 7.2.** $\displaystyle\int_0^1 x e^x dx = [(x - 1)e^x]_0^1 = 0 - (-1) = 1$.

**Lưu ý khi đổi biến trong tích phân xác định:** **đổi cả cận**, và không cần quay về biến cũ.

**Tính chất đối xứng hữu ích:** trên đoạn $[-a, a]$, tích phân của hàm **lẻ** bằng 0; của hàm **chẵn** bằng $2\int_0^a$.

### 7.3 Ứng dụng hình học

| Đại lượng | Công thức |
|---|---|
| **Diện tích** giữa hai đường | $S = \displaystyle\int_a^b \lvert f(x) - g(x)\rvert\,dx$ |
| **Độ dài cung** $y = f(x)$ | $L = \displaystyle\int_a^b \sqrt{1 + f'(x)^2}\,dx$ |
| **Thể tích tròn xoay** quanh trục $Ox$ | $V = \pi\displaystyle\int_a^b f(x)^2\,dx$ |
| **Diện tích mặt tròn xoay** quanh $Ox$ | $S = 2\pi\displaystyle\int_a^b \lvert f(x)\rvert\sqrt{1 + f'(x)^2}\,dx$ |

**Ví dụ 7.3.** Diện tích giữa $y = x$ và $y = x^2$. Hai đường cắt nhau tại $x = 0$ và $x = 1$; trên $[0, 1]$ thì $x \ge x^2$:

$$S = \int_0^1 (x - x^2)\,dx = \frac{1}{2} - \frac{1}{3} = \frac{1}{6}$$

**Ví dụ 7.4.** Độ dài cung $y = x^{3/2}$ với $0 \le x \le 4$. Ta có $y' = \frac{3}{2}x^{1/2}$:

$$L = \int_0^4 \sqrt{1 + \frac{9x}{4}}\,dx = \frac{8}{27}\left(1 + \frac{9x}{4}\right)^{3/2}\Big|_0^4 = \frac{8}{27}\left(10\sqrt{10} - 1\right) \approx 9{,}073$$

**Ví dụ 7.5.** Thể tích khi quay $y = \sqrt{x}$, $0 \le x \le 4$ quanh $Ox$: $V = \pi\displaystyle\int_0^4 x\,dx = 8\pi$.

### 7.4 Tích phân suy rộng

**Loại 1 — cận vô hạn:** $\displaystyle\int_a^{+\infty} f(x)\,dx = \lim_{b \to +\infty}\int_a^b f(x)\,dx$.

**Loại 2 — hàm không bị chặn** (ví dụ tại $x = a$): $\displaystyle\int_a^b f(x)\,dx = \lim_{\varepsilon \to 0^+}\int_{a + \varepsilon}^b f(x)\,dx$.

Tích phân **hội tụ** nếu giới hạn hữu hạn, ngược lại **phân kỳ**.

**Ví dụ 7.6.**

- $\displaystyle\int_1^{+\infty} \frac{dx}{x^2} = \lim_{b \to \infty}\left[-\frac{1}{x}\right]_1^b = 1$ → hội tụ.
- $\displaystyle\int_1^{+\infty} \frac{dx}{x} = \lim_{b \to \infty} \ln b = +\infty$ → phân kỳ.
- $\displaystyle\int_0^1 \frac{dx}{\sqrt{x}} = \lim_{\varepsilon \to 0^+}\left[2\sqrt{x}\right]_\varepsilon^1 = 2$ → hội tụ, dù hàm không bị chặn.

**Tích phân mẫu (thuộc lòng):**

$$\int_1^{+\infty} \frac{dx}{x^p} \text{ hội tụ} \iff p > 1 \qquad\qquad \int_0^1 \frac{dx}{x^p} \text{ hội tụ} \iff p < 1$$

**Tiêu chuẩn so sánh:** nếu $0 \le f(x) \le g(x)$ thì $\int g$ hội tụ ⇒ $\int f$ hội tụ; $\int f$ phân kỳ ⇒ $\int g$ phân kỳ. Dạng giới hạn: nếu $f(x) \sim g(x)$ (cả hai dương) thì hai tích phân **cùng** hội tụ hoặc **cùng** phân kỳ.

**Ví dụ 7.7.** $\displaystyle\int_1^{+\infty} \frac{dx}{x^2 + \sin^2 x}$ hội tụ, vì $0 < \dfrac{1}{x^2 + \sin^2 x} \le \dfrac{1}{x^2}$.

**Một tích phân nổi tiếng** (sẽ chứng minh ở Chương 10): $\displaystyle\int_0^{+\infty} e^{-x^2}dx = \frac{\sqrt{\pi}}{2}$ — nền tảng của phân phối chuẩn trong xác suất thống kê và AI.

---

## Chương 8. Hàm nhiều biến

### 8.1 Giới hạn và liên tục

$\lim\limits_{(x, y) \to (x_0, y_0)} f(x, y) = L$ đòi hỏi $f$ tiến tới $L$ theo **mọi con đường** — khác với một biến chỉ có hai phía.

**Cách chứng minh giới hạn KHÔNG tồn tại:** tìm hai đường đi cho hai giới hạn khác nhau.

**Ví dụ 8.1.** $f(x, y) = \dfrac{xy}{x^2 + y^2}$ tại $(0, 0)$. Đi theo đường $y = kx$:

$$f(x, kx) = \frac{kx^2}{x^2 + k^2x^2} = \frac{k}{1 + k^2}$$

Kết quả phụ thuộc $k$ (ví dụ $k = 0$ cho 0, $k = 1$ cho $\frac{1}{2}$) → **giới hạn không tồn tại**.

### 8.2 Đạo hàm riêng

**Đạo hàm riêng theo $x$:** coi mọi biến khác là **hằng số**, đạo hàm như hàm một biến.

**Ví dụ 8.2.** $f(x, y) = x^2 y + e^{xy}$:

$$f'_x = 2xy + y e^{xy} \qquad f'_y = x^2 + x e^{xy}$$

**Đạo hàm riêng cấp hai:** $f''_{xx}$, $f''_{yy}$, $f''_{xy}$, $f''_{yx}$. **Định lý Schwarz:** nếu các đạo hàm hỗn hợp liên tục thì $f''_{xy} = f''_{yx}$.

### 8.3 Vi phân toàn phần và xấp xỉ

$$df = f'_x\,dx + f'_y\,dy \qquad f(x_0 + \Delta x, y_0 + \Delta y) \approx f(x_0, y_0) + f'_x \Delta x + f'_y \Delta y$$

**Ví dụ 8.3.** Tính gần đúng $\sqrt{3{,}02^2 + 3{,}97^2}$. Chọn $f = \sqrt{x^2 + y^2}$ tại $(3, 4)$: $f = 5$, $f'_x = \frac{x}{f} = \frac{3}{5}$, $f'_y = \frac{4}{5}$:

$$f \approx 5 + \frac{3}{5}(0{,}02) + \frac{4}{5}(-0{,}03) = 4{,}988$$

Giá trị thật: $4{,}98812\ldots$

### 8.4 Đạo hàm hàm hợp

Nếu $z = f(x, y)$ với $x = x(t)$, $y = y(t)$:

$$\frac{dz}{dt} = \frac{\partial f}{\partial x}\frac{dx}{dt} + \frac{\partial f}{\partial y}\frac{dy}{dt}$$

Nếu $x = x(u, v)$, $y = y(u, v)$: $\dfrac{\partial z}{\partial u} = \dfrac{\partial f}{\partial x}\dfrac{\partial x}{\partial u} + \dfrac{\partial f}{\partial y}\dfrac{\partial y}{\partial u}$ (tương tự cho $v$).

**Ví dụ 8.4.** $z = x^2 y$, $x = \cos t$, $y = \sin t$:

$$\frac{dz}{dt} = 2xy \cdot (-\sin t) + x^2 \cdot \cos t = -2\cos t \sin^2 t + \cos^3 t = \cos t\,(1 - 3\sin^2 t)$$

**Hàm ẩn nhiều biến:** nếu $F(x, y) = 0$ thì $y'(x) = -\dfrac{F'_x}{F'_y}$ (kiểm tra lại Ví dụ 3.4 với $F = x^2 + y^2 - 25$).

### 8.5 Gradient và đạo hàm theo hướng

**Gradient:** $\nabla f = \left(\dfrac{\partial f}{\partial x}, \dfrac{\partial f}{\partial y}\right)$ (thêm $\frac{\partial f}{\partial z}$ nếu ba biến).

**Đạo hàm theo hướng** véc tơ đơn vị $\vec{u}$: $D_{\vec{u}} f = \nabla f \cdot \vec{u}$.

**Ý nghĩa của gradient** (cực kỳ quan trọng cho AI):

1. $\nabla f$ chỉ hướng mà $f$ **tăng nhanh nhất**; $-\nabla f$ chỉ hướng **giảm nhanh nhất**.
2. $\|\nabla f\|$ là tốc độ tăng lớn nhất đó.
3. $\nabla f$ **vuông góc** với đường mức (mặt mức) của $f$.

**Ví dụ 8.5.** $f = x^2 + 3xy$ tại $(1, 2)$ theo hướng $\vec{v} = (3, 4)$.

$\nabla f = (2x + 3y,\ 3x) = (8, 3)$ tại $(1, 2)$. Véc tơ đơn vị $\vec{u} = \left(\frac{3}{5}, \frac{4}{5}\right)$:

$$D_{\vec{u}} f = 8 \cdot \frac{3}{5} + 3 \cdot \frac{4}{5} = \frac{36}{5} = 7{,}2$$

> 🤖 Thuật toán **gradient descent** — trái tim của việc huấn luyện mạng nơ-ron — chỉ là lặp lại: "đi một bước nhỏ theo hướng $-\nabla f$". Xem Chương 18.

### 8.6 Cực trị tự do

**Bước 1 — điểm dừng:** giải $f'_x = 0$, $f'_y = 0$.

**Bước 2 — xét dấu** tại mỗi điểm dừng, với $A = f''_{xx}$, $B = f''_{xy}$, $C = f''_{yy}$ và $\Delta = AC - B^2$:

| Điều kiện | Kết luận |
|---|---|
| $\Delta > 0$, $A > 0$ | **Cực tiểu** |
| $\Delta > 0$, $A < 0$ | **Cực đại** |
| $\Delta < 0$ | **Không** phải cực trị (**điểm yên ngựa**) |
| $\Delta = 0$ | Chưa kết luận được, phải xét thêm |

> 🔗 **Liên hệ đại số tuyến tính:** $\begin{pmatrix} A & B \\ B & C \end{pmatrix}$ chính là **ma trận Hessian**. Điều kiện "$A > 0$, $\Delta > 0$" chính là **tiêu chuẩn Sylvester** cho ma trận **xác định dương** (dạng toàn phương xác định dương). Với hàm $n$ biến, cực tiểu ⇔ Hessian xác định dương.

**Ví dụ 8.6.** Tìm cực trị của $f = x^3 + y^3 - 3xy$.

*Bước 1:* $f'_x = 3x^2 - 3y = 0$, $f'_y = 3y^2 - 3x = 0$. Từ phương trình đầu $y = x^2$; thế vào phương trình sau: $x^4 = x$ → $x = 0$ hoặc $x = 1$. Hai điểm dừng: $(0, 0)$ và $(1, 1)$.

*Bước 2:* $A = 6x$, $B = -3$, $C = 6y$.

- Tại $(0, 0)$: $\Delta = 0 - 9 < 0$ → **điểm yên ngựa**.
- Tại $(1, 1)$: $A = 6 > 0$, $\Delta = 36 - 9 = 27 > 0$ → **cực tiểu**, $f(1, 1) = -1$.

### 8.7 Cực trị có điều kiện — nhân tử Lagrange

Tìm cực trị của $f(x, y)$ với ràng buộc $g(x, y) = 0$. Lập hàm Lagrange và giải hệ:

$$\mathcal{L} = f - \lambda g \qquad \begin{cases} f'_x = \lambda g'_x \\ f'_y = \lambda g'_y \\ g(x, y) = 0 \end{cases}$$

**Ý nghĩa hình học:** tại điểm cực trị, **đường mức của $f$ tiếp xúc với đường ràng buộc**, nên hai gradient **cùng phương**: $\nabla f = \lambda \nabla g$.

**Ví dụ 8.7.** Tìm cực trị của $f = x + y$ trên đường tròn $x^2 + y^2 = 2$.

$$\begin{cases} 1 = 2\lambda x \\ 1 = 2\lambda y \\ x^2 + y^2 = 2 \end{cases} \Rightarrow x = y \Rightarrow 2x^2 = 2 \Rightarrow x = \pm 1$$

- $(1, 1)$: $f = 2$ → **giá trị lớn nhất**.
- $(-1, -1)$: $f = -2$ → **giá trị nhỏ nhất**.

(Vì đường tròn là tập đóng và bị chặn, $f$ chắc chắn đạt max/min trên đó, nên so sánh giá trị là đủ.)

### 8.8 Giá trị lớn nhất, nhỏ nhất trên miền đóng

**Quy trình:** (1) tìm điểm dừng **bên trong** miền; (2) tìm cực trị **trên biên** (thay phương trình biên vào $f$ hoặc dùng Lagrange); (3) tính $f$ tại tất cả các điểm đó và **các góc** của biên; (4) so sánh.

---

## Bài tập Phần I

**Bài I.1.** Tính: (a) $\lim\limits_{x \to 0} \dfrac{\tan x - \sin x}{x^3}$; (b) $\lim\limits_{x \to \infty} \left(\dfrac{x + 1}{x - 1}\right)^x$; (c) $\lim\limits_{x \to 0} \dfrac{\sqrt{1 + x} - 1}{x}$.

**Bài I.2.** Tính đạo hàm của $y = \arcsin\sqrt{x}$.

**Bài I.3.** Khai triển Maclaurin của $e^x \cos x$ đến số hạng $x^3$.

**Bài I.4.** Khảo sát cực trị và điểm uốn của $y = x e^{-x}$.

**Bài I.5.** Tính: (a) $\displaystyle\int x^2 e^x dx$; (b) $\displaystyle\int \frac{dx}{x^2 + 4x + 5}$; (c) $\displaystyle\int_0^1 \frac{x}{1 + x^2}dx$; (d) $\displaystyle\int_0^{+\infty} x e^{-x}dx$.

**Bài I.6.** Tìm cực trị của $f(x, y) = x^2 + y^2 - 2x + 4y$.

**Bài I.7.** Tìm giá trị nhỏ nhất của $x^2 + y^2$ với điều kiện $x + 2y = 5$ (tức là khoảng cách bình phương từ gốc tọa độ tới một đường thẳng).

<details>
<summary><b>Đáp án Phần I</b> (bấm để mở)</summary>

**I.1.** (a) $\dfrac{1}{2}$ (xem cách làm đúng ở Chương 1.4). (b) $\dfrac{x + 1}{x - 1} = 1 + \dfrac{2}{x - 1}$, dạng $1^\infty$ → $e^{\lim \frac{2x}{x - 1}} = e^2$. (c) $\sqrt{1 + x} - 1 \sim \dfrac{x}{2}$ → $\dfrac{1}{2}$.

**I.2.** $y' = \dfrac{1}{\sqrt{1 - x}} \cdot \dfrac{1}{2\sqrt{x}} = \dfrac{1}{2\sqrt{x(1 - x)}}$.

**I.3.** Nhân hai khai triển $\left(1 + x + \frac{x^2}{2} + \frac{x^3}{6}\right)\left(1 - \frac{x^2}{2}\right)$ và giữ đến $x^3$: $e^x\cos x = 1 + x - \dfrac{x^3}{3} + o(x^3)$ (hệ số của $x^2$ bằng $\frac{1}{2} - \frac{1}{2} = 0$).

**I.4.** $y' = e^{-x}(1 - x) = 0 \iff x = 1$; $y'$ đổi dấu từ $+$ sang $-$ → **cực đại** $y(1) = \dfrac{1}{e}$. $y'' = e^{-x}(x - 2) = 0 \iff x = 2$, đổi dấu → **điểm uốn** $\left(2, \dfrac{2}{e^2}\right)$.

**I.5.** (a) Từng phần hai lần: $e^x(x^2 - 2x + 2) + C$. (b) $\arctan(x + 2) + C$. (c) $\dfrac{1}{2}\ln(1 + x^2)\Big|_0^1 = \dfrac{\ln 2}{2}$. (d) $[-(x + 1)e^{-x}]_0^{\infty} = 1$.

**I.6.** $f'_x = 2x - 2 = 0$, $f'_y = 2y + 4 = 0$ → điểm dừng $(1, -2)$. $A = 2 > 0$, $\Delta = 4 > 0$ → **cực tiểu** $f(1, -2) = -5$. (Hoặc nhận xét $f = (x - 1)^2 + (y + 2)^2 - 5$.)

**I.7.** $2x = \lambda$, $2y = 2\lambda$, $x + 2y = 5$ → $y = 2x$ → $5x = 5$ → điểm $(1, 2)$, giá trị nhỏ nhất **5**.

</details>

---

# PHẦN II — GIẢI TÍCH 2

## Chương 9. Ứng dụng phép tính vi phân trong hình học

### 9.1 Đường cong phẳng

| Dạng đường cong | Tiếp tuyến tại $M_0(x_0, y_0)$ |
|---|---|
| $y = f(x)$ | $y - y_0 = f'(x_0)(x - x_0)$ |
| $F(x, y) = 0$ | $F'_x(M_0)(x - x_0) + F'_y(M_0)(y - y_0) = 0$ |
| $x = x(t),\ y = y(t)$ | $\dfrac{x - x_0}{x'(t_0)} = \dfrac{y - y_0}{y'(t_0)}$ |

Với dạng $F(x, y) = 0$, véc tơ $\nabla F = (F'_x, F'_y)$ là **pháp tuyến** của đường cong.

**Độ cong** — đo mức độ "bẻ cong" của đường:

$$\kappa = \frac{|y''|}{\left(1 + y'^2\right)^{3/2}} \qquad \text{(dạng tham số: } \kappa = \frac{|x'y'' - x''y'|}{\left(x'^2 + y'^2\right)^{3/2}}\text{)}$$

**Bán kính cong** $R = 1 / \kappa$ là bán kính của đường tròn "ôm sát" đường cong nhất tại điểm đó.

**Ví dụ 9.1.** Đường tròn bán kính $R$ có độ cong $\frac{1}{R}$ ở mọi điểm. Parabol $y = x^2$ tại gốc: $y' = 0$, $y'' = 2$ → $\kappa = 2$, bán kính cong $\frac{1}{2}$.

> 🎮 Độ cong được dùng khi thiết kế đường đua, quỹ đạo camera, hay giới hạn tốc độ vào cua của xe AI (lực hướng tâm tỉ lệ với $v^2 \kappa$).

**Hình bao** của họ đường cong $F(x, y, c) = 0$ (đường tiếp xúc với mọi đường trong họ): khử $c$ từ hệ $F = 0$, $F'_c = 0$.

### 9.2 Đường cong trong không gian

Với $\vec{r}(t) = \big(x(t), y(t), z(t)\big)$, véc tơ tiếp tuyến là $\vec{r}\,'(t_0) = \big(x'(t_0), y'(t_0), z'(t_0)\big)$.

- **Tiếp tuyến:** $\dfrac{x - x_0}{x'(t_0)} = \dfrac{y - y_0}{y'(t_0)} = \dfrac{z - z_0}{z'(t_0)}$
- **Pháp diện** (mặt phẳng vuông góc với tiếp tuyến): $x'(t_0)(x - x_0) + y'(t_0)(y - y_0) + z'(t_0)(z - z_0) = 0$

**Ví dụ 9.2.** Đường xoắn ốc $\vec{r}(t) = (\cos t, \sin t, t)$ tại $t = 0$: điểm $(1, 0, 0)$, $\vec{r}\,'(0) = (0, 1, 1)$.

- Tiếp tuyến: $x = 1$, $y = z$ (theo quy ước: mẫu số bằng 0 thì tử số tương ứng bằng 0).
- Pháp diện: $y + z = 0$.

### 9.3 Mặt cong

**Mặt $F(x, y, z) = 0$:** véc tơ pháp tuyến là $\nabla F = (F'_x, F'_y, F'_z)$.

$$\text{Tiếp diện: } F'_x(x - x_0) + F'_y(y - y_0) + F'_z(z - z_0) = 0$$

**Mặt $z = f(x, y)$:** $\text{tiếp diện: } z - z_0 = f'_x(x - x_0) + f'_y(y - y_0)$.

**Ví dụ 9.3.** Tiếp diện của mặt cầu $x^2 + y^2 + z^2 = 14$ tại $(1, 2, 3)$: $\nabla F = (2x, 2y, 2z) = (2, 4, 6)$:

$$2(x - 1) + 4(y - 2) + 6(z - 3) = 0 \iff x + 2y + 3z = 14$$

**Ví dụ 9.4.** Tiếp diện của $z = x^2 + y^2$ tại $(1, 1, 2)$: $f'_x = 2$, $f'_y = 2$ → $z = 2 + 2(x - 1) + 2(y - 1)$, tức $z = 2x + 2y - 2$.

> 🎮 **Liên hệ game:** Với địa hình cho bởi bản đồ độ cao $z = h(x, y)$, véc tơ pháp tuyến dùng để **chiếu sáng** là $(-h'_x, -h'_y, 1)$ (rồi chuẩn hóa). Trong thực tế, các đạo hàm riêng này được tính gần đúng bằng sai phân giữa các ô lân cận của bản đồ độ cao.

---

## Chương 10. Tích phân bội

### 10.1 Tích phân kép

**Ý nghĩa:** $\displaystyle\iint_D f(x, y)\,dA$ là **thể tích** dưới mặt $z = f(x, y)$ trên miền $D$ (nếu $f \ge 0$). Khi $f = 1$, nó là **diện tích** của $D$.

**Định lý Fubini:** tính bằng hai tích phân lặp.

- **Miền chữ nhật** $[a, b] \times [c, d]$: $\displaystyle\iint_D f\,dA = \int_a^b\!\!\int_c^d f(x, y)\,dy\,dx$.
- **Miền loại 1** $\{a \le x \le b,\ y_1(x) \le y \le y_2(x)\}$: $\displaystyle\int_a^b \left(\int_{y_1(x)}^{y_2(x)} f\,dy\right)dx$.
- **Miền loại 2** $\{c \le y \le d,\ x_1(y) \le x \le x_2(y)\}$: $\displaystyle\int_c^d \left(\int_{x_1(y)}^{x_2(y)} f\,dx\right)dy$.

**Ví dụ 10.1.** $\displaystyle\iint_D (x + y)\,dA$, $D = [0, 1] \times [0, 2]$:

$$\int_0^1 \left[xy + \frac{y^2}{2}\right]_0^2 dx = \int_0^1 (2x + 2)\,dx = 1 + 2 = 3$$

**Ví dụ 10.2.** $\displaystyle\iint_D xy\,dA$, $D$ giới hạn bởi $y = x^2$ và $y = x$. Hai đường cắt nhau tại $x = 0, 1$, và $x^2 \le y \le x$:

$$\int_0^1 \int_{x^2}^{x} xy\,dy\,dx = \int_0^1 x \cdot \frac{x^2 - x^4}{2}dx = \frac{1}{2}\left(\frac{1}{4} - \frac{1}{6}\right) = \frac{1}{24}$$

> 💡 **Luôn vẽ miền $D$** trước khi đặt cận. Hầu hết lỗi tích phân bội nằm ở cận.

### 10.2 Đổi thứ tự tích phân

Khi tích phân theo thứ tự hiện tại **không tính được**, hãy mô tả lại miền theo thứ tự khác.

**Ví dụ 10.3.** $\displaystyle\int_0^1 \int_x^1 e^{y^2}\,dy\,dx$ — không có nguyên hàm sơ cấp của $e^{y^2}$ theo $y$.

Miền: $0 \le x \le 1$, $x \le y \le 1$ — tam giác, cũng chính là $0 \le y \le 1$, $0 \le x \le y$:

$$\int_0^1 \int_0^y e^{y^2}\,dx\,dy = \int_0^1 y\,e^{y^2}\,dy = \frac{e^{y^2}}{2}\Big|_0^1 = \frac{e - 1}{2}$$

### 10.3 Đổi biến và tọa độ cực

**Công thức đổi biến tổng quát:** với $x = x(u, v)$, $y = y(u, v)$:

$$\iint_D f(x, y)\,dx\,dy = \iint_{D'} f\big(x(u, v), y(u, v)\big)\,|J|\,du\,dv, \qquad J = \begin{vmatrix} x'_u & x'_v \\ y'_u & y'_v \end{vmatrix}$$

**Tọa độ cực:** $x = r\cos\varphi$, $y = r\sin\varphi$, $|J| = r$:

$$\boxed{dx\,dy = r\,dr\,d\varphi}$$

Dùng khi miền là **hình tròn, hình vành khăn, hình quạt**, hoặc hàm chứa $x^2 + y^2$.

**Ví dụ 10.4.** $\displaystyle\iint_{x^2 + y^2 \le 4} e^{-(x^2 + y^2)}\,dA = \int_0^{2\pi}\!\!\int_0^2 e^{-r^2}\,r\,dr\,d\varphi = 2\pi \cdot \frac{1 - e^{-4}}{2} = \pi\left(1 - e^{-4}\right)$

**Ví dụ 10.5 (tích phân Gauss).** Đặt $I = \displaystyle\int_{-\infty}^{+\infty} e^{-x^2}dx$. Ta có

$$I^2 = \int_{-\infty}^{+\infty}\!\!\int_{-\infty}^{+\infty} e^{-(x^2 + y^2)}\,dx\,dy = \int_0^{2\pi}\!\!\int_0^{\infty} e^{-r^2}\,r\,dr\,d\varphi = 2\pi \cdot \frac{1}{2} = \pi$$

Vậy $I = \sqrt{\pi}$ — một kết quả không thể có được bằng phương pháp một biến thông thường.

### 10.4 Tích phân bội ba

$\displaystyle\iiint_V f\,dV$ tính tương tự bằng ba tích phân lặp. Khi $f = 1$, nó là **thể tích** của $V$.

| Hệ tọa độ | Đổi biến | $dV$ | Dùng khi |
|---|---|---|---|
| **Trụ** | $x = r\cos\varphi$, $y = r\sin\varphi$, $z = z$ | $r\,dr\,d\varphi\,dz$ | Hình trụ, paraboloid, hình nón |
| **Cầu** | $x = \rho\sin\theta\cos\varphi$, $y = \rho\sin\theta\sin\varphi$, $z = \rho\cos\theta$ | $\rho^2 \sin\theta\,d\rho\,d\theta\,d\varphi$ | Hình cầu, chứa $x^2 + y^2 + z^2$ |

(Trong tọa độ cầu, $\theta \in [0, \pi]$ là góc so với trục $Oz$, $\varphi \in [0, 2\pi]$ là góc quanh trục $Oz$. Một số sách đổi tên hai góc này — hãy kiểm tra quy ước trong giáo trình.)

**Ví dụ 10.6.** Thể tích hình cầu bán kính $R$:

$$V = \int_0^{2\pi}\!\!\int_0^{\pi}\!\!\int_0^R \rho^2\sin\theta\,d\rho\,d\theta\,d\varphi = 2\pi \cdot 2 \cdot \frac{R^3}{3} = \frac{4}{3}\pi R^3$$

**Ví dụ 10.7.** Thể tích vật thể giới hạn bởi $z = 4 - x^2 - y^2$ và $z = 0$. Dùng tọa độ trụ, $0 \le z \le 4 - r^2$, $0 \le r \le 2$:

$$V = \int_0^{2\pi}\!\!\int_0^2 (4 - r^2)\,r\,dr\,d\varphi = 2\pi \left[2r^2 - \frac{r^4}{4}\right]_0^2 = 2\pi \cdot 4 = 8\pi$$

### 10.5 Ứng dụng

| Đại lượng | Công thức (với khối lượng riêng $\rho(x, y)$) |
|---|---|
| Diện tích | $S = \iint_D dA$ |
| Khối lượng | $m = \iint_D \rho\,dA$ |
| Trọng tâm | $\bar{x} = \dfrac{1}{m}\iint_D x\rho\,dA$, $\quad \bar{y} = \dfrac{1}{m}\iint_D y\rho\,dA$ |
| Mô-men quán tính quanh $Oz$ | $I_z = \iint_D (x^2 + y^2)\rho\,dA$ |

**Ví dụ 10.8.** Trọng tâm nửa hình tròn đơn vị $y \ge 0$ (đồng chất). Diện tích $\frac{\pi}{2}$; do đối xứng $\bar{x} = 0$:

$$\bar{y} = \frac{2}{\pi}\int_0^{\pi}\!\!\int_0^1 (r\sin\varphi)\,r\,dr\,d\varphi = \frac{2}{\pi} \cdot \frac{1}{3} \cdot 2 = \frac{4}{3\pi} \approx 0{,}424$$

> 🎮 Engine vật lý cần **khối lượng, trọng tâm, và tenxơ quán tính** của mỗi vật thể để mô phỏng chuyển động quay — tất cả đều là tích phân bội.

---

## Chương 11. Tích phân đường

### 11.1 Tích phân đường loại 1 (theo độ dài cung)

$$\int_C f(x, y)\,ds, \qquad ds = \sqrt{x'(t)^2 + y'(t)^2}\,dt$$

Ý nghĩa: **khối lượng của sợi dây** có khối lượng riêng $f$; khi $f = 1$ là **độ dài** đường cong. Không phụ thuộc chiều đi.

**Ví dụ 11.1.** $\displaystyle\int_C (x + y)\,ds$, $C$ là đoạn thẳng từ $(0, 0)$ đến $(1, 1)$. Tham số hóa $x = y = t$, $ds = \sqrt{2}\,dt$:

$$\int_0^1 2t\sqrt{2}\,dt = \sqrt{2}$$

### 11.2 Tích phân đường loại 2 (theo tọa độ)

$$\int_C P\,dx + Q\,dy = \int_{t_1}^{t_2} \Big(P\big(x(t), y(t)\big)x'(t) + Q\big(x(t), y(t)\big)y'(t)\Big)dt$$

Ý nghĩa: **công** của lực $\vec{F} = (P, Q)$ khi di chuyển dọc $C$. **Đổi chiều đi thì đổi dấu.**

**Ví dụ 11.2.** Công của lực $\vec{F} = (-y, x)$ khi đi một vòng đường tròn đơn vị ngược chiều kim đồng hồ. Tham số $x = \cos t$, $y = \sin t$, $t: 0 \to 2\pi$:

$$\oint_C -y\,dx + x\,dy = \int_0^{2\pi} \big(\sin^2 t + \cos^2 t\big)\,dt = 2\pi$$

### 11.3 Định lý Green

Với $C$ là biên của miền $D$, đi theo **chiều dương** (miền nằm bên trái khi đi):

$$\boxed{\oint_C P\,dx + Q\,dy = \iint_D \left(\frac{\partial Q}{\partial x} - \frac{\partial P}{\partial y}\right)dA}$$

**Kiểm tra với Ví dụ 11.2:** $\dfrac{\partial Q}{\partial x} - \dfrac{\partial P}{\partial y} = 1 - (-1) = 2$, nên vế phải $= 2 \times \text{diện tích hình tròn} = 2\pi$ ✓.

**Hệ quả — tính diện tích bằng tích phân đường:** $S = \dfrac{1}{2}\displaystyle\oint_C x\,dy - y\,dx$.

> 💻 Công thức này là cơ sở của **công thức "dây giày" (shoelace)** tính diện tích đa giác từ tọa độ các đỉnh: $S = \frac{1}{2}\left|\sum (x_i y_{i+1} - x_{i+1} y_i)\right|$ — rất hay dùng trong đồ họa và game.

### 11.4 Tích phân không phụ thuộc đường đi

Trên miền đơn liên, bốn điều sau **tương đương**:

1. $\dfrac{\partial P}{\partial y} = \dfrac{\partial Q}{\partial x}$.
2. $\displaystyle\oint_C P\,dx + Q\,dy = 0$ với mọi đường cong kín $C$.
3. Tích phân từ $A$ đến $B$ **không phụ thuộc đường đi**.
4. Tồn tại hàm thế $U$ với $dU = P\,dx + Q\,dy$; khi đó $\displaystyle\int_A^B P\,dx + Q\,dy = U(B) - U(A)$.

**Ví dụ 11.3.** $\displaystyle\int_{(0,0)}^{(1,2)} y\,dx + x\,dy$. Vì $\frac{\partial P}{\partial y} = 1 = \frac{\partial Q}{\partial x}$ và $y\,dx + x\,dy = d(xy)$:

$$\int_{(0,0)}^{(1,2)} y\,dx + x\,dy = xy\Big|_{(0,0)}^{(1,2)} = 2$$

Kiểm tra theo đường thẳng $y = 2x$ ($x = t$, $y = 2t$): $\int_0^1 (2t \cdot 1 + t \cdot 2)\,dt = 2$ ✓.

> 🎮 Trường lực như vậy gọi là **trường thế (bảo toàn)** — trọng lực, lực đàn hồi. Công chỉ phụ thuộc điểm đầu và cuối, nên **năng lượng được bảo toàn**. Chương 18 cho thấy một bộ tích phân số tồi có thể phá vỡ tính chất này.

---

## Chương 12. Tích phân mặt và lý thuyết trường

### 12.1 Tích phân mặt loại 1

$$\iint_S f\,dS, \qquad \text{với mặt } z = z(x, y): \quad dS = \sqrt{1 + z'^2_x + z'^2_y}\,dx\,dy$$

Khi $f = 1$: **diện tích mặt**. Ví dụ, dùng tham số hóa cầu, diện tích mặt cầu bán kính $R$ là $\displaystyle\int_0^{2\pi}\!\!\int_0^{\pi} R^2\sin\theta\,d\theta\,d\varphi = 4\pi R^2$.

### 12.2 Tích phân mặt loại 2 — thông lượng

$$\iint_S \vec{F} \cdot \vec{n}\,dS$$

là **thông lượng** (lưu lượng) của trường $\vec{F}$ đi qua mặt $S$ theo hướng pháp tuyến $\vec{n}$ đã chọn — ví dụ lượng chất lỏng chảy qua một tấm lưới mỗi giây. Đổi hướng $\vec{n}$ thì đổi dấu.

### 12.3 Các toán tử của lý thuyết trường

Với trường vô hướng $u(x, y, z)$ và trường véc tơ $\vec{F} = (P, Q, R)$:

| Toán tử | Công thức | Ý nghĩa vật lý |
|---|---|---|
| **Gradient** | $\nabla u = (u'_x, u'_y, u'_z)$ | Hướng tăng nhanh nhất |
| **Divergence** | $\operatorname{div}\vec{F} = P'_x + Q'_y + R'_z$ | **Nguồn/hố**: dương = chất lỏng "phun ra", âm = "hút vào" |
| **Rotation (curl)** | $\operatorname{rot}\vec{F} = \big(R'_y - Q'_z,\ P'_z - R'_x,\ Q'_x - P'_y\big)$ | **Độ xoáy** của trường |
| **Laplace** | $\Delta u = u''_{xx} + u''_{yy} + u''_{zz}$ | Mức độ $u$ khác trung bình lân cận (xuất hiện trong phương trình nhiệt, sóng) |

**Ví dụ 12.1.** $\vec{F} = (x^2 y,\ yz,\ xz)$:

- $\operatorname{div}\vec{F} = 2xy + z + x$
- $\operatorname{rot}\vec{F} = (0 - y,\ 0 - z,\ 0 - x^2) = (-y,\ -z,\ -x^2)$

**Hai loại trường đặc biệt:**

- **Trường thế:** $\operatorname{rot}\vec{F} = \vec{0}$ ⇔ $\vec{F} = \nabla U$ (trên miền đơn liên) — mở rộng của Chương 11.4 lên 3 chiều.
- **Trường ống (không nén được):** $\operatorname{div}\vec{F} = 0$ — ví dụ dòng chảy của nước.

> 🎮 Mô phỏng chất lỏng trong game (khói, nước) thường có một bước "chiếu" để làm cho trường vận tốc có **divergence bằng 0**, nhằm giữ chất lỏng không nén được.

### 12.4 Công thức Gauss–Ostrogradsky và Stokes

**Gauss–Ostrogradsky** (mặt kín $S$ bao khối $V$, pháp tuyến hướng **ra ngoài**):

$$\boxed{\iint_S \vec{F} \cdot \vec{n}\,dS = \iiint_V \operatorname{div}\vec{F}\,dV}$$

"Tổng lượng chảy ra khỏi mặt kín = tổng các nguồn bên trong."

**Ví dụ 12.2.** Thông lượng của $\vec{F} = (x, y, z)$ qua mặt cầu đơn vị hướng ra ngoài.

- *Tính trực tiếp:* trên mặt cầu đơn vị, $\vec{n} = (x, y, z)$ nên $\vec{F} \cdot \vec{n} = x^2 + y^2 + z^2 = 1$ → thông lượng = diện tích = $4\pi$.
- *Dùng Gauss:* $\operatorname{div}\vec{F} = 3$ → $3 \times \frac{4}{3}\pi = 4\pi$ ✓.

**Stokes** (mặt $S$ có biên $C$, hướng của $C$ và $\vec{n}$ phù hợp theo quy tắc bàn tay phải):

$$\boxed{\oint_C \vec{F} \cdot d\vec{r} = \iint_S \operatorname{rot}\vec{F} \cdot \vec{n}\,dS}$$

Định lý Green chính là trường hợp riêng của Stokes trong mặt phẳng. Kiểm tra với Ví dụ 11.2: $\vec{F} = (-y, x, 0)$ có $\operatorname{rot}\vec{F} = (0, 0, 2)$; thông lượng qua hình tròn đơn vị với $\vec{n} = (0, 0, 1)$ là $2 \times \pi = 2\pi$ ✓.

**Bức tranh thống nhất:**

| Định lý | "Tích phân của đạo hàm bên trong" = "giá trị trên biên" |
|---|---|
| Newton–Leibniz | $\int_a^b f'\,dx = f(b) - f(a)$ |
| Green | $\iint_D (Q'_x - P'_y)\,dA = \oint_C P\,dx + Q\,dy$ |
| Stokes | $\iint_S \operatorname{rot}\vec{F} \cdot \vec{n}\,dS = \oint_C \vec{F} \cdot d\vec{r}$ |
| Gauss | $\iiint_V \operatorname{div}\vec{F}\,dV = \iint_S \vec{F} \cdot \vec{n}\,dS$ |

---

## Bài tập Phần II

**Bài II.1.** Viết phương trình tiếp diện của mặt $z = xy$ tại điểm $(1, 2, 2)$.

**Bài II.2.** Tính $\displaystyle\iint_D xy\,dA$ với $D = \{0 \le x \le 1,\ 0 \le y \le x\}$.

**Bài II.3.** Đổi thứ tự rồi tính $\displaystyle\int_0^1\!\!\int_y^1 \sin(x^2)\,dx\,dy$.

**Bài II.4.** Tính $\displaystyle\iint_{x^2 + y^2 \le 1} (x^2 + y^2)\,dA$.

**Bài II.5.** Tính $\displaystyle\int_C x\,ds$ với $C$ là cung $y = x^2$, $0 \le x \le 1$.

**Bài II.6.** Dùng định lý Green tính $\displaystyle\oint_C x^2 y\,dx + x\,dy$ với $C$ là biên hình vuông $[0, 1]^2$, chiều dương.

**Bài II.7.** Tính thông lượng của $\vec{F} = (x, y, z)$ qua mặt biên của khối lập phương $[0, 1]^3$, hướng ra ngoài.

**Bài II.8.** Chứng minh $\vec{F} = (2xy + z,\ x^2,\ x)$ là trường thế và tìm hàm thế.

<details>
<summary><b>Đáp án Phần II</b> (bấm để mở)</summary>

**II.1.** $f'_x = y = 2$, $f'_y = x = 1$ → $z = 2 + 2(x - 1) + (y - 2)$, tức **$2x + y - z = 2$**.

**II.2.** $\displaystyle\int_0^1 x \cdot \frac{x^2}{2}\,dx = \frac{1}{8}$.

**II.3.** Miền: $0 \le y \le x \le 1$ → $\displaystyle\int_0^1\!\!\int_0^x \sin(x^2)\,dy\,dx = \int_0^1 x\sin(x^2)\,dx = \frac{1 - \cos 1}{2}$.

**II.4.** Tọa độ cực: $\displaystyle\int_0^{2\pi}\!\!\int_0^1 r^2 \cdot r\,dr\,d\varphi = \frac{\pi}{2}$.

**II.5.** $ds = \sqrt{1 + 4x^2}\,dx$ → $\displaystyle\int_0^1 x\sqrt{1 + 4x^2}\,dx = \frac{(1 + 4x^2)^{3/2}}{12}\Big|_0^1 = \frac{5\sqrt{5} - 1}{12}$.

**II.6.** $Q'_x - P'_y = 1 - x^2$ → $\displaystyle\int_0^1\!\!\int_0^1 (1 - x^2)\,dx\,dy = \frac{2}{3}$. (Đã kiểm tra thêm bằng cách tính trực tiếp trên 4 cạnh — cũng ra $\frac{2}{3}$.)

**II.7.** Theo Gauss: $\operatorname{div}\vec{F} = 3$, thể tích bằng 1 → thông lượng **3**.

**II.8.** $\operatorname{rot}\vec{F} = (0 - 0,\ 1 - 1,\ 2x - 2x) = \vec{0}$ → trường thế. Hàm thế: $U = x^2 y + xz$ (kiểm tra: $U'_x = 2xy + z$, $U'_y = x^2$, $U'_z = x$ ✓).

</details>

---

# PHẦN III — GIẢI TÍCH 3

## Chương 13. Chuỗi số

### 13.1 Khái niệm

**Chuỗi số** $\displaystyle\sum_{n=1}^{\infty} u_n$ là "tổng vô hạn". Gọi $S_n = u_1 + \dots + u_n$ là **tổng riêng**. Chuỗi **hội tụ** nếu $S_n \to S$ hữu hạn, và khi đó $S$ là **tổng của chuỗi**; ngược lại chuỗi **phân kỳ**.

**Hai chuỗi mẫu:**

- **Chuỗi hình học** $\displaystyle\sum_{n=0}^{\infty} q^n = \frac{1}{1 - q}$ nếu $|q| < 1$; phân kỳ nếu $|q| \ge 1$. Ví dụ: $\displaystyle\sum_{n=1}^{\infty} \frac{1}{2^n} = 1$.
- **Chuỗi Riemann** $\displaystyle\sum_{n=1}^{\infty} \frac{1}{n^p}$ hội tụ $\iff p > 1$. Đặc biệt **chuỗi điều hòa** $\sum \frac{1}{n}$ **phân kỳ** (dù số hạng tiến về 0), còn $\sum \frac{1}{n^2} = \frac{\pi^2}{6}$.

**Chuỗi "lồng nhau" (telescoping):**

**Ví dụ 13.1.** $\displaystyle\sum_{n=1}^{\infty} \frac{1}{n(n + 1)} = \sum_{n=1}^{\infty}\left(\frac{1}{n} - \frac{1}{n + 1}\right)$. Tổng riêng $S_n = 1 - \dfrac{1}{n + 1} \to 1$. Vậy tổng bằng **1**.

### 13.2 Điều kiện cần

$$\sum u_n \text{ hội tụ} \Rightarrow u_n \to 0$$

Dùng theo chiều ngược lại để **chứng minh phân kỳ**: nếu $u_n \not\to 0$ thì chuỗi phân kỳ. Ví dụ $\sum \frac{n}{n + 1}$ phân kỳ vì $u_n \to 1$.

> ⚠️ $u_n \to 0$ **không đủ** để chuỗi hội tụ — chuỗi điều hòa là phản ví dụ.

### 13.3 Các tiêu chuẩn cho chuỗi số dương

| Tiêu chuẩn | Cách dùng | Hợp với |
|---|---|---|
| **So sánh** | $0 \le u_n \le v_n$: $\sum v_n$ hội tụ ⇒ $\sum u_n$ hội tụ | |
| **So sánh giới hạn** | $u_n \sim v_n$ ⇒ cùng hội tụ hoặc cùng phân kỳ | Phân thức — so với $\sum \frac{1}{n^p}$ |
| **D'Alembert (tỉ số)** | $\lim \dfrac{u_{n+1}}{u_n} = D$: $D < 1$ hội tụ, $D > 1$ phân kỳ | Có **giai thừa**, **lũy thừa $a^n$** |
| **Cauchy (căn)** | $\lim \sqrt[n]{u_n} = C$: $C < 1$ hội tụ, $C > 1$ phân kỳ | Số hạng có dạng $(\dots)^n$ |
| **Tích phân** | $u_n = f(n)$, $f$ dương giảm: $\sum u_n$ và $\int_1^\infty f$ cùng tính chất | Có $\ln n$ |

(Khi $D = 1$ hoặc $C = 1$: tiêu chuẩn **không kết luận được**, phải dùng cách khác.)

**Ví dụ 13.2.** $\displaystyle\sum \frac{1}{n^2 + n + 1}$: vì $\dfrac{1}{n^2 + n + 1} \sim \dfrac{1}{n^2}$ và $\sum \frac{1}{n^2}$ hội tụ → **hội tụ**.

**Ví dụ 13.3.** $\displaystyle\sum \frac{n}{3^n}$: $\dfrac{u_{n+1}}{u_n} = \dfrac{n + 1}{3n} \to \dfrac{1}{3} < 1$ → **hội tụ** (và tổng bằng $\frac{3}{4}$).

**Ví dụ 13.4.** $\displaystyle\sum \left(\frac{n}{2n + 1}\right)^n$: $\sqrt[n]{u_n} = \dfrac{n}{2n + 1} \to \dfrac{1}{2} < 1$ → **hội tụ**.

**Ví dụ 13.5.** $\displaystyle\sum_{n \ge 2} \frac{1}{n\ln n}$: $\displaystyle\int_2^{\infty} \frac{dx}{x\ln x} = \big[\ln(\ln x)\big]_2^{\infty} = \infty$ → **phân kỳ**.

### 13.4 Chuỗi đan dấu và hội tụ tuyệt đối

**Tiêu chuẩn Leibniz:** chuỗi $\sum (-1)^n a_n$ với $a_n \ge 0$ **giảm dần về 0** thì hội tụ.

**Ví dụ 13.6.** $\displaystyle\sum_{n=1}^{\infty} \frac{(-1)^{n+1}}{n} = 1 - \frac{1}{2} + \frac{1}{3} - \dots = \ln 2$ hội tụ theo Leibniz.

**Hội tụ tuyệt đối và bán hội tụ:**

- $\sum |u_n|$ hội tụ ⇒ $\sum u_n$ hội tụ — gọi là **hội tụ tuyệt đối**.
- $\sum u_n$ hội tụ nhưng $\sum |u_n|$ phân kỳ — gọi là **bán hội tụ** (hội tụ có điều kiện). Chuỗi ở Ví dụ 13.6 là bán hội tụ.

> Chuỗi bán hội tụ có tính chất kỳ lạ: **đổi thứ tự các số hạng có thể làm thay đổi tổng** (định lý Riemann). Chuỗi hội tụ tuyệt đối thì không.

---

## Chương 14. Chuỗi lũy thừa và chuỗi Fourier

### 14.1 Chuỗi lũy thừa

$$\sum_{n=0}^{\infty} a_n (x - x_0)^n$$

Luôn có một **bán kính hội tụ** $R$: chuỗi hội tụ khi $|x - x_0| < R$, phân kỳ khi $|x - x_0| > R$, còn tại hai đầu mút **phải xét riêng**.

$$R = \lim_{n \to \infty} \left|\frac{a_n}{a_{n+1}}\right| \qquad \text{hoặc} \qquad \frac{1}{R} = \lim_{n \to \infty} \sqrt[n]{|a_n|}$$

**Quy trình tìm miền hội tụ:** (1) tính $R$; (2) xét riêng hai đầu mút bằng các tiêu chuẩn của Chương 13.

**Ví dụ 14.1.** $\displaystyle\sum_{n=1}^{\infty} \frac{x^n}{n}$: $R = \lim \dfrac{n + 1}{n} = 1$.

- $x = 1$: $\sum \frac{1}{n}$ phân kỳ.
- $x = -1$: $\sum \frac{(-1)^n}{n}$ hội tụ (Leibniz).

Miền hội tụ: $[-1, 1)$.

**Ví dụ 14.2.** $\displaystyle\sum_{n=1}^{\infty} \frac{(x - 2)^n}{3^n\, n}$: $R = \lim \dfrac{3^{n+1}(n + 1)}{3^n\, n} = 3$, tâm $x_0 = 2$ → khoảng $(-1, 5)$.

- $x = 5$: $\sum \frac{1}{n}$ phân kỳ.
- $x = -1$: $\sum \frac{(-1)^n}{n}$ hội tụ.

Miền hội tụ: $[-1, 5)$.

**Tính chất quan trọng:** bên trong khoảng hội tụ, được **đạo hàm và tích phân từng số hạng**.

**Ví dụ 14.3.** Tính $\displaystyle\sum_{n=1}^{\infty} n x^n$ với $|x| < 1$. Xuất phát từ $\sum x^n = \frac{1}{1 - x}$, đạo hàm hai vế: $\sum n x^{n-1} = \frac{1}{(1 - x)^2}$, rồi nhân với $x$:

$$\sum_{n=1}^{\infty} n x^n = \frac{x}{(1 - x)^2}$$

(Thay $x = \frac{1}{3}$: $\sum \frac{n}{3^n} = \frac{1/3}{4/9} = \frac{3}{4}$ — khớp với Ví dụ 13.3.)

**Chuỗi Taylor:** các khai triển Maclaurin ở Chương 4 chính là chuỗi lũy thừa. $e^x$, $\sin x$, $\cos x$ có $R = \infty$; $\ln(1 + x)$ và $\frac{1}{1 - x}$ có $R = 1$.

### 14.2 Chuỗi Fourier

**Ý tưởng:** biểu diễn một hàm **tuần hoàn** thành tổng các sóng $\sin$ và $\cos$ có tần số khác nhau.

Với hàm tuần hoàn chu kỳ $2\pi$:

$$f(x) \sim \frac{a_0}{2} + \sum_{n=1}^{\infty} \big(a_n \cos nx + b_n \sin nx\big)$$

$$a_n = \frac{1}{\pi}\int_{-\pi}^{\pi} f(x)\cos nx\,dx \qquad b_n = \frac{1}{\pi}\int_{-\pi}^{\pi} f(x)\sin nx\,dx$$

- Hàm **lẻ** → mọi $a_n = 0$ (chỉ còn $\sin$). Hàm **chẵn** → mọi $b_n = 0$ (chỉ còn $\cos$).
- **Định lý Dirichlet:** tại điểm liên tục, chuỗi hội tụ về $f(x)$; tại điểm gián đoạn, hội tụ về **trung bình cộng** hai giới hạn một phía.

**Ví dụ 14.4 (sóng vuông).** $f(x) = -1$ trên $(-\pi, 0)$, $f(x) = 1$ trên $(0, \pi)$. Hàm lẻ nên $a_n = 0$, và

$$b_n = \frac{2}{\pi}\int_0^{\pi} \sin nx\,dx = \frac{2}{\pi} \cdot \frac{1 - \cos n\pi}{n} = \begin{cases} \dfrac{4}{n\pi} & n \text{ lẻ} \\ 0 & n \text{ chẵn} \end{cases}$$

$$f(x) = \frac{4}{\pi}\left(\sin x + \frac{\sin 3x}{3} + \frac{\sin 5x}{5} + \dots\right)$$

Tính thử tổng riêng tại $x = \frac{\pi}{2}$ (giá trị đúng là 1):

```python
import math

# Chuỗi Fourier của sóng vuông: f(x) = (4/pi) * sum sin((2k-1)x) / (2k-1)
def partial_sum(x, terms):
    return 4 / math.pi * sum(math.sin((2 * k - 1) * x) / (2 * k - 1) for k in range(1, terms + 1))

for terms in [1, 3, 10, 100, 1000]:
    print(f"{terms:5d} so hang: f(pi/2) ~ {partial_sum(math.pi / 2, terms):.6f}")
```

**Kết quả:**

```text
    1 so hang: f(pi/2) ~ 1.273240
    3 so hang: f(pi/2) ~ 1.103474
   10 so hang: f(pi/2) ~ 0.968248
  100 so hang: f(pi/2) ~ 0.996817
 1000 so hang: f(pi/2) ~ 0.999682
```

Tổng riêng dao động quanh 1 rồi hội tụ dần. Gần điểm gián đoạn, tổng riêng luôn "vọt lố" khoảng 9% độ lớn bước nhảy, dù lấy bao nhiêu số hạng — gọi là **hiện tượng Gibbs**.

> 🎮🤖 **Ứng dụng:** chuỗi Fourier (và biến đổi Fourier) là nền tảng của **xử lý âm thanh** (bộ cân bằng, tổng hợp âm thanh trong game), **nén ảnh JPEG**, **nén âm thanh MP3**, và các kỹ thuật như *positional encoding* trong mô hình Transformer.

---

## Chương 15. Phương trình vi phân

**Phương trình vi phân (PTVP)** là phương trình chứa hàm chưa biết và đạo hàm của nó. Đây là ngôn ngữ của vật lý: định luật Newton $F = ma$ chính là một PTVP cấp hai.

- **Cấp** của PTVP = cấp cao nhất của đạo hàm.
- **Nghiệm tổng quát** chứa các hằng số tùy ý; **bài toán Cauchy** (bài toán giá trị ban đầu) cho thêm điều kiện để xác định các hằng số.

### 15.1 Phương trình cấp một

**a) Tách biến:** $y' = f(x)\,g(y)$ → $\displaystyle\int \frac{dy}{g(y)} = \int f(x)\,dx$.

**Ví dụ 15.1.** $y' = xy$, $y(0) = 1$. Tách biến: $\dfrac{dy}{y} = x\,dx$ → $\ln|y| = \dfrac{x^2}{2} + C$ → $y = C e^{x^2/2}$. Với $y(0) = 1$: $C = 1$, vậy $y = e^{x^2/2}$.

**b) Tuyến tính:** $y' + p(x)\,y = q(x)$. Nghiệm tổng quát:

$$\boxed{y = e^{-\int p\,dx}\left(\int q(x)\,e^{\int p\,dx}\,dx + C\right)}$$

(Cách nhớ: nhân hai vế với **thừa số tích phân** $\mu = e^{\int p\,dx}$, khi đó vế trái trở thành $(\mu y)'$.)

**Ví dụ 15.2.** $y' + y = x$, $y(0) = 0$. Có $p = 1$, $\mu = e^x$:

$$(e^x y)' = x e^x \Rightarrow e^x y = (x - 1)e^x + C \Rightarrow y = x - 1 + Ce^{-x}$$

Với $y(0) = 0$: $C = 1$, vậy $y = x - 1 + e^{-x}$.

**c) Bernoulli:** $y' + p(x)\,y = q(x)\,y^\alpha$ ($\alpha \ne 0, 1$). Đặt $z = y^{1 - \alpha}$ để đưa về phương trình tuyến tính.

**d) Vi phân toàn phần:** $P\,dx + Q\,dy = 0$ với $P'_y = Q'_x$. Nghiệm là $U(x, y) = C$ với $dU = P\,dx + Q\,dy$ (giống tìm hàm thế ở Chương 11.4).

**Ví dụ 15.3.** $(2xy + 1)\,dx + x^2\,dy = 0$: $P'_y = 2x = Q'_x$ ✓. Hàm $U = x^2 y + x$ → nghiệm $x^2 y + x = C$.

### 15.2 Phương trình tuyến tính cấp hai hệ số hằng

$$y'' + p\,y' + q\,y = f(x)$$

**Nghiệm tổng quát = nghiệm tổng quát của phương trình thuần nhất + một nghiệm riêng:** $y = \bar{y} + y^*$.

**Bước 1 — phương trình thuần nhất** ($f = 0$): giải **phương trình đặc trưng** $k^2 + pk + q = 0$:

| Nghiệm đặc trưng | Nghiệm tổng quát $\bar{y}$ |
|---|---|
| Hai nghiệm thực khác nhau $k_1 \ne k_2$ | $C_1 e^{k_1 x} + C_2 e^{k_2 x}$ |
| Nghiệm kép $k$ | $(C_1 + C_2 x)\,e^{kx}$ |
| Phức $k = \alpha \pm i\beta$ | $e^{\alpha x}(C_1\cos\beta x + C_2\sin\beta x)$ |

**Ví dụ 15.4.**

- $y'' - 5y' + 6y = 0$: $k^2 - 5k + 6 = 0 \Rightarrow k = 2, 3$ → $y = C_1 e^{2x} + C_2 e^{3x}$.
- $y'' + 4y' + 4y = 0$: $k = -2$ (kép) → $y = (C_1 + C_2 x)\,e^{-2x}$.
- $y'' + 2y' + 5y = 0$: $k = -1 \pm 2i$ → $y = e^{-x}(C_1\cos 2x + C_2\sin 2x)$.

**Bước 2 — nghiệm riêng bằng phương pháp hệ số bất định** (khi $f(x)$ có dạng đặc biệt):

| $f(x)$ | Dạng nghiệm riêng $y^*$ |
|---|---|
| $P_n(x)\,e^{ax}$ ($P_n$ là đa thức bậc $n$) | $x^s\,Q_n(x)\,e^{ax}$ |
| $e^{ax}\big(P(x)\cos bx + R(x)\sin bx\big)$ | $x^s\,e^{ax}\big(Q_1(x)\cos bx + Q_2(x)\sin bx\big)$ |

trong đó $s$ = **số lần** $a$ (hoặc $a + ib$) là nghiệm của phương trình đặc trưng (0, 1 hoặc 2). Hiện tượng $s > 0$ gọi là **cộng hưởng**.

**Ví dụ 15.5.** $y'' - 3y' + 2y = 4x$. Đặc trưng $k = 1, 2$; vế phải là đa thức bậc 1 với $a = 0$ (không phải nghiệm đặc trưng, $s = 0$) → thử $y^* = Ax + B$:

$$0 - 3A + 2(Ax + B) = 4x \Rightarrow 2A = 4,\ -3A + 2B = 0 \Rightarrow A = 2,\ B = 3$$

Nghiệm tổng quát: $y = C_1 e^x + C_2 e^{2x} + 2x + 3$.

**Ví dụ 15.6 (cộng hưởng).** $y'' + y = \sin x$. Đặc trưng $k = \pm i$, vế phải có $a + ib = i$ **trùng** nghiệm đặc trưng → $s = 1$, thử $y^* = x(A\cos x + B\sin x)$. Thay vào tìm được $A = -\frac{1}{2}$, $B = 0$:

$$y = C_1\cos x + C_2\sin x - \frac{x}{2}\cos x$$

Số hạng $x\cos x$ **lớn dần theo thời gian** — đó là **cộng hưởng**: đẩy xích đu đúng nhịp dao động riêng thì biên độ ngày càng lớn.

**Nguyên lý chồng chất:** nếu $f = f_1 + f_2$, tìm nghiệm riêng cho từng phần rồi cộng lại.

### 15.3 Ứng dụng

| Hiện tượng | Phương trình | Nghiệm |
|---|---|---|
| Tăng trưởng / phân rã | $y' = ky$ | $y = y_0 e^{kt}$ |
| Định luật làm nguội Newton | $T' = -k(T - T_{\text{môi trường}})$ | $T$ tiến dần về nhiệt độ môi trường theo hàm mũ |
| Lò xo không ma sát | $m y'' + k y = 0$ | Dao động điều hòa, tần số $\omega = \sqrt{k/m}$ |
| Lò xo có giảm chấn | $m y'' + c y' + k y = 0$ | Tắt dần (3 trường hợp ứng với 3 dạng nghiệm đặc trưng) |

**Ví dụ 15.7.** $y'' + y = 0$, $y(0) = 1$, $y'(0) = 0$ (lò xo kéo ra 1 đơn vị rồi thả). Nghiệm tổng quát $C_1\cos x + C_2\sin x$; điều kiện đầu cho $C_1 = 1$, $C_2 = 0$ → $y = \cos x$. Chương 18 sẽ giải chính bài toán này bằng máy tính.

> 🎮 **Lò xo có giảm chấn** là công cụ rất phổ biến trong game: camera bám theo nhân vật, hiệu ứng nảy của giao diện, hệ thống treo của xe. Chọn hệ số sao cho phương trình đặc trưng có **nghiệm kép** (giảm chấn tới hạn) sẽ cho chuyển động về đích **nhanh nhất mà không dao động**.

---

## Chương 16. Phép biến đổi Laplace

### 16.1 Ý tưởng

Biến đổi Laplace biến **phương trình vi phân** (theo $t$) thành **phương trình đại số** (theo $s$). Giải phương trình đại số rồi **biến đổi ngược** là có nghiệm.

$$F(s) = \mathcal{L}\{f(t)\} = \int_0^{\infty} e^{-st} f(t)\,dt$$

### 16.2 Bảng biến đổi cơ bản

| $f(t)$ | $F(s)$ | $f(t)$ | $F(s)$ |
|---|---|---|---|
| $1$ | $\dfrac{1}{s}$ | $\sin bt$ | $\dfrac{b}{s^2 + b^2}$ |
| $t^n$ | $\dfrac{n!}{s^{n+1}}$ | $\cos bt$ | $\dfrac{s}{s^2 + b^2}$ |
| $e^{at}$ | $\dfrac{1}{s - a}$ | $e^{at} f(t)$ | $F(s - a)$ (dịch chuyển) |

**Tính chất then chốt — biến đổi của đạo hàm:**

$$\mathcal{L}\{y'\} = sY - y(0) \qquad \mathcal{L}\{y''\} = s^2 Y - s\,y(0) - y'(0)$$

Điều kiện ban đầu được đưa **ngay vào** phương trình — không cần tìm hằng số sau.

### 16.3 Giải phương trình vi phân

**Quy trình:** (1) biến đổi Laplace hai vế; (2) giải tìm $Y(s)$; (3) tách $Y(s)$ thành **phân thức đơn giản**; (4) tra bảng để biến đổi ngược.

**Ví dụ 16.1.** $y'' + 3y' + 2y = 0$, $y(0) = 1$, $y'(0) = 0$.

$$(s^2 Y - s) + 3(sY - 1) + 2Y = 0 \Rightarrow Y(s^2 + 3s + 2) = s + 3$$

$$Y = \frac{s + 3}{(s + 1)(s + 2)} = \frac{2}{s + 1} - \frac{1}{s + 2} \Rightarrow y(t) = 2e^{-t} - e^{-2t}$$

**Ví dụ 16.2.** $y'' + y = 1$, $y(0) = 0$, $y'(0) = 0$.

$$s^2 Y + Y = \frac{1}{s} \Rightarrow Y = \frac{1}{s(s^2 + 1)} = \frac{1}{s} - \frac{s}{s^2 + 1} \Rightarrow y(t) = 1 - \cos t$$

**Ví dụ 16.3.** $y' + 2y = e^{-t}$, $y(0) = 1$.

$$sY - 1 + 2Y = \frac{1}{s + 1} \Rightarrow Y(s + 2) = \frac{s + 2}{s + 1} \Rightarrow Y = \frac{1}{s + 1} \Rightarrow y(t) = e^{-t}$$

> 🔧 Biến đổi Laplace là công cụ chuẩn trong **lý thuyết điều khiển** (hàm truyền của bộ điều khiển PID, robot, drone) và **phân tích mạch điện** — rất quan trọng với hướng kỹ sư máy tính và hệ thống nhúng.

---

## Bài tập Phần III

**Bài III.1.** Tính tổng $\displaystyle\sum_{n=1}^{\infty} \frac{1}{(n + 1)(n + 2)}$.

**Bài III.2.** Xét sự hội tụ: (a) $\displaystyle\sum \frac{n^2}{2^n}$; (b) $\displaystyle\sum \frac{(-1)^n}{\sqrt{n}}$ (hội tụ tuyệt đối hay bán hội tụ?).

**Bài III.3.** Tìm miền hội tụ của $\displaystyle\sum_{n=1}^{\infty} \frac{n\,x^n}{2^n}$.

**Bài III.4.** Giải: (a) $y' = 2xy$, $y(0) = 3$; (b) $y' - y = e^x$, $y(0) = 0$.

**Bài III.5.** Giải: (a) $y'' + 2y' + y = 0$, $y(0) = 1$, $y'(0) = 0$; (b) $y'' - 4y = 8$.

**Bài III.6.** Dùng biến đổi Laplace giải $y' + y = 1$, $y(0) = 0$.

<details>
<summary><b>Đáp án Phần III</b> (bấm để mở)</summary>

**III.1.** $\dfrac{1}{(n + 1)(n + 2)} = \dfrac{1}{n + 1} - \dfrac{1}{n + 2}$ → tổng riêng $\dfrac{1}{2} - \dfrac{1}{n + 2} \to$ **$\dfrac{1}{2}$**.

**III.2.** (a) D'Alembert: $\dfrac{(n + 1)^2}{2n^2} \to \dfrac{1}{2} < 1$ → **hội tụ** (tổng bằng 6). (b) Leibniz: $\frac{1}{\sqrt{n}}$ giảm về 0 → hội tụ; nhưng $\sum \frac{1}{\sqrt{n}}$ ($p = \frac{1}{2} \le 1$) phân kỳ → **bán hội tụ**.

**III.3.** $R = \lim \dfrac{n/2^n}{(n + 1)/2^{n+1}} = 2$. Tại $x = \pm 2$: số hạng là $n$ hoặc $(-1)^n n$, không tiến về 0 → phân kỳ. Miền hội tụ: **$(-2, 2)$**.

**III.4.** (a) $y = 3e^{x^2}$. (b) $\mu = e^{-x}$: $(e^{-x}y)' = 1$ → $y = (x + C)e^x$, $y(0) = 0$ → **$y = x e^x$**.

**III.5.** (a) $k = -1$ kép → $y = (C_1 + C_2 x)e^{-x}$; $y(0) = 1 \Rightarrow C_1 = 1$; $y'(0) = C_2 - C_1 = 0 \Rightarrow C_2 = 1$ → **$y = (1 + x)e^{-x}$**. (b) $\bar{y} = C_1 e^{2x} + C_2 e^{-2x}$; thử $y^* = A$: $-4A = 8 \Rightarrow A = -2$ → **$y = C_1 e^{2x} + C_2 e^{-2x} - 2$**.

**III.6.** $sY + Y = \dfrac{1}{s}$ → $Y = \dfrac{1}{s(s + 1)} = \dfrac{1}{s} - \dfrac{1}{s + 1}$ → **$y = 1 - e^{-t}$**.

</details>

---

# PHẦN IV — VẬN DỤNG

## Chương 17. Phương pháp số — khi máy tính làm giải tích

Trong thực tế, rất nhiều bài toán **không có công thức nghiệm** (tích phân không sơ cấp, phương trình phi tuyến, PTVP phức tạp). Máy tính giải chúng bằng **phương pháp số** — và mọi phương pháp số đều xây trên giải tích.

### 17.1 Đạo hàm số

Từ định nghĩa đạo hàm, thay giới hạn bằng một bước $h$ nhỏ:

| Công thức | Sai số | Nguồn gốc |
|---|---|---|
| Sai phân tiến $\dfrac{f(x + h) - f(x)}{h}$ | Cỡ $h$ | Taylor bậc 1 |
| Sai phân trung tâm $\dfrac{f(x + h) - f(x - h)}{2h}$ | Cỡ $h^2$ | Taylor bậc 2: các số hạng $h^2$ **triệt tiêu** nhau |

```python
import math

f, df = math.sin, math.cos      # biết đạo hàm đúng để so sánh
x = 1.0
exact = df(x)

print(f"{'h':>8} | {'sai phan tien':>14} | {'sai phan trung tam':>18}")
for p in range(1, 13, 2):
    h = 10.0 ** (-p)
    forward = (f(x + h) - f(x)) / h                # sai số cỡ h
    central = (f(x + h) - f(x - h)) / (2 * h)      # sai số cỡ h^2
    print(f"{h:8.0e} | {abs(forward - exact):14.2e} | {abs(central - exact):18.2e}")
```

**Kết quả:**

```text
       h |  sai phan tien | sai phan trung tam
   1e-01 |       4.29e-02 |           9.00e-04
   1e-03 |       4.21e-04 |           9.01e-08
   1e-05 |       4.21e-06 |           1.11e-11
   1e-07 |       4.18e-08 |           1.94e-10
   1e-09 |       5.25e-08 |           2.97e-09
   1e-11 |       1.17e-06 |           1.17e-06
```

**Hai bài học:**

1. Sai phân trung tâm chính xác hơn **rất nhiều** với cùng $h$ — đúng như Taylor dự đoán.
2. $h$ **càng nhỏ chưa chắc càng tốt**: khi $h$ quá nhỏ, phép trừ hai số gần bằng nhau bị **sai số làm tròn** của số thực máy tính (xem tài liệu kiến trúc máy tính, chuẩn IEEE 754) lấn át, nên sai số lại tăng. Với `double`, sai phân trung tâm tốt nhất khi $h$ khoảng $10^{-5}$.

### 17.2 Phương pháp Newton giải phương trình

Để giải $f(x) = 0$: tại điểm $x_n$, thay $f$ bằng **tiếp tuyến** (xấp xỉ tuyến tính, Chương 3.6) và lấy giao điểm của tiếp tuyến với trục hoành:

$$\boxed{x_{n+1} = x_n - \frac{f(x_n)}{f'(x_n)}}$$

```python
# Phương pháp Newton tìm nghiệm của f(x) = x^2 - 2, tức x = sqrt(2)
def f(x):
    return x * x - 2

def df(x):
    return 2 * x

x = 1.0
for i in range(1, 6):
    x = x - f(x) / df(x)        # x_moi = x - f(x) / f'(x)
    print(f"lan {i}: x = {x:.16f}   sai so = {abs(x - 2 ** 0.5):.1e}")
```

**Kết quả:**

```text
lan 1: x = 1.5000000000000000   sai so = 8.6e-02
lan 2: x = 1.4166666666666667   sai so = 2.5e-03
lan 3: x = 1.4142156862745099   sai so = 2.1e-06
lan 4: x = 1.4142135623746899   sai so = 1.6e-12
lan 5: x = 1.4142135623730951   sai so = 0.0e+00
```

Số chữ số đúng **tăng gấp đôi** sau mỗi bước (hội tụ bậc hai): sai số $10^{-2} \to 10^{-3} \to 10^{-6} \to 10^{-12}$. Nhược điểm: cần biết $f'$, và có thể không hội tụ nếu điểm xuất phát tồi hoặc $f'(x_n) \approx 0$.

### 17.3 Tích phân số

| Phương pháp | Ý tưởng | Sai số (với $n$ đoạn, $h = \frac{b - a}{n}$) |
|---|---|---|
| **Hình thang** | Nối các điểm bằng đoạn thẳng | Cỡ $h^2$ |
| **Simpson** ($n$ chẵn) | Nối từng bộ ba điểm bằng parabol | Cỡ $h^4$ |

$$\int_a^b f \approx \frac{h}{2}\Big[f_0 + 2(f_1 + \dots + f_{n-1}) + f_n\Big] \qquad \int_a^b f \approx \frac{h}{3}\Big[f_0 + 4(f_1 + f_3 + \dots) + 2(f_2 + f_4 + \dots) + f_n\Big]$$

```python
import math

def trapezoid(f, a, b, n):
    h = (b - a) / n
    s = (f(a) + f(b)) / 2 + sum(f(a + i * h) for i in range(1, n))
    return s * h

def simpson(f, a, b, n):          # n phải chẵn
    h = (b - a) / n
    s = f(a) + f(b)
    s += 4 * sum(f(a + i * h) for i in range(1, n, 2))
    s += 2 * sum(f(a + i * h) for i in range(2, n, 2))
    return s * h / 3

exact = 2.0                        # tích phân của sin x trên [0, pi]
print(f"{'n':>4} | {'sai so hinh thang':>18} | {'sai so Simpson':>15}")
for n in [4, 8, 16, 32]:
    et = abs(trapezoid(math.sin, 0, math.pi, n) - exact)
    es = abs(simpson(math.sin, 0, math.pi, n) - exact)
    print(f"{n:4d} | {et:18.2e} | {es:15.2e}")
```

**Kết quả:**

```text
   n |  sai so hinh thang |  sai so Simpson
   4 |           1.04e-01 |        4.56e-03
   8 |           2.58e-02 |        2.69e-04
  16 |           6.43e-03 |        1.66e-05
  32 |           1.61e-03 |        1.03e-06
```

Mỗi lần **gấp đôi** số đoạn: sai số hình thang giảm khoảng **4 lần** ($2^2$), sai số Simpson giảm khoảng **16 lần** ($2^4$) — khớp đúng lý thuyết.

---

## Chương 18. Giải tích trong game và AI

### 18.1 Mô phỏng vật lý trong game: tích phân phương trình chuyển động

Mỗi khung hình, engine vật lý phải giải PTVP $\;x'' = a(x, v)\;$ (định luật Newton) với bước thời gian $\Delta t$ cố định. Hai cách đơn giản nhất:

| Phương pháp | Công thức mỗi bước |
|---|---|
| **Euler hiện (explicit Euler)** | $x \leftarrow x + v\,\Delta t$; $\quad v \leftarrow v + a(x_{\text{cũ}})\,\Delta t$ (dùng giá trị **cũ** cho cả hai) |
| **Euler bán ẩn (semi-implicit / symplectic Euler)** | $v \leftarrow v + a(x)\,\Delta t$; $\quad x \leftarrow x + v_{\text{mới}}\,\Delta t$ |

Chỉ khác nhau ở **thứ tự cập nhật**, nhưng kết quả khác nhau rất xa. Thử với lò xo $x'' = -x$, $x(0) = 1$, $v(0) = 0$ (nghiệm đúng $x = \cos t$ ở Ví dụ 15.7; năng lượng $E = \frac{v^2 + x^2}{2}$ phải luôn bằng $0{,}5$):

```cpp
#include <cmath>
#include <cstdio>

// Lò xo: gia tốc a = -k * x (khối lượng m = 1). Nghiệm đúng: x(t) = cos(t) khi k = 1, x(0) = 1, v(0) = 0.
// Năng lượng E = (v^2 + k x^2) / 2 phải KHÔNG ĐỔI theo thời gian.
struct State {
    double x, v;
};

double energy(State s) { return 0.5 * (s.v * s.v + s.x * s.x); }

// Euler hiện (explicit Euler): dùng vận tốc CŨ để cập nhật vị trí
State explicit_euler(State s, double dt) {
    double a = -s.x;
    return {s.x + s.v * dt, s.v + a * dt};
}

// Euler bán ẩn (semi-implicit / symplectic Euler): cập nhật vận tốc TRƯỚC, rồi dùng vận tốc MỚI
State semi_implicit_euler(State s, double dt) {
    double a = -s.x;
    double v = s.v + a * dt;
    return {s.x + v * dt, v};
}

int main() {
    const double dt = 0.1;
    State e{1.0, 0.0}, se{1.0, 0.0};
    std::printf("%6s | %8s | %9s %11s | %9s %7s\n", "t", "x dung", "x Euler", "E Euler", "x ban an", "E");
    for (int step = 1; step <= 1000; ++step) {
        e = explicit_euler(e, dt);
        se = semi_implicit_euler(se, dt);
        if (step == 10 || step == 100 || step == 500 || step == 1000) {
            double t = step * dt;
            std::printf("%6.1f | %8.4f | %9.4f %11.4f | %9.4f %7.4f\n",
                        t, std::cos(t), e.x, energy(e), se.x, energy(se));
        }
    }
    std::printf("Nang luong ban dau: %.4f\n", energy(State{1.0, 0.0}));
}
```

**Kết quả:**

```text
     t |   x dung |   x Euler     E Euler |  x ban an       E
   1.0 |   0.5403 |    0.5708      0.5523 |    0.4978  0.4790
  10.0 |  -0.8391 |   -1.4088      1.3524 |   -0.8094  0.4778
  50.0 |   0.9650 |   10.9310     72.3864 |    0.9824  0.5119
 100.0 |   0.8623 |   94.2012  10479.5778 |    0.9062  0.5213
Nang luong ban dau: 0.5000
```

**Đọc kết quả:**

- **Euler hiện:** năng lượng **tăng liên tục** (đến $t = 100$ đã gấp hơn 20 000 lần ban đầu) — lò xo "tự tạo ra năng lượng" và vật văng ra xa. Trong game, đây là nguyên nhân của những vật thể **rung lắc rồi nổ tung**.
- **Euler bán ẩn:** năng lượng chỉ **dao động nhẹ quanh 0,5** và không bị trôi — vị trí bám sát nghiệm đúng.

Đây là lý do nhiều engine vật lý dùng **Euler bán ẩn** (hoặc tích phân **Verlet**) thay vì Euler hiện, dù độ phức tạp tính toán gần như giống hệt. Ngoài ra:

- **Bước thời gian cố định** giúp mô phỏng ổn định và lặp lại được — ý tưởng "fix your timestep" rất nổi tiếng trong lập trình game.
- Bước $\Delta t$ quá lớn có thể khiến mọi phương pháp **mất ổn định** (xem Bài IV.4).

### 18.2 Gradient descent — cách máy học "học"

Để tìm cực tiểu của hàm $f$ (thường là **hàm mất mát** trong AI), lặp:

$$\boxed{\vec{w} \leftarrow \vec{w} - \eta\,\nabla f(\vec{w})}$$

với $\eta$ là **tốc độ học (learning rate)**. Đây chính là "đi ngược hướng gradient" của Chương 8.5.

```python
# Cực tiểu hóa f(x, y) = (x - 3)^2 + 10 (y + 1)^2  — cực tiểu đúng tại (3, -1)
def grad(x, y):
    return 2 * (x - 3), 20 * (y + 1)

def run(lr, steps=60):
    x, y = 0.0, 0.0
    for _ in range(steps):
        gx, gy = grad(x, y)
        x -= lr * gx               # đi NGƯỢC hướng gradient
        y -= lr * gy
    return x, y

for lr in [0.01, 0.05, 0.09, 0.11]:
    x, y = run(lr)
    print(f"lr = {lr:4.2f}: x = {x:12.6f}, y = {y:14.6f}")
```

**Kết quả** (cực tiểu đúng là $(3, -1)$):

```text
lr = 0.01: x =     2.107341, y =      -0.999998
lr = 0.05: x =     2.994609, y =      -1.000000
lr = 0.09: x =     2.999980, y =      -0.999998
lr = 0.11: x =     2.999999, y =   56346.514353
```

**Phân tích:** theo trục $y$, mỗi bước nhân sai số với $(1 - 20\eta)$.

- $\eta = 0{,}01$: theo trục $x$, hệ số là $1 - 2\eta = 0{,}98$ — hội tụ **quá chậm**, sau 60 bước $x$ mới được 2,1.
- $\eta = 0{,}05$ và $0{,}09$: $|1 - 20\eta| < 1$ → hội tụ tốt.
- $\eta = 0{,}11$: $|1 - 20 \cdot 0{,}11| = 1{,}2 > 1$ → sai số theo $y$ **phóng đại** mỗi bước → **phân kỳ**.

Bài học: tốc độ học bị giới hạn bởi hướng **"dốc nhất"** (hướng có đạo hàm bậc hai lớn nhất — liên quan đến trị riêng lớn nhất của ma trận Hessian). Đây là lý do các thuật toán hiện đại như **Adam** tự điều chỉnh bước cho từng tham số.

### 18.3 Hồi quy tuyến tính và quy tắc dây chuyền

Học đường thẳng $y = wx + b$ khớp nhất với dữ liệu, bằng cách cực tiểu hóa **sai số bình phương trung bình**:

$$L(w, b) = \frac{1}{N}\sum_{i=1}^{N}(wx_i + b - y_i)^2$$

Theo **quy tắc dây chuyền**: $\dfrac{\partial L}{\partial w} = \dfrac{1}{N}\sum 2(wx_i + b - y_i)\cdot x_i$ và $\dfrac{\partial L}{\partial b} = \dfrac{1}{N}\sum 2(wx_i + b - y_i)$.

```python
# Học đường thẳng y = w*x + b từ dữ liệu bằng gradient descent
xs = [0.0, 1.0, 2.0, 3.0, 4.0]
ys = [1.0, 3.1, 4.9, 7.2, 8.8]          # dữ liệu gần với y = 2x + 1

def loss(w, b):                          # sai số bình phương trung bình (MSE)
    return sum((w * x + b - y) ** 2 for x, y in zip(xs, ys)) / len(xs)

def grad(w, b):                          # đạo hàm riêng theo w và b (quy tắc dây chuyền)
    n = len(xs)
    dw = sum(2 * (w * x + b - y) * x for x, y in zip(xs, ys)) / n
    db = sum(2 * (w * x + b - y) for x, y in zip(xs, ys)) / n
    return dw, db

# Kiểm tra gradient bằng sai phân trung tâm (gradient check)
w, b, h = 0.5, 0.5, 1e-6
dw, db = grad(w, b)
dw_num = (loss(w + h, b) - loss(w - h, b)) / (2 * h)
db_num = (loss(w, b + h) - loss(w, b - h)) / (2 * h)
print(f"gradient check: dw = {dw:.6f} (so: {dw_num:.6f}), db = {db:.6f} (so: {db_num:.6f})")

w, b, lr = 0.0, 0.0, 0.05
for step in range(1, 1001):
    dw, db = grad(w, b)
    w, b = w - lr * dw, b - lr * db
    if step in (1, 10, 100, 1000):
        print(f"buoc {step:4d}: w = {w:.4f}, b = {b:.4f}, loss = {loss(w, b):.5f}")
```

**Kết quả:**

```text
gradient check: dw = -19.880000 (so: -19.880000), db = -7.000000 (so: -7.000000)
buoc    1: w = 1.3940, b = 0.5000, loss = 3.61270
buoc   10: w = 2.0551, b = 0.8173, loss = 0.03794
buoc  100: w = 1.9756, b = 1.0441, loss = 0.01828
buoc 1000: w = 1.9700, b = 1.0600, loss = 0.01820
```

**Nhận xét:**

- **Kiểm tra gradient (gradient check):** so gradient tính bằng công thức với gradient tính bằng **sai phân trung tâm** (Chương 17.1). Nếu khớp, công thức đạo hàm của bạn đúng — kỹ thuật chuẩn khi tự cài đặt mạng nơ-ron.
- Kết quả hội tụ về $w = 1{,}97$, $b = 1{,}06$ — trùng với nghiệm chính xác của bài toán bình phương tối thiểu (có thể tính bằng cách giải $\frac{\partial L}{\partial w} = \frac{\partial L}{\partial b} = 0$, hoặc bằng phép chiếu trong đại số tuyến tính).
- **Lan truyền ngược (backpropagation)** trong mạng nơ-ron nhiều lớp chính là áp dụng **quy tắc dây chuyền** lặp đi lặp lại từ đầu ra ngược về đầu vào — không có gì hơn thế.

### 18.4 Những chỗ khác giải tích xuất hiện

| Lĩnh vực | Khái niệm giải tích |
|---|---|
| **Hoạt họa, easing** | Hàm $\text{smoothstep}(t) = 3t^2 - 2t^3$ có đạo hàm bằng 0 tại $t = 0$ và $t = 1$ → chuyển động **bắt đầu và kết thúc mượt** |
| **Đường cong Bézier, spline** | Đa thức tham số; đạo hàm cho **hướng tiếp tuyến** (hướng di chuyển, hướng quay của camera) |
| **Chiếu sáng, đồ họa** | Pháp tuyến từ gradient (Chương 9); phương trình kết xuất (rendering equation) là **tích phân** trên bán cầu, được tính gần đúng bằng phương pháp Monte Carlo |
| **Xác suất cho AI** | Hàm mật độ, kỳ vọng là **tích phân**; phân phối chuẩn dùng tích phân Gauss (Ví dụ 10.5) |
| **Tối ưu hóa** | Gradient, Hessian, nhân tử Lagrange (Chương 8) |
| **Âm thanh, tín hiệu** | Chuỗi và biến đổi Fourier (Chương 14) |
| **Điều khiển, robot** | PTVP và biến đổi Laplace (Chương 15, 16) |

---

## Bài tập Phần IV

**Bài IV.1.** Viết công thức Newton để tính $\sqrt[3]{10}$ và thực hiện 3 bước từ $x_0 = 2$.

**Bài IV.2.** Tính gần đúng $\displaystyle\int_0^1 x^2 dx$ bằng phương pháp hình thang và Simpson với $n = 2$. So sánh với giá trị đúng và giải thích kết quả của Simpson.

**Bài IV.3.** Áp dụng gradient descent cho $f(x) = x^2$. Công thức cập nhật là gì? Điều gì xảy ra với $\eta = 0{,}5$; $\eta = 1$; $\eta = 1{,}1$? Với $\eta$ nào thì thuật toán hội tụ?

**Bài IV.4.** Dùng Euler hiện giải $y' = -2y$, $y(0) = 1$ (nghiệm đúng $e^{-2t}$ luôn giảm về 0). Chứng minh rằng với bước $\Delta t$ thì $y_{n+1} = (1 - 2\Delta t)\,y_n$, và tìm điều kiện của $\Delta t$ để nghiệm số không bị "nổ". Điều gì xảy ra khi $\Delta t = 1{,}5$?

**Bài IV.5.** Vì sao trong Chương 17.1, sai số của sai phân tiến ở $h = 10^{-11}$ lại lớn hơn ở $h = 10^{-7}$?

<details>
<summary><b>Đáp án Phần IV</b> (bấm để mở)</summary>

**IV.1.** $f(x) = x^3 - 10$ → $x_{n+1} = x_n - \dfrac{x_n^3 - 10}{3x_n^2}$. Từ $x_0 = 2$: $x_1 \approx 2{,}1666667$; $x_2 \approx 2{,}1545036$; $x_3 \approx 2{,}1544347$ (giá trị đúng $\sqrt[3]{10} = 2{,}15443469\ldots$, sai số sau 3 bước chỉ khoảng $2 \times 10^{-9}$).

**IV.2.** $h = 0{,}5$. Hình thang: $0{,}5 \times \left(\frac{0}{2} + 0{,}25 + \frac{1}{2}\right) = 0{,}375$. Simpson: $\frac{0{,}5}{3}(0 + 4 \times 0{,}25 + 1) = \frac{1}{3}$ — **chính xác tuyệt đối**, vì Simpson xấp xỉ bằng parabol mà hàm $x^2$ chính là parabol (thực tế Simpson còn chính xác với mọi đa thức bậc 3).

**IV.3.** $f'(x) = 2x$ → $x \leftarrow x - 2\eta x = (1 - 2\eta)\,x$.

- $\eta = 0{,}5$: hệ số bằng 0 → tới **đúng cực tiểu sau 1 bước**.
- $\eta = 1$: hệ số $-1$ → $x$ **nhảy qua lại** giữa $x_0$ và $-x_0$ mãi mãi.
- $\eta = 1{,}1$: hệ số $-1{,}2$ → **phân kỳ**.

Hội tụ $\iff |1 - 2\eta| < 1 \iff 0 < \eta < 1$.

**IV.4.** $y_{n+1} = y_n + \Delta t \cdot (-2y_n) = (1 - 2\Delta t)\,y_n$, nên $y_n = (1 - 2\Delta t)^n$. Không "nổ" $\iff |1 - 2\Delta t| \le 1 \iff 0 < \Delta t \le 1$ (và để không dao động đổi dấu thì cần $\Delta t < 0{,}5$). Với $\Delta t = 1{,}5$: $y_n = (-2)^n$ → giá trị đổi dấu và **tăng vọt** — trong khi nghiệm thật giảm về 0. Đây là hiện tượng **mất ổn định số** mà engine game gặp khi bước thời gian quá lớn (ví dụ khi khung hình bị giật).

**IV.5.** Với $h$ rất nhỏ, $f(x + h)$ và $f(x)$ gần như bằng nhau; hiệu của chúng chỉ còn vài chữ số có nghĩa, và sai số làm tròn (cỡ $10^{-16}$) bị **chia cho $h$** nên bị khuếch đại lên cỡ $10^{-16}/h$. Khi $h$ đủ nhỏ, sai số làm tròn này lớn hơn sai số do công thức — nên tổng sai số tăng trở lại.

</details>

---

# PHỤ LỤC

## Phụ lục A. Bảng tra nhanh

### A.1 VCB tương đương khi $u \to 0$

$\sin u \sim \tan u \sim \arcsin u \sim \arctan u \sim \ln(1 + u) \sim e^u - 1 \sim u$

$1 - \cos u \sim \dfrac{u^2}{2}, \qquad a^u - 1 \sim u\ln a, \qquad (1 + u)^\alpha - 1 \sim \alpha u$

### A.2 Khai triển Maclaurin

| Hàm | Khai triển | Hội tụ khi |
|---|---|---|
| $e^x$ | $\sum \dfrac{x^n}{n!}$ | mọi $x$ |
| $\sin x$ | $\sum (-1)^n \dfrac{x^{2n+1}}{(2n+1)!}$ | mọi $x$ |
| $\cos x$ | $\sum (-1)^n \dfrac{x^{2n}}{(2n)!}$ | mọi $x$ |
| $\dfrac{1}{1 - x}$ | $\sum x^n$ | $\lvert x\rvert < 1$ |
| $\ln(1 + x)$ | $\sum (-1)^{n+1}\dfrac{x^n}{n}$ | $-1 < x \le 1$ |
| $\arctan x$ | $\sum (-1)^n \dfrac{x^{2n+1}}{2n + 1}$ | $\lvert x\rvert \le 1$ |

### A.3 Đổi biến trong tích phân bội

| Hệ tọa độ | Yếu tố thể tích / diện tích |
|---|---|
| Cực | $dx\,dy = r\,dr\,d\varphi$ |
| Trụ | $dV = r\,dr\,d\varphi\,dz$ |
| Cầu | $dV = \rho^2\sin\theta\,d\rho\,d\theta\,d\varphi$ |
| Tổng quát | nhân với $\lvert J \rvert$ (định thức Jacobi) |

### A.4 Chọn tiêu chuẩn hội tụ cho chuỗi số dương

| Số hạng có dạng | Dùng |
|---|---|
| Phân thức của $n$ | So sánh với $\sum \frac{1}{n^p}$ |
| Có $n!$ hoặc $a^n$ | D'Alembert |
| $(\dots)^n$ | Cauchy |
| Có $\ln n$ | Tích phân |
| Số hạng không tiến về 0 | Điều kiện cần → phân kỳ |

## Phụ lục B. Lỗi hay gặp

1. Thay VCB tương đương trong **tổng/hiệu** (Chương 1.4).
2. Dùng L'Hôpital khi **không phải** dạng $\frac{0}{0}$ hoặc $\frac{\infty}{\infty}$.
3. Quên hằng số $C$ trong nguyên hàm; quên **đổi cận** khi đổi biến tích phân xác định.
4. Tính tích phân xác định qua điểm hàm không xác định mà không nhận ra đó là **tích phân suy rộng** (ví dụ $\int_{-1}^1 \frac{dx}{x^2}$ **phân kỳ**, không phải bằng $-2$).
5. Quên $r$ khi đổi sang tọa độ cực, quên $\rho^2\sin\theta$ khi đổi sang tọa độ cầu.
6. Không vẽ miền trước khi đặt cận tích phân bội.
7. Kết luận chuỗi hội tụ chỉ vì $u_n \to 0$.
8. Quên xét **hai đầu mút** khi tìm miền hội tụ chuỗi lũy thừa.
9. Quên hệ số $x^s$ khi vế phải trùng nghiệm đặc trưng (cộng hưởng).
10. Chứng minh giới hạn hai biến tồn tại chỉ bằng **một** đường đi.
11. Nhầm $\nabla f$ (hướng **tăng**) với hướng cần đi khi **cực tiểu hóa** (phải là $-\nabla f$).

## Phụ lục C. Kiểm tra bài làm bằng SymPy

Sau khi **tự làm tay**, dùng thư viện **SymPy** để kiểm tra (`pip install sympy`):

```python
from sympy import (symbols, Function, Eq, sin, cos, exp, log, oo, pi,
                   limit, diff, integrate, series, summation, dsolve,
                   laplace_transform, hessian)

x, y, n, t, s = symbols('x y n t s')

print(limit((sin(x) - x) / x**3, x, 0))            # giới hạn
print(diff(x**2 * sin(x), x))                       # đạo hàm
print(integrate(x * exp(x), x))                     # nguyên hàm (không có + C)
print(integrate(sin(x), (x, 0, pi)))                # tích phân xác định
print(integrate(1 / x**2, (x, 1, oo)))              # tích phân suy rộng
print(series(exp(x) * cos(x), x, 0, 4))             # khai triển Taylor
print(summation(1 / n**2, (n, 1, oo)))              # tổng chuỗi
print(hessian(x**3 + y**3 - 3*x*y, (x, y)))         # ma trận Hessian

f = Function('f')
print(dsolve(Eq(f(x).diff(x, 2) - 3*f(x).diff(x) + 2*f(x), 4*x), f(x)))   # PTVP
print(laplace_transform(sin(2*t), t, s, noconds=True))                      # Laplace
```

**Kết quả:**

```text
-1/6
x**2*cos(x) + 2*x*sin(x)
(x - 1)*exp(x)
2
1
1 + x - x**3/3 + O(x**4)
pi**2/6
Matrix([[6*x, -3], [-3, 6*y]])
Eq(f(x), C1*exp(x) + C2*exp(2*x) + 2*x + 3)
2/(s**2 + 4)
```

> Công cụ trực tuyến như **WolframAlpha** cũng rất hữu ích, nhưng hãy dùng chúng để **kiểm tra**, không phải để **làm thay** — kỹ năng biến đổi bằng tay mới là thứ được đánh giá trong kỳ thi và giúp bạn hiểu bản chất.

## Phụ lục D. Lộ trình gợi ý

| Giai đoạn | Nội dung | Mục tiêu tối thiểu |
|---|---|---|
| Tuần 1–2 | Chương 1–3 | Tính giới hạn bằng VCB tương đương; đạo hàm thành thạo mọi dạng |
| Tuần 3 | Chương 4–5 | Khai triển Taylor; khảo sát hàm; bài toán tối ưu |
| Tuần 4–5 | Chương 6–7 | Nguyên hàm bằng đổi biến, từng phần, phân thức; tích phân suy rộng |
| Tuần 6 | Chương 8 | Đạo hàm riêng, gradient, cực trị tự do và có điều kiện |
| Tuần 7–8 | Chương 9–10 | Tích phân kép, bội ba; đổi thứ tự; tọa độ cực, trụ, cầu |
| Tuần 9 | Chương 11–12 | Tích phân đường, Green; tích phân mặt, Gauss |
| Tuần 10 | Chương 13–14 | Xét hội tụ chuỗi; miền hội tụ chuỗi lũy thừa |
| Tuần 11–12 | Chương 15–16 | PTVP cấp 1, cấp 2; Laplace |
| Song song | Chương 17–18 | Chạy lại và sửa đổi các chương trình; viết thêm mô phỏng của riêng bạn |

## Phụ lục E. Tài liệu nên dùng song song

- **Bài giảng, đề cương bài tập và đề thi các năm** của Khoa Toán – Tin, ĐH Bách khoa Hà Nội — nguồn chính thức, bám sát kỳ thi.
- **3Blue1Brown – "Essence of Calculus"** (YouTube) — giúp "nhìn thấy" đạo hàm, tích phân, Taylor. Rất nên xem trước khi học mỗi chương.
- **"Calculus"** (James Stewart) — giáo trình tiếng Anh phổ biến, nhiều ví dụ và hình vẽ.
- **"Calculus"** (Gilbert Strang, miễn phí trên MIT OpenCourseWare) cùng các khóa **MIT 18.01, 18.02, 18.03** (giải tích một biến, nhiều biến, phương trình vi phân).
- **Khan Academy** — ôn lại phần nền tảng nếu thấy hổng kiến thức.
- **"Mathematics for Machine Learning"** (Deisenroth, Faisal, Ong — bản PDF miễn phí) — cầu nối giải tích nhiều biến, đại số tuyến tính với AI.
- **Bài viết "Integration Basics" và "Fix Your Timestep!"** (Glenn Fiedler, blog Gaffer On Games) — tích phân số trong lập trình game.

---

*Chúc bạn học tốt! Giải tích là ngôn ngữ của sự **thay đổi** — và mọi thứ chuyển động trong game hay "học" trong AI đều nói bằng ngôn ngữ đó.*
