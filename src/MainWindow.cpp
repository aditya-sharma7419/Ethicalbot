#include "MainWindow.h"

#include "SafeCleanerEngine.h"

#include <QCheckBox>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    settingsModel = AppSettings();
    setupUi();
    setStatusMessage("Protection active", true);
    logEntry("System guard ready. Safe cleanup mode is enabled.");
    if (scanProgress) {
        scanProgress->setValue(92);
    }
    updateSettingsSummary();
}

void MainWindow::setupUi()
{
    setWindowTitle("Secure Shield Cleaner");
    resize(1180, 760);
    setStyleSheet(R"(
        QWidget {
            background: #061a22;
            color: #ebfbf7;
            font-family: "Segoe UI", Arial;
        }
        QMainWindow {
            background: #061a22;
        }
        QFrame, QWidget {
            border: 0px solid transparent;
        }
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #9ce947, stop:1 #18c6b4);
            color: #031c22;
            border: none;
            border-radius: 12px;
            padding: 10px 16px;
            font-weight: 700;
            transition: transform 120ms ease, filter 120ms ease;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #abf053, stop:1 #1cd7c8);
            transform: translateY(-1px);
            filter: brightness(1.04);
        }
        QPushButton.secondary {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #7bc7ff, stop:1 #4d96de);
            color: #edf8ff;
        }
        QPushButton.danger {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ff9ca7, stop:1 #ff6a88);
            color: #fff2f5;
        }
        QLineEdit, QPlainTextEdit {
            background: rgba(8, 22, 31, 0.8);
            border: 1px solid rgba(160, 177, 180, 0.18);
            border-radius: 12px;
            color: #ebfbf7;
            padding: 10px 12px;
        }
        QCheckBox {
            color: #ebfbf7;
        }
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 5px;
            border: 1px solid rgba(160, 177, 180, 0.2);
            background: rgba(8, 22, 31, 0.8);
        }
        QCheckBox::indicator:checked {
            background: #9ce947;
        }
        QLabel {
            color: #ebfbf7;
        }
        QProgressBar {
            border: 1px solid rgba(156, 233, 71, 0.18);
            border-radius: 12px;
            background: rgba(255, 255, 255, 0.04);
            text-align: center;
            color: #ebfbf7;
            height: 18px;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #9ce947, stop:1 #18c6b4);
            border-radius: 12px;
        }
        #sidebar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #102f39, stop:1 #071d26);
            border: 1px solid rgba(117, 199, 164, 0.25);
            border-radius: 22px;
            padding: 18px 16px;
        }
        #contentPanel {
            background: transparent;
        }
        #topBar {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 rgba(18, 38, 46, 0.96), stop:1 rgba(7, 24, 31, 0.95));
            border: 1px solid rgba(117, 199, 164, 0.25);
            border-radius: 18px;
            padding: 14px 18px;
        }
        #cardPanel {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 rgba(18, 38, 46, 0.96), stop:1 rgba(7, 24, 31, 0.95));
            border: 1px solid rgba(117, 199, 164, 0.25);
            border-radius: 18px;
            padding: 18px;
        }
        #miniScore {
            background: rgba(11, 35, 42, 0.7);
            border: 1px solid rgba(117, 199, 164, 0.25);
            border-radius: 16px;
            padding: 12px 14px;
        }
        #summaryChip {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 rgba(17, 41, 50, 0.9), stop:1 rgba(9, 27, 38, 0.9));
            border: 1px solid rgba(117, 199, 164, 0.25);
            border-radius: 16px;
            padding: 0 12px;
        }
        #navButton {
            background: rgba(18, 42, 48, 0.7);
            border-radius: 12px;
            border: 1px solid transparent;
            color: #ebfbf7;
            text-align: left;
            padding: 10px 12px;
        }
        #navButton.active {
            background: rgba(156, 233, 71, 0.14);
            border: 1px solid rgba(156, 233, 71, 0.3);
        }
        #statusBadge {
            background: rgba(156, 233, 71, 0.08);
            border: 1px solid rgba(156, 233, 71, 0.28);
            color: #9ce947;
            border-radius: 999px;
            padding: 8px 12px;
        }
    )");

    auto* central = new QWidget(this);
    auto* rootLayout = new QHBoxLayout(central);
    rootLayout->setSpacing(18);
    rootLayout->setContentsMargins(18, 18, 18, 18);

    auto* sidebar = new QFrame(central);
    sidebar->setObjectName("sidebar");
    auto* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setSpacing(16);
    sidebarLayout->setContentsMargins(12, 12, 12, 12);

    auto shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(18);
    shadow->setOffset(0, 10);
    shadow->setColor(QColor(0, 0, 0, 110));
    sidebar->setGraphicsEffect(shadow);

    auto* brand = new QWidget(sidebar);
    auto* brandLayout = new QHBoxLayout(brand);
    brandLayout->setContentsMargins(0, 0, 0, 0);
    auto* logo = new QLabel("S", brand);
    logo->setAlignment(Qt::AlignCenter);
    logo->setStyleSheet("QLabel { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #9ce947, stop:1 #18c6b4); color: #031c22; border-radius: 12px; font-size: 20px; font-weight: 900; min-width: 40px; min-height: 40px; }");
    auto* brandText = new QLabel("<span style='font-size:11px; letter-spacing:3px; color:#7fe7af;'>SECURE</span><br><b>Shield Cleaner</b>", brand);
    brandText->setWordWrap(true);
    brandLayout->addWidget(logo);
    brandLayout->addWidget(brandText);
    sidebarLayout->addWidget(brand);

    auto* navButtonsWidget = new QWidget(sidebar);
    auto* navLayout = new QVBoxLayout(navButtonsWidget);
    navLayout->setContentsMargins(0, 0, 0, 0);
    navLayout->setSpacing(8);

    auto* dashboardBtn = new QPushButton("Dashboard", navButtonsWidget);
    dashboardBtn->setObjectName("navButton");
    dashboardBtn->setProperty("active", true);
    auto* protectedBtn = new QPushButton("Protection", navButtonsWidget);
    protectedBtn->setObjectName("navButton");
    auto* cleanupBtn = new QPushButton("Cleanup", navButtonsWidget);
    cleanupBtn->setObjectName("navButton");
    auto* logsBtn = new QPushButton("Scan logs", navButtonsWidget);
    logsBtn->setObjectName("navButton");
    navigationButtons = { dashboardBtn, protectedBtn, cleanupBtn, logsBtn };
    navLayout->addWidget(dashboardBtn);
    navLayout->addWidget(protectedBtn);
    navLayout->addWidget(cleanupBtn);
    navLayout->addWidget(logsBtn);
    sidebarLayout->addWidget(navButtonsWidget);

    auto* scanWrapper = new QWidget(sidebar);
    auto* scanLayout = new QVBoxLayout(scanWrapper);
    scanLayout->setContentsMargins(0, 0, 0, 0);
    scanProgress = new QProgressBar(scanWrapper);
    scanProgress->setRange(0, 100);
    scanProgress->setValue(92);
    scanProgress->setAlignment(Qt::AlignCenter);
    scanProgress->setFormat("92%");
    scanLayout->addWidget(scanProgress, 0, Qt::AlignHCenter);
    auto* scanLabel = new QLabel("Threat scan", scanWrapper);
    scanLabel->setAlignment(Qt::AlignCenter);
    auto* scanMeta = new QLabel("Last check: 8 minutes ago", scanWrapper);
    scanMeta->setAlignment(Qt::AlignCenter);
    scanMeta->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    scanLayout->addWidget(scanLabel);
    scanLayout->addWidget(scanMeta);
    sidebarLayout->addWidget(scanWrapper);

    scoreLabel = new QLabel("98.7 / 100", sidebar);
    scoreLabel->setObjectName("miniScore");
    scoreLabel->setAlignment(Qt::AlignLeft);
    auto* scoreTitle = new QLabel("Security score", sidebar);
    scoreTitle->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    auto* scoreBox = new QWidget(sidebar);
    auto* scoreLayout = new QVBoxLayout(scoreBox);
    scoreLayout->setContentsMargins(0, 0, 0, 0);
    scoreLayout->addWidget(scoreTitle);
    scoreLayout->addWidget(scoreLabel);
    sidebarLayout->addWidget(scoreBox);
    sidebarLayout->addStretch();

    auto* contentPanel = new QWidget(central);
    contentPanel->setObjectName("contentPanel");
    auto* contentLayout = new QVBoxLayout(contentPanel);
    contentLayout->setSpacing(14);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    auto* topBar = new QFrame(contentPanel);
    topBar->setObjectName("topBar");
    auto* topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(16, 10, 16, 10);
    titleLabel = new QLabel("Premium device protection", topBar);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(20);
    titleLabel->setFont(titleFont);
    auto* topBadge = new QLabel("Protection active", topBar);
    topBadge->setObjectName("statusBadge");
    topBadge->setAlignment(Qt::AlignCenter);
    topBarLayout->addWidget(titleLabel);
    topBarLayout->addWidget(topBadge, 0, Qt::AlignRight);
    contentLayout->addWidget(topBar);

    auto* hero = new QFrame(contentPanel);
    hero->setObjectName("cardPanel");
    auto* heroLayout = new QHBoxLayout(hero);
    heroLayout->setContentsMargins(18, 12, 18, 12);
    auto* heroText = new QLabel("Monitor risky paths, prevent destructive deletions, and scan for junk files, orphaned app entries, and registry leftovers with security-grade guardrails.", hero);
    heroText->setWordWrap(true);
    heroText->setStyleSheet("QLabel { color: #b9d7d7; line-height: 1.6; }");
    auto* heroStatus = new QFrame(hero);
    heroStatus->setStyleSheet("QFrame { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 rgba(10, 46, 51, 0.9), stop:1 rgba(8, 22, 31, 0.9)); border: 1px solid rgba(107, 198, 255, 0.28); border-radius: 18px; }");
    auto* heroStatusLayout = new QVBoxLayout(heroStatus);
    auto* statusTitle = new QLabel("System status");
    statusTitle->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    statusLabel = new QLabel("Connecting…", heroStatus);
    QFont statusFont = statusLabel->font();
    statusFont.setBold(true);
    statusFont.setPointSize(16);
    statusLabel->setFont(statusFont);
    auto* statusSmall = new QLabel("Protected endpoint ready", heroStatus);
    statusSmall->setStyleSheet("QLabel { color: #b9d7d7; font-size: 10px; letter-spacing: 0.08em; text-transform: uppercase; }");
    heroStatusLayout->addWidget(statusTitle);
    heroStatusLayout->addWidget(statusLabel);
    heroStatusLayout->addWidget(statusSmall);
    heroLayout->addWidget(heroText, 1);
    heroLayout->addWidget(heroStatus, 0);
    contentLayout->addWidget(hero);

    auto* summaryRow = new QWidget(contentPanel);
    auto* summaryLayout = new QGridLayout(summaryRow);
    summaryLayout->setSpacing(12);
    auto* chip1 = new QWidget(summaryRow);
    chip1->setObjectName("summaryChip");
    auto* chip1Layout = new QHBoxLayout(chip1);
    auto* chip1Label = new QLabel("Threats blocked", chip1);
    chip1Label->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    auto* chip1Value = new QLabel("17", chip1);
    chip1Value->setStyleSheet("QLabel { color: #9ce947; font-size: 20px; font-weight: 800; }");
    chip1Layout->addWidget(chip1Label);
    chip1Layout->addWidget(chip1Value, 0, Qt::AlignRight);
    summaryLayout->addWidget(chip1, 0, 0);
    auto* chip2 = new QWidget(summaryRow);
    chip2->setObjectName("summaryChip");
    auto* chip2Layout = new QHBoxLayout(chip2);
    auto* chip2Label = new QLabel("Cleanup queue", chip2);
    chip2Label->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    auto* chip2Value = new QLabel("4 items", chip2);
    chip2Value->setStyleSheet("QLabel { color: #ebfbf7; font-size: 20px; font-weight: 800; }");
    chip2Layout->addWidget(chip2Label);
    chip2Layout->addWidget(chip2Value, 0, Qt::AlignRight);
    summaryLayout->addWidget(chip2, 0, 1);
    auto* chip3 = new QWidget(summaryRow);
    chip3->setObjectName("summaryChip");
    auto* chip3Layout = new QHBoxLayout(chip3);
    auto* chip3Label = new QLabel("Protected apps", chip3);
    chip3Label->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    auto* chip3Value = new QLabel("26", chip3);
    chip3Value->setStyleSheet("QLabel { color: #7bc7ff; font-size: 20px; font-weight: 800; }");
    chip3Layout->addWidget(chip3Label);
    chip3Layout->addWidget(chip3Value, 0, Qt::AlignRight);
    summaryLayout->addWidget(chip3, 0, 2);
    contentLayout->addWidget(summaryRow);

    auto* stats = new QWidget(contentPanel);
    auto* statsLayout = new QGridLayout(stats);
    statsLayout->setSpacing(12);
    auto* stat1 = new QFrame(stats);
    stat1->setObjectName("cardPanel");
    auto* stat1Layout = new QVBoxLayout(stat1);
    auto* label1 = new QLabel("Watch mode", stat1);
    label1->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    QLabel* watchValue = new QLabel("Monitoring Windows Registry and AppData changes...", stat1);
    watchValue->setWordWrap(true);
    stat1Layout->addWidget(label1);
    stat1Layout->addWidget(watchValue);
    statsLayout->addWidget(stat1, 0, 0);
    auto* stat2 = new QFrame(stats);
    stat2->setObjectName("cardPanel");
    auto* stat2Layout = new QVBoxLayout(stat2);
    auto* label2 = new QLabel("Protected entries", stat2);
    label2->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    QLabel* protectedValue = new QLabel("6", stat2);
    protectedValue->setStyleSheet("QLabel { font-size: 28px; font-weight: 800; }");
    stat2Layout->addWidget(label2);
    stat2Layout->addWidget(protectedValue);
    statsLayout->addWidget(stat2, 0, 1);
    auto* stat3 = new QFrame(stats);
    stat3->setObjectName("cardPanel");
    auto* stat3Layout = new QVBoxLayout(stat3);
    auto* label3 = new QLabel("Restore point", stat3);
    label3->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    QLabel* restoreValue = new QLabel("Ready", stat3);
    restoreValue->setStyleSheet("QLabel { font-size: 28px; font-weight: 800; }");
    stat3Layout->addWidget(label3);
    stat3Layout->addWidget(restoreValue);
    statsLayout->addWidget(stat3, 0, 2);
    contentLayout->addWidget(stats);

    auto* formPanel = new QFrame(contentPanel);
    formPanel->setObjectName("cardPanel");
    auto* formLayout = new QGridLayout(formPanel);
    formLayout->setColumnStretch(0, 1);
    formLayout->setColumnStretch(1, 0);
    auto* formTitle = new QLabel("Check a path before cleanup", formPanel);
    formTitle->setStyleSheet("QLabel { font-size: 20px; font-weight: 700; }");
    formLayout->addWidget(formTitle, 0, 0, 1, 2);
    pathInput = new QLineEdit(formPanel);
    pathInput->setText("C:/Users/demo/Downloads/temp");
    formLayout->addWidget(pathInput, 1, 0);
    validateButton = new QPushButton("Validate", formPanel);
    validateButton->setObjectName("validateBtn");
    formLayout->addWidget(validateButton, 1, 1);
    auto* actionRow = new QWidget(formPanel);
    auto* actionLayout = new QHBoxLayout(actionRow);
    actionLayout->setContentsMargins(0, 0, 0, 0);
    scanButton = new QPushButton("Activate watcher", actionRow);
    scanButton->setProperty("class", "secondary");
    scanButton->setStyleSheet("QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #7bc7ff, stop:1 #4d96de); color: #edf8ff; border-radius: 12px; padding: 10px 16px; }");
    restoreButton = new QPushButton("Create restore point", actionRow);
    restoreButton->setProperty("class", "secondary");
    restoreButton->setStyleSheet("QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #7bc7ff, stop:1 #4d96de); color: #edf8ff; border-radius: 12px; padding: 10px 16px; }");
    auto* deleteButton = new QPushButton("Delete path", actionRow);
    deleteButton->setProperty("class", "danger");
    deleteButton->setStyleSheet("QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ff9ca7, stop:1 #ff6a88); color: #fff2f5; border-radius: 12px; padding: 10px 16px; }");
    actionLayout->addWidget(scanButton);
    actionLayout->addWidget(restoreButton);
    actionLayout->addWidget(deleteButton);
    formLayout->addWidget(actionRow, 2, 0, 1, 2);
    auto* resultRow = new QWidget(formPanel);
    auto* resultLayout = new QVBoxLayout(resultRow);
    resultLayout->setContentsMargins(0, 0, 0, 0);
    auto* res = new QLabel("Enter a path to check whether it is protected by the app guardrails.", resultRow);
    res->setWordWrap(true);
    res->setStyleSheet("QLabel { background: rgba(148, 163, 184, 0.08); border: 1px solid transparent; border-radius: 12px; padding: 12px; }");
    resultLayout->addWidget(res);
    formLayout->addWidget(resultRow, 3, 0, 1, 2);
    contentLayout->addWidget(formPanel);

    auto* settingsPanel = new QFrame(contentPanel);
    settingsPanel->setObjectName("cardPanel");
    auto* settingsLayout = new QVBoxLayout(settingsPanel);
    auto* settingsTitle = new QLabel("Protection settings");
    settingsTitle->setStyleSheet("QLabel { font-size: 20px; font-weight: 700; }");
    settingsLayout->addWidget(settingsTitle);
    realTimeCheck = new QCheckBox("Real-time scanning", settingsPanel);
    autoBackupCheck = new QCheckBox("Auto backup before deletion", settingsPanel);
    silentModeCheck = new QCheckBox("Silent cleanup mode", settingsPanel);
    realTimeCheck->setChecked(settingsModel.realTime);
    autoBackupCheck->setChecked(settingsModel.backupBeforeDelete);
    silentModeCheck->setChecked(settingsModel.silentMode);
    settingsLayout->addWidget(realTimeCheck);
    settingsLayout->addWidget(autoBackupCheck);
    settingsLayout->addWidget(silentModeCheck);
    settingsSummaryLabel = new QLabel("Protection level: balanced", settingsPanel);
    settingsSummaryLabel->setStyleSheet("QLabel { background: rgba(156, 233, 71, 0.08); border: 1px solid rgba(156, 233, 71, 0.2); border-radius: 12px; padding: 10px; color: #7fe7af; }");
    settingsLayout->addWidget(settingsSummaryLabel);
    contentLayout->addWidget(settingsPanel);

    auto* shell = new QWidget(contentPanel);
    auto* shellLayout = new QVBoxLayout(shell);
    auto* modesTitle = new QLabel("Automation modes");
    modesTitle->setStyleSheet("QLabel { font-size: 20px; font-weight: 700; }");
    shellLayout->addWidget(modesTitle);

    auto* listLayout = new QGridLayout();
    listLayout->setSpacing(12);
    for (int i = 0; i < 3; ++i) {
        auto* card = new QFrame(shell);
        card->setObjectName("cardPanel");
        auto* cardLayout = new QVBoxLayout(card);
        auto* cardTitle = new QLabel((i == 0 ? "1. Automatic cleanup" : i == 1 ? "2. User-approved cleanup" : "3. Orphan registry cleanup"), card);
        cardTitle->setStyleSheet("QLabel { font-weight: 700; font-size: 16px; }");
        auto* cardText = new QLabel((i == 0 ? "Deletes unnecessary files, apps, and their registry surfaces after a scan." : i == 1 ? "Shows selected files or apps for the user, waits for approval, then removes them." : "Finds leftover .plist or ~/.config entries from apps no longer installed."), card);
        cardText->setWordWrap(true);
        cardText->setStyleSheet("QLabel { color: #b9d7d7; }");
        auto* modeButton = new QPushButton(i == 0 ? "Run automatic scan" : i == 1 ? "Review user items" : "Scan orphan registry", card);
        modeButton->setProperty("class", "secondary");
        cardLayout->addWidget(cardTitle);
        cardLayout->addWidget(cardText);
        cardLayout->addWidget(modeButton);
        listLayout->addWidget(card, 0, i);
    }
    shellLayout->addLayout(listLayout);
    contentLayout->addWidget(shell);

    auto* logPanel = new QFrame(contentPanel);
    logPanel->setObjectName("cardPanel");
    auto* logLayout = new QVBoxLayout(logPanel);
    auto* logTitle = new QLabel("Recent audit log");
    logTitle->setStyleSheet("QLabel { font-size: 20px; font-weight: 700; }");
    logLayout->addWidget(logTitle);
    logArea = new QPlainTextEdit(logPanel);
    logArea->setReadOnly(true);
    logArea->setPlainText("No actions logged yet.");
    logLayout->addWidget(logArea);
    contentLayout->addWidget(logPanel);

    rootLayout->addWidget(sidebar, 0);
    rootLayout->addWidget(contentPanel, 1);
    setCentralWidget(central);

    connect(validateButton, &QPushButton::clicked, this, &MainWindow::handleValidatePath);
    connect(scanButton, &QPushButton::clicked, this, &MainWindow::handleScan);
    connect(restoreButton, &QPushButton::clicked, this, &MainWindow::handleRestorePoint);
    connect(realTimeCheck, &QCheckBox::toggled, this, [this](bool checked) {
        settingsModel.realTime = checked;
        updateSettingsSummary();
    });
    connect(autoBackupCheck, &QCheckBox::toggled, this, [this](bool checked) {
        settingsModel.backupBeforeDelete = checked;
        updateSettingsSummary();
    });
    connect(silentModeCheck, &QCheckBox::toggled, this, [this](bool checked) {
        settingsModel.silentMode = checked;
        updateSettingsSummary();
    });
    connect(dashboardBtn, &QPushButton::clicked, this, [this, dashboardBtn]() { setNavigationSelection(dashboardBtn); });
    connect(protectedBtn, &QPushButton::clicked, this, [this, protectedBtn]() { setNavigationSelection(protectedBtn); });
    connect(cleanupBtn, &QPushButton::clicked, this, [this, cleanupBtn]() { setNavigationSelection(cleanupBtn); });
    connect(logsBtn, &QPushButton::clicked, this, [this, logsBtn]() { setNavigationSelection(logsBtn); });
    setNavigationSelection(dashboardBtn);
}

