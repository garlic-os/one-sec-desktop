#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsOpacityEffect>
#include <QMainWindow>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent,
               const QString &program,
               const QStringList &arguments,
               QString progName);
    ~MainWindow();

    void ui_buttonContinue_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsOpacityEffect    *opacityBreathe;
    QGraphicsOpacityEffect    *opacityReturn;
    QGraphicsOpacityEffect    *opacityAttempts;
    QGraphicsOpacityEffect    *opacityBg;
    QPropertyAnimation        *animBgFadeOut;
    QPropertyAnimation        *animWidgetBgGradientMoveUp;
    QPropertyAnimation        *animBreatheFadeIn;
    QParallelAnimationGroup   *animInhale;
    QPropertyAnimation        *animWidgetBgGradientMoveDown;
    QPropertyAnimation        *animBreatheFadeOut;
    QParallelAnimationGroup   *animExhale;
    QSequentialAnimationGroup *animBreathe;
    QPropertyAnimation        *animReturnFadeIn;
    QPropertyAnimation        *animAttemptsFadeOut;
    QParallelAnimationGroup   *animReturn;
};
#endif // MAINWINDOW_H
