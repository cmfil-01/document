// ============================================================
// Cross-Platform Modern Messenger Client with Authentication
// (Ứng dụng chat qua mạng, chạy được cả trên Windows và Linux)
// ============================================================
//
// File này dùng socket TCP để kết nối tới server, gửi/nhận tin nhắn,
// và vẽ giao diện dòng lệnh (terminal UI) bằng mã điều khiển ANSI
// (trên Linux) hoặc Windows Console API (trên Windows).
//
// Các phần chú thích (comment) thêm vào bắt đầu bằng "//>" để bạn
// dễ phân biệt với comment gốc của tác giả.

#include <iostream>   //> cin, cout, cerr — nhập/xuất dữ liệu ra màn hình
#include <cstring>    //> memset() — điền giá trị vào một vùng bộ nhớ (dùng để "dọn sạch" buffer)
#include <thread>     //> std::thread — tạo luồng (thread) chạy song song
#include <atomic>     //> std::atomic — biến an toàn khi nhiều luồng cùng đọc/ghi
#include <string>     //> std::string — kiểu chuỗi ký tự
#include <mutex>      //> std::mutex, std::lock_guard — khóa để tránh 2 luồng ghi màn hình cùng lúc

// ------------------------------------------------------------
//> Đây là kỹ thuật "biên dịch có điều kiện" (conditional compilation).
//> #ifdef WINDOWS_BUILD nghĩa là: "nếu macro WINDOWS_BUILD đã được định nghĩa
//> (thường truyền qua cờ biên dịch -DWINDOWS_BUILD) thì dùng đoạn code này,
//> ngược lại (#else) dùng đoạn code dành cho Linux/Unix".
//> Nhờ vậy, CÙNG MỘT file .cpp này có thể biên dịch ra chương trình chạy
//> được trên cả hai hệ điều hành, vì API socket/console của Windows và
//> Linux khác nhau hoàn toàn.
#ifdef WINDOWS_BUILD
    #include <winsock2.h>   //> Thư viện socket của Windows (Winsock)
    #include <ws2tcpip.h>   //> Các hàm tiện ích thêm cho Winsock (vd inet_pton)
    #include <windows.h>    //> API điều khiển console Windows (màu chữ, vị trí con trỏ...)
    #include <conio.h>      //> Hàm nhập ký tự không cần Enter (không dùng trực tiếp ở đây)
    #pragma comment(lib, "ws2_32.lib")
    //> #pragma comment(lib, ...) là chỉ thị riêng của trình biên dịch MSVC,
    //> báo cho linker "hãy liên kết (link) thêm thư viện ws2_32.lib"
    //> (thư viện chứa các hàm socket của Windows).
    typedef int socklen_t;
    //> Windows không có sẵn kiểu socklen_t như Linux, nên ta tự định nghĩa
    //> (typedef) nó là "int" cho tương thích.
    #define close closesocket
    //> #define ở đây là MACRO THAY THẾ VĂN BẢN: mọi chỗ trong code gọi
    //> hàm close(...) sẽ được trình biên dịch thay bằng closesocket(...)
    //> trước khi biên dịch — giúp code phía dưới dùng chung tên "close"
    //> cho cả hai hệ điều hành.
#else
    #include <arpa/inet.h>   //> inet_pton(), htons()... chuyển đổi địa chỉ IP/port
    #include <netinet/in.h>  //> struct sockaddr_in — cấu trúc địa chỉ mạng IPv4
    #include <sys/socket.h>  //> socket(), connect(), send(), recv()
    #include <unistd.h>      //> close(), usleep()
    #include <termios.h>     //> điều khiển chế độ terminal (không dùng trực tiếp ở đây)
    #include <sys/ioctl.h>   //> ioctl() — lấy kích thước cửa sổ terminal
    #include <signal.h>      //> signal() — bắt tín hiệu Ctrl+C (SIGINT)
    typedef int SOCKET;
    //> Linux dùng "int" thường để đại diện cho socket, còn Windows có
    //> kiểu riêng tên SOCKET. Dòng này tạo bí danh (alias) "SOCKET" = int
    //> để phần code còn lại dùng chung một tên biến kiểu cho cả hai OS.
    #define INVALID_SOCKET -1
    //> Trên Linux, socket() trả về -1 khi lỗi; Windows dùng hằng số
    //> INVALID_SOCKET có sẵn. Macro này giúp code dùng chung tên gọi.
#endif
// ------------------------------------------------------------

using namespace std;
//> Cho phép gọi thẳng cout, string, thread... mà không cần viết
//> std::cout, std::string, std::thread mỗi lần (tiện nhưng trong dự án
//> lớn thường được khuyên tránh dùng "using namespace std" ở phạm vi toàn cục).

