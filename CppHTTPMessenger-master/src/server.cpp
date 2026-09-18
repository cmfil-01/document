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

#ifdef WINDOWS_BUILD
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
#include <algorithm>    //> std::remove_if, dùng ở phần xóa client rời phòng chat
#include <map>          //> std::map — cấu trúc "từ điển" (key → value), tự sắp xếp theo key
#include <ctime>        //> time(), strftime(), localtime() — lấy và định dạng thời gian
#include <sstream>      //> std::stringstream — đọc/ghi dữ liệu như một luồng, dùng để tách chuỗi
#include <fstream>      //> std::ifstream, std::ofstream — đọc/ghi file trên đĩa
#include <functional>   //> std::hash — công cụ băm (hash) dữ liệu có sẵn của thư viện chuẩn

// ------------------------------------------------------------
//> Giống file client: dùng biên dịch có điều kiện để cùng một file .cpp
//> chạy được trên cả Windows lẫn Linux (API socket của 2 hệ điều hành khác nhau).
#ifdef WINDOWS_BUILD
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
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
    #include <arpa/inet.h>   //> inet_ntoa() — chuyển địa chỉ IP dạng nhị phân sang chuỗi
    typedef int SOCKET;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    //> Linux không có sẵn hằng số SOCKET_ERROR như Windows (nhiều hàm
    //> socket trên Linux trả về -1 khi lỗi), nên định nghĩa thêm để
    //> code phía dưới dùng chung một tên gọi cho cả 2 hệ điều hành.
#endif
// ------------------------------------------------------------

using namespace std;

// ------------------------------------------------------------
//> ĐỊNH NGHĨA KIỂU DỮ LIỆU RIÊNG (struct)
//> struct trong C++ giống class nhưng mặc định các thành viên là "public".
//> Dùng để gom nhiều biến liên quan lại thành MỘT kiểu dữ liệu mới.
struct User {
    string username;
    string passwordHash;
    //> Không lưu mật khẩu gốc, chỉ lưu bản đã băm (hash) — xem hashPassword() bên dưới.
};

struct Client {
    SOCKET socket;        //> socket kết nối riêng của client này
    string username;
    string ipAddress;
    bool authenticated;   //> đã đăng nhập thành công hay chưa
};
// ------------------------------------------------------------

// ------------------------------------------------------------
//> DỮ LIỆU TOÀN CỤC — CHIA SẺ GIỮA MỌI LUỒNG XỬ LÝ CLIENT
vector<Client> clients;
//> vector<Client>: một "mảng động" chứa các struct Client — có thể
//> push_back() thêm phần tử, erase() xóa phần tử, tự động cấp phát lại
//> bộ nhớ khi cần, khác với mảng C-style có kích thước cố định.
mutex clientsMutex;
//> Vì NHIỀU LUỒNG (mỗi client một luồng riêng — xem handleClient) có thể
//> cùng lúc đọc/ghi "clients", cần khóa mutex này mỗi khi truy cập để
//> tránh 2 luồng sửa vector cùng lúc gây lỗi/crash (data race).
map<string, User> users;
//> map<string, User>: cấu trúc kiểu "từ điển" — tra cứu nhanh một User
//> bằng KEY là username (chuỗi). users["alice"] sẽ trả về (hoặc tạo mới)
//> phần tử User có key "alice".
mutex usersMutex;
map<string, vector<string>> messageHistory;
//> map mà GIÁ TRỊ (value) lại là một vector<string> — tức mỗi key (ở đây
//> chỉ dùng key cố định là "global") tương ứng với cả một DANH SÁCH
//> tin nhắn, để lưu lịch sử chat.
mutex historyMutex;

const string USERS_FILE = "users.dat";
//> const string: hằng số chuỗi, không thể bị gán lại giá trị sau khi khởi tạo.
// ------------------------------------------------------------

