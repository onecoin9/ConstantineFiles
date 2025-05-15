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

    // ���� btnInport ��ť
    connect(ui->btnInport, &QPushButton::clicked, this, [=]()
        { on_btnInport_clicked("btnInport"); });

    // ���� btnInportSpec ��ť
    connect(ui->btnInportSpec, &QPushButton::clicked, this, [=]()
        { on_btnInport_clicked("btnInportSpec"); });
    // ������ʽ��
    tableStyleSheet = R"(
    QTableWidget {
        color: #000000; /* ��Ԫ��������ɫ��Ϊ��ɫ */
        background: #f8f8f8;
        border: 1px solid #242424;
        alternate-background-color: #525252; /* ��������ɫ */
        gridline-color: #242424;
    }
    QTableWidget::item:selected {
        color: #000000; /* ѡ����������ɫ��Ϊ��ɫ */
        background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #484848, stop:1 #383838);
    }
    QTableWidget::item:hover {
        background: #5B5B5B;
    }
    QHeaderView::section {
        text-align: center;
        background: #c5e3f6; /* �µı�ͷ����ɫ */
        padding: 3px;
        margin: 0px;
        color: #000000; /* ��ͷ������ɫ��Ϊ��ɫ */
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
        background: #e2f3f5; /* ѡ�����ɫ */
    }
    QTableWidget::item:hover {
        background: #88bef5; /* �������ɫ */
    }
    QTableWidget {
        alternate-background-color: #43dde6; /* �����б���ɫ */
    }
    QTableWidget::item {
        color: #000000; /* ��Ԫ��������ɫ��Ϊ��ɫ */
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

    QStringList headers = { "���","ID", "Class Name", "Alias", "Loop Count", "Test Mode", "Pass Do", "Fail Do", "Comment" };
    ui->tableFlow->setHorizontalHeaderLabels(headers);

    ui->tableFlow->horizontalHeader()->setStretchLastSection(true);
    ui->tableFlow->verticalHeader()->setVisible(false);
    ui->tableFlow->setColumnWidth(2, 200);

    ui->tableFlow->setStyleSheet(tableStyleSheet);
}

