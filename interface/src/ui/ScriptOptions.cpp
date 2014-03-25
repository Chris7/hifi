#include "ScriptOptions.h"
#include "ui_scriptOptions.h"
#include <QDebug>
#include "Application.h"
#include "Menu.h"

ScriptWidget::ScriptWidget(QWidget *parent, QString scriptName):
    QWidget(parent),
    scriptName(scriptName)
{
    setMouseTracking(true);
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
    sep->setStyleSheet("color: rgb(226, 226, 226);");
    vlay->addWidget(sep, 0, Qt::AlignTop);
    connect(scriptButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    hlay->setStretch(0, 1);
    hlay->setStretch(1, 0);
    scriptLabel->setStyleSheet("font: Helvetica, Arial, 'Dejavu Sans';"
                               "color: dark-grey;");
    scriptButton->setStyleSheet("image: url(:/icons/kill-script.svg) 1 1 1 1;"
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
        else if (engines[i]->isFinished())
            engines.removeAt(i);
    }
    scriptLabel->setText(scriptName+" ("+QString::number(engines.size())+")");
}

void ScriptWidget::buttonClicked()
{
    // kill one by destroying the last added script
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
    setFocus();
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
    //setMouseTracking(true);
    setStyleSheet("QLabel {"
                  "font: Helvetica, Arial, 'Dejavu Sans';"
                  "}");
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    // set localization to upper left of Application
    int x = Application::getInstance()->getGLWidget()->geometry().x();
    int y = Application::getInstance()->getGLWidget()->geometry().y();
    // Height seems to be about half where it needs to be. Probably
    // due to Menu not being drawn at time this is called. FIXME
    y += Menu::getInstance()->geometry().height()*2;
    move(x,y);
    QPixmap pix(":/images/close.svg");
    ui->scriptOptionsClose->setFixedSize(pix.rect().size());
    ui->headerLayout->setAlignment(ui->scriptOptionsClose, Qt::AlignRight|Qt::AlignVCenter);
    ui->headerLayout->setAlignment(ui->runningScriptsLabel, Qt::AlignLeft|Qt::AlignVCenter);
    connect(ui->stopAllButton, SIGNAL(clicked()), this, SLOT(killAll()));
    connect(ui->reloadAllButton, SIGNAL(clicked()), this, SLOT(reloadAll()));
    connect(ui->scriptOptionsClose, SIGNAL(clicked()), this, SLOT(hide()));
    // hide the recently loaded since it will not be populated at this point
    ui->recentScriptsWidget->hide();
    // have a hint to load scripts
    ui->scriptHint->setText("<font style='text-align:center;' color='#666666'>Scripts you are running will show up here.</font><br><a href='#launch_script'>Why not launch one?</a>");
    ui->scriptHint->setProperty("openExternalLinks", false);
    connect(ui->scriptHint, SIGNAL(linkActivated(QString)), this, SLOT(labelLinkClicked(QString)));
    ui->scriptHint->setStyleSheet("color: #666666;");
    ui->stackedWidget->setCurrentIndex(1);
}

void ScriptOptions::labelLinkClicked(QString link)
{
    if(link == "#launch_script"){
        Application::getInstance()->loadDialog();
    }
}

ScriptOptions::~ScriptOptions()
{
    delete ui;
}

void ScriptOptions::killAll(){
    Application::getInstance()->stopAllScripts();
    // clear all running
    QMapIterator<QString, ScriptWidget*> it(_widgets);
    while(it.hasNext()){
        it.next();
        it.value()->deleteLater();
    }
    _widgets.clear();
}

void ScriptOptions::reloadAll(){
    Application::getInstance()->reloadAllScripts();
}

void ScriptOptions::keyReleaseEvent(QKeyEvent *event){
    bool isMeta = event->modifiers().testFlag(Qt::ControlModifier);
    if(isMeta){
        switch (event->key()) {
            case Qt::Key_J:
                setVisible(false);
                break;
            default:
                break;
        }
    }
    else{
        switch (event->key()) {
            case Qt::Key_1:
                runRecent(1);
                break;
            case Qt::Key_2:
                runRecent(2);
                break;
            case Qt::Key_3:
                runRecent(3);
                break;
            case Qt::Key_4:
                runRecent(4);
                break;
            case Qt::Key_5:
                runRecent(5);
                break;
            case Qt::Key_6:
                runRecent(6);
                break;
            case Qt::Key_7:
                runRecent(7);
                break;
            case Qt::Key_8:
                runRecent(8);
                break;
            case Qt::Key_9:
                runRecent(9);
                break;
            default:
                break;
        }
    }
}

void ScriptOptions::runRecent(int scriptIndex)
{
    if(scriptIndex <= _pastScriptsList.size())
        Application::getInstance()->loadScript(_pastScriptsList[scriptIndex-1]->property("Script").toString());
    setFocus();
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
    // add to recent scripts
    if(!_pastScripts.contains(scriptName)){
        QPushButton* recentScript = new QPushButton();
        recentScript->setFocusPolicy(Qt::NoFocus);
        connect(recentScript, SIGNAL(clicked()), this, SLOT(recentScriptClicked()));
        recentScript->setProperty("Script", scriptName);
        recentScript->setFlat(true);
        _pastScriptsList.push_front(recentScript);
        // renumber buttons
        for(int i=0;i<_pastScriptsList.size();i++){
            if(i==9)
                break;
            QString name = _pastScriptsList[i]->property("Script").toString();
            _pastScriptsList[i]->setText(QString::number(i+1)+". "+name);
        }
        ui->recentScripts->insertWidget(0, recentScript, 1, Qt::AlignLeft);
        if(_pastScriptsList.size() < 9){
            QFrame* sep = new QFrame();
            sep->setFrameShape(QFrame::HLine);
            sep->setStyleSheet("color: rgb(226, 226, 226);");
            ui->recentScripts->insertWidget(1, sep, 0);
        }
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
    if(!firstScriptLoaded){
        // first load of a script
        ui->recentScriptsWidget->show();
        firstScriptLoaded = true;
    }
    // remove the script hint, show control buttons
    ui->stackedWidget->setCurrentIndex(0);

}

void ScriptOptions::scriptFinished(const QString& scriptFileName)
{
    if(_widgets.contains(scriptFileName)){
        ScriptWidget* scriptWidget = _widgets[scriptFileName];
        scriptWidget->cleanupEngines();
        if(!scriptWidget->engines.size()){
            ui->currentlyRunning->removeWidget(scriptWidget);
            scriptWidget->deleteLater();
            _widgets.remove(scriptFileName);
        }
    }
    if(_widgets.size() == 0){
        // no active scripts, hide controls and show script hint
        ui->stackedWidget->setCurrentIndex(1);
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
