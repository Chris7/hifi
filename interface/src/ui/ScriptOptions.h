#ifndef SCRIPTOPTIONS_H
#define SCRIPTOPTIONS_H

#include <QWidget>
#include <QSignalMapper>
#include <QPushButton>
#include <QHBoxLayout>

#include "ScriptEngine.h"

namespace Ui {
class ScriptOptions;
}

struct ScriptInfo {
    QString scriptName;
    QHBoxLayout* layout;
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
    void killScript(const QString &scriptName);
    void reloadScript(const QString &scriptName);
    void scriptFinished();

private:
    Ui::ScriptOptions *ui;
    QSignalMapper* _killScriptMapper;
    QSignalMapper* _reloadScriptMapper;
    QMap<ScriptEngine*, int > _scriptMapping;
    QMap<int, ScriptInfo > _scriptInfo;
};

#endif // SCRIPTOPTIONS_H