void ConfigDialog::initTableSpec()
{
    ui->tableSpec->setRowCount(0);
    ui->tableSpec->setColumnCount(17);

    QStringList headers = { "ID", "TestItemName", "Alias Name", "Params1",  "Value","Limit<", "Limit>", "Params2", "Value", "Limit<", "Limit>", "Params3", "Value", "Limit<", "Limit>", "Bin Name", "Comment" };
    ui->tableSpec->setHorizontalHeaderLabels(headers);

    ui->tableSpec->horizontalHeader()->setStretchLastSection(true);
    ui->tableSpec->verticalHeader()->setVisible(false);
    ui->tableSpec->setColumnWidth(1, 200);
    ui->tableSpec->setStyleSheet(tableStyleSheet);
}
void ConfigDialog::updateTableFlow()
{
    // ��ʱ�Ͽ��ź�����
    disconnect(ui->tableFlow, &QTableWidget::cellChanged, this, &ConfigDialog::on_tableFlowCellChanged);

    ui->tableFlow->setRowCount(testItems.size());
    for (int i = 0; i < testItems.size(); ++i)
    {
        QTableWidgetItem* indexItem = new QTableWidgetItem(QString::number(i + 1));
        indexItem->setTextAlignment(Qt::AlignCenter);
        indexItem->setFlags(indexItem->flags() & ~Qt::ItemIsEditable); // ��ֹ�༭�����
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

    // �ָ��ź�����
    connect(ui->tableFlow, &QTableWidget::cellChanged, this, &ConfigDialog::on_tableFlowCellChanged);
}

void ConfigDialog::populateTableSpecRow(int row, const SpecItem& item)
{
    ui->tableSpec->setItem(row, 0, new QTableWidgetItem(item.id));
    ui->tableSpec->setItem(row, 1, new QTableWidgetItem(item.testItemName));
    ui->tableSpec->setItem(row, 2, new QTableWidgetItem(item.aliasName));

    // Params1
    ui->tableSpec->setItem(row, 3, new QTableWidgetItem(item.params1Name));
    ui->tableSpec->setItem(row, 4, new QTableWidgetItem(item.params1Value));
    ui->tableSpec->setItem(row, 5, new QTableWidgetItem(item.limit1Lower));
    ui->tableSpec->setItem(row, 6, new QTableWidgetItem(item.limit1Upper));

    // Params2
    ui->tableSpec->setItem(row, 7, new QTableWidgetItem(item.params2Name));
    ui->tableSpec->setItem(row, 8, new QTableWidgetItem(item.params2Value));
    ui->tableSpec->setItem(row, 9, new QTableWidgetItem(item.limit2Lower));
    ui->tableSpec->setItem(row, 10, new QTableWidgetItem(item.limit2Upper));

    // Params3
    ui->tableSpec->setItem(row, 11, new QTableWidgetItem(item.params3Name));
    ui->tableSpec->setItem(row, 12, new QTableWidgetItem(item.params3Value));
    ui->tableSpec->setItem(row, 13, new QTableWidgetItem(item.limit3Lower));
    ui->tableSpec->setItem(row, 14, new QTableWidgetItem(item.limit3Upper));

    // Bin Name and Comment
    ui->tableSpec->setItem(row, 15, new QTableWidgetItem(item.binName));
    ui->tableSpec->setItem(row, 16, new QTableWidgetItem(item.comment));
}

void ConfigDialog::updateTableSpec()
{
    // ��ʱ�Ͽ��ź�����
    disconnect(ui->tableSpec, &QTableWidget::cellChanged, this, &ConfigDialog::on_tableSpecCellChanged);

    ui->tableSpec->setRowCount(specItems.size());
    for (int i = 0; i < specItems.size(); ++i)
    {
        populateTableSpecRow(i, specItems[i]);
    }

    // �ָ��ź�����
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

void ConfigDialog::on_btnInport_clicked(const QString& buttonId)
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("ѡ�������ļ�"), "", tr("JSON �ļ� (*.json *.tester_config)"));
    if (filePath.isEmpty())
    {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("����"), tr("�޷����ļ���%1").arg(filePath));
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    // �жϰ�ť ID�����������ĸ����
    if (buttonId == "btnInport")
    {
        // ���� tableFlow
        QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
        if (jsonDoc.isNull() || !jsonDoc.isObject())
        {
            QMessageBox::warning(this, tr("����"), tr("�ļ���ʽ�����������Ч"));
            return;
        }

        QJsonObject rootObj = jsonDoc.object();
        if (rootObj.contains("FlowConfig") && rootObj["FlowConfig"].isObject())
        {
            QJsonObject flowConfig = rootObj["FlowConfig"].toObject();
            if (flowConfig.contains("TestItems") && flowConfig["TestItems"].isArray())
            {
                QJsonArray jsonTestItems = flowConfig["TestItems"].toArray();
                testItems.clear(); // ���֮ǰ������

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
                updateTableFlow(); // ���� tableFlow
            }
            else
            {
                QMessageBox::warning(this, tr("����"), tr("FlowConfig ��ȱ�� TestItems ����"));
            }
        }
        else
        {
            QMessageBox::warning(this, tr("����"), tr("�����ļ���ȱ�� FlowConfig ����"));
        }
    }
    else if (buttonId == "btnInportSpec")
    {
        // ���� tableSpec
        QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
        if (jsonDoc.isNull() || !jsonDoc.isObject())
        {
            QMessageBox::warning(this, tr("����"), tr("�ļ���ʽ�����������Ч"));
            return;
        }

        QJsonObject rootObj = jsonDoc.object();
        if (rootObj.contains("SpecConfig") && rootObj["SpecConfig"].isObject())
        {
            QJsonObject specConfig = rootObj["SpecConfig"].toObject();
            if (specConfig.contains("SpecList") && specConfig["SpecList"].isArray())
            {
                QJsonArray jsonSpecItems = specConfig["SpecList"].toArray();
                specItems.clear(); // ���֮ǰ������

                for (const QJsonValue& value : jsonSpecItems)
                {
                    QJsonObject itemObj = value.toObject();
                    SpecItem specItem;
                    specItem.id = itemObj.value("SpecItemID").toString();
                    specItem.testItemName = itemObj.value("TestItemName").toString();
                    specItem.aliasName = itemObj.value("AliasName").toString();
            
                    // Params1
                    specItem.params1Name = itemObj.value("Params1Name").toString();
                    specItem.params1Value = itemObj.value("Params1Value").toString();
                    specItem.limit1Lower = itemObj.value("Limit1Lower").toString();
                    specItem.limit1Upper = itemObj.value("Limit1Upper").toString();
            
                    // Params2
                    specItem.params2Name = itemObj.value("Params2Name").toString();
                    specItem.params2Value = itemObj.value("Params2Value").toString();
                    specItem.limit2Lower = itemObj.value("Limit2Lower").toString();
                    specItem.limit2Upper = itemObj.value("Limit2Upper").toString();
            
                    // Params3
                    specItem.params3Name = itemObj.value("Params3Name").toString();
                    specItem.params3Value = itemObj.value("Params3Value").toString();
                    specItem.limit3Lower = itemObj.value("Limit3Lower").toString();
                    specItem.limit3Upper = itemObj.value("Limit3Upper").toString();
            
                    // Bin Name and Comment
                    specItem.binName = itemObj.value("BinName").toString();            
                    specItem.comment = itemObj.contains("Comment") ? itemObj.value("Comment").toString() : "";

                    specItems.append(specItem);
                }

                updateTableSpec(); // ���� tableSpec
            }
            else
            {
                QMessageBox::warning(this, tr("����"), tr("SpecConfig ��ȱ�� SpecItems ����"));
            }
        }
        else
        {
            QMessageBox::warning(this, tr("����"), tr("�����ļ���ȱ�� SpecConfig ����"));
        }
    }
    else
    {
        QMessageBox::warning(this, tr("����"), tr("FlowConfig ��ȱ�� TestItems ����"));
    }

}

