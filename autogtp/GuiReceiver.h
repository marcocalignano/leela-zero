#ifndef GUIRECEIVER_H
#define GUIRECEIVER_H

#include <QPlainTextEdit>

class GuiReceiver : public QPlainTextEdit
{
public:
    GuiReceiver();
    ~GuiReceiver() = default;
public slots:
    void getGuiString(const QString &s);
};

#endif // GUIRECEIVER_H
