#ifndef GUIRECEIVER_H
#define GUIRECEIVER_H

#include "Board.h"

class GuiReceiver : public BoardWidget
{
public:
    GuiReceiver();
    ~GuiReceiver() = default;
public slots:
    void getGuiMove(const QString &m, int moves);
};

#endif // GUIRECEIVER_H