void MainWindow::setupNavigation()
{
}

void MainWindow::updateSettingsSummary()
{
    const int activeCount = static_cast<int>(settingsModel.realTime) +
                            static_cast<int>(settingsModel.backupBeforeDelete) +
                            static_cast<int>(settingsModel.silentMode);
    const QString level = activeCount >= 2 ? "balanced" : (activeCount == 1 ? "smart" : "minimal");
    if (settingsSummaryLabel) {
        settingsSummaryLabel->setText(QString("Protection level: %1 · %2 settings active").arg(level).arg(activeCount));
    }
}

void MainWindow::setNavigationSelection(QPushButton* activeButton)
{
    for (auto* button : navigationButtons) {
        if (!button) {
            continue;
        }
        const bool active = button == activeButton;
        button->setProperty("active", active);
        button->setStyleSheet(active
            ? "QPushButton { background: rgba(156, 233, 71, 0.14); border: 1px solid rgba(156, 233, 71, 0.3); border-radius: 12px; padding: 10px 12px; color: #ebfbf7; text-align: left; }"
            : "QPushButton { background: rgba(18, 42, 48, 0.7); border-radius: 12px; border: 1px solid transparent; color: #ebfbf7; text-align: left; padding: 10px 12px; }");
        button->style()->unpolish(button);
        button->style()->polish(button);
    }
}

