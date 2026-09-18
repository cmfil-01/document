// ============================================================
// Cross-Platform CPP Messenger Server with Authentication
// (Server chat qua mạng, hỗ trợ đăng ký/đăng nhập, chạy được
//  cả trên Windows và Linux)
// ============================================================
//
// Đây là phần SERVER tương ứng với file client đã chú thích trước đó.
// Server này: lắng nghe kết nối TCP, xác thực người dùng (đăng ký/đăng
// nhập có lưu vào file), và chuyển tiếp (broadcast) tin nhắn giữa các
// client đang kết nối — mỗi client được xử lý trên một luồng riêng.
//
// Các dòng chú thích thêm vào bắt đầu bằng "//>" để dễ phân biệt với
// comment gốc của tác giả. Những khái niệm đã giải thích kỹ ở file
// client (ifdef, atomic, mutex cơ bản, socket/sockaddr_in, htons...)
// sẽ chỉ nhắc lại ngắn gọn ở đây; phần MỚI (map, vector, lambda,
// fstream, bind/listen/accept...) sẽ được giải thích kỹ hơn.
//
//> YÊU CẦU BIÊN DỊCH: file này dùng std::erase_if nên cần C++20.
//>   Linux:   g++ -std=c++20 -pthread server.cpp -o server
//>   Windows: cl /std:c++20 /DWINDOWS_BUILD server.cpp ws2_32.lib
//>            (hoặc MinGW: g++ -std=c++20 -DWINDOWS_BUILD server.cpp -lws2_32)

#if defined(WINDOWS_BUILD) || defined(_WIN32)
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    //> Macro này tắt cảnh báo "deprecated" (không khuyến khích dùng nữa)
    //> mà trình biên dịch MSVC hay đưa ra với một số hàm Winsock cũ.
    // DO NOT define _CRT_SECURE_NO_WARNINGS here; configure it in your build system instead.
#endif

#include <iostream>
#include <cstring>
#include <string>
#include <thread>
#include <vector>      //> std::vector — mảng động (tự thay đổi kích thước)
#include <mutex>
#include <algorithm>
#include <array>        //> std::array — mảng kích thước cố định, an toàn hơn mảng C-style
#include <map>          //> std::map — cấu trúc "từ điển" (key → value), tự sắp xếp theo key
#include <ctime>        //> time(), localtime_s/localtime_r — lấy và định dạng thời gian
#include <iomanip>      //> std::put_time — định dạng thời gian theo kiểu C++ (thay cho strftime)
#include <sstream>      //> std::stringstream — đọc/ghi dữ liệu như một luồng, dùng để tách chuỗi
#include <fstream>      //> std::ifstream, std::ofstream — đọc/ghi file trên đĩa
#include <functional>   //> std::hash — công cụ băm (hash) dữ liệu có sẵn của thư viện chuẩn

// ------------------------------------------------------------
//> Giống file client: dùng biên dịch có điều kiện để cùng một file .cpp
//> chạy được trên cả Windows lẫn Linux (API socket của 2 hệ điều hành khác nhau).
#if defined(WINDOWS_BUILD) || defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using socklen_t = int;
    //> HIỆN ĐẠI HÓA: dùng "using" thay cho "typedef" cũ. Hai cách hoàn toàn
    //> tương đương với kiểu đơn giản như thế này, nhưng "using" đọc xuôi
    //> hơn (tên_mới = kiểu_cũ) và là cách được khuyến nghị từ C++11 trở đi.
    // Don't #define close to closesocket -- that breaks std::ifstream::close() etc.
    //> Ghi chú quan trọng của tác giả: KHÔNG được #define close thành
    //> closesocket như file client đã làm, vì file server này còn dùng
    //> ifstream/ofstream (đọc/ghi file users.dat), mà các luồng file đó
    //> cũng có hàm thành viên tên là close() — nếu #define close thì
    //> trình biên dịch sẽ thay luôn cả file.close() thành file.closesocket(),
    //> gây lỗi biên dịch. Đây là lý do bên dưới có một HÀM RIÊNG
    //> closeSocket() để đóng socket một cách an toàn, thay vì dùng macro.
#else
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <arpa/inet.h>   //> inet_ntop() — chuyển địa chỉ IP dạng nhị phân sang chuỗi
    using SOCKET = int;
    constexpr SOCKET INVALID_SOCKET = -1;
    constexpr int SOCKET_ERROR = -1;
    //> HIỆN ĐẠI HÓA: thay "#define INVALID_SOCKET -1" bằng hằng số constexpr.
    //> Macro #define chỉ là THAY THẾ VĂN BẢN mù quáng: nó không có kiểu dữ
    //> liệu, không tuân theo phạm vi (scope), và có thể vô tình thay thế cả
    //> những chỗ trùng tên trong thư viện khác. constexpr tạo ra một hằng số
    //> THẬT SỰ, có kiểu rõ ràng (SOCKET / int), được trình biên dịch kiểm
    //> tra kiểu và vẫn được tính sẵn lúc biên dịch nên không hề chậm hơn.
    //> (Trên Windows, INVALID_SOCKET và SOCKET_ERROR đã có sẵn trong Winsock.)
#endif
// ------------------------------------------------------------

//> HIỆN ĐẠI HÓA: đã BỎ dòng "using namespace std;" ở phạm vi toàn cục.
//> Kéo toàn bộ thư viện chuẩn vào không gian tên toàn cục dễ gây "đụng tên"
//> (name collision) với code của chính mình — ví dụ std::bind (trong
//> <functional>) đụng với hàm bind() của socket, đúng kiểu rắc rối mà file
//> này đã phải né bằng "::bind" ở hàm main. Viết std:: đầy đủ ở mỗi chỗ dùng
//> dài hơn một chút nhưng rõ ràng và an toàn hơn — đây là cách làm được
//> khuyến nghị trong mọi dự án C++ nghiêm túc hiện nay.

