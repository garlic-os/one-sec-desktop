#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("garlicSoft");
    QCoreApplication::setApplicationName("one sec");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "one-sec_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption progNameOption(QStringList() << "n" << "name",
                                      QCoreApplication::translate("main", "Optional name for the program."),
                                      QCoreApplication::translate("main", "name"));
    parser.addOption(progNameOption);
    parser.process(app);

    QStringList arguments = parser.positionalArguments();
    if (arguments.empty()) {
        qCritical() << parser.helpText();
        return 1;
    }

    QString program = arguments.takeFirst();
    QString progName = parser.value(progNameOption);

    MainWindow window(nullptr, program, arguments, progName);
    window.show();
    return app.exec();
}
