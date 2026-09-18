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
//
//> YÊU CẦU BIÊN DỊCH: C++17 trở lên (nên dùng C++20 cho đồng bộ với server).
//>   Linux:   g++ -std=c++20 -pthread client.cpp -o client
//>   Windows: cl /std:c++20 /DWINDOWS_BUILD client.cpp ws2_32.lib
//>            (hoặc MinGW: g++ -std=c++20 -DWINDOWS_BUILD client.cpp -lws2_32)

#include <iostream>   //> cin, cout, cerr — nhập/xuất dữ liệu ra màn hình
#include <cstring>    //> các hàm thao tác chuỗi/bộ nhớ kiểu C
#include <thread>     //> std::thread — tạo luồng (thread) chạy song song
#include <atomic>     //> std::atomic — biến an toàn khi nhiều luồng cùng đọc/ghi
#include <string>     //> std::string — kiểu chuỗi ký tự
#include <mutex>      //> std::mutex, std::lock_guard — khóa để tránh 2 luồng ghi màn hình cùng lúc
#include <array>      //> std::array — mảng kích thước cố định, an toàn hơn mảng C-style
#include <chrono>     //> std::chrono — đơn vị thời gian chuẩn của C++ (thay Sleep/usleep)

// ------------------------------------------------------------
//> Đây là kỹ thuật "biên dịch có điều kiện" (conditional compilation).
//> #ifdef WINDOWS_BUILD nghĩa là: "nếu macro WINDOWS_BUILD đã được định nghĩa
//> (thường truyền qua cờ biên dịch -DWINDOWS_BUILD) thì dùng đoạn code này,
//> ngược lại (#else) dùng đoạn code dành cho Linux/Unix".
//> Nhờ vậy, CÙNG MỘT file .cpp này có thể biên dịch ra chương trình chạy
//> được trên cả hai hệ điều hành, vì API socket/console của Windows và
//> Linux khác nhau hoàn toàn.
#if defined(WINDOWS_BUILD) || defined(_WIN32)
    #include <winsock2.h>   //> Thư viện socket của Windows (Winsock)
    #include <ws2tcpip.h>   //> Các hàm tiện ích thêm cho Winsock (vd inet_pton)
    #include <windows.h>    //> API điều khiển console Windows (màu chữ, vị trí con trỏ...)
    #pragma comment(lib, "ws2_32.lib")
    //> #pragma comment(lib, ...) là chỉ thị riêng của trình biên dịch MSVC,
    //> báo cho linker "hãy liên kết (link) thêm thư viện ws2_32.lib"
    //> (thư viện chứa các hàm socket của Windows).
    using socklen_t = int;
    //> HIỆN ĐẠI HÓA: "using" thay cho "typedef" cũ — tương đương nhau nhưng
    //> "using" đọc xuôi hơn (tên_mới = kiểu_cũ) và được khuyến nghị từ C++11.
    //>
    //> HIỆN ĐẠI HÓA QUAN TRỌNG: đã BỎ dòng "#define close closesocket".
    //> Macro đó là THAY THẾ VĂN BẢN mù quáng: nó đổi MỌI chữ "close" trong
    //> phần code phía dưới — kể cả file.close(), stream.close(), hay một
    //> hàm thành viên tên close() của bất kỳ lớp nào — thành "closesocket",
    //> gây lỗi biên dịch khó hiểu ngay khi ta thêm một thư viện mới. Chính
    //> file server đã phải ghi chú cảnh báo về đúng cái bẫy này. Thay vào
    //> đó, bên dưới có hàm closeSocket() rõ ràng, có kiểm tra kiểu.
#else
    #include <arpa/inet.h>   //> inet_pton(), htons()... chuyển đổi địa chỉ IP/port
    #include <netinet/in.h>  //> struct sockaddr_in — cấu trúc địa chỉ mạng IPv4
    #include <sys/socket.h>  //> socket(), connect(), send(), recv()
    #include <unistd.h>      //> close(), write(), _exit()
    #include <sys/ioctl.h>   //> ioctl() — lấy kích thước cửa sổ terminal
    #include <signal.h>      //> struct sigaction và sigaction() — bắt SIGINT
    //> sigaction là API POSIX và được khai báo trong <signal.h>. Dùng trực tiếp
    //> header POSIX này để bảo đảm struct sigaction là kiểu hoàn chỉnh.
    using SOCKET = int;
    //> Linux dùng "int" thường để đại diện cho socket, còn Windows có
    //> kiểu riêng tên SOCKET. Dòng này tạo bí danh (alias) "SOCKET" = int
    //> để phần code còn lại dùng chung một tên biến kiểu cho cả hai OS.
    constexpr SOCKET INVALID_SOCKET = -1;
    //> HIỆN ĐẠI HÓA: hằng số constexpr thay cho "#define INVALID_SOCKET -1".
    //> Macro không có kiểu dữ liệu và không tuân theo phạm vi (scope);
    //> constexpr tạo hằng số thật, có kiểu, được trình biên dịch kiểm tra,
    //> và vẫn được tính sẵn lúc biên dịch nên không chậm hơn chút nào.
#endif
// ------------------------------------------------------------

//> HIỆN ĐẠI HÓA: đã BỎ "using namespace std;" ở phạm vi toàn cục.
//> Chính comment gốc ở đây cũng đã lưu ý "trong dự án lớn thường được
//> khuyên tránh dùng": nó kéo toàn bộ thư viện chuẩn vào không gian tên
//> toàn cục, dễ gây đụng tên với code của mình hoặc thư viện khác. Viết
//> std:: đầy đủ dài hơn chút nhưng rõ ràng và an toàn hơn hẳn.