#ifdef WINDOWS_BUILD
bool initWinsock() {
    //> Hàm trả về kiểu bool: true nếu khởi tạo Winsock thành công, false nếu lỗi.
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cerr << "WSAStartup failed: " << result << endl;
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
#ifdef WINDOWS_BUILD
    closesocket(s);
#else
    ::close(s);
    //> "::close" — dấu "::" đứng trước tên hàm mà KHÔNG có gì phía trước
    //> nghĩa là "hàm close() ở phạm vi toàn cục (global namespace)",
    //> để chắc chắn gọi đúng hàm close() của hệ thống (POSIX), không bị
    //> nhầm với một hàm/biến "close" nào khác cùng tên có thể tồn tại.
#endif
}

// Simple hash function (use a proper library like bcrypt in production)
string hashPassword(const string& password) {
    //> Ghi chú ngay trong code: đây chỉ là băm đơn giản để MINH HỌA,
    //> KHÔNG an toàn cho sản phẩm thật (nên dùng thư viện chuyên dụng
    //> như bcrypt/argon2 để băm mật khẩu trong thực tế).
    hash<string> hasher;
    //> std::hash<string> là một "functor" (đối tượng hàm) có sẵn trong
    //> thư viện chuẩn <functional>: gọi hasher(chuỗi) sẽ trả về một số
    //> băm (kiểu size_t) đại diện cho chuỗi đó.
    size_t hashValue = hasher(password + "SALT_2024");
    //> Nối thêm một chuỗi "muối" (salt) cố định vào trước khi băm, để
    //> tăng độ khó đoán so với băm mật khẩu trần trụi.
    stringstream ss;
    //> stringstream cho phép "đổ" (<<) nhiều kiểu dữ liệu khác nhau vào
    //> rồi lấy ra dưới dạng một chuỗi string duy nhất — ở đây dùng để
    //> chuyển số hashValue (kiểu size_t) thành chuỗi văn bản.
    ss << hashValue;
    return ss.str();
    //> ss.str() lấy toàn bộ nội dung đã "đổ" vào stringstream dưới dạng std::string.
}

void loadUsers() {
    lock_guard<mutex> lock(usersMutex);
    //> Khóa ngay từ đầu hàm: nhờ RAII, mutex tự mở khóa khi hàm kết thúc
    //> (dù kết thúc bằng return ở đâu) — đã giải thích chi tiết ở file client.
    ifstream file(USERS_FILE);
    //> ifstream ("input file stream") mở file để ĐỌC. Constructor này vừa
    //> tạo đối tượng file vừa thử mở file USERS_FILE luôn.
    if (!file.is_open()) {
        //> is_open() kiểm tra file có mở thành công không (ví dụ file chưa
        //> tồn tại ở lần chạy đầu tiên thì mở sẽ thất bại).
        cout << "[*] No existing users file found. Starting fresh." << endl;
        return;
    }
    
    string username, passwordHash;
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
    file.close();
    cout << "[*] Loaded " << users.size() << " users from database." << endl;
}

void saveUser(const string& username, const string& passwordHash) {
    lock_guard<mutex> lock(usersMutex);
    ofstream file(USERS_FILE, ios::app);
    //> ofstream ("output file stream") mở file để GHI.
    //> ios::app ("append") nghĩa là ghi THÊM VÀO CUỐI file (không xóa nội
    //> dung cũ) — khác với chế độ mặc định sẽ ghi đè toàn bộ file.
    if (file.is_open()) {
        file << username << " " << passwordHash << endl;
        file.close();
    }
}

bool userExists(const string& username) {
    lock_guard<mutex> lock(usersMutex);
    return users.find(username) != users.end();
    //> map::find(key) trả về một "iterator" (con trỏ duyệt) trỏ tới phần
    //> tử có key đó nếu tìm thấy, hoặc bằng users.end() (vị trí "sau phần
    //> tử cuối cùng", không đại diện phần tử thật) nếu KHÔNG tìm thấy.
    //> So sánh "!= users.end()" tức là "có tồn tại user này trong map".
}

