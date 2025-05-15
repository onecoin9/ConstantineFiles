#include "acroViewTester.h"

void acroViewTester::openConfigDialog()
{
    if (!m_configDialog) {
        m_configDialog = new ConfigDialog(this);
        QVBoxLayout* groupBoxLayout = new QVBoxLayout(ui.groupBoxMT);
        groupBoxLayout->setContentsMargins(0, 0, 0, 0);
        groupBoxLayout->addWidget(m_configDialog);
        m_configDialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    m_configDialog->show();
}

void acroViewTester::openFileApp()
{
    QSettings settings("AcroView", "acroViewTester");
    QString path = settings.value("Paths/Aprog2Path").toString();

    if (path.isEmpty()) {
        QMessageBox::warning(this, "����", "������·��");
        return;
    }

    QString filePath = path + "/aprog.exe";
    if (!QFile::exists(filePath)) {
        QMessageBox::warning(this, "����", "�ļ�������");
        return;
    }
    QProcess* process = new QProcess(this);
    process->start(filePath, QStringList() << "-r");
    QTimer::singleShot(3000, this, [this]() {
        this->raise();          
        this->activateWindow(); 
        });
}

void acroViewTester::loadViewSettings()
{
    QStringList visibleViews = settings.value("VisibleViews", QStringList()
        << "listViewProdInfo"
        << "tabWidgetMainView"
        << "tabWidgetViceView").toStringList();

    for (auto it = viewWidgets.begin(); it != viewWidgets.end(); ++it) {
        bool isVisible = visibleViews.contains(it.key());
        viewActions[it.key()]->setChecked(isVisible);
        it.value()->setVisible(isVisible);
    }
}

void acroViewTester::saveViewSettings()
{
    QStringList visibleViews;
    for (auto it = viewWidgets.begin(); it != viewWidgets.end(); ++it) {
        if (it.value()->isVisible()) {
            visibleViews << it.key();
        }
    }
    settings.setValue("VisibleViews", visibleViews);
    settings.sync();
}

void acroViewTester::onViewActionToggled(bool checked)
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) return;

    QString widgetKey = action->data().toString();
    if (viewWidgets.contains(widgetKey)) {
        viewWidgets[widgetKey]->setVisible(checked);
        saveViewSettings();
    }
}

void acroViewTester::setupExpandButton()
{
    QString buttonStyle =
        "QPushButton {"
        "    text-align: left;"
        "    padding-left: 25px;"
        "    padding-right: 25px;"
        "    border: 1px solid gray;"
        "    border-radius: 3px;"
        "    background-color: #B0C4DE;"
        "    color: black;"
        "    font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #9CB4D8;"
        "}";

    ui.pushButtonExpand->setStyleSheet(buttonStyle);
    ui.pushButtonExpand->setText(QString("��%1 - 36.36%").arg(1, 2, 10, QChar('0')));
    ui.pushButtonExpand->setMinimumWidth(200);
    ui.pushButtonExpand->setFixedHeight(25);
    m_statusDot = new QLabel(ui.pushButtonExpand);
    m_statusDot->setFixedSize(12, 12);
    m_statusDot->setStyleSheet(
        "background-color: yellow;"
        "border-radius: 6px;"
    );

    int dotY = (ui.pushButtonExpand->height() - m_statusDot->height()) / 2;
    m_statusDot->move(5, dotY);
    m_arrowIcon = new QLabel(ui.pushButtonExpand);
    m_arrowIcon->setFixedSize(16, 16);
    m_arrowIcon->setScaledContents(true);
    updateExpandButton(false);
}

void acroViewTester::updateExpandButton(bool expanded)
{
    QString iconPath = expanded ? ":/acroViewTester/qrc/pics/doubleUp.png"
        : ":/acroViewTester/qrc/pics/doubleDown .png";
    QPixmap arrowPixmap(iconPath);

    if (arrowPixmap.isNull()) {
        qDebug() << "Failed to load icon:" << iconPath;
        return;
    }

    m_arrowIcon->setPixmap(arrowPixmap);
    int arrowY = (ui.pushButtonExpand->height() - m_arrowIcon->height()) / 2;
    m_arrowIcon->move(ui.pushButtonExpand->width() - m_arrowIcon->width() - 5, arrowY);
}

void acroViewTester::on_pushButtonExpand_clicked()
{
    m_isExpanded = !m_isExpanded;
    updateExpandButton(m_isExpanded);

    if (m_isExpanded) {
        showWidgetA();
    }
    else {
        if (m_widgetA) {
            m_widgetA->hide();
        }
    }
}