// ------------------------------------------------------------
//> ĐỊNH NGHĨA KIỂU DỮ LIỆU RIÊNG (struct)
//> struct trong C++ giống class nhưng mặc định các thành viên là "public".
//> Dùng để gom nhiều biến liên quan lại thành MỘT kiểu dữ liệu mới.
struct User {
    std::string username;
    std::string passwordHash;
    //> Không lưu mật khẩu gốc, chỉ lưu bản đã băm (hash) — xem hashPassword() bên dưới.
};

struct Client {
    SOCKET socket{};        //> socket kết nối riêng của client này
    std::string username;
    std::string ipAddress;
    bool authenticated{false};   //> đã đăng nhập thành công hay chưa
    //> HIỆN ĐẠI HÓA: thêm "khởi tạo mặc định ngay tại chỗ khai báo"
    //> (default member initializer, có từ C++11) — "{}" và "{false}".
    //> Nhờ vậy, nếu sau này ai đó tạo một Client mà quên gán đủ các trường,
    //> các trường còn lại vẫn có giá trị xác định (0 / false) thay vì chứa
    //> "rác" trong bộ nhớ — một nguồn lỗi rất khó tìm.
};
// ------------------------------------------------------------

// ------------------------------------------------------------
//> DỮ LIỆU TOÀN CỤC — CHIA SẺ GIỮA MỌI LUỒNG XỬ LÝ CLIENT
std::vector<Client> clients;
//> vector<Client>: một "mảng động" chứa các struct Client — có thể
//> push_back() thêm phần tử, erase() xóa phần tử, tự động cấp phát lại
//> bộ nhớ khi cần, khác với mảng C-style có kích thước cố định.
std::mutex clientsMutex;
//> Vì NHIỀU LUỒNG (mỗi client một luồng riêng — xem handleClient) có thể
//> cùng lúc đọc/ghi "clients", cần khóa mutex này mỗi khi truy cập để
//> tránh 2 luồng sửa vector cùng lúc gây lỗi/crash (data race).
std::map<std::string, User> users;
//> map<string, User>: cấu trúc kiểu "từ điển" — tra cứu nhanh một User
//> bằng KEY là username (chuỗi). users["alice"] sẽ trả về (hoặc tạo mới)
//> phần tử User có key "alice".
std::mutex usersMutex;
std::map<std::string, std::vector<std::string>> messageHistory;
//> map mà GIÁ TRỊ (value) lại là một vector<string> — tức mỗi key (ở đây
//> chỉ dùng key cố định là "global") tương ứng với cả một DANH SÁCH
//> tin nhắn, để lưu lịch sử chat.
std::mutex historyMutex;

const std::string USERS_FILE = "users.dat";
//> const string: hằng số chuỗi, không thể bị gán lại giá trị sau khi khởi tạo.

constexpr std::size_t BUFFER_SIZE = 4096;
constexpr std::size_t MAX_HISTORY = 100;
constexpr unsigned short SERVER_PORT = 8080;
constexpr int LISTEN_BACKLOG = 10;
//> HIỆN ĐẠI HÓA: gom các "con số ma" (magic number) nằm rải rác trong code
//> thành hằng số có TÊN. Trước đây số 4096, 100, 8080, 10 được viết thẳng
//> vào giữa các hàm — muốn đổi cổng phải đi tìm từng chỗ, và người đọc phải
//> đoán xem con số đó nghĩa là gì. Đặt tên giúp code tự giải thích và chỉ
//> cần sửa một nơi duy nhất.
// ------------------------------------------------------------

#ifdef WINDOWS_BUILD
bool initWinsock() {
    //> Hàm trả về kiểu bool: true nếu khởi tạo Winsock thành công, false nếu lỗi.
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << '\n';
        return false;
    }
    return true;
}

void cleanupWinsock() {
    WSACleanup();
}
#endif

// Cross-platform socket close helper
inline void closeSocket(SOCKET s) {
    //> Từ khóa "inline" GỢI Ý cho trình biên dịch chèn thẳng nội dung
    //> hàm vào nơi gọi thay vì tạo lệnh gọi hàm riêng — thường dùng cho
    //> các hàm ngắn, được gọi nhiều nơi, giúp giảm chi phí gọi hàm.
    //> (Đây cũng là hàm thay thế an toàn cho việc #define close ở trên.)
#if defined(WINDOWS_BUILD) || defined(_WIN32) || defined(_WIN64)
    closesocket(s);
#else
    close(s);
    //> "::close" — dấu "::" đứng trước tên hàm mà KHÔNG có gì phía trước
    //> nghĩa là "hàm close() ở phạm vi toàn cục (global namespace)",
    //> để chắc chắn gọi đúng hàm close() của hệ thống (POSIX), không bị
    //> nhầm với một hàm/biến "close" nào khác cùng tên có thể tồn tại.
#endif
}

//> HIỆN ĐẠI HÓA: hàm mới, thay thế cho inet_ntoa() đã lỗi thời.
std::string ipToString(const sockaddr_in& addr) {
    //> inet_ntoa() (dùng ở bản cũ) có 2 vấn đề nghiêm trọng: nó trả về con
    //> trỏ tới một vùng đệm TĨNH DÙNG CHUNG bên trong thư viện, nên (1) KHÔNG
    //> AN TOÀN LUỒNG — server này chạy mỗi client một luồng, hai luồng gọi
    //> cùng lúc sẽ ghi đè kết quả của nhau; và (2) nó chỉ hỗ trợ IPv4.
    //> inet_ntop() ghi kết quả vào vùng đệm do CHÍNH TA cung cấp nên an toàn
    //> luồng, và hỗ trợ cả IPv6 nếu sau này cần mở rộng.
    std::array<char, INET_ADDRSTRLEN> ip{};
    //> INET_ADDRSTRLEN là hằng số có sẵn = độ dài tối đa của một chuỗi địa
    //> chỉ IPv4 (kể cả ký tự kết thúc), nên vùng đệm chắc chắn đủ chỗ.
    if (inet_ntop(AF_INET, &addr.sin_addr, ip.data(), ip.size()) == nullptr) {
        return "unknown";
    }
    return ip.data();
}