void ConfigDialog::loadFlowConfig(const QJsonObject& flowConfig)
{
    if (!flowConfig.contains("TestItems") || !flowConfig["TestItems"].isArray())
    {
        QMessageBox::warning(this, tr("����"), tr("FlowConfig ��ȱ�� TestItems ����"));
        return;
    }

    QJsonArray testItems = flowConfig["TestItems"].toArray();

    // ���� tableFlow ������������
    ui->tableFlow->setRowCount(testItems.size());
    ui->tableFlow->setColumnCount(8);

    // ���� TestItems ����
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

        // ���������
        QTableWidgetItem* idItem = new QTableWidgetItem(id);
        QTableWidgetItem* classNameItem = new QTableWidgetItem(className);
        QTableWidgetItem* aliasItem = new QTableWidgetItem(alias);
        QTableWidgetItem* loopCountItem = new QTableWidgetItem(loopCount);
        QTableWidgetItem* passDoItem = new QTableWidgetItem(passDo);
        QTableWidgetItem* failDoItem = new QTableWidgetItem(failDo);
        QTableWidgetItem* commentItem = new QTableWidgetItem(comment);

        // ���ñ��� tableFlow
        ui->tableFlow->setItem(i, 0, idItem);
        ui->tableFlow->setItem(i, 1, classNameItem);
        ui->tableFlow->setItem(i, 2, aliasItem);
        ui->tableFlow->setItem(i, 3, loopCountItem);
        ui->tableFlow->setItem(i, 5, passDoItem);
        ui->tableFlow->setItem(i, 6, failDoItem);
        ui->tableFlow->setItem(i, 7, commentItem);

        // ���� Test Mode ��Ϊ QComboBox
        QComboBox* comboBox = new QComboBox(this);
        comboBox->addItems({ "Skip", "Test" });
        comboBox->setCurrentText(testMode); // ����Ĭ��ֵ
        connect(comboBox, &QComboBox::currentTextChanged, this, [=](const QString& mode)
            { on_tableFlowTestModeChanged(i, mode); });
        ui->tableFlow->setCellWidget(i, 4, comboBox);
    }
}

