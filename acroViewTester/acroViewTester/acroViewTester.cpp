#include "acroViewTester.h"
#include "logManager.h" 
#include <iostream>
using namespace std;
#include "windows.h"
#include <thread>
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"  // support for loading levels from the environment variable
#include "spdlog/fmt/ostr.h" // support for user defined types
#include "spdlog/cfg/env.h"

acroViewTester::acroViewTester(QWidget* parent)
    : QMainWindow(parent)
    , settings("AcroView", "AcroViewTester")
{
    m_tabsInitialized = QVector<bool>(6, false);

    // 基础UI初始化，这会调用 spdlogTest()，其中 LogManager 被完整初始化
    initEssentials();

    // 将 tableMain 的模型设置为 LogManager 提供的模型
    ui.tableMain->setModel(LogManager::instance().getLogModel());
    // 构造函数中不再需要单独调用 initializeDB

    QTimer::singleShot(0, this, [this]() {
        initNonEssentials();
    });

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        updateTime();
        if (!jsonRpcClient.isConnected()) {
            connectToJsonRpcServer();
        }
    });
    timer->start(1000);
}

acroViewTester::~acroViewTester()
{
    if (directMenu) {
        delete directMenu;
        directMenu = nullptr;
    }

    if (settingDialog_ui) {
        delete settingDialog_ui;
        settingDialog_ui = nullptr;
    }

    viewActions.clear();
    viewWidgets.clear();
    jsonRpcClient.disconnectFromServer();
    tcphanlder.stopServer();
}

void acroViewTester::spdlogTest()
{
    // 使用完整的参数调用 LogManager 初始化
    // 这个调用会处理文件日志和数据库日志的初始化
    LogManager::instance().initialize("logs",               // logDir
                                     "acroViewTesterApp",  // appName
                                     "logs/acro_tester_logs.db", // dbPath
                                     1024 * 1024 * 5,      // maxFileSize (示例值: 5MB)
                                     10);                  // maxFiles (示例值: 10个文件)
                                     // 请根据 LogManager::initialize 的实际参数调整

    QObject::connect(&LogManager::instance(), &LogManager::aboutToShutdown, []() {
        LOG_INFO("Performing final cleanup before shutdown...");
    });
}

void acroViewTester::initEssentials()
{
    ui.setupUi(this);
    spdlogTest(); // LogManager 在这里被初始化
    QList<QTabWidget*> allTabWidgets = findChildren<QTabWidget*>();
    for (QTabWidget* tab : allTabWidgets) {
        tab->setCurrentIndex(0);
    }
    initStatusBar();
    initForm();
    loadViewSettings();
    //tcphanlder.startServer();
    //handlercontroller.sendAlarm();
}

void acroViewTester::initNonEssentials()
{
    // 只初始化当前显示的tab
    int currentIndex = ui.tabWidgetMainView->currentIndex();
    initTabContent(currentIndex);

    // 连接tab切换信号
    connect(ui.tabWidgetMainView, &QTabWidget::currentChanged,
        this, &acroViewTester::initTabContent);
}