// Simple hash function (use a proper library like bcrypt in production)
std::string hashPassword(const std::string& password) {
    //> Ghi chú ngay trong code: đây chỉ là băm đơn giản để MINH HỌA,
    //> KHÔNG an toàn cho sản phẩm thật (nên dùng thư viện chuyên dụng
    //> như bcrypt/argon2 để băm mật khẩu trong thực tế).
    std::hash<std::string> hasher;
    //> std::hash<string> là một "functor" (đối tượng hàm) có sẵn trong
    //> thư viện chuẩn <functional>: gọi hasher(chuỗi) sẽ trả về một số
    //> băm (kiểu size_t) đại diện cho chuỗi đó.
    const std::size_t hashValue = hasher(password + "SALT_2024");
    //> Nối thêm một chuỗi "muối" (salt) cố định vào trước khi băm, để
    //> tăng độ khó đoán so với băm mật khẩu trần trụi.
    return std::to_string(hashValue);
    //> HIỆN ĐẠI HÓA: std::to_string() thay cho việc tạo hẳn một
    //> stringstream rồi "đổ" số vào để lấy chuỗi ra. to_string() có từ
    //> C++11, ngắn gọn và nhanh hơn (không phải dựng cả một đối tượng
    //> luồng chỉ để đổi một con số thành chuỗi).
}

void loadUsers() {
    std::lock_guard<std::mutex> lock(usersMutex);
    //> Khóa ngay từ đầu hàm: nhờ RAII, mutex tự mở khóa khi hàm kết thúc
    //> (dù kết thúc bằng return ở đâu) — đã giải thích chi tiết ở file client.
    std::ifstream file(USERS_FILE);
    //> ifstream ("input file stream") mở file để ĐỌC. Constructor này vừa
    //> tạo đối tượng file vừa thử mở file USERS_FILE luôn.
    if (!file.is_open()) {
        //> is_open() kiểm tra file có mở thành công không (ví dụ file chưa
        //> tồn tại ở lần chạy đầu tiên thì mở sẽ thất bại).
        std::cout << "[*] No existing users file found. Starting fresh.\n";
        return;
    }

    std::string username;
    std::string passwordHash;
    while (file >> username >> passwordHash) {
        //> Toán tử ">>" trên một file stream đọc từng "từ" (token, ngăn
        //> cách bởi khoảng trắng/xuống dòng) — đọc lần lượt username rồi
        //> passwordHash. Biểu thức "file >> a >> b" trả về chính đối tượng
        //> file, và file tự chuyển thành "false" khi đọc tới hết dữ liệu,
        //> nên vòng while này tự dừng khi đọc hết file.
        users[username] = {username, passwordHash};
        //> {username, passwordHash} là "list initialization" — cách viết
        //> gọn để tạo một User mới với 2 trường được gán theo đúng thứ tự
        //> khai báo trong struct, rồi gán vào map users tại key username.
    }
    //> HIỆN ĐẠI HÓA: đã bỏ lời gọi file.close() thủ công. std::ifstream
    //> tuân theo RAII: destructor của nó TỰ ĐỘNG đóng file khi biến "file"
    //> ra khỏi phạm vi hàm — kể cả khi hàm thoát giữa chừng vì một ngoại lệ
    //> (exception), trường hợp mà close() thủ công sẽ bị bỏ qua.
    std::cout << "[*] Loaded " << users.size() << " users from database.\n";
}

void saveUser(const std::string& username, const std::string& passwordHash) {
    std::lock_guard<std::mutex> lock(usersMutex);
    std::ofstream file(USERS_FILE, std::ios::app);
    //> ofstream ("output file stream") mở file để GHI.
    //> ios::app ("append") nghĩa là ghi THÊM VÀO CUỐI file (không xóa nội
    //> dung cũ) — khác với chế độ mặc định sẽ ghi đè toàn bộ file.
    if (file.is_open()) {
        file << username << ' ' << passwordHash << '\n';
        //> HIỆN ĐẠI HÓA: dùng '\n' thay cho std::endl. endl vừa xuống dòng
        //> vừa ÉP GHI ngay xuống đĩa (flush) sau MỖI dòng — chậm không cần
        //> thiết. Dữ liệu vẫn được ghi đầy đủ khi file đóng lại.
    }
}

bool userExists(const std::string& username) {
    std::lock_guard<std::mutex> lock(usersMutex);
    return users.find(username) != users.end();
    //> Dùng find() để tương thích với C++17; map::contains() chỉ có từ C++20.
}