void ConfigDialog::initTablePin()
{
    // ��ʼ�� Pin ���ñ�
    ui->tablePinConfig->setRowCount(0);
    ui->tablePinConfig->setColumnCount(5);

    QStringList pinHeaders = { "Name", "PinType", "Sites", "Disable", "Comment" };
    ui->tablePinConfig->setHorizontalHeaderLabels(pinHeaders);

    ui->tablePinConfig->horizontalHeader()->setStretchLastSection(true);
    ui->tablePinConfig->verticalHeader()->setVisible(false);
    ui->tablePinConfig->setColumnWidth(0, 150); // �����п�
    ui->tablePinConfig->setStyleSheet(tableStyleSheet);

    // ��ʼ�� Pin Group ���ñ�
    ui->tablePinGroupConfig->setRowCount(0);
    ui->tablePinGroupConfig->setColumnCount(3);

    QStringList pinGroupHeaders = { "Name", "Value", "Comment" };
    ui->tablePinGroupConfig->setHorizontalHeaderLabels(pinGroupHeaders);

    ui->tablePinGroupConfig->horizontalHeader()->setStretchLastSection(true);
    ui->tablePinGroupConfig->verticalHeader()->setVisible(false);
    ui->tablePinGroupConfig->setColumnWidth(0, 150); // �����п�
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
    ui->tableBinConfig->setColumnWidth(0, 150); // �����п�
    ui->tableBinConfig->setStyleSheet(tableStyleSheet);
}

void ConfigDialog::on_btnExportSpec_clicked()
{
    // ���ļ�����Ի���
    QString filePath = QFileDialog::getSaveFileName(this, tr("���������ļ�"), "", tr("JSON �ļ� (*.json *.tester_config)"));
    if (filePath.isEmpty())
    {
        return;
    }

    // ��ȡ���е� JSON �ļ�
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("����"), tr("�޷����ļ���%1").arg(filePath));
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        QMessageBox::warning(this, tr("����"), tr("�ļ���ʽ�����������Ч"));
        return;
    }

    QJsonObject rootObject = jsonDoc.object();

    // �����µ� SpecList ����
    QJsonArray specArray;
    for (const SpecItem& item : specItems)
    {
        QJsonObject specObject;
        specObject["SpecItemID"] = item.id;
        specObject["TestItemName"] = item.testItemName;
        specObject["AliasName"] = item.aliasName;

        // Params1
        specObject["Params1Name"] = item.params1Name;
        specObject["Params1Value"] = item.params1Value;
        specObject["Limit1Lower"] = item.limit1Lower;
        specObject["Limit1Upper"] = item.limit1Upper;

        // Params2
        specObject["Params2Name"] = item.params2Name;
        specObject["Params2Value"] = item.params2Value;
        specObject["Limit2Lower"] = item.limit2Lower;
        specObject["Limit2Upper"] = item.limit2Upper;

        // Params3
        specObject["Params3Name"] = item.params3Name;
        specObject["Params3Value"] = item.params3Value;
        specObject["Limit3Lower"] = item.limit3Lower;
        specObject["Limit3Upper"] = item.limit3Upper;

        // Bin Name and Comment
        specObject["BinName"] = item.binName;
        specObject["Commment"] = item.comment;

        specArray.append(specObject);
    }

    // ���� SpecList ����
    if (!rootObject.contains("SpecConfig") || !rootObject["SpecConfig"].isObject())
    {
        rootObject["SpecConfig"] = QJsonObject(); // ��� SpecConfig �����ڣ��򴴽�
    }

    QJsonObject specConfig = rootObject["SpecConfig"].toObject();
    specConfig["SpecList"] = specArray; // ���� SpecList
    rootObject["SpecConfig"] = specConfig;

    // �����º�� JSON д���ļ�
    jsonDoc.setObject(rootObject);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("����"), tr("�޷������ļ���%1").arg(filePath));
        return;
    }

    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();

    QMessageBox::information(this, tr("�ɹ�"), tr("SpecList �ѳɹ����µ��ļ���%1").arg(filePath));
}

