#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Qt>
#include <QDateTime>
#include <QFile>
#include <QGraphicsOpacityEffect>
#include <QList>
#include <QMessageBox>
#include <QObject>
#include <QParallelAnimationGroup>
#include <QProcess>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSequentialAnimationGroup>
#include <QSettings>
#include <QVariant>

// void MainWindow::ui_buttonContinue_clicked() {
//     bool success = QProcess::startDetached(program, arguments);
//     if (!success) {
//         QMessageBox::critical(this, "Error", "Failed to start program: " + program);
//     }
//     qApp->quit();

// }

MainWindow::MainWindow(QWidget *parent,
           const QString &program,
           const QStringList &arguments,
           QString progName)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    setFixedSize(width(), height());

    // Load theme
    QFile themeFile(":/style.qss");
    themeFile.open(QFile::ReadOnly | QFile::Text);
    QTextStream themeFileStream(&themeFile);
    qApp->setStyleSheet(themeFileStream.readAll());

    // Connect up the Continue button
    // (Cancel button connected from within .ui file)
    QObject::connect(ui->buttonContinue, &QPushButton::clicked, this, [=]() {
        bool success = QProcess::startDetached(program, arguments);
        if (!success) {
            QMessageBox::critical(this, "Error", "Failed to start program: " + program);
        }
        qApp->quit();
    });

    // Remember attempt count
    QSettings settings;
    QString logName = "attempt-log" + program;
    QList<QVariant> attemptLog = settings.value(logName, {}).toList();
    attemptLog.append(QDateTime::currentSecsSinceEpoch());
    // Remove log entries from over 24h ago
    while (attemptLog.last().toUInt() - attemptLog[0].toUInt() > 24 * 60 * 60) {
        attemptLog.removeFirst();
    }
    ui->labelAttemptCount->setText(QString::number(attemptLog.size()));
    settings.setValue(logName, attemptLog);

    // Set program name from args
    if (progName.size() == 0) {
        progName = program.split(' ').last();
    }
    ui->labelMsgAttempts->setText(ui->labelMsgAttempts->text().replace("{progName}", progName));
    ui->labelMsgReturn->setText(ui->labelMsgReturn->text().replace("{progName}", progName));
    ui->buttonCancel->setText(ui->buttonCancel->text().replace("{progName}", progName));
    ui->buttonContinue->setText(ui->buttonContinue->text().replace("{progName}", progName));

    // Say "Attempt" instead of "Attempts" if count is 1
    if (attemptLog.size() == 1) {
        ui->labelMsgAttempts->setText(ui->labelMsgAttempts->text().replace("Attempts", "Attempt"));
    }

    // Z order
    ui->widgetBgGradient->lower();
    ui->wrapperAttempts->lower();

    // Click through layers without buttons
    ui->wrapperBreathe->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->wrapperReturn->setAttribute(Qt::WA_TransparentForMouseEvents);

    // ----------------------------------------
    // Animations
    opacityBreathe = new QGraphicsOpacityEffect(this);
    ui->wrapperBreathe->setGraphicsEffect(opacityBreathe);

    opacityReturn = new QGraphicsOpacityEffect(this);
    ui->wrapperReturn->setGraphicsEffect(opacityReturn);

    opacityAttempts = new QGraphicsOpacityEffect(this);
    ui->wrapperAttempts->setGraphicsEffect(opacityAttempts);

    opacityBg = new QGraphicsOpacityEffect(this);
    ui->widgetBgGradient->setGraphicsEffect(opacityBg);

    animBgFadeOut = new QPropertyAnimation(opacityBg, "opacity");
    animBgFadeOut->setDuration(500);
    animBgFadeOut->setStartValue(1);
    animBgFadeOut->setEndValue(0);
    animBgFadeOut->setEasingCurve(QEasingCurve::OutBack);

    animWidgetBgGradientMoveUp = new QPropertyAnimation(ui->widgetBgGradient, "pos", this);
    animWidgetBgGradientMoveUp->setDuration(3000);
    animWidgetBgGradientMoveUp->setStartValue(QPoint(0, 600));
    animWidgetBgGradientMoveUp->setEndValue(QPoint(0, 0));
    animWidgetBgGradientMoveUp->setEasingCurve(QEasingCurve::InOutCubic);

    animBreatheFadeIn = new QPropertyAnimation(opacityBreathe, "opacity");
    animBreatheFadeIn->setDuration(250);
    animBreatheFadeIn->setStartValue(0);
    animBreatheFadeIn->setEndValue(1);
    animBreatheFadeIn->setEasingCurve(QEasingCurve::InBack);

    animInhale = new QParallelAnimationGroup;
    animInhale->addAnimation(animWidgetBgGradientMoveUp);
    animInhale->addAnimation(animBreatheFadeIn);

    animWidgetBgGradientMoveDown = new QPropertyAnimation(ui->widgetBgGradient, "pos", this);
    animWidgetBgGradientMoveDown->setDuration(7000);
    animWidgetBgGradientMoveDown->setStartValue(QPoint(0, 0));
    animWidgetBgGradientMoveDown->setEndValue(QPoint(0, 600));
    animWidgetBgGradientMoveDown->setEasingCurve(QEasingCurve::InOutCubic);

    animBreatheFadeOut = new QPropertyAnimation(opacityBreathe, "opacity");
    animBreatheFadeOut->setDuration(500);
    animBreatheFadeOut->setStartValue(1);
    animBreatheFadeOut->setEndValue(0);
    animBreatheFadeOut->setEasingCurve(QEasingCurve::OutBack);

    animExhale = new QParallelAnimationGroup;
    animExhale->addAnimation(animWidgetBgGradientMoveDown);
    animExhale->addAnimation(animBreatheFadeOut);
    QObject::connect(animExhale, &QAbstractAnimation::stateChanged, this, [=](QAbstractAnimation::State newState/*, QAbstractAnimation::State oldState*/) {
        if (newState == QAbstractAnimation::Running) {
            ui->labelMsgItsTime->hide();
            opacityAttempts->setOpacity(1);
        }
        // Enable buttons when breathe exercise ends
        else if (newState == QAbstractAnimation::Stopped) {
            ui->buttonCancel->setEnabled(true);
            ui->buttonContinue->setEnabled(true);
        }
    });

    animBreathe = new QSequentialAnimationGroup;
    animBreathe->addAnimation(animInhale);
    animBreathe->addAnimation(animExhale);
    QObject::connect(animBreathe, &QAbstractAnimation::stateChanged, this, [=](QAbstractAnimation::State newState/*, QAbstractAnimation::State oldState*/) {
        if (newState == QAbstractAnimation::Running) {
            ui->labelMsgItsTime->show();
            opacityAttempts->setOpacity(0);
            opacityBg->setOpacity(1);
            ui->labelMsgReturn->hide();
        }
    });

    animReturnFadeIn = new QPropertyAnimation(opacityReturn, "opacity");
    animReturnFadeIn->setDuration(250);
    animReturnFadeIn->setStartValue(0);
    animReturnFadeIn->setEndValue(0.667);
    animReturnFadeIn->setEasingCurve(QEasingCurve::InBack);

    animAttemptsFadeOut = new QPropertyAnimation(opacityAttempts, "opacity");
    animAttemptsFadeOut->setDuration(250);
    animAttemptsFadeOut->setStartValue(0.667);
    animAttemptsFadeOut->setEndValue(0);
    animAttemptsFadeOut->setEasingCurve(QEasingCurve::OutBack);

    animReturn = new QParallelAnimationGroup;
    animReturn->addAnimation(animReturnFadeIn);
    animReturn->addAnimation(animAttemptsFadeOut);

    // Stop animation on lost focus,
    // Restart animation on focus
    QObject::connect(qApp, &QGuiApplication::applicationStateChanged, this, [=](Qt::ApplicationState state) {
        switch (state) {
            case Qt::ApplicationActive: {
                animBreathe->start();
                // Disable buttons at the start of the breathe exercise, until it ends
                ui->buttonCancel->setEnabled(false);
                ui->buttonContinue->setEnabled(false);
                break;
            }
            default: {
                // All other states are some form of losing focus
                animBreathe->stop();
                animBreatheFadeOut->start();
                animBgFadeOut->start();
                ui->labelMsgReturn->show();
                animReturn->start();
            }
        }
    });

    animBreathe->start();
}

MainWindow::~MainWindow()
{
    delete opacityBreathe;
    delete opacityReturn;
    delete opacityAttempts;
    delete opacityBg;
    delete animBgFadeOut;
    delete animWidgetBgGradientMoveUp;
    delete animBreatheFadeIn;
    delete animInhale;
    delete animWidgetBgGradientMoveDown;
    delete animBreatheFadeOut;
    delete animExhale;
    delete animBreathe;
    delete animReturnFadeIn;
    delete animAttemptsFadeOut;
    delete animReturn;
    delete ui;
}
