#ifndef ACR0VIEWTESTER_H
#define ACR0VIEWTESTER_H
#pragma execution_character_set("utf-8")
#pragma comment(lib, "Qt5Xlsx.lib")
#pragma comment(lib, "Qt5Xlsxd.lib")

#include <QtWidgets/QMainWindow>
#include "ui_acroViewTester.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QScrollArea>
#include <QStandardItemModel>
#include <QScreen>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QDateTime>
#include <QFileDialog>
#include <QAxObject>
#include <QTimer>
#include <QNetworkInterface>
#include <map>
#include <QProgressDialog> 
#include "testerDefinitions.h"
#include "directTriggerMenu.h"
#include "setting.h"
#include "ProductInfoModel.h"
#include "testsite.h"
#include "testSiteGrid.h"
#include "blockDetailDialog.h"
#include "measureDialog.h"
#include "tcpIpModule.h"
#include "jsonRpcClient.h"
#include "configDialog.h"
#include "version.h"
#include"inputDialog.h"
#include <QtCore/qprocess.h>
#include "./AutomaticManager/ACAutomaticManager.h"
#include <QTableWidget>
class acroViewTester : public QMainWindow
{
    Q_OBJECT

public:
    acroViewTester(QWidget* parent = nullptr);
    ~acroViewTester();

    struct ProductTestInfo {
        QString customerCode;
        QString productModel;
        QString weekly;
        QString customerBatch;
        QString workOrder;
        QString workStep;
        QString process;
        QString deviceNumber;
    };

    struct ItemText {
        QString text;
    };

private:
    // 初始化相关函数
    void initEssentials(); // 必要初始化
    void initNonEssentials(); // 非必要初始化
    void initTabContent(int index);
    void initBasicTab();    // Tab 0
    void initDatabaseTab(); // Tab 1
    void initAlarmTab();    // Tab 2
    
    bool m_tabsInitialized[6] = {false, false, false,false, false, false };  // 记录每个tab是否已初始化
    
    void initForm();
    void initProductModel();
    void initStatusBar();
    void setupTestSites();
    void setupMenuBar();
    void setupExpandButton();
    void setupViceView();
    void setupMeasurementDialog();

    // UI 相关函数
    void createViewMenu();
    void setupTableView();
    void setupDataUI();
    void updateExpandButton(bool expanded);
    void updateTableViewAlarmData();
    QWidget* createContentWidget(int index);
    QWidget* createStatusLegend();
    void addLegendToGroupBox();
    void showWidgetA();

    // 配置和设置相关函数
    void loadViewSettings();
    void saveViewSettings();
    void loadSettings();
    void loadStyle(const QString& qssFile);
    void loadComboBoxItems();
    void addSampleData();
    QString getLocalIPAddress();
    void clearViceViewControls();
    void createPathLabel(const QString& label, const QString& value, int row);
    QString getCurrentVersion();
    void showAgingTestControls();
    void showAG06Controls();
    void showAP8000Controls();
    // 数据处理相关函数
    void updateProductInfo(const QString& key, const QString& value);
    void mergeModels(const QList<QStandardItemModel*>& models, QStandardItemModel* targetModel);
    void addItemsToHandlers(const QList<QString>& items);
    QJsonObject collectAllBlocksStatus();
    void exportToExcel(const QString& fileName);

