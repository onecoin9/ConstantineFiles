#ifndef ACR0VIEWTESTER_H
#define ACR0VIEWTESTER_H

// 1. 系统和Qt基础头文件
#pragma execution_character_set("utf-8")
#pragma comment(lib, "Qt5Xlsx.lib")
#pragma comment(lib, "Qt5Xlsxd.lib")

#include <QtWidgets/QMainWindow>
#include <QtCore/qprocess.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include <QDateTime>

// 2. Qt UI相关头文件
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QScrollArea>
#include <QScreen>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QFileDialog>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

// 3. Qt网络相关头文件
#include <QNetworkInterface>
#include <QAxObject>

// 4. 自定义组件头文件
#include "ui_acroViewTester.h"
#include "testerDefinitions.h"
#include "directTriggerMenu.h"
#include "setting.h"
#include "ProductInfoModel.h"
#include "testsite.h"
#include "testSiteGrid.h"
#include "blockDetailDialog.h"
#include "measureDialog.h"
#include "configDialog.h"
#include "version.h"
#include "inputDialog.h"

// 5. 网络通信相关头文件
#include "tcpIpModule.h"
#include "jsonRpcClient.h"
#include "tcphandler.h"
#include "handlerController.h"

// 6. 自动化管理相关头文件
#include "./AutomaticManager/ACAutomaticManager.h"

// 7. 标准库
#include <map>

class acroViewTester : public QMainWindow
{
    Q_OBJECT

public:
    // 构造和析构
    acroViewTester(QWidget* parent = nullptr);
    ~acroViewTester();

    // 数据结构定义
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

    struct ViceViewControls {
        QVector<QPair<QLabel*, QLabel*>> pathLabels;
    };

private:
    // 1. 初始化相关函数
    void initEssentials();
    void initNonEssentials();
    void initForm();
    void initProductModel();
    void initStatusBar();
    void initAutoMatic();
    void initTabContent(int index);
    void initBasicTab();    
    void initDatabaseTab(); 
    void initAlarmTab();    

    // 2. UI设置相关函数
    void setupTestSites();
    void setupMenuBar();
    void setupExpandButton();
    void setupViceView();
    void setupMeasurementDialog();
    void setupTableView();
    void setupDataUI();
    void setupTestButtons();

    // 3. 视图更新函数
    void createViewMenu();
    void updateExpandButton(bool expanded);
    void updateTableViewAlarmData();
    void updateTime();
    void updateProductInfo(const QString& key, const QString& value);
    void updateGlobalCmd(const QString& selectedOption);

    // 4. 配置和设置相关函数
    void loadViewSettings();
    void saveViewSettings();
    void loadSettings();
    void loadStyle(const QString& qssFile);
    void loadComboBoxItems();
    void loadDoJobComboBoxItems();

    // 5. 数据处理函数
    void addSampleData();
    void addLegendToGroupBox();
    void addItemsToHandlers(const QList<QString>& items);
    void mergeModels(const QList<QStandardItemModel*>& models, QStandardItemModel* targetModel);
    QJsonObject collectAllBlocksStatus();
    void exportToExcel(const QString& fileName);

    // 6. UI创建函数
    QWidget* createContentWidget(int index);
    QWidget* createStatusLegend();
    void createPathLabel(const QString& label, const QString& value, int row);

    // 7. 网络通信相关函数
    void connectToJsonRpcServer();
    void handleJsonRequest(const QString& methodName, const QJsonObject& params);
    QString getLocalIPAddress();

    // 8. 命令发送函数
    void sendLoadProject();
    void sendStartService();
    void sendSiteScanAndConnect();
    void sendDoJob();
    void sendGetAdapterEn();
    void sendDoCustom();
    void sendLogInterface();
    void sendEventInterface();
    void sendGetJobResult();
    void sendGetProgrammerInfo();
    void sendGetProjectInfo();
    void sendGetSiteStatus();
    void sendBlocksStatus();
    void sendJsonRpcData();

    // 9. 辅助功能函数
    QString getCurrentVersion();
    void clearViceViewControls();
    void showWidgetA();
    void showAgingTestControls();
    void showAG06Controls();
    void showAP8000Controls();
    void openFileApp();
    void openConfigDialog();

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
	TcpHandler tcphanlder;
	HandlerController handlercontroller;
	bool m_isExpanded;
	struct ViceViewControls {
		QVector<QPair<QLabel*, QLabel*>> pathLabels;
	};
	ViceViewControls m_viceViewControls;
	ConfigDialog* m_configDialog = nullptr;
	IAutomatic* mAutomatic;
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