// ------------------------------------------------------------
//> BIẾN TOÀN CỤC (global variable) — dùng chung cho mọi hàm trong file.
atomic<bool> running(true);
//> std::atomic<bool>: một biến bool nhưng việc đọc/ghi nó được đảm bảo
//> "nguyên tử" (atomic) — nghĩa là không bị 2 luồng (thread) đọc/ghi
//> chồng chéo gây lỗi dữ liệu. Cần thiết vì chương trình có luồng chính
//> (nhập lệnh) và luồng nhận tin nhắn (receiveMessages) cùng đọc biến này.
atomic<bool> authenticated(false);
//> Tương tự — đánh dấu đã đăng nhập thành công hay chưa, được cả 2 luồng dùng.
SOCKET clientSocket;      //> "Ổ cắm" mạng dùng để giao tiếp với server
string username;          //> Tên người dùng sau khi đăng nhập
mutex displayMutex;
//> std::mutex ("mutual exclusion") — một loại khóa: luồng nào đang giữ
//> khóa thì luồng khác phải đợi, nhờ vậy hai luồng không vẽ lên màn hình
//> cùng lúc gây chồng chữ/lỗi giao diện.
int messageRow = 4;       //> Dòng hiện tại (trên terminal) sẽ in tin nhắn kế tiếp
// ------------------------------------------------------------

// ============================================================
//> NHÓM HÀM ĐIỀU KHIỂN TERMINAL (vẽ giao diện dòng lệnh)
// ============================================================

// Terminal control functions
void clearScreen() {
#ifdef WINDOWS_BUILD
    system("cls");
    //> system(...) gọi thẳng một lệnh của hệ điều hành, ở đây là lệnh
    //> "cls" (clear screen) của Windows Command Prompt.
#else
    cout << "\033[2J\033[1;1H";
    //> "\033" là ký tự ESC (escape, mã 27) mở đầu cho một "chuỗi thoát ANSI"
    //> (ANSI escape sequence) — một quy ước để terminal Linux/macOS hiểu
    //> là lệnh điều khiển (xóa màn hình, đổi màu, di chuyển con trỏ...)
    //> thay vì in ra chữ. "[2J" = xóa toàn bộ màn hình; "[1;1H" = đưa
    //> con trỏ về dòng 1, cột 1.
#endif
}

void moveCursor(int row, int col) {
#ifdef WINDOWS_BUILD
    COORD coord;
    //> COORD là struct có sẵn của Windows API, chứa 2 trường X, Y (kiểu SHORT).
    coord.X = (SHORT)(col - 1);
    coord.Y = (SHORT)(row - 1);
    //> Windows API đánh số tọa độ bắt đầu từ 0, còn hàm này nhận row/col
    //> bắt đầu từ 1 (giống người dùng thường đếm), nên phải trừ 1.
    //> (SHORT) ở đây là ép kiểu (type cast) từ int sang SHORT (số nguyên 16-bit).
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    //> GetStdHandle(STD_OUTPUT_HANDLE) lấy "tay cầm" (handle) đại diện cho
    //> luồng xuất chuẩn (màn hình console); hàm này đặt vị trí con trỏ nhập/in.
#else
    cout << "\033[" << row << ";" << col << "H";
    //> Chuỗi thoát ANSI "ESC[<row>;<col>H" nghĩa là "di chuyển con trỏ tới
    //> dòng row, cột col".
#endif
}

void setColor(const string& color) {
    //> Tham số truyền vào là "const string&" — tham chiếu (reference) tới
    //> chuỗi, có "const" nghĩa là hàm này CAM KẾT không sửa đổi chuỗi được
    //> truyền vào. Dùng reference thay vì truyền bản sao (copy) giúp
    //> tiết kiệm bộ nhớ/thời gian vì string không bị sao chép lại.
#ifdef WINDOWS_BUILD
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (color == "red") SetConsoleTextAttribute(hConsole, 12);
    //> SetConsoleTextAttribute nhận một số mã màu (0–15) quy định sẵn bởi
    //> Windows Console API để đổi màu chữ/nền tiếp theo sẽ in ra.
    else if (color == "green") SetConsoleTextAttribute(hConsole, 10);
    else if (color == "yellow") SetConsoleTextAttribute(hConsole, 14);
    else if (color == "blue") SetConsoleTextAttribute(hConsole, 9);
    else if (color == "magenta") SetConsoleTextAttribute(hConsole, 13);
    else if (color == "cyan") SetConsoleTextAttribute(hConsole, 11);
    else if (color == "white") SetConsoleTextAttribute(hConsole, 15);
    else if (color == "bold") SetConsoleTextAttribute(hConsole, 15);
    else if (color == "reset") SetConsoleTextAttribute(hConsole, 7);
#else
    if (color == "red") cout << "\033[31m";
    //> Chuỗi thoát ANSI "ESC[31m" = chuyển màu chữ tiếp theo sang đỏ.
    //> Các mã số (31, 32, 33...) là mã màu chuẩn ANSI (30–37 cho các màu cơ bản).
    else if (color == "green") cout << "\033[32m";
    else if (color == "yellow") cout << "\033[33m";
    else if (color == "blue") cout << "\033[34m";
    else if (color == "magenta") cout << "\033[35m";
    else if (color == "cyan") cout << "\033[36m";
    else if (color == "white") cout << "\033[37m";
    else if (color == "bold") cout << "\033[1m";
    else if (color == "reset") cout << "\033[0m";
    //> "ESC[0m" = đặt lại (reset) toàn bộ định dạng về mặc định.
#endif
}

