#ifndef SCRIPTOPTIONS_H
#define SCRIPTOPTIONS_H

#include <QWidget>
#include <QSignalMapper>
#include <QPushButton>

#include <Application.h>

namespace Ui {
class ScriptOptions;
}

class ScriptOptions : public QWidget
{
    Q_OBJECT

public:
    explicit ScriptOptions(QWidget *parent = 0);
    ~ScriptOptions();

    void loadRunningScripts();
    void loadRecentScripts();

public slots:
    void killScript(const QString &scriptName);
    void reloadScript(const QString &scriptName);

private:
    Ui::ScriptOptions *ui;
    QSignalMapper* killScriptMapper;
    QSignalMapper* reloadScriptMapper;
};

#endif // SCRIPTOPTIONS_H
