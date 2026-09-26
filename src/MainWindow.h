#pragma once

#include "SettingsModel.h"

#include <QMainWindow>
#include <QList>

class QCheckBox;
class QFrame;
class QGridLayout;
class QLineEdit;
class QLabel;
class QPlainTextEdit;
class QProgressBar;
class QPushButton;
class QVBoxLayout;
class QWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void handleValidatePath();
    void handleScan();
    void handleRestorePoint();

private:
    void setupUi();
    void setupNavigation();
    void updateSettingsSummary();
    void setNavigationSelection(QPushButton* activeButton);
    void setStatusMessage(const QString& message, bool isSafe);
    void logEntry(const QString& message);

    QLabel* titleLabel = nullptr;
    QLabel* statusLabel = nullptr;
    QProgressBar* scanProgress = nullptr;
    QLabel* scoreLabel = nullptr;
    QLineEdit* pathInput = nullptr;
    QPushButton* validateButton = nullptr;
    QPushButton* scanButton = nullptr;
    QPushButton* restoreButton = nullptr;
    QCheckBox* realTimeCheck = nullptr;
    QCheckBox* autoBackupCheck = nullptr;
    QCheckBox* silentModeCheck = nullptr;
    QLabel* settingsSummaryLabel = nullptr;
    QPlainTextEdit* logArea = nullptr;

    AppSettings settingsModel;
    QList<QPushButton*> navigationButtons;
};
