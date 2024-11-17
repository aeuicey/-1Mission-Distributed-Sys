#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib> // for system()
#include <sstream> // 添加这个头文件
#include <cstring> // for memset
#include <limits> // for std::numeric_limits
#include <codecvt> // for std::wstring_convert
#include <locale> // for std::locale

// 学员结构
struct Student {
    std::wstring name;
    int score = 0; // 总评分
    int completedTasks = 0; // 完成的任务数量
};

// 任务结构
struct Task {
    int id = 0; // 任务ID，默认值为0
    std::wstring name; // 任务名
    std::vector<std::wstring> assignees; // 分配给哪些学员
    bool isCompleted = false; // 任务是否完成
    int rating = -1; // 任务评分
};

// 全局变量
std::vector<Student> students; // 学员列表
std::vector<Task> tasks; // 任务列表
std::wstring password; // 密码
int nextTaskId = 1000; // 下一个任务ID
bool authenticated = false; // 是否已通过身份验证

// 加密密钥
const wchar_t* encryptionKey = L"secretkey";

// XOR 加密函数
std::wstring xorEncryptDecrypt(const std::wstring& input) {
    std::wstring output = input;
    size_t keyLen = wcslen(encryptionKey);
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = input[i] ^ encryptionKey[i % keyLen];
    }
    return output;
}

// 显示菜单
void showMenu() {
    std::wcout << L"请选择操作：\n"
        << L"1 - 发布任务\n"
        << L"2 - 分配任务\n"
        << L"3 - 浏览任务\n"
        << L"4 - 评分\n"
        << L"5 - 学员排名\n"
        << L"6 - 重置密码\n"
        << L"7 - 退出\n"
        << L"8 - 按空格键回到主菜单\n"
        << L"9 - 手动刷新屏幕\n";
}

// 密码检查
bool checkPassword() {
    std::wstring input;
    std::wcout << L"请输入密码: ";
    std::wcin >> input;
    if (input == password) {
        system("cls"); // 输入密码正确后自动刷新屏幕
        return true;
    }
    return false;
}

// 设置密码
void setupPassword() {
    std::wstring input;
    while (true) {
        std::wcout << L"设置新密码: ";
        std::wcin >> input;
        if (input.find_first_not_of(L' ') != std::wstring::npos) {
            password = input;
            authenticated = false; // 重置密码后需要重新验证
            break;
        }
        else {
            std::wcout << L"密码不能仅为空格，请重新输入。\n";
        }
    }
}

// 添加学生
void addStudents() {
    int count;
    std::wcout << L"请输入学员总数: ";
    std::wcin >> count;
    for (int i = 0; i < count; ++i) {
        std::wstring name;
        while (true) {
            std::wcout << L"请输入第" << i + 1 << L"位学员的名字: ";
            std::wcin >> name;
            if (name.find_first_not_of(L' ') != std::wstring::npos) {
                students.push_back({ name });
                break;
            }
            else {
                std::wcout << L"名字不能仅为空格，请重新输入。\n";
            }
        }
    }
}

// 发布任务
void publishTask() {
    std::wstring name;
    while (true) {
        std::wcout << L"请输入任务名称: ";
        std::wcin.ignore();
        std::getline(std::wcin, name);
        if (name.find_first_not_of(L' ') != std::wstring::npos) {
            tasks.push_back({ nextTaskId++, name, {}, false, -1 });
            std::wcout << L"任务发布成功，任务ID为: " << tasks.back().id << L"\n";
            break;
        }
        else {
            std::wcout << L"任务名称不能仅为空格，请重新输入。\n";
        }
    }
}

// 手动分配任务
void assignTask() {
    std::wcout << L"已发布的任务:\n";
    for (const auto& task : tasks) {
        std::wcout << L"ID: " << task.id << L", 名称: " << task.name << L"\n";
    }

    int id;
    std::wstring assignee;
    std::wcout << L"请输入要分配的任务ID: ";
    std::wcin >> id;
    auto task = std::find_if(tasks.begin(), tasks.end(), [id](const Task& t) { return t.id == id; });
    if (task != tasks.end()) {
        while (true) {
            std::wcout << L"请输入分配给哪个学员: ";
            std::wcin >> assignee;
            if (assignee.find_first_not_of(L' ') != std::wstring::npos) {
                if (std::any_of(students.begin(), students.end(), [assignee](const Student& s) { return s.name == assignee; })) {
                    task->assignees.push_back(assignee);
                    std::wcout << L"任务分配成功。\n";
                    break;
                }
                else {
                    std::wcout << L"未找到该学员。\n";
                }
            }
            else {
                std::wcout << L"学员名字不能仅为空格，请重新输入。\n";
            }
        }
    }
    else {
        std::wcout << L"未找到该任务。\n";
    }
}

