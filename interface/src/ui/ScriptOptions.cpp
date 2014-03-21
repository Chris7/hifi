#include "ScriptOptions.h"
#include "ui_scriptOptions.h"
#include <QDebug>
#include "Application.h"

ScriptWidget::ScriptWidget(QWidget *parent, QString scriptName):
    QWidget(parent),
    scriptName(scriptName)
{
    hlay = new QHBoxLayout();
    scriptButton = new QPushButton();
    scriptLabel = new QLabel();
    scriptButton->setFocusPolicy(Qt::NoFocus);
    QVBoxLayout* vlay = new QVBoxLayout();
    hlay->addWidget(scriptLabel, 0, Qt::AlignLeft|Qt::AlignVCenter);
    hlay->addWidget(scriptButton, 0, Qt::AlignRight|Qt::AlignVCenter);
    vlay->addLayout(hlay, 1);
    QFrame* sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);
    vlay->addWidget(sep, 0, Qt::AlignTop);
    connect(scriptButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    hlay->setStretch(0, 1);
    hlay->setStretch(1, 0);
    scriptLabel->setStyleSheet("color: dark-grey;");
    scriptButton->setStyleSheet("image: url(:/images/close.svg) 1 1 1 1;"
                             "border-top: 1px transparent;"
                             "border-left: 1px transparent;"
                             "border-right: 1px transparent;"
                             "border-bottom: 1px transparent;");
    hlay->setAlignment(scriptButton, Qt::AlignRight);
    setLayout(vlay);
}

ScriptWidget::~ScriptWidget()
{

}

void ScriptWidget::cleanupEngines()
{
    for(int i=engines.size()-1;i>=0;i--){
        if(engines[i] == NULL)
            engines.removeAt(i);
    }
    scriptLabel->setText(scriptName+" ("+QString::number(engines.size())+")");
}

void ScriptWidget::buttonClicked()
{
    // kill one by destroying the last added script
    qDebug()<<engines.size()<<"engines left";
    if(!engines.size()){
        scriptLabel->setText(scriptName+" ("+QString::number(engines.size())+")");
        this->deleteLater();
        return;
    }
    ScriptEngine* engine = engines.last();
    if(engine == NULL){
        engines.pop_back();
        scriptLabel->setText(scriptName+" ("+QString::number(engines.size())+")");
        buttonClicked();
    }
    else
        engine->stop();
}

void ScriptWidget::activeScript()
{
    scriptLabel->setText(scriptName+" ("+QString::number(engines.size())+")");
}

void ScriptWidget::inactiveScript()
{
    scriptLabel->setText(scriptName+" ("+QString::number(engines.size())+")");
}


ScriptOptions::ScriptOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptOptions)
{
    ui->setupUi(this);
    QPixmap pix(":/images/close.svg");
    ui->scriptOptionsClose->setFixedSize(pix.rect().size());
    ui->headerLayout->setAlignment(ui->scriptOptionsClose, Qt::AlignRight|Qt::AlignVCenter);
    ui->headerLayout->setAlignment(ui->runningScriptsLabel, Qt::AlignLeft|Qt::AlignVCenter);
}

ScriptOptions::~ScriptOptions()
{
    delete ui;
}