void acroViewTester::initTabContent(int index)
{
    // 检查 m_tabsInitialized 的索引是否正确。
    // 如果 case 3 对应 m_tabsInitialized[2]，那么这里应该是 m_tabsInitialized[index]
    // 但根据您的 case 3 对应 m_tabsInitialized[2]，case 4 对应 m_tabsInitialized[4]
    // 看起来 m_tabsInitialized 的索引与 tab index 可能存在偏移或不一致。
    // 假设 m_tabsInitialized 的索引与 case 语句中的预期行为一致：
    // case 0 -> m_tabsInitialized[0]
    // case 1 -> m_tabsInitialized[1]
    // case 3 -> m_tabsInitialized[2]  <-- 注意这个映射
    // case 4 -> m_tabsInitialized[4]
    // case 5 -> m_tabsInitialized[5]
    // 为了清晰，我们直接使用一个映射，或者确保 m_tabsInitialized 的索引与 tab index 一致。
    // 假设 m_tabsInitialized 的索引就是 tab 的实际索引，除了 case 3 映射到 m_tabsInitialized[2]
    
    bool alreadyInitialized = false;
    if (index == 3) { // 特殊处理 case 3 的 m_tabsInitialized 索引
        if (m_tabsInitialized.size() > 2 && m_tabsInitialized[2]) {
            alreadyInitialized = true;
        }
    } else if (index < m_tabsInitialized.size() && m_tabsInitialized[index]) {
        alreadyInitialized = true;
    }


    if (alreadyInitialized) {
        // 如果是日志标签页 (index == 3)，即使已初始化，也可能需要刷新视图
        if (index == 4) {
            LogManager::instance().refreshLogView(); // 确保视图是最新的
        }
        return;  // 如果已经初始化过，直接返回
    }

    switch (index) {
    case 0:  // 主页面
        setupTestSites();
        initProductModel();
        setupViceView();
        if (m_tabsInitialized.size() > 0) m_tabsInitialized[0] = true;
        break;

    case 1:
        setupTableView();
        setupExpandButton();
        addLegendToGroupBox();
        if (m_tabsInitialized.size() > 1) m_tabsInitialized[1] = true;
        break;

    case 4:  // 日志页面
        // LogManager 初始化和 setModel 已移至构造函数
        setupDataUI(); // 这个函数应该只包含UI相关的设置，不包括模型设置
        addSampleData(); // 只在第一次加载此tab时添加示例数据
        if (m_tabsInitialized.size() > 4) m_tabsInitialized[4] = true; // 标记为已初始化
        LogManager::instance().refreshLogView(); // 确保初次加载时数据正确显示
        break;
    case 3:
        updateTableViewAlarmData();
        if (m_tabsInitialized.size() > 2) m_tabsInitialized[1] = true;
        break;
    case 5:
        loadComboBoxItems();
        loadDoJobComboBoxItems();
        if (m_tabsInitialized.size() > 5) m_tabsInitialized[5] = true;
        break;
    }
}

