#include "configDialog.h"
#include "ui_ConfigDialog.h"
#include <QTableWidgetItem>
#include <QtWidgets>
#include <QtWidgets/qmessagebox.h>
#pragma execution_character_set("utf-8")
ConfigDialog::ConfigDialog(QWidget* parent) : QWidget(parent),
ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);

    // 连接 btnInport 按钮
    connect(ui->btnInport, &QPushButton::clicked, this, [=]()
        { on_btnInport_clicked("btnInport"); });

    // 连接 btnInportSpec 按钮
    connect(ui->btnInportSpec, &QPushButton::clicked, this, [=]()
        { on_btnInport_clicked("btnInportSpec"); });
    // 设置样式表
    tableStyleSheet = R"(
    QTableWidget {
        color: #000000; /* 单元格文字颜色改为黑色 */
        background: #f8f8f8;
        border: 1px solid #242424;
        alternate-background-color: #525252; /* 交替行颜色 */
        gridline-color: #242424;
    }
    QTableWidget::item:selected {
        color: #000000; /* 选中项文字颜色改为黑色 */
        background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #484848, stop:1 #383838);
    }
    QTableWidget::item:hover {
        background: #5B5B5B;
    }
    QHeaderView::section {
        text-align: center;
        background: #c5e3f6; /* 新的表头背景色 */
        padding: 3px;
        margin: 0px;
        color: #000000; /* 表头文字颜色改为黑色 */
        border: 1px solid #242424;
        border-left-width: 0;
    }
    QScrollBar:vertical {
        background: #484848;
        padding: 0px;
        border-radius: 6px;
        max-width: 12px;
    }
    QScrollBar::handle:vertical {
        background: #CCCCCC;
    }
    QScrollBar::handle:hover:vertical, QScrollBar::handle:pressed:vertical {
        background: #A7A7A7;
    }
    QScrollBar::sub-page:vertical {
        background: #444444;
    }
    QScrollBar::add-page:vertical {
        background: #5B5B5B;
    }
    QScrollBar::add-line:vertical {
        background: none;
    }
    QScrollBar::sub-line:vertical {
        background: none;
    }
    QTableWidget::item:selected {
        background: #e2f3f5; /* 选中项背景色 */
    }
    QTableWidget::item:hover {
        background: #88bef5; /* 悬浮项背景色 */
    }
    QTableWidget {
        alternate-background-color: #43dde6; /* 交替行背景色 */
    }
    QTableWidget::item {
        color: #000000; /* 单元格文字颜色改为黑色 */
    }
)";

    initTableFlow();
    initTableSpec();
    initTableBin();
    initTablePin();
    //connect(ui->btnExportFlow, &QPushButton::clicked, this, &ConfigDialog::on_btnExportFlow_clicked);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::initTableFlow()
{
    ui->tableFlow->setRowCount(0);
    ui->tableFlow->setColumnCount(9);

    QStringList headers = { "序号","ID", "Class Name", "Alias", "Loop Count", "Test Mode", "Pass Do", "Fail Do", "Comment" };
    ui->tableFlow->setHorizontalHeaderLabels(headers);

    ui->tableFlow->horizontalHeader()->setStretchLastSection(true);
    ui->tableFlow->verticalHeader()->setVisible(false);
    ui->tableFlow->setColumnWidth(2, 200);

    ui->tableFlow->setStyleSheet(tableStyleSheet);
}

