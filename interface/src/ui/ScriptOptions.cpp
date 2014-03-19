#include "ScriptOptions.h"
#include "ui_scriptOptions.h"

ScriptOptions::ScriptOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptOptions)
{
    ui->setupUi(this);

    // for connecting the kill/run signals
    killScriptMapper = new QSignalMapper(this);
    connect(killScriptMapper, SIGNAL(mapped(const QString &)), this, SLOT(killScript(const QString &)));
    reloadScriptMapper = new QSignalMapper(this);
    connect(reloadScriptMapper, SIGNAL(mapped(const QString &)), this, SLOT(reloadScript(const QString &)));

    // populate with currently running scripts
    loadRunningScripts();

    // take care of recently added now
    loadRecentScripts();

}

ScriptOptions::~ScriptOptions()
{
    delete ui;
}

void ScriptOptions::loadRunningScripts()
{
    Application* app = Application::getInstance();
    QStringList activeScripts = app->getActiveScripts();
    for(int i=0;i<activeScripts.size();i++) {
        QHBoxLayout* script_layout = new QHBoxLayout();
        QLabel* labelText = new QLabel(activeScripts[i]);
        QPushButton* labelKill = new QPushButton();
        labelKill->setStyleSheet("border-image: url(:/icons/kill_scripts.svg");
//        QPixmap pix(":/icons/kill_scripts.svg");
//        labelKill->setIcon(QIcon(pix));
//        labelKill->setIconSize(pix.rect().size());
        connect(labelKill, SIGNAL(clicked()), killScriptMapper, SLOT(map()));
        killScriptMapper->setMapping(labelKill, activeScripts[i]);
        script_layout->addWidget(labelText);
        script_layout->addWidget(labelKill);
        script_layout->setAlignment(labelKill, Qt::AlignRight|Qt::AlignVCenter);
        script_layout->setAlignment(labelText, Qt::AlignLeft|Qt::AlignVCenter);
        ui->currentlyRunning->addLayout(script_layout);
    }
}

void ScriptOptions::loadRecentScripts()
{
    Application* app = Application::getInstance();
    QStringList recentScripts = app->getRecentScripts();
    for(int i=0;i<recentScripts.size();i++) {
        QPushButton* labelReload = new QPushButton(recentScripts[i]);
        connect(labelReload, SIGNAL(clicked()), reloadScriptMapper, SLOT(map()));
        reloadScriptMapper->setMapping(labelReload, recentScripts[i]);
        ui->recentScripts->addWidget(labelReload);
    }
}

void ScriptOptions::killScript(const QString &scriptName)
{
    qDebug()<<"kill"<<scriptName;
}

void ScriptOptions::reloadScript(const QString &scriptName)
{
    qDebug()<<"reload"<<scriptName;
}
