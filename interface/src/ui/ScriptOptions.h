#ifndef SCRIPTOPTIONS_H
#define SCRIPTOPTIONS_H

#include <QWidget>
#include <QSignalMapper>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ScriptEngine.h"

namespace Ui {
class ScriptOptions;
}

class ScriptWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScriptWidget(QWidget *parent = 0, QString scriptName = 0, bool running = true);
    ~ScriptWidget();
    void inactiveScript();
    void activeScript();

    QString scriptName;
    bool running;
    QLabel* scriptLabel;
    QPushButton* scriptButton;
    QHBoxLayout* hlay;
    QVBoxLayout* parentLayout;

signals:
    void clicked();
};

class ScriptOptions : public QWidget
{
    Q_OBJECT

public:
    explicit ScriptOptions(QWidget *parent = 0);
    ~ScriptOptions();

    void clearAllScripts();
    void loadRunningScripts();
    void loadRecentScripts();
    void clearLayout(QLayout*, bool);
    void addRunningScript(ScriptEngine* engine, QString scriptName);
    void removeRunningScript(int scriptID);


public slots:
    void killScript(int);
    void scriptFinished();
    void scriptFinished(const QString&);

private:
    Ui::ScriptOptions *ui;
    QSignalMapper* _killScriptMapper;
    QMap<ScriptEngine*, int > _scriptMapping;
    QMap<int, ScriptWidget* > _scriptInfo;
    QPushButton* _reloadAllButton;
    QPushButton* _stopAllButton;
};

#endif // SCRIPTOPTIONS_H