int getTerminalHeight() {
#ifdef WINDOWS_BUILD
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    //> struct chứa thông tin về vùng đệm màn hình console (kích thước, vị trí cửa sổ...)
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    //> "&csbi" — lấy ĐỊA CHỈ (con trỏ / pointer) của biến csbi, để hàm này
    //> có thể ghi kết quả trực tiếp vào biến csbi của ta (kiểu "tham số ra"/out-param).
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    //> Chiều cao cửa sổ = hàng dưới cùng trừ hàng trên cùng, cộng 1 (vì tính cả 2 đầu).
#else
    struct winsize w;
    //> struct winsize (định nghĩa trong sys/ioctl.h) chứa số hàng/cột của terminal.
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    //> ioctl() là "input/output control" — một hàm hệ thống đa năng của Linux
    //> để gửi lệnh điều khiển tới thiết bị. TIOCGWINSZ là lệnh "lấy kích thước
    //> cửa sổ terminal", kết quả được ghi vào biến w qua con trỏ &w.
    return w.ws_row;
#endif
}

int getTerminalWidth() {
    //> Giống hệt logic của getTerminalHeight() ở trên, chỉ khác là lấy
    //> số CỘT (chiều rộng) thay vì số HÀNG.
#ifdef WINDOWS_BUILD
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
#endif
}

void drawHeader() {
    int width = getTerminalWidth();
    moveCursor(1, 1);
    setColor("cyan");
    setColor("bold");
    //> Gọi setColor 2 lần liên tiếp: trên Linux, mỗi lần cout thêm một mã
    //> ANSI mới, các mã có thể cộng dồn hiệu ứng (màu + đậm).
    cout << string(width, '=');
    //> string(width, '=') là một CONSTRUCTOR (hàm khởi tạo) của std::string:
    //> tạo ra một chuỗi có "width" ký tự, tất cả đều là ký tự '='.
    //> => in ra một dòng kẻ ngang dài bằng đúng bề rộng terminal.
    moveCursor(2, 1);
    string title = "  C++ MESSENGER  ";
    int padding = (width - (int)title.length()) / 2;
    //> title.length() trả về kiểu size_t (số nguyên không dấu); ép kiểu
    //> (int) để phép trừ với "width" (int) không bị cảnh báo/lệch kiểu.
    //> padding = số khoảng trắng cần thêm mỗi bên để tiêu đề nằm giữa màn hình.
    if (padding < 0) padding = 0;
    //> BUG ĐÃ SỬA: nếu terminal hẹp hơn cả title (width < 18), padding sẽ ÂM.
    //> string(count, ch) yêu cầu "count" là size_type (KHÔNG DẤU) — truyền
    //> một int âm vào sẽ bị ngầm ép kiểu thành một số DƯƠNG KHỔNG LỒ, khiến
    //> chương trình cố cấp phát bộ nhớ gần như vô hạn và crash với lỗi
    //> std::length_error ("basic_string::_M_create"). Chặn padding về 0 để
    //> không bao giờ truyền số âm vào constructor của string.
    cout << string(padding, ' ') << title;
    moveCursor(3, 1);
    cout << string(width, '=');
    setColor("reset");
}

