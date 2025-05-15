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
	dataModel->setHeaderData(0, Qt::Horizontal, tr("��־���"));
	dataModel->setHeaderData(1, Qt::Horizontal, tr("��־����"));
	dataModel->setHeaderData(2, Qt::Horizontal, tr("��־����ʱ��"));
	dataModel->setHeaderData(3, Qt::Horizontal, tr("��־����"));
	dataModel->setHeaderData(4, Qt::Horizontal, tr("��־����"));
	dataModel->setHeaderData(5, Qt::Horizontal, tr("�澯����"));
	dataModel->setHeaderData(6, Qt::Horizontal, tr("�澯����"));
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
		{"1", "ϵͳ����", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),
		 "Info", "ϵͳ��������", "��", "[]"},
		{"2", "�¶ȸ澯", QDateTime::currentDateTime().addSecs(-3600).toString(),
		 "Warning", "CPU�¶ȳ�����ֵ", "�¶ȣ�85��", "{\"sensor\":\"CPU01\"}"}
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
	QString fileName = QFileDialog::getSaveFileName(this, tr("����Excel�ļ�"),
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
	worksheet->setProperty("Name", "��־����");
	for (int col = 0; col < dataModel->columnCount(); ++col) {
		QAxObject* cell = worksheet->querySubObject("Cells(int,int)", 1, col + 1);
		cell->dynamicCall("SetValue(const QString&)", dataModel->headerData(col, Qt::Horizontal));
		cell->setProperty("HorizontalAlignment", -4108); // ���ж���
		delete cell;
	}

	// д������
	for (int row = 0; row < dataModel->rowCount(); ++row) {
		for (int col = 0; col < dataModel->columnCount(); ++col) {
			QAxObject* cell = worksheet->querySubObject("Cells(int,int)", row + 2, col + 1);
			cell->dynamicCall("SetValue(const QString&)", dataModel->item(row, col)->text());
			cell->setProperty("HorizontalAlignment", -4108);
			delete cell;
		}
	}

	// �Զ������п�
	for (int col = 1; col <= dataModel->columnCount(); ++col) {
		worksheet->querySubObject("Columns(int)", col)->dynamicCall("AutoFit()");
	}

	// �����ļ�
	workbook->dynamicCall("SaveAs(const QString&)", fileName);
	workbook->dynamicCall("Close()");
	excel->dynamicCall("Quit()");

	// ����COM����
	delete worksheet;
	delete workbook;
	delete workbooks;
	delete excel;

	QMessageBox::information(this, tr("�������"),
		tr("�ɹ���������%1").arg(fileName));
}