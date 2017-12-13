#include "GuiReceiver.h"

GuiReceiver::GuiReceiver() :
Widget()
{
    Table = new QTableWidget();
    Table->setFrameStyle(QFrame::NoFrame);
    Table->setFont(QFont("MingLiU", 12));
    TextEdit= new QTextEdit();
    TextEdit->setFontPointSize(10);
}

void GuiReceiver::getGuiString(const QString &s) {
    QStringList l = s.split('(');
    int color = Color::BLACK;
    if((l[0].toInt() % 2) == 0) {
        color = Color::WHITE;
    }         
    int x = l[1].at(0).unicode() - QChar('A').unicode();
    l[1].remove(0,1);
    l[1].remove(l[1].count()-2, 2);
    Child.Play(x, l[1].toInt(), color);
    repaint();
}