void drawFooter() {
    int height = getTerminalHeight();
    int width = getTerminalWidth();
    
    moveCursor(height - 3, 1);
    setColor("cyan");
    cout << string(width, '-');
    setColor("reset");
    
    moveCursor(height - 2, 1);
    setColor("yellow");
    int commandsPad = width - 28;
    if (commandsPad < 0) commandsPad = 0;
    //> BUG ĐÃ SỬA: cùng lý do như drawHeader() — nếu terminal hẹp hơn 28
    //> cột, "width - 28" sẽ ÂM và bị ép thành số khổng lồ khi đưa vào
    //> string(count, ' '), gây crash std::length_error. Chặn về 0.
    cout << "Commands: /help /users /quit" << string(commandsPad, ' ');
    //> Thêm khoảng trắng phía sau để "xóa" phần chữ cũ còn sót lại trên
    //> dòng đó, vì cout không tự xóa nội dung cũ khi ghi đè.
    setColor("reset");
    
    moveCursor(height - 1, 1);
    setColor("green");
    int promptPad = width - (int)username.length() - 3;
    if (promptPad < 0) promptPad = 0;
    //> BUG ĐÃ SỬA (đây chính là lỗi crash thực tế đã tái hiện được khi
    //> test): "width - username.length() - 3" trộn int với size_t (không
    //> dấu) nên toàn bộ phép tính được thăng cấp (promote) sang số KHÔNG
    //> DẤU — nếu username dài gần bằng hoặc hơn width, kết quả "ÂM" thật
    //> ra đã underflow thành một số dương khổng lồ NGAY TỪ PHÉP TRỪ, trước
    //> khi kịp truyền vào string(). Ép cả hai vế về int trước rồi mới chặn
    //> về 0 để tránh crash.
    cout << username << " > " << string(promptPad, ' ');
    setColor("reset");
    cout.flush();
    //> cout thường được "đệm" (buffered): dữ liệu có thể tạm giữ trong bộ nhớ
    //> trước khi thực sự hiện ra màn hình. flush() ép xuất ngay lập tức,
    //> quan trọng khi ta cần giao diện cập nhật NGAY (không chờ xuống dòng \n).
}

void drawUI() {
    clearScreen();
    drawHeader();
    drawFooter();
    moveCursor(4, 1);
}

void displayMessage(const string& msg) {
    lock_guard<mutex> lock(displayMutex);
    //> std::lock_guard là một "RAII wrapper": ngay khi được tạo ra (constructor),
    //> nó khóa (lock) displayMutex; và khi biến "lock" ra khỏi phạm vi hàm
    //> (kết thúc hàm displayMessage, dù bằng return bình thường hay exception),
    //> destructor của nó TỰ ĐỘNG mở khóa (unlock). Nhờ vậy không cần gọi
    //> unlock() thủ công và không lo quên mở khóa gây "deadlock" (kẹt luồng).
    
    int height = getTerminalHeight();
    int width = getTerminalWidth();
    int maxMessageRow = height - 4;
    
    if (messageRow >= maxMessageRow) {
        messageRow = 4;
        // Clear message area
        for (int i = 4; i < maxMessageRow; i++) {
            moveCursor(i, 1);
            cout << string(width, ' ');
        }
        //> Khi vùng hiển thị tin nhắn đầy, quay lại dòng 4 và xóa sạch
        //> vùng cũ trước khi in tiếp (thay vì cuộn màn hình).
    }
    
    moveCursor(messageRow, 1);
    cout << string(width, ' '); // Clear line
    moveCursor(messageRow, 1);
    
    // Color code messages
    if (msg.find("[ERROR]") != string::npos) {
        //> string::find(...) trả về VỊ TRÍ (index) của chuỗi con nếu tìm thấy,
        //> hoặc hằng số đặc biệt string::npos ("no position") nếu KHÔNG tìm thấy.
        //> So sánh "!= string::npos" tức là "có tìm thấy chuỗi con này".
        setColor("red");
        setColor("bold");
    } else if (msg.find("[SUCCESS]") != string::npos) {
        setColor("green");
        setColor("bold");
    } else if (msg.find("[SYSTEM]") != string::npos) {
        setColor("yellow");
        setColor("bold");
    } else if (msg.find(username + ":") != string::npos) {
        //> username + ":" — toán tử "+" trên std::string thực hiện NỐI CHUỖI
        //> (concatenation), tạo ra một chuỗi mới, không làm thay đổi username.
        setColor("green");
    } else {
        setColor("cyan");
    }
    
    // Wrap long messages
    if (msg.length() > (size_t)width - 2) {
        //> (size_t)width — ép kiểu int sang size_t (số nguyên không dấu),
        //> để so sánh cùng kiểu với msg.length() (vốn trả về size_t),
        //> tránh cảnh báo "so sánh có dấu với không dấu" của trình biên dịch.
        cout << msg.substr(0, width - 5) << "...";
        //> substr(0, n) trả về một chuỗi con gồm n ký tự đầu tiên của msg.
    } else {
        cout << msg;
    }
    
    setColor("reset");
    messageRow++;
    
    // Restore cursor to input line
    moveCursor(height - 1, (int)username.length() + 4);
    cout.flush();
}