void acroViewTester::showWidgetA()
{
    if (!m_widgetA) {
        m_widgetA = new QWidget(ui.tab6);
        QGridLayout* layout = new QGridLayout(m_widgetA);
        layout->setSpacing(2);
        layout->setContentsMargins(10, 10, 10, 10);

        QWidget* contentWidget = createContentWidget(0);
        contentWidget->setStyleSheet("border: 1px solid black;");
        contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        layout->addWidget(contentWidget, 0, 0); // ������ (0, 0) λ��

        m_widgetA->setLayout(layout);
        QRect buttonRect = ui.pushButtonExpand->geometry();
        m_widgetA->move(buttonRect.x() - 10, buttonRect.y() + buttonRect.height());
    }

    m_widgetA->show();
}

QWidget* acroViewTester::createContentWidget(int index)
{
    QWidget* widget = new QWidget();
    widget->setMinimumSize(460, 210);
    widget->setStyleSheet(
        "QWidget {"
        "   background-color: #deeefe;"
        "   border: 1px solid #D3D3D3;"
        "}"
    );

    QLabel* titleLabel = new QLabel(QString("��%1 - 36.36%").arg(index + 1, 2, 10, QChar('0')), widget);

    titleLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   color: black;"
        "   background-color: #B0C4DE;"
        "   padding: 2px;"
        "}"
    );

    struct DataItem {
        QString label;
        QString value;
        QString bgColor;
        int yPos;
    };

    QVector<DataItem> items = {
        {"��Ʒ��:", "8", "#90EE90", 30},
        {"������:", "14", "#FFB6C1", 55},
        {"�ز���:", "0", "#E6E6FA", 80},
        {"����%:", "36.36", "transparent", 105},
        {"��Ʒ:", "LP00820", "transparent", 130},
        {"���²���Ա:", "OP6705", "transparent", 155},
        {"���²���ʱ��:", "2025.04.07 12:31:11", "transparent", 180}
    };

    const int leftMargin = 4;
    const int labelWidth = 100;
    const int spacing = 0;
    const int valueWidth = 180;
    const int rowHeight = 20;

    for (const auto& item : items) {
        QLabel* label = new QLabel(widget);
        label->setText(item.label);
        label->setFixedSize(labelWidth, rowHeight);
        label->move(leftMargin, item.yPos);
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        label->setStyleSheet(
            "QLabel {"
            "   border: none;"
            "   color: black;"
            "}"
        );

        QLabel* value = new QLabel(widget);
        value->setText(item.value);
        value->setFixedSize(valueWidth, rowHeight);
        value->move(leftMargin + labelWidth + spacing, item.yPos);
        value->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        if (item.bgColor != "transparent") {
            value->setStyleSheet(QString(
                "QLabel {"
                "   background-color: %1;"
                "   padding: 2px;"
                "   border-radius: 2px;"
                "}"
            ).arg(item.bgColor));
        }
        else {
            value->setStyleSheet(
                "QLabel {"
                "   border: none;"
                "   color: black;"
                "}"
            );
        }
    }

    int totalWidth = leftMargin + labelWidth + spacing + valueWidth + leftMargin;
    widget->setFixedWidth(totalWidth);
    titleLabel->setMinimumSize(totalWidth, 20);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return widget;
}

QWidget* acroViewTester::createStatusLegend() {
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    struct StatusItem {
        QString color;
        QString text;
    };

    QVector<StatusItem> items = {
        {"#808080", "����"},
        {"#00FF00", "���� >= 90%"},
        {"#FFFF00", "���� < 90%"},
        {"#FF0000", "���� < 60%"}
    };

    for (const auto& item : items) {
        QHBoxLayout* hLayout = new QHBoxLayout();
        QLabel* colorLabel = new QLabel();
        colorLabel->setFixedSize(15, 15);
        colorLabel->setStyleSheet(QString("background-color: %1; border-radius: 7px;").arg(item.color));

        QLabel* textLabel = new QLabel(item.text);

        hLayout->addWidget(colorLabel);
        hLayout->addWidget(textLabel);
        hLayout->addStretch();
        layout->addLayout(hLayout);
    }

    return widget;
}

void acroViewTester::addLegendToGroupBox() 
{
    QWidget* legend = createStatusLegend();
    QVBoxLayout* layout = new QVBoxLayout(ui.groupBoxLibLabel);
    ui.groupBoxLibLabel->setStyleSheet("QGroupBox { border: 2px solid black; }");
    layout->addWidget(legend);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
}

