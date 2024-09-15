#Developed by Bootkitt

#include <Windows.h>
#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include "auth.hpp"
#include "utils.hpp"
#include "skStr.h"

namespace fs = std::filesystem;

std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);

const std::string compilation_date = xorstr_(__DATE__);
const std::string compilation_time = xorstr_(__TIME__);

bool AddRegistryValue(const std::wstring& keyPath, const std::wstring& valueName, DWORD valueData) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyPath.c_str(), 0, KEY_SET_VALUE, &hKey);
    if (result == ERROR_SUCCESS) {
        result = RegSetValueExW(hKey, valueName.c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&valueData), sizeof(valueData));
        RegCloseKey(hKey);
    }
    return result == ERROR_SUCCESS;
}

void TaskKill(const std::wstring& processName, int times) {
    for (int i = 0; i < times; ++i) {
        std::wstring command = xorstr_(L"taskkill /F /IM ") + processName;
        _wsystem(command.c_str());
    }
}

void HideRegistryOperation() {
    std::wstring command = xorstr_(L"reg add HKLM\\SYSTEM\\CurrentControlSet\\Control\\WMI\\Restrictions /v HideMachine /t REG_DWORD /d 1 /F");
    _wsystem(command.c_str());
}

using namespace KeyAuth;

std::string name = xorstr_("Projectid");
std::string ownerid = xorstr_("Ownerid");
std::string secret = xorstr_("YourSecret");
std::string version = xorstr_("1.0");
std::string url = xorstr_("https://keyauth.win/api/1.2/");
std::string path = xorstr_("");

api KeyAuthApp(name, ownerid, secret, version, url, path);

void SetConsoleColors() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hConsole, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, dwMode);
}

void OpenURL(const std::wstring& url) {
    HINSTANCE result = ShellExecuteW(NULL, xorstr_(L"open"), url.c_str(), NULL, NULL, SW_SHOWNORMAL);

    if ((int)result <= 32) {
        std::wcerr << xorstr_(L"Failed to open URL. Error code: ") << (int)result << std::endl;
    }
}

