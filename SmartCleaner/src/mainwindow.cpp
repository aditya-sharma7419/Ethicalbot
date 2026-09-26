#include "mainwindow.h"
#include "platform_native.h"
#include "rust_core.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QThreadPool>
#include <QStyle>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Smart Cleaner & Registry Watcher");
    resize(1000, 680);

    setupAvastStyleLayout();
    applyLightBlueTheme();

    watcher = new AppWatcher(this);
    connect(watcher, &AppWatcher::appUninstallDetected, this, &MainWindow::onWatcherTriggered);

    PlatformNative::CheckAndRequestPermissions();
}

void MainWindow::setupAvastStyleLayout()
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // Left Sidebar
    sidebarFrame = new QFrame(this);
    sidebarFrame->setObjectName("sidebarFrame");
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebarFrame);
    sidebarLayout->setContentsMargins(12, 24, 12, 24);

    QLabel *brandLabel = new QLabel("SMART CLEAN", sidebarFrame);
    brandLabel->setObjectName("brandLabel");
    sidebarLayout->addWidget(brandLabel);

    QPushButton *navStatus = new QPushButton(" System Status", sidebarFrame);
    QPushButton *navCleanup = new QPushButton(" Deep Clean", sidebarFrame);
    QPushButton *navOrphans = new QPushButton(" Registry Fix", sidebarFrame);

    navStatus->setObjectName("navButton");
    navCleanup->setObjectName("navButton");
    navOrphans->setObjectName("navButton");
    navStatus->setCheckable(true);
    navStatus->setChecked(true);

    sidebarLayout->addWidget(navStatus);
    sidebarLayout->addWidget(navCleanup);
    sidebarLayout->addWidget(navOrphans);
    sidebarLayout->addStretch();

    btnSelfUninstall = new QPushButton("Uninstall App", sidebarFrame);
    btnSelfUninstall->setObjectName("dangerButton");
    sidebarLayout->addWidget(btnSelfUninstall);

    // Workspace
    QVBoxLayout *mainWorkspace = new QVBoxLayout();
    mainWorkspace->setContentsMargins(24, 24, 24, 24);
    mainWorkspace->setSpacing(16);

    statusCard = new QFrame(this);
    statusCard->setObjectName("statusCardSafe");
    QHBoxLayout *cardLayout = new QHBoxLayout(statusCard);

    QVBoxLayout *statusTextLayout = new QVBoxLayout();
    statusTitle = new QLabel("System Protection Active", statusCard);
    statusTitle->setObjectName("statusTitle");
    statusSubtitle = new QLabel("All system paths, active processes, and registries are monitored.", statusCard);
    statusSubtitle->setObjectName("statusSubtitle");
    statusTextLayout->addWidget(statusTitle);
    statusTextLayout->addWidget(statusSubtitle);

    btnSmartScan = new QPushButton("Run Smart Scan", statusCard);
    btnSmartScan->setObjectName("primaryActionButton");

    cardLayout->addLayout(statusTextLayout);
    cardLayout->addStretch();
    cardLayout->addWidget(btnSmartScan);

    QHBoxLayout *configLayout = new QHBoxLayout();
    chkWatcherMode = new QCheckBox("Active Real-Time Watcher", this);
    chkWatcherMode->setChecked(true);
    chkOverrideProtected = new QCheckBox("Allow Protected Overrides (User Request)", this);
    configLayout->addWidget(chkWatcherMode);
    configLayout->addSpacing(20);
    configLayout->addWidget(chkOverrideProtected);
    configLayout->addStretch();

    dryRunTree = new QTreeWidget(this);
    dryRunTree->setHeaderLabels(QStringList() << "Action" << "Path / Registry Entry" << "Category" << "Status");
    dryRunTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);

    QHBoxLayout *bottomBar = new QHBoxLayout();
    btnExecuteCleanup = new QPushButton("Clean Selected Items", this);
    btnExecuteCleanup->setObjectName("secondaryActionButton");
    bottomBar->addStretch();
    bottomBar->addWidget(btnExecuteCleanup);

    mainWorkspace->addWidget(statusCard);
    mainWorkspace->addLayout(configLayout);
    mainWorkspace->addWidget(dryRunTree);
    mainWorkspace->addLayout(bottomBar);

    rootLayout->addWidget(sidebarFrame);
    rootLayout->addLayout(mainWorkspace);

    setCentralWidget(centralWidget);

    connect(btnSmartScan, &QPushButton::clicked, this, &MainWindow::startSmartScan);
    connect(navCleanup, &QPushButton::clicked, this, &MainWindow::startSmartScan);
    connect(navOrphans, &QPushButton::clicked, this, &MainWindow::startSmartScan);
    connect(btnExecuteCleanup, &QPushButton::clicked, this, &MainWindow::executeSelectedDeletions);
    connect(btnSelfUninstall, &QPushButton::clicked, this, &MainWindow::handleSelfUninstall);
}

void MainWindow::startSmartScan()
{
    dryRunTree->clear();
    statusCard->setObjectName("statusCardWarning");
    statusTitle->setText("Scanning System in Progress...");
    statusSubtitle->setText("Analyzing temporary files, .plist configs, and orphaned registries.");
    style()->unpolish(statusCard);
    style()->polish(statusCard);

    ScanTask *task = new ScanTask(QDir::tempPath(), chkOverrideProtected->isChecked());
    connect(task, &ScanTask::candidateFound, this, &MainWindow::addCandidateToTree);
    QThreadPool::globalInstance()->start(task);
}

void MainWindow::addCandidateToTree(const CleanupCandidate &candidate)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(dryRunTree);
    item->setCheckState(0, Qt::Checked);
    item->setText(1, candidate.path);
    item->setText(2, candidate.type);
    item->setText(3, candidate.isProtected ? "Protected (Override)" : "Safe to Remove");
}

void MainWindow::executeSelectedDeletions()
{
    PlatformNative::CreateRegistryBackupAndRestorePoint("Pre-Cleanup Restore Point");

    int count = 0;
    for (int i = 0; i < dryRunTree->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = dryRunTree->topLevelItem(i);
        if (item->checkState(0) == Qt::Checked)
        {
            QString path = item->text(1);
            if (trash_item(path.toStdString().c_str()))
            {
                count++;
            }
        }
    }

    statusCard->setObjectName("statusCardSafe");
    statusTitle->setText("System Cleaned Successfully");
    statusSubtitle->setText(QString("%1 items safely moved to Native OS Trash.").arg(count));
    style()->unpolish(statusCard);
    style()->polish(statusCard);
    dryRunTree->clear();
}

void MainWindow::onWatcherTriggered(const QString &path)
{
    if (chkWatcherMode->isChecked())
    {
        statusTitle->setText("Uninstall Event Detected");
        statusSubtitle->setText(QString("Change detected in %1. Cleaning leftover registry...").arg(path));
        startSmartScan();
    }
}

void MainWindow::handleSelfUninstall()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Uninstall Smart Cleaner",
        "Are you sure you want to remove this application, its registries, and temporary scripts?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        PlatformNative::InitiateSelfDeletion();
    }
}

void MainWindow::applyLightBlueTheme()
{
    QString qssPath = QCoreApplication::applicationDirPath() + "/assets/style.qss";

    QFile styleFile(qssPath);
    if (!styleFile.exists())
    {
        qssPath = "assets/style.qss";
        styleFile.setFileName(qssPath);
    }

    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&styleFile);
        this->setStyleSheet(stream.readAll());
        styleFile.close();
    }
}