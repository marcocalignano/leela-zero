#include "GuiReceiver.h"

GuiReceiver::GuiReceiver() :
QPlainTextEdit()
{
}

void GuiReceiver::getGuiString(const QString &s) {
    textCursor().movePosition(QTextCursor::End);
    insertPlainText(s);
}