void ConfigDialog::initTableSpec()
{
    ui->tableSpec->setRowCount(0);
    ui->tableSpec->setColumnCount(9);  // 减少列数

    QStringList headers = { 
        "ID", 
        "TestItemName", 
        "Alias Name", 
        "Params",      
        "Value",
        "Lower Limit", // 新增下限列
        "Upper Limit", // 新增上限列
        "Bin Name", 
        "Comment" 
    };
    ui->tableSpec->setHorizontalHeaderLabels(headers);

    ui->tableSpec->horizontalHeader()->setStretchLastSection(true);
    ui->tableSpec->verticalHeader()->setVisible(false);
    ui->tableSpec->setColumnWidth(1, 200);
    ui->tableSpec->setColumnWidth(5, 100); 
    ui->tableSpec->setColumnWidth(6, 100); 
    ui->tableSpec->setStyleSheet(tableStyleSheet);
}
void ConfigDialog::updateTableFlow()
{
    // 暂时断开信号连接
    disconnect(ui->tableFlow, &QTableWidget::cellChanged, this, &ConfigDialog::on_tableFlowCellChanged);

    ui->tableFlow->setRowCount(testItems.size());
    for (int i = 0; i < testItems.size(); ++i)
    {
        QTableWidgetItem* indexItem = new QTableWidgetItem(QString::number(i + 1));
        indexItem->setTextAlignment(Qt::AlignCenter);
        indexItem->setFlags(indexItem->flags() & ~Qt::ItemIsEditable); // 禁止编辑序号列
        ui->tableFlow->setItem(i, 0, indexItem);
        const TestItem& item = testItems[i];
        QTableWidgetItem* idItem = new QTableWidgetItem(item.id);
        QTableWidgetItem* classNameItem = new QTableWidgetItem(item.className);
        QTableWidgetItem* aliasItem = new QTableWidgetItem(item.alias);
        QTableWidgetItem* loopCountItem = new QTableWidgetItem(item.loopCount);
        QTableWidgetItem* passDoItem = new QTableWidgetItem(item.passDo);
        QTableWidgetItem* failDoItem = new QTableWidgetItem(item.failDo);
        QTableWidgetItem* comment = new QTableWidgetItem(item.comment);

        idItem->setTextAlignment(Qt::AlignCenter);
        classNameItem->setTextAlignment(Qt::AlignCenter);
        aliasItem->setTextAlignment(Qt::AlignCenter);
        loopCountItem->setTextAlignment(Qt::AlignCenter);
        passDoItem->setTextAlignment(Qt::AlignCenter);
        failDoItem->setTextAlignment(Qt::AlignCenter);
        comment->setTextAlignment(Qt::AlignCenter);

        ui->tableFlow->setItem(i, 1, idItem);
        ui->tableFlow->setItem(i, 2, classNameItem);
        ui->tableFlow->setItem(i, 3, aliasItem);
        ui->tableFlow->setItem(i, 4, loopCountItem);
        ui->tableFlow->setItem(i, 6, passDoItem);
        ui->tableFlow->setItem(i, 7, failDoItem);
        ui->tableFlow->setItem(i, 8, comment);

        QComboBox* comboBox = new QComboBox(this);
        comboBox->addItems({ "Skip", "Test" });    // Add options for the combo box
        comboBox->setCurrentText(item.testMode); // Set the current text to the item's test mode

        // Set the color based on the current text
        auto updateComboBoxColor = [comboBox](const QString& mode)
        {
            if (mode == "Test")
            {
                comboBox->setStyleSheet("QComboBox { color: green; }");
            }
            else if (mode == "Skip")
            {
                comboBox->setStyleSheet("QComboBox { color: red; }");
            }
        };

        // Apply the initial color
        updateComboBoxColor(item.testMode);

        connect(comboBox, &QComboBox::currentTextChanged, this, [=](const QString& mode)
            {
                updateComboBoxColor(mode); // Update the color when the text changes
                on_tableFlowTestModeChanged(i, mode); });

        ui->tableFlow->setCellWidget(i, 5, comboBox);
    }

    // 恢复信号连接
    connect(ui->tableFlow, &QTableWidget::cellChanged, this, &ConfigDialog::on_tableFlowCellChanged);
}

void ConfigDialog::populateTableSpecRow(int row, const SpecItem& item)
{
    ui->tableSpec->setItem(row, 0, new QTableWidgetItem(item.id));
    ui->tableSpec->setItem(row, 1, new QTableWidgetItem(item.testItemName));
    ui->tableSpec->setItem(row, 2, new QTableWidgetItem(item.aliasName));
    ui->tableSpec->setItem(row, 3, new QTableWidgetItem(item.params));
    ui->tableSpec->setItem(row, 4, new QTableWidgetItem(item.value));
    ui->tableSpec->setItem(row, 5, new QTableWidgetItem(item.lowerLimit));
    ui->tableSpec->setItem(row, 6, new QTableWidgetItem(item.upperLimit));
    ui->tableSpec->setItem(row, 7, new QTableWidgetItem(item.binName));
    ui->tableSpec->setItem(row, 8, new QTableWidgetItem(item.comment));

}