// ============================================================
//> LUỒNG (THREAD) RIÊNG ĐỂ NHẬN TIN NHẮN TỪ SERVER
// ============================================================
void receiveMessages() {
    char buffer[4096];
    //> Mảng ký tự (C-style array) cố định 4096 byte, dùng làm vùng đệm
    //> tạm để chứa dữ liệu thô nhận được từ socket.
    string recvBuffer;
    //> BUG ĐÃ SỬA — vùng đệm TÍCH LŨY giữa các lần recv(). TCP là một
    //> LUỒNG BYTE liên tục (byte stream), không phải luồng "tin nhắn":
    //> hệ điều hành được phép GỘP nhiều lần send() liên tiếp của server
    //> thành MỘT lần recv() ở đây (đã tái hiện được lỗi này khi test:
    //> "[SUCCESS] Login successful!" và "[SYSTEM] Type /help..." gửi
    //> cách nhau vài micro-giây bị gộp làm một, hiển thị dính liền và bị
    //> cắt bớt bởi logic "Wrap long messages" bên dưới) — hoặc ngược lại,
    //> CHIA một tin nhắn dài thành nhiều lần recv() nếu nó vượt quá kích
    //> thước buffer nội bộ của hệ điều hành. Vì vậy không thể coi mỗi lần
    //> recv() là đúng một tin nhắn; phải gom dữ liệu vào recvBuffer rồi
    //> tự tách theo ranh giới do server chèn vào (xem sendToClient() và
    //> broadcastMessage() bên server: mỗi tin nhắn được thêm '\n' ở cuối).
    
    while (running) {
        //> Vòng lặp chạy liên tục cho tới khi biến atomic "running" thành false
        //> (được đặt từ luồng khác — ví dụ khi người dùng gõ /quit hoặc Ctrl+C).
        memset(buffer, 0, sizeof(buffer));
        //> memset(buffer, 0, sizeof(buffer)) = điền toàn bộ vùng nhớ "buffer"
        //> bằng byte 0, để xóa sạch dữ liệu tin nhắn cũ trước khi nhận tin mới
        //> (tránh dữ liệu cũ còn sót lại phía sau tin nhắn mới ngắn hơn).
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        //> recv(...) là hàm socket chuẩn: CHỜ (blocking) cho tới khi có dữ liệu
        //> đến từ server, rồi ghi vào "buffer" tối đa sizeof(buffer) byte,
        //> và trả về SỐ BYTE thực sự nhận được.
        
        if (bytesReceived <= 0) {
            //> recv trả về 0 nghĩa là server đã đóng kết nối; số âm nghĩa là có lỗi.
            running = false;
            if (authenticated) {
                displayMessage("[SYSTEM] Connection lost!");
            }
            break;
            //> break thoát khỏi vòng while, kết thúc luồng nhận tin nhắn.
        }
        
        recvBuffer.append(buffer, bytesReceived);
        //> BUG ĐÃ SỬA: dùng append(buffer, bytesReceived) — độ dài TƯỜNG
        //> MINH — thay vì string(buffer) (vốn đọc tới ký tự NULL đầu tiên).
        //> Nếu một lần recv() nhận đủ 4096 byte (lấp đầy nguyên buffer),
        //> có thể không còn byte 0 nào sau dữ liệu thật, khiến string(buffer)
        //> đọc TRÀN ra ngoài mảng để tìm ký tự kết thúc — hành vi không xác
        //> định (undefined behavior). append(buffer, bytesReceived) luôn an
        //> toàn vì chỉ đọc đúng số byte đã thực sự nhận được.
        
        size_t newlinePos;
        while ((newlinePos = recvBuffer.find('\n')) != string::npos) {
            //> Mỗi khi tìm thấy '\n' trong recvBuffer, đó là ranh giới kết
            //> thúc của MỘT tin nhắn hoàn chỉnh. Lấy phần trước '\n' ra xử
            //> lý, rồi xóa luôn phần đó (kể cả '\n') khỏi recvBuffer —
            //> phần còn lại (nếu có) là dữ liệu của (một phần) tin nhắn
            //> TIẾP THEO, sẽ tiếp tục được nối thêm ở lần recv() sau. Vòng
            //> while này xử lý HẾT các tin nhắn trọn vẹn đang có trong
            //> buffer trước khi gọi recv() lần nữa, phòng trường hợp một
            //> lần recv() gộp sẵn nhiều tin nhắn.
            string msg = recvBuffer.substr(0, newlinePos);
            recvBuffer.erase(0, newlinePos + 1);
            
            // Check for successful login
            if (msg.find("[SUCCESS] Login successful") != string::npos) {
                authenticated = true;
            }
            
            // Display message during authentication phase
            if (!authenticated) {
                setColor("yellow");
                cout << msg << endl;
                //> endl không chỉ xuống dòng mà còn tự flush() luôn, khác với '\n'
                //> (chỉ xuống dòng, không ép xuất ngay).
                setColor("reset");
                cout.flush();
            } else {
                displayMessage(msg);
            }
        }
    }
}

