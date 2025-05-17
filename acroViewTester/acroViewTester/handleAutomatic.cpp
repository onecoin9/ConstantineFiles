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
        QMessageBox::warning(this, "警告", "任务信息不能为空!");
        return;
    }
    // 示例逻辑
    qDebug() << "设置任务:" << rdyInfo;
}

void acroViewTester::onTellDevReadyClicked()
{
    QString devReadyInfo = textEditDevReadyInfo->toPlainText();
    if (devReadyInfo.isEmpty()) {
        QMessageBox::warning(this, "警告", "站点使能信息不能为空！");
        return;
    }
    // 示例逻辑
    qDebug() << "站点使能信息:" << devReadyInfo;
}

void acroViewTester::onSetDoneSiteClicked()
{
    QString siteIndex = lineEditSiteIndex->text();
    QString burnResult = lineEditBurnResult->text();
    if (siteIndex.isEmpty() || burnResult.isEmpty()) {
        QMessageBox::warning(this, "警告", "站点编号或烧录结果不能为空！");
        return;
    }
    // 示例逻辑
    qDebug() << "站点编号:" << siteIndex << "烧录结果:" << burnResult;
}

void acroViewTester::onbtnQuerySiteMappingClicked()
{
    // 获取当前目录下的 config.ini 文件路径
    QString configFilePath = QCoreApplication::applicationDirPath() + QDir::separator() + "Config.ini";

    // 检查文件是否存在
    if (!QFile::exists(configFilePath)) {
        QMessageBox::warning(this, "错误", "配置文件 Config.ini 不存在！");
        return;
    }

    // 使用 QSettings 读取 ini 文件
    QSettings settings(configFilePath, QSettings::IniFormat);

    // 打印所有内容
    qDebug() << "配置文件内容:";
    foreach (const QString &group, settings.childGroups()) {
        settings.beginGroup(group);
        foreach (const QString &key, settings.childKeys()) {
            qDebug() << group << "/" << key << "=" << settings.value(key).toString();
        }
        settings.endGroup();
    }

    QString sitesAutoMap = settings.value("SitesAutoMap", "<Site01,1><Site02,2><Site03,3><Site04,4>").toString();

    qDebug() << "SitesAutoMap 字段内容:" << sitesAutoMap;

    if (sitesAutoMap.isEmpty()) {
        QMessageBox::warning(this, "错误", "配置文件中未找到 SitesAutoMap 字段或字段为空！");
        return;
    }

    // 解析 SitesAutoMap 字段
    QMap<QString, QString> siteMapping;
    QRegularExpression regex("<(\\w+),(\\d+)>"); // 匹配 <Site01,1> 格式
    QRegularExpressionMatchIterator it = regex.globalMatch(sitesAutoMap);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        if (match.hasMatch()) {
            QString siteAlias = match.captured(1); // 站点别名
            QString siteNumber = match.captured(2); // 站点编号
            siteMapping[siteAlias] = siteNumber;
            qDebug() << "解析到站点:" << siteAlias << "编号:" << siteNumber;
        }
    }

    if (siteMapping.isEmpty()) {
        QMessageBox::warning(this, "错误", "未解析到任何站点映射！");
        return;
    }

    // 检查是否包含 Site01
    if (!siteMapping.contains("Site01")) {
        QMessageBox::warning(this, "错误", "未找到 Site01 的映射！");
        return;
    }

    // 更新表格显示
    ui.tableWidgetSiteMapping->setRowCount(siteMapping.size());
    int row = 0;
    for (auto it = siteMapping.begin(); it != siteMapping.end(); ++it) {
        ui.tableWidgetSiteMapping->setItem(row, 0, new QTableWidgetItem(it.key()));   // 别名
        ui.tableWidgetSiteMapping->setItem(row, 1, new QTableWidgetItem(it.value())); // 编号
        row++;
    }

    // 打印 Site01 的编号
    qDebug() << "Site01 的编号为:" << siteMapping["Site01"];
}