void ConfigDialog::updateTableSpec()
{
    // 暂时断开信号连接
    disconnect(ui->tableSpec, &QTableWidget::cellChanged, this, &ConfigDialog::on_tableSpecCellChanged);

    ui->tableSpec->setRowCount(specItems.size());
    for (int i = 0; i < specItems.size(); ++i)
    {
        populateTableSpecRow(i, specItems[i]);
    }

    // 恢复信号连接
    connect(ui->tableSpec, &QTableWidget::cellChanged, this, &ConfigDialog::on_tableSpecCellChanged);
}

void ConfigDialog::on_tableFlowTestModeChanged(int row, const QString& mode)
{
    QTableWidgetItem* modeItem = ui->tableFlow->item(row, 4);
    if (!modeItem)
    {
        modeItem = new QTableWidgetItem(mode);
        ui->tableFlow->setItem(row, 4, modeItem);
    }
    else
    {
        modeItem->setText(mode);
    }
}

void ConfigDialog::updateSpecFromFlow(const TestItem& flowItem)
{
    // Parse comment string to get parameters
    QStringList params = parseCommentParams(flowItem.comment);
    
    // Generate spec items based on flow item
    generateSpecItems(flowItem, params);
    
    // Update the spec table
    updateTableSpec();
}

QStringList ConfigDialog::parseCommentParams(const QString& comment)
{
    // Split comment by separator (& or space)
    QStringList params = comment.split(QRegExp("[&\\s]+"), Qt::SkipEmptyParts);
    return params;
}

void ConfigDialog::generateSpecItems(const TestItem& flowItem, const QStringList& params)
{
    // Remove existing spec items for this flow item
    auto it = specItems.begin();
    while (it != specItems.end()) {
        if (it->testItemName == flowItem.className && 
            it->aliasName == flowItem.alias) {
            it = specItems.erase(it);
        } else {
            ++it;
        }
    }

    // Create new spec items for each parameter
    for (int i = 0; i < params.size(); ++i) {
        SpecItem specItem;
        specItem.id = generateSpecItemID(flowItem.id, i);
        specItem.testItemName = flowItem.className;
        specItem.aliasName = flowItem.alias;
        specItem.params = params[i];
        specItem.value = "";
        specItem.binName = QString("Bin_%1").arg(flowItem.id);
        specItem.comment = "";

        specItems.append(specItem);
    }
}

QString ConfigDialog::generateSpecItemID(const QString& baseID, int index)
{
    return QString("%1%2").arg(baseID).arg(QChar('a' + index));
}