bool DeletePath(const fs::path& path) {
    try {
        if (fs::exists(path)) {
            if (fs::is_directory(path)) {
                fs::remove_all(path);
            }
            else {
                fs::remove(path);
            }
            return true;
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << xorstr_("Error deleting ") << path << ": " << e.what() << std::endl;
    }
    return false;
}

int main()
{
    // Initialize console to support ANSI codes
    SetConsoleColors();

    // Freeing memory to prevent memory leak or memory scraping
    name.clear(); ownerid.clear(); secret.clear(); version.clear(); url.clear();
    std::string consoleTitle = xorstr_("Loader - Built at: ") + compilation_date + " " + compilation_time;
    SetConsoleTitleA(consoleTitle.c_str());

    const std::string greenColor = "\033[32m";
    const std::string redColor = "\033[31m";
    const std::string resetColor = "\033[0m";

    std::cout << redColor << xorstr_(R"(
__       ______                     __          _       __            ____         
\ \     / ____/  _____  _______  __/ /____     | |     / /___  ____  / __/__  _____
 \ \   / __/ | |/_/ _ \/ ___/ / / / __/ _ \    | | /| / / __ \/ __ \/ /_/ _ \/ ___/
 / /  / /____>  </  __/ /__/ /_/ / /_/  __/    | |/ |/ / /_/ / /_/ / __/  __/ /    
/_/  /_____/_/|_|\___/\___/\__,_/\__/\___/     |__/|__/\____/\____/_/  \___/_/     
  
discord.gg/executor                                                                                                  
t.me/executeos)") << resetColor << std::endl;

    KeyAuthApp.init();
    if (!KeyAuthApp.response.success)
    {
        std::cout << xorstr_("Status: ") << redColor << KeyAuthApp.response.message << resetColor << std::endl;
        Sleep(1500);
        exit(1);
    }

    if (fs::exists("Auth.json"))
    {
        if (!CheckIfJsonKeyExists("Auth.json", "username"))
        {
            std::string key = ReadFromJson("Auth.json", "license");
            KeyAuthApp.license(key);
            if (!KeyAuthApp.response.success)
            {
                std::remove("Auth.json");
                std::cout << xorstr_("Status: ") << redColor << KeyAuthApp.response.message << resetColor << std::endl;
                Sleep(1500);
                exit(1);
            }
        }
        else
        {
            std::string username = ReadFromJson("Auth.json", "username");
            std::string password = ReadFromJson("Auth.json", "password");
            KeyAuthApp.login(username, password);
            if (!KeyAuthApp.response.success)
            {
                std::remove("Auth.json");
                std::cout << xorstr_("Status: ") << redColor << KeyAuthApp.response.message << resetColor << std::endl;
                Sleep(1500);
                exit(1);
            }
        }
    }
    else
    {
        std::string username, password, key;

        std::cout << "\n\n[" << redColor << "+" << resetColor << "] Key: ";
        std::cin >> key;
        KeyAuthApp.license(key);

        if (!KeyAuthApp.response.success)
        {
            std::cout << xorstr_("Status: ") << redColor << KeyAuthApp.response.message << resetColor << std::endl;
            Sleep(1500);
            exit(1);
        }

        if (username.empty() || password.empty())
        {
            WriteToJson("Auth.json", "license", key, false, "", "");
        }
        else
        {
            WriteToJson("Auth.json", "username", username, true, "password", password);
        }
    }

    std::cout << "\n\n[" << redColor << "+" << resetColor << "] Expires in : DATE : " << __DATE__ << " HOUR : " << __TIME__ << std::endl;
    std::cout << "\n[" << redColor << "1" << resetColor << "] FiveM Spoofer (Perma)";
    std::cout << "\n[" << redColor << "2" << resetColor << "] FiveM Clean (Recommended)";
    std::cout << "\n[" << redColor << "3" << resetColor << "] Telegram Group";
    std::cout << "\n\n[" << greenColor << "+" << resetColor << "] Choose one of the options: ";

    std::vector<fs::path> paths = {
        fs::path(getenv("USERPROFILE")) / L"AppData/Roaming/CitizenFX",
        fs::path(getenv("USERPROFILE")) / L"AppData/Local/DigitalEntitlements",
        fs::path(getenv("USERPROFILE")) / L"AppData/Local/D3DSCache",
        fs::path(getenv("USERPROFILE")) / L"AppData/Local/CrashDumps",
        fs::path(getenv("USERPROFILE")) / L"AppData/Local/FiveM/FiveM.app",
    };

    const std::wstring keyPath = xorstr_(L"SYSTEM\\CurrentControlSet\\Control\\WMI\\Restrictions");
    const std::wstring valueName = xorstr_(L"HideMachine");
    const DWORD valueData = 1;

    int option;
    std::cin >> option;

    switch (option) {
    case 1:
        if (AddRegistryValue(keyPath, valueName, valueData)) {
            std::wcout << xorstr_(L"Registry value added or updated successfully.") << std::endl;
        }
        else {
            std::wcerr << xorstr_(L"Failed to add or update registry value.") << std::endl;
        }

        TaskKill(xorstr_(L"WmiPrvSE.exe"), 5);
        HideRegistryOperation();

        std::wcout << xorstr_(L"Operation completed.") << std::endl;
        break;

    case 2:
        for (const auto& path : paths) {
            if (DeletePath(path)) {
                std::wcout << xorstr_(L"Deleted: ") << path << std::endl;
            }
            else {
                std::wcerr << xorstr_(L"Failed to delete: ") << path << std::endl;
            }
        }

        std::cout << xorstr_("FiveM Clean Finished.") << std::endl;
        Sleep(5000);
        exit(0);
        break;

    case 3:
        std::cout << xorstr_("https://t.me/executeos") << std::endl;
        Sleep(5000);
        exit(0);
        break;

    default:
        std::cout << xorstr_("Wrong Choice! Restart Program") << std::endl;
        Sleep(5000);
        exit(0);
        break;
    }

    return 0;
}

std::string tm_to_readable_time(tm ctx) {
    char buffer[80];
    strftime(buffer, sizeof(buffer), xorstr_("%a %m/%d/%y %H:%M:%S %Z"), &ctx);
    return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
    return static_cast<std::time_t>(std::stol(timestamp));
}

static std::tm timet_to_tm(time_t timestamp) {
    std::tm context;
    localtime_s(&context, &timestamp);
    return context;
}
