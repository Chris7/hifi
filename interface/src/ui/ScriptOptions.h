#ifndef SCRIPTOPTIONS_H
#define SCRIPTOPTIONS_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLayout>

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
    void runRecent(int scriptIndex);
    void keyReleaseEvent(QKeyEvent*);
    bool firstScriptLoaded = false;

    QLabel* scriptHint;

public slots:
    void scriptFinished(const QString&);
    void recentScriptClicked();
    void killAll();
    void reloadAll();
    void labelLinkClicked(QString);
    void hideMe();

private:
    Ui::ScriptOptions *ui;
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
