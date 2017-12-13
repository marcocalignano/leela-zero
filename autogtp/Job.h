/*
    This file is part of Leela Zero.
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

#ifndef JOB_H
#define JOB_H

#include "Result.h"
#include <QObject>
#include <QAtomicInt>
#include <QtWidgets/QPlainTextEdit>
using VersionTuple = std::tuple<int, int>;

class Job : public QObject {
    Q_OBJECT
public:
    enum {
        RUNNING = 0,
        FINISHING
    };
    enum {
        Production = 0,
        Validation
    };
    Job(QString gpu);
    ~Job() = default;
    virtual Result execute() = 0;
    virtual void init(const QMap<QString,QString> &l);
    void finish() { m_state.store(FINISHING); }

signals:
    void sendGuiText(const QString &text);

protected:
    QAtomicInt m_state;
    QString m_option;
    QString m_gpu;
    VersionTuple m_leelazMinVersion;
};


class ProdutionJob : public Job {
    Q_OBJECT
public:
    ProdutionJob(QString gpu);
    ~ProdutionJob() = default;
    void init(const QMap<QString,QString> &l);
    Result execute();
private:
    QString m_network;
};

class ValidationJob : public Job {
    Q_OBJECT
public:
    ValidationJob(QString gpu);
    ~ValidationJob() = default;
    void init(const QMap<QString,QString> &l);
    Result execute();
private:
    QString m_firstNet;
    QString m_secondNet;
};

#endif // JOB_H