void MainWindow::handleValidatePath()
{
    const QString path = pathInput->text().trimmed();
    if (path.isEmpty()) {
        setStatusMessage("No path entered.", false);
        logEntry("Validation failed: no input path was provided.");
        return;
    }

    const std::string nativePath = path.toStdString();
    const bool isProtected = SafeCleanerEngine::isProtectedPath(nativePath);

    if (!isProtected) {
        setStatusMessage(QString("Path approved: %1").arg(path), true);
        logEntry(QString("Validated path: %1").arg(path));
    } else {
        setStatusMessage(QString("Blocked by protection: %1").arg(path), false);
        logEntry(QString("Blocked protected path: %1").arg(path));
    }
}

void MainWindow::handleScan()
{
    const auto candidates = SafeCleanerEngine::scanDevice("automatic");
    const QString summary = QString::fromStdString(SafeCleanerEngine::summarize(candidates, "automatic"));
    setStatusMessage(summary, true);
    logEntry(summary);
    if (scanProgress) {
        scanProgress->setValue(92);
    }
}

void MainWindow::handleRestorePoint()
{
    const QString message = QString::fromStdString(SafeCleanerEngine::createRestorePointDescription());
    setStatusMessage(message, true);
    logEntry(message);
}

void MainWindow::setStatusMessage(const QString& message, bool isSafe)
{
    const QString color = isSafe ? "#90EE90" : "#FF7A7A";
    if (statusLabel) {
        statusLabel->setText(QString("<span style='color:%1;'>%2</span>").arg(color, message));
    }
}