// ============================================================
//> XỬ LÝ TÍN HIỆU NGẮT (Ctrl+C) — mỗi hệ điều hành có cơ chế riêng
// ============================================================
#ifdef WINDOWS_BUILD
BOOL WINAPI ConsoleHandler(DWORD signal) {
    //> Đây là "hàm callback": ta không tự gọi hàm này, mà đăng ký nó với
    //> hệ điều hành (qua SetConsoleCtrlHandler bên dưới trong main), và
    //> Windows sẽ TỰ GỌI hàm này khi người dùng nhấn Ctrl+C.
    //> BOOL, WINAPI, DWORD là các kiểu/quy ước gọi hàm riêng của Windows API.
    if (signal == CTRL_C_EVENT) {
        running = false;
        closesocket(clientSocket);
        WSACleanup();
        //> WSACleanup() giải phóng tài nguyên mà WSAStartup() đã cấp phát
        //> lúc khởi động Winsock (xem trong main()).
        clearScreen();
        setColor("yellow");
        cout << "\n[*] Disconnected from server. Goodbye!\n";
        setColor("reset");
        exit(0);
        //> exit(0) thoát chương trình ngay lập tức với mã trả về 0 (thành công),
        //> khác với "return" vì có thể gọi từ bất kỳ đâu, không chỉ trong main().
    }
    return TRUE;
}
#else
void signalHandler(int signal) {
    //> Tương tự ConsoleHandler ở trên nhưng theo API chuẩn POSIX (Linux/macOS):
    //> hàm này được đăng ký bằng signal(SIGINT, signalHandler) trong main(),
    //> và hệ điều hành tự gọi nó khi nhận tín hiệu SIGINT (Ctrl+C).
    running = false;
    close(clientSocket);
    clearScreen();
    setColor("yellow");
    cout << "\n[*] Disconnected from server. Goodbye!\n";
    setColor("reset");
    exit(0);
}
#endif

// ============================================================
//> HÀM MAIN — điểm bắt đầu chạy chương trình
// ============================================================
int main() {
#ifdef WINDOWS_BUILD
    // Initialize Winsock
    WSADATA wsaData;
    //> Trên Windows, TRƯỚC KHI dùng bất kỳ hàm socket nào, bắt buộc phải
    //> khởi tạo thư viện Winsock bằng WSAStartup. wsaData sẽ được điền
    //> thông tin về phiên bản Winsock đang dùng.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        //> MAKEWORD(2, 2) yêu cầu Winsock phiên bản 2.2.
        //> WSAStartup trả về 0 nếu thành công, khác 0 nếu lỗi.
        cerr << "WSAStartup failed!" << endl;
        //> cerr — luồng xuất dành riêng cho thông báo LỖI (khác cout),
        //> thường không bị "đệm" (buffered) nên hiện ra ngay lập tức.
        return 1;
        //> return khác 0 trong main() theo quy ước là "chương trình kết thúc do lỗi".
    }
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    //> Đặt "code page" của console sang UTF-8, để hiển thị/nhập được
    //> tiếng Việt có dấu và các ký tự Unicode khác một cách chính xác.
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);
    //> Đăng ký hàm ConsoleHandler (định nghĩa ở trên) làm hàm xử lý khi
    //> người dùng nhấn Ctrl+C. Tham số TRUE nghĩa là "thêm handler này".
#else
    signal(SIGINT, signalHandler);
    //> Đăng ký signalHandler làm hàm xử lý tín hiệu SIGINT (Ctrl+C) trên Linux.
#endif
    
    clearScreen();
    setColor("cyan");
    setColor("bold");
    cout << "\n+========================================+\n";
    cout << "|   C++ Messenger Client v2.0            |\n";
    cout << "|   With User Authentication             |\n";
#ifdef WINDOWS_BUILD
    cout << "|        Windows Build                   |\n";
#else
    cout << "|        Linux/Unix Build                |\n";