// 自动分配任务
void autoAssignTask() {
    std::wcout << L"已发布的任务:\n";
    for (const auto& task : tasks) {
        std::wcout << L"ID: " << task.id << L", 名称: " << task.name << L"\n";
    }

    int id, numAssignees;
    std::wcout << L"请输入要分配的任务ID: ";
    std::wcin >> id;
    auto task = std::find_if(tasks.begin(), tasks.end(), [id](const Task& t) { return t.id == id; });
    if (task == tasks.end()) {
        std::wcout << L"未找到该任务。\n";
        return;
    }

    std::wcout << L"请输入任务需要的人数: ";
    std::wcin >> numAssignees;

    std::vector<Student> sortedStudents = students;
    std::sort(sortedStudents.begin(), sortedStudents.end(), [](const Student& a, const Student& b) {
        if (a.completedTasks == b.completedTasks) {
            return a.score < b.score;
        }
        return a.completedTasks < b.completedTasks;
        });

    for (int i = 0; i < numAssignees && i < sortedStudents.size(); ++i) {
        task->assignees.push_back(sortedStudents[i].name);
        std::wcout << L"任务 " << task->id << L" 已自动分配给 " << sortedStudents[i].name << L"。\n";
    }
}

// 浏览任务
void listTasks() {
    std::wcout << L"进行中的任务:\n";
    for (const auto& task : tasks) {
        if (!task.isCompleted) {
            std::wcout << L"ID: " << task.id << L", 名称: " << task.name
                << L", 分配给: ";
            for (const auto& assignee : task.assignees) {
                std::wcout << assignee << L" ";
            }
            std::wcout << L"\n";
        }
    }
    std::wcout << L"已完成的任务:\n";
    for (const auto& task : tasks) {
        if (task.isCompleted) {
            std::wcout << L"ID: " << task.id << L", 名称: " << task.name
                << L", 完成人: ";
            for (const auto& assignee : task.assignees) {
                std::wcout << assignee << L" ";
            }
            std::wcout << L", 评分: " << (task.rating == -1 ? L"未评分" : std::to_wstring(task.rating)) << L"\n";
        }
    }
}

// 评分
void rateTask() {
    std::wcout << L"正在进行的任务:\n";
    for (const auto& task : tasks) {
        if (!task.isCompleted) {
            std::wcout << L"ID: " << task.id << L", 名称: " << task.name << L", 分配给: ";
            for (const auto& assignee : task.assignees) {
                std::wcout << assignee << L" ";
            }
            std::wcout << L"\n";
        }
    }

    int id, score;
    std::wcout << L"请输入要评分的任务ID: ";
    std::wcin >> id;
    auto task = std::find_if(tasks.begin(), tasks.end(), [id](const Task& t) { return t.id == id; });
    if (task != tasks.end() && !task->isCompleted) {
        std::wcout << L"请输入评分(0-100): ";
        std::wcin >> score;
        if (score >= 0 && score <= 100) {
            task->isCompleted = true;
            task->rating = score;
            for (const auto& assignee : task->assignees) {
                auto student = std::find_if(students.begin(), students.end(), [&assignee](const Student& s) { return s.name == assignee; });
                if (student != students.end()) {
                    student->score += score;
                    student->completedTasks++;
                }
            }
            std::wcout << L"评分成功。\n";
        }
        else {
            std::wcout << L"评分范围错误。\n";
        }
    }
    else {
        std::wcout << L"未找到该任务或任务已完成。\n";
    }
}

// 学员排名
void rankStudents() {
    std::vector<Student> sortedStudents = students;
    std::sort(sortedStudents.begin(), sortedStudents.end(), [](const Student& a, const Student& b) {
        if (a.score == b.score) {
            return a.completedTasks > b.completedTasks;
        }
        return a.score > b.score;
        });

    std::wcout << L"学员排名:\n";
    for (size_t i = 0; i < sortedStudents.size(); ++i) {
        std::wcout << i + 1 << L". " << sortedStudents[i].name << L": 评分 " << sortedStudents[i].score
            << L", 完成任务数量 " << sortedStudents[i].completedTasks << L"\n";
    }
}

// 保存配置
void saveConfig() {
    std::wofstream config(L"config.txt", std::ios::binary);
    if (config) {
        std::wstring encryptedPassword = xorEncryptDecrypt(password);
        config << L"PASSWORD " << encryptedPassword << L"\n";
        config << L"NEXT_TASK_ID " << nextTaskId << L"\n"; 
        config << L"STUDENTS " << students.size() << L"\n";
        for (const auto& s : students) {
            std::wstring encryptedName = xorEncryptDecrypt(s.name);
            config << L"STUDENT " << encryptedName << L" " << s.score << L" " << s.completedTasks << L"\n";
        }
        config << L"TASKS " << tasks.size() << L"\n";
        for (const auto& t : tasks) {
            std::wstring encryptedName = xorEncryptDecrypt(t.name);
            config << L"TASK " << t.id << L" " << encryptedName << L" " << t.isCompleted << L" " << t.rating << L" " << t.assignees.size();
            for (const auto& assignee : t.assignees) {
                std::wstring encryptedAssignee = xorEncryptDecrypt(assignee);
                config << L" " << encryptedAssignee;
            }
            config << L"\n";
        }
    }
}

