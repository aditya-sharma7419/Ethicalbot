#ifndef SCANNER_WATCHER_H
#define SCANNER_WATCHER_H

#include <QObject>
#include <QRunnable>
#include <QFileSystemWatcher>

struct CleanupCandidate
{
    QString path;
    QString type;
    bool isProtected;
};

class ScanTask : public QObject, public QRunnable
{
    Q_OBJECT
private:
    QString scanDirectory;
    bool overrideProtection;

public:
    ScanTask(const QString &dir, bool overrideProt)
        : scanDirectory(dir), overrideProtection(overrideProt) {}

    void run() override;

signals:
    void candidateFound(const CleanupCandidate &candidate);
    void scanFinished();
};

class AppWatcher : public QObject
{
    Q_OBJECT
private:
    QFileSystemWatcher watcher;

public:
    explicit AppWatcher(QObject *parent = nullptr);

signals:
    void appUninstallDetected(const QString &path);

private slots:
    void onDirectoryChanged(const QString &path);
};

#endif // SCANNER_WATCHER_H