// ------------------------------------------------------------
//> BIẾN TOÀN CỤC (global variable) — dùng chung cho mọi hàm trong file.
std::atomic<bool> running{true};
//> std::atomic<bool>: một biến bool nhưng việc đọc/ghi nó được đảm bảo
//> "nguyên tử" (atomic) — nghĩa là không bị 2 luồng (thread) đọc/ghi
//> chồng chéo gây lỗi dữ liệu. Cần thiết vì chương trình có luồng chính
//> (nhập lệnh) và luồng nhận tin nhắn (receiveMessages) cùng đọc biến này.
std::atomic<bool> authenticated{false};
//> Tương tự — đánh dấu đã đăng nhập thành công hay chưa, được cả 2 luồng dùng.
//> HIỆN ĐẠI HÓA: khởi tạo bằng ngoặc nhọn {} thay vì ngoặc tròn (). Cách
//> viết "{}" (uniform initialization, C++11) thống nhất cho mọi kiểu dữ
//> liệu và ngăn các phép chuyển đổi kiểu làm mất dữ liệu một cách âm thầm.
SOCKET clientSocket{INVALID_SOCKET};   //> "Ổ cắm" mạng dùng để giao tiếp với server
std::string username;                  //> Tên người dùng sau khi đăng nhập
std::mutex displayMutex;
//> std::mutex ("mutual exclusion") — một loại khóa: luồng nào đang giữ
//> khóa thì luồng khác phải đợi, nhờ vậy hai luồng không vẽ lên màn hình
//> cùng lúc gây chồng chữ/lỗi giao diện.
int messageRow = 4;       //> Dòng hiện tại (trên terminal) sẽ in tin nhắn kế tiếp
//> Lưu ý: messageRow chỉ được phép đọc/ghi khi ĐANG GIỮ displayMutex —
//> xem displayMessage() và nhánh xử lý lệnh /clear trong main().

constexpr std::size_t BUFFER_SIZE = 4096;
constexpr unsigned short SERVER_PORT = 8080;
constexpr const char* SERVER_IP = "127.0.0.1";
constexpr int HEADER_ROWS = 4;     //> Số dòng dành cho phần tiêu đề phía trên
constexpr int FOOTER_ROWS = 4;     //> Số dòng dành cho phần chân (lệnh + ô nhập)
//> HIỆN ĐẠI HÓA: gom các "con số ma" (magic number) rải rác thành hằng số
//> có TÊN. Trước đây các số 4096, 8080, "127.0.0.1", 4 được viết thẳng vào
//> giữa code — muốn đổi địa chỉ server phải đi tìm từng chỗ, và người đọc
//> phải tự đoán ý nghĩa của con số.
// ------------------------------------------------------------

// ============================================================
//> NHÓM HÀM ĐIỀU KHIỂN TERMINAL (vẽ giao diện dòng lệnh)
// ============================================================

// Cross-platform socket close helper
inline void closeSocket(SOCKET s) {
    //> HIỆN ĐẠI HÓA: hàm này thay thế cho macro "#define close closesocket"
    //> đã bị gỡ bỏ ở phần đầu file (xem giải thích ở đó). Đây chính là cách
    //> mà file server vẫn luôn làm — giờ hai file thống nhất cùng một kiểu.
#if defined(WINDOWS_BUILD) || defined(_WIN32)
    closesocket(s);
#else
    close(s);
    //> "::close" — dấu "::" nghĩa là "hàm close() ở phạm vi toàn cục", để
    //> chắc chắn gọi đúng hàm close() của hệ thống (POSIX).
#endif
}

inline void shutdownSocket(SOCKET s) {
    //> HIỆN ĐẠI HÓA / SỬA DATA RACE: shutdown() báo cho hệ điều hành đóng
    //> chiều đọc & ghi của kết nối, khiến lời gọi recv() đang BỊ CHẶN ở
    //> luồng nhận tin lập tức trả về 0 — nhờ vậy luồng đó tự thoát vòng lặp
    //> và kết thúc. Đây là cách đúng để "đánh thức" một luồng đang chờ
    //> trên socket, thay vì đóng thẳng socket trong khi luồng khác còn
    //> đang dùng nó (xem giải thích ở phần dọn dẹp cuối hàm main).
#if defined(WINDOWS_BUILD) || defined(_WIN32)
    shutdown(s, SD_BOTH);
#else
    ::shutdown(s, SHUT_RDWR);
#endif
}

// Terminal control functions
void clearScreen() {
#ifdef WINDOWS_BUILD
    //> HIỆN ĐẠI HÓA: thay cho system("cls"). Gọi system() để xóa màn hình
    //> là cách làm bị chê từ lâu: nó phải nạp cả một tiến trình shell mới
    //> (rất chậm so với việc chỉ xóa màn hình), phụ thuộc vào lệnh "cls"
    //> có tồn tại trong PATH hay không, và về nguyên tắc còn là một lỗ
    //> hổng bảo mật nếu PATH bị kẻ xấu can thiệp. Dùng thẳng Console API
    //> vừa nhanh vừa chắc chắn.
    const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    const DWORD cells = static_cast<DWORD>(csbi.dwSize.X) * static_cast<DWORD>(csbi.dwSize.Y);
    const COORD origin{0, 0};
    DWORD written = 0;
    FillConsoleOutputCharacterA(hConsole, ' ', cells, origin, &written);
    //> Ghi đè toàn bộ vùng đệm màn hình bằng ký tự khoảng trắng...
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cells, origin, &written);
    //> ...và đặt lại thuộc tính màu cho toàn bộ vùng đó.
    SetConsoleCursorPosition(hConsole, origin);
    //> Cuối cùng đưa con trỏ về góc trên bên trái (0,0).
#else
    std::cout << "\033[2J\033[1;1H";
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
    coord.X = static_cast<SHORT>(col - 1);
    coord.Y = static_cast<SHORT>(row - 1);
    //> Windows API đánh số tọa độ bắt đầu từ 0, còn hàm này nhận row/col
    //> bắt đầu từ 1 (giống người dùng thường đếm), nên phải trừ 1.
    //> HIỆN ĐẠI HÓA: static_cast<SHORT> thay cho ép kiểu kiểu C "(SHORT)".
    //> Ép kiểu kiểu C rất "mạnh tay" — nó lặng lẽ thử cả những cách nguy
    //> hiểm (bỏ const, diễn giải lại con trỏ). static_cast chỉ cho phép
    //> các chuyển đổi hợp lý và báo lỗi nếu ta viết sai.
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    //> GetStdHandle(STD_OUTPUT_HANDLE) lấy "tay cầm" (handle) đại diện cho
    //> luồng xuất chuẩn (màn hình console); hàm này đặt vị trí con trỏ nhập/in.
#else
    std::cout << "\033[" << row << ";" << col << "H";
    //> Chuỗi thoát ANSI "ESC[<row>;<col>H" nghĩa là "di chuyển con trỏ tới
    //> dòng row, cột col".
