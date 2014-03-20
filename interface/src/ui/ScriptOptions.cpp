#include "ScriptOptions.h"
#include "ui_scriptOptions.h"
#include <QDebug>
#include "Application.h"

ScriptWidget::ScriptWidget(QWidget *parent, QString scriptName, bool running):
    QWidget(parent),
    scriptName(scriptName),
    running(running)
{
    hlay = new QHBoxLayout();
    scriptButton = new QPushButton();
    scriptLabel = new QLabel();
    scriptButton->setFocusPolicy(Qt::NoFocus);
    if (running)
        activeScript();
    else
        inactiveScript();
    QVBoxLayout* vlay = new QVBoxLayout();
    hlay->addWidget(scriptLabel, 0, Qt::AlignLeft|Qt::AlignVCenter);
    hlay->addWidget(scriptButton, 0, Qt::AlignRight|Qt::AlignVCenter);
    vlay->addLayout(hlay, 1);
    QFrame* sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);
    sep->setFixedWidth(parent->width());
    vlay->addWidget(sep, 0, Qt::AlignTop);
    connect(scriptButton, SIGNAL(clicked()), this, SIGNAL(clicked()));
    setLayout(vlay);
}

ScriptWidget::~ScriptWidget()
{

}

void ScriptWidget::activeScript()
{
    running = true;
    hlay->setStretch(0, 1);
    hlay->setStretch(1, 0);
    scriptLabel->setText(scriptName);
    scriptLabel->setStyleSheet("color: dark-grey;");
    scriptButton->setStyleSheet("image: url(:/images/close.svg) 1 1 1 1;"
                             "border-top: 1px transparent;"
                             "border-left: 1px transparent;"
                             "border-right: 1px transparent;"
                             "border-bottom: 1px transparent;");
}

void ScriptWidget::inactiveScript()
{
    scriptLabel->setText(QString::number(parentLayout->indexOf(this)+1));
    scriptButton->setStyleSheet("color: dark-grey;");
    scriptButton->setFlat(true);
    scriptButton->setText(scriptName);
    hlay->setStretch(0, 0);
    hlay->setStretch(1, 1);
    running = false;
}


ScriptOptions::ScriptOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptOptions)
{
    ui->setupUi(this);
    // for connecting the kill/run signals
    _killScriptMapper = new QSignalMapper(this);
    connect(_killScriptMapper, SIGNAL(mapped(int)), this, SLOT(killScript(int)));
    QPixmap pix(":/images/close.svg");
    ui->scriptOptionsClose->setFixedSize(pix.rect().size());
    ui->headerLayout->setAlignment(ui->scriptOptionsClose, Qt::AlignRight|Qt::AlignVCenter);
    ui->headerLayout->setAlignment(ui->runningScriptsLabel, Qt::AlignLeft|Qt::AlignVCenter);
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
    ScriptWidget* newScript = new ScriptWidget(this, scriptName, true);
    connect(newScript, SIGNAL(clicked()), _killScriptMapper, SLOT(map()));
    int openID = _scriptMapping.size();
    _killScriptMapper->setMapping(newScript, openID);
    _scriptMapping[engine] = openID;
    ui->currentlyRunning->addWidget(newScript, 1, Qt::AlignLeft);
    _scriptInfo[openID] = newScript;
}

void ScriptOptions::scriptFinished()
{
    // This method can ONLY be called by a signal/slot, otherwise
    // sender() will be NULL
    QObject* engine = QObject::sender();
    if (engine == NULL)
        return;
    ScriptEngine* scriptEngine = qobject_cast<ScriptEngine*>(engine);
    if (!_scriptMapping.contains(scriptEngine))
        return;
    int scriptID = _scriptMapping[scriptEngine];
    removeRunningScript(scriptID);
    _scriptMapping.remove(scriptEngine);
    _scriptInfo.remove(scriptID);
}

void ScriptOptions::scriptFinished(const QString& _)
{
    // This method can ONLY be called by a signal/slot, otherwise
    // sender() will be NULL
    QObject* engine = QObject::sender();
    if (engine == NULL)
        return;
    ScriptEngine* scriptEngine = qobject_cast<ScriptEngine*>(engine);
    if (!_scriptMapping.contains(scriptEngine))
        return;
    int scriptID = _scriptMapping[scriptEngine];
    removeRunningScript(scriptID);
    _scriptMapping.remove(scriptEngine);
    _scriptInfo.remove(scriptID);
}

void ScriptOptions::removeRunningScript(int scriptID)
{
    if (!_scriptInfo.contains(scriptID))
        return;
    ScriptWidget* script = _scriptInfo[scriptID];
    ui->currentlyRunning->removeWidget(script);
    // Add to our past run scripts if it doesn't exist
    for(int i=0;i<ui->recentScripts->count();i++){
        QLayoutItem* item = ui->recentScripts->takeAt(i);
        if (QWidget* widget = item->widget()){
            ScriptWidget* l_script = qobject_cast<ScriptWidget*>(widget);
            if(l_script->scriptName == script->scriptName && script != l_script){
                l_script->deleteLater();
                return;
            }
        }
    }
    ui->recentScripts->addWidget(script,1, Qt::AlignLeft|Qt::AlignVCenter);
    script->parentLayout=ui->recentScripts;
    script->inactiveScript();
}

void ScriptOptions::killScript(int scriptID)
{
    if (!_scriptInfo.contains(scriptID))
        return;
    ScriptWidget* script = _scriptInfo[scriptID];
    if(script->running)
        removeRunningScript(scriptID);
    else
        Application::getInstance()->loadScript(script->scriptName);
}