bool registerUser(const std::string& username, const std::string& passwordHash) {
    //> BUG ĐÃ SỬA: hàm này gộp "kiểm tra tồn tại" + "thêm vào map" thành
    //> MỘT thao tác nguyên tử (atomic), cùng giữ MỘT lock usersMutex từ
    //> đầu tới cuối. Code gốc gọi userExists() (tự khóa rồi tự mở khóa)
    //> XONG rồi mới gán trực tiếp "users[user] = ..." ở NGOÀI, không giữ
    //> khóa nào — giữa 2 bước đó có một khoảng hở (race window): nếu 2
    //> luồng cùng đăng ký MỘT username gần như đồng thời, cả hai có thể
    //> cùng thấy userExists() trả về false rồi cùng ghi đè lên nhau, hoặc
    //> tệ hơn, một luồng đọc/ghi map trong khi luồng khác đang sửa cấu
    //> trúc nội bộ của nó (data race — hành vi không xác định, khi test
    //> với nhiều client đăng ký đồng thời đã thấy một số kết nối bị treo
    //> nhiều giây). Gộp lại và giữ khóa xuyên suốt sẽ loại bỏ hoàn toàn
    //> khoảng hở đó.
    std::lock_guard<std::mutex> lock(usersMutex);
    const auto [it, inserted] = users.try_emplace(username, User{username, passwordHash});
    //> HIỆN ĐẠI HÓA: try_emplace() (C++17) làm đúng một việc "chèn nếu key
    //> chưa có" trong MỘT lần tra cứu duy nhất, thay vì tra cứu 2 lần
    //> (find() rồi lại users[key] = ...). Nó trả về một std::pair, và cú
    //> pháp "auto [it, inserted] = ..." là "structured binding" (C++17):
    //> tách thẳng 2 thành phần của pair ra 2 biến có tên rõ nghĩa, thay vì
    //> phải viết result.first / result.second khó đọc.
    (void)it;
    //> (void)it — nói rõ với trình biên dịch "tôi cố ý không dùng biến it",
    //> để tránh cảnh báo "biến không được sử dụng".
    return inserted;
    //> inserted == false nghĩa là key đã tồn tại từ trước → đăng ký thất bại.
}

bool verifyPassword(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(usersMutex);
    const auto it = users.find(username);
    if (it == users.end()) {
        return false;
    }
    return it->second.passwordHash == hashPassword(password);
    //> HIỆN ĐẠI HÓA: tái sử dụng iterator "it" đã tìm được ở trên, thay vì
    //> gọi users[username] một lần nữa. Ngoài chuyện đỡ phải tra cứu 2 lần,
    //> đây còn là một sửa lỗi tiềm ẩn: toán tử [] của map sẽ TỰ ĐỘNG TẠO MỚI
    //> một phần tử rỗng nếu key không tồn tại — tức là nó có thể ÂM THẦM
    //> THÊM user rỗng vào map. it->second truy cập thẳng giá trị đã tìm
    //> thấy nên không bao giờ làm thay đổi map.
}

void broadcastMessage(const std::string& message, SOCKET senderSocket) {
    //> "Broadcast" = gửi cùng một tin nhắn tới NHIỀU client cùng lúc.
    std::lock_guard<std::mutex> lock(clientsMutex);
    const std::string framed = message + "\n";
    //> BUG ĐÃ SỬA: thêm '\n' vào cuối mỗi tin nhắn làm RANH GIỚI. TCP là
    //> một luồng byte liên tục — nếu server gọi send() nhiều lần liên
    //> tiếp (ví dụ 2 broadcastMessage() gần nhau), phía client có thể
    //> nhận được tất cả trong MỘT lần recv() duy nhất, dính liền không
    //> phân biệt được đâu là tin nào (đã tái hiện được lỗi này khi test).
    //> Chèn '\n' cố định cho phép phía client (xem receiveMessages() bên
    //> client) tự tách lại đúng từng tin nhắn dù chúng đến gộp hay bị
    //> chia nhỏ qua nhiều lần recv().
    for (const auto& client : clients) {
        //> "range-based for loop" (vòng lặp theo phạm vi) — cú pháp hiện
        //> đại của C++11 trở lên, duyệt qua TỪNG phần tử của "clients"
        //> mà không cần quản lý chỉ số (index) thủ công như vòng for cũ.
        //> "auto" nhờ trình biên dịch TỰ SUY RA kiểu dữ liệu (ở đây là Client);
        //> "const auto&" nghĩa là lấy THAM CHIẾU chỉ-đọc tới từng phần tử,
        //> tránh sao chép (copy) không cần thiết mỗi vòng lặp.
        if (client.socket != senderSocket && client.authenticated) {
            //> Không gửi lại cho chính người gửi (senderSocket), và chỉ
            //> gửi cho các client ĐÃ đăng nhập xong.
            send(client.socket, framed.c_str(), static_cast<int>(framed.length()), 0);
        }
    }
}

void sendToClient(SOCKET socket, const std::string& message) {
    const std::string framed = message + "\n";
    //> BUG ĐÃ SỬA: cùng lý do như broadcastMessage() — thêm '\n' làm ranh
    //> giới tin nhắn để phía client tách đúng từng tin, tránh bị gộp dính
    //> liền (xem chú thích chi tiết ở broadcastMessage() phía trên).
    send(socket, framed.c_str(), static_cast<int>(framed.length()), 0);
    //> HIỆN ĐẠI HÓA: static_cast<int>(...) thay cho ép kiểu kiểu C "(int)".
    //> Ép kiểu kiểu C rất "mạnh tay": nó sẽ lặng lẽ thử mọi cách kể cả
    //> những cách nguy hiểm (bỏ const, diễn giải lại con trỏ sang kiểu
    //> hoàn toàn khác). static_cast chỉ cho phép các chuyển đổi hợp lý và
    //> sẽ BÁO LỖI nếu ta viết sai — đồng thời dễ tìm kiếm trong code.
}