void acroViewTester::setupViceView()
{
    m_scrollArea = new QScrollArea(ui.groupBoxViceView);
    m_scrollContent = new QWidget();

    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollBar:vertical {"
        "    border: none;"
        "    background: #F0F0F0;"
        "    width: 10px;"
        "    margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: #CCCCCC;"
        "    min-height: 20px;"
        "    border-radius: 5px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0px;"
        "}"
    );

    QVBoxLayout* groupBoxLayout = new QVBoxLayout(ui.groupBoxViceView);
    groupBoxLayout->setContentsMargins(5, 5, 5, 5);
    groupBoxLayout->addWidget(m_scrollArea);

    m_scrollArea->setWidget(m_scrollContent);
    QSettings settings("AcroView", "acroViewTester");
    QString currentScene = settings.value("Scene/CurrentScene", "").toString();
    if (!currentScene.isEmpty()) {
        onSceneChanged(currentScene);
    }
}

void acroViewTester::clearViceViewControls()
{
    for (const auto& pair : m_viceViewControls.pathLabels) {
        delete pair.first;
        delete pair.second;
    }
    m_viceViewControls.pathLabels.clear();

    m_scrollContent->setMinimumSize(0, 0);
}

void acroViewTester::createPathLabel(const QString& label, const QString& value, int row)
{
    int leftMargin = 10;
    int topMargin = 10;
    int rowHeight = 25;
    int labelWidth = 100;
    int valueWidth = 300;
    int valueLeftMargin = leftMargin + labelWidth + 10;

    // ������ǩ
    QLabel* labelWidget = new QLabel(label + ":", m_scrollContent);  // ע�⸸widget��Ϊm_scrollContent
    QLabel* valueWidget = new QLabel(value, m_scrollContent);

    // ����λ��
    labelWidget->setGeometry(leftMargin, topMargin + row * rowHeight, labelWidth, rowHeight);
    valueWidget->setGeometry(valueLeftMargin, topMargin + row * rowHeight, valueWidth, rowHeight);

    // ������ʽ
    labelWidget->setStyleSheet("QLabel { color: black; font-size: 12px; }");
    valueWidget->setStyleSheet("QLabel { color: blue; font-size: 12px; }");

    // �����ǩ��
    m_viceViewControls.pathLabels.append(qMakePair(labelWidget, valueWidget));

    // ��������widget�Ĵ�С����Ӧ���б�ǩ
    int contentHeight = topMargin + (row + 1) * rowHeight + 10;  // ��ӵײ��߾�
    int contentWidth = valueLeftMargin + valueWidth + 10;        // ����Ҳ�߾�
    m_scrollContent->setMinimumSize(contentWidth, contentHeight);
}

void acroViewTester::showAgingTestControls()
{
    clearViceViewControls();
    QSettings settings("AcroView", "acroViewTester");

    createPathLabel("��־·��", settings.value("Paths/LogPath").toString(), 0);
    createPathLabel("��־ת��·��", settings.value("Paths/LogTransPath").toString(), 1);
    createPathLabel("����·��", settings.value("Paths/ReportPath").toString(), 2);
    createPathLabel("�Զ���·��", settings.value("Paths/AutoPath").toString(), 3);
    createPathLabel("MES·��", settings.value("Paths/MesPath").toString(), 4);
    createPathLabel("�䷽·��", settings.value("Paths/RecipePath").toString(), 5);
    createPathLabel("��Ŀ·��", settings.value("Paths/ProjectPath").toString(), 6);
    createPathLabel("Aprog2·��", settings.value("Paths/Aprog2Path").toString(), 7);
    createPathLabel("MultiAprog·��", settings.value("Paths/MultiAprogPath").toString(), 8);
}

void acroViewTester::showAG06Controls()
{
    clearViceViewControls();
    QSettings settings("AcroView", "acroViewTester");
    createPathLabel("AG06��־·��", settings.value("Paths/LogPath").toString(), 0);
    createPathLabel("AG06����·��", settings.value("Paths/RecipePath").toString(), 1);
    createPathLabel("AG06����·��", settings.value("Paths/ReportPath").toString(), 2);
    createPathLabel("AG06�Զ���·��", settings.value("Paths/AutoPath").toString(), 3);
    createPathLabel("AG06MES·��", settings.value("Paths/MesPath").toString(), 4);
    createPathLabel("AG06�䷽·��", settings.value("Paths/RecipePath").toString(), 5);
    createPathLabel("��Ŀ·��", settings.value("Paths/ProjectPath").toString(), 6);
    createPathLabel("Aprog2·��", settings.value("Paths/Aprog2Path").toString(), 7);
}