#endif
}

void setColor(const std::string& color) {
    //> Tham số truyền vào là "const string&" — tham chiếu (reference) tới
    //> chuỗi, có "const" nghĩa là hàm này CAM KẾT không sửa đổi chuỗi được
    //> truyền vào. Dùng reference thay vì truyền bản sao (copy) giúp
    //> tiết kiệm bộ nhớ/thời gian vì string không bị sao chép lại.
#ifdef WINDOWS_BUILD
    const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
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
    if (color == "red") std::cout << "\033[31m";
    //> Chuỗi thoát ANSI "ESC[31m" = chuyển màu chữ tiếp theo sang đỏ.
    //> Các mã số (31, 32, 33...) là mã màu chuẩn ANSI (30–37 cho các màu cơ bản).
    else if (color == "green") std::cout << "\033[32m";
    else if (color == "yellow") std::cout << "\033[33m";
    else if (color == "blue") std::cout << "\033[34m";
    else if (color == "magenta") std::cout << "\033[35m";
    else if (color == "cyan") std::cout << "\033[36m";
    else if (color == "white") std::cout << "\033[37m";
    else if (color == "bold") std::cout << "\033[1m";
    else if (color == "reset") std::cout << "\033[0m";
    //> "ESC[0m" = đặt lại (reset) toàn bộ định dạng về mặc định.
#endif
}

int getTerminalHeight() {
#if defined(WINDOWS_BUILD) || defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    //> struct chứa thông tin về vùng đệm màn hình console (kích thước, vị trí cửa sổ...)
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) return 24;
    //> "&csbi" — lấy ĐỊA CHỈ (con trỏ / pointer) của biến csbi, để hàm này
    //> có thể ghi kết quả trực tiếp vào biến csbi của ta (kiểu "tham số ra"/out-param).
    const int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    //> Chiều cao cửa sổ = hàng dưới cùng trừ hàng trên cùng, cộng 1 (vì tính cả 2 đầu).
    return height > 0 ? height : 24;
#else
    struct winsize w{};
    //> struct winsize (định nghĩa trong sys/ioctl.h) chứa số hàng/cột của terminal.
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0) return 24;
    //> ioctl() là "input/output control" — một hàm hệ thống đa năng của Linux
    //> để gửi lệnh điều khiển tới thiết bị. TIOCGWINSZ là lệnh "lấy kích thước
    //> cửa sổ terminal", kết quả được ghi vào biến w qua con trỏ &w.
    //> HIỆN ĐẠI HÓA + AN TOÀN: bản cũ BỎ QUA giá trị trả về của ioctl(). Nếu
    //> lời gọi thất bại (ví dụ khi đầu ra bị chuyển hướng vào file/pipe chứ
    //> không phải terminal thật), biến w giữ nguyên giá trị rác/0 và cả giao
    //> diện sẽ tính toán sai. Giờ ta kiểm tra và trả về kích thước mặc định
    //> 24x80 hợp lý khi không xác định được.
    return w.ws_row > 0 ? w.ws_row : 24;
#endif
}

int getTerminalWidth() {
    //> Giống hệt logic của getTerminalHeight() ở trên, chỉ khác là lấy
    //> số CỘT (chiều rộng) thay vì số HÀNG.
#if defined(WINDOWS_BUILD) || defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) return 80;
    const int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return width > 0 ? width : 80;
#else
    struct winsize w{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0) return 80;
    return w.ws_col > 0 ? w.ws_col : 80;
#endif
}

void drawHeader() {
    const int width = getTerminalWidth();
    moveCursor(1, 1);
    setColor("cyan");
    setColor("bold");
    //> Gọi setColor 2 lần liên tiếp: trên Linux, mỗi lần cout thêm một mã
    //> ANSI mới, các mã có thể cộng dồn hiệu ứng (màu + đậm).
    std::cout << std::string(width, '=');
    //> string(width, '=') là một CONSTRUCTOR (hàm khởi tạo) của std::string:
    //> tạo ra một chuỗi có "width" ký tự, tất cả đều là ký tự '='.
    //> => in ra một dòng kẻ ngang dài bằng đúng bề rộng terminal.
    moveCursor(2, 1);
    const std::string title = "  C++ MESSENGER  ";
    int padding = (width - static_cast<int>(title.length())) / 2;
    //> title.length() trả về kiểu size_t (số nguyên không dấu); ép kiểu
    //> sang int để phép trừ với "width" (int) không bị lệch kiểu.
    //> padding = số khoảng trắng cần thêm mỗi bên để tiêu đề nằm giữa màn hình.
    if (padding < 0) padding = 0;
    //> BUG ĐÃ SỬA: nếu terminal hẹp hơn cả title (width < 18), padding sẽ ÂM.
    //> string(count, ch) yêu cầu "count" là size_type (KHÔNG DẤU) — truyền
    //> một int âm vào sẽ bị ngầm ép kiểu thành một số DƯƠNG KHỔNG LỒ, khiến
    //> chương trình cố cấp phát bộ nhớ gần như vô hạn và crash với lỗi
    //> std::length_error ("basic_string::_M_create"). Chặn padding về 0 để
    //> không bao giờ truyền số âm vào constructor của string.
    std::cout << std::string(padding, ' ') << title;
    moveCursor(3, 1);
    std::cout << std::string(width, '=');
    setColor("reset");
}

