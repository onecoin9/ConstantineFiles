#include "acroViewTester.h"
#include "logManager.h" // 确保包含了 LogManager 头文件
#include <QDateTime>

void acroViewTester::setupDataUI()
{
   ui.tableMain->setAlternatingRowColors(true);
    ui.tableMain->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableMain->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.tableMain->horizontalHeader()->setStretchLastSection(true);
    ui.tableMain->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    connect(ui.exportDataButton, &QPushButton::clicked, this, &acroViewTester::on_exportButton_clicked);
}


void acroViewTester::addSampleData()
{
    // 示例数据结构，可以根据 LogManager::addLogToDB 的参数进行调整
    struct SampleLogEntry {
        QString eventName;      // 将作为主要 message 传递给 LogManager
        LogManager::LogLevel level;
        QString additionalDetail; // 将作为 detail 传递给 LogManager
        QString jsonData;         // 将作为 data 传递给 LogManager
    };

    QVector<SampleLogEntry> sampleEntries = {
        {
            QStringLiteral("系统启动"),
            LogManager::Info,
            QStringLiteral("系统正常启动，所有模块已加载"),
            QStringLiteral("{\"module_count\": 5}")
        },
        {
            QStringLiteral("温度告警"),
            LogManager::Warn,
            QStringLiteral("CPU核心温度达到阈值"),
            QStringLiteral("{\"sensor\":\"CPU01\", \"temp\":85, \"threshold\":80}")
        },
        {
            QStringLiteral("用户登录尝试"),
            LogManager::Debug,
            QStringLiteral("用户 'testuser' 尝试登录"),
            QStringLiteral("{\"ip\":\"192.168.1.100\"}")
        },
        {
            QStringLiteral("数据库连接失败"),
            LogManager::Error,
            QStringLiteral("无法连接到主数据库服务器"),
            QStringLiteral("{\"server\":\"main_db_server\", \"error_code\":1045}")
        }
    };

    for (const auto& entry : sampleEntries) {
        // 使用 LogManager 添加日志
        // LogManager 会处理时间戳和ID，并将日志存入数据库及更新UI模型
        LogManager::instance().addLogToDB(entry.level, entry.eventName, entry.additionalDetail, entry.jsonData);
    }
    // LogManager::instance().refreshLogView(); // addLogToDB 内部的 onLogAdded 应该会调用 refreshLogView
}

void acroViewTester::on_exportButton_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("导出Excel文件"),
        "", tr("Excel Files (*.xlsx *.xls)"));
    if (fileName.isEmpty()) return;
    exportToExcel(fileName);
}

void acroViewTester::exportToExcel(const QString& fileName)
{
    QStandardItemModel* modelToExport = LogManager::instance().getLogModel();
    if (!modelToExport) {
        QMessageBox::warning(this, tr("错误"), tr("日志模型不可用"));
        return;
    }

    // 创建 Excel 应用程序实例
    QAxObject* excel = new QAxObject("Excel.Application", this);
    if (excel->isNull()) {
        QMessageBox::critical(this,
            QStringLiteral("错误"),
            QStringLiteral("无法创建Excel应用程序实例"));
        delete excel;
        return;
    }
    excel->setProperty("Visible", false); // 后台运行

    QAxObject* workbooks = excel->querySubObject("Workbooks");
    QAxObject* workbook = workbooks->querySubObject("Add");
    QAxObject* worksheets = workbook->querySubObject("Worksheets");

    if (!worksheets) {
        QMessageBox::critical(this,
            QString::fromUtf8("错误"),
            QString::fromUtf8("无法获取工作表集合"));
        excel->dynamicCall("Quit()");
        delete excel;
        return;
    }

    QAxObject* worksheet = worksheets->querySubObject("Item(int)", 1);
    worksheet->setProperty("Name", QString::fromUtf8("日志导出"));

    // 写入表头
    for (int col = 0; col < modelToExport->columnCount(); ++col) {
        QAxObject* cell = worksheet->querySubObject("Cells(int,int)", 1, col + 1);
        QString headerText = modelToExport->headerData(col, Qt::Horizontal).toString();
        cell->setProperty("Value", headerText);
        // cell->setProperty("HorizontalAlignment", -4108); // xlCenter
        delete cell;
    }

    // 写入数据
    for (int row = 0; row < modelToExport->rowCount(); ++row) {
        for (int col = 0; col < modelToExport->columnCount(); ++col) {
            QAxObject* cell = worksheet->querySubObject("Cells(int,int)", row + 2, col + 1);
            QStandardItem* item = modelToExport->item(row, col);
            if (item) {
                cell->setProperty("Value", item->text());
            }
            // cell->setProperty("HorizontalAlignment", -4108);
            delete cell;
        }
    }

    QAxObject* usedRange = worksheet->querySubObject("UsedRange");
    if (usedRange) {
        usedRange->querySubObject("Columns")->dynamicCall("AutoFit");
        delete usedRange;
    }
    
    workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(fileName));
    workbook->dynamicCall("Close(false)"); // Close without saving changes again
    excel->dynamicCall("Quit()");

    delete worksheet;
    delete worksheets;
    delete workbook;
    delete workbooks;
    delete excel;

    QMessageBox::information(this,
        QString::fromUtf8("导出完成"),
        QString::fromUtf8("成功导出到：%1").arg(fileName));
}