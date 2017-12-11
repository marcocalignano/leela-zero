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

constexpr int AUTOGTP_VERSION = 9;
Q_DECLARE_METATYPE(QTextBlock)
Q_DECLARE_METATYPE(QTextCursor)

int main(int argc, char *argv[]) {
    qRegisterMetaType<QTextBlock>();
    qRegisterMetaType<QTextCursor>();
    QApplication app(argc, argv);
    app.setApplicationName("autogtp");
    app.setApplicationVersion(QString("v%1").arg(AUTOGTP_VERSION));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption gamesNumOption(
        {"g", "gamesNum"},
              "Play 'gamesNum' games on one GPU at the same time.",
              "num", "1");
    QCommandLineOption gpusOption(
        {"u", "gpus"},
              "Index of the GPU to use for multiple GPUs support.",
              "num");
    QCommandLineOption keepSgfOption(
        {"k", "keepSgf" },
              "Save SGF files after each self-play game.",
              "output directory");
    QCommandLineOption keepDebugOption(
        { "d", "debug" }, "Save training and extra debug files after each self-play game.",
                          "output directory");

    parser.addOption(gamesNumOption);
    parser.addOption(gpusOption);
    parser.addOption(keepSgfOption);
    parser.addOption(keepDebugOption);

    // Process the actual command line arguments given by the user
    parser.process(app);
    int gamesNum = parser.value(gamesNumOption).toInt();
    QStringList gpusList = parser.values(gpusOption);
    int gpusNum = gpusList.count();
    if (gpusNum == 0) {
        gpusNum = 1;
    }

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
    QMutex mutex;
    Management boss(gpusNum, gamesNum, gpusList, AUTOGTP_VERSION, parser.value(keepSgfOption), parser.value(keepDebugOption), &mutex);
    QMainWindow main;
    main.setCentralWidget(&boss);
    main.show();
    boss.giveAssignments();
    cerr.flush();
    cout.flush();
    int res = app.exec();
    mutex.lock(); // block from return without GUI
    mutex.unlock();
    return res;
}