void drawFooter() {
    const int height = getTerminalHeight();
    const int width = getTerminalWidth();

    moveCursor(height - 3, 1);
    setColor("cyan");
    std::cout << std::string(width, '-');
    setColor("reset");

    moveCursor(height - 2, 1);
    setColor("yellow");
    const std::string commands = "Commands: /help /users /quit";
    int commandsPad = width - static_cast<int>(commands.length());
    if (commandsPad < 0) commandsPad = 0;
    //> BUG ĐÃ SỬA: cùng lý do như drawHeader() — nếu terminal hẹp hơn dòng
    //> lệnh này, phần đệm sẽ ÂM và bị ép thành số khổng lồ khi đưa vào
    //> string(count, ' '), gây crash std::length_error. Chặn về 0.
    //> HIỆN ĐẠI HÓA: lấy độ dài từ chính chuỗi (commands.length()) thay vì
    //> viết cứng số 28 — nếu sau này sửa nội dung dòng lệnh mà quên sửa số
    //> 28 thì phần căn lề sẽ sai, một lỗi rất dễ mắc.
    std::cout << commands << std::string(commandsPad, ' ');
    //> Thêm khoảng trắng phía sau để "xóa" phần chữ cũ còn sót lại trên
    //> dòng đó, vì cout không tự xóa nội dung cũ khi ghi đè.
    setColor("reset");

    moveCursor(height - 1, 1);
    setColor("green");
    int promptPad = width - static_cast<int>(username.length()) - 3;
    if (promptPad < 0) promptPad = 0;
    //> BUG ĐÃ SỬA (đây chính là lỗi crash thực tế đã tái hiện được khi
    //> test): "width - username.length() - 3" trộn int với size_t (không
    //> dấu) nên toàn bộ phép tính được thăng cấp (promote) sang số KHÔNG
    //> DẤU — nếu username dài gần bằng hoặc hơn width, kết quả "ÂM" thật
    //> ra đã underflow thành một số dương khổng lồ NGAY TỪ PHÉP TRỪ, trước
    //> khi kịp truyền vào string(). Ép cả hai vế về int trước rồi mới chặn
    //> về 0 để tránh crash.
    std::cout << username << " > " << std::string(promptPad, ' ');
    setColor("reset");
    std::cout.flush();
    //> cout thường được "đệm" (buffered): dữ liệu có thể tạm giữ trong bộ nhớ
    //> trước khi thực sự hiện ra màn hình. flush() ép xuất ngay lập tức,
    //> quan trọng khi ta cần giao diện cập nhật NGAY (không chờ xuống dòng \n).
}

void drawUI() {
    clearScreen();
    drawHeader();
    drawFooter();
    moveCursor(HEADER_ROWS, 1);
}

void displayMessage(const std::string& msg) {
    std::lock_guard<std::mutex> lock(displayMutex);
    //> std::lock_guard là một "RAII wrapper": ngay khi được tạo ra (constructor),
    //> nó khóa (lock) displayMutex; và khi biến "lock" ra khỏi phạm vi hàm
    //> (kết thúc hàm displayMessage, dù bằng return bình thường hay exception),
    //> destructor của nó TỰ ĐỘNG mở khóa (unlock). Nhờ vậy không cần gọi
    //> unlock() thủ công và không lo quên mở khóa gây "deadlock" (kẹt luồng).

    const int height = getTerminalHeight();
    const int width = getTerminalWidth();
    const int maxMessageRow = height - FOOTER_ROWS;

    if (messageRow >= maxMessageRow) {
        messageRow = HEADER_ROWS;
        // Clear message area
        for (int i = HEADER_ROWS; i < maxMessageRow; i++) {
            moveCursor(i, 1);
            std::cout << std::string(width, ' ');
        }
        //> Khi vùng hiển thị tin nhắn đầy, quay lại dòng 4 và xóa sạch
        //> vùng cũ trước khi in tiếp (thay vì cuộn màn hình).
    }

    moveCursor(messageRow, 1);
    std::cout << std::string(width, ' '); // Clear line
    moveCursor(messageRow, 1);

    // Color code messages
    if (msg.find("[ERROR]") != std::string::npos) {
        //> string::find(...) trả về VỊ TRÍ (index) của chuỗi con nếu tìm thấy,
        //> hoặc hằng số đặc biệt string::npos ("no position") nếu KHÔNG tìm thấy.
        //> So sánh "!= string::npos" tức là "có tìm thấy chuỗi con này".
        setColor("red");
        setColor("bold");
    } else if (msg.find("[SUCCESS]") != std::string::npos) {
        setColor("green");
        setColor("bold");
    } else if (msg.find("[SYSTEM]") != std::string::npos) {
        setColor("yellow");
        setColor("bold");
    } else if (msg.find(username + ":") != std::string::npos) {
        //> username + ":" — toán tử "+" trên std::string thực hiện NỐI CHUỖI
        //> (concatenation), tạo ra một chuỗi mới, không làm thay đổi username.
        setColor("green");
    } else {
        setColor("cyan");
    }

    // Wrap long messages
    const int maxLen = width > 2 ? width - 2 : 1;
    if (static_cast<int>(msg.length()) > maxLen) {
        //> HIỆN ĐẠI HÓA + BUG ĐÃ SỬA: bản cũ so sánh bằng cách ép width sang
        //> size_t: "msg.length() > (size_t)width - 2". Nếu terminal hẹp
        //> (width < 2), phép "(size_t)width - 2" underflow thành số khổng lồ
        //> và điều kiện luôn SAI. Ở đây ta so sánh hoàn toàn bằng số CÓ DẤU
        //> (int) nên phép trừ không bao giờ underflow.
        const int cut = width > 5 ? width - 5 : 0;
        std::cout << msg.substr(0, static_cast<std::size_t>(cut)) << "...";
        //> substr(0, n) trả về một chuỗi con gồm n ký tự đầu tiên của msg.
        //> BUG ĐÃ SỬA: chặn "width - 5" không cho âm — substr() nhận tham số
        //> không dấu nên một số âm sẽ biến thành số khổng lồ (thực tế substr
        //> sẽ cắt tới hết chuỗi, làm vỡ bố cục giao diện trên terminal hẹp).
    } else {
        std::cout << msg;
    }

    setColor("reset");
    messageRow++;

    // Restore cursor to input line
    moveCursor(height - 1, static_cast<int>(username.length()) + 4);
    std::cout.flush();
}

