# ĐẠI SỐ TUYẾN TÍNH — NỀN TẢNG VỮNG CHẮC

*Tài liệu tự học bám theo cấu trúc học phần **Đại số (MI1141)** của Đại học Bách khoa Hà Nội*

---

## 0. Cách dùng tài liệu này

### 0.1 Cấu trúc

Tài liệu đi theo đúng thứ tự 5 chương của học phần Đại số ở Bách khoa:

| Chương | Nội dung | Vai trò |
|---|---|---|
| 1 | Tập hợp – Logic – Ánh xạ – Số phức | Ngôn ngữ nền để nói chuyện toán học |
| 2 | Ma trận – Định thức – Hệ phương trình tuyến tính | **Công cụ tính toán** dùng xuyên suốt |
| 3 | Không gian véc tơ | **Khung lý thuyết** (cơ sở, số chiều, tọa độ) |
| 4 | Ánh xạ tuyến tính – Trị riêng – Chéo hóa | Nối lý thuyết với ma trận |
| 5 | Không gian Euclide – Dạng toàn phương | Đo độ dài, góc; phân loại dạng toàn phương |

Mỗi mục đều theo mạch: **ý tưởng trực quan → định nghĩa → ví dụ giải chi tiết → dạng bài hay gặp**. Cuối mỗi chương có **bài tập tự luyện kèm đáp án**.

> **Lưu ý:** Đây là tài liệu tự học, không thay thế giáo trình và slide chính thức của Khoa Toán – Tin. Đề cương có thể khác nhau đôi chút giữa các nhóm ngành (MI1141, MI1142, MI1143...), nên hãy đối chiếu với đề cương lớp bạn.

### 0.2 Ký hiệu dùng trong tài liệu

| Ký hiệu | Ý nghĩa |
|---|---|
| $\mathbb{R}, \mathbb{C}$ | Tập số thực, tập số phức |
| $\mathbb{R}^n$ | Không gian các bộ $n$ số thực $(x_1, \dots, x_n)$ |
| $M_{m \times n}$ | Tập ma trận $m$ hàng, $n$ cột |
| $I$ (giáo trình BK hay viết $E$) | Ma trận đơn vị |
| $A^T$ | Ma trận chuyển vị |
| $\det A$ hoặc $\lvert A \rvert$ | Định thức của $A$ |
| $r(A)$ | Hạng của ma trận $A$ |
| $P_n[x]$ | Không gian các đa thức bậc **không quá** $n$ |
| ĐLTT / PTTT | Độc lập tuyến tính / Phụ thuộc tuyến tính |
| $\text{span}\{\dots\}$ | Không gian con sinh bởi một hệ véc tơ |
| $[x]_B$ | Tọa độ của véc tơ $x$ trong cơ sở $B$ |
| $\text{Ker} f, \text{Im} f$ | Hạt nhân, ảnh của ánh xạ $f$ |

### 0.3 Cách học hiệu quả

1. **Đọc ví dụ, che lời giải, tự làm lại.** Đại số tuyến tính học bằng tay, không học bằng mắt.
2. **Làm bài tập trước khi xem đáp án.** Sai cũng được, quan trọng là biết mình sai ở bước nào.
3. **Kiểm tra bằng máy sau khi làm tay** (xem Phụ lục B). Bạn biết lập trình, hãy tận dụng điều đó.
4. **Đừng bỏ qua Chương 2.** Gần như mọi bài ở Chương 3, 4, 5 cuối cùng đều quy về: tính định thức, tìm hạng, hoặc giải hệ phương trình.

---

## CHƯƠNG 1. TẬP HỢP – LOGIC – ÁNH XẠ – SỐ PHỨC

### 1.1 Logic mệnh đề

**Mệnh đề** là một câu khẳng định có giá trị **đúng (1)** hoặc **sai (0)**, không thể vừa đúng vừa sai.

- "2 + 3 = 5" là mệnh đề đúng.
- "Hà Nội là thủ đô của Pháp" là mệnh đề sai.
- "Hôm nay trời đẹp quá!" không phải mệnh đề (không xác định đúng/sai).

#### Các phép toán logic

| Phép toán | Ký hiệu | Đọc là | Đúng khi |
|---|---|---|---|
| Phủ định | $\overline{p}$ hoặc $\neg p$ | không $p$ | $p$ sai |
| Hội | $p \land q$ | $p$ và $q$ | cả hai đều đúng |
| Tuyển | $p \lor q$ | $p$ hoặc $q$ | ít nhất một cái đúng |
| Kéo theo | $p \to q$ | nếu $p$ thì $q$ | **chỉ sai khi $p$ đúng mà $q$ sai** |
| Tương đương | $p \leftrightarrow q$ | $p$ khi và chỉ khi $q$ | $p$, $q$ cùng giá trị |

**Bảng chân trị:**

| $p$ | $q$ | $\neg p$ | $p \land q$ | $p \lor q$ | $p \to q$ | $p \leftrightarrow q$ |
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| 1 | 1 | 0 | 1 | 1 | 1 | 1 |
| 1 | 0 | 0 | 0 | 1 | **0** | 0 |
| 0 | 1 | 1 | 0 | 1 | 1 | 0 |
| 0 | 0 | 1 | 0 | 0 | 1 | 1 |

**Hiểu phép kéo theo bằng lời hứa:** "Nếu trời mưa ($p$) thì tôi mang ô ($q$)". Lời hứa chỉ bị **phá vỡ** khi trời mưa mà tôi không mang ô. Nếu trời không mưa, tôi mang hay không mang ô đều không sai lời hứa. Đó là lý do hai dòng cuối của cột $p \to q$ đều bằng 1.

#### Các công thức tương đương cần thuộc

$$p \to q \equiv \neg p \lor q$$

$$p \to q \equiv \neg q \to \neg p \quad \text{(phản đảo — dùng để chứng minh gián tiếp)}$$

$$\neg(p \land q) \equiv \neg p \lor \neg q, \qquad \neg(p \lor q) \equiv \neg p \land \neg q \quad \text{(De Morgan)}$$

$$\neg(p \to q) \equiv p \land \neg q$$

**Ví dụ 1.1.** Chứng minh $p \to q \equiv \neg p \lor q$.

*Lời giải.* Lập bảng chân trị:

| $p$ | $q$ | $\neg p$ | $\neg p \lor q$ | $p \to q$ |
|:-:|:-:|:-:|:-:|:-:|
| 1 | 1 | 0 | 1 | 1 |
| 1 | 0 | 0 | 0 | 0 |
| 0 | 1 | 1 | 1 | 1 |
| 0 | 0 | 1 | 1 | 1 |

Hai cột cuối giống hệt nhau ở mọi dòng, nên hai mệnh đề tương đương. $\blacksquare$

#### Lượng từ

- $\forall x$: "với mọi $x$"
- $\exists x$: "tồn tại $x$"

**Quy tắc phủ định lượng từ** (rất hay thi trắc nghiệm): đổi $\forall \leftrightarrow \exists$ và phủ định phần sau.

$$\neg\big(\forall x, P(x)\big) \equiv \exists x, \neg P(x) \qquad \neg\big(\exists x, P(x)\big) \equiv \forall x, \neg P(x)$$

*Ví dụ:* Phủ định của "$\forall x \in \mathbb{R}, x^2 \ge 0$" là "$\exists x \in \mathbb{R}, x^2 < 0$".

> 💻 **Liên hệ lập trình:** `!(a && b)` tương đương `!a || !b` — chính là De Morgan. Bạn sẽ dùng điều này khi rút gọn điều kiện `if`.

---

### 1.2 Tập hợp

#### Các phép toán

- **Hợp:** $A \cup B = \{x \mid x \in A \lor x \in B\}$
- **Giao:** $A \cap B = \{x \mid x \in A \land x \in B\}$
- **Hiệu:** $A \setminus B = \{x \mid x \in A \land x \notin B\}$
- **Phần bù** (trong tập $X$): $\overline{A} = X \setminus A$
- **Tích Descartes:** $A \times B = \{(a, b) \mid a \in A, b \in B\}$

**Ví dụ 1.2.** Cho $A = \{1, 2, 3\}$, $B = \{2, 3, 4\}$. Khi đó:

- $A \cup B = \{1, 2, 3, 4\}$
- $A \cap B = \{2, 3\}$
- $A \setminus B = \{1\}$, $B \setminus A = \{4\}$
- $A \times B$ có $3 \times 3 = 9$ phần tử, ví dụ $(1, 2), (1, 3), \dots$

**Luật De Morgan cho tập hợp:**

$$\overline{A \cup B} = \overline{A} \cap \overline{B}, \qquad \overline{A \cap B} = \overline{A} \cup \overline{B}$$

#### Cách chứng minh đẳng thức tập hợp

Phương pháp chuẩn: chứng minh "$x$ thuộc vế trái $\Leftrightarrow$ $x$ thuộc vế phải", dùng các công thức logic ở mục 1.1.

**Ví dụ 1.3.** Chứng minh $A \setminus (B \cup C) = (A \setminus B) \cap (A \setminus C)$.

*Lời giải.*

$$
\begin{aligned}
x \in A \setminus (B \cup C)
&\Leftrightarrow x \in A \land \neg(x \in B \lor x \in C) \\
&\Leftrightarrow x \in A \land (x \notin B \land x \notin C) \quad \text{(De Morgan)}\\
&\Leftrightarrow (x \in A \land x \notin B) \land (x \in A \land x \notin C) \\
&\Leftrightarrow x \in (A \setminus B) \cap (A \setminus C). \quad \blacksquare
\end{aligned}
$$

---

### 1.3 Ánh xạ

**Định nghĩa.** Ánh xạ $f: X \to Y$ là một quy tắc cho mỗi phần tử $x \in X$ tương ứng với **đúng một** phần tử $f(x) \in Y$.

- **Ảnh** của tập $A \subset X$: $f(A) = \{f(x) \mid x \in A\}$
- **Nghịch ảnh** của tập $B \subset Y$: $f^{-1}(B) = \{x \in X \mid f(x) \in B\}$

#### Ba loại ánh xạ quan trọng

| Loại | Định nghĩa | Hiểu nôm na |
|---|---|---|
| **Đơn ánh** | $f(x_1) = f(x_2) \Rightarrow x_1 = x_2$ | Không có hai đầu vào nào chung đầu ra |
| **Toàn ánh** | $\forall y \in Y, \exists x \in X: f(x) = y$ | Mọi phần tử của $Y$ đều "được trúng" |
| **Song ánh** | Vừa đơn ánh vừa toàn ánh | Ghép cặp 1–1 hoàn hảo → có **ánh xạ ngược** $f^{-1}$ |

**Ví dụ 1.4.** Xét $f: \mathbb{R} \to \mathbb{R}$, $f(x) = x^2$.

- Không đơn ánh vì $f(-1) = f(1) = 1$ mà $-1 \ne 1$.
- Không toàn ánh vì $y = -1$ không có $x$ nào để $x^2 = -1$.
- Nghịch ảnh: $f^{-1}([1, 4]) = \{x \mid 1 \le x^2 \le 4\} = [-2, -1] \cup [1, 2]$.

**Ví dụ 1.5.** Xét $g: \mathbb{R} \to \mathbb{R}$, $g(x) = 2x + 1$. Chứng minh $g$ là song ánh và tìm $g^{-1}$.

*Lời giải.*

- *Đơn ánh:* $g(x_1) = g(x_2) \Rightarrow 2x_1 + 1 = 2x_2 + 1 \Rightarrow x_1 = x_2$.
- *Toàn ánh:* Với mọi $y$, chọn $x = \dfrac{y - 1}{2}$ thì $g(x) = y$.

Vậy $g$ là song ánh, và $g^{-1}(y) = \dfrac{y - 1}{2}$. $\blacksquare$

**Hợp thành** của $f: X \to Y$ và $g: Y \to Z$ là $g \circ f: X \to Z$, $(g \circ f)(x) = g(f(x))$. Chú ý thứ tự: **$f$ làm trước, $g$ làm sau**.

> 💻 **Liên hệ lập trình:** Hàm băm (hash) không phải đơn ánh — đó là lý do có "va chạm" (collision). Khi mã hóa/giải mã dữ liệu, ta cần song ánh để giải mã được đúng.

---

### 1.4 Cấu trúc đại số (đọc để hiểu, ít khi ra bài khó)

Cho tập $G$ với một phép toán $*$. $(G, *)$ là **nhóm** nếu:

1. **Kết hợp:** $(a * b) * c = a * (b * c)$
2. **Có phần tử trung hòa** $e$: $a * e = e * a = a$
3. **Mọi phần tử đều có nghịch đảo** $a'$: $a * a' = a' * a = e$

Nếu thêm **giao hoán** ($a * b = b * a$) thì gọi là **nhóm Abel**.

- $(\mathbb{Z}, +)$ là nhóm Abel: trung hòa là 0, nghịch đảo của $a$ là $-a$.
- $(\mathbb{Z}, \cdot)$ **không** là nhóm: số 2 không có nghịch đảo trong $\mathbb{Z}$ (vì $1/2 \notin \mathbb{Z}$).

**Vành** là tập có hai phép toán $+$ và $\cdot$, trong đó $(X, +)$ là nhóm Abel, phép nhân kết hợp và phân phối với phép cộng.

**Trường** là vành giao hoán có đơn vị, trong đó **mọi phần tử khác 0 đều có nghịch đảo** với phép nhân. Ví dụ: $\mathbb{Q}, \mathbb{R}, \mathbb{C}$ là trường; $\mathbb{Z}$ không phải trường.

> Vì sao cần biết? Không gian véc tơ ở Chương 3 được định nghĩa "trên một trường" — thường là $\mathbb{R}$.

---

### 1.5 Số phức

#### Dạng đại số

$$z = a + bi, \quad a, b \in \mathbb{R}, \quad i^2 = -1$$

- Phần thực $\text{Re}\, z = a$, phần ảo $\text{Im}\, z = b$
- Số phức liên hợp: $\overline{z} = a - bi$
- Mô đun: $|z| = \sqrt{a^2 + b^2}$, và $z \cdot \overline{z} = |z|^2$

**Phép toán:** cộng/trừ/nhân như đa thức, thay $i^2 = -1$. **Phép chia:** nhân cả tử và mẫu với liên hợp của mẫu.

**Ví dụ 1.6.** Tính $(2 + 3i)(1 - i)$ và $\dfrac{2 + 3i}{1 - i}$.

*Lời giải.*

$$(2 + 3i)(1 - i) = 2 - 2i + 3i - 3i^2 = 2 + i + 3 = 5 + i$$

$$\frac{2 + 3i}{1 - i} = \frac{(2 + 3i)(1 + i)}{(1 - i)(1 + i)} = \frac{2 + 2i + 3i + 3i^2}{1 + 1} = \frac{-1 + 5i}{2} = -\frac{1}{2} + \frac{5}{2}i$$

#### Dạng lượng giác — trọng tâm của phần số phức

Biểu diễn $z = a + bi$ là điểm $(a, b)$ trên mặt phẳng. Khi đó:

$$z = r(\cos\varphi + i \sin\varphi), \quad r = |z|, \quad \cos\varphi = \frac{a}{r}, \quad \sin\varphi = \frac{b}{r}$$

$\varphi$ gọi là **argument** của $z$.