void acroViewTester::showAP8000Controls()
{
    clearViceViewControls();
    QSettings settings("AcroView", "acroViewTester");

    createPathLabel("AP8000��־·��", settings.value("Paths/LogPath").toString(), 0);
    createPathLabel("AP8000����·��", settings.value("Paths/RecipePath").toString(), 1);
    createPathLabel("AP8000����·��", settings.value("Paths/ReportPath").toString(), 2);
    createPathLabel("AP8000�Զ���·��", settings.value("Paths/AutoPath").toString(), 3);
    createPathLabel("AP8000MES·��", settings.value("Paths/MesPath").toString(), 4);
    createPathLabel("AP8000�䷽·��", settings.value("Paths/RecipePath").toString(), 5);
    createPathLabel("��Ŀ·��", settings.value("Paths/ProjectPath").toString(), 6);
    createPathLabel("MultiAprog·��", settings.value("Paths/MultiAprogPath").toString(), 7);
}

void acroViewTester::onSceneChanged(const QString& scene)
{
    if (scene == "�ϻ�����") {
        showAgingTestControls();
    }
    else if (scene == "AG06") {
        showAG06Controls();
    }
    else if (scene == "AP8000") {
        showAP8000Controls();
    }
}

void acroViewTester::onPushButtonStartClicked() {
    InputDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QSettings settings("AcroView", "acroViewTester");
        QString customerCode = settings.value("customerCode").toString();
        QString productModel = settings.value("productModel").toString();
        QString weekly = settings.value("weekly").toString();
        QString customerBatch = settings.value("customerBatch").toString();
        QString workOrder = settings.value("workOrder").toString();
        QString workStep = settings.value("workStep").toString();
        QString process = settings.value("process").toString();
        QString deviceNumber = settings.value("deviceNumber").toString();

        testInfo.customerCode = customerCode;
        testInfo.productModel = productModel;
        testInfo.weekly = weekly;
        testInfo.customerBatch = customerBatch;
        testInfo.workOrder = workOrder;
        testInfo.workStep = workStep;
        testInfo.process = process;
        testInfo.deviceNumber = deviceNumber;
    }
}

void acroViewTester::setupMeasurementDialog() {
    MeasurementDialog* measurementDialog = new MeasurementDialog(this);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(measurementDialog, 0, Qt::AlignLeft | Qt::AlignTop);

    ui.groupBoxMT->setLayout(layout);
}

void acroViewTester::setupTableView()
{
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({ "��λ", "����SN", "�������", "����SN",
                                     "ʧ��ʱ��", "���ʱ��", "����",
                                     "ʧ��ֵ", "����", "����", "�׶�" });

    QList<QStandardItem*> row;
    row << new QStandardItem("A01")
        << new QStandardItem("SN123456")
        << new QStandardItem("1")
        << new QStandardItem("SN789012")
        << new QStandardItem("2023-10-01 12:00:00")
        << new QStandardItem("+00:05:00")
        << new QStandardItem("Voltage")
        << new QStandardItem("3.2")
        << new QStandardItem("3.0")
        << new QStandardItem("3.5")
        << new QStandardItem("���Խ׶�1");
    model->appendRow(row);

    ui.tableViewBaseData->setModel(model);
    ui.tableViewBaseData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void acroViewTester::updateTableViewAlarmData()
{
    tableViewAlarmData = ui.tableViewAlarmData;

    // ����ģ�ͺ�����
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({ "����ʱ��", "����", "������", "��Ϣ" });

    QList<QStandardItem*> row1;
    row1 << new QStandardItem("2025-04-09 12:22:13")
        << new QStandardItem("Alarm")
        << new QStandardItem("1001")
        << new QStandardItem("LIBO1ͨѶ�쳣");

    QList<QStandardItem*> row2;
    row2 << new QStandardItem("2025-04-09 13:22:13")
        << new QStandardItem("Alarm")
        << new QStandardItem("1002")
        << new QStandardItem("LIBO2ͨѶ�쳣");

    model->appendRow(row1);
    model->appendRow(row2);

    tableViewAlarmData->setModel(model);
    tableViewAlarmData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void acroViewTester::createViewMenu()
{
    viewMenu = menuBar()->addMenu(tr("��ͼ"));
    viewWidgets = {
        {"listViewProdInfo", ui.listViewProdInfo},
        {"tabWidgetMainView", ui.tabWidgetMainView},
        {"groupBoxViceView", ui.groupBoxViceView}
    };
    QMap<QString, QString> viewNames = {
        {"listViewProdInfo", tr("��Ʒ��Ϣ�б�")},
        {"tabWidgetMainView", tr("����ͼ")},
        {"groupBoxViceView", tr("����ͼ")}
    };

    for (auto it = viewWidgets.begin(); it != viewWidgets.end(); ++it) {
        QAction* action = new QAction(viewNames[it.key()], this);
        action->setCheckable(true);
        action->setChecked(true);
        action->setData(it.key());
        connect(action, &QAction::toggled, this, &acroViewTester::onViewActionToggled);
        viewMenu->addAction(action);
        viewActions[it.key()] = action;
    }
}