// ============================================================
//> LUỒNG (THREAD) RIÊNG ĐỂ NHẬN TIN NHẮN TỪ SERVER
// ============================================================
void receiveMessages() {
    std::array<char, BUFFER_SIZE> buffer{};
    //> HIỆN ĐẠI HÓA: std::array thay cho mảng C-style "char buffer[4096]".
    //> std::array biết kích thước của chính nó (buffer.size()) nên không
    //> thể truyền nhầm độ dài, không bị "phân rã" thành con trỏ trần khi
    //> truyền vào hàm, và "{}" tự khởi tạo toàn bộ về 0 ngay lúc khai báo.
    std::string recvBuffer;
    //> BUG ĐÃ SỬA — vùng đệm TÍCH LŨY giữa các lần recv(). TCP là một
    //> LUỒNG BYTE liên tục (byte stream), không phải luồng "tin nhắn":
    //> hệ điều hành được phép GỘP nhiều lần send() liên tiếp của server
    //> thành MỘT lần recv() ở đây (đã tái hiện được lỗi này khi test:
    //> "[SUCCESS] Login successful!" và "[SYSTEM] Type /help..." gửi
    //> cách nhau vài micro-giây bị gộp làm một, hiển thị dính liền và bị
    //> cắt bớt bởi logic "Wrap long messages" bên trên) — hoặc ngược lại,
    //> CHIA một tin nhắn dài thành nhiều lần recv() nếu nó vượt quá kích
    //> thước buffer nội bộ của hệ điều hành. Vì vậy không thể coi mỗi lần
    //> recv() là đúng một tin nhắn; phải gom dữ liệu vào recvBuffer rồi
    //> tự tách theo ranh giới do server chèn vào (xem sendToClient() và
    //> broadcastMessage() bên server: mỗi tin nhắn được thêm '\n' ở cuối).

    while (running) {
        //> Vòng lặp chạy liên tục cho tới khi biến atomic "running" thành false
        //> (được đặt từ luồng khác — ví dụ khi người dùng gõ /quit hoặc Ctrl+C).
        buffer.fill('\0');
        //> HIỆN ĐẠI HÓA: buffer.fill('\0') thay cho memset(buffer, 0, sizeof(buffer)).
        //> Cùng tác dụng "xóa sạch vùng đệm" nhưng là hàm thành viên của
        //> std::array nên không thể truyền nhầm kích thước.
        const int bytesReceived = recv(clientSocket, buffer.data(), static_cast<int>(buffer.size()), 0);
        //> recv(...) là hàm socket chuẩn: CHỜ (blocking) cho tới khi có dữ liệu
        //> đến từ server, rồi ghi vào "buffer" tối đa buffer.size() byte,
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

        recvBuffer.append(buffer.data(), static_cast<std::size_t>(bytesReceived));
        //> BUG ĐÃ SỬA: dùng append(dữ_liệu, số_byte) — độ dài TƯỜNG MINH —
        //> thay vì string(buffer) (vốn đọc tới ký tự NULL đầu tiên).
        //> Nếu một lần recv() nhận đủ 4096 byte (lấp đầy nguyên buffer),
        //> có thể không còn byte 0 nào sau dữ liệu thật, khiến string(buffer)
        //> đọc TRÀN ra ngoài mảng để tìm ký tự kết thúc — hành vi không xác
        //> định (undefined behavior). Cách này luôn an toàn vì chỉ đọc đúng
        //> số byte đã thực sự nhận được.

        std::size_t newlinePos;
        while ((newlinePos = recvBuffer.find('\n')) != std::string::npos) {
            //> Mỗi khi tìm thấy '\n' trong recvBuffer, đó là ranh giới kết
            //> thúc của MỘT tin nhắn hoàn chỉnh. Lấy phần trước '\n' ra xử
            //> lý, rồi xóa luôn phần đó (kể cả '\n') khỏi recvBuffer —
            //> phần còn lại (nếu có) là dữ liệu của (một phần) tin nhắn
            //> TIẾP THEO, sẽ tiếp tục được nối thêm ở lần recv() sau. Vòng
            //> while này xử lý HẾT các tin nhắn trọn vẹn đang có trong
            //> buffer trước khi gọi recv() lần nữa, phòng trường hợp một
            //> lần recv() gộp sẵn nhiều tin nhắn.
            const std::string msg = recvBuffer.substr(0, newlinePos);
            recvBuffer.erase(0, newlinePos + 1);

            // Check for successful login
            if (msg.find("[SUCCESS] Login successful") != std::string::npos) {
                authenticated = true;
            }

            // Display message during authentication phase
            if (!authenticated) {
                setColor("yellow");
                std::cout << msg << '\n';
                setColor("reset");
                std::cout.flush();
                //> HIỆN ĐẠI HÓA: '\n' + flush() tường minh thay cho std::endl.
                //> endl = '\n' + flush gộp lại; tách ra giúp thấy rõ chỗ nào
                //> thực sự CẦN đẩy dữ liệu ra màn hình ngay (ở đây là cần,
                //> vì giao diện phải hiện lập tức).
            } else {
                displayMessage(msg);
            }
        }
    }
}