void ConfigDialog::syncSpecWithFlow()
{
    for (const TestItem& flowItem : testItems) {
        updateSpecFromFlow(flowItem);
    }
}
void ConfigDialog::on_btnInport_clicked(const QString& buttonId)
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择配置文件"), "", tr("JSON 文件 (*.json *.tester_config)"));
    if (filePath.isEmpty())
    {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("错误"), tr("无法打开文件：%1").arg(filePath));
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    // 判断按钮 ID，决定更新哪个表格
    if (buttonId == "btnInport")
    {
        // 更新 tableFlow
        QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
        if (jsonDoc.isNull() || !jsonDoc.isObject())
        {
            QMessageBox::warning(this, tr("错误"), tr("文件格式错误或内容无效"));
            return;
        }

        QJsonObject rootObj = jsonDoc.object();
        if (rootObj.contains("FlowConfig") && rootObj["FlowConfig"].isObject())
        {
            QJsonObject flowConfig = rootObj["FlowConfig"].toObject();
            if (flowConfig.contains("TestItems") && flowConfig["TestItems"].isArray())
            {
                QJsonArray jsonTestItems = flowConfig["TestItems"].toArray();
                testItems.clear(); // 清空之前的数据

                for (const QJsonValue& value : jsonTestItems)
                {
                    QJsonObject item = value.toObject();
                    TestItem testItem;
                    testItem.id = item.value("TestItemID").toString();
                    testItem.className = item.value("ClassName").toString();
                    testItem.alias = item.value("Alias").toString();
                    testItem.testMode = item.value("TestMode").toString();
                    testItem.loopCount = QString::number(item.value("LoopCount").toInt());
                    testItem.passDo = item.value("PassDo").toString();
                    testItem.failDo = item.value("FailDo").toString();
                    testItem.comment = item.contains("Comment") ? item.value("Comment").toString() : "";

                    testItems.append(testItem);
                }
                updateTableFlow(); // 更新 tableFlow
            }
            else
            {
                QMessageBox::warning(this, tr("错误"), tr("FlowConfig 中缺少 TestItems 部分"));
            }
        }
        else
        {
            QMessageBox::warning(this, tr("错误"), tr("配置文件中缺少 FlowConfig 部分"));
        }
    }
    else if (buttonId == "btnInportSpec")
    {
        // 更新 tableSpec
        QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
        if (jsonDoc.isNull() || !jsonDoc.isObject())
        {
            QMessageBox::warning(this, tr("错误"), tr("文件格式错误或内容无效"));
            return;
        }

        QJsonObject rootObj = jsonDoc.object();
        if (rootObj.contains("SpecConfig") && rootObj["SpecConfig"].isObject())
        {
            QJsonObject specConfig = rootObj["SpecConfig"].toObject();
            if (specConfig.contains("SpecList") && specConfig["SpecList"].isArray())
            {
                QJsonArray jsonSpecItems = specConfig["SpecList"].toArray();
                specItems.clear(); // 清空之前的数据

                for (const QJsonValue& value : jsonSpecItems)
                {
                    QJsonObject itemObj = value.toObject();
                    SpecItem specItem;
                    specItem.id = itemObj.value("SpecItemID").toString();
                    specItem.testItemName = itemObj.value("TestItemName").toString();
                    specItem.aliasName = itemObj.value("AliasName").toString();
                    specItem.params = itemObj.value("Params").toString();
                    specItem.value = itemObj.value("Value").toString();
                    specItem.lowerLimit = itemObj.value("LowerLimit").toString(); // 添加下限
                    specItem.upperLimit = itemObj.value("UpperLimit").toString(); // 添加上限
                    specItem.binName = itemObj.value("BinName").toString();
                    specItem.comment = itemObj.contains("Comment") ?
                        itemObj.value("Comment").toString() : "";

                    specItems.append(specItem);
                }

                updateTableSpec(); // 更新 tableSpec
            }
            else
            {
                QMessageBox::warning(this, tr("错误"), tr("SpecConfig 中缺少 SpecItems 部分"));
            }
        }
        else
        {
            QMessageBox::warning(this, tr("错误"), tr("配置文件中缺少 SpecConfig 部分"));
        }
    }
    else
    {
        QMessageBox::warning(this, tr("错误"), tr("FlowConfig 中缺少 TestItems 部分"));
    }

}

void ConfigDialog::loadFlowConfig(const QJsonObject& flowConfig)
{
    if (!flowConfig.contains("TestItems") || !flowConfig["TestItems"].isArray())
    {
        QMessageBox::warning(this, tr("错误"), tr("FlowConfig 中缺少 TestItems 部分"));
        return;
    }

    QJsonArray testItems = flowConfig["TestItems"].toArray();

    // 设置 tableFlow 的行数和列数
    ui->tableFlow->setRowCount(testItems.size());
    ui->tableFlow->setColumnCount(8);

    // 遍历 TestItems 数据
    for (int i = 0; i < testItems.size(); ++i)
    {
        QJsonObject item = testItems[i].toObject();

        QString id = QString::number(item.value("TestItemID").toInt());
        QString className = item.value("ClassName").toString();
        QString alias = item.value("Alias").toString();
        QString testMode = item.value("TestMode").toString();
        QString loopCount = QString::number(item.value("LoopCount").toInt());
        QString passDo = item.value("PassDo").toString();
        QString failDo = item.value("FailDo").toString();
        QString comment = item.contains("Comment") ? item.value("Comment").toString() : "";

        // 创建表格项
        QTableWidgetItem* idItem = new QTableWidgetItem(id);
        QTableWidgetItem* classNameItem = new QTableWidgetItem(className);
        QTableWidgetItem* aliasItem = new QTableWidgetItem(alias);
        QTableWidgetItem* loopCountItem = new QTableWidgetItem(loopCount);
        QTableWidgetItem* passDoItem = new QTableWidgetItem(passDo);
        QTableWidgetItem* failDoItem = new QTableWidgetItem(failDo);
        QTableWidgetItem* commentItem = new QTableWidgetItem(comment);

        // 设置表格项到 tableFlow
        ui->tableFlow->setItem(i, 0, idItem);
        ui->tableFlow->setItem(i, 1, classNameItem);
        ui->tableFlow->setItem(i, 2, aliasItem);
        ui->tableFlow->setItem(i, 3, loopCountItem);
        ui->tableFlow->setItem(i, 5, passDoItem);
        ui->tableFlow->setItem(i, 6, failDoItem);
        ui->tableFlow->setItem(i, 7, commentItem);

        // 设置 Test Mode 列为 QComboBox
        QComboBox* comboBox = new QComboBox(this);
        comboBox->addItems({ "Skip", "Test" });
        comboBox->setCurrentText(testMode); // 设置默认值
        connect(comboBox, &QComboBox::currentTextChanged, this, [=](const QString& mode)
            { on_tableFlowTestModeChanged(i, mode); });
        ui->tableFlow->setCellWidget(i, 4, comboBox);
    }
}