void MainWindow::logEntry(const QString& message)
{
    if (logArea) {
        logArea->appendPlainText(message);
    }
}

    auto* central = new QWidget(this);
    auto* rootLayout = new QHBoxLayout(central);
    rootLayout->setSpacing(18);
    rootLayout->setContentsMargins(18, 18, 18, 18);

    auto* sidebar = new QFrame(central);
    sidebar->setObjectName("sidebar");
    auto* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setSpacing(16);
    sidebarLayout->setContentsMargins(12, 12, 12, 12);

    auto* brand = new QWidget(sidebar);
    auto* brandLayout = new QHBoxLayout(brand);
    brandLayout->setContentsMargins(0, 0, 0, 0);
    auto* logo = new QLabel("S", brand);
    logo->setAlignment(Qt::AlignCenter);
    logo->setStyleSheet("QLabel { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #9ce947, stop:1 #18c6b4); color: #031c22; border-radius: 12px; font-size: 20px; font-weight: 900; min-width: 40px; min-height: 40px; }");
    auto* brandText = new QLabel("<span style='font-size:11px; letter-spacing:3px; color:#7fe7af;'>SECURE</span><br><b>Shield Cleaner</b>", brand);
    brandText->setWordWrap(true);
    brandLayout->addWidget(logo);
    brandLayout->addWidget(brandText);
    sidebarLayout->addWidget(brand);

    auto* navButtons = new QWidget(sidebar);
    auto* navLayout = new QVBoxLayout(navButtons);
    navLayout->setContentsMargins(0, 0, 0, 0);
    navLayout->setSpacing(8);
    auto* dashboardBtn = new QPushButton("Dashboard", navButtons);
    dashboardBtn->setObjectName("navButton");
    auto* protectedBtn = new QPushButton("Protection", navButtons);
    protectedBtn->setObjectName("navButton");
    auto* cleanupBtn = new QPushButton("Cleanup", navButtons);
    cleanupBtn->setObjectName("navButton");
    auto* logsBtn = new QPushButton("Scan logs", navButtons);
    logsBtn->setObjectName("navButton");
    navLayout->addWidget(dashboardBtn);
    navLayout->addWidget(protectedBtn);
    navLayout->addWidget(cleanupBtn);
    navLayout->addWidget(logsBtn);
    sidebarLayout->addWidget(navButtons);

    auto* scanWrapper = new QWidget(sidebar);
    auto* scanLayout = new QVBoxLayout(scanWrapper);
    scanLayout->setContentsMargins(0, 0, 0, 0);
    scanProgress = new QProgressBar(scanWrapper);
    scanProgress->setRange(0, 100);
    scanProgress->setValue(92);
    scanProgress->setAlignment(Qt::AlignCenter);
    scanProgress->setFormat("92%" );
    scanLayout->addWidget(scanProgress, 0, Qt::AlignHCenter);
    auto* scanLabel = new QLabel("Threat scan", scanWrapper);
    scanLabel->setAlignment(Qt::AlignCenter);
    auto* scanMeta = new QLabel("Last check: 8 minutes ago", scanWrapper);
    scanMeta->setAlignment(Qt::AlignCenter);
    scanMeta->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    scanLayout->addWidget(scanLabel);
    scanLayout->addWidget(scanMeta);
    sidebarLayout->addWidget(scanWrapper);

    scoreLabel = new QLabel("98.7 / 100", sidebar);
    scoreLabel->setObjectName("miniScore");
    scoreLabel->setAlignment(Qt::AlignLeft);
    auto* scoreTitle = new QLabel("Security score", sidebar);
    scoreTitle->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    auto* scoreBox = new QWidget(sidebar);
    auto* scoreLayout = new QVBoxLayout(scoreBox);
    scoreLayout->setContentsMargins(0, 0, 0, 0);
    scoreLayout->addWidget(scoreTitle);
    scoreLayout->addWidget(scoreLabel);
    sidebarLayout->addWidget(scoreBox);
    sidebarLayout->addStretch();

    auto* contentPanel = new QWidget(central);
    contentPanel->setObjectName("contentPanel");
    auto* contentLayout = new QVBoxLayout(contentPanel);
    contentLayout->setSpacing(14);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    auto* topBar = new QFrame(contentPanel);
    topBar->setObjectName("topBar");
    auto* topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(16, 10, 16, 10);
    titleLabel = new QLabel("Premium device protection", topBar);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(20);
    titleLabel->setFont(titleFont);
    auto* topBadge = new QLabel("Protection active", topBar);
    topBadge->setObjectName("statusBadge");
    topBadge->setAlignment(Qt::AlignCenter);
    topBarLayout->addWidget(titleLabel);
    topBarLayout->addWidget(topBadge, 0, Qt::AlignRight);
    contentLayout->addWidget(topBar);

    auto* hero = new QFrame(contentPanel);
    hero->setObjectName("cardPanel");
    auto* heroLayout = new QHBoxLayout(hero);
    heroLayout->setContentsMargins(18, 12, 18, 12);
    auto* heroText = new QLabel("Monitor risky paths, prevent destructive deletions, and scan for junk files, orphaned app entries, and registry leftovers with security-grade guardrails.", hero);
    heroText->setWordWrap(true);
    heroText->setStyleSheet("QLabel { color: #b9d7d7; line-height: 1.6; }");
    auto* heroStatus = new QFrame(hero);
    heroStatus->setStyleSheet("QFrame { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 rgba(10, 46, 51, 0.9), stop:1 rgba(8, 22, 31, 0.9)); border: 1px solid rgba(107, 198, 255, 0.28); border-radius: 18px; }");
    auto* heroStatusLayout = new QVBoxLayout(heroStatus);
    auto* statusTitle = new QLabel("System status");
    statusTitle->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    statusLabel = new QLabel("Connecting…", heroStatus);
    QFont statusFont = statusLabel->font();
    statusFont.setBold(true);
    statusFont.setPointSize(16);
    statusLabel->setFont(statusFont);
    auto* statusSmall = new QLabel("Protected endpoint ready", heroStatus);
    statusSmall->setStyleSheet("QLabel { color: #b9d7d7; font-size: 10px; letter-spacing: 0.08em; text-transform: uppercase; }");
    heroStatusLayout->addWidget(statusTitle);
    heroStatusLayout->addWidget(statusLabel);
    heroStatusLayout->addWidget(statusSmall);
    heroLayout->addWidget(heroText, 1);
    heroLayout->addWidget(heroStatus, 0);
    contentLayout->addWidget(hero);

    auto* summaryRow = new QWidget(contentPanel);
    auto* summaryLayout = new QGridLayout(summaryRow);
    summaryLayout->setSpacing(12);
    auto* chips = new QList<QWidget*>();
    auto* chip1 = new QWidget(summaryRow);
    chip1->setObjectName("summaryChip");
    auto* chip1Layout = new QHBoxLayout(chip1);
    auto* chip1Label = new QLabel("Threats blocked", chip1);
    chip1Label->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    auto* chip1Value = new QLabel("17", chip1);
    chip1Value->setStyleSheet("QLabel { color: #9ce947; font-size: 20px; font-weight: 800; }");
    chip1Layout->addWidget(chip1Label);
    chip1Layout->addWidget(chip1Value, 0, Qt::AlignRight);
    summaryLayout->addWidget(chip1, 0, 0);
    auto* chip2 = new QWidget(summaryRow);
    chip2->setObjectName("summaryChip");
    auto* chip2Layout = new QHBoxLayout(chip2);
    auto* chip2Label = new QLabel("Cleanup queue", chip2);
    chip2Label->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    auto* chip2Value = new QLabel("4 items", chip2);
    chip2Value->setStyleSheet("QLabel { color: #ebfbf7; font-size: 20px; font-weight: 800; }");
    chip2Layout->addWidget(chip2Label);
    chip2Layout->addWidget(chip2Value, 0, Qt::AlignRight);
    summaryLayout->addWidget(chip2, 0, 1);
    auto* chip3 = new QWidget(summaryRow);
    chip3->setObjectName("summaryChip");
    auto* chip3Layout = new QHBoxLayout(chip3);
    auto* chip3Label = new QLabel("Protected apps", chip3);
    chip3Label->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    auto* chip3Value = new QLabel("26", chip3);
    chip3Value->setStyleSheet("QLabel { color: #7bc7ff; font-size: 20px; font-weight: 800; }");
    chip3Layout->addWidget(chip3Label);
    chip3Layout->addWidget(chip3Value, 0, Qt::AlignRight);
    summaryLayout->addWidget(chip3, 0, 2);
    contentLayout->addWidget(summaryRow);

    auto* stats = new QWidget(contentPanel);
    auto* statsLayout = new QGridLayout(stats);
    statsLayout->setSpacing(12);
    auto* stat1 = new QFrame(stats);
    stat1->setObjectName("cardPanel");
    auto* stat1Layout = new QVBoxLayout(stat1);
    auto* label1 = new QLabel("Watch mode", stat1);
    label1->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    QLabel* watchValue = new QLabel("Monitoring Windows Registry and AppData changes...", stat1);
    watchValue->setWordWrap(true);
    stat1Layout->addWidget(label1);
    stat1Layout->addWidget(watchValue);
    statsLayout->addWidget(stat1, 0, 0);
    auto* stat2 = new QFrame(stats);
    stat2->setObjectName("cardPanel");
    auto* stat2Layout = new QVBoxLayout(stat2);
    auto* label2 = new QLabel("Protected entries", stat2);
    label2->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    QLabel* protectedValue = new QLabel("6", stat2);
    protectedValue->setStyleSheet("QLabel { font-size: 28px; font-weight: 800; }");
    stat2Layout->addWidget(label2);
    stat2Layout->addWidget(protectedValue);
    statsLayout->addWidget(stat2, 0, 1);
    auto* stat3 = new QFrame(stats);
    stat3->setObjectName("cardPanel");
    auto* stat3Layout = new QVBoxLayout(stat3);
    auto* label3 = new QLabel("Restore point", stat3);
    label3->setStyleSheet("QLabel { color: #b9d7d7; font-size: 11px; letter-spacing: 0.08em; text-transform: uppercase; }");
    QLabel* restoreValue = new QLabel("Ready", stat3);
    restoreValue->setStyleSheet("QLabel { font-size: 28px; font-weight: 800; }");
    stat3Layout->addWidget(label3);
    stat3Layout->addWidget(restoreValue);
    statsLayout->addWidget(stat3, 0, 2);
    contentLayout->addWidget(stats);

    auto* formPanel = new QFrame(contentPanel);
    formPanel->setObjectName("cardPanel");
    auto* formLayout = new QGridLayout(formPanel);
    formLayout->setColumnStretch(0, 1);
    formLayout->setColumnStretch(1, 0);
    auto* formTitle = new QLabel("Check a path before cleanup", formPanel);
    formTitle->setStyleSheet("QLabel { font-size: 20px; font-weight: 700; }");
    formLayout->addWidget(formTitle, 0, 0, 1, 2);
    pathInput = new QLineEdit(formPanel);
    pathInput->setText("C:/Users/demo/Downloads/temp");
    formLayout->addWidget(pathInput, 1, 0);
    validateButton = new QPushButton("Validate", formPanel);
    validateButton->setObjectName("validateBtn");
    formLayout->addWidget(validateButton, 1, 1);
    auto* actionRow = new QWidget(formPanel);
    auto* actionLayout = new QHBoxLayout(actionRow);
    actionLayout->setContentsMargins(0, 0, 0, 0);
    scanButton = new QPushButton("Activate watcher", actionRow);
    scanButton->setProperty("class", "secondary");
    scanButton->setStyleSheet("QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #7bc7ff, stop:1 #4d96de); color: #edf8ff; border-radius: 12px; padding: 10px 16px; }");
    restoreButton = new QPushButton("Create restore point", actionRow);
    restoreButton->setProperty("class", "secondary");
    restoreButton->setStyleSheet("QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #7bc7ff, stop:1 #4d96de); color: #edf8ff; border-radius: 12px; padding: 10px 16px; }");
    auto* deleteButton = new QPushButton("Delete path", actionRow);
    deleteButton->setProperty("class", "danger");
    deleteButton->setStyleSheet("QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ff9ca7, stop:1 #ff6a88); color: #fff2f5; border-radius: 12px; padding: 10px 16px; }");
    actionLayout->addWidget(scanButton);
    actionLayout->addWidget(restoreButton);
    actionLayout->addWidget(deleteButton);
    formLayout->addWidget(actionRow, 2, 0, 1, 2);
    auto* resultRow = new QWidget(formPanel);
    auto* resultLayout = new QVBoxLayout(resultRow);
    resultLayout->setContentsMargins(0, 0, 0, 0);
    auto* res = new QLabel("Enter a path to check whether it is protected by the app guardrails.", resultRow);
    res->setWordWrap(true);
    res->setStyleSheet("QLabel { background: rgba(148, 163, 184, 0.08); border: 1px solid transparent; border-radius: 12px; padding: 12px; }");
    resultLayout->addWidget(res);
    formLayout->addWidget(resultRow, 3, 0, 1, 2);
    contentLayout->addWidget(formPanel);

    auto* settingsPanel = new QFrame(contentPanel);
    settingsPanel->setObjectName("cardPanel");
    auto* settingsLayout = new QVBoxLayout(settingsPanel);
    auto* settingsTitle = new QLabel("Protection settings");
    settingsTitle->setStyleSheet("QLabel { font-size: 20px; font-weight: 700; }");
    settingsLayout->addWidget(settingsTitle);
    realTimeCheck = new QCheckBox("Real-time scanning", settingsPanel);
    autoBackupCheck = new QCheckBox("Auto backup before deletion", settingsPanel);
    silentModeCheck = new QCheckBox("Silent cleanup mode", settingsPanel);
    realTimeCheck->setChecked(true);
    autoBackupCheck->setChecked(true);
    settingsLayout->addWidget(realTimeCheck);
    settingsLayout->addWidget(autoBackupCheck);
    settingsLayout->addWidget(silentModeCheck);
    auto* summary = new QLabel("Protection level: balanced", settingsPanel);
    summary->setStyleSheet("QLabel { background: rgba(156, 233, 71, 0.08); border: 1px solid rgba(156, 233, 71, 0.2); border-radius: 12px; padding: 10px; color: #7fe7af; }");
    settingsLayout->addWidget(summary);
    contentLayout->addWidget(settingsPanel);

    auto* shell = new QWidget(contentPanel);
    auto* shellLayout = new QVBoxLayout(shell);
    auto* modesTitle = new QLabel("Automation modes");
    modesTitle->setStyleSheet("QLabel { font-size: 20px; font-weight: 700; }");
    shellLayout->addWidget(modesTitle);

    auto* listLayout = new QGridLayout();
    listLayout->setSpacing(12);
    for (int i = 0; i < 3; ++i) {
        auto* card = new QFrame(shell);
        card->setObjectName("cardPanel");
        auto* cardLayout = new QVBoxLayout(card);
        auto* cardTitle = new QLabel((i == 0 ? "1. Automatic cleanup" : i == 1 ? "2. User-approved cleanup" : "3. Orphan registry cleanup"), card);
        cardTitle->setStyleSheet("QLabel { font-weight: 700; font-size: 16px; }");
        auto* cardText = new QLabel((i == 0 ? "Deletes unnecessary files, apps, and their registry surfaces after a scan." : i == 1 ? "Shows selected files or apps for the user, waits for approval, then removes them." : "Finds leftover .plist or ~/.config entries from apps no longer installed."), card);
        cardText->setWordWrap(true);
        cardText->setStyleSheet("QLabel { color: #b9d7d7; }");
        auto* modeButton = new QPushButton(i == 0 ? "Run automatic scan" : i == 1 ? "Review user items" : "Scan orphan registry", card);
        modeButton->setProperty("class", "secondary");
        cardLayout->addWidget(cardTitle);
        cardLayout->addWidget(cardText);
        cardLayout->addWidget(modeButton);
        listLayout->addWidget(card, 0, i);
    }
    shellLayout->addLayout(listLayout);
    contentLayout->addWidget(shell);

    auto* logPanel = new QFrame(contentPanel);
    logPanel->setObjectName("cardPanel");
    auto* logLayout = new QVBoxLayout(logPanel);
    auto* logTitle = new QLabel("Recent audit log");
    logTitle->setStyleSheet("QLabel { font-size: 20px; font-weight: 700; }");
    logLayout->addWidget(logTitle);
    logArea = new QPlainTextEdit(logPanel);
    logArea->setReadOnly(true);
    logArea->setPlainText("No actions logged yet.");
    logLayout->addWidget(logArea);
    contentLayout->addWidget(logPanel);

    rootLayout->addWidget(sidebar, 0);
    rootLayout->addWidget(contentPanel, 1);
    setCentralWidget(central);

    connect(validateButton, &QPushButton::clicked, this, &MainWindow::handleValidatePath);
    connect(scanButton, &QPushButton::clicked, this, &MainWindow::handleScan);
    connect(restoreButton, &QPushButton::clicked, this, &MainWindow::handleRestorePoint);
}