void acroViewTester::initForm()
{
    ui.pushButtonStart->setIconSize(QSize(36, 36));
    ui.pushButtonPause->setIconSize(QSize(36, 36));
    ui.pushButtonUser->setIconSize(QSize(36, 36));
    ui.pushButtonSetting->setIconSize(QSize(36, 36));
    ui.pushButtonDatabase->setIconSize(QSize(36, 36));
    ui.pushButtonAlarmInfo->setIconSize(QSize(36, 36));
    setupMenuBar();

    connect(ui.pushButtonSetting, &QToolButton::clicked, this, &acroViewTester::settingTrigger);
    connect(ui.pushButtonDatabase, &QPushButton::clicked, [this]() {
        ui.tabWidgetMainView->setCurrentIndex(1);//后续需要定义宏
        });
    connect(ui.pushButtonAlarmInfo, &QPushButton::clicked, [this]() {
        ui.tabWidgetMainView->setCurrentIndex(2);//后续需要定义宏
        });

    QWidget* firstTab = ui.tabWidgetMainView->widget(0);
    if (!firstTab->layout()) {
        QVBoxLayout* layout = new QVBoxLayout(firstTab);
        firstTab->setLayout(layout);
    }
    jsonRpcResultModel = new QStandardItemModel(0, 2, this);
    jsonRpcResultModel->setHorizontalHeaderLabels({ "Attribute", "Value" });
    jsonRpcResultModel1 = new QStandardItemModel(0, 2, this);
    jsonRpcResultModel1->setHorizontalHeaderLabels({ "Attribute", "Value" });
    ui.tableViewJsonRpcResult->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    for (int i = 0; i < 8; ++i) {
        QStandardItemModel* model = new QStandardItemModel(this);
        model->setColumnCount(2);
        model->setHorizontalHeaderLabels({ "Attribute", "Value" });
        jsonModels.append(model);
        jsonModels1.append(model);
    }
    currentModelIndex = 0;
    currentModelIndex1 = 0;
    connect(ui.pushButtonSite1, &QPushButton::clicked, this, [=]() { onPushButtonSiteClicked(0); });
    connect(ui.pushButtonSite2, &QPushButton::clicked, this, [=]() { onPushButtonSiteClicked(1); });
    connect(ui.pushButtonSite3, &QPushButton::clicked, this, [=]() { onPushButtonSiteClicked(2); });
    connect(ui.pushButtonSite4, &QPushButton::clicked, this, [=]() { onPushButtonSiteClicked(3); });
    connect(ui.pushButtonSite5, &QPushButton::clicked, this, [=]() { onPushButtonSiteClicked(4); });
    connect(ui.pushButtonSite6, &QPushButton::clicked, this, [=]() { onPushButtonSiteClicked(5); });
    connect(ui.pushButtonSite7, &QPushButton::clicked, this, [=]() { onPushButtonSiteClicked(6); });
    connect(ui.pushButtonSite8, &QPushButton::clicked, this, [=]() { onPushButtonSiteClicked(7); });
    connect(ui.pushButtonJsonRpcResult, &QPushButton::clicked, this, [=]() { onpushButtonJsonRpcResultClicked(); });

    //setupMeasurementDialog();
    connect(ui.pushButtonStart, &QPushButton::clicked, this, &acroViewTester::onPushButtonStartClicked);
    connect(ui.pushButtonPause, &QPushButton::clicked, this, &acroViewTester::openFileApp);
    connect(ui.pushButtonSendJsonRpc, &QPushButton::clicked, this, &acroViewTester::sendJsonRpcData);
    connect(ui.pushButtonOpenConfigDialog, &QPushButton::clicked, this, &acroViewTester::openConfigDialog);
    ui.jsonRpcResultLabel->setText("Waiting for JSON-RPC response...");

    connect(&jsonRpcClient, &JsonRpcClient::connected, this, &acroViewTester::onJsonRpcConnected);
    connect(&jsonRpcClient, &JsonRpcClient::disconnected, this, &acroViewTester::onJsonRpcDisconnected);
    connect(&jsonRpcClient, &JsonRpcClient::socketError, this, &acroViewTester::onJsonRpcSocketError);
    connect(&jsonRpcClient, &JsonRpcClient::protocolError, this, &acroViewTester::onJsonRpcProtocolError);
    connect(&jsonRpcClient, &JsonRpcClient::serverCommandReceived, this, &acroViewTester::onJsonRpcServerCommandReceived);
    connect(&jsonRpcClient, &JsonRpcClient::responseReceived, this, &acroViewTester::onJsonRpcResponseReceived);
}

void acroViewTester::onPushButtonSiteClicked(int index)
{
    QString resultText = "Parsed JSON Results for Index " + QString::number(index + 1);
    ui.labelJsonRpcResult->setText(resultText);
    if (index >= 0 && index < currentModelIndex)
    {
        currentModelIndex = index;
        ui.tableViewJsonRpcResult->setModel(jsonModels[index]);
    }
}


void acroViewTester::onpushButtonJsonRpcResultClicked()
{
    QString resultText = "解析结果:";
    ui.labelJsonRpcResult->setText(resultText);
    ui.tableViewJsonRpcResult->setModel(jsonRpcResultModel);
}


void acroViewTester::mergeModels(const QList<QStandardItemModel*>& models, QStandardItemModel* targetModel)
{
    for (QStandardItemModel* model : models) {
        for (int i = 0; i < model->rowCount(); ++i) {
            QList<QStandardItem*> items;
            for (int j = 0; j < model->columnCount(); ++j) {
                items.append(model->item(i, j)->clone());
            }
            targetModel->appendRow(items);
        }
    }
}


