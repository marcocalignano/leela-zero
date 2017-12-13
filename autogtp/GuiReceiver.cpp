#include "GuiReceiver.h"

GuiReceiver::GuiReceiver() :
BoardWidget()
{
    Table = new QTableWidget();
    Table->setFrameStyle(QFrame::NoFrame);
    Table->setFont(QFont("MingLiU", 12));
    TextEdit= new QTextEdit();
    TextEdit->setFontPointSize(10);
}

void GuiReceiver::getGuiMove(const QString &m, int moves) {
    QString tmp = m;
    int x = m.at(0).cell();
    if(x > QChar('I').cell()) {
        --x;
    }
    x -= QChar('A').cell();
    int y = 19 - tmp.remove(0,1).toInt();
    int color = ((moves % 2) == 0) ?
                Board::Color::WHITE : Board::Color::BLACK;
    Child.Play(x, y, color);
    repaint();
}