std::string getCurrentTime() {
    const std::time_t now = std::time(nullptr);
    //> time(nullptr) trả về thời điểm hiện tại dưới dạng "Unix timestamp"
    //> (số giây kể từ 1/1/1970), kiểu time_t.
    //> HIỆN ĐẠI HÓA: nullptr thay cho số 0. nullptr (C++11) là một giá trị
    //> con trỏ rỗng THẬT SỰ, có kiểu riêng, không thể bị nhầm lẫn với số
    //> nguyên 0 khi trình biên dịch chọn hàm nạp chồng (overload).
    std::tm timeInfo{};
#if defined(WINDOWS_BUILD) || defined(_WIN32)
    localtime_s(&timeInfo, &now);
#else
    localtime_r(&now, &timeInfo);
#endif
    //> HIỆN ĐẠI HÓA: localtime() (bản cũ) trả về con trỏ tới một struct tm
    //> TĨNH DÙNG CHUNG toàn chương trình — y hệt vấn đề của inet_ntoa:
    //> KHÔNG AN TOÀN LUỒNG. Server này có nhiều luồng cùng gọi getCurrentTime()
    //> mỗi khi có tin nhắn, hai luồng gọi cùng lúc có thể nhận về giờ sai.
    //> localtime_r (POSIX) / localtime_s (Windows) ghi kết quả vào biến
    //> timeInfo của riêng ta nên hoàn toàn an toàn.
    std::ostringstream out;
    out << std::put_time(&timeInfo, "%H:%M:%S");
    //> HIỆN ĐẠI HÓA: std::put_time thay cho strftime() + mảng char thô.
    //> Cùng cú pháp định dạng ("%H:%M:%S" = giờ:phút:giây, 24 giờ) nhưng
    //> kết quả đi thẳng vào một luồng chuỗi C++, không cần tự khai báo
    //> mảng char và tự lo chuyện tràn bộ đệm.
    return out.str();
}