**Quy tắc nhân và chia:** với $z_1 = r_1(\cos\varphi_1 + i\sin\varphi_1)$, $z_2 = r_2(\cos\varphi_2 + i\sin\varphi_2)$:

$$z_1 z_2 = r_1 r_2 \big[\cos(\varphi_1 + \varphi_2) + i \sin(\varphi_1 + \varphi_2)\big]$$

$$\frac{z_1}{z_2} = \frac{r_1}{r_2} \big[\cos(\varphi_1 - \varphi_2) + i \sin(\varphi_1 - \varphi_2)\big]$$

Nói gọn: **nhân thì mô đun nhân, góc cộng**.

**Công thức Moivre:**

$$z^n = r^n (\cos n\varphi + i \sin n\varphi)$$

**Ví dụ 1.7.** Tính $(1 + i\sqrt{3})^6$.

*Lời giải.* $r = \sqrt{1 + 3} = 2$; $\cos\varphi = \frac{1}{2}$, $\sin\varphi = \frac{\sqrt{3}}{2}$ nên $\varphi = \frac{\pi}{3}$.

$$(1 + i\sqrt{3})^6 = 2^6 \left(\cos\frac{6\pi}{3} + i\sin\frac{6\pi}{3}\right) = 64(\cos 2\pi + i \sin 2\pi) = 64$$

Nếu khai triển nhị thức bậc 6 thì rất dài — đây là sức mạnh của dạng lượng giác.

#### Căn bậc $n$ của số phức

Số phức $z = r(\cos\varphi + i\sin\varphi) \ne 0$ có **đúng $n$ căn bậc $n$**:

$$w_k = \sqrt[n]{r}\left(\cos\frac{\varphi + 2k\pi}{n} + i \sin\frac{\varphi + 2k\pi}{n}\right), \quad k = 0, 1, \dots, n - 1$$

Về hình học, $n$ căn này là $n$ đỉnh của một **đa giác đều** nội tiếp đường tròn bán kính $\sqrt[n]{r}$.

**Ví dụ 1.8.** Tìm các căn bậc 3 của $-8$.

*Lời giải.* $-8 = 8(\cos\pi + i\sin\pi)$. Các căn bậc 3:

$$w_k = 2\left(\cos\frac{\pi + 2k\pi}{3} + i\sin\frac{\pi + 2k\pi}{3}\right), \quad k = 0, 1, 2$$

- $k = 0$: $w_0 = 2\left(\cos\frac{\pi}{3} + i \sin\frac{\pi}{3}\right) = 1 + i\sqrt{3}$
- $k = 1$: $w_1 = 2(\cos\pi + i\sin\pi) = -2$
- $k = 2$: $w_2 = 2\left(\cos\frac{5\pi}{3} + i\sin\frac{5\pi}{3}\right) = 1 - i\sqrt{3}$

#### Phương trình bậc hai trong $\mathbb{C}$

Trong $\mathbb{C}$, phương trình bậc hai **luôn có nghiệm**, kể cả khi $\Delta < 0$.

**Ví dụ 1.9.** Giải $z^2 + 2z + 5 = 0$.

*Lời giải.* $\Delta' = 1 - 5 = -4 = (2i)^2$. Vậy $z = -1 \pm 2i$.

**Định lý cơ bản của đại số:** Mọi đa thức bậc $n \ge 1$ với hệ số phức có **đúng $n$ nghiệm phức** (kể cả bội). Định lý này sẽ quay lại ở Chương 4 khi tìm trị riêng.

**Dạng mũ (Euler):** $e^{i\varphi} = \cos\varphi + i\sin\varphi$, nên $z = re^{i\varphi}$.

> 🎮 **Liên hệ game:** Nhân một số phức với $\cos\theta + i\sin\theta$ (mô đun 1) chính là **quay điểm đó một góc $\theta$** quanh gốc tọa độ. Trong 3D, ý tưởng này được mở rộng thành **quaternion** — công cụ xoay vật thể chuẩn trong Unity và Unreal.

#### 📝 Dạng bài hay gặp (Chương 1)

- Kiểm tra hai mệnh đề tương đương; phủ định mệnh đề có lượng từ.
- Chứng minh đẳng thức tập hợp; tìm ảnh, nghịch ảnh.
- Xét đơn ánh, toàn ánh, song ánh; tìm ánh xạ ngược.
- Tính lũy thừa bậc cao của số phức bằng Moivre; tìm căn bậc $n$; giải phương trình trong $\mathbb{C}$.

---

### Bài tập tự luyện Chương 1

**Bài 1.1.** Bằng bảng chân trị, chứng minh $\neg(p \land q) \equiv \neg p \lor \neg q$.

**Bài 1.2.** Viết phủ định của mệnh đề: "$\forall \varepsilon > 0, \exists n \in \mathbb{N}: \frac{1}{n} < \varepsilon$".

**Bài 1.3.** Cho $f: \mathbb{R} \to \mathbb{R}$, $f(x) = x^3 + 1$. Chứng minh $f$ là song ánh và tìm $f^{-1}$.

**Bài 1.4.** Tính $(1 + i)^{10}$.

**Bài 1.5.** Giải phương trình $z^2 = -3 + 4i$.

<details>
<summary><b>Đáp án Chương 1</b> (bấm để mở)</summary>

**1.1.** Lập bảng 4 dòng; hai cột $\neg(p \land q)$ và $\neg p \lor \neg q$ đều là $0, 1, 1, 1$.

**1.2.** "$\exists \varepsilon > 0, \forall n \in \mathbb{N}: \frac{1}{n} \ge \varepsilon$".

**1.3.** Đơn ánh: $x_1^3 + 1 = x_2^3 + 1 \Rightarrow x_1^3 = x_2^3 \Rightarrow x_1 = x_2$. Toàn ánh: với mọi $y$, chọn $x = \sqrt[3]{y - 1}$. Vậy $f^{-1}(y) = \sqrt[3]{y - 1}$.

**1.4.** $1 + i = \sqrt{2}\left(\cos\frac{\pi}{4} + i\sin\frac{\pi}{4}\right)$, nên $(1 + i)^{10} = 32\left(\cos\frac{5\pi}{2} + i\sin\frac{5\pi}{2}\right) = 32i$.

**1.5.** Đặt $z = x + yi$: $x^2 - y^2 = -3$ và $2xy = 4$. Giải được $z = \pm(1 + 2i)$. Kiểm tra: $(1 + 2i)^2 = 1 + 4i - 4 = -3 + 4i$. ✓

</details>

---

## CHƯƠNG 2. MA TRẬN – ĐỊNH THỨC – HỆ PHƯƠNG TRÌNH TUYẾN TÍNH

> Đây là chương **quan trọng nhất về kỹ năng**. Ba thao tác cốt lõi — tính định thức, tìm hạng, giải hệ — sẽ được dùng lại trong mọi chương sau.

### 2.1 Ma trận và các phép toán

**Ma trận** cỡ $m \times n$ là bảng số gồm $m$ hàng, $n$ cột:

$$A = \begin{pmatrix} a_{11} & a_{12} & \cdots & a_{1n} \\ a_{21} & a_{22} & \cdots & a_{2n} \\ \vdots & & \ddots & \vdots \\ a_{m1} & a_{m2} & \cdots & a_{mn} \end{pmatrix} = (a_{ij})_{m \times n}$$

$a_{ij}$ là phần tử ở **hàng $i$, cột $j$**.

#### Các loại ma trận đặc biệt

- **Ma trận vuông** cấp $n$: số hàng = số cột = $n$. Các phần tử $a_{11}, a_{22}, \dots, a_{nn}$ tạo thành **đường chéo chính**.
- **Ma trận đơn vị** $I$: vuông, đường chéo chính toàn số 1, còn lại bằng 0.
- **Ma trận chéo**: chỉ có đường chéo chính khác 0.
- **Ma trận tam giác trên/dưới**: các phần tử phía dưới/phía trên đường chéo chính bằng 0.
- **Ma trận đối xứng**: $A^T = A$, tức $a_{ij} = a_{ji}$.

#### Các phép toán

- **Cộng** (cùng cỡ): cộng từng phần tử tương ứng.
- **Nhân với số** $k$: nhân $k$ vào mọi phần tử.
- **Chuyển vị** $A^T$: đổi hàng thành cột.
- **Nhân hai ma trận:** $A_{m \times \mathbf{p}} \cdot B_{\mathbf{p} \times n} = C_{m \times n}$, với

$$c_{ij} = a_{i1}b_{1j} + a_{i2}b_{2j} + \dots + a_{ip}b_{pj} \quad \text{(hàng } i \text{ của } A \text{ "nhân" cột } j \text{ của } B\text{)}$$

**Điều kiện nhân được: số cột của $A$ = số hàng của $B$.**

**Ví dụ 2.1.** Cho $A = \begin{pmatrix} 1 & 2 \\ 3 & 4 \end{pmatrix}$, $B = \begin{pmatrix} 0 & 1 \\ 1 & 0 \end{pmatrix}$. Tính $AB$ và $BA$.

*Lời giải.*

$$AB = \begin{pmatrix} 1 \cdot 0 + 2 \cdot 1 & 1 \cdot 1 + 2 \cdot 0 \\ 3 \cdot 0 + 4 \cdot 1 & 3 \cdot 1 + 4 \cdot 0 \end{pmatrix} = \begin{pmatrix} 2 & 1 \\ 4 & 3 \end{pmatrix}, \qquad BA = \begin{pmatrix} 3 & 4 \\ 1 & 2 \end{pmatrix}$$

$AB \ne BA$. **Phép nhân ma trận không giao hoán** — đây là lỗi sai phổ biến nhất khi mới học.

#### Tính chất cần nhớ

- $A(BC) = (AB)C$ (kết hợp)
- $A(B + C) = AB + AC$ (phân phối)
- $AI = IA = A$
- $(AB)^T = B^T A^T$ (**đảo thứ tự!**)

#### Ba phép biến đổi sơ cấp theo hàng

1. Đổi chỗ hai hàng: $h_i \leftrightarrow h_j$
2. Nhân một hàng với số $k \ne 0$: $h_i \to k h_i$
3. Cộng vào một hàng bội của hàng khác: $h_i \to h_i + k h_j$

Đây là "con dao đa năng" để tính định thức, tìm hạng, tìm nghịch đảo và giải hệ.

> 🎮 **Liên hệ game:** Mỗi phép biến đổi (quay, co giãn, tịnh tiến) là một ma trận. Kết hợp nhiều phép = nhân ma trận. Vì $AB \ne BA$ nên "quay rồi tịnh tiến" khác "tịnh tiến rồi quay" — bạn sẽ gặp đúng lỗi này khi đặt vị trí vật thể trong engine.
>
> 🤖 **Liên hệ AI:** Một lớp (layer) trong mạng nơ-ron tính $y = Wx + b$ — chính là một phép nhân ma trận cộng một véc tơ.

---

### 2.2 Định thức

Định thức là **một con số** gắn với ma trận vuông. Nó cho biết ma trận có khả nghịch không ($\det A \ne 0$) và, về hình học, cho biết phép biến đổi làm diện tích/thể tích thay đổi bao nhiêu lần.

#### Định thức cấp 2 và cấp 3

$$\begin{vmatrix} a & b \\ c & d \end{vmatrix} = ad - bc$$

**Cấp 3 — quy tắc Sarrus:** chép thêm cột 1, cột 2 sang bên phải; lấy tổng 3 đường chéo xuống **trừ** tổng 3 đường chéo lên.

$$\begin{vmatrix} a_{11} & a_{12} & a_{13} \\ a_{21} & a_{22} & a_{23} \\ a_{31} & a_{32} & a_{33} \end{vmatrix} = (a_{11}a_{22}a_{33} + a_{12}a_{23}a_{31} + a_{13}a_{21}a_{32}) - (a_{13}a_{22}a_{31} + a_{11}a_{23}a_{32} + a_{12}a_{21}a_{33})$$

**Ví dụ 2.2.** Tính $D = \begin{vmatrix} 1 & 2 & 3 \\ 4 & 5 & 6 \\ 7 & 8 & 10 \end{vmatrix}$.

*Lời giải.*

$$D = (1 \cdot 5 \cdot 10 + 2 \cdot 6 \cdot 7 + 3 \cdot 4 \cdot 8) - (3 \cdot 5 \cdot 7 + 1 \cdot 6 \cdot 8 + 2 \cdot 4 \cdot 10) = 230 - 233 = -3$$

> ⚠️ Quy tắc Sarrus **chỉ dùng cho cấp 3**. Cấp 4 trở lên phải dùng khai triển Laplace hoặc biến đổi sơ cấp.

#### Khai triển Laplace (cấp bất kỳ)

- **Định thức con** $M_{ij}$: định thức thu được khi xóa hàng $i$, cột $j$.
- **Phần bù đại số:** $A_{ij} = (-1)^{i+j} M_{ij}$. Dấu $(-1)^{i+j}$ xen kẽ như bàn cờ:

$$\begin{pmatrix} + & - & + & \cdots \\ - & + & - & \cdots \\ + & - & + & \cdots \\ \vdots & & & \ddots \end{pmatrix}$$

Khai triển theo hàng $i$:

$$\det A = a_{i1}A_{i1} + a_{i2}A_{i2} + \dots + a_{in}A_{in}$$

**Mẹo:** chọn hàng/cột có **nhiều số 0 nhất** để khai triển.

#### Tính chất của định thức (rất quan trọng)

1. $\det A^T = \det A$ → mọi tính chất đúng với hàng thì đúng với cột.
2. Đổi chỗ hai hàng → định thức **đổi dấu**.
3. Nhân một hàng với $k$ → định thức **nhân $k$**.
4. Hai hàng bằng nhau hoặc tỉ lệ → định thức **bằng 0**.
5. Cộng vào một hàng bội của hàng khác → định thức **không đổi**.
6. Ma trận tam giác → định thức = **tích các phần tử trên đường chéo chính**.
7. $\det(AB) = \det A \cdot \det B$.
8. $\det(kA) = k^n \det A$ với $A$ cấp $n$ (**không phải** $k \det A$ — rất hay nhầm).

**Chiến lược chung:** dùng tính chất 5 để tạo nhiều số 0, rồi khai triển Laplace hoặc đưa về tam giác.

**Ví dụ 2.3.** Tính $D = \begin{vmatrix} 1 & 2 & 3 & 4 \\ 2 & 3 & 4 & 1 \\ 3 & 4 & 1 & 2 \\ 4 & 1 & 2 & 3 \end{vmatrix}$.

*Lời giải.* Nhận xét: tổng mỗi hàng đều bằng 10. Cộng cột 2, 3, 4 vào cột 1 (định thức không đổi), rồi rút 10 ra ngoài:

$$D = \begin{vmatrix} 10 & 2 & 3 & 4 \\ 10 & 3 & 4 & 1 \\ 10 & 4 & 1 & 2 \\ 10 & 1 & 2 & 3 \end{vmatrix} = 10 \begin{vmatrix} 1 & 2 & 3 & 4 \\ 1 & 3 & 4 & 1 \\ 1 & 4 & 1 & 2 \\ 1 & 1 & 2 & 3 \end{vmatrix}$$

Lấy hàng 2, 3, 4 trừ hàng 1:

$$D = 10 \begin{vmatrix} 1 & 2 & 3 & 4 \\ 0 & 1 & 1 & -3 \\ 0 & 2 & -2 & -2 \\ 0 & -1 & -1 & -1 \end{vmatrix} = 10 \cdot 1 \cdot \begin{vmatrix} 1 & 1 & -3 \\ 2 & -2 & -2 \\ -1 & -1 & -1 \end{vmatrix} \quad \text{(khai triển theo cột 1)}$$

Tính định thức cấp 3 bằng Sarrus: $(2 + 2 + 6) - (-6 + 2 - 2) = 10 - (-6) = 16$.

Vậy $D = 10 \cdot 16 = 160$.

---

### 2.3 Ma trận nghịch đảo

**Định nghĩa.** Ma trận vuông $A$ **khả nghịch** nếu tồn tại $A^{-1}$ sao cho $AA^{-1} = A^{-1}A = I$.

$$\boxed{A \text{ khả nghịch} \iff \det A \ne 0}$$

#### Cách 1: Công thức ma trận phụ hợp

$$A^{-1} = \frac{1}{\det A} \begin{pmatrix} A_{11} & A_{21} & \cdots & A_{n1} \\ A_{12} & A_{22} & \cdots & A_{n2} \\ \vdots & & \ddots & \vdots \\ A_{1n} & A_{2n} & \cdots & A_{nn} \end{pmatrix}$$

> ⚠️ Chú ý chỉ số: phần bù đại số **của hàng $i$** được xếp vào **cột $i$** (tức là lấy chuyển vị ma trận phần bù). Quên chuyển vị là lỗi rất hay gặp.

**Công thức nhanh cho cấp 2** (nên thuộc lòng):

$$\begin{pmatrix} a & b \\ c & d \end{pmatrix}^{-1} = \frac{1}{ad - bc} \begin{pmatrix} d & -b \\ -c & a \end{pmatrix}$$

(đổi chỗ hai phần tử chéo chính, đổi dấu hai phần tử chéo phụ).

#### Cách 2: Gauss–Jordan (nên dùng cho cấp 3 trở lên)

Viết ma trận ghép $[A \mid I]$, dùng biến đổi sơ cấp **theo hàng** đưa vế trái về $I$. Khi đó vế phải chính là $A^{-1}$:

$$[A \mid I] \xrightarrow{\text{biến đổi hàng}} [I \mid A^{-1}]$$

**Ví dụ 2.4.** Tìm nghịch đảo của $A = \begin{pmatrix} 1 & 2 & 3 \\ 0 & 1 & 4 \\ 5 & 6 & 0 \end{pmatrix}$.

*Lời giải.*

$$\left(\begin{array}{ccc|ccc} 1 & 2 & 3 & 1 & 0 & 0 \\ 0 & 1 & 4 & 0 & 1 & 0 \\ 5 & 6 & 0 & 0 & 0 & 1 \end{array}\right) \xrightarrow{h_3 - 5h_1} \left(\begin{array}{ccc|ccc} 1 & 2 & 3 & 1 & 0 & 0 \\ 0 & 1 & 4 & 0 & 1 & 0 \\ 0 & -4 & -15 & -5 & 0 & 1 \end{array}\right)$$

$$\xrightarrow{h_3 + 4h_2} \left(\begin{array}{ccc|ccc} 1 & 2 & 3 & 1 & 0 & 0 \\ 0 & 1 & 4 & 0 & 1 & 0 \\ 0 & 0 & 1 & -5 & 4 & 1 \end{array}\right)$$

Giờ khử ngược lên trên:

$$\xrightarrow[h_1 - 3h_3]{h_2 - 4h_3} \left(\begin{array}{ccc|ccc} 1 & 2 & 0 & 16 & -12 & -3 \\ 0 & 1 & 0 & 20 & -15 & -4 \\ 0 & 0 & 1 & -5 & 4 & 1 \end{array}\right) \xrightarrow{h_1 - 2h_2} \left(\begin{array}{ccc|ccc} 1 & 0 & 0 & -24 & 18 & 5 \\ 0 & 1 & 0 & 20 & -15 & -4 \\ 0 & 0 & 1 & -5 & 4 & 1 \end{array}\right)$$

Vậy $A^{-1} = \begin{pmatrix} -24 & 18 & 5 \\ 20 & -15 & -4 \\ -5 & 4 & 1 \end{pmatrix}$.

**Tự kiểm tra:** nhân thử hàng 1 của $A$ với cột 1 của $A^{-1}$: $1(-24) + 2(20) + 3(-5) = 1$ ✓. Luôn nên kiểm tra ít nhất vài phần tử.

#### Tính chất

- $(AB)^{-1} = B^{-1}A^{-1}$ (đảo thứ tự)
- $(A^T)^{-1} = (A^{-1})^T$
- $\det(A^{-1}) = \dfrac{1}{\det A}$

#### Phương trình ma trận

- $AX = B \Rightarrow X = A^{-1}B$ (nhân $A^{-1}$ **bên trái**)
- $XA = B \Rightarrow X = BA^{-1}$ (nhân $A^{-1}$ **bên phải**)

Vì phép nhân không giao hoán, **không được** viết $X = B/A$ hay đổi vị trí tùy tiện.

---

### 2.4 Hạng của ma trận

**Định nghĩa.** Hạng $r(A)$ là **cấp cao nhất của các định thức con khác 0** của $A$.

**Cách tính trong thực hành:** dùng biến đổi sơ cấp đưa $A$ về **dạng bậc thang**. Khi đó:

$$r(A) = \text{số hàng khác 0 của ma trận bậc thang}$$

Ma trận bậc thang là ma trận mà: hàng toàn 0 nằm dưới cùng; phần tử khác 0 đầu tiên của mỗi hàng nằm lệch dần sang phải so với hàng trên.

**Ví dụ 2.5.** Tìm hạng của $A = \begin{pmatrix} 1 & 2 & -1 & 0 \\ 2 & 5 & 1 & 3 \\ 3 & 7 & 0 & 3 \end{pmatrix}$.

*Lời giải.*

$$A \xrightarrow[h_3 - 3h_1]{h_2 - 2h_1} \begin{pmatrix} 1 & 2 & -1 & 0 \\ 0 & 1 & 3 & 3 \\ 0 & 1 & 3 & 3 \end{pmatrix} \xrightarrow{h_3 - h_2} \begin{pmatrix} 1 & 2 & -1 & 0 \\ 0 & 1 & 3 & 3 \\ 0 & 0 & 0 & 0 \end{pmatrix}$$

Có 2 hàng khác 0, vậy $r(A) = 2$.

**Ví dụ 2.6 (dạng hay thi).** Biện luận hạng của $A = \begin{pmatrix} 1 & 1 & 1 \\ 1 & 2 & 3 \\ 1 & 3 & m \end{pmatrix}$ theo $m$.

*Lời giải.*

$$A \xrightarrow[h_3 - h_1]{h_2 - h_1} \begin{pmatrix} 1 & 1 & 1 \\ 0 & 1 & 2 \\ 0 & 2 & m - 1 \end{pmatrix} \xrightarrow{h_3 - 2h_2} \begin{pmatrix} 1 & 1 & 1 \\ 0 & 1 & 2 \\ 0 & 0 & m - 5 \end{pmatrix}$$

- Nếu $m \ne 5$: $r(A) = 3$.
- Nếu $m = 5$: $r(A) = 2$.

> **Mẹo:** Với ma trận vuông, $r(A) = n \iff \det A \ne 0$. Ở ví dụ trên $\det A = m - 5$, khớp với kết quả.

---

### 2.5 Hệ phương trình tuyến tính

Hệ $m$ phương trình, $n$ ẩn viết gọn dạng ma trận:

$$AX = B, \quad A = (a_{ij})_{m \times n}, \quad X = \begin{pmatrix} x_1 \\ \vdots \\ x_n \end{pmatrix}, \quad B = \begin{pmatrix} b_1 \\ \vdots \\ b_m \end{pmatrix}$$

**Ma trận mở rộng:** $\overline{A} = [A \mid B]$.

#### Định lý Kronecker–Capelli (xương sống của cả phần này)

$$\boxed{\text{Hệ có nghiệm} \iff r(A) = r(\overline{A})}$$

Khi hệ có nghiệm, đặt $r = r(A) = r(\overline{A})$:

| Trường hợp | Kết luận |
|---|---|
| $r(A) < r(\overline{A})$ | **Vô nghiệm** |
| $r = n$ (số ẩn) | **Nghiệm duy nhất** |
| $r < n$ | **Vô số nghiệm**, phụ thuộc $n - r$ tham số |

#### Phương pháp Cramer (hệ vuông, $\det A \ne 0$)

$$x_i = \frac{D_i}{D}, \quad D = \det A, \quad D_i = \text{định thức khi thay cột } i \text{ của } A \text{ bằng cột } B$$

**Ví dụ 2.7.** Giải hệ $\begin{cases} x + y + z = 6 \\ 2x - y + z = 3 \\ x + 2y - z = 2 \end{cases}$

*Lời giải.*

$$D = \begin{vmatrix} 1 & 1 & 1 \\ 2 & -1 & 1 \\ 1 & 2 & -1 \end{vmatrix} = 7, \quad D_x = \begin{vmatrix} 6 & 1 & 1 \\ 3 & -1 & 1 \\ 2 & 2 & -1 \end{vmatrix} = 7$$

$$D_y = \begin{vmatrix} 1 & 6 & 1 \\ 2 & 3 & 1 \\ 1 & 2 & -1 \end{vmatrix} = 14, \quad D_z = \begin{vmatrix} 1 & 1 & 6 \\ 2 & -1 & 3 \\ 1 & 2 & 2 \end{vmatrix} = 21$$

Vậy $x = 1$, $y = 2$, $z = 3$. (Thử lại vào cả 3 phương trình để chắc chắn.)

> Cramer đẹp về lý thuyết nhưng tốn công khi hệ lớn. Với hệ từ cấp 3 trở lên, **Gauss thường nhanh hơn**.

#### Phương pháp Gauss (dùng được cho mọi hệ)

1. Viết ma trận mở rộng $\overline{A}$.
2. Biến đổi sơ cấp **theo hàng** đưa về bậc thang. (**Không** được biến đổi cột, vì cột ứng với ẩn.)
3. So sánh $r(A)$ và $r(\overline{A})$ để kết luận.
4. Nếu có nghiệm: giải ngược từ dưới lên. Ẩn không ứng với "bậc thang" được chọn làm **tham số tự do**.

**Ví dụ 2.8.** Giải hệ $\begin{cases} x_1 + 2x_2 - x_3 + x_4 = 2 \\ 2x_1 + 4x_2 + x_3 - x_4 = 1 \\ x_1 + 2x_2 + 2x_3 - 2x_4 = -1 \end{cases}$

*Lời giải.*

$$\left(\begin{array}{cccc|c} 1 & 2 & -1 & 1 & 2 \\ 2 & 4 & 1 & -1 & 1 \\ 1 & 2 & 2 & -2 & -1 \end{array}\right) \xrightarrow[h_3 - h_1]{h_2 - 2h_1} \left(\begin{array}{cccc|c} 1 & 2 & -1 & 1 & 2 \\ 0 & 0 & 3 & -3 & -3 \\ 0 & 0 & 3 & -3 & -3 \end{array}\right) \xrightarrow{h_3 - h_2} \left(\begin{array}{cccc|c} 1 & 2 & -1 & 1 & 2 \\ 0 & 0 & 3 & -3 & -3 \\ 0 & 0 & 0 & 0 & 0 \end{array}\right)$$

$r(A) = r(\overline{A}) = 2 < 4$ nên hệ có vô số nghiệm, phụ thuộc $4 - 2 = 2$ tham số.

Các "bậc thang" nằm ở cột $x_1$ và $x_3$, nên chọn $x_2 = s$, $x_4 = t$ làm tham số:

- Từ hàng 2: $3x_3 - 3t = -3 \Rightarrow x_3 = t - 1$
- Từ hàng 1: $x_1 = 2 - 2s + x_3 - t = 2 - 2s + (t - 1) - t = 1 - 2s$

**Nghiệm tổng quát:** $(x_1, x_2, x_3, x_4) = (1 - 2s,\; s,\; t - 1,\; t)$, với $s, t \in \mathbb{R}$.

#### Biện luận hệ theo tham số (dạng bài rất hay thi)

**Ví dụ 2.9.** Biện luận số nghiệm của hệ $\begin{cases} x + y + mz = 1 \\ x + my + z = 1 \\ mx + y + z = 1 \end{cases}$

*Lời giải.* Đây là hệ vuông, nên xét định thức trước:

$$D = \begin{vmatrix} 1 & 1 & m \\ 1 & m & 1 \\ m & 1 & 1 \end{vmatrix} = -(m - 1)^2(m + 2)$$

(Bạn hãy tự tính lại bằng Sarrus để luyện tay.)

- **$m \ne 1$ và $m \ne -2$:** $D \ne 0$, hệ có nghiệm duy nhất. Do tính đối xứng của hệ, $x = y = z$; thay vào được $x = y = z = \dfrac{1}{m + 2}$.
- **$m = 1$:** cả ba phương trình đều là $x + y + z = 1$. Khi đó $r(A) = r(\overline{A}) = 1 < 3$: **vô số nghiệm**, phụ thuộc 2 tham số.
- **$m = -2$:** cộng cả ba phương trình vế theo vế: vế trái $= (1 + 1 - 2)(x + y + z) = 0$, vế phải $= 3$. Mâu thuẫn $0 = 3$: **vô nghiệm**. (Kiểm tra bằng hạng: $r(A) = 2$, $r(\overline{A}) = 3$.)

> **Quy trình chuẩn cho hệ vuông có tham số:** tính $D$ → tìm giá trị làm $D = 0$ → với các giá trị đó, **thay số cụ thể vào** rồi dùng Gauss để xét riêng.

#### Hệ thuần nhất $AX = 0$

- **Luôn có nghiệm** $X = 0$ (nghiệm tầm thường), vì $r(A) = r(\overline{A})$ luôn đúng.
- Có **nghiệm không tầm thường** $\iff r(A) < n$.
- Nếu $A$ vuông: có nghiệm không tầm thường $\iff \det A = 0$.

Kết quả này là chìa khóa để tìm véc tơ riêng ở Chương 4.

#### 📝 Dạng bài hay gặp (Chương 2)

- Tính định thức cấp 3, cấp 4 (có thể chứa tham số); tìm $m$ để $\det A = 0$.
- Tìm ma trận nghịch đảo; tìm $m$ để ma trận khả nghịch; giải phương trình ma trận.
- Tìm hạng, biện luận hạng theo $m$.
- Giải hệ bằng Gauss; biện luận số nghiệm theo tham số.
- Chứng minh các tính chất đơn giản (ví dụ: nếu $A^2 = 0$ thì $I - A$ khả nghịch).

---

### Bài tập tự luyện Chương 2

**Bài 2.1.** Tính $\begin{vmatrix} 2 & 1 & 3 \\ 1 & 0 & 2 \\ 4 & 1 & 1 \end{vmatrix}$.

**Bài 2.2.** Tìm nghịch đảo của $\begin{pmatrix} 2 & 1 \\ 5 & 3 \end{pmatrix}$.

**Bài 2.3.** Tìm hạng của $\begin{pmatrix} 1 & 2 & -1 & 3 \\ 2 & 4 & 1 & 0 \\ 3 & 6 & 0 & 3 \end{pmatrix}$.

**Bài 2.4.** Giải hệ $\begin{cases} x + 2y - z = 1 \\ 2x + 4y + z = 5 \\ 3x + 6y = 6 \end{cases}$

