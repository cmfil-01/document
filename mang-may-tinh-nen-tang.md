# MẠNG MÁY TÍNH — NỀN TẢNG VỮNG CHẮC

*Tài liệu tự học: hiểu từng tầng, chia mạng thành thạo, tự chẩn đoán sự cố và tự viết được chương trình mạng*

---

## 0. Cách dùng tài liệu này

### 0.1 Cấu trúc

| Phần | Chương | Nội dung |
|---|---|---|
| **I. Nền tảng** | 1–4 | Khái niệm, hiệu năng, mô hình phân tầng, tầng vật lý, tầng liên kết |
| **II. Tầng mạng** | 5–10 | IPv4, **chia mạng con**, IP header và ICMP, định tuyến, NAT và DHCP, IPv6 |
| **III. Tầng giao vận** | 11–12 | Cổng, UDP, TCP |
| **IV. Tầng ứng dụng** | 13–15 | DNS, HTTP/HTTPS, các giao thức khác |
| **V. Vận dụng** | 16–20 | Bảo mật cơ bản, công cụ chẩn đoán, hành trình một yêu cầu web, **lập trình socket**, mạng cho game |
| **Phụ lục** | A–D | Bảng cổng, bảng subnet, hiểu lầm phổ biến, lộ trình và tài liệu |

Mỗi chương đi theo mạch: **ý tưởng (bằng ví dụ đời thường) → cơ chế hoạt động → ví dụ cụ thể → điều hay nhầm → tự kiểm tra**. Cuối mỗi phần có **bài tập kèm đáp án**.

> ✅ **Về độ chính xác:** mọi phép tính chia mạng con, IPv6, phân mảnh gói tin và thuật toán định tuyến trong tài liệu đã được kiểm tra lại bằng thư viện `ipaddress` của Python. Các chương trình socket ở Chương 19 đã được biên dịch và chạy thật trên Linux. Riêng output của các lệnh như `ping`, `traceroute`, `dig` chỉ là **ví dụ minh họa** — trên máy bạn, địa chỉ và thời gian sẽ khác.

### 0.2 Đơn vị cần nắm trước

| Ký hiệu | Ý nghĩa | Lưu ý |
|---|---|---|
| **b** (bit) | Đơn vị nhỏ nhất: 0 hoặc 1 | Tốc độ mạng đo bằng **bit**/giây |
| **B** (byte) | 8 bit | Dung lượng file đo bằng **byte** |
| **Mbps** | Megabit/giây | Gói cước "100 Mbps" = 100 **megabit**/giây |
| **MB/s** | Megabyte/giây | Trình tải file thường hiển thị đơn vị này |

$$100 \text{ Mbps} = \frac{100}{8} = 12{,}5 \text{ MB/s}$$

Đây là lý do mạng "100 Mbps" chỉ tải được khoảng 12 MB mỗi giây — không phải nhà mạng "ăn gian".

---

# PHẦN I — NỀN TẢNG

## Chương 1. Mạng máy tính là gì?

### 1.1 Các thành phần

**Mạng máy tính** là tập hợp các thiết bị được kết nối để **trao đổi dữ liệu**.

| Thành phần | Ví dụ | Vai trò |
|---|---|---|
| **Thiết bị đầu cuối (host)** | Máy tính, điện thoại, server, máy chơi game | Tạo ra và tiêu thụ dữ liệu |
| **Đường truyền (link)** | Cáp đồng, cáp quang, sóng Wi-Fi, 4G/5G | Mang tín hiệu |
| **Switch (bộ chuyển mạch)** | Switch trong phòng máy | Nối các thiết bị **trong cùng một mạng** |
| **Router (bộ định tuyến)** | Router nhà mạng, router lõi Internet | Nối **các mạng khác nhau** với nhau |
| **Giao thức (protocol)** | IP, TCP, HTTP | **Bộ quy tắc** để hai bên hiểu nhau |

Router Wi-Fi ở nhà bạn thực ra là **nhiều thiết bị gộp lại**: router + switch vài cổng + điểm phát Wi-Fi + máy chủ DHCP + NAT (sẽ học ở các chương sau).

### 1.2 Phân loại theo phạm vi

| Loại | Phạm vi | Ví dụ |
|---|---|---|
| **PAN** | Vài mét quanh một người | Tai nghe Bluetooth |
| **LAN** | Một nhà, một tòa nhà | Mạng Wi-Fi gia đình, phòng máy |
| **MAN** | Một thành phố | Mạng cáp quang của một thành phố |
| **WAN** | Quốc gia, toàn cầu | Mạng của nhà mạng, **Internet** |

**Internet** là "mạng của các mạng": hàng chục nghìn mạng độc lập (của nhà mạng, công ty, trường học...) nối với nhau bằng cùng một bộ giao thức TCP/IP.

### 1.3 Mô hình giao tiếp

- **Client–server:** client gửi yêu cầu, server phục vụ. Ví dụ: trình duyệt ↔ web server, game client ↔ game server.
- **Peer-to-peer (P2P):** các máy ngang hàng, vừa là client vừa là server. Ví dụ: BitTorrent, một số game kết nối trực tiếp giữa người chơi.

### 1.4 Chuyển mạch gói

Internet dùng **chuyển mạch gói (packet switching)**: dữ liệu được cắt thành các **gói (packet)** nhỏ, mỗi gói tự tìm đường đến đích, và các gói của nhiều người **dùng chung** đường truyền.

```text
Chuyển mạch kênh (điện thoại cố định kiểu cũ):
  A ═══════════ đường dành riêng suốt cuộc gọi ═══════════ B     (lãng phí khi im lặng)

Chuyển mạch gói (Internet):
  A ─[1]─[2]─[3]─►  router  ─►  router  ─►  B                   (các gói xen kẽ với gói của người khác)
  C ─[x]─[y]──────►
```

Ưu điểm: tận dụng đường truyền rất hiệu quả. Nhược điểm: khi đông, gói phải **xếp hàng** (trễ) hoặc bị **bỏ** (mất gói).

### 1.5 Các chỉ số hiệu năng

| Chỉ số | Ý nghĩa | Ví dụ đời thường |
|---|---|---|
| **Băng thông (bandwidth)** | Tốc độ **tối đa** đường truyền cho phép | Số làn của con đường |
| **Thông lượng (throughput)** | Tốc độ **thực tế** đạt được | Số xe thực sự đi qua mỗi phút |
| **Độ trễ (latency)** | Thời gian một gói đi từ nguồn đến đích | Thời gian đi hết con đường |
| **Jitter** | Độ **dao động** của độ trễ | Lúc đi 10 phút, lúc đi 30 phút |
| **Mất gói (packet loss)** | Tỉ lệ gói không đến được đích | Xe bị lạc giữa đường |

**Bốn thành phần của độ trễ** tại mỗi chặng:

| Thành phần | Nguyên nhân | Phụ thuộc vào |
|---|---|---|
| **Xử lý (processing)** | Router đọc header, quyết định chuyển đi đâu | Sức mạnh của router |
| **Hàng đợi (queuing)** | Chờ các gói khác được gửi trước | Mức độ tắc nghẽn |
| **Truyền (transmission)** | Đẩy từng bit của gói lên dây | Kích thước gói $L$ và băng thông $R$: $d = L / R$ |
| **Lan truyền (propagation)** | Tín hiệu di chuyển trên dây | Khoảng cách $d$ và tốc độ tín hiệu $s$: $d / s$ |

**Ví dụ 1.1.** Tính:

(a) Thời gian tải file 10 MB qua đường truyền 100 Mbps (bỏ qua mọi độ trễ khác).
(b) Độ trễ lan truyền qua 3000 km cáp quang (tốc độ tín hiệu khoảng $2 \times 10^8$ m/s).
(c) Độ trễ truyền của một gói 1500 byte trên đường 10 Mbps.

*Lời giải.*

(a) $10 \text{ MB} = 80 \text{ Mb}$, nên thời gian $= 80 / 100 = 0{,}8$ giây.

(b) $\dfrac{3 \times 10^6 \text{ m}}{2 \times 10^8 \text{ m/s}} = 0{,}015 \text{ s} = 15 \text{ ms}$.

(c) $1500 \text{ byte} = 12\,000 \text{ bit}$, nên $\dfrac{12\,000}{10^7} = 0{,}0012 \text{ s} = 1{,}2 \text{ ms}$.

> 💡 **Bài học từ (b):** Độ trễ lan truyền bị giới hạn bởi **tốc độ ánh sáng** — không nâng cấp băng thông nào giảm được. Đó là lý do game thủ chọn server **gần mình về địa lý**.

### ✅ Tự kiểm tra

1. Gói cước 300 Mbps tải file 1,5 GB mất khoảng bao lâu (lý tưởng)?
2. Nâng băng thông từ 100 Mbps lên 1 Gbps có giảm được độ trễ lan truyền không?

<details><summary>Đáp án</summary>

1. $1{,}5 \text{ GB} = 12\,000 \text{ Mb}$, nên $12\,000 / 300 = 40$ giây.
2. Không. Độ trễ lan truyền chỉ phụ thuộc khoảng cách và tốc độ tín hiệu. Băng thông cao hơn chỉ giảm **độ trễ truyền** và giúp chở được nhiều dữ liệu hơn.

</details>

---

## Chương 2. Mô hình phân tầng: OSI và TCP/IP

### 2.1 Vì sao phải phân tầng?

Gửi một bức thư qua bưu điện: bạn **viết thư** → **cho vào phong bì, ghi địa chỉ** → bưu điện **phân loại, chọn tuyến** → xe **chở đi**. Mỗi bên chỉ lo phần việc của mình, và bạn không cần biết thư đi bằng xe tải hay máy bay.

Mạng máy tính cũng vậy: chia thành các **tầng**, mỗi tầng chỉ làm một việc và chỉ "nói chuyện" với tầng ngay trên và ngay dưới nó. Nhờ vậy, có thể thay Wi-Fi bằng cáp quang mà trình duyệt không cần sửa gì.

### 2.2 Hai mô hình

| # | Tầng OSI | Tầng TCP/IP | Chức năng | Giao thức tiêu biểu | Đơn vị dữ liệu (PDU) | Thiết bị |
|---|---|---|---|---|---|---|
| 7 | Ứng dụng (Application) | **Ứng dụng** | Dịch vụ cho người dùng | HTTP, DNS, SMTP, SSH | Thông điệp (message) | |
| 6 | Trình diễn (Presentation) | ↑ | Mã hóa, nén, định dạng | TLS, JPEG | | |
| 5 | Phiên (Session) | ↑ | Quản lý phiên làm việc | | | |
| 4 | Giao vận (Transport) | **Giao vận** | Truyền **giữa hai tiến trình**; tin cậy, luồng | TCP, UDP | **Segment** (TCP) / Datagram (UDP) | |
| 3 | Mạng (Network) | **Mạng (Internet)** | Định địa chỉ và **tìm đường** giữa các mạng | IP, ICMP | **Packet** | Router |
| 2 | Liên kết dữ liệu (Data Link) | **Liên kết** | Truyền **trong một mạng**, giữa hai thiết bị kề nhau | Ethernet, Wi-Fi | **Frame** | Switch |
| 1 | Vật lý (Physical) | ↓ | Truyền **bit** qua môi trường | Cáp, sóng vô tuyến | **Bit** | Hub, repeater |

- **OSI** (7 tầng) là mô hình **lý thuyết**, dùng để giảng dạy và nói chuyện ("lỗi ở tầng 2", "switch tầng 3").
- **TCP/IP** là mô hình **thực tế** của Internet. Tầng 5–6 của OSI được gộp vào tầng ứng dụng.

**Mẹo nhớ OSI từ dưới lên:** **P**hysical – **D**ata link – **N**etwork – **T**ransport – **S**ession – **P**resentation – **A**pplication → "**P**lease **D**o **N**ot **T**hrow **S**ausage **P**izza **A**way".

### 2.3 Đóng gói (encapsulation)

Khi gửi dữ liệu, mỗi tầng **thêm header của mình** vào phía trước dữ liệu nhận từ tầng trên. Bên nhận làm ngược lại: mỗi tầng **bóc header của mình** rồi chuyển phần còn lại lên trên.

```text
BÊN GỬI (đi xuống)                                                BÊN NHẬN (đi lên)

Ứng dụng :                              [ Dữ liệu HTTP ]            ▲ bóc
Giao vận :                     [ TCP ][ Dữ liệu HTTP ]              ▲ bóc TCP
Mạng     :               [ IP ][ TCP ][ Dữ liệu HTTP ]              ▲ bóc IP
Liên kết :     [ Ethernet ][ IP ][ TCP ][ Dữ liệu HTTP ][ FCS ]     ▲ bóc Ethernet
Vật lý   :     0110100101110010110... (tín hiệu điện / quang / sóng)
```

| Header | Chứa thông tin chính | Trả lời câu hỏi |
|---|---|---|
| Ethernet | Địa chỉ **MAC** nguồn/đích | Gửi cho **thiết bị kế bên** nào? |
| IP | Địa chỉ **IP** nguồn/đích | Gửi tới **máy nào** trên Internet? |
| TCP/UDP | Số **cổng (port)** nguồn/đích | Giao cho **chương trình nào** trên máy đó? |

**Ba lớp địa chỉ này là ý tưởng quan trọng nhất của toàn bộ môn mạng.** Hãy nhớ: MAC để đi **từng chặng**, IP để đi **từ đầu đến cuối**, port để tìm đúng **ứng dụng**.

### ✅ Tự kiểm tra

1. Switch thông thường làm việc ở tầng nào? Router ở tầng nào?
2. Dữ liệu ở tầng giao vận (TCP) được gọi là gì?
3. Trong ba loại địa chỉ MAC, IP, port — loại nào **thay đổi** ở mỗi chặng khi gói đi qua các router?

<details><summary>Đáp án</summary>

1. Switch: tầng 2 (liên kết). Router: tầng 3 (mạng).
2. Segment.
3. Địa chỉ MAC (header Ethernet được tạo mới ở mỗi chặng). Địa chỉ IP và port giữ nguyên từ đầu đến cuối (trừ khi đi qua NAT — Chương 9).

</details>

---

## Chương 3. Tầng vật lý

### 3.1 Môi trường truyền dẫn

| Môi trường | Đặc điểm | Dùng ở đâu |
|---|---|---|
| **Cáp xoắn đôi (UTP)** | Rẻ, dễ lắp; tối đa khoảng 100 m | Mạng LAN trong nhà, văn phòng |
| **Cáp quang (fiber)** | Truyền bằng ánh sáng; rất nhanh, rất xa, không bị nhiễu điện từ | Đường trục, cáp quang về nhà (FTTH), cáp biển |
| **Sóng vô tuyến** | Không cần dây; dễ bị nhiễu, bị vật cản | Wi-Fi, Bluetooth, 4G/5G |

**Cáp xoắn đôi thường gặp:**

| Loại | Tốc độ tiêu biểu | Khoảng cách |
|---|---|---|
| Cat5e | 1 Gbps | 100 m |
| Cat6 | 10 Gbps | khoảng 55 m (1 Gbps thì 100 m) |
| Cat6a | 10 Gbps | 100 m |

**Cáp quang:** **đơn mode (single-mode)** đi được hàng chục km, dùng cho đường dài; **đa mode (multi-mode)** rẻ hơn, dùng trong tòa nhà, trung tâm dữ liệu.

### 3.2 Wi-Fi

| Tên thương mại | Chuẩn | Băng tần |
|---|---|---|
| Wi-Fi 4 | 802.11n | 2,4 GHz và 5 GHz |
| Wi-Fi 5 | 802.11ac | 5 GHz |
| Wi-Fi 6 / 6E | 802.11ax | 2,4 / 5 GHz (6E thêm 6 GHz) |
| Wi-Fi 7 | 802.11be | 2,4 / 5 / 6 GHz |

- **2,4 GHz:** đi xa, xuyên tường tốt hơn, nhưng chậm hơn và dễ nhiễu (lò vi sóng, Bluetooth, hàng xóm...).
- **5 GHz / 6 GHz:** nhanh hơn, ít nhiễu hơn, nhưng tầm phủ ngắn hơn.

> 🎮 **Cho game thủ:** Wi-Fi có jitter và mất gói cao hơn cáp mạng do nhiễu và cơ chế chia sẻ sóng. Khi chơi game cạnh tranh, **cắm dây** là cách cải thiện độ ổn định rẻ nhất.

### 3.3 Simplex, half-duplex, full-duplex

| Chế độ | Ý nghĩa | Ví dụ |
|---|---|---|
| Simplex | Chỉ một chiều | Phát thanh |
| Half-duplex | Hai chiều nhưng **lần lượt** | Bộ đàm |
| Full-duplex | Hai chiều **cùng lúc** | Điện thoại, Ethernet hiện đại |