bool registerUser(const string& username, const string& passwordHash) {
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
    lock_guard<mutex> lock(usersMutex);
    if (users.find(username) != users.end()) {
        return false;
    }
    users[username] = {username, passwordHash};
    return true;
}

bool verifyPassword(const string& username, const string& password) {
    lock_guard<mutex> lock(usersMutex);
    if (users.find(username) == users.end()) {
        return false;
    }
    return users[username].passwordHash == hashPassword(password);
    //> users[username] — toán tử [] trên map: truy cập giá trị (User) ứng
    //> với key username, rồi lấy trường .passwordHash để so sánh với bản
    //> băm của mật khẩu vừa nhập.
}

void broadcastMessage(const string& message, SOCKET senderSocket) {
    //> "Broadcast" = gửi cùng một tin nhắn tới NHIỀU client cùng lúc.
    lock_guard<mutex> lock(clientsMutex);
    string framed = message + "\n";
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
            send(client.socket, framed.c_str(), (int)framed.length(), 0);
        }
    }
}

void sendToClient(SOCKET socket, const string& message) {
    string framed = message + "\n";
    //> BUG ĐÃ SỬA: cùng lý do như broadcastMessage() — thêm '\n' làm ranh
    //> giới tin nhắn để phía client tách đúng từng tin, tránh bị gộp dính
    //> liền (xem chú thích chi tiết ở broadcastMessage() phía trên).
    send(socket, framed.c_str(), (int)framed.length(), 0);
}