#endif
    cout << "+========================================+\n\n";
    setColor("reset");
    
    setColor("yellow");
    cout << "Connecting to server...\n";
    setColor("reset");
    
    // --------------------------------------------------------
    //> TẠO SOCKET
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    //> socket(domain, type, protocol):
    //>   AF_INET     = dùng địa chỉ IPv4
    //>   SOCK_STREAM = socket kiểu "luồng" (stream), tức giao thức TCP
    //>                 (đảm bảo thứ tự và độ tin cậy dữ liệu — khác với
    //>                  SOCK_DGRAM dùng cho UDP)
    //>   0           = để hệ thống tự chọn giao thức mặc định (TCP cho SOCK_STREAM)
    if (clientSocket == INVALID_SOCKET) {
        setColor("red");
        cerr << "Error creating socket!" << endl;
        setColor("reset");
#ifdef WINDOWS_BUILD
        WSACleanup();
#endif
        return 1;
    }
    
    // --------------------------------------------------------
    //> CHUẨN BỊ ĐỊA CHỈ SERVER CẦN KẾT NỐI TỚI
    sockaddr_in serverAddress;
    //> struct sockaddr_in — cấu trúc chuẩn để mô tả một địa chỉ mạng IPv4,
    //> gồm: họ địa chỉ (family), số hiệu cổng (port), địa chỉ IP...
    memset(&serverAddress, 0, sizeof(serverAddress));
    //> Xóa sạch toàn bộ struct về 0 trước khi điền dữ liệu — thói quen an toàn
    //> chuẩn khi làm việc với các struct hệ thống, tránh giá trị rác còn sót.
    serverAddress.sin_family = AF_INET;      //> họ địa chỉ: IPv4
    serverAddress.sin_port = htons(8080);
    //> htons = "host to network short": chuyển số port từ thứ tự byte của
    //> máy tính hiện tại (host byte order) sang thứ tự byte chuẩn mạng
    //> (network byte order — luôn là big-endian). Bắt buộc phải gọi vì
    //> các máy khác nhau có thể lưu số nguyên theo thứ tự byte khác nhau.
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        //> inet_pton ("presentation to network") chuyển chuỗi địa chỉ IP
        //> dạng con người đọc được ("127.0.0.1" = địa chỉ loopback, tức
        //> chính máy đang chạy chương trình) sang dạng nhị phân mà struct
        //> sockaddr_in cần, rồi ghi kết quả vào &serverAddress.sin_addr.
        //> Trả về giá trị <= 0 nghĩa là chuỗi địa chỉ không hợp lệ.
        perror("Invalid address");
        //> perror in ra thông báo lỗi kèm mô tả lỗi hệ thống gần nhất (errno).
        return 1;
    }    
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        //> connect() thực sự thiết lập kết nối TCP tới server.
        //> (struct sockaddr*)&serverAddress — ép kiểu con trỏ: hàm connect()
        //> theo chuẩn C nhận tham số kiểu "struct sockaddr*" chung chung,
        //> nên ta phải ép con trỏ sockaddr_in* (cụ thể cho IPv4) về kiểu
        //> tổng quát hơn sockaddr* mà hàm yêu cầu.
        setColor("red");
        cerr << "Error connecting to server!" << endl;
        setColor("reset");
        close(clientSocket);
#ifdef WINDOWS_BUILD
        WSACleanup();
#endif
        return 1;
    }
    
    setColor("green");
    cout << "Connected to server!\n\n";
    setColor("reset");
    
    // --------------------------------------------------------
    //> TẠO LUỒNG (THREAD) RIÊNG ĐỂ LẮNG NGHE TIN NHẮN TỪ SERVER,
    //> chạy song song với luồng chính (đang xử lý nhập liệu từ bàn phím).
    thread receiveThread(receiveMessages);
    //> Tạo một std::thread mới, cho nó chạy hàm receiveMessages() ngay lập tức
    //> trên một luồng hệ điều hành riêng biệt.
    receiveThread.detach();
    //> detach() "tách" luồng ra khỏi đối tượng thread trong main — luồng
    //> con sẽ tiếp tục chạy độc lập, ngay cả sau khi hàm main() không còn
    //> "quản lý" đối tượng receiveThread nữa (khác với join(), vốn khiến
    //> main phải DỪNG LẠI chờ luồng con chạy xong).
    
    // Wait for welcome message
#ifdef WINDOWS_BUILD
    Sleep(200);
    //> Sleep() của Windows nhận tham số tính bằng MILLI-giây.
#else
    usleep(200000);
    //> usleep() của Linux nhận tham số tính bằng MICRO-giây,
    //> nên 200000 micro-giây = 200 mili-giây (tương đương Sleep(200) bên trên).