void acroViewTester::loadSettings()
{
    QSettings settings("AcroView", "acroViewTester");
    int rows = settings.value("Grid/SiteRows", 2).toInt();
    int cols = settings.value("Grid/SiteCols", 2).toInt();
    int baseRow = settings.value("Grid/BaseRows", 3).toInt();
    int baseCol = settings.value("Grid/BaseCols", 3).toInt();

}

unsigned int CalculateCRC32MPEG2(const std::vector<unsigned char>& data) {
    unsigned int crc = 0xFFFFFFFF; // 初始值
    unsigned int polynomial = 0x04C11DB7;

    for (unsigned char byte : data) {
        crc ^= (byte << 24); // 将当前字节移到最高位
        for (int i = 0; i < 8; ++i) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ polynomial;
            }
            else {
                crc <<= 1;
            }
        }
        crc &= 0xFFFFFFFF; // 保持 CRC 为 32 位
    }
    return crc; // 不取反
}

void acroViewTester::initStatusBar()
{

    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    ipLabel = new QLabel(this);
    ipLabel->setStyleSheet("color: #333333; font-weight: bold;");
    statusBar->addWidget(ipLabel, 1);

    QFrame* separator = new QFrame(this);
    separator->setFrameStyle(QFrame::HLine | QFrame::Plain);
    separator->setStyleSheet("border: 1px solid #cccccc;");
    statusBar->addPermanentWidget(separator);

    QWidget* rightPanel = new QWidget(this);
    QHBoxLayout* rightLayout = new QHBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(10);

    versionLabel = new QLabel(this);
    versionLabel->setStyleSheet("color: #666666;font-size: 14px;font-weight: bold;");
    rightLayout->addWidget(versionLabel);

    timeLabel = new QLabel(this);
    timeLabel->setStyleSheet("color: #666666; font-size: 14px;font-weight: bold;");
    rightLayout->addWidget(timeLabel);

    userLabel = new QLabel(this);
    userLabel->setStyleSheet("color: #666666;font-weight: bold;");
    rightLayout->addWidget(userLabel);

    statusBar->addPermanentWidget(rightPanel);

    ipLabel->setText("IP: " + getLocalIPAddress());
    versionLabel->setText("v" + getCurrentVersion());
    userLabel->setText("用户: Admin");

    updateTime();

}
void acroViewTester::loadDoJobComboBoxItems()
{
    QList<ItemText> items;
    items.append({ "" }); // 添加空选项
    items.append({ "Program" });
    items.append({ "Erase" });
    items.append({ "Verify" });
    items.append({ "BlankCheck" });
    items.append({ "Secure" });
    items.append({ "Read" });

    ui.comboBoxDoJobJson->clear();
    for (const auto& item : items) {
        ui.comboBoxDoJobJson->addItem(item.text);
    }

    // 不设置默认选项，保持为空
    ui.comboBoxDoJobJson->setCurrentIndex(0);

    // 连接信号槽，当选项改变时更新全局变量
    connect(ui.comboBoxDoJobJson, &QComboBox::currentTextChanged, this, &acroViewTester::onComboBoxDoJobJsonChanged);
}

void acroViewTester::updateTime() {
    QDateTime current = QDateTime::currentDateTime();
    timeLabel->setText(current.toString("yyyy-MM-dd hh:mm:ss"));
}

