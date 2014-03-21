#ifndef SCRIPTOPTIONS_H
#define SCRIPTOPTIONS_H

#include <QWidget>
#include <QSignalMapper>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

#include "ScriptEngine.h"

namespace Ui {
class ScriptOptions;
}

class ScriptWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScriptWidget(QWidget *parent = 0, QString scriptName = 0);
    ~ScriptWidget();
    void inactiveScript();
    void activeScript();

    QString scriptName;
    QLabel* scriptLabel;
    QPushButton* scriptButton;
    QHBoxLayout* hlay;
    QList<ScriptEngine*> engines;

    void cleanupEngines();

signals:
    void clicked();

public slots:
    void buttonClicked();
};

class ScriptOptions : public QWidget
{
    Q_OBJECT

public:
    explicit ScriptOptions(QWidget *parent = 0);
    ~ScriptOptions();

    void loadRunningScripts();
    void loadRecentScripts();
    void addRunningScript(ScriptEngine* engine, QString scriptName);
    void removeRunningScript(int scriptID);
    void removeRunningScript(ScriptEngine* engine);


public slots:
//    void killScript(int);
    void scriptFinished();
    void scriptFinished(const QString&);
    void recentScriptClicked();

private:
    Ui::ScriptOptions *ui;
//    QSignalMapper* _killScriptMapper;
    int _scriptIndex = 0;
    QMap<QString, ScriptWidget*> _widgets;
    QMap<QString, QPushButton*> _pastScripts;
    QList<QPushButton*> _pastScriptsList;
    QMap<ScriptEngine*, int > _scriptMapping;
    QMap<int, ScriptWidget* > _scriptInfo;
    QPushButton* _reloadAllButton;
    QPushButton* _stopAllButton;
    QSet<int> stopped;
};

#endif // SCRIPTOPTIONS_H