#endif
    
    // --------------------------------------------------------
    //> VÒNG LẶP ĐĂNG NHẬP: lặp lại việc đọc lệnh từ bàn phím và gửi cho
    //> server, cho tới khi biến "authenticated" chuyển thành true.
    string input;
    while (!authenticated && running) {
        setColor("cyan");
        cout << "Enter command: ";
        setColor("reset");
        getline(cin, input);
        //> getline(cin, input) đọc TOÀN BỘ MỘT DÒNG người dùng gõ (kể cả
        //> khoảng trắng) vào biến input, khác với "cin >> input" chỉ đọc
        //> tới khoảng trắng đầu tiên.
        
        if (!input.empty()) {
            send(clientSocket, input.c_str(), (int)input.length(), 0);
            //> send() gửi dữ liệu qua socket. input.c_str() chuyển
            //> std::string sang con trỏ chuỗi kiểu C (const char*) mà
            //> hàm send() (viết bằng C) yêu cầu; (int)input.length() ép
            //> kiểu độ dài chuỗi (size_t) sang int cho khớp tham số hàm.
            
            // Wait for server response
#ifdef WINDOWS_BUILD
            Sleep(300);
#else
            usleep(300000);
#endif
            
            // Check if authentication succeeded and extract username
            if (authenticated) {
                //> Lưu ý: biến "authenticated" được cập nhật bởi LUỒNG KHÁC
                //> (receiveMessages ở trên) khi nhận được thông báo đăng nhập
                //> thành công từ server — đây là lý do cần kiểu atomic<bool>,
                //> để đọc giá trị mới nhất một cách an toàn giữa hai luồng.
                size_t firstSpace = input.find(' ');
                size_t secondSpace = input.find(' ', firstSpace + 1);
                //> find(' ', firstSpace + 1) — bắt đầu tìm khoảng trắng
                //> từ vị trí (firstSpace + 1) trở đi, tức là tìm khoảng
                //> trắng THỨ HAI (bỏ qua khoảng trắng thứ nhất).
                //> Ví dụ input = "LOGIN alice pass123" thì lệnh này tách ra
                //> được username nằm giữa khoảng trắng thứ 1 và thứ 2.
                if (firstSpace != string::npos && secondSpace != string::npos) {
                    username = input.substr(firstSpace + 1, secondSpace - firstSpace - 1);
                    //> substr(vị_trí_bắt_đầu, số_ký_tự) trích chuỗi con:
                    //> lấy phần nằm giữa 2 khoảng trắng — chính là username.
                }
                
                // Small delay before switching to UI mode
#ifdef WINDOWS_BUILD
                Sleep(500);
#else
                usleep(500000);
#endif
                break;
                //> Thoát vòng lặp đăng nhập, chuyển sang giao diện chat chính.
            }
        }
    }
    
    if (!authenticated) {
        //> Nếu thoát vòng lặp trên mà VẪN chưa đăng nhập được (ví dụ mất
        //> kết nối khiến running = false), thì dừng chương trình luôn.
        close(clientSocket);
#ifdef WINDOWS_BUILD
        WSACleanup();
#endif
        return 1;
    }
    
    // Initialize UI after authentication
    drawUI();
    
    // --------------------------------------------------------
    //> VÒNG LẶP CHÍNH: đọc tin nhắn người dùng gõ và gửi đi, cho tới khi
    //> gõ lệnh /quit hoặc mất kết nối (running = false).
    string message;
    while (running) {
        // Position cursor at input line
        int height = getTerminalHeight();
        moveCursor(height - 1, (int)username.length() + 4);
        
        getline(cin, message);
        
        if (!message.empty()) {
            if (message == "/quit") {
                running = false;
                send(clientSocket, message.c_str(), (int)message.length(), 0);
                break;
            } else if (message == "/clear") {
                messageRow = 4;
                drawUI();
            } else {
                send(clientSocket, message.c_str(), (int)message.length(), 0);
                
                // Clear the input line after sending
                lock_guard<mutex> lock(displayMutex);
                //> Khóa displayMutex ở đây vì luồng chính đang chuẩn bị vẽ
                //> lại dòng nhập lệnh, trong khi luồng receiveMessages có
                //> thể đồng thời đang gọi displayMessage() để vẽ tin nhắn
                //> mới — khóa này ngăn 2 luồng vẽ màn hình chồng lên nhau.
                moveCursor(height - 1, 1);
                setColor("green");
                int promptPad = getTerminalWidth() - (int)username.length() - 3;
                if (promptPad < 0) promptPad = 0;
                //> BUG ĐÃ SỬA: chặn số đệm âm như đã giải thích trong
                //> drawFooter(), để tránh crash std::length_error khi
                //> terminal bị thu hẹp hoặc username dài.
                cout << username << " > " << string(promptPad, ' ');
                setColor("reset");
                moveCursor(height - 1, (int)username.length() + 4);
                cout.flush();
            }
            //> Lưu ý: biến "lock" (lock_guard) được khai báo BÊN TRONG khối
            //> else {...}, nên nó tự động unlock ngay khi khối else kết thúc
            //> (hết phạm vi / scope) — không cần gọi unlock() thủ công.
        }
    }
    
    // --------------------------------------------------------
    //> DỌN DẸP TRƯỚC KHI THOÁT CHƯƠNG TRÌNH
    close(clientSocket);
#ifdef WINDOWS_BUILD
    WSACleanup();
#endif
    clearScreen();
    setColor("yellow");
    cout << "\n[*] Disconnected from server. Goodbye!\n";
    setColor("reset");
    
    return 0;
    //> return 0 trong main() theo quy ước nghĩa là "chương trình kết thúc
    //> bình thường, không có lỗi".
}