// 加载配置
bool loadConfig() {
    std::wifstream config(L"config.txt", std::ios::binary);
    if (config) {
        std::wstring line;
        while (std::getline(config, line)) {
            std::wistringstream iss(line);
            std::wstring tag;
            iss >> tag;

            if (tag == L"PASSWORD") {
                iss >> password;
                password = xorEncryptDecrypt(password);
            }
            else if (tag == L"NEXT_TASK_ID") {
                iss >> nextTaskId;
            }
            else if (tag == L"STUDENTS") {
                size_t studentCount;
                iss >> studentCount;
                for (size_t i = 0; i < studentCount; ++i) {
                    std::getline(config, line);
                    std::wistringstream studentLine(line);
                    std::wstring studentTag;
                    studentLine >> studentTag;
                    if (studentTag == L"STUDENT") {
                        Student s;
                        std::wstring encryptedName;
                        studentLine >> encryptedName >> s.score >> s.completedTasks;
                        s.name = xorEncryptDecrypt(encryptedName);
                        students.push_back(s);
                    }
                }
            }
            else if (tag == L"TASKS") {
                size_t taskCount;
                iss >> taskCount;
                for (size_t i = 0; i < taskCount; ++i) {
                    std::getline(config, line);
                    std::wistringstream taskLine(line);
                    std::wstring taskTag;
                    taskLine >> taskTag;
                    if (taskTag == L"TASK") {
                        Task t;
                        size_t assigneeCount;
                        std::wstring encryptedName;
                        taskLine >> t.id >> encryptedName >> t.isCompleted >> t.rating >> assigneeCount;
                        t.name = xorEncryptDecrypt(encryptedName);
                        for (size_t j = 0; j < assigneeCount; ++j) {
                            std::wstring encryptedAssignee;
                            taskLine >> encryptedAssignee;
                            t.assignees.push_back(xorEncryptDecrypt(encryptedAssignee));
                        }
                        tasks.push_back(t);
                    }
                }
            }
        }
        return true;
    }
    return false;
}

// 检查输入是否为空格
bool isInputBlank(const std::wstring& input) {
    return input.find_first_not_of(L' ') == std::wstring::npos;
}

int main() {
    // 设置本地化
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    std::wcin.imbue(std::locale());

    // 初始化
    if (!loadConfig()) {
        setupPassword();
        addStudents();
    }

    // 验证密码
    while (!authenticated) {
        if (checkPassword()) {
            authenticated = true;
        }
        else {
            std::wcout << L"密码错误，请重试。\n";
        }
    }

    while (true) {
        showMenu();
        int choice;
        std::wcin >> choice;

        // 检查输入是否为空格
        if (std::wcin.fail() || isInputBlank(std::to_wstring(choice))) {
            std::wcin.clear(); // 清除错误状态
            std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n'); // 忽略剩余输入
            continue; // 回到主菜单
        }

        switch (choice) {
        case 7:
            saveConfig(); // 在退出前保存配置
            return 0; // 退出
        case 8: // 按空格键回到主菜单
            std::wcout << L"按空格键回到主菜单...";
            wchar_t c;
            std::wcin.get(c); // 读取一个字符
            while (c != L' ') {
                std::wcin.get(c); // 继续读取直到空格键被按下
            }
            continue; // 回到主菜单
        case 9: // 手动刷新屏幕
            system("cls"); // 手动刷新屏幕
            continue; // 回到主菜单
        case 6: // 重置密码
            setupPassword();
            break;
        case 1: // 发布任务
            publishTask();
            break;
        case 2: // 分配任务
            std::wcout << L"选择分配模式：\n"
                << L"1 - 手动分配\n"
                << L"2 - 自动分配\n";
            int mode;
            std::wcin >> mode;
            if (mode == 1) {
                assignTask();
            }
            else if (mode == 2) {
                autoAssignTask();
            }
            else {
                std::wcout << L"无效的选择。\n";
            }
            break;
        case 3: // 浏览任务
            listTasks();
            break;
        case 4: // 评分
            rateTask();
            break;
        case 5: // 学员排名
            rankStudents();
            break;
        default:
            std::wcout << L"无效的选择，请重新输入！\n";
        }
        saveConfig(); // 每步操作后保存
        std::wcout << L"按回车键继续...";
        std::wcin.ignore(); // 忽略之前的换行符
        std::wcin.get(); // 等待用户按回车键
        system("cls"); // 刷新屏幕
    }

    return 0;
}