// ============================================================
//> XỬ LÝ TÍN HIỆU NGẮT (Ctrl+C) — mỗi hệ điều hành có cơ chế riêng
// ============================================================
#if defined(WINDOWS_BUILD) || defined(_WIN32)
BOOL WINAPI ConsoleHandler(DWORD signal) {
    //> Đây là "hàm callback": ta không tự gọi hàm này, mà đăng ký nó với
    //> hệ điều hành (qua SetConsoleCtrlHandler bên dưới trong main), và
    //> Windows sẽ TỰ GỌI hàm này khi người dùng nhấn Ctrl+C.
    //> BOOL, WINAPI, DWORD là các kiểu/quy ước gọi hàm riêng của Windows API.
    if (signal == CTRL_C_EVENT) {
        running = false;
        closeSocket(clientSocket);
        WSACleanup();
        //> WSACleanup() giải phóng tài nguyên mà WSAStartup() đã cấp phát
        //> lúc khởi động Winsock (xem trong main()).
        clearScreen();
        setColor("yellow");
        std::cout << "\n[*] Disconnected from server. Goodbye!\n";
        setColor("reset");
        std::cout.flush();
        std::exit(0);
        //> exit(0) thoát chương trình ngay lập tức với mã trả về 0 (thành công),
        //> khác với "return" vì có thể gọi từ bất kỳ đâu, không chỉ trong main().
    }
    return TRUE;
}
#else
extern "C" void signalHandler(int /*signal*/) {
    //> Tương tự ConsoleHandler ở trên nhưng theo API chuẩn POSIX (Linux/macOS):
    //> hàm này được đăng ký bằng sigaction() trong main(), và hệ điều hành
    //> tự gọi nó khi nhận tín hiệu SIGINT (Ctrl+C).
    //> HIỆN ĐẠI HÓA: bỏ tên tham số (chỉ để lại /*signal*/ trong comment) vì
    //> ta không dùng tới nó — cách này khiến trình biên dịch không còn cảnh
    //> báo "tham số không được sử dụng" mà vẫn giữ đúng chữ ký hàm.
    running = false;
    //> HIỆN ĐẠI HÓA QUAN TRỌNG — AN TOÀN TÍN HIỆU: bản cũ gọi std::cout,
    //> clearScreen() và exit() ngay trong signal handler. Đó là các hàm
    //> KHÔNG "async-signal-safe": tín hiệu có thể ập đến đúng lúc luồng
    //> khác đang ở giữa một thao tác cout hoặc đang giữ khóa nội bộ của
    //> thư viện, và gọi lại chúng từ handler có thể gây kẹt (deadlock)
    //> hoặc hỏng dữ liệu. Chuẩn POSIX chỉ cho phép dùng một danh sách hàm
    //> hạn chế — write() và _exit() nằm trong danh sách đó, còn printf/
    //> cout/exit thì không.
    const char msg[] = "\033[2J\033[1;1H\033[33m\n[*] Disconnected from server. Goodbye!\n\033[0m";
    const ssize_t ignored = write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    //> write() ghi thẳng xuống file descriptor 1 (màn hình), không qua bộ
    //> đệm của iostream. Chuỗi trên chính là các mã ANSI mà clearScreen()
    //> và setColor("yellow") vẫn dùng, ghép sẵn thành một lần ghi.
    (void)ignored;
    //> (void)ignored — nói rõ "tôi cố ý bỏ qua giá trị trả về", vì trong
    //> handler ta không thể làm gì thêm nếu write() lỗi.
    _exit(0);
    //> _exit() thoát NGAY mà không chạy các destructor / hàm dọn dẹp của
    //> thư viện chuẩn — đúng thứ ta cần trong signal handler, vì chạy chúng
    //> ở đây mới là thứ có thể gây kẹt.
}
#endif

// ============================================================
//> HÀM MAIN — điểm bắt đầu chạy chương trình
// ============================================================
int main() {
#if defined(WINDOWS_BUILD) || defined(_WIN32)
    // Initialize Winsock
    WSADATA wsaData;
    //> Trên Windows, TRƯỚC KHI dùng bất kỳ hàm socket nào, bắt buộc phải
    //> khởi tạo thư viện Winsock bằng WSAStartup. wsaData sẽ được điền
    //> thông tin về phiên bản Winsock đang dùng.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        //> MAKEWORD(2, 2) yêu cầu Winsock phiên bản 2.2.
        //> WSAStartup trả về 0 nếu thành công, khác 0 nếu lỗi.
        std::cerr << "WSAStartup failed!\n";
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
    struct sigaction sa{};
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, nullptr);
    //> HIỆN ĐẠI HÓA: sigaction() thay cho signal(SIGINT, signalHandler).
    //> Hàm signal() cũ có hành vi KHÁC NHAU giữa các hệ điều hành (có nơi
    //> tự gỡ handler sau lần đầu kích hoạt, có nơi không; cách xử lý các
    //> lời gọi hệ thống bị ngắt cũng khác nhau), nên chuẩn POSIX khuyến
    //> nghị dùng sigaction() — hành vi được định nghĩa rõ ràng và cho phép
    //> kiểm soát chi tiết (mặt nạ tín hiệu sa_mask, cờ sa_flags).
#endif

    clearScreen();
    setColor("cyan");
    setColor("bold");
    std::cout << "\n+========================================+\n";
    std::cout << "|   C++ Messenger Client v2.0            |\n";
    std::cout << "|   With User Authentication             |\n";
#ifdef WINDOWS_BUILD
    std::cout << "|        Windows Build                   |\n";
#else
    std::cout << "|        Linux/Unix Build                |\n";
#endif
    std::cout << "+========================================+\n\n";
    setColor("reset");

    setColor("yellow");
    std::cout << "Connecting to server...\n";
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
        std::cerr << "Error creating socket!\n";
        setColor("reset");
#ifdef WINDOWS_BUILD
        WSACleanup();
#endif
        return 1;
    }

    // --------------------------------------------------------
    //> CHUẨN BỊ ĐỊA CHỈ SERVER CẦN KẾT NỐI TỚI
    sockaddr_in serverAddress{};
    //> struct sockaddr_in — cấu trúc chuẩn để mô tả một địa chỉ mạng IPv4,
    //> gồm: họ địa chỉ (family), số hiệu cổng (port), địa chỉ IP...
    //> HIỆN ĐẠI HÓA: "{}" (value initialization) tự đặt TOÀN BỘ struct về 0
    //> ngay lúc khai báo, thay cho lời gọi memset(&serverAddress, 0, ...)
    //> riêng ở dòng sau. Cùng tác dụng "tránh giá trị rác còn sót", nhưng
    //> gắn liền với khai báo nên không thể quên, và không cần dùng tới hàm
    //> memset của thư viện C.
    serverAddress.sin_family = AF_INET;      //> họ địa chỉ: IPv4
    serverAddress.sin_port = htons(SERVER_PORT);
    //> htons = "host to network short": chuyển số port từ thứ tự byte của
    //> máy tính hiện tại (host byte order) sang thứ tự byte chuẩn mạng
    //> (network byte order — luôn là big-endian). Bắt buộc phải gọi vì
    //> các máy khác nhau có thể lưu số nguyên theo thứ tự byte khác nhau.
    if (inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr) <= 0) {
        //> inet_pton ("presentation to network") chuyển chuỗi địa chỉ IP
        //> dạng con người đọc được ("127.0.0.1" = địa chỉ loopback, tức
        //> chính máy đang chạy chương trình) sang dạng nhị phân mà struct
        //> sockaddr_in cần, rồi ghi kết quả vào &serverAddress.sin_addr.
        //> Trả về giá trị <= 0 nghĩa là chuỗi địa chỉ không hợp lệ.
        setColor("red");
        std::cerr << "Invalid address: " << SERVER_IP << '\n';
        setColor("reset");
        //> HIỆN ĐẠI HÓA: dùng std::cerr thay cho perror(). perror() in ra
        //> mô tả của biến errno — nhưng inet_pton() KHÔNG đặt errno khi
        //> chuỗi địa chỉ sai định dạng (nó chỉ trả về 0), nên thông báo của
        //> perror ở đây rất dễ gây hiểu nhầm (thường in ra "Success").
        closeSocket(clientSocket);
#ifdef WINDOWS_BUILD
        WSACleanup();
#endif
        return 1;
    }

    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        //> connect() thực sự thiết lập kết nối TCP tới server.
        //> reinterpret_cast<sockaddr*>(&serverAddress) — ép kiểu con trỏ:
        //> hàm connect() theo chuẩn C nhận tham số kiểu "struct sockaddr*"
        //> chung chung, nên ta phải "diễn giải lại" con trỏ sockaddr_in*
        //> (cụ thể cho IPv4) thành kiểu tổng quát hơn sockaddr* mà hàm yêu cầu.
        //> HIỆN ĐẠI HÓA: reinterpret_cast nói rõ đây là phép ép kiểu "mạnh",
        //> dễ nhận ra khi đọc và dễ tìm kiếm, thay cho "(struct sockaddr*)".
        setColor("red");
        std::cerr << "Error connecting to server!\n";
        setColor("reset");
        closeSocket(clientSocket);
