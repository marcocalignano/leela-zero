#ifndef GUIRECEIVER_H
#define GUIRECEIVER_H

#include "Window.h"

class GuiReceiver : public Widget
{
public:
    GuiReceiver();
    ~GuiReceiver() = default;
public slots:
    void getGuiString(const QString &s);
};

#endif // GUIRECEIVER_H