void acroViewTester::setupTestSites()
{
    QSettings settings("AcroView", "acroViewTester");
    int siteRows = settings.value("Grid/SiteRows", 2).toInt();
    int siteCols = settings.value("Grid/SiteCols", 2).toInt();
    int baseRow = settings.value("Grid/BaseRows", 3).toInt();
    int baseCol = settings.value("Grid/BaseCols", 3).toInt();
    qDeleteAll(m_testSites);
    m_testSites.clear();
    m_testSites.reserve(baseRow * baseCol);
    QWidget* scrollAreaContent = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(scrollAreaContent);
    gridLayout->setSpacing(2);  // 设置较小的间距
    gridLayout->setContentsMargins(0, 0, 0, 0);

    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();

    // 根据站点数量调整高度
    int baseHeight = (screenGeometry.height() - 300) / 10;
    int fixedHeight = baseHeight;
    if (siteRows * siteCols <= 8)
    {
        fixedHeight = baseHeight * 4;
    }
    else if (siteRows * siteCols <= 16)
    {
        fixedHeight = baseHeight * 3;
    }
    else if (siteRows * siteCols <= 32)
    {
        fixedHeight = baseHeight * 2;
    }

    fixedHeight = qBound(60, fixedHeight, 240);

    int testSiteDirection = settings.value("Grid/SiteDirection", 0).toInt();

    switch (testSiteDirection)
    {
    case 0:
        for (int i = 0; i < siteRows * siteCols; ++i) {
            TestSite* site = new TestSite(i + 1, this);
            connect(site, &TestSite::startClicked, this, &acroViewTester::onPushButtonStartClicked);
            site->setFixedHeight(fixedHeight);
            m_testSites.append(site);
            gridLayout->addWidget(site, i / siteCols, i % siteCols);
        }
        break;
    case 1:
        for (int i = 0; i < siteRows * siteCols; ++i) {
            TestSite* site = new TestSite(siteRows * siteCols - i, this);
            site->setFixedHeight(fixedHeight);
            m_testSites.append(site);
            gridLayout->addWidget(site, i / siteCols, i % siteCols);
        }
        break;
    default:
        break;
    }

    scrollAreaContent->setLayout(gridLayout);
    ui.scrollArea->setWidget(scrollAreaContent);
    ui.scrollArea->setWidgetResizable(true);
}

void acroViewTester::onSettingsChanged()
{
    setupTestSites();
}

void acroViewTester::initProductModel()
{
    productModel = new ProductInfoModel(this);
    ui.listViewProdInfo->setStyleSheet(
        "QListView {"
        "    background-color: rgb(0, 32, 48);"
        "    color: white;"
        "    border: none;"
        "}"
        "QListView::item {"
        "    height: 25px;"
        "    padding: 5px;"
        "}"
        "QListView::item:selected {"
        "    background-color: rgb(0, 48, 72);"
        "}"
    );

    ui.listViewProdInfo->setModel(productModel);
    ui.listViewProdInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.listViewProdInfo->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.listViewProdInfo->setSpacing(1);
    connect(ui.listViewProdInfo->selectionModel(), &QItemSelectionModel::currentChanged,
        this, &acroViewTester::onProductItemSelected);
}

void acroViewTester::onProductItemSelected(const QModelIndex& current, const QModelIndex& previous)
{
    if (current.isValid()) {
        QString itemText = current.data(Qt::DisplayRole).toString();
    }
}

void acroViewTester::updateProductInfo(const QString& key, const QString& value)
{
    if (productModel) {
        productModel->updateValue(key, value);
    }
}

void acroViewTester::onGridSizeChanged(int rows, int cols, int baseRows, int baseCols)
{
    QSettings settings("AcroView", "acroViewTester");

    settings.setValue("Grid/SiteRows", rows);
    settings.setValue("Grid/SiteCols", cols);
    settings.setValue("Grid/BaseRows", baseRows);
    settings.setValue("Grid/BaseCols", baseCols);
    settings.sync();
    try {
        setupTestSites();

        if (ui.scrollArea->widget()) {

            int width = cols * 200 + (cols - 1) * 5;
            int height = rows * 200 + (rows - 1) * 5;
            ui.scrollArea->widget()->setMinimumSize(width, height);
            ui.scrollArea->widget()->updateGeometry();
        }

        qDebug() << "Grid size updated successfully:";
        qDebug() << "Rows:" << rows << "Cols:" << cols;
        qDebug() << "Total TestSites:" << m_testSites.size();

    }
    catch (const std::exception& e) {
        qDebug() << "Error updating grid size:" << e.what();
        QMessageBox::critical(this, tr("Error"),
            tr("Failed to update grid size: %1").arg(e.what()));
    }
}

