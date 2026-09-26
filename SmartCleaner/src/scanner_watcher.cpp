#include "scanner_watcher.h"
#include "rust_core.h"
#include <QDir>
#include <QFileInfo>

void ScanTask::run()
{
    QDir dir(scanDirectory);
    if (!dir.exists())
    {
        emit scanFinished();
        return;
    }

    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &info : entries)
    {
        std::string pathStd = info.absoluteFilePath().toStdString();
        bool protectedPath = is_path_protected(pathStd.c_str());

        if (protectedPath && !overrideProtection)
        {
            continue;
        }

        CleanupCandidate candidate;
        candidate.path = info.absoluteFilePath();
        candidate.isProtected = protectedPath;

        if (info.suffix() == "plist" || info.filePath().contains(".config"))
        {
            candidate.type = "Orphaned Registry / Config";
        }
        else
        {
            candidate.type = "Unnecessary Cache File";
        }

        emit candidateFound(candidate);
    }
    emit scanFinished();
}

AppWatcher::AppWatcher(QObject *parent) : QObject(parent)
{
#if defined(__APPLE__)
    watcher.addPath("/Applications");
#elif defined(_WIN32)
    watcher.addPath("C:\\Program Files");
#else
    watcher.addPath(QDir::homePath() + "/.config");
#endif
    connect(&watcher, &QFileSystemWatcher::directoryChanged, this, &AppWatcher::onDirectoryChanged);
}

void AppWatcher::onDirectoryChanged(const QString &path)
{
    emit appUninstallDetected(path);
}