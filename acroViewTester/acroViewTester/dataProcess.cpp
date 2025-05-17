#include"acroViewTester.h"
void acroViewTester::setupDataUI()
{
    ui.tableMain->setAlternatingRowColors(true);
    ui.tableMain->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableMain->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.tableMain->horizontalHeader()->setStretchLastSection(true);
    ui.tableMain->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    dataModel = new QStandardItemModel(this);
    dataModel->setColumnCount(7);
    dataModel->setHeaderData(0, Qt::Horizontal, tr("日志序号"));
    dataModel->setHeaderData(1, Qt::Horizontal, tr("日志名称"));
    dataModel->setHeaderData(2, Qt::Horizontal, tr("日志发生时间"));
    dataModel->setHeaderData(3, Qt::Horizontal, tr("日志类型"));
    dataModel->setHeaderData(4, Qt::Horizontal, tr("日志内容"));
    dataModel->setHeaderData(5, Qt::Horizontal, tr("告警详情"));
    dataModel->setHeaderData(6, Qt::Horizontal, tr("告警数据"));
    ui.tableMain->setModel(dataModel);

    connect(ui.exportDataButton, &QPushButton::clicked, this, &acroViewTester::on_exportButton_clicked);

}

void acroViewTester::addSampleData()
{
    // 预定义日志数据结构
    struct LogEntry {
        QString id;
        QString event;
        QString time;
        QString level;
        QString message;
        QString detail;
        QString data;
    };

    // 创建示例数据
    QVector<LogEntry> logEntries = {
        {
            QStringLiteral("1"), 
            QStringLiteral("系统启动"),
            QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss")),
            QStringLiteral("Info"),
            QStringLiteral("系统正常启动"),
            QStringLiteral("none"),
            QStringLiteral("[]")
        },
        {
            QStringLiteral("2"),
            QStringLiteral("温度告警"),
            QDateTime::currentDateTime().addSecs(-3600).toString(QStringLiteral("yyyy-MM-dd hh:mm:ss")),
            QStringLiteral("Warning"),
            QStringLiteral("cpu over temperature"),
            QStringLiteral("temp85"),
            QStringLiteral("{\"sensor\":\"CPU01\"}")
        }
    };

    // 将数据添加到模型中
    for (const auto& entry : logEntries) {
        QList<QStandardItem*> row;
        row << new QStandardItem(entry.id)
            << new QStandardItem(entry.event)
            << new QStandardItem(entry.time)
            << new QStandardItem(entry.level)
            << new QStandardItem(entry.message)
            << new QStandardItem(entry.detail)
            << new QStandardItem(entry.data);

        // 设置文本对齐方式
        for (auto* item : row) {
            item->setTextAlignment(Qt::AlignCenter);
        }

        dataModel->appendRow(row);
    }
}

void acroViewTester::on_exportButton_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("导出Excel文件"),
        "", tr("Excel Files (*.xlsx *.xls)"));
    if (fileName.isEmpty()) return;
    exportToExcel(fileName);
}

void acroViewTester::exportToExcel(const QString& fileName) 
{
    // 创建 Excel 应用程序实例
    QAxObject* excel = new QAxObject("Excel.Application", this);
    if (excel->isNull()) {  // 修改这里：使用 isNull() 替代 isValid()
        QMessageBox::critical(this, 
            QStringLiteral("错误"),
            QStringLiteral("无法创建Excel应用程序实例"));
        delete excel;
        return;
    }

    QAxObject* workbooks = excel->querySubObject("Workbooks");
    QAxObject* workbook = workbooks->querySubObject("Add");
    QAxObject* worksheets = workbook->querySubObject("Worksheets");
    
    if (!worksheets) {
        QMessageBox::critical(this, 
            QString::fromUtf8("错误"),
            QString::fromUtf8("cant get worksheets"));
        excel->dynamicCall("Quit()");
        delete excel;
        return;
    }

    QAxObject* worksheet = worksheets->querySubObject("Item(int)", 1);
    
    // 设置工作表名称
    worksheet->setProperty("Name", QString::fromUtf8("日志导出"));

    // 写入表头
    for (int col = 0; col < dataModel->columnCount(); ++col) {
        QAxObject* cell = worksheet->querySubObject("Cells(int,int)", 1, col + 1);
        QString headerText = dataModel->headerData(col, Qt::Horizontal).toString();
        cell->setProperty("Value", headerText);
        cell->setProperty("HorizontalAlignment", -4108); // xlCenter
        delete cell;
    }

    // 写入数据
    for (int row = 0; row < dataModel->rowCount(); ++row) {
        for (int col = 0; col < dataModel->columnCount(); ++col) {
            QAxObject* cell = worksheet->querySubObject("Cells(int,int)", row + 2, col + 1);
            QString cellText = dataModel->item(row, col)->text();
            cell->setProperty("Value", cellText);
            cell->setProperty("HorizontalAlignment", -4108);
            delete cell;
        }
    }

    // 自动调整列宽
    QAxObject* usedRange = worksheet->querySubObject("UsedRange");
    usedRange->querySubObject("Columns")->dynamicCall("AutoFit");
    delete usedRange;

    // 保存并关闭
    workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(fileName));
    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");

    // 清理 COM 对象
    delete worksheet;
    delete worksheets;
    delete workbook;
    delete workbooks;
    delete excel;

    QMessageBox::information(this, 
        QString::fromUtf8("导出完成"),
        QString::fromUtf8("成功导出到：%1").arg(fileName));
}