void ConfigDialog::on_tableSpecCellChanged(int row, int column)
{
    if (row < 0 || row >= specItems.size())
    {
        return; // ����к���Ч��ֱ�ӷ���
    }

    SpecItem& item = specItems[row]; // ��ȡ��Ӧ�� SpecItem

    // �����кŸ��� SpecItem �Ķ�Ӧ�ֶ�
    switch (column)
    {
    case 0: // ID
        item.id = ui->tableSpec->item(row, column)->text();
        break;
    case 1: // TestItemName
        item.testItemName = ui->tableSpec->item(row, column)->text();
        break;
    case 2: // AliasName
        item.aliasName = ui->tableSpec->item(row, column)->text();
        break;
    case 3: // Params1Name
        item.params1Name = ui->tableSpec->item(row, column)->text();
        break;
    case 4: // Params1Value
        item.params1Value = ui->tableSpec->item(row, column)->text();
        break;
    case 5: // Limit1Lower
        item.limit1Lower = ui->tableSpec->item(row, column)->text();
        break;
    case 6: // Limit1Upper
        item.limit1Upper = ui->tableSpec->item(row, column)->text();
        break;
    case 7: // Params2Name
        item.params2Name = ui->tableSpec->item(row, column)->text();
        break;
    case 8: // Params2Value
        item.params2Value = ui->tableSpec->item(row, column)->text();
        break;
    case 9: // Limit2Lower
        item.limit2Lower = ui->tableSpec->item(row, column)->text();
        break;
    case 10: // Limit2Upper
        item.limit2Upper = ui->tableSpec->item(row, column)->text();
        break;
    case 11: // Params3Name
        item.params3Name = ui->tableSpec->item(row, column)->text();
        break;
    case 12: // Params3Value
        item.params3Value = ui->tableSpec->item(row, column)->text();
        break;
    case 13: // Limit3Lower
        item.limit3Lower = ui->tableSpec->item(row, column)->text();
        break;
    case 14: // Limit3Upper
        item.limit3Upper = ui->tableSpec->item(row, column)->text();
        break;
    case 15: // BinName
        item.binName = ui->tableSpec->item(row, column)->text();
        break;
    case 16: // Comment
        item.comment = ui->tableSpec->item(row, column)->text();
        break;
    default:
        break;
    }
}

void ConfigDialog::on_tableFlowCellChanged(int row, int column)
{
    if (row < 0 || row >= testItems.size())
    {
        return; // ����к���Ч��ֱ�ӷ���
    }

    TestItem& item = testItems[row]; // ��ȡ��Ӧ�� TestItem

    // �����кŸ��� TestItem �Ķ�Ӧ�ֶ�
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
    // ���ļ�����Ի���
    QString filePath = QFileDialog::getSaveFileName(this, tr("���������ļ�"), "", tr("JSON �ļ� (*.json *.tester_config)"));
    if (filePath.isEmpty())
    {
        return;
    }

    // ��ȡ���е� JSON �ļ�
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("����"), tr("�޷����ļ���%1").arg(filePath));
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        QMessageBox::warning(this, tr("����"), tr("�ļ���ʽ�����������Ч"));
        return;
    }

    QJsonObject rootObject = jsonDoc.object();

    // �����µ� TestItems ����
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

    // ���� FlowConfig ����
    if (!rootObject.contains("FlowConfig") || !rootObject["FlowConfig"].isObject())
    {
        rootObject["FlowConfig"] = QJsonObject(); // ��� FlowConfig �����ڣ��򴴽�
    }

    QJsonObject flowConfig = rootObject["FlowConfig"].toObject();
    flowConfig["TestItems"] = testItemsArray; // ���� TestItems
    rootObject["FlowConfig"] = flowConfig;

    // �����º�� JSON д���ļ�
    jsonDoc.setObject(rootObject);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("����"), tr("�޷������ļ���%1").arg(filePath));
        return;
    }

    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();

    QMessageBox::information(this, tr("�ɹ�"), tr("FlowConfig �ѳɹ����µ��ļ���%1").arg(filePath));
}