void ScriptOptions::addRunningScript(ScriptEngine* engine, QString scriptName)
{
    int openID = _scriptIndex++;
    _scriptMapping[engine] = openID;
    ScriptWidget* script;
    if (_widgets.contains(scriptName))
        script = _widgets[scriptName];
    else{
        script = new ScriptWidget(this, scriptName);
        _widgets[scriptName] = script;
        ui->currentlyRunning->addWidget(script, 1, Qt::AlignLeft);
    }
    script->engines.append(engine);
    script->activeScript();
    _scriptInfo[openID] = script;
    //add to recent scripts
    if(!_pastScripts.contains(scriptName)){
        QPushButton* recentScript = new QPushButton();
        recentScript->setFocusPolicy(Qt::NoFocus);
        connect(recentScript, SIGNAL(clicked()), this, SLOT(recentScriptClicked()));
        recentScript->setProperty("Script", scriptName);
        recentScript->setFlat(true);
        _pastScriptsList.push_front(recentScript);
        //renumber buttons
        for(int i=0;i<_pastScriptsList.size();i++){
            if(i==9)
                break;
            QString name = _pastScriptsList[i]->property("Script").toString();
            _pastScriptsList[i]->setText(QString::number(i+1)+". "+name);
        }
        ui->recentScripts->insertWidget(0, recentScript, 1, Qt::AlignLeft);
        _pastScripts[scriptName] = recentScript;
        // obliterate scripts beyond 9
        while(_pastScriptsList.size() > 9){
            QPushButton* button = _pastScriptsList.last();
            _pastScriptsList.pop_back();
            QString name = button->property("Script").toString();
            _pastScripts.remove(name);
            button->deleteLater();
        }
    }
}

void ScriptOptions::scriptFinished()
{
    // This method can ONLY be called by a signal/slot, otherwise
    // sender() will be NULL. This method may never be called
    // because the scriptengine is likely to not exist at this point.
    QObject* engine = QObject::sender();
    if (engine == NULL)
        return;
    ScriptEngine* scriptEngine = qobject_cast<ScriptEngine*>(engine);
    removeRunningScript(scriptEngine);
}

void ScriptOptions::scriptFinished(const QString& scriptFileName)
{
    // This method can ONLY be called by a signal/slot, otherwise
    // sender() will be NULL
    QObject* engine = QObject::sender();
    if (engine == NULL){
        // Script finished and engine is destroyed, we can fetch its widget with the filename
        if(_widgets.contains(scriptFileName)){
            ScriptWidget* scriptWidget = _widgets[scriptFileName];
            scriptWidget->cleanupEngines();
            if(!scriptWidget->engines.size()){
                ui->currentlyRunning->removeWidget(scriptWidget);
                scriptWidget->deleteLater();
                _widgets.remove(scriptFileName);
            }
        }
    }
    else{
        ScriptEngine* scriptEngine = qobject_cast<ScriptEngine*>(engine);
        removeRunningScript(scriptEngine);
    }
}

void ScriptOptions::removeRunningScript(int scriptID)
{
    if (!_scriptInfo.contains(scriptID))
        return;
    if (stopped.contains(scriptID))
        return;
    stopped.insert(scriptID);
    ScriptWidget* scriptWidget = _scriptInfo[scriptID];
    scriptWidget->inactiveScript();
    if(!scriptWidget->engines.size()){
        ui->currentlyRunning->removeWidget(scriptWidget);
        QString scriptName = scriptWidget->scriptName;
        scriptWidget->deleteLater();
        _widgets.remove(scriptName);
    }
}

void ScriptOptions::removeRunningScript(ScriptEngine* engine)
{
    if (!_scriptMapping.contains(engine))
        return;
    int scriptID = _scriptMapping[engine];
    if (stopped.contains(scriptID))
        return;
    stopped.insert(scriptID);
    ScriptWidget* scriptWidget = _scriptInfo[scriptID];
    scriptWidget->inactiveScript();
    scriptWidget->engines.removeAll(engine);
    if(!scriptWidget->engines.size()){
        ui->currentlyRunning->removeWidget(scriptWidget);
        QString scriptName = scriptWidget->scriptName;
        scriptWidget->deleteLater();
        _widgets.remove(scriptName);
    }
}

void ScriptOptions::recentScriptClicked()
{
    // This method can ONLY be called by a signal/slot, otherwise
    // sender() will be NULL
    QObject* sender = QObject::sender();
    if (sender == NULL)
        return;
    QPushButton* button = qobject_cast<QPushButton*>(sender);
    Application::getInstance()->loadScript(button->property("Script").toString());
}