string getCurrentTime() {
    time_t now = time(0);
    //> time(0) trả về thời điểm hiện tại dưới dạng "Unix timestamp"
    //> (số giây kể từ 1/1/1970), kiểu time_t.
    char buf[80];
    strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&now));
    //> localtime(&now) chuyển timestamp thô thành cấu trúc "struct tm"
    //> (giờ/phút/giây/ngày/tháng/năm theo múi giờ máy đang chạy).
    //> strftime(buf, kích_thước_buf, định_dạng, thời_gian) định dạng thời
    //> gian đó thành chuỗi theo mẫu — "%H:%M:%S" nghĩa là giờ:phút:giây
    //> (24 giờ), rồi ghi kết quả vào mảng ký tự buf.
    return string(buf);
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
    char buffer[4096];
    string username;
    string ipAddr = inet_ntoa(clientAddr.sin_addr);
    //> inet_ntoa ("network to ASCII") chuyển địa chỉ IP dạng nhị phân
    //> (trong clientAddr.sin_addr) sang chuỗi con người đọc được, ví dụ "192.168.1.5".
    bool authenticated = false;
    //> Biến "authenticated" ở đây là biến CỤC BỘ (local) riêng của luồng
    //> này — khác với struct Client.authenticated dùng để lưu trạng thái
    //> chia sẻ trong danh sách "clients" toàn cục.
    
    // Send authentication prompt
    sendToClient(clientSocket, "[SYSTEM] Welcome! Commands: /login username password OR /register username password");
    
    // Authentication loop
    while (!authenticated) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            //> Client ngắt kết nối trước khi đăng nhập xong.
            closeSocket(clientSocket);
            return;
            //> return (không giá trị, vì hàm này trả về void) — kết thúc
            //> hàm handleClient ngay lập tức, luồng xử lý client này dừng.
        }
        
        string command(buffer);
        stringstream ss(command);
        //> Tạo stringstream TỪ một chuỗi có sẵn (command) — cho phép "đọc"
        //> chuỗi đó giống như đọc từ một file/luồng nhập liệu.
        string action, user, pass;
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
            
            string hashedPass = hashPassword(pass);
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
            cout << "[+] New user registered: " << user << endl;
            
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
                lock_guard<mutex> lock(clientsMutex);
                bool alreadyLoggedIn = false;
                for (const auto& client : clients) {
                    if (client.username == user && client.authenticated) {
                        alreadyLoggedIn = true;
                        break;
                    }
                }
                if (alreadyLoggedIn) {
                    sendToClient(clientSocket, "[ERROR] User already logged in!");
                    continue;
                    //> Lưu ý: continue ở đây thoát khỏi vòng while bên ngoài
                    //> (vòng đăng nhập), KHÔNG phải vòng for phía trên (vòng
                    //> for đã kết thúc bình thường trước đó) — vẫn nằm trong
                    //> khối {} của mutex nhưng "continue" tác động tới vòng
                    //> lặp gần nhất bao quanh nó, ở đây chính là while(!authenticated).
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
        lock_guard<mutex> lock(clientsMutex);
        clients.push_back({clientSocket, username, ipAddr, true});
        //> vector::push_back(...) thêm một phần tử mới vào CUỐI vector.
        //> {clientSocket, username, ipAddr, true} tạo trực tiếp một Client
        //> mới theo đúng thứ tự 4 trường của struct, không cần viết tên struct.
        cout << "\n[+] " << username << " logged in from " << ipAddr << endl;
        cout << "[*] Active users: " << clients.size() << endl;
    }
    
    // Notify all clients
    string joinMsg = "[SYSTEM] " + username + " joined the chat";
    broadcastMessage(joinMsg, clientSocket);
    
    // Send welcome message
    string welcome = "[SYSTEM] Type /help for commands";
    sendToClient(clientSocket, welcome);
    
    // Main message loop
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        
        if (bytesReceived <= 0) {
            break;
        }
        
        string message(buffer);
        
        if (message == "/quit") {
            break;
        } else if (message == "/users") {
            lock_guard<mutex> lock(clientsMutex);
            stringstream ss;
            ss << clients.size();
            string userList = "\n[SYSTEM] === Active Users (" + ss.str() + ") ===\n";
            for (const auto& c : clients) {
                if (c.authenticated) {
                    userList += "[SYSTEM] - " + c.username;
                    if (c.socket == clientSocket) userList += " (you)";
                    userList += "\n";
                }
            }
            sendToClient(clientSocket, userList);
        } else if (message == "/help") {
            string help = "\n[SYSTEM] === Commands ===\n";
            help += "[SYSTEM] /users - List all users\n";
            help += "[SYSTEM] /help - Show this help\n";
            help += "[SYSTEM] /quit - Leave chat\n";
            sendToClient(clientSocket, help);
        } else {
            string timestamp = getCurrentTime();
            string fullMessage = "[" + timestamp + "] " + username + ": " + message;
            
            // Store in history
            {
                lock_guard<mutex> lock(historyMutex);
                messageHistory["global"].push_back(fullMessage);
                //> messageHistory["global"] — nếu key "global" CHƯA tồn tại
                //> trong map, toán tử [] sẽ TỰ ĐỘNG tạo mới một vector<string>
                //> rỗng cho key đó rồi trả về tham chiếu tới nó; sau đó
                //> .push_back(...) thêm tin nhắn vào cuối vector đó.
                if (messageHistory["global"].size() > 100) {
                    messageHistory["global"].erase(messageHistory["global"].begin());
                    //> .begin() trả về iterator trỏ tới PHẦN TỬ ĐẦU TIÊN;
                    //> .erase(iterator) xóa đúng phần tử đó — tức là khi
                    //> lịch sử vượt quá 100 tin, xóa bớt tin CŨ NHẤT
                    //> (giữ vector luôn tối đa 100 tin nhắn gần nhất).
                }
            }
            
            // Broadcast to all authenticated clients
            broadcastMessage(fullMessage, (SOCKET)-1);
            //> (SOCKET)-1 — ép kiểu số -1 sang kiểu SOCKET, dùng làm giá trị
            //> "không trùng với socket thật nào cả", để broadcastMessage
            //> gửi tin nhắn này cho TẤT CẢ client (không loại trừ ai),
            //> vì điều kiện "client.socket != senderSocket" trong hàm đó
            //> sẽ luôn đúng với mọi client thật.
            
            // Log to server console
            cout << fullMessage << endl;
        }
    }
    
    // Client disconnected
    {
        lock_guard<mutex> lock(clientsMutex);
        clients.erase(
            remove_if(clients.begin(), clients.end(),
                [clientSocket](const Client& c) { return c.socket == clientSocket; }),
            clients.end()
        );
        //> Đây là "kỹ thuật erase–remove" kinh điển của C++ để xóa phần
        //> tử khỏi vector theo ĐIỀU KIỆN:
        //>  - std::remove_if(begin, end, điều_kiện) duyệt qua vector,
        //>    "dồn" các phần tử KHÔNG thỏa điều kiện lên đầu, rồi trả về
        //>    một iterator đánh dấu ranh giới — phần còn lại phía sau
        //>    (các phần tử thỏa điều kiện) coi như "rác", nhưng chưa thực
        //>    sự bị xóa khỏi vector (kích thước vector KHÔNG đổi).
        //>  - vector::erase(iterator_ranh_giới, end()) mới thực sự cắt bỏ
        //>    phần "rác" đó ra khỏi vector, thu nhỏ kích thước thật sự.
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
        cout << "\n[-] " << username << " left the chat" << endl;
        cout << "[*] Active users: " << clients.size() << endl;
    }
    
    string leaveMsg = "[SYSTEM] " + username + " left the chat";
    broadcastMessage(leaveMsg, (SOCKET)-1);
    
    closeSocket(clientSocket);
}