void ConfigDialog::initTablePin()
{
    // 初始化 Pin 配置表
    ui->tablePinConfig->setRowCount(0);
    ui->tablePinConfig->setColumnCount(5);

    QStringList pinHeaders = { "Name", "PinType", "Sites", "Disable", "Comment" };
    ui->tablePinConfig->setHorizontalHeaderLabels(pinHeaders);

    ui->tablePinConfig->horizontalHeader()->setStretchLastSection(true);
    ui->tablePinConfig->verticalHeader()->setVisible(false);
    ui->tablePinConfig->setColumnWidth(0, 150); // 设置列宽
    ui->tablePinConfig->setStyleSheet(tableStyleSheet);

    // 初始化 Pin Group 配置表
    ui->tablePinGroupConfig->setRowCount(0);
    ui->tablePinGroupConfig->setColumnCount(3);

    QStringList pinGroupHeaders = { "Name", "Value", "Comment" };
    ui->tablePinGroupConfig->setHorizontalHeaderLabels(pinGroupHeaders);

    ui->tablePinGroupConfig->horizontalHeader()->setStretchLastSection(true);
    ui->tablePinGroupConfig->verticalHeader()->setVisible(false);
    ui->tablePinGroupConfig->setColumnWidth(0, 150); // 设置列宽
    ui->tablePinGroupConfig->setStyleSheet(tableStyleSheet);
}

void ConfigDialog::initTableBin()
{
    ui->tableBinConfig->setRowCount(0);
    ui->tableBinConfig->setColumnCount(6);

    QStringList headers = { "Name", "SW_BIN", "HW_BIN", "IsPassBin", "Disable", "Comment" };
    ui->tableBinConfig->setHorizontalHeaderLabels(headers);

    ui->tableBinConfig->horizontalHeader()->setStretchLastSection(true);
    ui->tableBinConfig->verticalHeader()->setVisible(false);
    ui->tableBinConfig->setColumnWidth(0, 150); // 设置列宽
    ui->tableBinConfig->setStyleSheet(tableStyleSheet);
}

void ConfigDialog::on_btnExportSpec_clicked()
{
    // 打开文件保存对话框
    QString filePath = QFileDialog::getSaveFileName(this, tr("保存配置文件"), "", tr("JSON 文件 (*.json *.tester_config)"));
    if (filePath.isEmpty())
    {
        return;
    }

    // 读取现有的 JSON 文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("错误"), tr("无法打开文件：%1").arg(filePath));
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        QMessageBox::warning(this, tr("错误"), tr("文件格式错误或内容无效"));
        return;
    }

    QJsonObject rootObject = jsonDoc.object();

    // 构造新的 SpecList 数据
    QJsonArray specArray;
    for (const SpecItem& item : specItems)
    {
        QJsonObject specObject;
        specObject["SpecItemID"] = item.id;
        specObject["TestItemName"] = item.testItemName;
        specObject["AliasName"] = item.aliasName;
        specObject["Params"] = item.params;
        specObject["Value"] = item.value;
        specObject["LowerLimit"] = item.lowerLimit; // 添加下限
        specObject["UpperLimit"] = item.upperLimit; // 添加上限
        specObject["BinName"] = item.binName;
        specObject["Comment"] = item.comment;

        specArray.append(specObject);
    }

    // 更新 SpecList 部分
    if (!rootObject.contains("SpecConfig") || !rootObject["SpecConfig"].isObject())
    {
        rootObject["SpecConfig"] = QJsonObject(); // 如果 SpecConfig 不存在，则创建
    }

    QJsonObject specConfig = rootObject["SpecConfig"].toObject();
    specConfig["SpecList"] = specArray; // 更新 SpecList
    rootObject["SpecConfig"] = specConfig;

    // 将更新后的 JSON 写回文件
    jsonDoc.setObject(rootObject);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("错误"), tr("无法保存文件：%1").arg(filePath));
        return;
    }

    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();

    QMessageBox::information(this, tr("成功"), tr("SpecList 已成功更新到文件：%1").arg(filePath));
}