void MainWindow::handleValidatePath()
{
    const QString path = pathInput->text().trimmed();
    if (path.isEmpty()) {
        setStatusMessage("No path entered.", false);
        logEntry("Validation failed: no input path was provided.");
        return;
    }

    const std::string nativePath = path.toStdString();
    const bool isProtected = SafeCleanerEngine::isProtectedPath(nativePath);

    if (!isProtected) {
        setStatusMessage(QString("Path approved: %1").arg(path), true);
        logEntry(QString("Validated path: %1").arg(path));
    } else {
        setStatusMessage(QString("Blocked by protection: %1").arg(path), false);
        logEntry(QString("Blocked protected path: %1").arg(path));
    }
}

void MainWindow::handleScan()
{
    const auto candidates = SafeCleanerEngine::scanDevice("automatic");
    const QString summary = QString::fromStdString(SafeCleanerEngine::summarize(candidates, "automatic"));
    setStatusMessage(summary, true);
    logEntry(summary);
    scanProgress->setValue(92);
}

void MainWindow::handleRestorePoint()
{
    const QString message = QString::fromStdString(SafeCleanerEngine::createRestorePointDescription());
    setStatusMessage(message, true);
    logEntry(message);
}

void MainWindow::setStatusMessage(const QString& message, bool isSafe)
{
    const QString color = isSafe ? "#90EE90" : "#FF7A7A";
    if (statusLabel) {
        statusLabel->setText(QString("<span style='color:%1;'>%2</span>").arg(color, message));
    }
}

void MainWindow::logEntry(const QString& message)
{
    if (logArea) {
        logArea->appendPlainText(message);
    }
}