// ============================================================
//> HÀM MAIN — khởi động server, mở cổng lắng nghe, và vòng lặp
//> chấp nhận kết nối mới từ các client.
// ============================================================
int main() {
    cout << "+========================================+\n";
    cout << "|   C++ Messenger Server v2.0            |\n";
    cout << "|   With User Authentication             |\n";
#ifdef WINDOWS_BUILD
    cout << "|        Windows Build                   |\n";
#else
    cout << "|        Linux/Unix Build                |\n";
#endif
    cout << "+========================================+\n\n";
    
    // Load existing users
    loadUsers();
    
#ifdef WINDOWS_BUILD
    if (!initWinsock()) {
        return 1;
    }
#endif
    
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    //> Tạo socket TCP/IPv4 — giống hệt cách client tạo socket, nhưng
    //> socket này của SERVER sẽ dùng để LẮNG NGHE kết nối tới, chứ
    //> không "connect" đi đâu cả.
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Error creating socket!" << endl;
#ifdef WINDOWS_BUILD
        cleanupWinsock();
#endif
        return 1;
    }
    
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    //> setsockopt() đặt một "tùy chọn" cho socket. Ở đây bật SO_REUSEADDR
    //> (Socket Option: Reuse Address) — cho phép server khởi động lại và
    //> bind lại đúng cổng 8080 NGAY LẬP TỨC, thay vì phải đợi hệ điều
    //> hành giải phóng cổng đó (thường mất khoảng 1–2 phút sau khi server
    //> cũ tắt, do cơ chế TCP TIME_WAIT).
    //> (char*)&opt — lấy địa chỉ của biến opt rồi ép kiểu con trỏ sang
    //> char* vì setsockopt() (hàm C tổng quát) nhận tham số kiểu "con trỏ
    //> tới dữ liệu bất kỳ" dưới dạng void*/char*, không biết trước kiểu cụ thể.
    
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    //> INADDR_ANY là hằng số đặc biệt nghĩa là "lắng nghe trên MỌI địa chỉ
    //> IP của máy này" (ví dụ máy có cả IP mạng LAN lẫn 127.0.0.1, server
    //> sẽ nhận kết nối tới từ cả hai) — khác với client trước đó chỉ định
    //> đích danh một địa chỉ cụ thể ("127.0.0.1") để KẾT NỐI TỚI.
    
