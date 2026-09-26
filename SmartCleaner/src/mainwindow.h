#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QFrame>
#include "scanner_watcher.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QFrame *sidebarFrame;
    QFrame *statusCard;
    QLabel *statusTitle;
    QLabel *statusSubtitle;
    QPushButton *btnSmartScan;

    QTreeWidget *dryRunTree;
    QPushButton *btnExecuteCleanup;
    QPushButton *btnSelfUninstall;
    QCheckBox *chkOverrideProtected;
    QCheckBox *chkWatcherMode;

    AppWatcher *watcher;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void startSmartScan();
    void addCandidateToTree(const CleanupCandidate &candidate);
    void executeSelectedDeletions();
    void handleSelfUninstall();
    void onWatcherTriggered(const QString &path);
    void setupAvastStyleLayout();
    void applyLightBlueTheme();
};

#endif // MAINWINDOW_H