// ============================================================
//> HÀM XỬ LÝ MỘT CLIENT — mỗi client kết nối sẽ chạy hàm này trên
//> MỘT LUỒNG RIÊNG (xem thread clientThread trong main() bên dưới),
//> nhờ vậy server phục vụ được nhiều người dùng cùng lúc.
// ============================================================
void handleClient(SOCKET clientSocket, sockaddr_in clientAddr) {
    //> Hàm nhận tham số theo GIÁ TRỊ (không phải tham chiếu), nghĩa là
    //> clientSocket và clientAddr được SAO CHÉP riêng cho luồng này —
    //> an toàn vì mỗi luồng có bản sao độc lập, không bị luồng khác đụng vào.
    std::array<char, BUFFER_SIZE> buffer{};
    //> HIỆN ĐẠI HÓA: std::array thay cho mảng C-style "char buffer[4096]".
    //> std::array biết kích thước của chính nó (buffer.size()) nên không
    //> thể truyền nhầm độ dài, không bị "phân rã" thành con trỏ trần khi
    //> truyền vào hàm, và "{}" ở cuối tự động khởi tạo toàn bộ về 0 ngay
    //> lúc khai báo (thay cho lần memset đầu tiên).
    std::string username;
    const std::string ipAddr = ipToString(clientAddr);
    //> Dùng hàm ipToString() an toàn luồng ở trên thay cho inet_ntoa().
    bool authenticated = false;
    //> Biến "authenticated" ở đây là biến CỤC BỘ (local) riêng của luồng
    //> này — khác với struct Client.authenticated dùng để lưu trạng thái
    //> chia sẻ trong danh sách "clients" toàn cục.

    // Send authentication prompt
    sendToClient(clientSocket, "[SYSTEM] Welcome! Commands: /login username password OR /register username password");

    // Authentication loop
    while (!authenticated) {
        buffer.fill('\0');
        //> HIỆN ĐẠI HÓA: buffer.fill('\0') thay cho memset(buffer, 0, sizeof(buffer)).
        //> Cùng tác dụng "xóa sạch vùng đệm", nhưng là hàm thành viên của
        //> std::array nên không thể truyền nhầm kích thước — memset với
        //> sizeof() sai là một lỗi kinh điển khi làm việc với con trỏ.
        const int bytesReceived = recv(clientSocket, buffer.data(), static_cast<int>(buffer.size()), 0);
        if (bytesReceived <= 0) {
            //> Client ngắt kết nối trước khi đăng nhập xong.
            closeSocket(clientSocket);
            return;
            //> return (không giá trị, vì hàm này trả về void) — kết thúc
            //> hàm handleClient ngay lập tức, luồng xử lý client này dừng.
        }

        const std::string command(buffer.data(), static_cast<std::size_t>(bytesReceived));
        //> HIỆN ĐẠI HÓA + AN TOÀN: tạo chuỗi với ĐỘ DÀI TƯỜNG MINH thay vì
        //> string(buffer) (vốn đọc tới ký tự NULL đầu tiên). Nếu một lần
        //> recv() lấp đầy trọn vùng đệm mà không còn byte 0 nào phía sau,
        //> cách cũ sẽ đọc TRÀN ra ngoài mảng — hành vi không xác định.
        std::istringstream ss(command);
        //> HIỆN ĐẠI HÓA: istringstream (chỉ-đọc) thay cho stringstream
        //> (đọc-ghi). Ở đây ta chỉ tách chuỗi ra để đọc, không ghi gì vào,
        //> nên dùng đúng loại luồng cần thiết giúp thể hiện rõ ý định.
        std::string action;
        std::string user;
        std::string pass;
        ss >> action >> user >> pass;
        //> Đọc lần lượt 3 "từ" cách nhau bởi khoảng trắng từ chuỗi lệnh,
        //> ví dụ command = "/login alice matkhau123" thì sau dòng này:
        //> action = "/login", user = "alice", pass = "matkhau123".
        //> Nếu chuỗi có ít từ hơn 3, các biến còn thiếu sẽ giữ chuỗi rỗng.

        if (action == "/register") {
            if (user.empty() || pass.empty()) {
                sendToClient(clientSocket, "[ERROR] Usage: /register username password");
                continue;
                //> continue: bỏ qua phần code còn lại của vòng lặp, quay
                //> lại đầu vòng while để nhận lệnh tiếp theo từ client.
            }

            const std::string hashedPass = hashPassword(pass);
            if (!registerUser(user, hashedPass)) {
                //> BUG ĐÃ SỬA: trước đây code gọi userExists(user) riêng rồi
                //> mới gán "users[user] = ..." riêng — hai bước tách rời,
                //> không nguyên tử (xem chú thích chi tiết ở hàm
                //> registerUser() phía trên). Giờ chỉ cần MỘT lệnh gọi
                //> registerUser(): nó tự kiểm tra + thêm vào map trong
                //> cùng một lock, trả về false nếu username đã tồn tại.
                sendToClient(clientSocket, "[ERROR] Username already exists!");
                continue;
            }
            saveUser(user, hashedPass);
            sendToClient(clientSocket, "[SUCCESS] Registration successful! Now use /login username password");
            std::cout << "[+] New user registered: " << user << '\n';

        } else if (action == "/login") {
            if (user.empty() || pass.empty()) {
                sendToClient(clientSocket, "[ERROR] Usage: /login username password");
                continue;
            }

            if (!userExists(user)) {
                sendToClient(clientSocket, "[ERROR] Username not found! Use /register first.");
                continue;
            }

            if (!verifyPassword(user, pass)) {
                sendToClient(clientSocket, "[ERROR] Invalid password!");
                continue;
            }

            // Check if user already logged in
            {
                //> Cặp dấu ngoặc nhọn {} "trần" (không đi kèm if/for/hàm)
                //> tạo ra một PHẠM VI (scope) con nhân tạo. Mục đích ở đây:
                //> biến "lock" bên trong sẽ TỰ ĐỘNG unlock ngay khi ra khỏi
                //> cặp {} này — tức là mutex chỉ bị khóa trong đúng đoạn
                //> kiểm tra "đã đăng nhập chưa", không giữ khóa lâu hơn mức cần thiết.
                std::lock_guard<std::mutex> lock(clientsMutex);
                const bool alreadyLoggedIn = std::any_of(
                    clients.begin(), clients.end(),
                    [&user](const Client& c) { return c.username == user && c.authenticated; });
                //> HIỆN ĐẠI HÓA: std::any_of() thay cho vòng for thủ công có
                //> biến cờ (flag) alreadyLoggedIn và lệnh break. Thuật toán
                //> chuẩn nói thẳng ý định — "có PHẦN TỬ NÀO thỏa điều kiện
                //> không?" — thay vì bắt người đọc phải lần theo vòng lặp để
                //> suy ra. Nó cũng tự dừng ngay khi tìm thấy, y như break.
                if (alreadyLoggedIn) {
                    sendToClient(clientSocket, "[ERROR] User already logged in!");
                    continue;
                    //> Lưu ý: continue ở đây thoát khỏi vòng while bên ngoài
                    //> (vòng đăng nhập) — "continue" tác động tới vòng lặp
                    //> gần nhất bao quanh nó, ở đây chính là while(!authenticated).
                }
            }
            //> Ra khỏi cặp {} phía trên → "lock" đã tự động unlock.

            username = user;
            authenticated = true;
            sendToClient(clientSocket, "[SUCCESS] Login successful! Welcome to the chat!");

        } else {
            sendToClient(clientSocket, "[ERROR] Unknown command. Use /login or /register");
        }
    }

    // Add authenticated client to list
    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.push_back({clientSocket, username, ipAddr, true});
        //> vector::push_back(...) thêm một phần tử mới vào CUỐI vector.
        //> {clientSocket, username, ipAddr, true} tạo trực tiếp một Client
        //> mới theo đúng thứ tự 4 trường của struct, không cần viết tên struct.
        std::cout << "\n[+] " << username << " logged in from " << ipAddr << '\n';
        std::cout << "[*] Active users: " << clients.size() << '\n';
    }

    // Notify all clients
    const std::string joinMsg = "[SYSTEM] " + username + " joined the chat";
    broadcastMessage(joinMsg, clientSocket);

    // Send welcome message
    sendToClient(clientSocket, "[SYSTEM] Type /help for commands");

    // Main message loop
    while (true) {
        buffer.fill('\0');
        const int bytesReceived = recv(clientSocket, buffer.data(), static_cast<int>(buffer.size()), 0);

        if (bytesReceived <= 0) {
            break;
        }

        const std::string message(buffer.data(), static_cast<std::size_t>(bytesReceived));

        if (message == "/quit") {
            break;
        } else if (message == "/users") {
            std::lock_guard<std::mutex> lock(clientsMutex);
            std::string userList = "\n[SYSTEM] === Active Users (" + std::to_string(clients.size()) + ") ===\n";
            //> HIỆN ĐẠI HÓA: std::to_string() thay cho việc dựng một
            //> stringstream chỉ để đổi clients.size() thành chuỗi.
            for (const auto& c : clients) {
                if (c.authenticated) {
                    userList += "[SYSTEM] - " + c.username;
                    if (c.socket == clientSocket) userList += " (you)";
                    userList += "\n";
                }
            }
            sendToClient(clientSocket, userList);
        } else if (message == "/help") {
            std::string help = "\n[SYSTEM] === Commands ===\n";
            help += "[SYSTEM] /users - List all users\n";
            help += "[SYSTEM] /help - Show this help\n";
            help += "[SYSTEM] /quit - Leave chat\n";
            sendToClient(clientSocket, help);
        } else {
            const std::string timestamp = getCurrentTime();
            const std::string fullMessage = "[" + timestamp + "] " + username + ": " + message;

            // Store in history
            {
                std::lock_guard<std::mutex> lock(historyMutex);
                auto& history = messageHistory["global"];
                //> HIỆN ĐẠI HÓA: lấy MỘT tham chiếu "history" tới vector rồi
                //> dùng lại, thay vì viết messageHistory["global"] lặp đi lặp
                //> lại 4 lần (mỗi lần đều phải tra cứu lại trong map).
                //> messageHistory["global"] — nếu key "global" CHƯA tồn tại
                //> trong map, toán tử [] sẽ TỰ ĐỘNG tạo mới một vector<string>
                //> rỗng cho key đó rồi trả về tham chiếu tới nó.
                history.push_back(fullMessage);
                if (history.size() > MAX_HISTORY) {
                    history.erase(history.begin());
                    //> .begin() trả về iterator trỏ tới PHẦN TỬ ĐẦU TIÊN;
                    //> .erase(iterator) xóa đúng phần tử đó — tức là khi
                    //> lịch sử vượt quá 100 tin, xóa bớt tin CŨ NHẤT
                    //> (giữ vector luôn tối đa 100 tin nhắn gần nhất).
                }
            }

            // Broadcast to all authenticated clients
            broadcastMessage(fullMessage, INVALID_SOCKET);
            //> HIỆN ĐẠI HÓA: dùng hằng số INVALID_SOCKET thay cho "(SOCKET)-1".
            //> Cả hai đều là giá trị "không trùng với socket thật nào cả" để
            //> broadcastMessage gửi cho TẤT CẢ client (không loại trừ ai), vì
            //> điều kiện "client.socket != senderSocket" sẽ luôn đúng. Nhưng
            //> INVALID_SOCKET nói rõ ý định và đúng trên cả 2 hệ điều hành,
            //> còn ép kiểu (SOCKET)-1 trên Windows (SOCKET là số KHÔNG DẤU)
            //> là một phép ép kiểu dễ gây hiểu nhầm.

            // Log to server console
            std::cout << fullMessage << '\n';
        }
    }

    // Client disconnected
    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.erase(
            std::remove_if(clients.begin(), clients.end(),
                           [clientSocket](const Client& c) { return c.socket == clientSocket; }),
            clients.end());
        //> HIỆN ĐẠI HÓA: std::erase_if (C++20) thay cho "kỹ thuật erase–remove"
        //> cũ: clients.erase(std::remove_if(begin, end, điều_kiện), end()).
        //> Cách cũ phải viết 2 hàm lồng nhau và rất dễ viết sai (quên tham số
        //> end() thứ hai là lỗi kinh điển, khiến chỉ xóa đúng 1 phần tử).
        //> std::erase_if làm đúng việc đó trong một lời gọi, đọc phát hiểu ngay:
        //> "xóa khỏi clients những phần tử thỏa điều kiện".
        //>
        //> [clientSocket](const Client& c) { return c.socket == clientSocket; }
        //> là một LAMBDA — hàm không tên được định nghĩa ngay tại chỗ dùng.
        //> Cấu trúc lambda: [danh_sách_bắt_biến](tham_số) { thân_hàm }
        //>  - "[clientSocket]" nghĩa là lambda này "bắt" (capture) biến
        //>    clientSocket từ phạm vi bên ngoài, SAO CHÉP giá trị của nó
        //>    vào bên trong lambda để dùng được ở thân hàm.
        //>  - "(const Client& c)" là tham số của lambda, giống tham số hàm bình thường.
        //>  - Thân hàm trả về true nếu socket của client c KHỚP với
        //>    clientSocket vừa ngắt kết nối — tức "đánh dấu client này để xóa".
        std::cout << "\n[-] " << username << " left the chat\n";
        std::cout << "[*] Active users: " << clients.size() << '\n';
    }

    const std::string leaveMsg = "[SYSTEM] " + username + " left the chat";
    broadcastMessage(leaveMsg, INVALID_SOCKET);

    closeSocket(clientSocket);
}