void ConfigDialog::on_tableSpecCellChanged(int row, int column)
{
    if (row < 0 || row >= specItems.size()) {
        return;
    }

    SpecItem& item = specItems[row];

    switch (column) {
    case 0: // ID
        item.id = ui->tableSpec->item(row, column)->text();
        break;
    case 1: // TestItemName
        item.testItemName = ui->tableSpec->item(row, column)->text();
        break;
    case 2: // AliasName
        item.aliasName = ui->tableSpec->item(row, column)->text();
        break;
    case 3: // Params
        item.params = ui->tableSpec->item(row, column)->text();
        break;
    case 4: // Value
        item.value = ui->tableSpec->item(row, column)->text();
        break;
    case 5: // Lower Limit
        item.lowerLimit = ui->tableSpec->item(row, column)->text();
        break;
    case 6: // Upper Limit
        item.upperLimit = ui->tableSpec->item(row, column)->text();
        break;
    case 7: // BinName
        item.binName = ui->tableSpec->item(row, column)->text();
        break;
    case 8: // Comment
        item.comment = ui->tableSpec->item(row, column)->text();
        break;
    }
}

void ConfigDialog::on_tableFlowCellChanged(int row, int column)
{
    if (row < 0 || row >= testItems.size())
    {
        return; // 如果行号无效，直接返回
    }

    TestItem& item = testItems[row]; // 获取对应的 TestItem

    // 根据列号更新 TestItem 的对应字段
    switch (column)
    {
    case 0: // ID
        item.id = ui->tableFlow->item(row, column)->text();
        break;
    case 1: // ClassName
        item.className = ui->tableFlow->item(row, column)->text();
        break;
    case 2: // Alias
        item.alias = ui->tableFlow->item(row, column)->text();
        break;
    case 3: // LoopCount
        item.loopCount = ui->tableFlow->item(row, column)->text();
        break;
    case 4: // TestMode (handled by ComboBox)
        if (QComboBox* comboBox = qobject_cast<QComboBox*>(ui->tableFlow->cellWidget(row, column)))
        {
            item.testMode = comboBox->currentText();
        }
        break;
    case 5: // PassDo
        item.passDo = ui->tableFlow->item(row, column)->text();
        break;
    case 6: // FailDo
        item.failDo = ui->tableFlow->item(row, column)->text();
        break;
    case 7: // Comment
        item.comment = ui->tableFlow->item(row, column)->text();
        break;
    default:
        break;
    }
}

void ConfigDialog::on_btnExportFlow_clicked()
{
    // 打开文件保存对话框
    QString filePath = QFileDialog::getSaveFileName(this, tr("保存配置文件"), "", tr("JSON 文件 (*.json *.tester_config)"));
    if (filePath.isEmpty())
    {
        return;
    }

    // 读取现有的 JSON 文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("错误"), tr("无法打开文件：%1").arg(filePath));
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        QMessageBox::warning(this, tr("错误"), tr("文件格式错误或内容无效"));
        return;
    }

    QJsonObject rootObject = jsonDoc.object();

    // 构造新的 TestItems 数据
    QJsonArray testItemsArray;
    for (const TestItem& item : testItems)
    {
        QJsonObject testObject;
        testObject["TestItemID"] = item.id;
        testObject["ClassName"] = item.className;
        testObject["Alias"] = item.alias;
        testObject["TestMode"] = item.testMode;
        testObject["LoopCount"] = item.loopCount.toInt();
        testObject["PassDo"] = item.passDo;
        testObject["FailDo"] = item.failDo;
        testObject["Comment"] = item.comment;

        testItemsArray.append(testObject);
    }

    // 更新 FlowConfig 部分
    if (!rootObject.contains("FlowConfig") || !rootObject["FlowConfig"].isObject())
    {
        rootObject["FlowConfig"] = QJsonObject(); // 如果 FlowConfig 不存在，则创建
    }

    QJsonObject flowConfig = rootObject["FlowConfig"].toObject();
    flowConfig["TestItems"] = testItemsArray; // 更新 TestItems
    rootObject["FlowConfig"] = flowConfig;

    // 将更新后的 JSON 写回文件
    jsonDoc.setObject(rootObject);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("错误"), tr("无法保存文件：%1").arg(filePath));
        return;
    }

    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();

    QMessageBox::information(this, tr("成功"), tr("FlowConfig 已成功更新到文件：%1").arg(filePath));
}

