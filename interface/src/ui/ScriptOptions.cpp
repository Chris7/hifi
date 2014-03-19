#include "ScriptOptions.h"
#include "ui_scriptOptions.h"
#include <QDebug>

ScriptOptions::ScriptOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptOptions)
{
    ui->setupUi(this);

    // for connecting the kill/run signals
    _killScriptMapper = new QSignalMapper(this);
    connect(_killScriptMapper, SIGNAL(mapped(int)), this, SLOT(_killScript(int)));
    _reloadScriptMapper = new QSignalMapper(this);
    connect(_reloadScriptMapper, SIGNAL(mapped(const QString &)), this, SLOT(_reloadScript(const QString &)));
}

ScriptOptions::~ScriptOptions()
{
    delete ui;
}

void ScriptOptions::clearLayout(QLayout* layout, bool deleteWidgets = true)
{
    // from http://stackoverflow.com/questions/4272196/qt-remove-all-widgets-from-layout
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (deleteWidgets)
        {
            if (QWidget* widget = item->widget())
                delete widget;
        }
        if (QLayout* childLayout = item->layout())
            clearLayout(childLayout, deleteWidgets);
        delete item;
    }
}

void ScriptOptions::clearAllScripts()
{
    // this is an emergency function that should probably never be used
    clearLayout(ui->currentlyRunning, true);
    clearLayout(ui->recentScripts, true);
}

void ScriptOptions::addRunningScript(ScriptEngine* engine, QString scriptName)
{
    qDebug()<<scriptName<<"added";
    QHBoxLayout* script_layout = new QHBoxLayout();
    QLabel* labelText = new QLabel(scriptName);
    QPushButton* labelKill = new QPushButton();
    labelKill->setStyleSheet("border-image: url(:/icons/kill_scripts.svg");
    connect(labelKill, SIGNAL(clicked()), _killScriptMapper, SLOT(map()));
    int openID = _scriptMapping.size();
    _killScriptMapper->setMapping(labelKill, openID);
    _scriptMapping[engine] = openID;
    ScriptInfo scriptInfo;
    scriptInfo.layout = script_layout;
    scriptInfo.scriptName = scriptName;
    _scriptInfo[openID] = scriptInfo;
    script_layout->addWidget(labelText);
    script_layout->addWidget(labelKill);
    script_layout->setAlignment(labelKill, Qt::AlignRight|Qt::AlignVCenter);
    script_layout->setAlignment(labelText, Qt::AlignLeft|Qt::AlignVCenter);
    ui->currentlyRunning->addLayout(script_layout);
}

void ScriptOptions::scriptFinished()
{
    // This method can ONLY be called by a signal/slot, otherwise
    // sender() will be NULL
    QObject* engine = QObject::sender();
    if (engine == NULL)
        return;
    ScriptEngine* scriptEngine = qobject_cast<ScriptEngine*>(engine);
    int scriptID = _scriptMapping[scriptEngine];
    removeRunningScript(scriptID);
    _scriptMapping.remove(scriptEngine);
    _scriptInfo.remove(scriptID);
}

void ScriptOptions::removeRunningScript(int scriptID)
{
    ScriptInfo scriptInfo = _scriptInfo[scriptID];
    QHBoxLayout* layout = scriptInfo.layout;
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (QWidget* widget = item->widget())
            delete widget;
    }
    delete layout;

}

void ScriptOptions::killScript(const QString &scriptName)
{
    qDebug()<<"kill"<<scriptName;
}

void ScriptOptions::reloadScript(const QString &scriptName)
{
    qDebug()<<"reload"<<scriptName;
}