    // JSON-RPC 相关函数
    void handleJsonRequest(const QString& methodName, const QJsonObject& params);
    void connectToJsonRpcServer();
    void initAutoMatic();
    void updateGlobalCmd(const QString& selectedOption);
    void loadDoJobComboBoxItems();
    // 发送命令相关函数
    void sendLoadProject();
    void sendStartService();
    void sendSiteScanAndConnect();
    void sendDoJob();
    void sendGetAdapterEn();
    //void sendSetAdapterEn();
    void sendDoCustom();
    void sendLogInterface();
    void sendEventInterface();
    void sendGetJobResult();
    void sendGetProgrammerInfo();
    void sendGetProjectInfo();
    void sendGetSiteStatus();
    void sendBlocksStatus();

private slots:
    // UI 事件处理
    void on_pushButtonExpand_clicked();
    void on_pushButtonSendUID_clicked();
    void on_exportButton_clicked();
    void onPushButtonSiteClicked(int index);
    void onpushButtonJsonRpcResultClicked();
    void onPushButtonStartClicked();
    void onSettingsChanged();
    void onGridSizeChanged(int rows, int cols, int baseRows, int baseCols);
    void onViewActionToggled(bool checked);
    void onProductItemSelected(const QModelIndex& current, const QModelIndex& previous);
    void onSceneChanged(const QString& scene);

    // JSON-RPC 事件处理
    void onJsonRpcConnected();
    void onJsonRpcDisconnected();
    void onJsonRpcSocketError(QAbstractSocket::SocketError error);
    void onJsonRpcProtocolError(const QString& errorString);
    void onJsonRpcServerCommandReceived(const QString& cmd, const QJsonObject& data);
    void onJsonRpcResponseReceived(qint64 id, const QJsonValue& result);

    // 其他事件处理
    void updateTime();
    void openFileApp();
    void sendJsonRpcData();
    void settingTrigger();
    void handleMenuTrigger();
    void handleAutomatic();
    //页面处理
    void openConfigDialog();
    void onComboBoxDoJobJsonChanged(const QString& selectedOption);
private:
    // 成员变量
    Ui::acroViewTesterClass ui;
    DirectTriggerMenu* directMenu;
    settingDialog* settingDialog_ui = nullptr;
    QSettings settings;
    QMenu* viewMenu;
    QStatusBar* statusBar;
    QLabel* ipLabel;
    QLabel* versionLabel;
    QLabel* timeLabel;
    QLabel* userLabel;
    QLabel* m_statusDot;
    QLabel* m_arrowIcon;
    QLabel* jsonRpcResultLabel;
    QTimer* timer;
    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;
    QWidget* m_widgetA;
    QWidget* m_scrollContent;
    QTableView* tableViewAlarmData;
    ProductInfoModel* productModel;
    QStandardItemModel* dataModel;
    QStandardItemModel* jsonRpcResultModel;
    QStandardItemModel* jsonRpcResultModel1;
    QList<QStandardItemModel*> jsonModels;
    QList<QStandardItemModel*> jsonModels1;
    QMap<QString, QAction*> viewActions;
    QMap<QString, QWidget*> viewWidgets;
    QVector<TestSite*> m_testSites;
    TcpIpModule tcpModule;
    JsonRpcClient jsonRpcClient;
    std::map<QString, std::function<void()>> handlers;
    ProductTestInfo testInfo;
    int currentModelIndex;
    int currentModelIndex1;
    bool m_isExpanded;
    struct ViceViewControls {
        QVector<QPair<QLabel*, QLabel*>> pathLabels;
    };
    ViceViewControls m_viceViewControls;
    ConfigDialog* m_configDialog = nullptr;
    IAutomatic *mAutomatic;
    DeviceInfo deviceInfo;
    JobResult jobResult;
    ProjectInfo projectInfo;
private slots:
        void onbtnQuerySiteMappingClicked();
        void onSetTaskClicked();
        void onTellDevReadyClicked();
        void onSetDoneSiteClicked();

private:

    // 控件声明
    QTableWidget* tableWidgetSiteMapping;
    QLineEdit* lineEditTaskInfo;
    QTextEdit* textEditDevReadyInfo;
    QLineEdit* lineEditSiteIndex;
    QLineEdit* lineEditBurnResult;

    //测试dump
    private slots:
        void onTestNullPointer();
        void onTestArrayOverflow();
        void onTestDivideByZero();
        void onTestStackOverflow();

private:
    void setupTestButtons();

};

#endif