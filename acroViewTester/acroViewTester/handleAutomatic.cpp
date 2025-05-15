#include "acroViewTester.h"
void acroViewTester::handleAutomatic()
{
    mAutomatic->TellDevReady("{\"ProjInfo\":{\"AdapterNum\":16}, \"SiteReady\" : [{\"SKTRdy\":\"0x0000\", \"SiteAlias\" : \"Site01\", \"\" : 1, \"SiteSN\" : \"1111\"}, {\"SKTRdy\":\"0x0000\",\"SiteAlias\" : \"Site02\",\"SiteEnvRdy\" : 1,\"SiteSN\" : \"2222\"}] }");
}

void acroViewTester::initAutoMatic()
{
    mAutomatic = ACAutomaticManager::instance()->GetAutomaticPlugin();
    //mAutomatic->QuerySiteEn();
    std::string RdyInfo = "3,10,D:/DeviceD/JunkaiTest/vauto/1.tsk,0,0,0,0,0,0,0,0,0,0,,,,,,,";
    mAutomatic->SetTask(RdyInfo);
    mAutomatic->TellDevReady("{\"ProjInfo\":{\"AdapterNum\":16}, \"SiteReady\" : [{\"SKTRdy\":\"0x00FF\", \"SiteAlias\" : \"Site01\", \"\" : 1, \"SiteSN\" : \"1111\"}, {\"SKTRdy\":\"0x00FF\",\"SiteAlias\" : \"Site02\",\"SiteEnvRdy\" : 1,\"SiteSN\" : \"2222\"}] }");

    connect(ui.btnSetTask, &QPushButton::clicked, this, &acroViewTester::onSetTaskClicked);
    connect(ui.btnTellDevReady, &QPushButton::clicked, this, &acroViewTester::onTellDevReadyClicked);
    connect(ui.btnSetDoneSite, &QPushButton::clicked, this, &acroViewTester::onSetDoneSiteClicked);
    connect(ui.btnQuerySiteMapping, &QPushButton::clicked, this, &acroViewTester::onbtnQuerySiteMappingClicked);
}


void acroViewTester::onSetTaskClicked()
{
    QString rdyInfo = ui.lineEditTaskFilePath->text();
    if (rdyInfo.isEmpty()) {
        QMessageBox::warning(this, "����", "������Ϣ����Ϊ�գ�");
        return;
    }
    // ʾ���߼�
    qDebug() << "��������:" << rdyInfo;
}

void acroViewTester::onTellDevReadyClicked()
{
    QString devReadyInfo = textEditDevReadyInfo->toPlainText();
    if (devReadyInfo.isEmpty()) {
        QMessageBox::warning(this, "����", "վ��ʹ����Ϣ����Ϊ�գ�");
        return;
    }
    // ʾ���߼�
    qDebug() << "վ��ʹ����Ϣ:" << devReadyInfo;
}

void acroViewTester::onSetDoneSiteClicked()
{
    QString siteIndex = lineEditSiteIndex->text();
    QString burnResult = lineEditBurnResult->text();
    if (siteIndex.isEmpty() || burnResult.isEmpty()) {
        QMessageBox::warning(this, "����", "վ���Ż���¼�������Ϊ�գ�");
        return;
    }
    // ʾ���߼�
    qDebug() << "վ����:" << siteIndex << "��¼���:" << burnResult;
}

void acroViewTester::onbtnQuerySiteMappingClicked()
{
    // ��ȡ��ǰĿ¼�µ� config.ini �ļ�·��
    QString configFilePath = QCoreApplication::applicationDirPath() + QDir::separator() + "Config.ini";

    // ����ļ��Ƿ����
    if (!QFile::exists(configFilePath)) {
        QMessageBox::warning(this, "����", "�����ļ� Config.ini �����ڣ�");
        return;
    }

    // ʹ�� QSettings ��ȡ ini �ļ�
    QSettings settings(configFilePath, QSettings::IniFormat);

    // ��ӡ��������
    qDebug() << "�����ļ�����:";
    foreach (const QString &group, settings.childGroups()) {
        settings.beginGroup(group);
        foreach (const QString &key, settings.childKeys()) {
            qDebug() << group << "/" << key << "=" << settings.value(key).toString();
        }
        settings.endGroup();
    }

    QString sitesAutoMap = settings.value("SitesAutoMap", "<Site01,1><Site02,2><Site03,3><Site04,4>").toString();

    qDebug() << "SitesAutoMap �ֶ�����:" << sitesAutoMap;

    if (sitesAutoMap.isEmpty()) {
        QMessageBox::warning(this, "����", "�����ļ���δ�ҵ� SitesAutoMap �ֶλ��ֶ�Ϊ�գ�");
        return;
    }

    // ���� SitesAutoMap �ֶ�
    QMap<QString, QString> siteMapping;
    QRegularExpression regex("<(\\w+),(\\d+)>"); // ƥ�� <Site01,1> ��ʽ
    QRegularExpressionMatchIterator it = regex.globalMatch(sitesAutoMap);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        if (match.hasMatch()) {
            QString siteAlias = match.captured(1); // վ�����
            QString siteNumber = match.captured(2); // վ����
            siteMapping[siteAlias] = siteNumber;
            qDebug() << "������վ��:" << siteAlias << "���:" << siteNumber;
        }
    }

    if (siteMapping.isEmpty()) {
        QMessageBox::warning(this, "����", "δ�������κ�վ��ӳ�䣡");
        return;
    }

    // ����Ƿ���� Site01
    if (!siteMapping.contains("Site01")) {
        QMessageBox::warning(this, "����", "δ�ҵ� Site01 ��ӳ�䣡");
        return;
    }

    // ���±����ʾ
    ui.tableWidgetSiteMapping->setRowCount(siteMapping.size());
    int row = 0;
    for (auto it = siteMapping.begin(); it != siteMapping.end(); ++it) {
        ui.tableWidgetSiteMapping->setItem(row, 0, new QTableWidgetItem(it.key()));   // ����
        ui.tableWidgetSiteMapping->setItem(row, 1, new QTableWidgetItem(it.value())); // ���
        row++;
    }

    // ��ӡ Site01 �ı��
    qDebug() << "Site01 �ı��Ϊ:" << siteMapping["Site01"];
}