void ConfigDialog::on_btnAddFlow_clicked()
{
    // 创建一个默认的 TestItem
    TestItem newItem;
    newItem.id = ""; // 暂时不设置 ID
    newItem.className = "NewClass";
    newItem.alias = "NewAlias";
    newItem.testMode = "Test";
    newItem.loopCount = "1";
    newItem.passDo = "";
    newItem.failDo = "";
    newItem.comment = "";

    // 添加到 testItems 列表
    testItems.append(newItem);

    // 重新分配所有 TestItem 的 ID
    for (int i = 0; i < testItems.size(); ++i)
    {
        testItems[i].id = QString::number(i + 1); // ID 从 1 开始
    }

    // 更新表格
    updateTableFlow();

    // 滚动到最后一行
    ui->tableFlow->scrollToBottom();
}

void ConfigDialog::on_btnInsertFlow_clicked()
{
    // 获取当前选中的行
    int currentRow = ui->tableFlow->currentRow();
    if (currentRow < 0)
    {
        QMessageBox::warning(this, tr("警告"), tr("请先选择要插入的位置"));
        return;
    }

    // 创建一个默认的 TestItem
    TestItem newItem;
    newItem.id = ""; // 暂时不设置 ID
    newItem.className = "NewClass";
    newItem.alias = "NewAlias";
    newItem.testMode = "Test";
    newItem.loopCount = "1";
    newItem.passDo = "";
    newItem.failDo = "";
    newItem.comment = "";

    // 插入到 testItems 列表
    testItems.insert(currentRow, newItem);

    // 重新分配所有 TestItem 的 ID
    for (int i = 0; i < testItems.size(); ++i)
    {
        testItems[i].id = QString::number(i + 1); // ID 从 1 开始
    }

    // 更新表格
    updateTableFlow();

    // 选中插入的行
    ui->tableFlow->selectRow(currentRow);
}

void ConfigDialog::on_btnDeleteFlow_clicked()
{
    // 获取当前选中的行
    int currentRow = ui->tableFlow->currentRow();
    if (currentRow < 0 || currentRow >= testItems.size())
    {
        QMessageBox::warning(this, tr("警告"), tr("请先选择要删除的行"));
        return;
    }

    // 从 testItems 列表中删除对应的项
    testItems.removeAt(currentRow);

    // 更新表格
    updateTableFlow();
}

void ConfigDialog::on_btnMoveUpFlow_clicked()
{
    int currentRow = ui->tableFlow->currentRow();
    if (currentRow <= 0) // 如果当前行是第一行或未选中，无法上移
    {
        QMessageBox::warning(this, tr("警告"), tr("无法上移：已是第一行或未选中"));
        return;
    }

    // 交换 `specItems` 中的当前行和上一行
    std::swap(testItems[currentRow], testItems[currentRow - 1]);

    // 更新表格
    updateTableFlow();

    // 选中上移后的行
    ui->tableFlow->selectRow(currentRow - 1);
}

void ConfigDialog::on_btnMoveDownFlow_clicked()
{
    int currentRow = ui->tableFlow->currentRow();
    if (currentRow < 0 || currentRow >= testItems.size() - 1) // 如果当前行是最后一行或未选中，无法下移
    {
        QMessageBox::warning(this, tr("警告"), tr("无法下移：已是最后一行或未选中"));
        return;
    }

    // 交换 `specItems` 中的当前行和下一行
    std::swap(testItems[currentRow], testItems[currentRow + 1]);

    // 更新表格
    updateTableFlow();

    // 选中下移后的行
    ui->tableFlow->selectRow(currentRow + 1);
}