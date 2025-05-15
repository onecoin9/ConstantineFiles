#include "acroViewTester.h"
#include <iostream>
using namespace std;
#include "windows.h"
#include <thread>
acroViewTester::acroViewTester(QWidget* parent)
    : QMainWindow(parent)
    , settings("AcroView", "AcroViewTester")
{
    ui.setupUi(this);
    initForm();
    initAutoMatic();
    loadViewSettings();
    initProductModel();
    setupTableView();
    setupTestSites();
    loadComboBoxItems();
    loadDoJobComboBoxItems();
    setupExpandButton();
    addLegendToGroupBox();
    setupViceView();
    setupDataUI();
    addSampleData();
    initStatusBar();
    updateTableViewAlarmData();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &acroViewTester::updateTime);
    timer->start(1000);
    
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &acroViewTester::connectToJsonRpcServer);
    timer->start(1000);
    //m_configDialog = new ConfigDialog(this);

   // 使用volatile防止编译器优化掉除零操作
    //volatile int denominator = 0;

    //cout << "程序开始运行..." << endl;

    //// 故意触发除零异常
    //int result = 42 / denominator;  // 这里会触发异常

    //// 这行代码永远不会被执行
    //cout << "程序正常结束。" << endl;

    //// 添加以下代码防止程序提前退出
    //while (true) {
    //    Sleep(5);

    //    int* a = NULL;
    //    *a = 2;
    //}
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