#ifdef WINDOWS_BUILD
        WSACleanup();
#endif
        return 1;
    }

    setColor("green");
    std::cout << "Connected to server!\n\n";
    setColor("reset");

    // --------------------------------------------------------
    //> TẠO LUỒNG (THREAD) RIÊNG ĐỂ LẮNG NGHE TIN NHẮN TỪ SERVER,
    //> chạy song song với luồng chính (đang xử lý nhập liệu từ bàn phím).
    std::thread receiveThread(receiveMessages);
    //> Tạo một std::thread mới chạy hàm receiveMessages() ngay lập tức trên
    //> một luồng hệ điều hành riêng biệt.
    //>
    //> HIỆN ĐẠI HÓA / SỬA DATA RACE: bản cũ gọi ngay .detach() để "thả" luồng
    //> chạy độc lập. Nhưng luồng đó dùng chung biến clientSocket với luồng
    //> chính, mà khi thoát chương trình, luồng chính lại đóng socket đó —
    //> trong khi luồng nhận tin CÒN ĐANG bị chặn bên trong recv() trên chính
    //> socket ấy (ThreadSanitizer đã bắt được đúng lỗi này). Đóng một socket
    //> đang được luồng khác sử dụng là hành vi không xác định, và nguy hiểm
    //> hơn nữa: hệ điều hành có thể cấp lại ngay số hiệu (fd) vừa giải phóng
    //> cho một file/kết nối mới, khiến luồng kia vô tình đọc nhầm tài nguyên
    //> hoàn toàn khác. Giữ luồng ở dạng "join được" cho phép luồng chính CHỜ
    //> nó kết thúc hẳn rồi mới đóng socket — xem phần dọn dẹp cuối main().

    // Wait for welcome message
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    //> HIỆN ĐẠI HÓA: std::this_thread::sleep_for + std::chrono thay cho cặp
    //> #ifdef Sleep(200) / usleep(200000). Bản cũ phải viết 2 nhánh vì
    //> Sleep() của Windows tính bằng MILLI-giây còn usleep() của Linux tính
    //> bằng MICRO-giây — rất dễ nhầm lẫn sai 1000 lần. sleep_for() là hàm
    //> chuẩn C++11, chạy trên mọi hệ điều hành, và đơn vị được ghi rõ ngay
    //> trong code (milliseconds) nên không thể hiểu nhầm. Nhờ vậy 3 cặp
    //> #ifdef trong hàm main đã được xóa bỏ hoàn toàn.
    //> (usleep() còn bị chuẩn POSIX đánh dấu là lỗi thời/obsolete từ 2008.)

    // --------------------------------------------------------
    //> VÒNG LẶP ĐĂNG NHẬP: lặp lại việc đọc lệnh từ bàn phím và gửi cho
    //> server, cho tới khi biến "authenticated" chuyển thành true.
    std::string input;
    while (!authenticated && running) {
        setColor("cyan");
        std::cout << "Enter command: ";
        setColor("reset");
        std::cout.flush();
        if (!std::getline(std::cin, input)) {
            //> getline(cin, input) đọc TOÀN BỘ MỘT DÒNG người dùng gõ (kể cả
            //> khoảng trắng) vào biến input, khác với "cin >> input" chỉ đọc
            //> tới khoảng trắng đầu tiên.
            //> HIỆN ĐẠI HÓA + AN TOÀN: kiểm tra giá trị trả về của getline.
            //> Bản cũ bỏ qua nó, nên khi luồng nhập bị đóng (ví dụ người dùng
            //> nhấn Ctrl+D, hoặc đầu vào được nạp từ file đã hết), cin rơi
            //> vào trạng thái lỗi và getline lập tức trả về mà không đọc gì —
            //> khiến vòng while quay vòng vô tận, ngốn 100% CPU.
            running = false;
            break;
        }

        if (!input.empty()) {
            send(clientSocket, input.c_str(), static_cast<int>(input.length()), 0);
            //> send() gửi dữ liệu qua socket. input.c_str() chuyển
            //> std::string sang con trỏ chuỗi kiểu C (const char*) mà
            //> hàm send() (viết bằng C) yêu cầu; static_cast<int> ép
            //> kiểu độ dài chuỗi (size_t) sang int cho khớp tham số hàm.

            // Wait for server response
            std::this_thread::sleep_for(std::chrono::milliseconds(300));

            // Check if authentication succeeded and extract username
            if (authenticated) {
                //> Lưu ý: biến "authenticated" được cập nhật bởi LUỒNG KHÁC
                //> (receiveMessages ở trên) khi nhận được thông báo đăng nhập
                //> thành công từ server — đây là lý do cần kiểu atomic<bool>,
                //> để đọc giá trị mới nhất một cách an toàn giữa hai luồng.
                const std::size_t firstSpace = input.find(' ');
                const std::size_t secondSpace = input.find(' ', firstSpace + 1);
                //> find(' ', firstSpace + 1) — bắt đầu tìm khoảng trắng
                //> từ vị trí (firstSpace + 1) trở đi, tức là tìm khoảng
                //> trắng THỨ HAI (bỏ qua khoảng trắng thứ nhất).
                //> Ví dụ input = "LOGIN alice pass123" thì lệnh này tách ra
                //> được username nằm giữa khoảng trắng thứ 1 và thứ 2.
                if (firstSpace != std::string::npos && secondSpace != std::string::npos) {
                    std::lock_guard<std::mutex> lock(displayMutex);
                    username = input.substr(firstSpace + 1, secondSpace - firstSpace - 1);
                    //> substr(vị_trí_bắt_đầu, số_ký_tự) trích chuỗi con:
                    //> lấy phần nằm giữa 2 khoảng trắng — chính là username.
                    //> BUG ĐÃ SỬA (data race — ThreadSanitizer phát hiện được):
                    //> luồng chính GHI vào biến toàn cục "username" ngay tại đây,
                    //> trong khi luồng receiveMessages có thể đang ĐỌC chính biến
                    //> đó bên trong displayMessage() để tô màu và căn lề. Luồng
                    //> đọc vốn đã giữ displayMutex, nhưng luồng ghi thì không —
                    //> mà một khóa chỉ có tác dụng khi MỌI bên truy cập đều dùng
                    //> nó. Ghi std::string không phải thao tác nguyên tử (nó sửa
                    //> cả con trỏ dữ liệu lẫn độ dài), nên luồng kia có thể đọc
                    //> trúng trạng thái dang dở → hành vi không xác định.
                }

                // Small delay before switching to UI mode
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
                //> Thoát vòng lặp đăng nhập, chuyển sang giao diện chat chính.
            }
        }
    }

    if (!authenticated) {
        //> Nếu thoát vòng lặp trên mà VẪN chưa đăng nhập được (ví dụ mất
        //> kết nối khiến running = false), thì dừng chương trình luôn.
        running = false;
        shutdownSocket(clientSocket);
        if (receiveThread.joinable()) receiveThread.join();
        //> Cùng trình tự dọn dẹp an toàn như ở cuối main(): đánh thức luồng
        //> nhận tin bằng shutdown(), chờ nó kết thúc hẳn, rồi mới đóng socket.
        closeSocket(clientSocket);
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
    std::string message;
    while (running) {
        // Position cursor at input line
        const int height = getTerminalHeight();
        moveCursor(height - 1, static_cast<int>(username.length()) + 4);

        if (!std::getline(std::cin, message)) {
            //> Kiểm tra giá trị trả về của getline vì lý do đã giải thích ở
            //> vòng lặp đăng nhập phía trên (tránh lặp vô tận khi hết đầu vào).
            running = false;
            break;
        }

        if (!message.empty()) {
            if (message == "/quit") {
                running = false;
                send(clientSocket, message.c_str(), static_cast<int>(message.length()), 0);
                break;
            } else if (message == "/clear") {
                {
                    std::lock_guard<std::mutex> lock(displayMutex);
                    messageRow = HEADER_ROWS;
                }
                //> BUG ĐÃ SỬA: bản cũ gán messageRow = 4 mà KHÔNG giữ khóa
                //> displayMutex, trong khi luồng receiveMessages có thể đang
                //> đọc/tăng chính biến này bên trong displayMessage() —
                //> một data race thực sự. Đặt phép gán vào trong một phạm vi
                //> {} có khóa để hai luồng không đụng nhau; khóa được nhả
                //> ngay sau đó để drawUI() bên dưới không bị kẹt.
                drawUI();
            } else {
                send(clientSocket, message.c_str(), static_cast<int>(message.length()), 0);

                // Clear the input line after sending
                std::lock_guard<std::mutex> lock(displayMutex);
                //> Khóa displayMutex ở đây vì luồng chính đang chuẩn bị vẽ
                //> lại dòng nhập lệnh, trong khi luồng receiveMessages có
                //> thể đồng thời đang gọi displayMessage() để vẽ tin nhắn
                //> mới — khóa này ngăn 2 luồng vẽ màn hình chồng lên nhau.
                moveCursor(height - 1, 1);
                setColor("green");
                int promptPad = getTerminalWidth() - static_cast<int>(username.length()) - 3;
                if (promptPad < 0) promptPad = 0;
                //> BUG ĐÃ SỬA: chặn số đệm âm như đã giải thích trong
                //> drawFooter(), để tránh crash std::length_error khi
                //> terminal bị thu hẹp hoặc username dài.
                std::cout << username << " > " << std::string(promptPad, ' ');
                setColor("reset");
                moveCursor(height - 1, static_cast<int>(username.length()) + 4);
                std::cout.flush();
            }
            //> Lưu ý: biến "lock" (lock_guard) được khai báo BÊN TRONG khối
            //> else {...}, nên nó tự động unlock ngay khi khối else kết thúc
            //> (hết phạm vi / scope) — không cần gọi unlock() thủ công.
        }
    }

    // --------------------------------------------------------
    //> DỌN DẸP TRƯỚC KHI THOÁT CHƯƠNG TRÌNH
    running = false;
    //> 1) Báo cho luồng nhận tin biết là sắp dừng.
    shutdownSocket(clientSocket);
    //> 2) Đánh thức luồng đó nếu nó đang bị chặn trong recv(): shutdown()
    //>    khiến recv() trả về 0 ngay, luồng thoát vòng lặp và kết thúc.
    if (receiveThread.joinable()) receiveThread.join();
    //> 3) CHỜ luồng kết thúc hoàn toàn. Đây là bước then chốt để sửa data
    //>    race mà ThreadSanitizer phát hiện: chỉ khi chắc chắn không còn
    //>    luồng nào đụng tới socket nữa thì mới được phép đóng nó.
    //>    joinable() kiểm tra luồng có đang ở trạng thái chờ join hay không
    //>    — gọi join() hai lần trên cùng một luồng sẽ làm chương trình sập.
    closeSocket(clientSocket);
    //> 4) Giờ mới đóng socket, khi đã không còn ai dùng tới nó.
#ifdef WINDOWS_BUILD
    WSACleanup();
#endif
    clearScreen();
    setColor("yellow");
    std::cout << "\n[*] Disconnected from server. Goodbye!\n";
    setColor("reset");
    std::cout.flush();

    return 0;
    //> return 0 trong main() theo quy ước nghĩa là "chương trình kết thúc
    //> bình thường, không có lỗi".
}