**Hub** (thiết bị tầng 1, gần như không còn dùng) chỉ khuếch đại và phát tín hiệu ra **mọi cổng**, khiến mọi máy dùng chung một kênh half-duplex và dễ **xung đột (collision)**. Switch đã thay thế hoàn toàn hub.

---

## Chương 4. Tầng liên kết dữ liệu: Ethernet, MAC, switch, ARP, VLAN

### 4.1 Địa chỉ MAC

Mỗi card mạng có một **địa chỉ MAC** dài **48 bit**, viết dạng 6 cặp hex:

```text
    a4:5e:60:c2:13:8f
    └──┬───┘ └──┬───┘
      OUI      định danh do nhà sản xuất gán
 (mã nhà sản xuất)
```

- Dùng để giao frame **trong cùng một mạng LAN**.
- Địa chỉ **broadcast**: `ff:ff:ff:ff:ff:ff` — gửi cho **mọi máy** trong mạng LAN.
- Các hệ điều hành hiện đại có thể dùng **MAC ngẫu nhiên** cho từng mạng Wi-Fi để bảo vệ quyền riêng tư.

### 4.2 Cấu trúc frame Ethernet

```text
┌───────────┬──────────┬─────────┬───────────┬─────────────────────┬─────────┐
│ Preamble  │ MAC đích │ MAC     │ EtherType │ Dữ liệu (payload)   │   FCS   │
│ + SFD     │          │ nguồn   │           │                     │  (CRC)  │
│  8 byte   │  6 byte  │ 6 byte  │  2 byte   │   46 – 1500 byte    │ 4 byte  │
└───────────┴──────────┴─────────┴───────────┴─────────────────────┴─────────┘
```

- **EtherType** cho biết bên trong chứa gì: `0x0800` = IPv4, `0x86DD` = IPv6, `0x0806` = ARP.
- **FCS** (Frame Check Sequence) là mã **CRC** để **phát hiện lỗi**: bên nhận tính lại, nếu không khớp thì **bỏ frame**. Ethernet chỉ phát hiện lỗi, không sửa và không gửi lại — việc gửi lại (nếu cần) là của tầng trên (TCP).
- **MTU** (Maximum Transmission Unit) của Ethernet là **1500 byte** — kích thước tối đa của phần payload. Con số này sẽ gặp lại nhiều lần.

### 4.3 Switch hoạt động thế nào?

Switch duy trì một **bảng địa chỉ MAC** (MAC address table): "địa chỉ MAC nào nằm ở cổng nào".

**Quy trình khi nhận một frame:**

1. **Học (learn):** ghi lại *MAC nguồn* của frame ↔ *cổng* vừa nhận.
2. **Tra bảng** với *MAC đích*:
   - Có trong bảng → **chuyển tiếp (forward)** ra đúng một cổng.
   - Không có, hoặc là broadcast → **tràn (flood)** ra mọi cổng trừ cổng nhận.

**Ví dụ 4.1.** Switch 3 cổng, bảng ban đầu rỗng. Máy A (cổng 1), B (cổng 2), C (cổng 3).

| Bước | Sự kiện | Switch làm gì | Bảng MAC sau bước này |
|---|---|---|---|
| 1 | A gửi cho B | Học A ở cổng 1; chưa biết B → **flood** ra cổng 2, 3 | A → 1 |
| 2 | B trả lời A | Học B ở cổng 2; đã biết A → gửi **chỉ** ra cổng 1 | A → 1, B → 2 |
| 3 | A gửi cho B lần nữa | Đã biết B → gửi **chỉ** ra cổng 2 | A → 1, B → 2 |

Switch tự học, không cần cấu hình — nên gọi là "cắm là chạy".

**Miền xung đột và miền quảng bá:**

- Mỗi cổng switch là một **miền xung đột (collision domain)** riêng. Với full-duplex, xung đột không còn xảy ra.
- Toàn bộ switch (không chia VLAN) là **một miền quảng bá (broadcast domain)**: một broadcast đến được mọi máy. **Router chặn broadcast** — mỗi cổng router là một miền quảng bá riêng.

### 4.4 ARP — từ địa chỉ IP tìm ra địa chỉ MAC

Máy A (IP `192.168.1.10`) muốn gửi gói cho B (`192.168.1.20`) trong cùng mạng. A biết IP của B, nhưng để đóng frame Ethernet, A cần **MAC** của B. Đó là việc của **ARP** (Address Resolution Protocol).

```text
A ──► BROADCAST (ff:ff:ff:ff:ff:ff):  "Ai là 192.168.1.20? Hãy báo cho 192.168.1.10"
      (mọi máy trong LAN đều nhận, chỉ B trả lời)

A ◄── B (unicast):                   "192.168.1.20 là tôi, MAC của tôi là b8:27:eb:44:55:66"

A lưu vào bảng ARP (ARP cache) để lần sau không phải hỏi lại.
```

Xem bảng ARP trên máy của bạn:

```bash
arp -a              # Windows, macOS, Linux (nếu có net-tools)
ip neigh            # Linux
```

**Điểm then chốt:** nếu B nằm ở **mạng khác**, A **không** hỏi MAC của B (vì B không nhận được broadcast của A). A sẽ hỏi MAC của **default gateway** (router) và gửi frame cho router. Chương 8 sẽ giải thích chi tiết.

### 4.5 VLAN — chia một switch thành nhiều mạng logic

**VLAN** (Virtual LAN) cho phép một switch vật lý hoạt động như **nhiều switch riêng biệt**:

```text
          Switch
 ┌─────────────────────────────┐
 │ cổng 1-8:  VLAN 10 (Kế toán) │   → một miền quảng bá riêng
 │ cổng 9-16: VLAN 20 (Kỹ thuật)│   → một miền quảng bá riêng
 │ cổng 24:   trunk (802.1Q)    │   → mang frame của NHIỀU VLAN, có gắn thẻ VLAN ID
 └─────────────────────────────┘
```

- Máy ở VLAN 10 và VLAN 20 **không** nói chuyện trực tiếp được với nhau; phải đi qua **router** (hoặc switch tầng 3).
- Lợi ích: tách biệt bảo mật, giảm broadcast, tổ chức mạng theo phòng ban thay vì theo vị trí dây.
- Chuẩn **802.1Q** chèn một **thẻ (tag) 4 byte** chứa VLAN ID (12 bit, tức 4094 VLAN dùng được) vào frame trên đường trunk.

### 4.6 Truy cập đường truyền dùng chung

- **CSMA/CD** (Ethernet kiểu cũ, half-duplex): nghe trước khi nói; nếu phát hiện xung đột thì dừng, chờ ngẫu nhiên rồi gửi lại. Ethernet full-duplex qua switch không còn cần cơ chế này.
- **CSMA/CA** (Wi-Fi): vì không phát hiện được xung đột trong không khí, Wi-Fi cố gắng **tránh** xung đột: chờ kênh rảnh, chờ thêm một khoảng ngẫu nhiên, và cần **ACK** xác nhận cho mỗi frame.

### ✅ Tự kiểm tra

1. Switch nhận một frame có MAC đích chưa có trong bảng. Nó làm gì?
2. Máy A gửi gói tới `8.8.8.8` (ở Internet). Frame Ethernet do A tạo ra có MAC đích là của ai?
3. Vì sao ARP request phải là broadcast còn ARP reply thì không cần?

<details><summary>Đáp án</summary>

1. Flood ra mọi cổng trừ cổng vừa nhận.
2. MAC của default gateway (router), vì `8.8.8.8` nằm ở mạng khác.
3. Người hỏi chưa biết MAC của đích nên phải hỏi tất cả. Người trả lời đã biết MAC của người hỏi (có trong ARP request) nên gửi thẳng.

</details>

---

## Bài tập Phần I

**Bài I.1.** Một game gửi gói 200 byte lên server 20 lần mỗi giây. Băng thông **tải lên** tối thiểu cần cho riêng phần dữ liệu này là bao nhiêu kbps?

**Bài I.2.** Server đặt ở Singapore, cách người chơi khoảng 2400 km cáp quang. Tính độ trễ lan truyền **một chiều**, và độ trễ **khứ hồi** (RTT) tối thiểu chỉ tính riêng phần lan truyền.

**Bài I.3.** Sắp xếp các header sau theo thứ tự xuất hiện trong một frame, từ ngoài vào trong: TCP, Ethernet, HTTP, IP.

**Bài I.4.** Switch 4 cổng, bảng MAC rỗng. Lần lượt: A (cổng 1) gửi cho C; C (cổng 3) trả lời A; D (cổng 4) gửi broadcast. Với mỗi sự kiện, frame được gửi ra những cổng nào?

<details>
<summary><b>Đáp án Phần I</b> (bấm để mở)</summary>

**I.1.** $200 \times 8 \times 20 = 32\,000$ bit/s $= 32$ kbps (chưa tính header của các tầng dưới).

**I.2.** Một chiều: $\dfrac{2{,}4 \times 10^6}{2 \times 10^8} = 0{,}012$ s $= 12$ ms. Khứ hồi: $24$ ms. Thực tế sẽ lớn hơn do cáp không đi thẳng và còn trễ xử lý, hàng đợi.

**I.3.** Ethernet → IP → TCP → HTTP.

**I.4.**

- A gửi C: chưa biết C → flood ra cổng 2, 3, 4. (Bảng: A → 1)
- C trả lời A: đã biết A → chỉ ra cổng 1. (Bảng: A → 1, C → 3)
- D gửi broadcast: flood ra cổng 1, 2, 3. (Bảng: A → 1, C → 3, D → 4)

</details>

---

# PHẦN II — TẦNG MẠNG

## Chương 5. Địa chỉ IPv4

### 5.1 Cấu trúc

Địa chỉ IPv4 dài **32 bit**, viết thành **4 số thập phân** (mỗi số là 1 byte, từ 0 đến 255), cách nhau bởi dấu chấm:

```text
      192     .    168     .     1      .     10
   11000000    10101000    00000001     00001010
```

Tổng cộng có $2^{32} \approx 4{,}3$ tỉ địa chỉ — **không đủ** cho mọi thiết bị trên thế giới. Đây là lý do có NAT (Chương 9) và IPv6 (Chương 10).

**Đổi nhanh nhị phân ↔ thập phân** — thuộc lòng các giá trị trong một byte:

| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| Giá trị | 128 | 64 | 32 | 16 | 8 | 4 | 2 | 1 |

Ví dụ: $168 = 128 + 32 + 8 = 10101000_2$.

### 5.2 Phần mạng và phần host

Mỗi địa chỉ IP gồm hai phần:

- **Phần mạng (network):** giống nhau với mọi máy trong cùng một mạng.
- **Phần host:** phân biệt từng máy trong mạng đó.

Ranh giới giữa hai phần được xác định bởi **mặt nạ mạng (subnet mask)**: các bit **1** đánh dấu phần mạng, các bit **0** đánh dấu phần host.

```text
Địa chỉ : 192.168.1.10   = 11000000.10101000.00000001.00001010
Mask    : 255.255.255.0  = 11111111.11111111.11111111.00000000
                           └────────── phần mạng ──────────┘└ host ┘
```

**Ký hiệu CIDR:** viết số bit 1 của mask sau dấu `/`. Ví dụ: `255.255.255.0` = **/24**, nên ta viết `192.168.1.10/24`.

### 5.3 Ba địa chỉ quan trọng của một mạng

Lấy **địa chỉ IP AND mask** (theo từng bit) để ra **địa chỉ mạng**:

| Tên | Cách tìm | Ví dụ với `192.168.1.10/24` |
|---|---|---|
| **Địa chỉ mạng** | Mọi bit host = 0 | `192.168.1.0` |
| **Địa chỉ broadcast** | Mọi bit host = 1 | `192.168.1.255` |
| **Dải địa chỉ dùng được cho host** | Nằm giữa hai địa chỉ trên | `192.168.1.1` → `192.168.1.254` |

**Số host dùng được** trong mạng có $h$ bit host:

$$\boxed{\text{số host} = 2^h - 2}$$

(trừ 2 vì địa chỉ mạng và địa chỉ broadcast không gán cho máy nào). Với /24: $h = 8$, số host $= 254$.

### 5.4 Các dải địa chỉ đặc biệt (cần thuộc)

| Dải | Ý nghĩa |
|---|---|
| `10.0.0.0/8` | **Riêng (private)** — dùng trong mạng nội bộ |
| `172.16.0.0/12` (tức `172.16.0.0` → `172.31.255.255`) | **Riêng** |
| `192.168.0.0/16` | **Riêng** — phổ biến nhất ở gia đình |
| `127.0.0.0/8` | **Loopback** — `127.0.0.1` là "chính máy này" (`localhost`) |
| `169.254.0.0/16` | **Link-local** — máy tự gán khi **không xin được IP từ DHCP** |
| `100.64.0.0/10` | **CGNAT** — nhà mạng dùng cho NAT cấp nhà mạng |
| `0.0.0.0` | "Mọi địa chỉ" (khi server lắng nghe) hoặc "chưa có địa chỉ" |
| `255.255.255.255` | Broadcast trong mạng hiện tại |
| `224.0.0.0/4` | Multicast (gửi cho một nhóm máy) |
| `192.0.2.0/24`, `198.51.100.0/24`, `203.0.113.0/24` | **Dành cho tài liệu, ví dụ** — không bao giờ xuất hiện trên Internet thật (tài liệu này dùng chúng trong các ví dụ) |

- **Địa chỉ riêng** không được định tuyến trên Internet; muốn ra Internet phải qua **NAT**.
- Nếu máy bạn có IP `169.254.x.x`, gần như chắc chắn là **DHCP bị lỗi** — dấu hiệu chẩn đoán rất hữu ích.

### 5.5 Phân lớp địa chỉ (kiến thức lịch sử)

Trước năm 1993, IPv4 chia theo **lớp** với mask cố định:

| Lớp | Byte đầu | Mask mặc định |
|---|---|---|
| A | 1 – 126 | /8 |
| B | 128 – 191 | /16 |
| C | 192 – 223 | /24 |
| D | 224 – 239 | Multicast |
| E | 240 – 255 | Dự phòng |

Cách chia này rất lãng phí (một công ty cần 300 địa chỉ phải nhận cả lớp B với 65 534 địa chỉ), nên đã được thay bằng **CIDR** — mask có độ dài **tùy ý**. Ngày nay chỉ cần biết thuật ngữ này khi đọc tài liệu cũ.

---

## Chương 6. Chia mạng con (Subnetting)

> Đây là kỹ năng **tính toán** quan trọng nhất của môn mạng, xuất hiện trong mọi kỳ thi và mọi công việc quản trị mạng. Hãy làm tay thật nhiều.

### 6.1 Bảng tra mask

| CIDR | Mask | Số địa chỉ | Số host dùng được |
|---|---|---|---|
| /24 | 255.255.255.0 | 256 | 254 |
| /25 | 255.255.255.128 | 128 | 126 |
| /26 | 255.255.255.192 | 64 | 62 |
| /27 | 255.255.255.224 | 32 | 30 |
| /28 | 255.255.255.240 | 16 | 14 |
| /29 | 255.255.255.248 | 8 | 6 |
| /30 | 255.255.255.252 | 4 | 2 |
| /31 | 255.255.255.254 | 2 | 2 (chỉ cho đường nối điểm–điểm) |
| /32 | 255.255.255.255 | 1 | 1 (một máy duy nhất) |

Các giá trị có thể xuất hiện trong **một byte** của mask: `0, 128, 192, 224, 240, 248, 252, 254, 255`.

### 6.2 Phương pháp "số ma thuật" (block size)

Với một địa chỉ và mask bất kỳ:

1. Tìm **byte "thú vị"**: byte đầu tiên của mask **không phải 255** (và không phải 0 nếu các byte trước đều 255).
2. Tính **kích thước khối**: $\text{block} = 256 - \text{giá trị mask ở byte đó}$.
3. Các mạng con bắt đầu tại **bội số của block** trong byte đó. Tìm bội số lớn nhất **không vượt quá** giá trị byte tương ứng của địa chỉ → đó là **địa chỉ mạng**.
4. **Broadcast** = (địa chỉ mạng kế tiếp) − 1.
5. Các byte **sau** byte thú vị: ở địa chỉ mạng đều bằng 0, ở broadcast đều bằng 255.

**Ví dụ 6.1.** Cho `192.168.10.77/26`. Tìm mask, địa chỉ mạng, broadcast, dải host và số host.

*Lời giải.*

- /26 → mask `255.255.255.192`. Byte thú vị là byte 4, block $= 256 - 192 = 64$.
- Các mạng con trong byte 4: `0, 64, 128, 192`. Số 77 nằm trong khối bắt đầu tại **64**.
- Địa chỉ mạng: `192.168.10.64`. Mạng kế tiếp bắt đầu tại 128, nên broadcast là `192.168.10.127`.
- Dải host: `192.168.10.65` → `192.168.10.126`.
- Số host: $2^{6} - 2 = 62$.