// ============================================================
//> HÀM MAIN — khởi động server, mở cổng lắng nghe, và vòng lặp
//> chấp nhận kết nối mới từ các client.
// ============================================================
int main() {
    std::cout << "+========================================+\n";
    std::cout << "|   C++ Messenger Server v2.0            |\n";
    std::cout << "|   With User Authentication             |\n";
#ifdef WINDOWS_BUILD
    std::cout << "|        Windows Build                   |\n";
#else
    std::cout << "|        Linux/Unix Build                |\n";
#endif
    std::cout << "+========================================+\n\n";

    // Load existing users
    loadUsers();

#ifdef WINDOWS_BUILD
    if (!initWinsock()) {
        return 1;
    }
#endif

    const SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    //> Tạo socket TCP/IPv4 — giống hệt cách client tạo socket, nhưng
    //> socket này của SERVER sẽ dùng để LẮNG NGHE kết nối tới, chứ
    //> không "connect" đi đâu cả.
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket!\n";
#ifdef WINDOWS_BUILD
        cleanupWinsock();
#endif
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
    //> setsockopt() đặt một "tùy chọn" cho socket. Ở đây bật SO_REUSEADDR
    //> (Socket Option: Reuse Address) — cho phép server khởi động lại và
    //> bind lại đúng cổng 8080 NGAY LẬP TỨC, thay vì phải đợi hệ điều
    //> hành giải phóng cổng đó (thường mất khoảng 1–2 phút sau khi server
    //> cũ tắt, do cơ chế TCP TIME_WAIT).
    //> HIỆN ĐẠI HÓA: reinterpret_cast<const char*> thay cho "(char*)".
    //> setsockopt() là hàm C tổng quát nên nhận con trỏ kiểu char*, buộc
    //> ta phải "diễn giải lại" con trỏ int* thành char* — đúng bản chất là
    //> một reinterpret_cast. Viết rõ ra như vậy giúp người đọc thấy ngay
    //> đây là chỗ ép kiểu "mạnh", cần để mắt tới, chứ không lẫn vào các
    //> phép ép kiểu vô hại khác.

    sockaddr_in serverAddress{};
    //> HIỆN ĐẠI HÓA: "{}" (value initialization) tự động đặt TOÀN BỘ struct
    //> về 0 ngay lúc khai báo. Bản cũ KHÔNG hề xóa struct này trước khi
    //> điền — các trường không được gán (ví dụ sin_zero) sẽ chứa giá trị
    //> rác từ ngăn xếp, một lỗi tiềm ẩn thật sự khi làm việc với struct
    //> của hệ thống.
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    //> INADDR_ANY là hằng số đặc biệt nghĩa là "lắng nghe trên MỌI địa chỉ
    //> IP của máy này" (ví dụ máy có cả IP mạng LAN lẫn 127.0.0.1, server
    //> sẽ nhận kết nối tới từ cả hai) — khác với client trước đó chỉ định
    //> đích danh một địa chỉ cụ thể ("127.0.0.1") để KẾT NỐI TỚI.

    if (::bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        //> bind() "gắn" socket vào một địa chỉ IP + cổng cụ thể trên máy hiện
        //> tại — bắt buộc phải làm TRƯỚC KHI listen(), để hệ điều hành biết
        //> gửi các kết nối tới cổng 8080 vào đúng socket này.
        //> "::bind" — dùng "::" để gọi hàm bind() toàn cục của hệ thống, tránh
        //> nhầm lẫn nếu có hàm/thành viên nào khác tên "bind" trong phạm vi này
        //> (ví dụ std::bind trong <functional>, vốn là một hàm hoàn toàn khác).
        std::cerr << "Error binding socket!\n";
        closeSocket(serverSocket);
#ifdef WINDOWS_BUILD
        cleanupWinsock();
#endif
        return 1;
    }

    if (listen(serverSocket, LISTEN_BACKLOG) == SOCKET_ERROR) {
        //> listen(socket, backlog) chuyển socket sang chế độ "lắng nghe"
        //> kết nối tới. Tham số thứ hai (10) là "backlog" — số lượng kết
        //> nối tối đa được xếp hàng chờ xử lý nếu server đang bận, chưa
        //> kịp accept() kịp; kết nối vượt quá số này có thể bị từ chối.
        std::cerr << "Error listening on socket!\n";
        closeSocket(serverSocket);
#ifdef WINDOWS_BUILD
        cleanupWinsock();
#endif
        return 1;
    }

    std::cout << "[*] Server started on port " << SERVER_PORT << '\n';
    std::cout << "[*] Waiting for connections...\n" << std::endl;
    //> Ở đây vẫn dùng std::endl một cách CÓ CHỦ Ý: nó ép đẩy dữ liệu ra
    //> màn hình ngay (flush), để dòng thông báo "đang chờ kết nối" hiện
    //> lên lập tức thay vì nằm chờ trong bộ đệm trước khi server chặn ở
    //> accept(). Đây đúng là trường hợp endl có ích — khác với việc rải
    //> endl ở mọi dòng xuất chỉ vì thói quen.

    while (true) {
        //> Vòng lặp VÔ HẠN (while (true)) — server chạy mãi, liên tục chờ
        //> và chấp nhận kết nối mới, cho tới khi bị dừng thủ công (ví dụ Ctrl+C).
        sockaddr_in clientAddr{};
        socklen_t clientAddrLen = sizeof(clientAddr);
        //> accept() cần biết TRƯỚC kích thước của struct clientAddr (qua
        //> con trỏ &clientAddrLen) để biết ghi tối đa bao nhiêu byte vào đó.

        const SOCKET clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
        //> accept() CHỜ (blocking) cho tới khi có một client mới kết nối
        //> tới, sau đó tạo ra MỘT SOCKET MỚI riêng biệt (clientSocket) dùng
        //> để giao tiếp với đúng client đó — khác với serverSocket (vẫn
        //> tiếp tục chỉ dùng để lắng nghe các kết nối tiếp theo).
        //> Đồng thời, accept() điền thông tin địa chỉ của client (IP,
        //> port) vào clientAddr thông qua con trỏ &clientAddr.
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error accepting connection!\n";
            continue;
            //> Nếu accept() lỗi, bỏ qua và quay lại chờ kết nối tiếp theo,
            //> KHÔNG làm sập cả server.
        }

        std::thread(handleClient, clientSocket, clientAddr).detach();
        //> Tạo một luồng mới chạy hàm handleClient với 2 tham số truyền kèm
        //> (clientSocket, clientAddr) — cú pháp std::thread(tên_hàm,
        //> đối_số_1, đối_số_2, ...) tự động truyền các đối số này vào đúng
        //> vị trí tham số tương ứng của hàm.
        //> .detach() tách luồng ra chạy độc lập — server không cần "chờ"
        //> luồng này xử lý xong mới tiếp tục vòng lặp accept() client khác.
        //> HIỆN ĐẠI HÓA: tạo và detach ngay trên một dòng, thay vì đặt tên
        //> biến clientThread rồi mới gọi detach. Biến đó chỉ tồn tại đúng
        //> một dòng và không dùng lại, nên việc đặt tên chỉ gợi ý sai rằng
        //> ta còn quản lý luồng đó.
    }

    //> Phần dọn dẹp bên dưới KHÔNG BAO GIỜ được chạy tới, vì vòng while(true)
    //> phía trên là vô hạn và không có lệnh break nào thoát ra — server chỉ
    //> dừng khi bị buộc tắt từ bên ngoài (ví dụ Ctrl+C, hoặc kill process).
    //> Đây là điểm thường gặp trong các chương trình server đơn giản kiểu này.
}