void ConfigDialog::on_btnAddFlow_clicked()
{
    // ����һ��Ĭ�ϵ� TestItem
    TestItem newItem;
    newItem.id = ""; // ��ʱ������ ID
    newItem.className = "NewClass";
    newItem.alias = "NewAlias";
    newItem.testMode = "Test";
    newItem.loopCount = "1";
    newItem.passDo = "";
    newItem.failDo = "";
    newItem.comment = "";

    // ��ӵ� testItems �б�
    testItems.append(newItem);

    // ���·������� TestItem �� ID
    for (int i = 0; i < testItems.size(); ++i)
    {
        testItems[i].id = QString::number(i + 1); // ID �� 1 ��ʼ
    }

    // ���±��
    updateTableFlow();

    // ���������һ��
    ui->tableFlow->scrollToBottom();
}

void ConfigDialog::on_btnInsertFlow_clicked()
{
    // ��ȡ��ǰѡ�е���
    int currentRow = ui->tableFlow->currentRow();
    if (currentRow < 0)
    {
        QMessageBox::warning(this, tr("����"), tr("����ѡ��Ҫ�����λ��"));
        return;
    }

    // ����һ��Ĭ�ϵ� TestItem
    TestItem newItem;
    newItem.id = ""; // ��ʱ������ ID
    newItem.className = "NewClass";
    newItem.alias = "NewAlias";
    newItem.testMode = "Test";
    newItem.loopCount = "1";
    newItem.passDo = "";
    newItem.failDo = "";
    newItem.comment = "";

    // ���뵽 testItems �б�
    testItems.insert(currentRow, newItem);

    // ���·������� TestItem �� ID
    for (int i = 0; i < testItems.size(); ++i)
    {
        testItems[i].id = QString::number(i + 1); // ID �� 1 ��ʼ
    }

    // ���±��
    updateTableFlow();

    // ѡ�в������
    ui->tableFlow->selectRow(currentRow);
}

void ConfigDialog::on_btnDeleteFlow_clicked()
{
    // ��ȡ��ǰѡ�е���
    int currentRow = ui->tableFlow->currentRow();
    if (currentRow < 0 || currentRow >= testItems.size())
    {
        QMessageBox::warning(this, tr("����"), tr("����ѡ��Ҫɾ������"));
        return;
    }

    // �� testItems �б���ɾ����Ӧ����
    testItems.removeAt(currentRow);

    // ���±��
    updateTableFlow();
}

void ConfigDialog::on_btnMoveUpFlow_clicked()
{
    int currentRow = ui->tableFlow->currentRow();
    if (currentRow <= 0) // �����ǰ���ǵ�һ�л�δѡ�У��޷�����
    {
        QMessageBox::warning(this, tr("����"), tr("�޷����ƣ����ǵ�һ�л�δѡ��"));
        return;
    }

    // ���� `specItems` �еĵ�ǰ�к���һ��
    std::swap(testItems[currentRow], testItems[currentRow - 1]);

    // ���±��
    updateTableFlow();

    // ѡ�����ƺ����
    ui->tableFlow->selectRow(currentRow - 1);
}

void ConfigDialog::on_btnMoveDownFlow_clicked()
{
    int currentRow = ui->tableFlow->currentRow();
    if (currentRow < 0 || currentRow >= testItems.size() - 1) // �����ǰ�������һ�л�δѡ�У��޷�����
    {
        QMessageBox::warning(this, tr("����"), tr("�޷����ƣ��������һ�л�δѡ��"));
        return;
    }

    // ���� `specItems` �еĵ�ǰ�к���һ��
    std::swap(testItems[currentRow], testItems[currentRow + 1]);

    // ���±��
    updateTableFlow();

    // ѡ�����ƺ����
    ui->tableFlow->selectRow(currentRow + 1);
}