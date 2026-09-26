#include "platform_native.h"
#include <QProcess>
#include <QDir>
#include <QCoreApplication>
#include <fstream>
#include <cstdlib>

#if defined(_WIN32)
#include <windows.h>
#include <srrestoreptapi.h>
#endif

bool PlatformNative::CheckAndRequestPermissions()
{
#if defined(_WIN32)
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup))
    {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    if (!isAdmin)
    {
        wchar_t szPath[MAX_PATH];
        GetModuleFileNameW(NULL, szPath, MAX_PATH);
        SHELLEXECUTEINFOW sei = {sizeof(sei)};
        sei.lpVerb = L"runas";
        sei.lpFile = szPath;
        sei.nShow = SW_NORMAL;
        ShellExecuteExW(&sei);
        QCoreApplication::quit();
        return false;
    }
    return true;
#elif defined(__APPLE__)
    QProcess::execute("open", QStringList() << "x-apple.systempreferences:com.apple.preference.security?Privacy_AllFiles");
    return true;
#else
    return QProcess::execute("pkexec", QStringList() << "whoami") == 0;
#endif
}

bool PlatformNative::CreateRegistryBackupAndRestorePoint(const QString &actionName)
{
#if defined(_WIN32)
    QString backupPath = QDir::homePath() + "/AppData/Local/Temp/Registry_Backup.reg";
    QString regCmd = QString("reg export HKCU\\Software \"%1\" /y").arg(backupPath);
    std::system(regCmd.toStdString().c_str());

    RESTOREPOINTINFOA restoreInfo;
    STATEMGRSTATUS status;
    restoreInfo.dwEventType = BEGIN_SYSTEM_CHANGE;
    restoreInfo.dwRestorePtType = APPLICATION_INSTALL;
    restoreInfo.llSequenceNumber = 0;
    strncpy(restoreInfo.szDescription, actionName.toStdString().c_str(), 64);

    return SRSetRestorePointA(&restoreInfo, &status) == TRUE;
#else
    Q_UNUSED(actionName);
    return true;
#endif
}

void PlatformNative::InitiateSelfDeletion()
{
    QString appPath = QCoreApplication::applicationFilePath();
    qint64 pid = QCoreApplication::applicationPid();

#if defined(_WIN32)
    QString scriptPath = QDir::tempPath() + "/self_clean.bat";
    std::ofstream script(scriptPath.toStdString());
    script << "@echo off\n"
           << ":loop\n"
           << "tasklist | find \"" << pid << "\" > nul\n"
           << "if not errorlevel 1 (timeout /t 1 > nul & goto loop)\n"
           << "del /f /q \"" << appPath.toNativeSeparators().toStdString() << "\"\n"
           << "del \"%~f0\"\n";
    script.close();
    QProcess::startDetached("cmd.exe", QStringList() << "/c" << scriptPath);
#else
    QString scriptPath = QDir::tempPath() + "/self_clean.sh";
    std::ofstream script(scriptPath.toStdString());
    script << "#!/bin/sh\n"
           << "while kill -0 " << pid << " 2>/dev/null; do sleep 1; done\n"
           << "rm -f \"" << appPath.toStdString() << "\"\n"
           << "rm -f \"$0\"\n";
    script.close();
    QProcess::execute("chmod", QStringList() << "+x" << scriptPath);
    QProcess::startDetached("/bin/sh", QStringList() << scriptPath);
#endif
    QCoreApplication::quit();
}