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
	dataModel->insertRow(0);
	for (int col = 0; col < 7; ++col) {
		QStandardItem* headerItem = new QStandardItem(dataModel->headerData(col, Qt::Horizontal).toString());
		headerItem->setTextAlignment(Qt::AlignCenter);
		dataModel->setItem(0, col, headerItem);
	}
	dataModel->removeRows(0, 1);
	QList<QStringList> logData = {
		{"1", "系统启动", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),
		 "Info", "系统正常启动", "无", "[]"},
		{"2", "温度告警", QDateTime::currentDateTime().addSecs(-3600).toString(),
		 "Warning", "CPU温度超过阈值", "温度：85℃", "{\"sensor\":\"CPU01\"}"}
	};

	for (const auto& data : logData) {
		int row = dataModel->rowCount();
		dataModel->insertRow(row);
		for (int col = 0; col < 7; ++col) {
			QStandardItem* item = new QStandardItem(data[col]);
			item->setTextAlignment(Qt::AlignCenter);
			dataModel->setItem(row, col, item);
		}
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
	QAxObject* excel = new QAxObject("Excel.Application", this);
	QAxObject* workbooks = excel->querySubObject("Workbooks");
	QAxObject* workbook = workbooks->querySubObject("Add");
	QAxObject* worksheets = workbook->querySubObject("Worksheets");
	if (worksheets == nullptr) {
		qDebug() << "Failed to get worksheets.";
		return;
	}

	QAxObject* worksheet = worksheets->querySubObject("Item", 1);
	if (worksheet == nullptr) {
		qDebug() << "Failed to get worksheet.";
		return;
	}
	worksheet->setProperty("Name", "日志导出");
	for (int col = 0; col < dataModel->columnCount(); ++col) {
		QAxObject* cell = worksheet->querySubObject("Cells(int,int)", 1, col + 1);
		cell->dynamicCall("SetValue(const QString&)", dataModel->headerData(col, Qt::Horizontal));
		cell->setProperty("HorizontalAlignment", -4108); // 居中对齐
		delete cell;
	}

	// 写入数据
	for (int row = 0; row < dataModel->rowCount(); ++row) {
		for (int col = 0; col < dataModel->columnCount(); ++col) {
			QAxObject* cell = worksheet->querySubObject("Cells(int,int)", row + 2, col + 1);
			cell->dynamicCall("SetValue(const QString&)", dataModel->item(row, col)->text());
			cell->setProperty("HorizontalAlignment", -4108);
			delete cell;
		}
	}

	// 自动调整列宽
	for (int col = 1; col <= dataModel->columnCount(); ++col) {
		worksheet->querySubObject("Columns(int)", col)->dynamicCall("AutoFit()");
	}

	// 保存文件
	workbook->dynamicCall("SaveAs(const QString&)", fileName);
	workbook->dynamicCall("Close()");
	excel->dynamicCall("Quit()");

	// 清理COM对象
	delete worksheet;
	delete workbook;
	delete workbooks;
	delete excel;

	QMessageBox::information(this, tr("导出完成"),
		tr("成功导出到：%1").arg(fileName));
}