if (::bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {    cerr << "Error binding socket!" << endl;
    //> bind() "gắn" socket vào một địa chỉ IP + cổng cụ thể trên máy hiện
    //> tại — bắt buộc phải làm TRƯỚC KHI listen(), để hệ điều hành biết
    //> gửi các kết nối tới cổng 8080 vào đúng socket này.
    //> "::bind" — dùng "::" để gọi hàm bind() toàn cục của hệ thống, tránh
    //> nhầm lẫn nếu có hàm/thành viên nào khác tên "bind" trong phạm vi này
    //> (ví dụ std::bind trong <functional>, vốn là một hàm hoàn toàn khác).
    closeSocket(serverSocket);
#ifdef WINDOWS_BUILD
    cleanupWinsock();
#endif
    return 1;
}
    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        //> listen(socket, backlog) chuyển socket sang chế độ "lắng nghe"
        //> kết nối tới. Tham số thứ hai (10) là "backlog" — số lượng kết
        //> nối tối đa được xếp hàng chờ xử lý nếu server đang bận, chưa
        //> kịp accept() kịp; kết nối vượt quá số này có thể bị từ chối.
        cerr << "Error listening on socket!" << endl;
        closeSocket(serverSocket);
#ifdef WINDOWS_BUILD
        cleanupWinsock();
#endif
        return 1;
    }
    
    cout << "[*] Server started on port 8080" << endl;
    cout << "[*] Waiting for connections...\n" << endl;
    
    while (true) {
        //> Vòng lặp VÔ HẠN (while (true)) — server chạy mãi, liên tục chờ
        //> và chấp nhận kết nối mới, cho tới khi bị dừng thủ công (ví dụ Ctrl+C).
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        //> accept() cần biết TRƯỚC kích thước của struct clientAddr (qua
        //> con trỏ &clientAddrLen) để biết ghi tối đa bao nhiêu byte vào đó.

        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);        
        //> accept() CHỜ (blocking) cho tới khi có một client mới kết nối
        //> tới, sau đó tạo ra MỘT SOCKET MỚI riêng biệt (clientSocket) dùng
        //> để giao tiếp với đúng client đó — khác với serverSocket (vẫn
        //> tiếp tục chỉ dùng để lắng nghe các kết nối tiếp theo).
        //> Đồng thời, accept() điền thông tin địa chỉwhen của client (IP,
        //> port) vào clientAddr thông qua con trỏ &clientAddr.
        if (clientSocket == INVALID_SOCKET) {
            
            cerr << "Error accepting connection!" << endl;
            continue;
            //> Nếu accept() lỗi, bỏ qua và quay lại chờ kết nối tiếp theo,
            //> KHÔNG làm sập cả server.
        }

        thread clientThread(handleClient, clientSocket, clientAddr);
        //> Tạo một luồng mới, cho luồng đó chạy hàm handleClient với 2
        //> tham số truyền kèm (clientSocket, clientAddr) — cú pháp
        //> std::thread(tên_hàm, đối_số_1, đối_số_2, ...) tự động truyền
        //> các đối số này vào đúng vị trí tham số tương ứng của hàm.
        clientThread.detach();
        //> Tách luồng ra chạy độc lập (giống receiveThread.detach() ở file
        //> client) — server không cần "chờ" luồng này xử lý xong mới tiếp
        //> tục vòng lặp accept() các client khác.
    }

    
    closeSocket(serverSocket);
#ifdef WINDOWS_BUILD
    cleanupWinsock();
#endif
    return 0;
    //> Trên thực tế, đoạn code này KHÔNG BAO GIỜ được chạy tới, vì vòng
    //> while(true) phía trên là vô hạn và không có lệnh break nào thoát
    //> ra — server chỉ dừng khi bị buộc tắt từ bên ngoài (ví dụ Ctrl+C,
    //> hoặc kill process). Đây là điểm thường gặp trong các chương trình
    //> server đơn giản kiểu này.
}