**Ví dụ 6.2 (byte thú vị không nằm ở cuối).** Cho `172.16.45.200/20`.

*Lời giải.*

- /20 = 8 + 8 + 4, nên mask là `255.255.240.0`. Byte thú vị là **byte 3**, block $= 256 - 240 = 16$.
- Các bội số của 16: `0, 16, 32, 48, ...`. Số 45 nằm trong khối bắt đầu tại **32**.
- Địa chỉ mạng: `172.16.32.0` (byte 4 = 0).
- Mạng kế tiếp: `172.16.48.0` → broadcast: `172.16.47.255` (byte 4 = 255).
- Dải host: `172.16.32.1` → `172.16.47.254`.
- Số host: $h = 32 - 20 = 12$, nên $2^{12} - 2 = 4094$.

### 6.3 Hai máy có cùng mạng không?

Hai máy **cùng mạng** khi và chỉ khi địa chỉ mạng của chúng (tính với **cùng mask**) giống nhau. Nếu cùng mạng, chúng gửi thẳng cho nhau (qua ARP); nếu khác mạng, phải đi qua router.

**Ví dụ 6.3.** Với mask /23, các cặp nào cùng mạng: `10.0.5.20`, `10.0.4.200`, `10.0.6.1`?

*Lời giải.* /23 → mask `255.255.254.0`, byte thú vị là byte 3, block $= 2$.

- `10.0.5.20`: 5 thuộc khối bắt đầu tại 4 → mạng `10.0.4.0/23`.
- `10.0.4.200`: 4 thuộc khối tại 4 → mạng `10.0.4.0/23`.
- `10.0.6.1`: 6 thuộc khối tại 6 → mạng `10.0.6.0/23`.

Vậy `10.0.5.20` và `10.0.4.200` **cùng mạng** (dù byte thứ 3 khác nhau!), còn `10.0.6.1` ở mạng khác.

### 6.4 Chia một mạng thành các mạng con bằng nhau

**Công thức:** mượn $n$ bit từ phần host thì được $2^n$ mạng con.

**Ví dụ 6.4.** Chia `192.168.1.0/24` thành 4 mạng con bằng nhau.

*Lời giải.* $4 = 2^2$ nên mượn 2 bit: /24 → **/26**, block = 64.

| Mạng con | Địa chỉ mạng | Dải host | Broadcast |
|---|---|---|---|
| 1 | 192.168.1.0/26 | .1 – .62 | .63 |
| 2 | 192.168.1.64/26 | .65 – .126 | .127 |
| 3 | 192.168.1.128/26 | .129 – .190 | .191 |
| 4 | 192.168.1.192/26 | .193 – .254 | .255 |

**Khi đề bài cho yêu cầu về số host**, chọn prefix nhỏ nhất (mạng lớn nhất cần thiết) sao cho $2^h - 2 \ge$ số host cần. Nếu đề bài cho **cả** số mạng con lẫn số host, phải kiểm tra **cả hai** điều kiện.

### 6.5 VLSM — mạng con có kích thước khác nhau

Thực tế các phòng ban có số máy rất khác nhau. **VLSM** (Variable Length Subnet Mask) cấp cho mỗi mạng con **vừa đủ** kích thước.

**Quy trình:**

1. Sắp xếp các yêu cầu **từ lớn đến nhỏ**.
2. Với mỗi yêu cầu, chọn prefix nhỏ nhất đáp ứng được số host.
3. Cấp phát **liên tiếp** từ đầu dải, mạng sau bắt đầu ngay sau broadcast của mạng trước.

**Ví dụ 6.5.** Từ `192.168.50.0/24`, cấp địa chỉ cho: phòng Kỹ thuật **100** máy, phòng Kinh doanh **50** máy, phòng Kế toán **25** máy, và **1 đường nối** giữa hai router (cần 2 địa chỉ).

*Lời giải.*

| Yêu cầu | Host cần | Prefix chọn | Host tối đa | Địa chỉ mạng | Broadcast |
|---|---|---|---|---|---|
| Kỹ thuật | 100 | /25 | 126 | 192.168.50.0/25 | 192.168.50.127 |
| Kinh doanh | 50 | /26 | 62 | 192.168.50.128/26 | 192.168.50.191 |
| Kế toán | 25 | /27 | 30 | 192.168.50.192/27 | 192.168.50.223 |
| Đường nối | 2 | /30 | 2 | 192.168.50.224/30 | 192.168.50.227 |

Phần còn trống: `192.168.50.228` → `192.168.50.255`, để dành cho mở rộng.

> ⚠️ **Vì sao phải cấp từ lớn đến nhỏ?** Nếu cấp mạng nhỏ trước, các mạng lớn có thể không còn khối liên tục phù hợp (mạng con phải bắt đầu tại bội số của block size), gây lãng phí hoặc chồng lấn.

### 6.6 Gộp mạng (summarization / supernetting)

Ngược lại với chia mạng: gộp nhiều mạng liên tiếp thành **một** để bảng định tuyến gọn hơn.

**Ví dụ 6.6.** Gộp `192.168.4.0/24`, `192.168.5.0/24`, `192.168.6.0/24`, `192.168.7.0/24`.

*Lời giải.* Viết byte thứ 3 ra nhị phân:

```text
  4 = 000001|00
  5 = 000001|01
  6 = 000001|10
  7 = 000001|11
      └─────┘ 6 bit chung
```

Phần chung: 8 + 8 + **6** = 22 bit. Kết quả: **`192.168.4.0/22`**.

Điều kiện gộp được "sạch": số mạng là **lũy thừa của 2** và mạng đầu tiên phải bắt đầu tại **bội số** của khối mới (ở đây 4 là bội số của 4).

### ✅ Tự kiểm tra

1. `/29` có bao nhiêu host dùng được?
2. Địa chỉ `192.168.1.64/26` có gán được cho một máy tính không?

<details><summary>Đáp án</summary>

1. $2^3 - 2 = 6$.
2. Không. Đó là **địa chỉ mạng** của mạng con `192.168.1.64/26`.

</details>

---

## Chương 7. IP header, phân mảnh và ICMP

### 7.1 Header IPv4

```text
 0               8               16              24             31
┌───────┬───────┬───────────────┬───────────────────────────────┐
│Version│  IHL  │  DSCP  │ ECN  │         Total Length          │
├───────┴───────┴───────────────┼─────┬─────────────────────────┤
│        Identification         │Flags│     Fragment Offset     │
├───────────────┬───────────────┼─────┴─────────────────────────┤
│      TTL      │   Protocol    │        Header Checksum        │
├───────────────┴───────────────┴───────────────────────────────┤
│                    Địa chỉ IP nguồn (32 bit)                   │
├───────────────────────────────────────────────────────────────┤
│                    Địa chỉ IP đích (32 bit)                    │
├───────────────────────────────────────────────────────────────┤
│                    Options (hiếm dùng)                         │
└───────────────────────────────────────────────────────────────┘
          Kích thước tối thiểu: 20 byte
```

| Trường | Ý nghĩa |
|---|---|
| **Version** | 4 với IPv4 |
| **IHL** | Độ dài header (tính theo đơn vị 4 byte; tối thiểu 5 → 20 byte) |
| **DSCP/ECN** | Ưu tiên chất lượng dịch vụ; báo hiệu tắc nghẽn |
| **Total Length** | Tổng độ dài gói (header + dữ liệu), tối đa 65 535 byte |
| **Identification, Flags, Fragment Offset** | Dùng khi **phân mảnh** |
| **TTL** (Time To Live) | Giảm 1 qua mỗi router; về 0 thì gói bị **hủy** → chống gói chạy vòng vô hạn |
| **Protocol** | Bên trong chứa gì: **1** = ICMP, **6** = TCP, **17** = UDP |
| **Header Checksum** | Kiểm tra lỗi của **riêng header** |

### 7.2 Phân mảnh (fragmentation)

Khi gói IP lớn hơn MTU của đường truyền tiếp theo, router (với IPv4) có thể **cắt** nó thành nhiều mảnh. Bên nhận ghép lại dựa trên:

- **Identification:** các mảnh của cùng một gói có cùng giá trị này.
- **Cờ MF (More Fragments):** 1 = còn mảnh phía sau; 0 = mảnh cuối.
- **Fragment Offset:** vị trí của mảnh trong gói gốc, tính theo **đơn vị 8 byte**.
- **Cờ DF (Don't Fragment):** nếu bật, router không được cắt mà phải hủy gói và báo lỗi ICMP.

**Ví dụ 7.1.** Gói IP có tổng độ dài 4000 byte (header 20 byte, dữ liệu 3980 byte) đi qua đường có MTU 1500. Tìm các mảnh.

*Lời giải.* Mỗi mảnh có header 20 byte, nên dữ liệu tối đa $1500 - 20 = 1480$ byte (chia hết cho 8 ✓).

| Mảnh | Dữ liệu | Tổng độ dài | Offset | MF |
|---|---|---|---|---|
| 1 | 1480 byte | 1500 | 0 | 1 |
| 2 | 1480 byte | 1500 | $1480 / 8 = 185$ | 1 |
| 3 | $3980 - 2960 = 1020$ byte | 1040 | $2960 / 8 = 370$ | 0 |

Phân mảnh làm chậm mạng và nếu mất **một** mảnh thì cả gói phải gửi lại. Vì vậy, các hệ thống hiện đại cố gắng **tránh** phân mảnh bằng kỹ thuật **Path MTU Discovery** (bật cờ DF và dò MTU nhỏ nhất trên đường đi). IPv6 **không cho router phân mảnh**.

### 7.3 ICMP — giao thức báo lỗi và chẩn đoán

**ICMP** chạy trên IP (Protocol = 1) và dùng để **báo lỗi** và **kiểm tra**.

| Loại ICMP | Khi nào xuất hiện |
|---|---|
| **Echo Request / Echo Reply** | Lệnh `ping` |
| **Destination Unreachable** | Không tới được mạng/máy/cổng đích; hoặc cần phân mảnh nhưng DF đang bật |
| **Time Exceeded** | TTL về 0 |
| **Redirect** | Router báo có đường tốt hơn |

**`ping`** gửi Echo Request và đo thời gian nhận Echo Reply → kiểm tra **có tới được không** và **độ trễ khứ hồi (RTT)**.

**`traceroute`** (Windows: `tracert`) lợi dụng TTL một cách thông minh:

```text
Gửi gói TTL=1 → router 1 giảm về 0, hủy gói, gửi lại "Time Exceeded"  → biết router 1
Gửi gói TTL=2 → router 2 giảm về 0, hủy gói, gửi lại "Time Exceeded"  → biết router 2
Gửi gói TTL=3 → ...
... cho đến khi tới đích
```

Kết quả là danh sách **các router trên đường đi** và độ trễ tới từng router. Một số router cấu hình không trả lời ICMP, nên bạn có thể thấy dòng `* * *` — điều này không nhất thiết là lỗi.

---

## Chương 8. Định tuyến (Routing)

### 8.1 Default gateway — cửa ra khỏi mạng

Khi máy A cần gửi một gói, nó tự hỏi: **"Đích có nằm trong mạng của mình không?"** (so địa chỉ mạng như Chương 6.3).

```text
                   ┌── Cùng mạng ──► ARP hỏi MAC của ĐÍCH ──► gửi thẳng
A cần gửi gói ─────┤
                   └── Khác mạng ──► ARP hỏi MAC của GATEWAY ──► gửi cho router
```

**Default gateway** là địa chỉ IP của router trong mạng LAN (thường là `192.168.1.1` hoặc `192.168.0.1` ở gia đình). Máy có IP đúng nhưng **sai gateway** vẫn nói chuyện được với máy cùng mạng, nhưng **không ra được Internet**.

### 8.2 Bảng định tuyến và quy tắc "khớp dài nhất"

Router quyết định chuyển gói đi đâu dựa vào **bảng định tuyến**. Khi địa chỉ đích khớp với **nhiều** dòng, router chọn dòng có **prefix dài nhất** (cụ thể nhất) — gọi là **longest prefix match**.

**Ví dụ 8.1.** Bảng định tuyến:

| Mạng đích | Chuyển tới |
|---|---|
| `0.0.0.0/0` (default route — khớp mọi địa chỉ) | ISP |
| `10.0.0.0/8` | R1 |
| `10.1.0.0/16` | R2 |
| `10.1.2.0/24` | R3 |

Gói đến các đích sau được chuyển đi đâu?

| Đích | Các dòng khớp | Dòng dài nhất | Kết quả |
|---|---|---|---|
| `10.1.2.5` | /0, /8, /16, /24 | /24 | **R3** |
| `10.1.9.9` | /0, /8, /16 | /16 | **R2** |
| `10.200.0.1` | /0, /8 | /8 | **R1** |
| `8.8.8.8` | /0 | /0 | **ISP** |

Xem bảng định tuyến trên máy của bạn:

```bash
route print        # Windows
ip route           # Linux
netstat -rn        # macOS
```

### 8.3 Định tuyến tĩnh và động

| | Định tuyến tĩnh (static) | Định tuyến động (dynamic) |
|---|---|---|
| Cách tạo | Quản trị viên nhập tay | Router tự trao đổi thông tin với nhau |
| Khi đường hỏng | Không tự đổi | Tự tìm đường khác |
| Dùng khi | Mạng nhỏ, đường duy nhất | Mạng lớn, nhiều đường dự phòng |

### 8.4 Hai họ thuật toán định tuyến động

| | Vector khoảng cách (distance vector) | Trạng thái liên kết (link state) |
|---|---|---|
| Ý tưởng | Mỗi router chỉ biết **"hàng xóm nói gì"** và cộng dồn khoảng cách | Mỗi router biết **toàn bộ bản đồ mạng**, tự tính đường ngắn nhất |
| Thuật toán | Bellman–Ford | **Dijkstra** |
| Giao thức | **RIP** (đo bằng số chặng, tối đa 15) | **OSPF** (đo bằng "chi phí", thường theo băng thông) |
| Hội tụ | Chậm, có thể lặp vòng | Nhanh |

**Ví dụ 8.2 (Dijkstra).** Tìm đường ngắn nhất từ A đến mọi router trong mạng sau (số trên cạnh là chi phí):

```text
A ───2─── B ───4─── D ───7─── F
│         │      ╱  │         │
5         1    1    2         1
│         │  ╱      │         │
└──────── C ───6─── E ────────┘
```

Các cạnh: A–B 2, A–C 5, B–C 1, B–D 4, C–D 1, C–E 6, D–E 2, D–F 7, E–F 1.

*Lời giải.* Mỗi bước, chọn router **chưa chốt** có khoảng cách tạm thời nhỏ nhất, chốt nó, rồi cập nhật các hàng xóm.

| Bước | Chốt | A | B | C | D | E | F |
|---|---|---|---|---|---|---|---|
| 0 | A | **0** | 2 | 5 | ∞ | ∞ | ∞ |
| 1 | B | | **2** | 3 (qua B) | 6 (qua B) | ∞ | ∞ |
| 2 | C | | | **3** | 4 (qua C) | 9 (qua C) | ∞ |
| 3 | D | | | | **4** | 6 (qua D) | 11 (qua D) |
| 4 | E | | | | | **6** | 7 (qua E) |
| 5 | F | | | | | | **7** |

Đường ngắn nhất từ A đến F: **A → B → C → D → E → F**, tổng chi phí **7** (dù đi A → B → D → F chỉ có 3 chặng nhưng chi phí là 13).

### 8.5 BGP — định tuyến giữa các "quốc gia" của Internet

Internet được chia thành hàng chục nghìn **hệ tự trị (AS — Autonomous System)**, mỗi AS là mạng của một tổ chức (nhà mạng, công ty lớn...) và có một số hiệu riêng.

- **Bên trong** một AS: dùng OSPF, RIP... (gọi chung là IGP).
- **Giữa** các AS: dùng **BGP** (chạy trên TCP cổng 179). BGP không chỉ chọn đường ngắn nhất mà còn theo **chính sách** kinh doanh (ai trả tiền cho ai, ai ưu tiên ai).

BGP là "chất keo" giữ Internet lại với nhau. Cấu hình BGP sai ở một nhà mạng lớn từng khiến nhiều dịch vụ lớn trên thế giới bị gián đoạn hàng giờ.

---

## Chương 9. NAT và DHCP

### 9.1 NAT — nhiều máy dùng chung một IP công khai

Nhà bạn có nhiều thiết bị với địa chỉ **riêng** (`192.168.1.x`), nhưng nhà mạng chỉ cấp **một** địa chỉ **công khai**. Router dùng **NAT** (Network Address Translation) — chính xác hơn là **PAT/NAPT** (NAT kèm đổi cổng) — để chia sẻ địa chỉ đó.

```text
   Mạng nhà (riêng)                   Router (NAT)                     Internet
                                   IP công khai: 203.0.113.5

 Laptop     192.168.1.10:50000 ──► đổi thành 203.0.113.5:40001 ──► Server 198.51.100.20:443
 Điện thoại 192.168.1.11:50000 ──► đổi thành 203.0.113.5:40002 ──► Server 198.51.100.20:443

                         Bảng NAT của router
 ┌────────────────────────┬──────────────────────┬─────────────────────────┐
 │ Bên trong              │ Bên ngoài            │ Đích                    │
 ├────────────────────────┼──────────────────────┼─────────────────────────┤
 │ 192.168.1.10:50000     │ 203.0.113.5:40001    │ 198.51.100.20:443       │
 │ 192.168.1.11:50000     │ 203.0.113.5:40002    │ 198.51.100.20:443       │
 └────────────────────────┴──────────────────────┴─────────────────────────┘
```

**Luồng đi:** router thay IP/cổng **nguồn** bằng IP công khai và một cổng mới, ghi vào bảng.
**Luồng về:** gói trả lời tới `203.0.113.5:40002` → router tra bảng → đổi đích thành `192.168.1.11:50000` → chuyển vào trong.

**Hệ quả quan trọng:**

- Thiết bị bên ngoài **không thể chủ động kết nối** vào máy bên trong, vì router không biết chuyển cho ai. Đây vừa là một lớp "bảo vệ" tự nhiên, vừa là **rào cản** cho việc tự dựng server hay chơi game P2P.
- **Port forwarding:** tự thêm một dòng cố định vào bảng NAT, ví dụ "mọi kết nối tới cổng 25565 → `192.168.1.10:25565`" để bạn bè vào được server game dựng tại nhà.
- **CGNAT:** nhiều nhà mạng không cấp IP công khai cho từng nhà mà NAT **thêm một lần nữa** ở phía nhà mạng (thường dùng dải `100.64.0.0/10`). Khi đó port forwarding trên router nhà **không có tác dụng**. Cách kiểm tra: so sánh IP ở cổng WAN của router với IP hiển thị trên các trang "what is my IP" — nếu khác nhau, bạn đang ở sau CGNAT.
- **NAT traversal:** các ứng dụng P2P (gọi video, game) dùng kỹ thuật như **STUN**, **TURN**, **hole punching** để hai máy sau NAT vẫn nói chuyện được với nhau.

### 9.2 DHCP — tự động cấp địa chỉ

Khi bạn kết nối Wi-Fi, máy tự nhận được IP, mask, gateway, DNS — nhờ **DHCP**. Quy trình **DORA** (client dùng cổng UDP 68, server dùng cổng 67):

```text
Client (chưa có IP)                                        DHCP Server
     │ ── 1. DISCOVER (broadcast): "Có server DHCP nào không?" ──►  │
     │ ◄─ 2. OFFER: "Tôi đề nghị cho bạn 192.168.1.23"          ─── │
     │ ── 3. REQUEST (broadcast): "Tôi nhận 192.168.1.23 nhé"   ──► │
     │ ◄─ 4. ACK: "Đồng ý. Kèm mask, gateway, DNS, thời hạn 24h" ─── │
```

- Bước 3 vẫn là broadcast để báo cho **mọi** server DHCP (nếu có nhiều) biết client đã chọn ai.
- Địa chỉ được cấp theo **thời hạn thuê (lease)**; client phải gia hạn trước khi hết hạn.
- **DHCP reservation:** cấu hình router để một thiết bị (theo MAC) **luôn** nhận cùng một IP — rất tiện cho máy chủ hay máy in trong nhà, và cần thiết khi dùng port forwarding.
- Không liên lạc được server DHCP → máy tự gán `169.254.x.x` (Chương 5.4).

---

## Chương 10. IPv6

### 10.1 Vì sao cần IPv6?

IPv4 chỉ có khoảng 4,3 tỉ địa chỉ và đã cạn. **IPv6** dùng địa chỉ **128 bit** — khoảng $3{,}4 \times 10^{38}$ địa chỉ, đủ để mọi thiết bị có địa chỉ công khai riêng mà **không cần NAT**.

### 10.2 Cách viết và rút gọn

Địa chỉ IPv6 gồm **8 nhóm**, mỗi nhóm 4 chữ số hex (16 bit), cách nhau bởi dấu `:`.

**Hai quy tắc rút gọn:**

1. **Bỏ số 0 ở đầu** mỗi nhóm: `0db8` → `db8`, `0042` → `42`, `0000` → `0`.
2. **Thay một dãy liên tiếp các nhóm toàn 0** bằng `::`. Chỉ được dùng `::` **một lần** trong một địa chỉ (nếu không sẽ không biết mỗi chỗ có bao nhiêu nhóm). Nếu có nhiều dãy, rút gọn dãy **dài nhất**; nếu bằng nhau, rút gọn dãy **đầu tiên**.

**Ví dụ 10.1.** Rút gọn `2001:0db8:0000:0000:0000:ff00:0042:8329`.

*Lời giải.*

- Bỏ số 0 đầu: `2001:db8:0:0:0:ff00:42:8329`
- Thay dãy `0:0:0` bằng `::`: **`2001:db8::ff00:42:8329`**

**Ví dụ 10.2.** Rút gọn `2001:0db8:0000:0000:0001:0000:0000:0001`.

*Lời giải.* Sau bước 1: `2001:db8:0:0:1:0:0:1`. Có **hai** dãy 0 dài bằng nhau (2 nhóm), rút gọn dãy **đầu tiên**: **`2001:db8::1:0:0:1`**.

**Khôi phục địa chỉ đầy đủ:** đếm số nhóm hiện có, `::` thay cho số nhóm còn thiếu để đủ 8. Ví dụ `2001:db8::1` có 3 nhóm → `::` thay cho 5 nhóm 0 → `2001:0db8:0000:0000:0000:0000:0000:0001`.

### 10.3 Các loại địa chỉ IPv6

| Dải | Loại | Tương đương IPv4 |
|---|---|---|
| `::1` | Loopback | `127.0.0.1` |
| `fe80::/10` | **Link-local** — mọi giao diện IPv6 đều tự có, chỉ dùng trong một đoạn mạng | `169.254.0.0/16` |
| `fc00::/7` (thực tế là `fd00::/8`) | **Unique Local (ULA)** — dùng nội bộ | Địa chỉ riêng |
| `2000::/3` | **Global Unicast** — địa chỉ công khai | Địa chỉ công khai |
| `ff00::/8` | **Multicast** | `224.0.0.0/4` |
| `::` | Chưa có địa chỉ | `0.0.0.0` |

**IPv6 không có broadcast**; thay vào đó dùng multicast.

### 10.4 Cấu trúc và cấu hình

Một địa chỉ global unicast điển hình chia đôi:

```text
 2001:0db8:abcd:0012 : 0000:0000:0000:0001
 └───── 64 bit ─────┘ └────── 64 bit ─────┘
   Prefix mạng           Interface ID
   (nhà mạng cấp /48 hoặc /56, bạn tự chia thành các /64)
```

- Mạng con IPv6 tiêu chuẩn có kích thước **/64**. Một khách hàng nhận **/48** có thể tạo $2^{64-48} = 65\,536$ mạng con /64.
- **SLAAC:** máy tự tạo địa chỉ bằng cách lấy prefix mà router quảng bá, ghép với interface ID tự sinh — **không cần DHCP**. (Vẫn có DHCPv6 nếu muốn quản lý tập trung.)
- **NDP** (Neighbor Discovery Protocol, dựa trên ICMPv6) thay thế **ARP**.

### 10.5 Header IPv6 và chuyển đổi

- Header IPv6 **cố định 40 byte**, đơn giản hơn IPv4: không có checksum, không có trường phân mảnh (router không phân mảnh). TTL được đổi tên thành **Hop Limit**.
- **Dual stack:** hầu hết thiết bị ngày nay chạy **song song** IPv4 và IPv6; hệ điều hành ưu tiên IPv6 khi cả hai bên đều hỗ trợ.

---

## Bài tập Phần II

**Bài II.1.** Cho `192.168.100.150/27`. Tìm mask, địa chỉ mạng, broadcast, dải host, số host.

**Bài II.2.** Cho `10.20.30.40/13`. Tìm mask, địa chỉ mạng, broadcast và số host.

**Bài II.3.** Một công ty cần **6 mạng con**, mỗi mạng **ít nhất 25 máy**, từ dải `192.168.20.0/24`. Chọn prefix và liệt kê 6 địa chỉ mạng đầu tiên.

**Bài II.4 (VLSM).** Từ `10.0.0.0/24`, cấp cho: mạng A cần 60 máy, mạng B cần 28 máy, mạng C cần 12 máy, và **3 đường nối** router–router.

**Bài II.5.** Gộp `172.16.12.0/24`, `172.16.13.0/24`, `172.16.14.0/24`, `172.16.15.0/24` thành một mạng.

**Bài II.6.** Với bảng định tuyến ở Ví dụ 8.1, gói tới `10.1.3.7` được chuyển đi đâu?

**Bài II.7.** Rút gọn `fe80:0000:0000:0000:0202:b3ff:fe1e:8329`.

**Bài II.8.** Một gói IP 2500 byte (header 20 byte) đi qua đường có MTU 1000. Tính số mảnh, độ dài dữ liệu và offset của mỗi mảnh.

**Bài II.9.** Máy của bạn có IP `169.254.37.12`. Vấn đề có khả năng nhất là gì?

<details>
<summary><b>Đáp án Phần II</b> (bấm để mở)</summary>

**II.1.** Mask `255.255.255.224`, block 32. 150 thuộc khối bắt đầu tại 128. Mạng `192.168.100.128`, broadcast `192.168.100.159`, host `.129` → `.158`, **30** host.

**II.2.** /13 = 8 + 5 → mask `255.248.0.0`, byte thú vị là byte 2, block $= 256 - 248 = 8$. 20 thuộc khối tại 16. Mạng `10.16.0.0`, broadcast `10.23.255.255`, số host $2^{19} - 2 = 524\,286$.

**II.3.** Cần $2^n \ge 6$ nên $n = 3$ → **/27** (8 mạng con), mỗi mạng có 30 host ≥ 25 ✓. Sáu mạng đầu: `192.168.20.0`, `.32`, `.64`, `.96`, `.128`, `.160` (đều /27).

**II.4.**

| Mạng | Cần | Prefix | Địa chỉ mạng | Broadcast |
|---|---|---|---|---|
| A | 60 | /26 (62) | 10.0.0.0/26 | 10.0.0.63 |
| B | 28 | /27 (30) | 10.0.0.64/27 | 10.0.0.95 |
| C | 12 | /28 (14) | 10.0.0.96/28 | 10.0.0.111 |
| Nối 1 | 2 | /30 | 10.0.0.112/30 | 10.0.0.115 |
| Nối 2 | 2 | /30 | 10.0.0.116/30 | 10.0.0.119 |
| Nối 3 | 2 | /30 | 10.0.0.120/30 | 10.0.0.123 |

**II.5.** 12 = `00001100`, 15 = `00001111` → 6 bit chung ở byte 3 → **`172.16.12.0/22`**.

**II.6.** Khớp /0, /8 và `10.1.0.0/16` (không khớp `10.1.2.0/24` vì byte 3 là 3). Dài nhất là /16 → **R2**.

**II.7.** **`fe80::202:b3ff:fe1e:8329`**.

**II.8.** Dữ liệu gốc $2500 - 20 = 2480$ byte. Mỗi mảnh chở tối đa $1000 - 20 = 980$ byte, nhưng phải là bội số của 8 → **976** byte.

| Mảnh | Dữ liệu | Offset | MF |
|---|---|---|---|
| 1 | 976 | 0 | 1 |
| 2 | 976 | 122 | 1 |
| 3 | 528 | 244 | 0 |

(Kiểm tra: $976 + 976 + 528 = 2480$ ✓; $976 / 8 = 122$ ✓.)

**II.9.** Máy không nhận được địa chỉ từ **DHCP** (router tắt DHCP, cáp/Wi-Fi có vấn đề, hoặc server DHCP hết địa chỉ), nên tự gán địa chỉ link-local.

</details>

---

# PHẦN III — TẦNG GIAO VẬN

## Chương 11. Cổng (port) và UDP

### 11.1 Cổng — địa chỉ của ứng dụng

Địa chỉ IP đưa gói tới **đúng máy**, nhưng trên máy có hàng chục chương trình đang dùng mạng. **Số cổng (port)** — một số 16 bit (0 đến 65 535) — cho biết gói thuộc về **chương trình nào**.

| Dải cổng | Tên gọi | Ví dụ |
|---|---|---|
| 0 – 1023 | **Well-known** (thường cần quyền quản trị để mở) | 22 SSH, 53 DNS, 80 HTTP, 443 HTTPS |
| 1024 – 49 151 | **Registered** | 3306 MySQL, 5432 PostgreSQL, 25565 Minecraft |
| 49 152 – 65 535 | **Dynamic / ephemeral** — hệ điều hành tự cấp cho phía client | (Linux mặc định dùng 32 768 – 60 999) |

(Bảng cổng phổ biến đầy đủ ở Phụ lục A.)

**Socket** là một đầu mút giao tiếp. Một kết nối TCP được xác định **duy nhất** bởi bộ 5 thông tin:

$$\text{(giao thức, IP nguồn, cổng nguồn, IP đích, cổng đích)}$$

Nhờ đó, một web server trên cổng 443 phục vụ được hàng nghìn client cùng lúc: mỗi kết nối khác nhau ở IP hoặc cổng **của client**.

### 11.2 UDP — đơn giản và nhanh

**UDP** (User Datagram Protocol) gần như chỉ thêm số cổng vào IP. Header chỉ **8 byte**:

```text
┌─────────────────────┬─────────────────────┐
│  Cổng nguồn (16)    │   Cổng đích (16)    │
├─────────────────────┼─────────────────────┤
│  Độ dài (16)        │   Checksum (16)     │
└─────────────────────┴─────────────────────┘
│              Dữ liệu ...                  │
```

**Đặc điểm của UDP:**

- **Không kết nối:** gửi luôn, không bắt tay trước.
- **Không đảm bảo:** gói có thể **mất**, **trùng**, hoặc **đến sai thứ tự** — UDP không sửa.
- **Giữ ranh giới thông điệp:** gửi một datagram 100 byte thì bên kia nhận đúng một datagram 100 byte (khác với TCP).
- **Nhanh, độ trễ thấp, không bị "kẹt" chờ gửi lại.**

**Dùng UDP khi:** dữ liệu **cũ thì vô giá trị** (vị trí nhân vật trong game, khung hình video call, âm thanh trực tiếp), hoặc giao dịch nhỏ một lượt hỏi–đáp (DNS, DHCP, NTP).

---

## Chương 12. TCP

### 12.1 TCP mang lại gì?

**TCP** (Transmission Control Protocol) xây trên IP (vốn không tin cậy) một **dòng byte tin cậy** giữa hai tiến trình:

- **Hướng kết nối:** bắt tay trước khi gửi dữ liệu.
- **Tin cậy:** dữ liệu mất sẽ được **gửi lại**; dữ liệu đến **đúng thứ tự**, không trùng.
- **Kiểm soát luồng (flow control):** không gửi nhanh hơn khả năng nhận của bên kia.
- **Kiểm soát tắc nghẽn (congestion control):** không gửi nhanh hơn khả năng chịu đựng của mạng.

### 12.2 Header TCP

```text
 0                               16                              31
┌───────────────────────────────┬───────────────────────────────┐
│        Cổng nguồn (16)        │        Cổng đích (16)         │
├───────────────────────────────┴───────────────────────────────┤
│                    Sequence Number (32)                        │
├───────────────────────────────────────────────────────────────┤
│                 Acknowledgment Number (32)                     │
├────────┬────────┬─────────────┬───────────────────────────────┤
│ Offset │  Dự    │ Cờ (flags)  │         Window (16)           │
│  (4)   │  trữ   │ CWR ECE URG │                               │
│        │        │ ACK PSH RST │                               │
│        │        │ SYN FIN     │                               │
├────────┴────────┴─────────────┼───────────────────────────────┤
│         Checksum (16)         │      Urgent Pointer (16)      │
├───────────────────────────────┴───────────────────────────────┤
│                    Options (MSS, SACK, ...)                    │
└───────────────────────────────────────────────────────────────┘
          Kích thước tối thiểu: 20 byte
```

| Trường | Ý nghĩa |
|---|---|
| **Sequence Number** | Số thứ tự của **byte đầu tiên** trong segment này |
| **Acknowledgment Number** | "Tôi đã nhận đủ mọi byte **trước** số này, hãy gửi byte số này tiếp" |
| **SYN** | Mở kết nối |
| **ACK** | Trường Acknowledgment có giá trị |
| **FIN** | "Tôi đã gửi xong" (đóng một chiều) |
| **RST** | Hủy kết nối ngay (ví dụ: kết nối tới cổng không có ai lắng nghe) |
| **Window** | Bên gửi header này còn nhận được bao nhiêu byte nữa |

**MSS** (Maximum Segment Size): lượng dữ liệu tối đa trong một segment. Với Ethernet: $1500 - 20 \text{ (IP)} - 20 \text{ (TCP)} = 1460$ byte.

### 12.3 Bắt tay 3 bước (three-way handshake)

```text
    Client                                             Server (đang LISTEN)
      │                                                     │
      │ ─── SYN, seq = 1000 ──────────────────────────────► │
      │                                                     │
      │ ◄── SYN + ACK, seq = 5000, ack = 1001 ───────────── │
      │                                                     │
      │ ─── ACK, seq = 1001, ack = 5001 ──────────────────► │
      │                                                     │
   ESTABLISHED                                         ESTABLISHED
```

- Mỗi bên chọn một **số thứ tự ban đầu (ISN)** ngẫu nhiên (ở đây 1000 và 5000).
- SYN "tiêu tốn" 1 số thứ tự, nên ACK trả lời là ISN + 1.
- Vì sao 3 bước mà không phải 2? Để **cả hai bên** đều xác nhận được rằng mình gửi được **và** nhận được, đồng thời thống nhất số thứ tự của cả hai chiều.

### 12.4 Truyền dữ liệu: seq và ack

Tiếp tục ví dụ trên:

```text
Client ─── seq = 1001, 200 byte dữ liệu ────────────────► Server
Client ◄── ack = 1201 ─────────────────────────────────── Server   (đã nhận byte 1001..1200)

Client ◄── seq = 5001, 500 byte dữ liệu ───────────────── Server
Client ─── ack = 5501 ──────────────────────────────────► Server   (đã nhận byte 5001..5500)
```

**Quy tắc:** $\text{ack} = \text{seq của segment nhận được} + \text{số byte dữ liệu}$.

**Gửi lại khi mất gói:**

- **Hết giờ (timeout):** không nhận được ACK sau một khoảng thời gian (RTO, tính dựa trên RTT đo được) → gửi lại.
- **Gửi lại nhanh (fast retransmit):** nhận được **3 ACK trùng lặp** cho cùng một số → suy ra có gói bị mất → gửi lại ngay, không chờ timeout.

### 12.5 Kiểm soát luồng — cửa sổ trượt

Bên nhận quảng bá kích thước **cửa sổ nhận (receive window)** trong mỗi header: "tôi còn chỗ cho $W$ byte". Bên gửi chỉ được gửi tối đa $W$ byte chưa được xác nhận.

```text
Dòng byte của bên gửi:
 ┌──────────────┬──────────────────────┬─────────────────────┬────────────────┐
 │ đã gửi, đã   │ đã gửi, CHỜ ACK      │ được phép gửi ngay  │ chưa được gửi  │
 │ được ACK     │                      │                     │                │
 └──────────────┴──────────────────────┴─────────────────────┴────────────────┘
                └───────────── cửa sổ (window) ─────────────┘
                     cửa sổ "trượt" sang phải mỗi khi nhận ACK
```

Nếu bên nhận báo window = 0 (bộ đệm đầy vì ứng dụng đọc chậm), bên gửi phải **tạm dừng**.

### 12.6 Kiểm soát tắc nghẽn

Bên gửi còn duy trì **cửa sổ tắc nghẽn (cwnd)** — ước lượng mạng chịu được bao nhiêu. Lượng được gửi = $\min(\text{cwnd}, \text{window của bên nhận})$.

| Giai đoạn | Hành vi |
|---|---|
| **Slow start** | Bắt đầu với cwnd nhỏ, **tăng gấp đôi** sau mỗi RTT (tăng theo hàm mũ) đến ngưỡng `ssthresh` |
| **Congestion avoidance** | Tăng **thêm 1 MSS** mỗi RTT (tăng tuyến tính) |
| **Mất gói (3 ACK trùng)** | **Giảm một nửa** cwnd — cơ chế **AIMD** (tăng cộng, giảm nhân) |
| **Timeout** | Coi là tắc nghẽn nặng: đưa cwnd về rất nhỏ, làm lại slow start |

```text
 cwnd
  ▲
  │                    ╱│        ╱│
  │                  ╱  │      ╱  │        ← tăng tuyến tính, mất gói thì giảm nửa
  │        ssthresh╱    │    ╱    │    ╱   (hình "răng cưa" đặc trưng của TCP)
  │            ╱        │  ╱      │  ╱
  │          ╱          │╱        │╱
  │        ╱ ← slow start (tăng theo hàm mũ)
  │      ╱
  │___╱______________________________________► thời gian
```

Các thuật toán hiện đại như **CUBIC** (mặc định trên Linux) hay **BBR** cải tiến ý tưởng này, nhưng nguyên lý "dò băng thông rồi lùi lại khi có dấu hiệu tắc nghẽn" vẫn giữ nguyên.

### 12.7 Đóng kết nối

```text
    Client                                            Server
      │ ─── FIN ────────────────────────────────────►  │   "tôi gửi xong rồi"
      │ ◄── ACK ────────────────────────────────────   │
      │               (server có thể gửi nốt dữ liệu)   │
      │ ◄── FIN ────────────────────────────────────   │   "tôi cũng xong"
      │ ─── ACK ────────────────────────────────────►  │
   TIME_WAIT (chờ 2 × MSL)                           CLOSED
```

- Mỗi chiều đóng **độc lập**, nên cần 4 thông điệp (đôi khi bước 2 và 3 gộp lại).
- Bên đóng trước vào trạng thái **TIME_WAIT** một thời gian (thường 30 giây đến vài phút) để xử lý các gói đến muộn. Đây là lý do khi tắt rồi bật lại server ngay, bạn có thể gặp lỗi "Address already in use" — cách xử lý là tùy chọn `SO_REUSEADDR` (Chương 19).

### 12.8 TCP hay UDP?

| Tiêu chí | TCP | UDP |
|---|---|---|
| Kết nối | Có (bắt tay 3 bước) | Không |
| Tin cậy, đúng thứ tự | ✅ | ❌ (ứng dụng tự lo nếu cần) |
| Ranh giới thông điệp | ❌ **Dòng byte** — ứng dụng tự tách | ✅ Mỗi datagram là một thông điệp |
| Kiểm soát luồng / tắc nghẽn | ✅ | ❌ |
| Header | 20 – 60 byte | 8 byte |
| Độ trễ | Cao hơn; một gói mất làm **kẹt** cả dòng phía sau (*head-of-line blocking*) | Thấp |
| Dùng cho | Web, email, tải file, SSH, API, chat | Game thời gian thực, gọi thoại/video, DNS, streaming trực tiếp, QUIC |

> ⚠️ **Hiểu lầm kinh điển:** "Gửi `send()` 2 lần thì bên kia `recv()` 2 lần." **Sai với TCP.** TCP là **dòng byte**: hai lần gửi có thể bị gộp thành một lần nhận, hoặc một lần gửi bị chia ra nhiều lần nhận. Ứng dụng phải tự **đóng khung (framing)** thông điệp, ví dụ thêm độ dài vào trước mỗi thông điệp, hoặc dùng ký tự phân cách như `\n`.

### ✅ Tự kiểm tra

1. Server có ISN = 7000. Sau bắt tay, server gửi một segment 300 byte. Client trả lời với ack bằng bao nhiêu?
2. Kết nối tới một cổng không có chương trình nào lắng nghe (TCP), client thường nhận được gì?

<details><summary>Đáp án</summary>

1. Byte dữ liệu đầu tiên có số 7001, nên segment chứa byte 7001..7300 → **ack = 7301**.
2. Một segment có cờ **RST**; chương trình nhận lỗi "Connection refused".

</details>

---

## Bài tập Phần III

**Bài III.1.** Client (ISN = 200) kết nối tới server (ISN = 900). Viết giá trị seq/ack của 3 bước bắt tay, sau đó client gửi 100 byte và server gửi 50 byte. Ghi seq và ack của các segment này.

**Bài III.2.** Với mỗi ứng dụng, chọn TCP hay UDP và giải thích ngắn: (a) tải bản cập nhật game 20 GB; (b) gửi vị trí người chơi 30 lần/giây; (c) tin nhắn chat trong game; (d) gọi thoại trong đội.

**Bài III.3.** Vì sao một game dùng TCP có thể bị "giật" rõ rệt khi mạng mất gói, dù băng thông vẫn còn dư?

<details>
<summary><b>Đáp án Phần III</b> (bấm để mở)</summary>

**III.1.**

| Segment | seq | ack |
|---|---|---|
| Client → SYN | 200 | — |
| Server → SYN+ACK | 900 | 201 |
| Client → ACK | 201 | 901 |
| Client → 100 byte | 201 | 901 |
| Server → 50 byte (kèm ACK) | 901 | 301 |
| Client → ACK | 301 | 951 |

**III.2.** (a) TCP — cần đủ và đúng từng byte. (b) UDP — vị trí cũ vô giá trị, không nên chờ gửi lại. (c) TCP (hoặc UDP có cơ chế tin cậy riêng) — tin nhắn không được mất và phải đúng thứ tự. (d) UDP — trễ thấp quan trọng hơn việc thỉnh thoảng mất vài mẩu âm thanh.

**III.3.** *Head-of-line blocking*: khi một segment bị mất, TCP phải chờ gửi lại nó, và **mọi dữ liệu phía sau** (dù đã tới nơi) đều bị giữ lại chưa giao cho ứng dụng. Đồng thời, cơ chế kiểm soát tắc nghẽn giảm tốc độ gửi.

</details>

---

# PHẦN IV — TẦNG ỨNG DỤNG

## Chương 13. DNS — danh bạ của Internet

### 13.1 Ý tưởng

Con người nhớ tên (`www.example.com`), máy tính cần địa chỉ IP. **DNS** (Domain Name System) chuyển đổi tên thành địa chỉ. DNS thường chạy trên **UDP cổng 53** (chuyển sang **TCP 53** khi phản hồi lớn).

### 13.2 Cấu trúc phân cấp

```text
                          . (root)
          ┌───────────────┼────────────────┐
         com             org               vn          ← TLD (Top-Level Domain)
          │                                 │
       example                             edu         ← tên miền cấp 2, 3...
          │                                 │
         www                               ...         ← tên máy / tên miền con
```

Tên miền đọc từ **phải sang trái**: `www.example.com.` — dấu chấm cuối cùng (thường được ẩn đi) chính là **root**.

### 13.3 Quá trình phân giải

```text
 Máy bạn                 Resolver đệ quy                     Các DNS server
 (stub)                  (của nhà mạng, 8.8.8.8, 1.1.1.1...)
   │                         │
   │ 1. www.example.com? ───►│
   │                         │ 2. hỏi ROOT ──────────────► "Tôi không biết, hỏi server của .com"
   │                         │ 3. hỏi server .com ───────► "Hỏi server của example.com"
   │                         │ 4. hỏi server example.com ► "www.example.com = 198.51.100.20"
   │◄── 5. 198.51.100.20 ────│
```

- **Truy vấn đệ quy (recursive):** máy bạn nhờ resolver "tìm giùm đến khi có kết quả".
- **Truy vấn lặp (iterative):** resolver tự đi hỏi từng cấp, mỗi cấp chỉ chỉ đường tới cấp tiếp theo.
- **Bộ nhớ đệm (cache):** mỗi bản ghi có **TTL** (thời gian sống). Trong thời gian đó, resolver, hệ điều hành và trình duyệt đều dùng lại kết quả mà không hỏi lại. Đó là lý do đổi IP của tên miền cần thời gian mới "lan" hết.
- **File `hosts`** (`/etc/hosts` trên Linux/macOS, `C:\Windows\System32\drivers\etc\hosts` trên Windows) được kiểm tra **trước** DNS — hữu ích để thử nghiệm.

### 13.4 Các loại bản ghi

| Bản ghi | Ý nghĩa | Ví dụ |
|---|---|---|
| **A** | Tên → địa chỉ **IPv4** | `example.com → 198.51.100.20` |
| **AAAA** | Tên → địa chỉ **IPv6** | |
| **CNAME** | Bí danh: tên này là tên khác | `www.shop.com → shop.cdn-provider.net` |
| **MX** | Máy chủ nhận **email** của tên miền | |
| **NS** | Server DNS có thẩm quyền của tên miền | |
| **TXT** | Văn bản tùy ý (xác minh quyền sở hữu, chống giả mạo email: SPF, DKIM...) | |
| **PTR** | Tra ngược: IP → tên | |
| **SRV** | Vị trí (máy + cổng) của một dịch vụ | Một số game dùng để tìm server |

### 13.5 Thử nghiệm

```bash
nslookup example.com            # có trên mọi hệ điều hành
dig example.com A               # Linux/macOS, chi tiết hơn
dig example.com MX +short
dig +trace example.com          # xem toàn bộ quá trình từ root
```

*Ví dụ minh họa output rút gọn của `dig +short`:*

```text
$ dig example.com A +short
198.51.100.20
```

> **Mẹo chẩn đoán:** nếu `ping 8.8.8.8` chạy được nhưng `ping google.com` báo "không tìm thấy máy", thì mạng vẫn thông — **DNS** mới là thứ bị lỗi.

---

## Chương 14. HTTP và HTTPS

### 14.1 HTTP — giao thức của web

**HTTP** là giao thức **yêu cầu – phản hồi**, dạng **văn bản** (với HTTP/1.1), chạy trên TCP (cổng 80; HTTPS dùng cổng 443).

**Một yêu cầu (request):**

```http
GET /products?page=2 HTTP/1.1
Host: shop.example.com
User-Agent: Mozilla/5.0
Accept: text/html
Cookie: session=abc123

```

- Dòng đầu: **phương thức**, **đường dẫn**, **phiên bản**.
- Tiếp theo là các **header** dạng `Tên: giá trị`.
- Một **dòng trống** đánh dấu hết header; sau đó (nếu có) là **thân (body)**.
- Mỗi dòng kết thúc bằng `\r\n`.

**Một phản hồi (response):**

```http
HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8
Content-Length: 1256
Set-Cookie: session=abc123; HttpOnly; Secure

<!doctype html>
<html>...
```

### 14.2 Phương thức

| Phương thức | Ý nghĩa | An toàn (không đổi dữ liệu)? | Idempotent (gọi nhiều lần = một lần)? |
|---|---|:-:|:-:|
| **GET** | Lấy tài nguyên | ✅ | ✅ |
| **POST** | Tạo mới / gửi dữ liệu xử lý | ❌ | ❌ |
| **PUT** | Thay thế toàn bộ tài nguyên | ❌ | ✅ |
| **PATCH** | Sửa một phần | ❌ | ❌ (thường) |
| **DELETE** | Xóa | ❌ | ✅ |
| **HEAD** | Như GET nhưng chỉ lấy header | ✅ | ✅ |
| **OPTIONS** | Hỏi server hỗ trợ gì (dùng trong CORS) | ✅ | ✅ |

### 14.3 Mã trạng thái

| Nhóm | Ý nghĩa | Hay gặp |
|---|---|---|
| **1xx** | Thông tin | 101 Switching Protocols (nâng cấp lên WebSocket) |
| **2xx** | Thành công | **200** OK, **201** Created, **204** No Content |
| **3xx** | Chuyển hướng | **301** Moved Permanently, **302** Found, **304** Not Modified |
| **4xx** | **Lỗi phía client** | **400** Bad Request, **401** Unauthorized, **403** Forbidden, **404** Not Found, **429** Too Many Requests |
| **5xx** | **Lỗi phía server** | **500** Internal Server Error, **502** Bad Gateway, **503** Service Unavailable, **504** Gateway Timeout |

**Phân biệt 401 và 403:** 401 = "bạn là ai? hãy đăng nhập"; 403 = "tôi biết bạn là ai, nhưng bạn không có quyền".

### 14.4 Trạng thái, cookie và REST

- HTTP là **phi trạng thái (stateless)**: mỗi yêu cầu độc lập, server không tự nhớ bạn. Để duy trì phiên đăng nhập, server gửi **cookie** (`Set-Cookie`) hoặc cấp **token**, và client gửi lại trong mỗi yêu cầu.
- **REST API** tổ chức dịch vụ theo **tài nguyên** và dùng đúng phương thức HTTP:

```text
GET    /api/players          → danh sách người chơi
GET    /api/players/42       → người chơi số 42
POST   /api/players          → tạo người chơi mới (dữ liệu JSON trong body)
PATCH  /api/players/42       → sửa một phần thông tin
DELETE /api/players/42       → xóa
```

### 14.5 Các phiên bản HTTP

| Phiên bản | Đặc điểm chính |
|---|---|
| **HTTP/1.1** | Văn bản; giữ kết nối để dùng lại (keep-alive); mỗi kết nối xử lý **tuần tự** từng yêu cầu → trình duyệt mở nhiều kết nối song song |
| **HTTP/2** | Nhị phân; **ghép kênh (multiplexing)** nhiều yêu cầu trên **một** kết nối TCP; nén header. Nhưng vẫn bị head-of-line blocking ở tầng TCP khi mất gói |
| **HTTP/3** | Chạy trên **QUIC** — giao thức xây trên **UDP**, tự lo tin cậy **theo từng luồng**, tích hợp sẵn TLS 1.3, kết nối nhanh hơn và không bị một gói mất làm kẹt mọi luồng |

### 14.6 HTTPS và TLS

**HTTPS = HTTP + TLS.** TLS bảo đảm ba điều:

1. **Bí mật (confidentiality):** người ở giữa không đọc được nội dung.
2. **Toàn vẹn (integrity):** không sửa được nội dung mà không bị phát hiện.
3. **Xác thực (authentication):** bạn đang nói chuyện đúng với `bank.com` thật, không phải kẻ giả mạo.

**Ý tưởng bắt tay TLS 1.3 (đơn giản hóa):**

```text
Client                                                     Server
  │ ── ClientHello: phiên bản, bộ mã hỗ trợ, khóa công khai tạm thời ──► │
  │ ◄── ServerHello: khóa công khai tạm thời của server,                 │
  │     CHỨNG CHỈ số (certificate) + chữ ký chứng minh sở hữu ────────── │
  │                                                                       │
  │  Client kiểm tra chứng chỉ: do CA đáng tin cấp? đúng tên miền?        │
  │  còn hạn? Hai bên cùng tính ra một KHÓA PHIÊN chung (trao đổi khóa    │
  │  Diffie–Hellman) mà người nghe lén không tính được.                   │
  │                                                                       │
  │ ◄══════ Dữ liệu HTTP được mã hóa bằng khóa phiên (đối xứng) ══════►  │
```

- **Mật mã bất đối xứng** (khóa công khai/khóa bí mật) dùng để **xác thực** và **thỏa thuận khóa** — chậm nhưng không cần chia sẻ bí mật trước.
- **Mật mã đối xứng** (như AES) dùng cho **dữ liệu** — nhanh.
- **Chứng chỉ số** được ký bởi **CA (Certificate Authority)**; hệ điều hành và trình duyệt có sẵn danh sách CA tin cậy. Đây là **chuỗi tin cậy (chain of trust)**.
- TLS 1.3 hoàn tất bắt tay chỉ trong **1 vòng khứ hồi**.

> ⚠️ HTTPS bảo vệ **nội dung**, nhưng người quan sát mạng vẫn biết bạn kết nối tới **địa chỉ IP** nào và (thường) **tên miền** nào.

### 14.7 WebSocket

HTTP theo kiểu "client hỏi, server đáp". Với ứng dụng cần server **chủ động đẩy** dữ liệu (chat, game trên trình duyệt, bảng giá trực tiếp), dùng **WebSocket**:

1. Client gửi yêu cầu HTTP có header `Upgrade: websocket`.
2. Server trả `101 Switching Protocols`.
3. Từ đó, **cùng kết nối TCP** trở thành kênh **hai chiều, liên tục**, trao đổi theo từng thông điệp (frame).

Vì chạy trên TCP, WebSocket vẫn chịu head-of-line blocking. Trình duyệt muốn gửi dữ liệu kiểu "không cần tin cậy" như UDP thì dùng **WebRTC data channel** (Chương 20).

---

## Chương 15. Các giao thức ứng dụng khác

| Giao thức | Cổng | Công dụng | Ghi chú |
|---|---|---|---|
| **SSH** | 22 | Đăng nhập và điều khiển máy từ xa an toàn | Nên dùng khóa thay cho mật khẩu |
| **SFTP / SCP** | 22 | Truyền file qua SSH | Thay thế FTP |
| **FTP** | 20, 21 | Truyền file | **Không mã hóa** — tránh dùng |
| **Telnet** | 23 | Đăng nhập từ xa | **Không mã hóa** — chỉ dùng để thử kết nối cổng |
| **SMTP** | 25 (giữa server), 587 (client gửi, dùng STARTTLS), 465 (TLS) | **Gửi** email | |
| **IMAP** | 143, 993 (TLS) | **Đọc** email, đồng bộ trên nhiều thiết bị | |
| **POP3** | 110, 995 (TLS) | Tải email về máy | Ít dùng hơn IMAP |
| **NTP** | 123 (UDP) | Đồng bộ đồng hồ | Sai giờ có thể làm TLS báo lỗi chứng chỉ |
| **RDP** | 3389 | Điều khiển máy Windows từ xa | Không nên mở trực tiếp ra Internet |

**Hành trình một email:** trình soạn thư → (SMTP) → server gửi của bạn → tra **bản ghi MX** của tên miền người nhận → (SMTP) → server nhận → người nhận đọc bằng (IMAP/POP3).

---

# PHẦN V — VẬN DỤNG

## Chương 16. Bảo mật mạng cơ bản

### 16.1 Ba mục tiêu (bộ ba CIA)

| Mục tiêu | Ý nghĩa | Công cụ bảo vệ |
|---|---|---|
| **Confidentiality** (bí mật) | Chỉ người được phép mới đọc được | Mã hóa (TLS, VPN) |
| **Integrity** (toàn vẹn) | Dữ liệu không bị sửa trái phép | Hàm băm, chữ ký số, MAC mật mã |
| **Availability** (sẵn sàng) | Dịch vụ luôn dùng được | Dự phòng, chống DDoS |

### 16.2 Các mối đe dọa phổ biến (cần biết để phòng tránh)

| Mối đe dọa | Cơ chế (tóm tắt) | Phòng tránh |
|---|---|---|
| **Nghe lén (sniffing)** | Đọc lưu lượng không mã hóa trên đường truyền, nhất là Wi-Fi công cộng | Luôn dùng HTTPS, SSH; VPN khi ở mạng lạ |
| **Người đứng giữa (MITM)** | Chen vào giữa hai bên, đọc/sửa dữ liệu | TLS với **kiểm tra chứng chỉ**; không bỏ qua cảnh báo chứng chỉ |
| **Giả mạo ARP (ARP spoofing)** | Gửi ARP reply giả để nhận lưu lượng thay cho gateway | Tính năng bảo vệ trên switch (Dynamic ARP Inspection); mã hóa đầu cuối |
| **Giả mạo DNS** | Trả về địa chỉ sai cho một tên miền | DNSSEC, DNS qua HTTPS/TLS; TLS vẫn phát hiện được server giả |
| **DoS / DDoS** | Làm quá tải dịch vụ bằng lượng lớn yêu cầu, thường từ nhiều máy bị chiếm quyền | Dịch vụ chống DDoS, giới hạn tần suất (rate limiting), CDN |
| **Quét cổng** | Dò xem máy đang mở những dịch vụ nào | Chỉ mở cổng thật sự cần; tường lửa |
| **Lừa đảo (phishing)** | Trang web / email giả mạo để lấy mật khẩu | Kiểm tra tên miền, xác thực hai lớp (2FA) |

### 16.3 Tường lửa (firewall)

Tường lửa **cho phép hoặc chặn** lưu lượng theo quy tắc.

- **Không trạng thái (stateless):** xét từng gói độc lập theo IP/cổng/giao thức.
- **Có trạng thái (stateful):** theo dõi các kết nối đang mở; tự động cho phép gói **trả lời** của kết nối do bên trong khởi tạo. Đây là loại phổ biến nhất hiện nay.
- **Nguyên tắc vàng:** **chặn mặc định, chỉ mở những gì cần** (default deny).

Ví dụ tập quy tắc cho một game server:

| Hướng | Giao thức | Cổng | Hành động |
|---|---|---|---|
| Vào | UDP | 7777 (cổng game) | Cho phép |
| Vào | TCP | 22 (SSH) | Chỉ cho phép từ IP quản trị |
| Vào | Mọi thứ khác | | **Chặn** |
| Ra | Mọi thứ | | Cho phép |

### 16.4 VPN

**VPN** tạo một **đường hầm được mã hóa** từ máy bạn tới một máy chủ VPN. Mọi lưu lượng đi trong đường hầm, nên người trên mạng cục bộ (ví dụ Wi-Fi quán cà phê) không đọc được.

Dùng để: truy cập mạng nội bộ công ty từ xa; bảo vệ khi dùng mạng không tin cậy. Các giao thức phổ biến: **WireGuard**, **OpenVPN**, **IPsec**.

> VPN chuyển **niềm tin** từ nhà mạng sang **nhà cung cấp VPN** — nó không làm bạn "vô hình" tuyệt đối.

### 16.5 Thói quen an toàn

- Wi-Fi dùng **WPA2** hoặc tốt nhất là **WPA3**; không dùng WEP (đã bị phá từ lâu). Đổi mật khẩu quản trị mặc định của router.
- Tắt các dịch vụ không dùng; không mở RDP, SSH, cơ sở dữ liệu trực tiếp ra Internet nếu không cần.
- Cập nhật firmware router và hệ điều hành.
- Khi viết server: **không bao giờ tin dữ liệu từ client** — luôn kiểm tra độ dài, định dạng, giá trị (đặc biệt quan trọng trong game để chống gian lận).

---

## Chương 17. Công cụ chẩn đoán và quy trình xử lý sự cố

### 17.1 Bộ công cụ

| Mục đích | Windows | Linux / macOS |
|---|---|---|
| Xem IP, mask, gateway | `ipconfig /all` | `ip addr`, `ip route` (macOS: `ifconfig`) |
| Kiểm tra kết nối | `ping 8.8.8.8` | `ping -c 4 8.8.8.8` |
| Xem đường đi | `tracert example.com` | `traceroute example.com` (hoặc `mtr`) |
| Tra DNS | `nslookup example.com` | `dig example.com`, `nslookup` |
| Xóa cache DNS | `ipconfig /flushdns` | tùy bản phân phối |
| Xem kết nối và cổng đang mở | `netstat -ano` | `ss -tulpn` (hoặc `netstat -tulpn`) |
| Xem bảng ARP | `arp -a` | `ip neigh` |
| Thử kết nối tới một cổng TCP | `Test-NetConnection host -Port 443` (PowerShell) | `nc -vz host 443` |
| Gửi yêu cầu HTTP | `curl -v https://example.com` | `curl -v https://example.com` |
| Bắt và phân tích gói | Wireshark | Wireshark, `tcpdump` |

*Ví dụ minh họa output của `ping`:*

```text
$ ping -c 3 198.51.100.20
PING 198.51.100.20 56(84) bytes of data.
64 bytes from 198.51.100.20: icmp_seq=1 ttl=56 time=31.2 ms
64 bytes from 198.51.100.20: icmp_seq=2 ttl=56 time=30.8 ms
64 bytes from 198.51.100.20: icmp_seq=3 ttl=56 time=45.9 ms

--- 198.51.100.20 ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2003ms
rtt min/avg/max/mdev = 30.8/35.9/45.9/7.0 ms
```

Đọc kết quả: `time` là RTT; `packet loss` là tỉ lệ mất gói; `mdev` phản ánh **jitter**; `ttl=56` cho thấy gói đã đi qua khoảng vài đến vài chục router (TTL ban đầu thường là 64, 128 hoặc 255 tùy hệ điều hành).

### 17.2 Bộ lọc Wireshark hay dùng

| Bộ lọc | Hiển thị |
|---|---|
| `ip.addr == 192.168.1.10` | Gói có liên quan đến địa chỉ này |
| `tcp.port == 443` | Lưu lượng HTTPS |
| `dns` | Chỉ DNS |
| `arp` | Chỉ ARP |
| `tcp.flags.syn == 1 && tcp.flags.ack == 0` | Các yêu cầu mở kết nối TCP |
| `tcp.analysis.retransmission` | Các gói TCP bị gửi lại (dấu hiệu mất gói) |
| `http.request` | Các yêu cầu HTTP (không mã hóa) |

> 💡 Cách học mạng hiệu quả nhất: mở Wireshark, truy cập một trang web, rồi **tìm lại** trong danh sách gói: truy vấn DNS, bắt tay TCP, bắt tay TLS. Mọi thứ trong tài liệu này sẽ hiện ra trước mắt bạn.

### 17.3 Quy trình xử lý sự cố "không vào được mạng"

Đi **từ tầng thấp lên cao**, dừng ở bước đầu tiên thất bại:

```text
 1. Vật lý / liên kết : Đèn cáp mạng có sáng? Wi-Fi đã kết nối chưa?
          │ OK
 2. Có địa chỉ IP?    : ipconfig / ip addr
          │            └─ 169.254.x.x hoặc không có IP  →  lỗi DHCP
          │ OK
 3. Tới được gateway? : ping <địa chỉ gateway>
          │            └─ thất bại  →  lỗi mạng LAN / Wi-Fi / router
          │ OK
 4. Ra được Internet? : ping 8.8.8.8
          │            └─ thất bại  →  lỗi router hoặc đường truyền nhà mạng
          │ OK
 5. DNS hoạt động?    : nslookup example.com
          │            └─ thất bại  →  lỗi DNS (thử đổi sang 1.1.1.1 hoặc 8.8.8.8)
          │ OK
 6. Dịch vụ đích?     : curl -v https://... , nc -vz host port
                       └─ thất bại  →  tường lửa, dịch vụ tắt, sai cổng, lỗi chứng chỉ
```

**Các "triệu chứng → nguyên nhân" hay gặp:**

| Triệu chứng | Nguyên nhân thường gặp |
|---|---|
| IP `169.254.x.x` | Không liên lạc được DHCP |
| Ping IP được, ping tên miền không được | DNS |
| Vào được máy cùng mạng, không ra được Internet | Sai default gateway, hoặc router mất kết nối WAN |
| "Connection refused" | Máy đích tồn tại nhưng **không có chương trình lắng nghe** cổng đó |
| "Connection timed out" | Tường lửa chặn, sai IP, hoặc máy đích không bật |
| Trang web báo lỗi chứng chỉ | Sai giờ hệ thống, chứng chỉ hết hạn, hoặc có kẻ đứng giữa |
| Game giật, ping dao động mạnh | Wi-Fi nhiễu, có thiết bị khác tải nặng, server ở xa |
| Bạn bè không vào được server tự dựng | Chưa port forwarding, tường lửa máy chặn, hoặc đang ở sau **CGNAT** |

---

## Chương 18. Tổng hợp: chuyện gì xảy ra khi bạn gõ `https://www.example.com`?

Chương này nối **mọi thứ** trong tài liệu thành một câu chuyện. Giả sử laptop vừa kết nối Wi-Fi.

**① DHCP (Ch.9):** laptop nhận IP `192.168.1.10/24`, gateway `192.168.1.1`, DNS `192.168.1.1`.

**② DNS (Ch.13):** trình duyệt kiểm tra cache → hệ điều hành kiểm tra cache và file `hosts` → gửi truy vấn tới `192.168.1.1` (UDP 53). Vì DNS server nằm cùng mạng, laptop dùng **ARP (Ch.4)** để tìm MAC của nó. Resolver hỏi lần lượt root → `.com` → `example.com` và trả về `198.51.100.20`.

**③ Định tuyến (Ch.8):** `198.51.100.20` **không** cùng mạng `192.168.1.0/24` → gói sẽ gửi cho **gateway**. Frame Ethernet/Wi-Fi có **MAC đích = MAC của router**, nhưng **IP đích = `198.51.100.20`**.

**④ NAT (Ch.9):** router đổi IP/cổng nguồn `192.168.1.10:51234` thành IP công khai của nhà bạn, ghi vào bảng NAT.

**⑤ Qua Internet (Ch.8):** gói đi qua hàng chục router của nhiều nhà mạng (nhiều AS, định tuyến bằng BGP giữa các AS). Ở mỗi chặng: **header Ethernet được thay mới**, **TTL giảm 1**, IP đích giữ nguyên.

**⑥ TCP (Ch.12):** bắt tay 3 bước với `198.51.100.20:443`.

**⑦ TLS (Ch.14):** bắt tay TLS, kiểm tra chứng chỉ của `www.example.com`, thỏa thuận khóa phiên.

**⑧ HTTP (Ch.14):** gửi `GET / HTTP/1.1` (hoặc HTTP/2) đã mã hóa → server trả `200 OK` kèm HTML.

**⑨ Trình duyệt** đọc HTML, phát hiện cần thêm CSS, JavaScript, ảnh → lặp lại các bước trên (thường dùng lại kết nối có sẵn) → hiển thị trang.

**⑩ Đóng kết nối (Ch.12):** sau một thời gian không dùng, trao đổi FIN/ACK.

```text
 Laptop ──Wi-Fi──► Router nhà ──quang──► Nhà mạng ──► ... các AS khác ... ──► Server
 [MAC laptop→router]  [MAC mới mỗi chặng]                                    
 [IP 192.168.1.10 → 198.51.100.20]  →NAT→  [IP công khai → 198.51.100.20]
 [cổng 51234 → 443]                  →NAT→  [cổng mới   → 443]
```

> 🎯 **Bài tự kiểm tra tổng hợp:** mở Wireshark, xóa cache DNS, rồi truy cập một trang web. Hãy tìm lại **từng bước** ②, ⑥, ⑦ trong danh sách gói bắt được.

---

## Chương 19. Lập trình socket

> Các chương trình dưới đây viết bằng **C** với **POSIX socket API** (Linux, macOS). Trên Windows, API **Winsock** gần như giống hệt — xem bảng khác biệt ở mục 19.6. Biên dịch bằng: `gcc -Wall -Wextra ten_file.c -o ten_file`.

### 19.1 Các hàm cơ bản

| Hàm | Server TCP | Client TCP | UDP | Công dụng |
|---|:-:|:-:|:-:|---|
| `socket()` | ✅ | ✅ | ✅ | Tạo socket |
| `bind()` | ✅ | (hiếm) | ✅ (bên nhận) | Gắn socket vào địa chỉ + cổng |
| `listen()` | ✅ | | | Chuyển sang trạng thái chờ kết nối |
| `accept()` | ✅ | | | Nhận một kết nối → trả về **socket mới** |
| `connect()` | | ✅ | (tùy chọn) | Bắt tay 3 bước với server |
| `send()` / `recv()` | ✅ | ✅ | | Gửi / nhận trên kết nối TCP |
| `sendto()` / `recvfrom()` | | | ✅ | Gửi / nhận datagram, kèm địa chỉ |
| `close()` | ✅ | ✅ | ✅ | Đóng socket |

```text
        SERVER                                   CLIENT
  socket()                                   socket()
  bind(cổng 9000)                               │
  listen()                                      │
  accept()  ◄──────── bắt tay 3 bước ────────  connect()
     │  (trả về socket mới cho client này)      │
  recv()    ◄──────────── dữ liệu ───────────  send()
  send()    ────────────── dữ liệu ──────────► recv()
  close()   ◄────────────── FIN ─────────────  close()
```

### 19.2 Thứ tự byte mạng (network byte order)

CPU x86/ARM thường lưu số theo kiểu **little-endian** (byte thấp trước), còn giao thức mạng quy ước **big-endian** (byte cao trước). Phải chuyển đổi khi ghi cổng và địa chỉ vào cấu trúc socket:

| Hàm | Ý nghĩa |
|---|---|
| `htons()` / `ntohs()` | Host ↔ Network cho số **16 bit** (cổng) |
| `htonl()` / `ntohl()` | Host ↔ Network cho số **32 bit** (địa chỉ IPv4) |
| `inet_pton()` / `inet_ntop()` | Chuỗi `"192.168.1.10"` ↔ dạng nhị phân |

```c
#include <arpa/inet.h>   // htons, htonl, inet_pton, inet_ntop
#include <stdint.h>
#include <stdio.h>

static void dump(const char *label, const void *p, size_t n) {
    const unsigned char *b = p;
    printf("%-22s", label);
    for (size_t i = 0; i < n; i++) printf("%02x ", b[i]);
    printf("\n");
}

int main(void) {
    uint16_t port = 0x1F90;                 // 8080
    uint16_t net_port = htons(port);        // host -> network byte order
    dump("port (thu tu may):", &port, sizeof port);
    dump("port (thu tu mang):", &net_port, sizeof net_port);
    printf("ntohs tra lai: %u\n", ntohs(net_port));

    // Chuyển địa chỉ IP dạng chữ <-> 4 byte nhị phân
    struct in_addr addr;
    if (inet_pton(AF_INET, "192.168.1.10", &addr) != 1) return 1;
    dump("192.168.1.10 ->", &addr, sizeof addr);

    char text[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr, text, sizeof text);
    printf("Doi nguoc lai: %s\n", text);
    return 0;
}
```

**Kết quả** (trên máy little-endian như x86 hoặc ARM):

```text
port (thu tu may):    90 1f
port (thu tu mang):   1f 90
ntohs tra lai: 8080
192.168.1.10 ->       c0 a8 01 0a
Doi nguoc lai: 192.168.1.10
```

Số 8080 = `0x1F90`. Trong bộ nhớ máy, byte thấp `90` đứng trước; sau `htons`, byte cao `1f` đứng trước — đúng thứ tự mạng. Quên `htons` là lỗi rất phổ biến: server sẽ lắng nghe ở cổng `0x901F` = 36 895 thay vì 8080.

### 19.3 UDP: gửi và nhận datagram

Chương trình sau tự gửi một datagram cho **chính nó** qua địa chỉ loopback, để bạn thấy trọn vẹn quy trình UDP trong một file:

```c
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
    // 1) Tạo socket UDP
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) { perror("socket"); return 1; }

    // 2) Gắn vào 127.0.0.1, cổng 0 = để hệ điều hành tự chọn cổng trống
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(fd, (struct sockaddr *)&addr, sizeof addr) < 0) { perror("bind"); return 1; }

    // Hỏi lại xem hệ điều hành đã chọn cổng nào
    socklen_t len = sizeof addr;
    getsockname(fd, (struct sockaddr *)&addr, &len);
    printf("Da gan socket vao 127.0.0.1, cong do he dieu hanh chon\n");

    // 3) Gửi một datagram cho chính mình
    const char *msg = "hello";
    ssize_t sent = sendto(fd, msg, strlen(msg), 0, (struct sockaddr *)&addr, sizeof addr);
    printf("Da gui %zd byte\n", sent);

    // 4) Nhận datagram, đồng thời biết ai gửi
    char buf[64];
    struct sockaddr_in from;
    socklen_t from_len = sizeof from;
    ssize_t n = recvfrom(fd, buf, sizeof buf - 1, 0, (struct sockaddr *)&from, &from_len);
    if (n < 0) { perror("recvfrom"); return 1; }
    buf[n] = '\0';

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &from.sin_addr, ip, sizeof ip);
    printf("Nhan %zd byte: \"%s\" tu %s (cung cong: %s)\n", n, buf, ip,
           from.sin_port == addr.sin_port ? "co" : "khong");

    close(fd);
    return 0;
}
```

**Kết quả:**

```text
Da gan socket vao 127.0.0.1, cong do he dieu hanh chon
Da gui 5 byte
Nhan 5 byte: "hello" tu 127.0.0.1 (cung cong: co)
```

Để ý: UDP **không** cần `listen`, `accept` hay `connect`. Mỗi `recvfrom` trả về **đúng một datagram** cùng địa chỉ người gửi.

### 19.4 TCP: server và client "echo"

**Server** — nhận kết nối, gửi trả lại mọi thứ nhận được:

```c
// file: tcp_server.c — server "echo": gửi trả lại mọi thứ nhận được
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 9000

// send() có thể chỉ gửi được MỘT PHẦN dữ liệu -> phải lặp đến khi gửi hết
static int send_all(int fd, const char *buf, size_t len) {
    while (len > 0) {
        ssize_t n = send(fd, buf, len, 0);
        if (n <= 0) return -1;
        buf += n;
        len -= (size_t)n;
    }
    return 0;
}

int main(void) {
    // 1) socket(): tạo "đầu cắm" TCP (SOCK_STREAM) dùng IPv4 (AF_INET)
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); return 1; }

    // Cho phép chạy lại server ngay, không phải chờ hết TIME_WAIT
    int yes = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    // 2) bind(): gắn socket vào địa chỉ + cổng
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);   // nhận kết nối trên mọi card mạng
    addr.sin_port = htons(PORT);                // nhớ đổi sang thứ tự byte mạng
    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof addr) < 0) { perror("bind"); return 1; }

    // 3) listen(): chuyển sang trạng thái chờ kết nối
    if (listen(listen_fd, 16) < 0) { perror("listen"); return 1; }
    printf("Server dang lang nghe o cong %d...\n", PORT);
    fflush(stdout);

    for (;;) {
        // 4) accept(): chờ và nhận MỘT kết nối -> trả về socket MỚI dành riêng cho client đó
        struct sockaddr_in client;
        socklen_t client_len = sizeof client;
        int conn_fd = accept(listen_fd, (struct sockaddr *)&client, &client_len);
        if (conn_fd < 0) { perror("accept"); continue; }

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, ip, sizeof ip);
        printf("Ket noi tu %s:%u\n", ip, ntohs(client.sin_port));
        fflush(stdout);

        // 5) recv()/send(): đọc đến khi client đóng kết nối (recv trả về 0)
        char buf[1024];
        ssize_t n;
        while ((n = recv(conn_fd, buf, sizeof buf, 0)) > 0) {
            if (send_all(conn_fd, buf, (size_t)n) < 0) break;
        }
        printf("Client %s da ngat ket noi\n", ip);
        fflush(stdout);

        // 6) close(): đóng socket của client này, quay lại chờ client khác
        close(conn_fd);
    }
}
```

**Client** — kết nối tới server, gửi một câu và in phản hồi:

```c
// file: tcp_client.c — gửi một thông điệp tới server echo và in phản hồi
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    const char *server_ip = "127.0.0.1";
    const char *msg = argc > 1 ? argv[1] : "Xin chao server";

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return 1; }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    if (inet_pton(AF_INET, server_ip, &addr.sin_addr) != 1) {
        fprintf(stderr, "Dia chi IP khong hop le\n");
        return 1;
    }

    // connect(): thực hiện bắt tay 3 bước với server
    if (connect(fd, (struct sockaddr *)&addr, sizeof addr) < 0) { perror("connect"); return 1; }

    size_t len = strlen(msg);
    if (send(fd, msg, len, 0) != (ssize_t)len) { perror("send"); return 1; }

    // TCP là DÒNG BYTE: một lần recv có thể chỉ nhận một phần -> đọc đến khi đủ
    char buf[1024];
    size_t got = 0;
    while (got < len && got < sizeof buf - 1) {
        ssize_t n = recv(fd, buf + got, sizeof buf - 1 - got, 0);
        if (n <= 0) break;
        got += (size_t)n;
    }
    buf[got] = '\0';
    printf("Server tra loi: %s\n", buf);

    close(fd);   // gửi FIN -> bắt đầu đóng kết nối
    return 0;
}
```

**Chạy thử** trên hai cửa sổ terminal:

```bash
# Terminal 1
gcc -Wall -Wextra tcp_server.c -o tcp_server
./tcp_server

# Terminal 2
gcc -Wall -Wextra tcp_client.c -o tcp_client
./tcp_client "Hello TCP"
./tcp_client
```

**Kết quả ở Terminal 2:**

```text
Server tra loi: Hello TCP
Server tra loi: Xin chao server
```

**Kết quả ở Terminal 1** (số cổng của client do hệ điều hành chọn, sẽ khác trên máy bạn):

```text
Server dang lang nghe o cong 9000...
Ket noi tu 127.0.0.1:39664
Client 127.0.0.1 da ngat ket noi
Ket noi tu 127.0.0.1:39666
Client 127.0.0.1 da ngat ket noi
```

**Những điểm then chốt trong code:**

- `accept()` trả về một **socket mới** cho từng client; socket lắng nghe vẫn tiếp tục chờ client khác.
- `send()` có thể chỉ gửi **một phần** dữ liệu → hàm `send_all` lặp đến khi gửi hết.
- `recv()` trả về **0** khi phía bên kia đóng kết nối, **âm** khi có lỗi.
- Client đọc trong vòng lặp vì TCP là **dòng byte** (Chương 12.8).
- Server này xử lý **lần lượt từng client**. Muốn phục vụ nhiều client cùng lúc, có ba hướng: mỗi client một **luồng** (thread); dùng **`select`/`poll`/`epoll`** (Linux) hoặc **IOCP** (Windows) để một luồng theo dõi nhiều socket; hoặc dùng thư viện như **Asio** (C++) hay **libuv**.

### 19.5 Tự viết HTTP client

HTTP chỉ là **văn bản gửi qua TCP**. Chương trình sau dùng `getaddrinfo` để phân giải tên miền (hỗ trợ cả IPv4 và IPv6), rồi gửi một yêu cầu GET "bằng tay":

```c
// file: http_get.c — tự viết HTTP client tối giản: ./http_get <host> <port> <path>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Cach dung: %s <host> <port> <path>\n", argv[0]);
        return 1;
    }
    const char *host = argv[1], *port = argv[2], *path = argv[3];

    // 1) Phân giải tên miền (DNS) -> danh sách địa chỉ (có thể có cả IPv4 và IPv6)
    struct addrinfo hints = {0}, *res;
    hints.ai_family = AF_UNSPEC;        // chấp nhận cả IPv4 lẫn IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP
    int err = getaddrinfo(host, port, &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    // 2) Thử lần lượt từng địa chỉ cho đến khi kết nối được
    int fd = -1;
    for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0) continue;
        if (connect(fd, p->ai_addr, p->ai_addrlen) == 0) break;
        close(fd);
        fd = -1;
    }
    freeaddrinfo(res);
    if (fd < 0) {
        fprintf(stderr, "Khong ket noi duoc toi %s:%s\n", host, port);
        return 1;
    }

    // 3) Gửi yêu cầu HTTP — chỉ là văn bản, mỗi dòng kết thúc bằng \r\n
    char req[512];
    int req_len = snprintf(req, sizeof req,
                           "GET %s HTTP/1.1\r\n"
                           "Host: %s\r\n"
                           "Connection: close\r\n"
                           "\r\n",                  // dòng trống: hết phần header
                           path, host);
    if (send(fd, req, (size_t)req_len, 0) != req_len) { perror("send"); return 1; }

    // 4) Đọc toàn bộ phản hồi (server sẽ đóng kết nối vì "Connection: close")
    char buf[4096];
    size_t total = 0;
    int printed_status = 0;
    ssize_t n;
    while ((n = recv(fd, buf, sizeof buf, 0)) > 0) {
        if (!printed_status) {                       // in dòng trạng thái đầu tiên
            char *eol = memchr(buf, '\r', (size_t)n);
            if (eol) printf("Dong trang thai: %.*s\n", (int)(eol - buf), buf);
            printed_status = 1;
        }
        total += (size_t)n;
    }
    printf("Tong cong nhan %zu byte (gom header + body)\n", total);
    close(fd);
    return 0;
}
```

**Kết quả** khi thử với một web server chạy trên chính máy (`python3 -m http.server 8000` trong một thư mục có file `index.html`):

```text
$ ./http_get localhost 8000 /index.html
Dong trang thai: HTTP/1.0 200 OK
Tong cong nhan 203 byte (gom header + body)

$ ./http_get localhost 8000 /khong-co
Dong trang thai: HTTP/1.0 404 File not found
Tong cong nhan 520 byte (gom header + body)

$ ./http_get khong-ton-tai.invalid 80 /
getaddrinfo: Name or service not known
```

(Số byte phụ thuộc phiên bản Python và nội dung file. Phần lớn website thật hiện nay chỉ phục vụ nội dung qua HTTPS; khi gửi yêu cầu HTTP thường tới cổng 80, bạn thường nhận được mã chuyển hướng (`301` hoặc `308`) sang địa chỉ `https://` — đúng như Chương 14 mô tả. Chương trình này không hỗ trợ TLS; muốn gọi HTTPS từ C/C++, hãy dùng thư viện như libcurl hoặc OpenSSL.)

### 19.6 Khác biệt khi dùng Winsock (Windows)

| POSIX | Winsock | Ghi chú |
|---|---|---|
| (không cần) | `WSAStartup(MAKEWORD(2, 2), &wsa)` / `WSACleanup()` | Gọi một lần khi bắt đầu / kết thúc chương trình |
| `#include <sys/socket.h>` ... | `#include <winsock2.h>` và `<ws2tcpip.h>` | Liên kết với thư viện `ws2_32` |
| `int fd` | `SOCKET s` | Kiểu riêng |
| `fd < 0` | `s == INVALID_SOCKET` | Kiểm tra lỗi khi tạo socket |
| Hàm trả về `-1` | Hàm trả về `SOCKET_ERROR` | |
| `close(fd)` | `closesocket(s)` | |
| `errno`, `perror()` | `WSAGetLastError()` | |
| `ssize_t` | `int` | Kiểu trả về của `send`/`recv` |

Phần còn lại (`bind`, `listen`, `accept`, `connect`, `send`, `recv`, `htons`, `inet_pton`, `getaddrinfo`...) dùng gần như y hệt.

### 19.7 Thiết kế giao thức riêng cho ứng dụng

Khi tự viết client–server, bạn cần định nghĩa **định dạng thông điệp**:

- **Văn bản, phân cách bằng dòng** (`"MOVE 10 20\n"`): dễ đọc, dễ debug, hơi tốn băng thông.
- **Nhị phân có tiền tố độ dài**: `[độ dài 4 byte][loại 1 byte][dữ liệu...]` — gọn, nhanh; nhớ dùng thứ tự byte mạng và kiểu có kích thước cố định (`uint32_t`).
- **Định dạng có sẵn:** JSON (dễ dùng), Protocol Buffers, FlatBuffers, MessagePack (gọn, nhanh).

Luôn **kiểm tra độ dài** trước khi đọc dữ liệu từ mạng — tin tưởng mù quáng vào trường độ dài do client gửi là nguồn gốc của nhiều lỗ hổng bảo mật.

---
## Chương 20. Mạng cho game online

### 20.1 Kiến trúc

| Kiến trúc | Cách hoạt động | Ưu điểm | Nhược điểm |
|---|---|---|---|
| **Client–server có thẩm quyền (authoritative server)** | Server chạy mô phỏng "thật"; client chỉ gửi **input** và hiển thị kết quả | Chống gian lận tốt, dễ đồng bộ | Tốn chi phí server; độ trễ phụ thuộc vị trí server |
| **P2P / host–client** | Một người chơi làm host, hoặc các máy gửi trực tiếp cho nhau | Không cần server riêng | Dễ gian lận; vướng NAT; host rời thì trận đấu gặp vấn đề |

Hầu hết game nhiều người chơi cạnh tranh hiện nay dùng **server có thẩm quyền**. Nguyên tắc: **"Không bao giờ tin client"** — client gửi "tôi bấm phím tiến", không phải "tôi đang ở tọa độ (100, 200)".

### 20.2 Tick rate và gửi dữ liệu

- Server cập nhật mô phỏng theo nhịp cố định gọi là **tick** (thường từ khoảng 20 đến vài chục lần mỗi giây; một số game bắn súng cạnh tranh dùng 64 hoặc 128 tick).
- Mỗi tick, server gửi **ảnh chụp trạng thái (snapshot)** cho client. Để tiết kiệm băng thông: chỉ gửi **phần thay đổi** so với lần trước (delta), lượng tử hóa số thực (ví dụ lưu góc quay bằng 16 bit thay vì `double` 64 bit), và chỉ gửi đối tượng **ở gần** người chơi.
- **UDP** là lựa chọn phổ biến cho dữ liệu thời gian thực. Game thường tự xây một lớp **"tin cậy khi cần"** trên UDP: vị trí thì gửi liên tục và chấp nhận mất; sự kiện quan trọng (nhặt vật phẩm, chat) thì đánh số thứ tự và yêu cầu xác nhận. Các thư viện như **ENet**, **GameNetworkingSockets**, hay giao thức **QUIC** cung cấp sẵn những tính năng này.

### 20.3 Đối phó với độ trễ

Với độ trễ 100 ms, nếu client phải chờ server xác nhận mới di chuyển nhân vật, game sẽ có cảm giác "dính". Bốn kỹ thuật kinh điển:

| Kỹ thuật | Ý tưởng |
|---|---|
| **Dự đoán phía client (client-side prediction)** | Client **tự mô phỏng** ngay kết quả input của mình, không chờ server |
| **Đối chiếu với server (server reconciliation)** | Khi nhận trạng thái chính thức, client **sửa lại** vị trí và **áp lại** các input server chưa xử lý |
| **Nội suy thực thể (entity interpolation)** | Hiển thị người chơi **khác** ở một thời điểm hơi lùi về quá khứ (ví dụ 100 ms), nội suy mượt giữa hai snapshot |
| **Bù độ trễ (lag compensation)** | Khi xử lý phát bắn, server **"tua lại"** vị trí mục tiêu về đúng thời điểm người bắn nhìn thấy |

```text
Thời gian ─────────────────────────────────────────────────►

Client:  [nhấn →] di chuyển NGAY (dự đoán) ... nhận xác nhận từ server → khớp? giữ nguyên : sửa lại
Server:           ......... nhận input (trễ ~50ms) → mô phỏng → gửi trạng thái chính thức .........
```

### 20.4 Game trên trình duyệt

Trình duyệt **không cho phép** mở socket TCP/UDP tùy ý. Các lựa chọn:

| Công nghệ | Nền tảng | Đặc điểm |
|---|---|---|
| **WebSocket** | TCP | Đơn giản, hỗ trợ rộng rãi; tin cậy và đúng thứ tự nên chịu head-of-line blocking |
| **WebRTC data channel** | UDP (qua DTLS/SCTP) | Cấu hình được chế độ **không tin cậy, không cần thứ tự** — gần với UDP nhất; thiết lập phức tạp hơn (cần máy chủ báo hiệu, STUN/TURN) |
| **WebTransport** | HTTP/3 (QUIC) | Hỗ trợ cả luồng tin cậy lẫn datagram; mới hơn nên cần kiểm tra mức độ hỗ trợ của trình duyệt |

Với game trình duyệt nhịp độ vừa phải, **WebSocket** thường là điểm khởi đầu hợp lý nhất; chỉ chuyển sang WebRTC hoặc WebTransport khi head-of-line blocking thật sự gây vấn đề.

### 20.5 Đo lường khi phát triển

- Luôn thử game trong điều kiện **mạng xấu**: thêm độ trễ, jitter, mất gói giả lập (Linux: `tc netem`; Windows: công cụ như Clumsy; nhiều engine có sẵn công cụ giả lập).
- Hiển thị các chỉ số RTT, tỉ lệ mất gói, băng thông gửi/nhận ngay trong game ở bản debug.

---

## Bài tập Phần IV và V

**Bài V.1.** `ping 8.8.8.8` thành công nhưng trình duyệt báo "DNS_PROBE_FINISHED_NXDOMAIN" với mọi trang. Bạn kiểm tra và xử lý thế nào?

**Bài V.2.** Sắp xếp đúng thứ tự: bắt tay TLS, truy vấn DNS, bắt tay TCP, yêu cầu HTTP, ARP tìm MAC của gateway (giả sử cache đều trống, DNS server là router).

**Bài V.3.** Một API trả về `401` khi bạn gọi không kèm token và `403` khi bạn dùng token của tài khoản thường để gọi chức năng dành cho quản trị viên. Giải thích sự khác nhau.

**Bài V.4.** Sửa chương trình `tcp_client.c` để nhận địa chỉ IP của server từ tham số dòng lệnh thứ hai (`./tcp_client "tin nhan" 192.168.1.50`). Cần chạy server thế nào để máy khác trong mạng LAN kết nối được?

**Bài V.5.** Bạn dựng server game tại nhà ở cổng UDP 7777. Bạn bè ở xa không vào được. Liệt kê các nguyên nhân có thể có theo thứ tự nên kiểm tra.

**Bài V.6.** Vì sao một game bắn súng nên gửi vị trí qua UDP, nhưng thông báo "người chơi X đã bị hạ gục" thì cần được gửi **tin cậy**?

<details>
<summary><b>Đáp án Phần IV và V</b> (bấm để mở)</summary>

**V.1.** Mạng vẫn thông (ra được Internet bằng IP), lỗi nằm ở **DNS**. Kiểm tra bằng `nslookup example.com`; thử chỉ định DNS khác: `nslookup example.com 1.1.1.1`. Nếu cách thứ hai chạy được, đổi DNS của máy hoặc router sang `1.1.1.1` / `8.8.8.8`, rồi xóa cache DNS (`ipconfig /flushdns` trên Windows). Kiểm tra thêm file `hosts` xem có dòng lạ không.

**V.2.** ARP tìm MAC gateway → truy vấn DNS → bắt tay TCP → bắt tay TLS → yêu cầu HTTP.

**V.3.** `401 Unauthorized`: server **chưa biết bạn là ai** (thiếu hoặc sai thông tin xác thực). `403 Forbidden`: server **đã biết bạn là ai**, nhưng tài khoản này **không có quyền** thực hiện thao tác.

**V.4.** Thay dòng khai báo địa chỉ thành:

```c
const char *server_ip = argc > 2 ? argv[2] : "127.0.0.1";
```

Server trong tài liệu đã bind vào `INADDR_ANY` (`0.0.0.0`) nên nhận kết nối từ mọi card mạng — không cần sửa. Nhưng cần: biết IP LAN của máy chạy server (`ipconfig` / `ip addr`), và **tường lửa** trên máy đó cho phép cổng TCP 9000 đi vào.

**V.5.** (1) Server có đang chạy và lắng nghe đúng cổng/giao thức không (`ss -ulpn` / `netstat -ano`)? (2) Tường lửa trên máy server có cho phép UDP 7777 không? (3) Router đã **port forwarding** UDP 7777 tới đúng IP LAN của máy server chưa (nên đặt DHCP reservation để IP không đổi)? (4) Bạn bè có dùng đúng **IP công khai** hiện tại không? (5) Nhà mạng có đang dùng **CGNAT** không (so IP cổng WAN của router với IP công khai)? Nếu có, cần xin IP công khai, dùng VPN/tunnel, hoặc thuê server.

**V.6.** Vị trí được gửi liên tục, gói mới luôn thay thế gói cũ — gửi lại gói mất chỉ làm tăng độ trễ mà không có ích. Còn sự kiện hạ gục chỉ xảy ra **một lần** và làm thay đổi trạng thái trận đấu; nếu bị mất, client sẽ hiển thị sai vĩnh viễn. Vì vậy sự kiện cần được đánh số, xác nhận và gửi lại khi mất.

</details>

---

# PHỤ LỤC

## Phụ lục A. Bảng cổng phổ biến

| Cổng | Giao thức | Dịch vụ |
|---|---|---|
| 20, 21 | TCP | FTP |
| 22 | TCP | SSH, SFTP, SCP |
| 23 | TCP | Telnet |
| 25 | TCP | SMTP (giữa các mail server) |
| 53 | UDP / TCP | DNS |
| 67, 68 | UDP | DHCP (server, client) |
| 80 | TCP | HTTP |
| 110 | TCP | POP3 |
| 123 | UDP | NTP |
| 143 | TCP | IMAP |
| 179 | TCP | BGP |
| 443 | TCP (và UDP cho HTTP/3) | HTTPS |
| 465, 587 | TCP | SMTP có mã hóa / gửi thư từ client |
| 993 | TCP | IMAP qua TLS |
| 995 | TCP | POP3 qua TLS |
| 1194 | UDP | OpenVPN (mặc định) |
| 3306 | TCP | MySQL / MariaDB |
| 3389 | TCP | RDP (Remote Desktop) |
| 5432 | TCP | PostgreSQL |
| 6379 | TCP | Redis |
| 8080 | TCP | HTTP thay thế (thường dùng khi phát triển) |
| 27017 | TCP | MongoDB |
| 51820 | UDP | WireGuard (mặc định) |

## Phụ lục B. Bảng tra nhanh chia mạng

**Lũy thừa của 2:**

| $n$ | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 10 | 12 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|
| $2^n$ | 2 | 4 | 8 | 16 | 32 | 64 | 128 | 256 | 1 024 | 4 096 | 65 536 |

**Mask theo prefix (ở các byte khác nhau):**

| Byte thứ 2 | Byte thứ 3 | Byte thứ 4 | Giá trị byte mask | Block size |
|---|---|---|---|---|
| /9 | /17 | /25 | 128 | 128 |
| /10 | /18 | /26 | 192 | 64 |
| /11 | /19 | /27 | 224 | 32 |
| /12 | /20 | /28 | 240 | 16 |
| /13 | /21 | /29 | 248 | 8 |
| /14 | /22 | /30 | 252 | 4 |
| /15 | /23 | /31 | 254 | 2 |
| /16 | /24 | /32 | 255 | 1 |

Ví dụ: /20 → nhìn cột "Byte thứ 3" → mask `255.255.240.0`, block 16 ở byte thứ 3.

**Quy trình 5 bước** (Chương 6.2): tìm byte thú vị → block = 256 − mask → tìm bội số của block gần nhất không vượt quá → broadcast = mạng kế tiếp − 1 → số host = $2^h - 2$.

**Kiểm tra lại bằng Python** (rất tiện khi tự luyện):

```python
import ipaddress

net = ipaddress.ip_interface("172.16.45.200/20").network
print(net, net.netmask, net.broadcast_address, net.num_addresses - 2)
# 172.16.32.0/20 255.255.240.0 172.16.47.255 4094

for sub in ipaddress.ip_network("192.168.1.0/24").subnets(new_prefix=26):
    print(sub)
```

## Phụ lục C. Hiểu lầm phổ biến

1. **"Mbps và MB/s là một."** Sai — chênh nhau 8 lần.
2. **"Băng thông cao thì ping thấp."** Không nhất thiết; độ trễ chủ yếu do khoảng cách, số chặng và tắc nghẽn.
3. **"Địa chỉ MAC dùng để định tuyến trên Internet."** Sai — MAC chỉ có ý nghĩa trong một mạng LAN, bị thay ở mỗi chặng.
4. **"Mạng /24 có 256 máy dùng được."** Sai — chỉ 254 (trừ địa chỉ mạng và broadcast).
5. **"Gửi 2 lần thì nhận 2 lần" với TCP.** Sai — TCP là dòng byte.
6. **"UDP không đáng tin nên không nên dùng."** Sai — UDP là lựa chọn đúng cho dữ liệu thời gian thực, và nhiều giao thức hiện đại (QUIC) xây trên nó.
7. **"HTTPS che giấu hoàn toàn tôi đang truy cập trang nào."** Không — IP đích và thường cả tên miền vẫn nhìn thấy được.
8. **"NAT là tường lửa."** NAT **tình cờ** chặn kết nối chủ động từ ngoài vào, nhưng nó không được thiết kế để bảo mật; vẫn cần tường lửa thật.
9. **"Ping không được nghĩa là máy đích tắt."** Không chắc — nhiều máy và tường lửa chặn ICMP.
10. **"`127.0.0.1` và IP LAN của máy là như nhau."** Không — server chỉ bind vào `127.0.0.1` thì **máy khác không kết nối được**; phải bind vào `0.0.0.0` hoặc IP LAN.
11. **"IPv6 chỉ là IPv4 với địa chỉ dài hơn."** Không hẳn — IPv6 không có broadcast, dùng NDP thay ARP, router không phân mảnh, có SLAAC.

## Phụ lục D. Lộ trình và tài liệu

### D.1 Lộ trình gợi ý (khoảng 8 tuần)

| Tuần | Nội dung | Mục tiêu tối thiểu |
|---|---|---|
| 1 | Chương 1–4 | Giải thích được đóng gói và vai trò của MAC, IP, port; hiểu switch và ARP |
| 2–3 | Chương 5–6 | **Chia mạng con không cần máy tính** trong vòng 1–2 phút mỗi bài |
| 4 | Chương 7–10 | Đọc được bảng định tuyến; giải thích NAT, DHCP; rút gọn IPv6 |
| 5 | Chương 11–12 | Vẽ lại bắt tay TCP và tính được seq/ack |
| 6 | Chương 13–15 | Dùng thành thạo `dig`/`nslookup`, `curl -v`; đọc hiểu request/response |
| 7 | Chương 16–18 + Wireshark | Tự tìm lại DNS, TCP, TLS trong một lần bắt gói |
| 8 | Chương 19–20 | Chạy được server/client echo; mở rộng thành phòng chat nhiều người |

### D.2 Tài liệu nên dùng song song

- **"Computer Networking: A Top-Down Approach"** (Kurose & Ross) — giáo trình nhập môn phổ biến nhất, đi từ tầng ứng dụng xuống.
- **"Computer Networks"** (Tanenbaum) — giáo trình kinh điển, đi từ tầng vật lý lên.
- **"TCP/IP Illustrated, Vol. 1"** (Stevens) — khi muốn hiểu sâu từng giao thức qua gói tin thật.
- **Beej's Guide to Network Programming** — hướng dẫn lập trình socket bằng C miễn phí, rất dễ đọc.
- **"High Performance Browser Networking"** (Ilya Grigorik, đọc miễn phí trên web) — TCP, TLS, HTTP/2, WebSocket, WebRTC từ góc nhìn hiệu năng.
- **Cisco Networking Academy / tài liệu ôn CCNA** — nếu muốn đi theo hướng quản trị mạng, có phần thực hành với Packet Tracer.
- **Wireshark** — công cụ học mạng tốt nhất; kèm các file bắt gói mẫu trên trang wiki của Wireshark.
- **Cho lập trình game mạng:** loạt bài "Fast-Paced Multiplayer" của Gabriel Gambetta; blog "Gaffer On Games" của Glenn Fiedler; tài liệu "Source Multiplayer Networking" trên Valve Developer Community.
- **RFC** (tài liệu đặc tả chính thức, tại rfc-editor.org) — khi cần câu trả lời chính xác tuyệt đối, ví dụ RFC 791 (IPv4), RFC 9293 (TCP), RFC 8446 (TLS 1.3).

---

*Chúc bạn học tốt! Mạng máy tính học nhanh nhất khi bạn **nhìn thấy nó**: hãy mở Wireshark và chạy thử mọi thứ trong tài liệu này.*
