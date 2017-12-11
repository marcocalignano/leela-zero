/*
    This file is part of Leela Zero.
    Copyright (C) 2017 Gian-Carlo Pascutto
    Copyright (C) 2017 Marco Calignano

    Leela Zero is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leela Zero is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Leela Zero.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCore/QTimer>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QCommandLineParser>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTextBlock>
#include <QTextCursor>
#include <chrono>
#include <QCommandLineParser>
#include <iostream>
#include "Game.h"
#include "Management.h"

constexpr int AUTOGTP_VERSION = 10;
//Q_DECLARE_METATYPE(QTextBlock)
//Q_DECLARE_METATYPE(QTextCursor)

int main(int argc, char *argv[]) {
//    qRegisterMetaType<QTextBlock>();
//    qRegisterMetaType<QTextCursor>();
    QApplication app(argc, argv);
    app.setApplicationName("autogtp");
    app.setApplicationVersion(QString("v%1").arg(AUTOGTP_VERSION));

    //QTimer::singleShot(0, &app, SLOT(quit()));

    /* TODO with graphic interface
     *
    parser.addOption(gamesNumOption);
    parser.addOption(gpusOption);
    parser.addOption(keepSgfOption);
    parser.addOption(keepDebugOption);
    if (parser.isSet(keepSgfOption)) {
        if (!QDir().mkpath(parser.value(keepSgfOption))) {
            cerr << "Couldn't create output directory for self-play SGF files!"
                 << endl;
            return EXIT_FAILURE;
        }
    }
    if (parser.isSet(keepDebugOption)) {
        if (!QDir().mkpath(parser.value(keepDebugOption))) {
            cerr << "Couldn't create output directory for self-play Debug files!"
                 << endl;
            return EXIT_FAILURE;
        }
    }
    */

    int gamesNum = 3 ;//parser.value(gamesNumOption).toInt();
    QStringList gpusList();
    int gpusNum = 1;

    // Map streams
    QTextStream cin(stdin, QIODevice::ReadOnly);
    QTextStream cout(stdout, QIODevice::WriteOnly);
#if defined(LOG_ERRORS_TO_FILE)
    // Log stderr to file
    QFile caFile("output.txt");
    caFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    if(!caFile.isOpen()){
        qDebug() << "- Error, unable to open" << "outputFilename" << "for output";
    }
    QTextStream cerr(&caFile);
#else
    QTextStream cerr(stderr, QIODevice::WriteOnly);
#endif
    cerr << "AutoGTP v" << AUTOGTP_VERSION << endl;
    cerr << "Using " << gamesNum << " thread(s)." << endl;

    QMutex mutex;
    Management boss(gpusNum, gamesNum, QStringList(), AUTOGTP_VERSION, "", "", &mutex);
    QMainWindow main;
    main.setCentralWidget(&boss);
    main.show();
    boss.giveAssignments();
    cerr.flush();
    cout.flush();
    return app.exec();
}