**Bài 2.5.** Tìm $m$ để hệ $\begin{cases} mx + y = 1 \\ x + my = 1 \end{cases}$ có nghiệm duy nhất. Với $m$ còn lại, hệ vô nghiệm hay vô số nghiệm?

**Bài 2.6.** Tìm ma trận $X$ thỏa mãn $X \begin{pmatrix} 1 & 2 \\ 3 & 4 \end{pmatrix} = \begin{pmatrix} 1 & 0 \\ 0 & 2 \end{pmatrix}$.

<details>
<summary><b>Đáp án Chương 2</b> (bấm để mở)</summary>

**2.1.** Khai triển theo hàng 2 (có số 0): kết quả bằng $6$.

**2.2.** $\det = 6 - 5 = 1$, nên nghịch đảo là $\begin{pmatrix} 3 & -1 \\ -5 & 2 \end{pmatrix}$.

**2.3.** $h_2 - 2h_1$ và $h_3 - 3h_1$ đều cho $(0, 0, 3, -6)$. Sau khi khử còn 2 hàng khác 0, vậy hạng bằng $2$.

**2.4.** Hàng 3 = hàng 1 + hàng 2 (cả vế phải: $1 + 5 = 6$), nên $r(A) = r(\overline{A}) = 2 < 3$: vô số nghiệm. Từ $h_2 - 2h_1$: $3z = 3 \Rightarrow z = 1$. Đặt $y = t$: $x = 2 - 2t$. Nghiệm: $(2 - 2t,\; t,\; 1)$.

**2.5.** $D = m^2 - 1$. Nghiệm duy nhất $\iff m \ne \pm 1$. Khi $m = 1$: hai phương trình trùng nhau → vô số nghiệm. Khi $m = -1$: $-x + y = 1$ và $x - y = 1$, cộng lại được $0 = 2$ → vô nghiệm.

**2.6.** $X = \begin{pmatrix} 1 & 0 \\ 0 & 2 \end{pmatrix} \begin{pmatrix} 1 & 2 \\ 3 & 4 \end{pmatrix}^{-1} = \begin{pmatrix} -2 & 1 \\ 3 & -1 \end{pmatrix}$. (Nhớ: nghịch đảo nhân **bên phải**.)

</details>

---

## CHƯƠNG 3. KHÔNG GIAN VÉC TƠ

> Chương 2 cho bạn **công cụ tính**. Chương 3 cho bạn **ngôn ngữ để hiểu** những con số đó có nghĩa là gì. Đây là chương trừu tượng nhất, nhưng gần như mọi bài đều quy về: **lập ma trận → tìm hạng hoặc giải hệ**.

### 3.1 Khái niệm không gian véc tơ

**Ý tưởng:** Không gian véc tơ là một tập hợp các đối tượng mà ta **cộng được với nhau** và **nhân được với một số**, và hai phép toán này "cư xử tử tế" như với mũi tên trong mặt phẳng.

**Định nghĩa.** Tập $V \ne \emptyset$ cùng phép cộng và phép nhân với số thực là **không gian véc tơ (KGVT)** trên $\mathbb{R}$ nếu với mọi $u, v, w \in V$ và $k, l \in \mathbb{R}$:

1. $u + v = v + u$
2. $(u + v) + w = u + (v + w)$
3. Có véc tơ không $\theta$: $u + \theta = u$
4. Mỗi $u$ có véc tơ đối $-u$: $u + (-u) = \theta$
5. $k(u + v) = ku + kv$
6. $(k + l)u = ku + lu$
7. $(kl)u = k(lu)$
8. $1 \cdot u = u$

**Các ví dụ quan trọng** (cần thuộc):

| Không gian | Phần tử | Véc tơ không |
|---|---|---|
| $\mathbb{R}^n$ | Bộ $n$ số $(x_1, \dots, x_n)$ | $(0, \dots, 0)$ |
| $M_{m \times n}$ | Ma trận cỡ $m \times n$ | Ma trận không |
| $P_n[x]$ | Đa thức bậc $\le n$ | Đa thức $0$ |

> **Chú ý:** Tập các đa thức **bậc đúng bằng** $n$ không phải KGVT, vì nó không chứa đa thức $0$, và tổng hai đa thức bậc $n$ có thể có bậc nhỏ hơn: $(x^2 + 1) + (-x^2) = 1$.

---

### 3.2 Không gian véc tơ con

**Định nghĩa.** $W \subset V$, $W \ne \emptyset$ là **không gian con** của $V$ nếu $W$ **khép kín** với hai phép toán:

$$\forall u, v \in W, \forall k \in \mathbb{R}: \quad u + v \in W \quad \text{và} \quad ku \in W$$

**Mẹo loại nhanh:** Nếu $W$ **không chứa véc tơ không** thì $W$ chắc chắn không phải không gian con.

**Ví dụ 3.1.** Chứng minh $W = \{(x, y, z) \in \mathbb{R}^3 \mid x + y - z = 0\}$ là không gian con của $\mathbb{R}^3$.

*Lời giải.* $(0, 0, 0) \in W$ nên $W \ne \emptyset$. Lấy $u = (x_1, y_1, z_1)$, $v = (x_2, y_2, z_2) \in W$, tức $x_1 + y_1 - z_1 = 0$ và $x_2 + y_2 - z_2 = 0$.

- $u + v = (x_1 + x_2, y_1 + y_2, z_1 + z_2)$ có $(x_1 + x_2) + (y_1 + y_2) - (z_1 + z_2) = 0 + 0 = 0$, nên $u + v \in W$.
- $ku = (kx_1, ky_1, kz_1)$ có $kx_1 + ky_1 - kz_1 = k \cdot 0 = 0$, nên $ku \in W$.

Vậy $W$ là không gian con. $\blacksquare$

**Ví dụ 3.2.** $W = \{(x, y) \mid x + y = 1\}$ **không** là không gian con vì $(0, 0) \notin W$.

**Ví dụ 3.3.** $W = \{(x, y) \mid xy \ge 0\}$ chứa $(0, 0)$ nhưng **không** là không gian con: $(1, 0) \in W$, $(0, -1) \in W$ nhưng tổng $(1, -1)$ có tích $-1 < 0$.

> **Quy luật nhận dạng:** Tập nghiệm của hệ phương trình tuyến tính **thuần nhất** (vế phải bằng 0) luôn là không gian con. Có vế phải khác 0, hoặc có bình phương, tích, bất đẳng thức... thì thường không phải.

**Giao và tổng:** Nếu $U, W$ là không gian con thì $U \cap W$ và $U + W = \{u + w\}$ cũng là không gian con. Nhưng $U \cup W$ **thường không** phải.

---

### 3.3 Tổ hợp tuyến tính – Độc lập và phụ thuộc tuyến tính

**Tổ hợp tuyến tính** của hệ $\{v_1, \dots, v_m\}$ là véc tơ dạng $k_1 v_1 + k_2 v_2 + \dots + k_m v_m$.

**Không gian sinh** $\text{span}\{v_1, \dots, v_m\}$ là tập **tất cả** các tổ hợp tuyến tính của chúng. Đây luôn là một không gian con.

**Định nghĩa.** Hệ $\{v_1, \dots, v_m\}$ là:

- **Độc lập tuyến tính (ĐLTT)** nếu: $k_1 v_1 + \dots + k_m v_m = \theta \Rightarrow k_1 = \dots = k_m = 0$
- **Phụ thuộc tuyến tính (PTTT)** nếu không ĐLTT, tức là có một véc tơ biểu diễn được qua các véc tơ còn lại.

**Hiểu trực quan:** ĐLTT nghĩa là "không có véc tơ nào thừa" — mỗi véc tơ mang lại một hướng mới.

#### Cách kiểm tra trong thực hành (với $\mathbb{R}^n$)

Xếp $m$ véc tơ thành $m$ hàng của ma trận $A$, rồi tính hạng:

$$\boxed{r(A) = m \iff \text{ĐLTT}, \qquad r(A) < m \iff \text{PTTT}}$$

Nếu $m = n$ (ma trận vuông): **ĐLTT $\iff \det A \ne 0$**.

**Hệ quả hữu ích:**

- Hệ chứa véc tơ không thì PTTT.
- Trong $\mathbb{R}^n$, một hệ có **nhiều hơn $n$** véc tơ thì chắc chắn PTTT.

**Ví dụ 3.4.** Xét tính ĐLTT của $u_1 = (1, 2, 3)$, $u_2 = (0, 1, 1)$, $u_3 = (1, 3, 4)$.

*Lời giải.* $\begin{vmatrix} 1 & 2 & 3 \\ 0 & 1 & 1 \\ 1 & 3 & 4 \end{vmatrix} = 0$, nên hệ PTTT. Thực tế dễ thấy $u_3 = u_1 + u_2$.

**Ví dụ 3.5.** Tìm $m$ để $u = (1, m, 3) \in \text{span}\{(1, 1, 1), (1, 2, 3)\}$.

*Lời giải.* Cần tìm $a, b$ sao cho $a(1, 1, 1) + b(1, 2, 3) = (1, m, 3)$:

$$\begin{cases} a + b = 1 \\ a + 2b = m \\ a + 3b = 3 \end{cases}$$

Từ phương trình 1 và 3: $2b = 2 \Rightarrow b = 1$, $a = 0$. Thay vào phương trình 2: $m = 2$.

> **Tổng quát:** "$u \in \text{span}\{\dots\}$" $\iff$ hệ phương trình tương ứng **có nghiệm** $\iff r(A) = r(\overline{A})$. Bài dạng này chính là bài Kronecker–Capelli ở Chương 2.

---

### 3.4 Cơ sở – Số chiều – Tọa độ

**Định nghĩa.** Hệ $B = \{e_1, \dots, e_n\}$ là **cơ sở** của $V$ nếu:

1. $B$ độc lập tuyến tính, và
2. $B$ sinh ra $V$ (mọi véc tơ của $V$ đều là tổ hợp tuyến tính của $B$).

**Số chiều** $\dim V$ là số véc tơ trong một cơ sở (mọi cơ sở đều có cùng số véc tơ).

| Không gian | Cơ sở chính tắc | Số chiều |
|---|---|---|
| $\mathbb{R}^n$ | $(1, 0, \dots, 0), \dots, (0, \dots, 0, 1)$ | $n$ |
| $P_n[x]$ | $1, x, x^2, \dots, x^n$ | $n + 1$ (**hay nhầm thành $n$**) |
| $M_{m \times n}$ | Các ma trận chỉ có một số 1 | $mn$ |

**Định lý (tiết kiệm công sức).** Nếu $\dim V = n$ thì **mọi hệ $n$ véc tơ ĐLTT đều là cơ sở**. Không cần kiểm tra điều kiện sinh.

Hệ quả: **$n$ véc tơ trong $\mathbb{R}^n$ là cơ sở $\iff$ định thức của chúng khác 0.**

#### Tọa độ

Nếu $B = \{e_1, \dots, e_n\}$ là cơ sở và $x = x_1 e_1 + \dots + x_n e_n$ thì bộ số $(x_1, \dots, x_n)$ là **tọa độ** của $x$ trong cơ sở $B$, ký hiệu:

$$[x]_B = \begin{pmatrix} x_1 \\ \vdots \\ x_n \end{pmatrix}$$

Tọa độ trong một cơ sở cho trước là **duy nhất**.

**Ví dụ 3.6.** Chứng minh $B = \{e_1 = (1, 1, 0),\; e_2 = (0, 1, 1),\; e_3 = (1, 0, 1)\}$ là cơ sở của $\mathbb{R}^3$ và tìm tọa độ của $x = (2, 3, 1)$ trong $B$.

*Lời giải.*

*Cơ sở:* $\begin{vmatrix} 1 & 1 & 0 \\ 0 & 1 & 1 \\ 1 & 0 & 1 \end{vmatrix} = 2 \ne 0$, nên 3 véc tơ ĐLTT trong $\mathbb{R}^3$ (có $\dim = 3$), do đó là cơ sở.

*Tọa độ:* Tìm $a, b, c$ với $a(1, 1, 0) + b(0, 1, 1) + c(1, 0, 1) = (2, 3, 1)$:

$$\begin{cases} a + c = 2 \\ a + b = 3 \\ b + c = 1 \end{cases}$$

Cộng ba phương trình: $2(a + b + c) = 6 \Rightarrow a + b + c = 3$. Suy ra $b = 1$, $c = 0$, $a = 2$.

Vậy $[x]_B = (2, 1, 0)^T$. Kiểm tra: $2(1, 1, 0) + 1(0, 1, 1) + 0 = (2, 3, 1)$ ✓.

> **Mẹo cho đa thức và ma trận:** Đồng nhất đa thức $a_0 + a_1 x + \dots + a_n x^n$ với véc tơ hệ số $(a_0, a_1, \dots, a_n) \in \mathbb{R}^{n+1}$. Tương tự, ma trận $2 \times 2$ đồng nhất với véc tơ trong $\mathbb{R}^4$. Nhờ vậy, mọi bài về $P_n[x]$ và $M_{m \times n}$ đều quy về bài trong $\mathbb{R}^k$ quen thuộc.

---

### 3.5 Hạng của hệ véc tơ – Tìm cơ sở của không gian sinh

**Hạng** của hệ véc tơ là số véc tơ ĐLTT tối đa trong hệ. Trong thực hành:

$$\dim \text{span}\{v_1, \dots, v_m\} = r(A), \quad A \text{ có các hàng là } v_1, \dots, v_m$$

**Cơ sở** của không gian sinh: các **hàng khác 0** của ma trận bậc thang (khi chỉ biến đổi theo hàng).

**Ví dụ 3.7.** Tìm cơ sở và số chiều của $W = \text{span}\{(1, 2, -1, 0),\; (2, 5, 1, 3),\; (3, 7, 0, 3)\}$.

*Lời giải.* Đây chính là ma trận ở Ví dụ 2.5:

$$\begin{pmatrix} 1 & 2 & -1 & 0 \\ 2 & 5 & 1 & 3 \\ 3 & 7 & 0 & 3 \end{pmatrix} \to \begin{pmatrix} 1 & 2 & -1 & 0 \\ 0 & 1 & 3 & 3 \\ 0 & 0 & 0 & 0 \end{pmatrix}$$

Vậy $\dim W = 2$, một cơ sở là $\{(1, 2, -1, 0),\; (0, 1, 3, 3)\}$.

(Cũng có thể chọn 2 véc tơ ban đầu $\{(1, 2, -1, 0), (2, 5, 1, 3)\}$ vì chúng ĐLTT.)

---

### 3.6 Đổi cơ sở

Cho hai cơ sở $B = \{e_1, \dots, e_n\}$ và $B' = \{e'_1, \dots, e'_n\}$ của $V$.

**Ma trận chuyển** từ $B$ sang $B'$ là ma trận $P$ mà **cột thứ $j$ là tọa độ của $e'_j$ trong cơ sở $B$**:

$$P = \Big( [e'_1]_B \;\; [e'_2]_B \;\; \cdots \;\; [e'_n]_B \Big)$$

**Công thức đổi tọa độ:**

$$\boxed{[x]_B = P \, [x]_{B'}} \qquad \Longleftrightarrow \qquad [x]_{B'} = P^{-1} [x]_B$$

**Trường hợp hay gặp nhất:** $B$ là cơ sở chính tắc. Khi đó $P$ đơn giản là **ma trận có các cột là các véc tơ của $B'$**.

**Ví dụ 3.8.** Trong $\mathbb{R}^2$, cho $B' = \{(1, -2), (1, 1)\}$. Tìm tọa độ của $x = (3, 0)$ trong $B'$.

*Lời giải.* Ma trận chuyển từ cơ sở chính tắc sang $B'$:

$$P = \begin{pmatrix} 1 & 1 \\ -2 & 1 \end{pmatrix}, \quad P^{-1} = \frac{1}{3}\begin{pmatrix} 1 & -1 \\ 2 & 1 \end{pmatrix}$$

$$[x]_{B'} = P^{-1} \begin{pmatrix} 3 \\ 0 \end{pmatrix} = \frac{1}{3}\begin{pmatrix} 3 \\ 6 \end{pmatrix} = \begin{pmatrix} 1 \\ 2 \end{pmatrix}$$

Kiểm tra: $1 \cdot (1, -2) + 2 \cdot (1, 1) = (3, 0)$ ✓. (Cơ sở $B'$ này sẽ xuất hiện lại ở Chương 4.)

> **Mẹo:** Nếu cả $B$ và $B'$ đều không phải chính tắc, gọi $P_B$, $P_{B'}$ là các ma trận có cột là véc tơ của từng cơ sở. Khi đó ma trận chuyển từ $B$ sang $B'$ là $P = P_B^{-1} P_{B'}$.

> 🎮 **Liên hệ game:** Đây chính là chuyện **local space** và **world space**. Vị trí khẩu súng "gắn trên tay nhân vật" là tọa độ trong cơ sở của nhân vật; engine phải đổi sang cơ sở của thế giới để vẽ. Ma trận `localToWorldMatrix` trong Unity đóng vai trò giống $P$.

---

### 3.7 Không gian nghiệm của hệ thuần nhất

Tập nghiệm $W = \{X \in \mathbb{R}^n \mid AX = 0\}$ là một không gian con của $\mathbb{R}^n$, và:

$$\boxed{\dim W = n - r(A)}$$

**Cách tìm cơ sở** (gọi là **hệ nghiệm cơ bản**):

1. Giải hệ bằng Gauss, được nghiệm tổng quát theo $k = n - r(A)$ tham số.
2. Lần lượt cho **một** tham số bằng 1, các tham số còn lại bằng 0. Thu được $k$ véc tơ — đó là một cơ sở.

**Ví dụ 3.9.** Tìm cơ sở và số chiều của không gian nghiệm:

$$\begin{cases} x_1 + 2x_2 + x_3 - x_4 = 0 \\ 2x_1 + 4x_2 + 3x_3 + x_4 = 0 \end{cases}$$

*Lời giải.*

$$\begin{pmatrix} 1 & 2 & 1 & -1 \\ 2 & 4 & 3 & 1 \end{pmatrix} \xrightarrow{h_2 - 2h_1} \begin{pmatrix} 1 & 2 & 1 & -1 \\ 0 & 0 & 1 & 3 \end{pmatrix}$$

$r(A) = 2$, nên $\dim W = 4 - 2 = 2$. Chọn $x_2 = s$, $x_4 = t$:

- $x_3 = -3t$
- $x_1 = -2s - x_3 + t = -2s + 4t$

Nghiệm tổng quát: $(-2s + 4t,\; s,\; -3t,\; t) = s(-2, 1, 0, 0) + t(4, 0, -3, 1)$.

Cơ sở: $\{(-2, 1, 0, 0),\; (4, 0, -3, 1)\}$.

> 🤖 **Liên hệ AI:** Trong học máy, mỗi dữ liệu thường là một véc tơ trong $\mathbb{R}^n$ (ví dụ: *embedding* của một từ có hàng trăm chiều). Các khái niệm span, cơ sở, số chiều là nền để hiểu các kỹ thuật **giảm chiều dữ liệu** như PCA.

#### 📝 Dạng bài hay gặp (Chương 3)

- Chứng minh một tập là (hoặc không là) không gian con.
- Xét ĐLTT/PTTT, thường có tham số $m$.
- Tìm $m$ để một véc tơ thuộc span của hệ cho trước.
- Chứng minh hệ là cơ sở; tìm tọa độ; tìm ma trận chuyển cơ sở.
- Tìm cơ sở và số chiều của: không gian sinh, không gian nghiệm, không gian con trong $P_n[x]$.

---

### Bài tập tự luyện Chương 3

**Bài 3.1.** Chứng minh $W = \{(x, y, z) \mid x - 2y + z = 0\}$ là không gian con của $\mathbb{R}^3$. Tìm một cơ sở và số chiều của $W$.

**Bài 3.2.** Hệ $\{(1, 0, 1),\; (2, 1, 0),\; (0, 1, -2)\}$ ĐLTT hay PTTT? Nếu PTTT, chỉ ra một hệ thức phụ thuộc.

**Bài 3.3.** Tìm cơ sở và số chiều của không gian nghiệm: $\begin{cases} x_1 + x_2 - x_3 + x_4 = 0 \\ 2x_1 + x_2 + x_3 - x_4 = 0 \end{cases}$

**Bài 3.4.** Trong $P_2[x]$, tìm tọa độ của $p(x) = 3 + 2x + x^2$ trong cơ sở $\{1,\; 1 + x,\; 1 + x + x^2\}$.

**Bài 3.5.** Tìm $m$ để $\{(1, 1, m),\; (1, m, 1),\; (m, 1, 1)\}$ là một cơ sở của $\mathbb{R}^3$.

<details>
<summary><b>Đáp án Chương 3</b> (bấm để mở)</summary>

**3.1.** Chứng minh khép kín như Ví dụ 3.1. Từ $x = 2y - z$, chọn $y = s$, $z = t$: $(x, y, z) = s(2, 1, 0) + t(-1, 0, 1)$. Cơ sở $\{(2, 1, 0), (-1, 0, 1)\}$, $\dim W = 2$.

**3.2.** Định thức bằng 0 nên PTTT. Hệ thức: $(0, 1, -2) = (2, 1, 0) - 2(1, 0, 1)$.

**3.3.** Nghiệm tổng quát: $x_1 = -2x_3 + 2x_4$, $x_2 = 3x_3 - 3x_4$. Cơ sở $\{(-2, 3, 1, 0),\; (2, -3, 0, 1)\}$, số chiều bằng 2.

**3.4.** $a \cdot 1 + b(1 + x) + c(1 + x + x^2) = 3 + 2x + x^2$. So sánh hệ số: $c = 1$, $b + c = 2$, $a + b + c = 3$. Suy ra tọa độ là $(1, 1, 1)$.

**3.5.** Định thức bằng $-(m - 1)^2(m + 2)$ (chính là ma trận ở Ví dụ 2.9). Là cơ sở $\iff m \ne 1$ và $m \ne -2$.

</details>

---

## CHƯƠNG 4. ÁNH XẠ TUYẾN TÍNH – TRỊ RIÊNG – CHÉO HÓA

> Chương này trả lời câu hỏi: **ma trận thực chất là gì?** Câu trả lời: ma trận là cách ghi lại một **ánh xạ tuyến tính** sau khi đã chọn cơ sở. Hiểu được điều này, trị riêng và chéo hóa sẽ trở nên rất tự nhiên.

### 4.1 Khái niệm ánh xạ tuyến tính

**Định nghĩa.** Ánh xạ $f: V \to W$ giữa hai KGVT là **tuyến tính** nếu với mọi $u, v \in V$, $k \in \mathbb{R}$:

$$f(u + v) = f(u) + f(v) \qquad \text{và} \qquad f(ku) = kf(u)$$

Gộp lại: $f(au + bv) = af(u) + bf(v)$. Nói nôm na: **$f$ "giữ nguyên" phép cộng và phép nhân với số**.

**Hệ quả:** $f(\theta) = \theta$. Nếu $f(\theta) \ne \theta$ thì $f$ chắc chắn không tuyến tính.

**Nhận dạng nhanh trong $\mathbb{R}^n$:** $f$ tuyến tính khi **mỗi thành phần** của $f(x)$ là biểu thức **bậc nhất, không có hằng số tự do**.

**Ví dụ 4.1.**

- $f(x, y) = (x + y,\; x - y,\; 2x)$: **tuyến tính**.
- $g(x, y) = (x + 1,\; y)$: **không**, vì $g(0, 0) = (1, 0) \ne (0, 0)$.
- $h(x, y) = (xy,\; x)$: **không**, vì $h(2 \cdot (1, 1)) = h(2, 2) = (4, 2)$ nhưng $2h(1, 1) = (2, 2)$.
- Phép lấy đạo hàm $D: P_n[x] \to P_{n}[x]$, $D(p) = p'$: **tuyến tính**, vì $(ap + bq)' = ap' + bq'$.

---

### 4.2 Hạt nhân và ảnh

- **Hạt nhân:** $\text{Ker} f = \{v \in V \mid f(v) = \theta\}$ — không gian con của $V$.
- **Ảnh:** $\text{Im} f = \{f(v) \mid v \in V\}$ — không gian con của $W$.

**Định lý số chiều** (rất hay dùng):

$$\boxed{\dim V = \dim \text{Ker} f + \dim \text{Im} f}$$

**Liên hệ với đơn ánh, toàn ánh:**

- $f$ đơn ánh $\iff \text{Ker} f = \{\theta\}$
- $f$ toàn ánh $\iff \text{Im} f = W$
- $f$ song ánh tuyến tính được gọi là **đẳng cấu**.

#### Cách tìm trong thực hành (khi $f$ có ma trận $A$)

| Cần tìm | Cách làm |
|---|---|
| $\text{Ker} f$ | Giải hệ thuần nhất $AX = 0$ (như mục 3.7) |
| $\dim \text{Im} f$ | Bằng $r(A)$ |
| Cơ sở của $\text{Im} f$ | Các **cột** của $A$ nằm ở vị trí "bậc thang" sau khi khử Gauss theo hàng |

**Ví dụ 4.2.** Cho $f: \mathbb{R}^3 \to \mathbb{R}^3$, $f(x, y, z) = (x + y - z,\; 2x + 3y - z,\; 3x + 5y - z)$. Tìm cơ sở và số chiều của $\text{Ker} f$ và $\text{Im} f$.

*Lời giải.* Ma trận của $f$ (đọc hệ số từng thành phần):

$$A = \begin{pmatrix} 1 & 1 & -1 \\ 2 & 3 & -1 \\ 3 & 5 & -1 \end{pmatrix} \xrightarrow[h_3 - 3h_1]{h_2 - 2h_1} \begin{pmatrix} 1 & 1 & -1 \\ 0 & 1 & 1 \\ 0 & 2 & 2 \end{pmatrix} \xrightarrow{h_3 - 2h_2} \begin{pmatrix} 1 & 1 & -1 \\ 0 & 1 & 1 \\ 0 & 0 & 0 \end{pmatrix}$$

*Hạt nhân:* Từ ma trận bậc thang: $y + z = 0$ và $x + y - z = 0$. Đặt $z = t$: $y = -t$, $x = 2t$.

$$\text{Ker} f = \text{span}\{(2, -1, 1)\}, \quad \dim \text{Ker} f = 1$$

(Kiểm tra: $f(2, -1, 1) = (2 - 1 - 1,\; 4 - 3 - 1,\; 6 - 5 - 1) = (0, 0, 0)$ ✓)

*Ảnh:* Bậc thang nằm ở cột 1 và cột 2, nên lấy cột 1 và cột 2 của ma trận **ban đầu**:

$$\text{Im} f = \text{span}\{(1, 2, 3),\; (1, 3, 5)\}, \quad \dim \text{Im} f = 2$$

*Kiểm tra định lý số chiều:* $1 + 2 = 3 = \dim \mathbb{R}^3$ ✓. Vì $\text{Ker} f \ne \{\theta\}$ nên $f$ không đơn ánh (và cũng không toàn ánh).

---

### 4.3 Ma trận của ánh xạ tuyến tính

**Định nghĩa.** Cho $f: V \to W$, cơ sở $B = \{e_1, \dots, e_n\}$ của $V$ và cơ sở $C$ của $W$. **Ma trận của $f$** là ma trận có:

$$\boxed{\text{cột thứ } j = [f(e_j)]_C \quad \text{(tọa độ của ảnh véc tơ cơ sở thứ } j\text{)}}$$

Khi đó: $[f(x)]_C = A \, [x]_B$.

> **Câu thần chú:** "Cột của ma trận là **ảnh của các véc tơ cơ sở**." Nhớ câu này, bạn sẽ tự dựng được ma trận của bất kỳ phép biến đổi nào.

**Ví dụ 4.3 (phép quay trong mặt phẳng).** Tìm ma trận của phép quay $R_\alpha$ quanh gốc tọa độ một góc $\alpha$ ngược chiều kim đồng hồ.

*Lời giải.* Quay $e_1 = (1, 0)$ được $(\cos\alpha, \sin\alpha)$. Quay $e_2 = (0, 1)$ được $(-\sin\alpha, \cos\alpha)$. Xếp chúng thành các cột:

$$R_\alpha = \begin{pmatrix} \cos\alpha & -\sin\alpha \\ \sin\alpha & \cos\alpha \end{pmatrix}$$

> 🎮 Đây đúng là ma trận quay dùng trong mọi game 2D. Để ý $\det R_\alpha = 1$ (quay không làm thay đổi diện tích) và $R_\alpha^{-1} = R_\alpha^T$ (quay ngược lại = chuyển vị).

**Ví dụ 4.4 (đạo hàm).** Tìm ma trận của $D: P_2[x] \to P_2[x]$, $D(p) = p'$ trong cơ sở $\{1, x, x^2\}$.

*Lời giải.* $D(1) = 0$, $D(x) = 1$, $D(x^2) = 2x$. Tọa độ lần lượt là $(0, 0, 0)$, $(1, 0, 0)$, $(0, 2, 0)$. Xếp thành cột:

$$A = \begin{pmatrix} 0 & 1 & 0 \\ 0 & 0 & 2 \\ 0 & 0 & 0 \end{pmatrix}$$

#### Ma trận của toán tử khi đổi cơ sở

Với **toán tử tuyến tính** $f: V \to V$, nếu $A$ là ma trận của $f$ trong cơ sở $B$, $P$ là ma trận chuyển từ $B$ sang $B'$, thì ma trận của $f$ trong $B'$ là:

$$\boxed{A' = P^{-1} A P}$$

Hai ma trận liên hệ như vậy gọi là **đồng dạng**. Chúng có cùng định thức, cùng vết (tổng đường chéo) và cùng trị riêng.

**Ví dụ 4.5.** Cho $f(x, y) = (4x + y,\; 2x + 3y)$. Tìm ma trận của $f$ trong cơ sở $B' = \{(1, -2),\; (1, 1)\}$.

*Lời giải.* Cách nhanh nhất là áp dụng thẳng "câu thần chú":

- $f(1, -2) = (4 - 2,\; 2 - 6) = (2, -4) = \mathbf{2} \cdot (1, -2) + 0 \cdot (1, 1)$
- $f(1, 1) = (5, 5) = 0 \cdot (1, -2) + \mathbf{5} \cdot (1, 1)$

$$A' = \begin{pmatrix} 2 & 0 \\ 0 & 5 \end{pmatrix}$$

Ma trận ban đầu $A = \begin{pmatrix} 4 & 1 \\ 2 & 3 \end{pmatrix}$ trông "lộn xộn", nhưng trong cơ sở $B'$ nó trở thành **ma trận chéo**. Trong cơ sở này, $f$ chỉ làm một việc đơn giản: kéo giãn hướng $(1, -2)$ gấp 2 lần, và hướng $(1, 1)$ gấp 5 lần.

Làm sao tìm ra cơ sở "đẹp" như vậy? Đó chính là nội dung của trị riêng và chéo hóa.

---

### 4.4 Trị riêng và véc tơ riêng

**Định nghĩa.** Số $\lambda$ là **trị riêng** của ma trận vuông $A$ nếu tồn tại véc tơ $v \ne \theta$ sao cho:

$$Av = \lambda v$$

Khi đó $v$ là **véc tơ riêng** ứng với $\lambda$.

**Ý nghĩa hình học:** Véc tơ riêng là hướng mà phép biến đổi **không làm xoay**, chỉ kéo giãn (hoặc co lại, hoặc lật ngược) với hệ số $\lambda$.

#### Quy trình tìm

1. Giải **phương trình đặc trưng** $\det(A - \lambda I) = 0$ để tìm các trị riêng $\lambda$.
2. Với mỗi $\lambda$, giải hệ thuần nhất $(A - \lambda I)X = 0$. Các nghiệm **khác 0** là véc tơ riêng. Tập nghiệm (kể cả 0) là **không gian riêng** $E(\lambda)$.

**Tính chất giúp tự kiểm tra:**

- Tổng các trị riêng = **vết** của $A$ (tổng đường chéo chính).
- Tích các trị riêng = $\det A$.
- Ma trận tam giác có trị riêng chính là các phần tử trên đường chéo chính.
- Véc tơ riêng ứng với các trị riêng **khác nhau** thì ĐLTT.

**Ví dụ 4.6.** Tìm trị riêng, véc tơ riêng của $A = \begin{pmatrix} 4 & 1 \\ 2 & 3 \end{pmatrix}$.

*Lời giải.*

$$\det(A - \lambda I) = \begin{vmatrix} 4 - \lambda & 1 \\ 2 & 3 - \lambda \end{vmatrix} = (4 - \lambda)(3 - \lambda) - 2 = \lambda^2 - 7\lambda + 10 = (\lambda - 2)(\lambda - 5)$$

Trị riêng: $\lambda_1 = 2$, $\lambda_2 = 5$. (Kiểm tra: $2 + 5 = 7 = 4 + 3$ ✓; $2 \cdot 5 = 10 = \det A$ ✓.)

- **$\lambda_1 = 2$:** $(A - 2I)X = 0 \iff \begin{pmatrix} 2 & 1 \\ 2 & 1 \end{pmatrix}\begin{pmatrix} x \\ y \end{pmatrix} = 0 \iff 2x + y = 0$. Véc tơ riêng: $t(1, -2)$, $t \ne 0$.
- **$\lambda_2 = 5$:** $(A - 5I)X = 0 \iff \begin{pmatrix} -1 & 1 \\ 2 & -2 \end{pmatrix}\begin{pmatrix} x \\ y \end{pmatrix} = 0 \iff x = y$. Véc tơ riêng: $t(1, 1)$, $t \ne 0$.

Đây đúng là cơ sở $B'$ "đẹp" ở Ví dụ 4.5!

**Ví dụ 4.7 (cấp 3, có trị riêng bội).** Tìm trị riêng, véc tơ riêng của $A = \begin{pmatrix} 2 & 0 & 0 \\ 0 & 1 & 1 \\ 0 & 1 & 1 \end{pmatrix}$.

*Lời giải.* Khai triển theo hàng 1:

$$\det(A - \lambda I) = (2 - \lambda)\begin{vmatrix} 1 - \lambda & 1 \\ 1 & 1 - \lambda \end{vmatrix} = (2 - \lambda)\big[(1 - \lambda)^2 - 1\big] = (2 - \lambda)(\lambda^2 - 2\lambda) = -\lambda(\lambda - 2)^2$$

Trị riêng: $\lambda = 0$ (bội 1), $\lambda = 2$ (bội 2).

- **$\lambda = 2$:** $A - 2I = \begin{pmatrix} 0 & 0 & 0 \\ 0 & -1 & 1 \\ 0 & 1 & -1 \end{pmatrix}$. Chỉ còn một phương trình $y = z$, với $x$ tự do. Đặt $x = s$, $y = z = t$:

$$E(2) = \text{span}\{(1, 0, 0),\; (0, 1, 1)\}, \quad \dim E(2) = 2$$

- **$\lambda = 0$:** $AX = 0 \iff 2x = 0$ và $y + z = 0$. Suy ra $E(0) = \text{span}\{(0, 1, -1)\}$.

---

### 4.5 Chéo hóa ma trận

**Định nghĩa.** Ma trận vuông $A$ **chéo hóa được** nếu tồn tại ma trận khả nghịch $P$ sao cho $P^{-1}AP = D$ là ma trận chéo.

#### Điều kiện chéo hóa được

$$A_{n \times n} \text{ chéo hóa được} \iff A \text{ có } n \text{ véc tơ riêng ĐLTT}$$

Cụ thể, cần **cả hai** điều sau:

1. Phương trình đặc trưng có đủ $n$ nghiệm thực (tính cả bội).
2. Với mỗi trị riêng $\lambda$: $\dim E(\lambda) = $ **bội** của $\lambda$.

**Điều kiện đủ (dùng nhanh):** $A$ có $n$ trị riêng **phân biệt** thì chắc chắn chéo hóa được.

#### Quy trình chéo hóa (4 bước)

1. Tìm các trị riêng.
2. Với mỗi trị riêng, tìm cơ sở của không gian riêng.
3. Kiểm tra tổng số véc tơ thu được có bằng $n$ không. Nếu thiếu → **không chéo hóa được**.
4. Lập $P$ có **các cột là các véc tơ riêng**; $D$ là ma trận chéo có các trị riêng **theo đúng thứ tự** tương ứng.

**Ví dụ 4.8.** Chéo hóa ma trận ở Ví dụ 4.7.

*Lời giải.* Ta có 3 véc tơ riêng ĐLTT: $(1, 0, 0)$, $(0, 1, 1)$ ứng với $\lambda = 2$ và $(0, 1, -1)$ ứng với $\lambda = 0$. Vậy $A$ chéo hóa được với:

$$P = \begin{pmatrix} 1 & 0 & 0 \\ 0 & 1 & 1 \\ 0 & 1 & -1 \end{pmatrix}, \qquad P^{-1}AP = D = \begin{pmatrix} 2 & 0 & 0 \\ 0 & 2 & 0 \\ 0 & 0 & 0 \end{pmatrix}$$

**Ví dụ 4.9 (không chéo hóa được).** $A = \begin{pmatrix} 1 & 1 \\ 0 & 1 \end{pmatrix}$.

*Lời giải.* Ma trận tam giác nên $\lambda = 1$ (bội 2). Nhưng $A - I = \begin{pmatrix} 0 & 1 \\ 0 & 0 \end{pmatrix}$ cho $y = 0$, nên $E(1) = \text{span}\{(1, 0)\}$ chỉ có số chiều 1 < 2. **Không chéo hóa được.**

#### Ứng dụng: tính lũy thừa $A^n$

Từ $P^{-1}AP = D$ suy ra $A = PDP^{-1}$, do đó:

$$A^n = (PDP^{-1})(PDP^{-1})\cdots(PDP^{-1}) = PD^nP^{-1}$$

Mà lũy thừa ma trận chéo thì chỉ cần lũy thừa từng phần tử trên đường chéo.

**Ví dụ 4.10.** Tính $A^n$ với $A = \begin{pmatrix} 4 & 1 \\ 2 & 3 \end{pmatrix}$.

*Lời giải.* Từ Ví dụ 4.6:

$$P = \begin{pmatrix} 1 & 1 \\ -2 & 1 \end{pmatrix}, \quad D = \begin{pmatrix} 2 & 0 \\ 0 & 5 \end{pmatrix}, \quad P^{-1} = \frac{1}{3}\begin{pmatrix} 1 & -1 \\ 2 & 1 \end{pmatrix}$$

$$A^n = PD^nP^{-1} = \frac{1}{3}\begin{pmatrix} 1 & 1 \\ -2 & 1 \end{pmatrix}\begin{pmatrix} 2^n & 0 \\ 0 & 5^n \end{pmatrix}\begin{pmatrix} 1 & -1 \\ 2 & 1 \end{pmatrix} = \frac{1}{3}\begin{pmatrix} 2^n + 2 \cdot 5^n & -2^n + 5^n \\ -2 \cdot 2^n + 2 \cdot 5^n & 2 \cdot 2^n + 5^n \end{pmatrix}$$

**Tự kiểm tra với $n = 1$:** hàng 1 cho $\frac{2 + 10}{3} = 4$ và $\frac{-2 + 5}{3} = 1$ ✓. Luôn thử $n = 1$ để bắt lỗi.

> 🤖 **Liên hệ AI:** Trị riêng và véc tơ riêng là trái tim của **PCA** (phân tích thành phần chính) — véc tơ riêng của ma trận hiệp phương sai chỉ ra các hướng dữ liệu biến thiên nhiều nhất. Thuật toán **PageRank** của Google cũng là bài toán tìm véc tơ riêng.
>
> 🎮 **Liên hệ game:** Trong mô phỏng vật lý, véc tơ riêng của **tenxơ quán tính** cho biết các trục quay "tự nhiên" của vật thể.

#### 📝 Dạng bài hay gặp (Chương 4)

- Chứng minh một ánh xạ là tuyến tính; tìm ma trận chính tắc.
- Tìm cơ sở, số chiều của $\text{Ker} f$, $\text{Im} f$; xét đơn ánh, toàn ánh.
- Tìm ma trận của $f$ trong cơ sở khác (dùng "câu thần chú" hoặc $A' = P^{-1}AP$).
- Tìm trị riêng, véc tơ riêng; xét tính chéo hóa được; chéo hóa.
- Tính $A^n$; tìm $m$ để ma trận chéo hóa được.

---

### Bài tập tự luyện Chương 4

**Bài 4.1.** Cho $f(x, y) = (x + 2y,\; 3x - y)$. Chứng minh $f$ tuyến tính, tìm ma trận chính tắc và cho biết $f$ có phải đẳng cấu không.

**Bài 4.2.** Cho $f: \mathbb{R}^3 \to \mathbb{R}^2$, $f(x, y, z) = (x + y + z,\; x - z)$. Tìm cơ sở và số chiều của $\text{Ker} f$ và $\text{Im} f$.

**Bài 4.3.** Chéo hóa $A = \begin{pmatrix} 1 & 2 \\ 2 & 1 \end{pmatrix}$, rồi tính $A^n$.

**Bài 4.4.** Ma trận $A = \begin{pmatrix} 3 & 1 \\ 0 & 3 \end{pmatrix}$ có chéo hóa được không? Vì sao?

**Bài 4.5.** Chéo hóa $A = \begin{pmatrix} 3 & -2 & 0 \\ -2 & 3 & 0 \\ 0 & 0 & 5 \end{pmatrix}$.

<details>
<summary><b>Đáp án Chương 4</b> (bấm để mở)</summary>

**4.1.** Mỗi thành phần là bậc nhất, không có hằng số. Ma trận $A = \begin{pmatrix} 1 & 2 \\ 3 & -1 \end{pmatrix}$, $\det A = -7 \ne 0$ nên $\text{Ker} f = \{\theta\}$ và $f$ là đẳng cấu.

**4.2.** Giải $x + y + z = 0$, $x - z = 0$: $x = z = t$, $y = -2t$. $\text{Ker} f = \text{span}\{(1, -2, 1)\}$, số chiều bằng 1. Theo định lý số chiều, $\dim \text{Im} f = 3 - 1 = 2$, tức $\text{Im} f = \mathbb{R}^2$ (cơ sở chính tắc). $f$ toàn ánh nhưng không đơn ánh.

**4.3.** Phương trình đặc trưng $(1 - \lambda)^2 - 4 = 0$ cho $\lambda = 3$ và $\lambda = -1$. Véc tơ riêng lần lượt là $(1, 1)$ và $(1, -1)$.

$P = \begin{pmatrix} 1 & 1 \\ 1 & -1 \end{pmatrix}$, $D = \begin{pmatrix} 3 & 0 \\ 0 & -1 \end{pmatrix}$, và

$$A^n = \frac{1}{2}\begin{pmatrix} 3^n + (-1)^n & 3^n - (-1)^n \\ 3^n - (-1)^n & 3^n + (-1)^n \end{pmatrix}$$

**4.4.** Không. $\lambda = 3$ bội 2 nhưng $A - 3I = \begin{pmatrix} 0 & 1 \\ 0 & 0 \end{pmatrix}$ cho $\dim E(3) = 1 < 2$.

**4.5.** $\det(A - \lambda I) = -(\lambda - 5)^2(\lambda - 1)$.

- $\lambda = 5$ (bội 2): $x + y = 0$, $z$ tự do. $E(5) = \text{span}\{(1, -1, 0), (0, 0, 1)\}$, số chiều 2 ✓.
- $\lambda = 1$: $x = y$, $z = 0$. $E(1) = \text{span}\{(1, 1, 0)\}$.

$P = \begin{pmatrix} 1 & 0 & 1 \\ -1 & 0 & 1 \\ 0 & 1 & 0 \end{pmatrix}$, $D = \text{diag}(5, 5, 1)$.

</details>

---

## CHƯƠNG 5. KHÔNG GIAN EUCLIDE – DẠNG TOÀN PHƯƠNG

> Đến Chương 4, ta mới chỉ biết cộng và co giãn véc tơ. Chương 5 bổ sung thêm khả năng **đo độ dài và góc** nhờ tích vô hướng — thứ mà game và AI dùng liên tục.

### 5.1 Tích vô hướng – Không gian Euclide

**Định nghĩa.** **Tích vô hướng** trên KGVT $V$ là một quy tắc cho mỗi cặp $u, v$ một số thực $\langle u, v \rangle$, thỏa mãn với mọi $u, v, w \in V$, $k \in \mathbb{R}$:

1. $\langle u, v \rangle = \langle v, u \rangle$ (đối xứng)
2. $\langle u + v, w \rangle = \langle u, w \rangle + \langle v, w \rangle$
3. $\langle ku, v \rangle = k\langle u, v \rangle$
4. $\langle u, u \rangle \ge 0$, và $\langle u, u \rangle = 0 \iff u = \theta$ (xác định dương)

KGVT có tích vô hướng gọi là **không gian Euclide**.

**Hai tích vô hướng hay gặp nhất:**

- Trên $\mathbb{R}^n$ (**chính tắc**): $\langle x, y \rangle = x_1 y_1 + x_2 y_2 + \dots + x_n y_n$
- Trên $P_n[x]$: $\langle p, q \rangle = \displaystyle\int_a^b p(x) q(x)\, dx$ (thường lấy $[a, b] = [0, 1]$ hoặc $[-1, 1]$)

#### Độ dài, góc, khoảng cách

$$\|u\| = \sqrt{\langle u, u \rangle}, \qquad d(u, v) = \|u - v\|, \qquad \cos\theta = \frac{\langle u, v \rangle}{\|u\| \, \|v\|}$$

**Bất đẳng thức Cauchy–Schwarz** (đảm bảo $\cos\theta$ luôn nằm trong $[-1, 1]$):

$$|\langle u, v \rangle| \le \|u\| \cdot \|v\|$$

**Ví dụ 5.1.** Trong $\mathbb{R}^3$ với tích vô hướng chính tắc, tính góc giữa $u = (1, 2, 2)$ và $v = (3, 0, 4)$.

*Lời giải.* $\langle u, v \rangle = 3 + 0 + 8 = 11$, $\|u\| = \sqrt{1 + 4 + 4} = 3$, $\|v\| = \sqrt{9 + 16} = 5$. Vậy $\cos\theta = \dfrac{11}{15}$.

**Ví dụ 5.2.** Trong $P_1[x]$ với $\langle p, q \rangle = \int_0^1 p(x)q(x)\,dx$, tính góc giữa $p = 1$ và $q = x$.

*Lời giải.*

$$\langle p, q \rangle = \int_0^1 x\, dx = \frac{1}{2}, \quad \|p\| = \sqrt{\int_0^1 1\, dx} = 1, \quad \|q\| = \sqrt{\int_0^1 x^2\, dx} = \frac{1}{\sqrt{3}}$$

$$\cos\theta = \frac{1/2}{1 \cdot 1/\sqrt{3}} = \frac{\sqrt{3}}{2} \Rightarrow \theta = \frac{\pi}{6}$$

> Ví dụ này cho thấy sức mạnh của tư duy trừu tượng: "góc giữa hai đa thức" nghe lạ, nhưng mọi công thức vẫn dùng y hệt.

---

### 5.2 Trực giao – Quá trình Gram–Schmidt

- $u$ **trực giao** với $v$ ($u \perp v$) nếu $\langle u, v \rangle = 0$.
- Hệ **trực giao**: các véc tơ đôi một trực giao. Hệ trực giao không chứa $\theta$ thì **luôn ĐLTT**.
- Hệ **trực chuẩn**: trực giao và mọi véc tơ có độ dài 1.

**Vì sao cơ sở trực chuẩn tuyệt vời?** Với cơ sở trực chuẩn $\{e_1, \dots, e_n\}$, tìm tọa độ **không cần giải hệ**:

$$x = \langle x, e_1 \rangle e_1 + \langle x, e_2 \rangle e_2 + \dots + \langle x, e_n \rangle e_n$$

#### Quá trình trực giao hóa Gram–Schmidt

Từ hệ ĐLTT $\{v_1, \dots, v_n\}$, dựng hệ trực giao $\{u_1, \dots, u_n\}$:

$$
\begin{aligned}
u_1 &= v_1 \\
u_2 &= v_2 - \frac{\langle v_2, u_1 \rangle}{\langle u_1, u_1 \rangle} u_1 \\
u_3 &= v_3 - \frac{\langle v_3, u_1 \rangle}{\langle u_1, u_1 \rangle} u_1 - \frac{\langle v_3, u_2 \rangle}{\langle u_2, u_2 \rangle} u_2 \\
&\;\;\vdots
\end{aligned}
$$

Cuối cùng **chuẩn hóa**: $e_i = \dfrac{u_i}{\|u_i\|}$.

**Ý tưởng:** mỗi bước lấy véc tơ mới **trừ đi phần "bóng" (hình chiếu)** của nó trên các véc tơ đã có, phần còn lại sẽ vuông góc với tất cả.

> **Mẹo tính tay:** Sau mỗi bước, được phép **nhân $u_i$ với một số khác 0** để khử phân số. Hướng không đổi nên kết quả vẫn đúng.

**Ví dụ 5.3.** Trực chuẩn hóa hệ $v_1 = (1, 1, 0)$, $v_2 = (1, 0, 1)$, $v_3 = (0, 1, 1)$.

*Lời giải.*

- $u_1 = (1, 1, 0)$, $\|u_1\|^2 = 2$.
- $u_2 = (1, 0, 1) - \dfrac{1}{2}(1, 1, 0) = \left(\dfrac{1}{2}, -\dfrac{1}{2}, 1\right)$. Nhân 2 để gọn: $u_2 = (1, -1, 2)$, $\|u_2\|^2 = 6$.
- $\langle v_3, u_1 \rangle = 1$, $\langle v_3, u_2 \rangle = 0 - 1 + 2 = 1$:

$$u_3 = (0, 1, 1) - \frac{1}{2}(1, 1, 0) - \frac{1}{6}(1, -1, 2) = \left(-\frac{2}{3}, \frac{2}{3}, \frac{2}{3}\right)$$

Nhân $\frac{3}{2}$: $u_3 = (-1, 1, 1)$.

**Kiểm tra trực giao:** $\langle u_1, u_3 \rangle = -1 + 1 + 0 = 0$ ✓, $\langle u_2, u_3 \rangle = -1 - 1 + 2 = 0$ ✓.

**Chuẩn hóa:**

$$e_1 = \frac{1}{\sqrt{2}}(1, 1, 0), \quad e_2 = \frac{1}{\sqrt{6}}(1, -1, 2), \quad e_3 = \frac{1}{\sqrt{3}}(-1, 1, 1)$$

#### Hình chiếu trực giao lên không gian con

Nếu $\{e_1, \dots, e_k\}$ là cơ sở **trực chuẩn** của không gian con $W$, hình chiếu của $x$ lên $W$ là:

$$\text{proj}_W(x) = \langle x, e_1 \rangle e_1 + \dots + \langle x, e_k \rangle e_k$$

Véc tơ $x - \text{proj}_W(x)$ vuông góc với $W$, và **khoảng cách từ $x$ đến $W$** bằng $\|x - \text{proj}_W(x)\|$.

**Ví dụ 5.4.** Tìm hình chiếu của $x = (1, 2, 3)$ lên $W = \text{span}\{(1, 1, 0), (1, -1, 2)\}$ và khoảng cách từ $x$ đến $W$.

*Lời giải.* Từ Ví dụ 5.3, $e_1 = \frac{1}{\sqrt{2}}(1, 1, 0)$, $e_2 = \frac{1}{\sqrt{6}}(1, -1, 2)$ là cơ sở trực chuẩn của $W$.

$$\langle x, e_1 \rangle e_1 = \frac{3}{2}(1, 1, 0), \qquad \langle x, e_2 \rangle e_2 = \frac{5}{6}(1, -1, 2)$$

$$\text{proj}_W(x) = \left(\frac{7}{3}, \frac{2}{3}, \frac{5}{3}\right), \qquad x - \text{proj}_W(x) = \frac{4}{3}(-1, 1, 1)$$

Khoảng cách: $\left\|\frac{4}{3}(-1, 1, 1)\right\| = \dfrac{4\sqrt{3}}{3}$.

> 🤖 **Liên hệ AI:** Bài toán **bình phương tối thiểu** (hồi quy tuyến tính) thực chất là tìm hình chiếu của véc tơ dữ liệu lên một không gian con. Ngoài ra, **cosine similarity** — độ đo phổ biến để so sánh hai embedding — chính là $\cos\theta$ ở mục 5.1.

---

### 5.3 Ma trận trực giao – Chéo hóa trực giao

**Ma trận trực giao:** ma trận vuông $P$ thỏa $P^T P = I$, tức là:

$$P^{-1} = P^T \iff \text{các cột của } P \text{ là một hệ trực chuẩn}$$

(Tính nghịch đảo chỉ bằng chuyển vị — rất tiện! Ma trận quay $R_\alpha$ ở Ví dụ 4.3 là một ma trận trực giao.)

#### Định lý về ma trận đối xứng thực

Nếu $A$ là ma trận **đối xứng** thực thì:

1. Mọi trị riêng của $A$ đều là số thực.
2. Véc tơ riêng ứng với các trị riêng khác nhau thì **trực giao** với nhau.
3. $A$ **luôn chéo hóa trực giao được**: tồn tại ma trận trực giao $P$ để $P^T A P = D$ chéo.

#### Quy trình chéo hóa trực giao

1. Tìm các trị riêng.
2. Với mỗi trị riêng, tìm cơ sở của không gian riêng.
3. **Nếu không gian riêng có số chiều $\ge 2$:** dùng Gram–Schmidt **bên trong** không gian riêng đó. (Véc tơ thuộc các không gian riêng khác nhau đã tự động trực giao.)
4. Chuẩn hóa tất cả các véc tơ.
5. Lập $P$ với các cột là các véc tơ trực chuẩn đó. Khi đó $P^T A P = D$.

**Ví dụ 5.5.** Chéo hóa trực giao $A = \begin{pmatrix} 2 & 1 & 1 \\ 1 & 2 & 1 \\ 1 & 1 & 2 \end{pmatrix}$.

*Lời giải.*

*Trị riêng:* Để ý tổng mỗi hàng bằng 4, nên $A(1, 1, 1)^T = 4(1, 1, 1)^T$, tức $\lambda = 4$ là trị riêng. Vết bằng 6 và $\det A = 4$, nên hai trị riêng còn lại có tổng 2, tích 1, tức là $\lambda = 1$ (bội 2). (Bạn có thể tự kiểm tra bằng cách giải $\det(A - \lambda I) = 0$.)

*$\lambda = 4$:* $E(4) = \text{span}\{(1, 1, 1)\}$.

*$\lambda = 1$:* $A - I$ có ba hàng đều là $(1, 1, 1)$, nên chỉ còn phương trình $x + y + z = 0$. Một cơ sở: $v_1 = (1, -1, 0)$, $v_2 = (1, 0, -1)$. Hai véc tơ này **chưa trực giao**, nên dùng Gram–Schmidt:

$$u_1 = (1, -1, 0), \quad u_2 = (1, 0, -1) - \frac{1}{2}(1, -1, 0) = \left(\frac{1}{2}, \frac{1}{2}, -1\right) \to (1, 1, -2)$$

*Chuẩn hóa và lập $P$:*

$$P = \begin{pmatrix} \frac{1}{\sqrt{2}} & \frac{1}{\sqrt{6}} & \frac{1}{\sqrt{3}} \\[4pt] -\frac{1}{\sqrt{2}} & \frac{1}{\sqrt{6}} & \frac{1}{\sqrt{3}} \\[4pt] 0 & -\frac{2}{\sqrt{6}} & \frac{1}{\sqrt{3}} \end{pmatrix}, \qquad P^T A P = \begin{pmatrix} 1 & 0 & 0 \\ 0 & 1 & 0 \\ 0 & 0 & 4 \end{pmatrix}$$

---

### 5.4 Dạng song tuyến tính và dạng toàn phương

**Dạng song tuyến tính** $\varphi(u, v)$ là ánh xạ $V \times V \to \mathbb{R}$ tuyến tính theo **từng biến** khi cố định biến kia. Nó **đối xứng** nếu $\varphi(u, v) = \varphi(v, u)$.

**Dạng toàn phương** sinh bởi dạng song tuyến tính đối xứng $\varphi$ là $Q(x) = \varphi(x, x)$.

Trong $\mathbb{R}^n$, mọi dạng toàn phương đều viết được dưới dạng:

$$Q(x) = x^T A x = \sum_{i,j} a_{ij} x_i x_j, \qquad A \text{ đối xứng}$$

#### Cách lập ma trận của dạng toàn phương

- Hệ số của $x_i^2$ → đặt vào **đường chéo** $a_{ii}$.
- Hệ số của $x_i x_j$ ($i \ne j$) → **chia đôi**, đặt vào cả $a_{ij}$ và $a_{ji}$.

> ⚠️ Quên chia đôi hệ số của số hạng chéo là lỗi **phổ biến nhất** của chương này.

**Ví dụ 5.6.** $Q(x, y, z) = x^2 + 2y^2 + 3z^2 + 2xy + 4xz$ có ma trận

$$A = \begin{pmatrix} 1 & 1 & 2 \\ 1 & 2 & 0 \\ 2 & 0 & 3 \end{pmatrix}$$

(Hệ số của $xy$ là 2 → mỗi ô $a_{12}, a_{21}$ nhận 1. Hệ số của $xz$ là 4 → mỗi ô $a_{13}, a_{31}$ nhận 2. Không có $yz$ → $a_{23} = a_{32} = 0$.)

**Dạng chính tắc:** dạng toàn phương chỉ còn các bình phương, không có số hạng chéo:

$$Q = \lambda_1 y_1^2 + \lambda_2 y_2^2 + \dots + \lambda_n y_n^2$$

---

### 5.5 Đưa dạng toàn phương về dạng chính tắc

#### Cách 1: Phương pháp Lagrange (gom bình phương)

Chọn một biến có bình phương, gom **tất cả** các số hạng chứa biến đó thành một bình phương đủ, rồi lặp lại với phần còn lại.

**Ví dụ 5.7.** Đưa $Q = x^2 + 2y^2 + 3z^2 + 2xy + 4xz$ về dạng chính tắc.

*Lời giải.* Gom các số hạng chứa $x$: $x^2 + 2xy + 4xz = (x + y + 2z)^2 - y^2 - 4z^2 - 4yz$. Do đó:

$$Q = (x + y + 2z)^2 + y^2 - 4yz - z^2$$

Gom các số hạng chứa $y$: $y^2 - 4yz = (y - 2z)^2 - 4z^2$. Do đó:

$$Q = (x + y + 2z)^2 + (y - 2z)^2 - 5z^2$$

Đổi biến $y_1 = x + y + 2z$, $y_2 = y - 2z$, $y_3 = z$, ta được dạng chính tắc:

$$\boxed{Q = y_1^2 + y_2^2 - 5y_3^2}$$

> **Trường hợp không có bình phương nào**, ví dụ $Q = 2xy$: đổi biến $x = u + v$, $y = u - v$ để tạo bình phương, được $Q = 2u^2 - 2v^2$.

#### Cách 2: Chéo hóa trực giao

Đặt $x = Py$ với $P$ là ma trận trực giao chéo hóa $A$. Khi đó:

$$Q = x^T A x = y^T (P^T A P) y = \lambda_1 y_1^2 + \dots + \lambda_n y_n^2$$

**Các hệ số chính tắc chính là các trị riêng.** Cách này dài hơn Lagrange, nhưng phép đổi biến là phép quay (giữ nguyên hình dạng), nên dùng được để **nhận dạng đường/mặt bậc hai**.

**Ví dụ 5.8.** Đưa $Q = 5x^2 + 4xy + 2y^2$ về chính tắc bằng phép biến đổi trực giao.

*Lời giải.* $A = \begin{pmatrix} 5 & 2 \\ 2 & 2 \end{pmatrix}$. Phương trình đặc trưng: $\lambda^2 - 7\lambda + 6 = 0$, nên $\lambda = 1$ và $\lambda = 6$.

- $\lambda = 1$: $4x + 2y = 0$, véc tơ riêng chuẩn hóa $\frac{1}{\sqrt{5}}(1, -2)$.
- $\lambda = 6$: $-x + 2y = 0$, véc tơ riêng chuẩn hóa $\frac{1}{\sqrt{5}}(2, 1)$.

Với $P = \dfrac{1}{\sqrt{5}}\begin{pmatrix} 1 & 2 \\ -2 & 1 \end{pmatrix}$ và phép đổi biến $\begin{pmatrix} x \\ y \end{pmatrix} = P \begin{pmatrix} y_1 \\ y_2 \end{pmatrix}$:

$$Q = y_1^2 + 6y_2^2$$

#### Luật quán tính Sylvester

Dù đưa về chính tắc bằng cách nào, **số hệ số dương, số hệ số âm và số hệ số bằng 0 luôn không đổi**. (Giá trị cụ thể của các hệ số thì có thể khác nhau.)

Ví dụ: ở Ví dụ 5.7, Lagrange cho các hệ số $1, 1, -5$ (hai dương, một âm). Nếu tính trị riêng của ma trận đó, bạn cũng sẽ được hai trị riêng dương và một trị riêng âm.

---

### 5.6 Dạng toàn phương xác định dương

**Định nghĩa.** Dạng toàn phương $Q$ là:

- **Xác định dương** nếu $Q(x) > 0$ với mọi $x \ne \theta$
- **Xác định âm** nếu $Q(x) < 0$ với mọi $x \ne \theta$
- **Không xác định** nếu $Q$ nhận cả giá trị dương lẫn âm

#### Ba cách kiểm tra xác định dương

| Cách | Điều kiện xác định dương |
|---|---|
| Dạng chính tắc | Mọi hệ số đều dương |
| Trị riêng | Mọi trị riêng của $A$ đều dương |
| **Tiêu chuẩn Sylvester** | Mọi **định thức con chính góc trái** đều dương |

Các định thức con chính góc trái của $A$ cấp $n$:

$$D_1 = a_{11}, \quad D_2 = \begin{vmatrix} a_{11} & a_{12} \\ a_{21} & a_{22} \end{vmatrix}, \quad \dots, \quad D_n = \det A$$

- **Xác định dương** $\iff D_1 > 0, D_2 > 0, \dots, D_n > 0$
- **Xác định âm** $\iff$ dấu xen kẽ bắt đầu bằng âm: $D_1 < 0, D_2 > 0, D_3 < 0, \dots$

**Tiêu chuẩn Sylvester là công cụ chính cho bài "tìm $m$".**

**Ví dụ 5.9.** Dạng toàn phương ở Ví dụ 5.6 có $D_1 = 1 > 0$, $D_2 = \begin{vmatrix} 1 & 1 \\ 1 & 2 \end{vmatrix} = 1 > 0$, nhưng $D_3 = \det A = -5 < 0$. Vậy $Q$ **không** xác định dương — khớp với dạng chính tắc có hệ số $-5$.

**Ví dụ 5.10.** Tìm $m$ để $Q = 2x^2 + y^2 + z^2 + 2mxy + 2xz$ xác định dương.

*Lời giải.*

$$A = \begin{pmatrix} 2 & m & 1 \\ m & 1 & 0 \\ 1 & 0 & 1 \end{pmatrix}$$

- $D_1 = 2 > 0$ luôn đúng.
- $D_2 = 2 - m^2 > 0 \iff -\sqrt{2} < m < \sqrt{2}$.
- $D_3 = 2(1 - 0) - m(m - 0) + 1(0 - 1) = 1 - m^2 > 0 \iff -1 < m < 1$.

Kết hợp các điều kiện: $Q$ xác định dương $\iff -1 < m < 1$.

---

### 5.7 Nhận dạng đường bậc hai

Phương trình $ax^2 + 2bxy + cy^2 = d$ (với $d > 0$) được nhận dạng bằng cách chéo hóa trực giao vế trái thành $\lambda_1 y_1^2 + \lambda_2 y_2^2$:

| Dấu của $\lambda_1, \lambda_2$ | Đường cong |
|---|---|
| Cùng dương | **Elip** |
| Trái dấu | **Hyperbol** |
| Một trị riêng bằng 0 | Cặp đường thẳng song song (hoặc suy biến) |

**Ví dụ 5.11.** Nhận dạng đường cong $5x^2 + 4xy + 2y^2 = 6$.

*Lời giải.* Theo Ví dụ 5.8, sau phép quay $x = Py$, phương trình trở thành:

$$y_1^2 + 6y_2^2 = 6 \iff \frac{y_1^2}{6} + \frac{y_2^2}{1} = 1$$

Đây là **elip** với bán trục $\sqrt{6}$ theo hướng $(1, -2)$ và bán trục $1$ theo hướng $(2, 1)$.

Ý tưởng tương tự áp dụng cho **mặt bậc hai** trong không gian ba chiều (mặt elipxoit, hyperboloit...).

> 🎮 **Liên hệ game:** Tích vô hướng xuất hiện ở khắp nơi:
> - **Chiếu sáng:** độ sáng bề mặt tỉ lệ với $\langle n, l \rangle$ (pháp tuyến và hướng ánh sáng, đã chuẩn hóa).
> - **Kẻ địch ở phía trước hay phía sau?** Xét dấu của tích vô hướng giữa hướng nhìn và hướng tới kẻ địch.
> - **Camera:** việc dựng hệ trục "trước – phải – trên" vuông góc cho camera dùng đúng tinh thần của Gram–Schmidt.
>
> 🤖 **Liên hệ AI:** Ma trận Hessian **xác định dương** cho biết hàm số lồi tại điểm đó — đây là nền tảng lý thuyết của tối ưu hóa khi huấn luyện mô hình.

#### 📝 Dạng bài hay gặp (Chương 5)

- Kiểm tra một biểu thức có phải tích vô hướng không; tính độ dài, góc (kể cả trong $P_n[x]$ với tích phân).
- Trực chuẩn hóa Gram–Schmidt; tìm hình chiếu, khoảng cách đến không gian con.
- Chéo hóa trực giao ma trận đối xứng.
- Lập ma trận dạng toàn phương; đưa về chính tắc (Lagrange hoặc trực giao).
- Tìm $m$ để dạng toàn phương xác định dương (Sylvester).
- Nhận dạng đường/mặt bậc hai.

---

### Bài tập tự luyện Chương 5

**Bài 5.1.** Tính góc giữa $u = (1, 0, 1)$ và $v = (0, 1, 1)$ trong $\mathbb{R}^3$.

**Bài 5.2.** Trực chuẩn hóa hệ $\{(1, 0, 1),\; (1, 1, 0)\}$.

**Bài 5.3.** Đưa $Q = x^2 + 2xy + 2y^2 + 2yz + 3z^2$ về dạng chính tắc bằng Lagrange. $Q$ có xác định dương không? Kiểm tra lại bằng Sylvester.

**Bài 5.4.** Nhận dạng đường cong $x^2 + 4xy + y^2 = 3$.

**Bài 5.5.** Tìm $m$ để $Q = x^2 + y^2 + 5z^2 + 2mxy - 2xz + 4yz$ xác định dương.

<details>
<summary><b>Đáp án Chương 5</b> (bấm để mở)</summary>

**5.1.** $\cos\theta = \dfrac{1}{\sqrt{2}\cdot\sqrt{2}} = \dfrac{1}{2}$, nên $\theta = 60°$.

**5.2.** $u_1 = (1, 0, 1)$; $u_2 = (1, 1, 0) - \frac{1}{2}(1, 0, 1) = \left(\frac{1}{2}, 1, -\frac{1}{2}\right) \to (1, 2, -1)$. Hệ trực chuẩn: $\left\{\frac{1}{\sqrt{2}}(1, 0, 1),\; \frac{1}{\sqrt{6}}(1, 2, -1)\right\}$.

**5.3.** $Q = (x + y)^2 + (y + z)^2 + 2z^2$. Chính tắc: $y_1^2 + y_2^2 + 2y_3^2$, mọi hệ số dương nên **xác định dương**. Sylvester: $A = \begin{pmatrix} 1 & 1 & 0 \\ 1 & 2 & 1 \\ 0 & 1 & 3 \end{pmatrix}$ có $D_1 = 1$, $D_2 = 1$, $D_3 = 2$, đều dương ✓.

**5.4.** $A = \begin{pmatrix} 1 & 2 \\ 2 & 1 \end{pmatrix}$ có trị riêng $3$ và $-1$ (xem Bài 4.3). Sau phép quay: $3y_1^2 - y_2^2 = 3 \iff y_1^2 - \dfrac{y_2^2}{3} = 1$. Đây là **hyperbol**.

**5.5.** $A = \begin{pmatrix} 1 & m & -1 \\ m & 1 & 2 \\ -1 & 2 & 5 \end{pmatrix}$. $D_1 = 1 > 0$; $D_2 = 1 - m^2 > 0 \iff |m| < 1$; $D_3 = -m(5m + 4) > 0 \iff -\frac{4}{5} < m < 0$. Kết hợp: $-\dfrac{4}{5} < m < 0$.

</details>

---

## PHỤ LỤC A. BẢNG TRA NHANH: "BÀI NÀY QUY VỀ CÁI GÌ?"

Hầu hết các bài của môn Đại số đều quy về **ba thao tác của Chương 2**. Bảng dưới giúp bạn nhận ra nhanh:

| Câu hỏi | Quy về |
|---|---|
| Ma trận có khả nghịch không? | Tính $\det A$ |
| Hệ $n$ véc tơ trong $\mathbb{R}^n$ có ĐLTT / là cơ sở không? | Tính định thức |
| Hệ $m$ véc tơ có ĐLTT không? | Tìm hạng, so với $m$ |
| Số chiều và cơ sở của không gian sinh? | Tìm hạng (khử Gauss theo hàng) |
| Véc tơ có thuộc span không? Tìm tọa độ? | Giải hệ phương trình |
| Số chiều và cơ sở của không gian nghiệm? | Giải hệ thuần nhất; $\dim = n - r(A)$ |
| $\text{Ker} f$? | Giải hệ thuần nhất $AX = 0$ |
| $\dim \text{Im} f$? | Tìm hạng $r(A)$ |
| Trị riêng? | Giải $\det(A - \lambda I) = 0$ |
| Véc tơ riêng? | Giải hệ thuần nhất $(A - \lambda I)X = 0$ |
| Dạng toàn phương xác định dương? | Tính các định thức con chính (Sylvester) |

#### Các lỗi sai kinh điển cần tránh

1. Cho rằng $AB = BA$.
2. Viết $\det(kA) = k\det A$ (đúng phải là $k^n \det A$).
3. Quên chuyển vị khi dùng công thức phần phụ hợp để tìm nghịch đảo.
4. Biến đổi **cột** khi giải hệ phương trình.
5. Nghĩ $\dim P_n[x] = n$ (đúng là $n + 1$).
6. Quên điều kiện véc tơ riêng phải **khác 0**.
7. Xếp véc tơ riêng vào $P$ theo **hàng** thay vì theo **cột**, hoặc xếp sai thứ tự so với $D$.
8. Quên **chia đôi** hệ số số hạng chéo khi lập ma trận dạng toàn phương.
9. Dùng Gram–Schmidt trộn lẫn các không gian riêng khác nhau (không cần thiết) nhưng lại quên trực giao hóa **bên trong** một không gian riêng có số chiều $\ge 2$.

---

## PHỤ LỤC B. KIỂM TRA ĐÁP ÁN BẰNG PYTHON

Sau khi làm tay, dùng thư viện **SymPy** (tính toán chính xác, có phân số và căn) để kiểm tra. Cài đặt: `pip install sympy`.

```python
from sympy import Matrix, symbols, factor, eye, GramSchmidt

A = Matrix([[4, 1],
            [2, 3]])

print(A.det())            # Định thức
print(A.inv())            # Ma trận nghịch đảo
print(A.rank())           # Hạng
print(A.rref())           # Dạng bậc thang rút gọn (kèm vị trí các cột "bậc thang")
print(A.nullspace())      # Cơ sở của Ker (không gian nghiệm AX = 0)
print(A.columnspace())    # Cơ sở của Im

lam = symbols('lambda')
print(factor((A - lam * eye(2)).det()))   # Đa thức đặc trưng
print(A.eigenvects())     # [(trị riêng, bội, [véc tơ riêng...]), ...]

P, D = A.diagonalize()    # A = P * D * P^(-1)
print(P, D)

# Giải hệ AX = B
B = Matrix([1, 2])
print(A.solve(B))

# Gram–Schmidt (orthonormal=True để chuẩn hóa luôn)
vs = [Matrix([1, 1, 0]), Matrix([1, 0, 1]), Matrix([0, 1, 1])]
print(GramSchmidt(vs, orthonormal=True))

# Bài có tham số m
m = symbols('m')
M = Matrix([[1, 1, m], [1, m, 1], [m, 1, 1]])
print(factor(M.det()))    # -(m - 1)**2*(m + 2)
```

> **Lưu ý:** Máy có thể trả về véc tơ riêng khác với đáp án của bạn một hệ số nhân (ví dụ $(-\frac{1}{2}, 1)$ thay vì $(1, -2)$). Cả hai đều đúng vì cùng phương.

Khi chuyển sang lập trình game hoặc AI thực tế, bạn sẽ dùng **NumPy** (tính số thực, nhanh) thay cho SymPy: `numpy.linalg.det`, `numpy.linalg.inv`, `numpy.linalg.eig`, `numpy.linalg.solve`...

---

## PHỤ LỤC C. GỢI Ý LỘ TRÌNH VÀ TÀI LIỆU ĐI KÈM

### Lộ trình gợi ý (khoảng 6–8 tuần)

| Tuần | Nội dung | Mục tiêu tối thiểu |
|---|---|---|
| 1 | Chương 1 | Thành thạo số phức dạng lượng giác |
| 2–3 | Chương 2 | Tính định thức cấp 4, giải hệ Gauss, biện luận tham số **không cần nhìn tài liệu** |
| 4 | Chương 3 | Tìm cơ sở, số chiều, tọa độ |
| 5–6 | Chương 4 | Chéo hóa ma trận cấp 3 trọn vẹn |
| 7 | Chương 5 | Gram–Schmidt, Sylvester, Lagrange |
| 8 | Ôn tập | Làm đề thi các kỳ trước, bấm giờ |

### Tài liệu nên dùng song song

- **Bài giảng, slide và đề cương bài tập** của Khoa Toán – Tin, ĐH Bách khoa Hà Nội — nguồn chính thức, bám sát đề thi.
- **Bài giảng Đại số tuyến tính** của TS. Bùi Xuân Diệu (ĐHBK Hà Nội) — trình bày rõ ràng, nhiều ví dụ.
- **Đề thi giữa kỳ, cuối kỳ các năm trước** — tài liệu do các CLB hỗ trợ học tập của trường tổng hợp.
- **3Blue1Brown – Essence of Linear Algebra** (YouTube) — giúp "nhìn thấy" ý nghĩa hình học của ma trận, định thức, trị riêng. Rất hợp để xem trước khi học mỗi chương.
- **MIT 18.06 – Gilbert Strang** (MIT OpenCourseWare) — khi muốn đào sâu hơn, đặc biệt theo hướng ứng dụng.

### Bước tiếp theo cho hướng game và AI

Sau khi vững tài liệu này, hãy học thêm (theo thứ tự):

1. **Tích có hướng** (cross product) và **tọa độ thuần nhất** (ma trận $4 \times 4$ cho biến đổi 3D) — cho đồ họa và game.
2. **Quaternion** — cho phép quay 3D trong game engine.
3. **Phân tích SVD** và **PCA** — cho AI và xử lý dữ liệu.
4. **Giải tích nhiều biến** (gradient, Hessian) — kết hợp với đại số tuyến tính để hiểu cách huấn luyện mạng nơ-ron.

---

*Chúc bạn học tốt! Hãy nhớ: đại số tuyến tính không khó, nó chỉ cần **luyện tay đều đặn**.*
