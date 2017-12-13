#include "Board.h"

Board::Board()
{
    BOARD_KOMI = "7.5";
    Komi = 7.5;
}

void Board::Reset(int x, int y)
{
    Size = Width = x;
    Height = y;

    DATA_WIDTH = Width + 1;
    DATA_HEIGHT = Height + 2;

    DATA_START = Width + 2;
    DATA_END = (Width + 1) * (Height + 1) - 1;

    Side[0] = 1; Side[1] = DATA_WIDTH;
    Side[2] = -1; Side[3] = -DATA_WIDTH;

    Side[4] = DATA_WIDTH + 1; Side[5] = DATA_WIDTH - 1;
    Side[6] = -DATA_WIDTH + 1; Side[7] = -DATA_WIDTH - 1;

    int a = 3;
    int b = Size - 4;
    int c = (Size - 1) / 2;

    if (Size < 13) {
        a = 2;
        b = Size - 3;
    }

    Star[0] = b; Star[1] = a;
    Star[2] = a; Star[3] = b;
    Star[4] = a; Star[5] = a;
    Star[6] = b; Star[7] = b;
    Star[8] = c; Star[9] = c;
    Star[10] = a; Star[11] = c;
    Star[12] = b; Star[13] = c;
    Star[14] = c; Star[15] = a;
    Star[16] = c; Star[17] = b;

    Clear();
}

void Board::Clear()
{
    State.Table[DATA_WIDTH * DATA_HEIGHT] = OUT;
    State.Ko = 0, State.Pass = 0, State.Turn = BLACK;
    Record.Index = 0, Record2.Index = -1;

    ClearMark();

    for (int i = 1, k = 0; i <= DATA_HEIGHT; i++) {
        for (int j = 1; j <= DATA_WIDTH; j++, k++) {
            if (i == 1 || i == DATA_HEIGHT || j == 1) State.Table[k] = OUT;
            else State.Table[k] = EMPTY;
        }
    }

    std::vector<GoNode>().swap(Record.Node);

    GoNode node;
    node.Turn = BLACK;
    node.Pass = 0;
    Record.Node.push_back(node);
}

void Board::ClearMark()
{
    memset(Mark, 0 , sizeof(Mark));
    Path = 0;
}

int Board::GetPoint(int x, int y)
{
    return (y + 1) * DATA_WIDTH + x + 1;
}

int Board::GetColor(int x, int y)
{
    return State.Table[GetPoint(x, y)];
}

int Board::CheckBound(int x, int y)
{
    return (x >= 0 && x < Width && y >= 0 && y < Height);
}

// Play Board at z for color with bound, ko and empty check
int Board::Play(int x, int y, int color, int mode)
{
    int z = 0;

    if (CheckBound(x, y)) {
        z = GetPoint(x, y);
        if (State.Table[z] != EMPTY || z == State.Ko) return 0;
    }

    if (mode && Record2.Index < 0)
        Record2 = Record; // backup for test play

    Cut();

    GoNode node;
    int other = OtherColor(color);
    State.Ko = 0;

    if (z == 0) {
        State.Pass++;
        node.Prop.push_back(MakeProp(TOKEN_PASS, color, 0, 0));
    }
    else {
        State.Pass = 0;
        State.Table[z] = color;
        GoProp Prop = MakeProp(TOKEN_PLAY, color, x, y, Record.Index + 1);
        node.Prop.push_back(Prop);
        Status[z].Move = Prop.Move;

        int r[4] = {0, 0, 0, 0};
        int size, Ko = 0, cp = 0;

        for (int i = 0; i < 4; i++) // round check
            r[State.Table[z + Side[i]]] = 1;

        for (int i = 0; i < 4; i++) {
            int k = z + Side[i];
            if (State.Table[k] == other) {
                size = Remove(k, other, node);
                if (size == 1) Ko = k;
                cp = cp + size;
            }
        }

        if (cp == 1 && r[color] == 0 && r[0] == 0)
            State.Ko = Ko;
        if (cp == 0)
            Remove(z, color, node);
    }

    State.Turn = other;
    node.Turn = State.Turn;
    node.Pass = State.Pass;
    node.Ko = State.Ko;
    Record.Node.push_back(node);
    Record.Index++;
    return 1;
}

int Board::Remove(int z, int color, GoNode &node)
{
    Path++;
    Mark[z] = Path;

    SLIST_CLEAR(Block);
    SLIST_PUSH(Block, z); // point to check

    for (int n = 1; n <= Block[0]; n++) {
        int k = Block[n];
        for (int i = 0; i < 4; i++) {
            int k2 = k + Side[i];
            int p = State.Table[k2];
            if (p == EMPTY) return 0; // liberty
            if (p == color && Mark[k2] != Path) {
                Mark[k2] = Path;
                SLIST_PUSH(Block, k2);
            }
        }
    }

    for (int n = 1; n <= Block[0]; n++) {
        int k = Block[n];
        State.Table[k] = EMPTY;
        int y = k / DATA_WIDTH;
        int x = k - y * DATA_WIDTH;
        node.Prop.push_back(MakeProp(TOKEN_TAKE, color, x - 1, y - 1, Status[z].Move));
    }

    return Block[0];
}

void Board::Cut()
{
    int n = Record.Node.size() - Record.Index - 1;
    while (n--) Record.Node.pop_back();
}

Board::GoProp Board::MakeProp(int label, int value, int x, int y, int index)
{
    GoProp Prop;

    Prop.Label = label;
    Prop.Value = value;
    Prop.Move = index;
    Prop.Col = x;
    Prop.Row = y;

    return Prop;
}

int Board::GetProp(QString &str, int i, int j)
{
    QString str2 = str.mid(i, j - i);
    str2 = str2.toUpper();

    if (str2 == "B")
        return TOKEN_BLACK;
    if (str2 == "W")
        return TOKEN_WHITE;
    if (str2 == "C" || str2 == "TC")
        return TOKEN_COMMENT;
    if (str2 == "SZ")
        return TOKEN_SIZE;
    if (str2 == "PB")
        return TOKEN_PLAY_BLACK;
    if (str2 == "PW")
        return TOKEN_PLAY_WHITE;
    if (str2 == "AB")
        return TOKEN_ADD_BLACK;
    if (str2 == "AW")
        return TOKEN_ADD_WHITE;
    if (str2 == "EV" || str2 == "TE")
        return TOKEN_EVENT;
    if (str2 == "LB")
        return TOKEN_LABEL;
    if (str2 == "MA")
        return TOKEN_MARK;
    if (str2 == "TR")
        return TOKEN_TRIANGLE;
    if (str2 == "CR")
        return TOKEN_CIRCLE;
    if (str2 == "SQ")
        return TOKEN_SQUARE;
    if (str2 == "DT" || str2 == "RD")
        return TOKEN_DATE;
    if (str2 == "KM" || str2 == "KO")
        return TOKEN_KOMI;
    if (str2 == "RE")
        return TOKEN_RESULT;
    if (str2 == "HA")
        return TOKEN_HANDICAP;

    return TOKEN_NONE;
}

BoardWidget::BoardWidget(QWidget *parent) : QWidget(parent)
{
    TextEdit = NULL;
    Table = NULL;
    Child.Reset();
    Child.BOARD_DATE = QDate::currentDate().toString("yyyy-MM-dd");
    Mode = BOARD_FILE;
    View = VIEW_LABEL;
    Side = 2; // play black & white
    Cursor[0] = QPoint(-1, -1);
    Cursor[1] = QPoint(-1, -1);
}

void BoardWidget::ShowTable(int init)
{
    if (Table) {
        int n = Child.BOARD_HANDICAP.isEmpty() ? 6 : 7;

        if (init) {
            Table->clear();
            Table->setColumnCount(2);
            Table->setRowCount(n);
            Table->horizontalHeader()->setStretchLastSection(true);
            Table->horizontalHeader()->hide();
            Table->verticalHeader()->hide();

            Table->setItem(0, 0, new QTableWidgetItem("Event"));
            Table->setItem(1, 0, new QTableWidgetItem("Date"));
            Table->setItem(2, 0, new QTableWidgetItem("Black"));
            Table->setItem(3, 0, new QTableWidgetItem("White"));
            Table->setItem(4, 0, new QTableWidgetItem("Komi"));

            if (n == 6)
                Table->setItem(5, 0, new QTableWidgetItem("Result"));
            else {
                Table->setItem(5, 0, new QTableWidgetItem("Handicap"));
                Table->setItem(6, 0, new QTableWidgetItem("Result"));
            }

            Table->setItem(0, 1, new QTableWidgetItem(Child.BOARD_EVENT));
            Table->setItem(1, 1, new QTableWidgetItem(Child.BOARD_DATE));
            Table->setItem(2, 1, new QTableWidgetItem(Child.PLAYER_BLACK));
            Table->setItem(3, 1, new QTableWidgetItem(Child.PLAYER_WHITE));
            Table->setItem(4, 1, new QTableWidgetItem(Child.BOARD_KOMI));

            if (n == 6)
                Table->setItem(5, 1, new QTableWidgetItem(Child.BOARD_RESULT));
            else {
                Table->setItem(5, 1, new QTableWidgetItem(Child.BOARD_HANDICAP));
                Table->setItem(6, 1, new QTableWidgetItem(Child.BOARD_RESULT));
            }
        }
        if (Child.Record.Index > 0) {
            if (Table->rowCount() == n) {
                Table->insertRow(n);
                Table->setItem(n, 0, new QTableWidgetItem("Move"));
            }
            Table->setItem(n, 1, new QTableWidgetItem(tr("%1").arg(Child.Record.Index)));
        }
        else {
            if (Table->rowCount() > n) {
                Table->removeRow(n);
            }
        }
    }

    if (TextEdit) {
        TextEdit->setPlainText(Child.Record.Node[Child.Record.Index].Text);
    }

    View &= ~VIEW_SCORE;
    repaint();
}

void BoardWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    GridSize = std::min(size().width(), size().height()) / (Child.Size + 2.5);
    int theme = 0;

    QColor ColorBack(216, 172, 76);
    painter.setBackground(QBrush(ColorBack));
    if (theme) painter.drawTiledPixmap(0, 0, size().width(), size().height(), QPixmap(":/RESOURCE/Wood.png"));
    else painter.fillRect(0, 0, size().width(), size().height(), ColorBack);

    int Width = (Child.Size - 1) * GridSize;
    int Grid = GridSize * 2;
    double Font = GridSize / 2.8;
    double Radius = GridSize * 0.4823 - 0.5869;

    BOARD_LEFT = (size().width() - Width) / 2;
    BOARD_TOP = (size().height() - Width) / 2;
    BOARD_RIGHT = BOARD_LEFT + Width;
    BOARD_DOWN = BOARD_TOP + Width;

    if (GridSize > 6) {
        painter.setPen(QPen(QColor(48, 48, 48), 0, Qt::SolidLine, Qt::FlatCap));

        for (int i = 0, d = 0; i < Child.Size; i++, d += GridSize) {
            painter.drawLine(BOARD_LEFT + d, BOARD_TOP, BOARD_LEFT + d, BOARD_DOWN);
            painter.drawLine(BOARD_LEFT, BOARD_TOP + d, BOARD_RIGHT + 1, BOARD_TOP + d);
        }

        painter.setRenderHint(QPainter::Antialiasing);
//      painter.setRenderHint(QPainter::HighQualityAntialiasing);
//      painter.setBackgroundMode(Qt::TransparentMode);

        if (Font >= 6) {
            painter.setPen(Qt::black);
            painter.setFont(QFont("Arial", Font));

            // Grid Coord //
            if (View & VIEW_LABEL) {
                int k1 = BOARD_TOP - GridSize * 1.05;
                int k2 = BOARD_DOWN + GridSize * 1.05;

                for (int i = 0, x = BOARD_LEFT; i < Child.Size; i++, x += GridSize) {
                    QChar Value = QChar('A' + i + (i < 8 ? 0 : 1));
                    if (!(View & VIEW_SCORE)) painter.drawText(x - GridSize + 1, k1 - GridSize + 1, Grid, Grid, Qt::AlignCenter, Value);
                    painter.drawText(x - GridSize + 1, k2 - GridSize + 1, Grid, Grid, Qt::AlignCenter, Value);
                }

                k1 = BOARD_LEFT - GridSize * 1.18;
                k2 = BOARD_RIGHT + GridSize * 1.17;

                for (int i = 0, y = BOARD_TOP; i < Child.Size; i++, y += GridSize) {
                    QString Value = QString::number(Child.Size - i);
                    painter.drawText(k1 - GridSize - 0, y - GridSize + 1, Grid, Grid, Qt::AlignCenter, Value);
                    painter.drawText(k2 - GridSize - 0, y - GridSize + 1, Grid, Grid, Qt::AlignCenter, Value);
                }
            }
            if (View & VIEW_SCORE) {
                painter.setFont(QFont("Arial", Font + 1));
                painter.drawText(0, BOARD_TOP - GridSize * 1.2 - GridSize + 1, size().width(), Grid, Qt::AlignCenter, Child.BOARD_SCORE);
            }
        }

        // Star //
        if (Child.Size >= 13) {
            painter.setPen(QPen(QColor(15, 15, 15), 1.2, Qt::SolidLine));
            painter.setBrush(Qt::black);

            for (int i = 0, k = 0; i < 9; i++, k += 2) {
                if ((Child.Size & 1) == 0 && i >= 4) break;
                painter.drawEllipse(BOARD_LEFT + Child.Star[k] * GridSize - 2, BOARD_TOP + Child.Star[k + 1] * GridSize - 2, 5, 5);
            }
        }

        if (Radius >= 0) {
            int size = GridSize / 4.8;
            QPolygonF Mark, Triangle;
            Mark << QPoint(-1 * size, -1 * size) << QPoint(1 * size, -1 * size) << QPoint(-1 * size, 1 * size) << QPoint(1 * size, 1 * size);
            double dsize = GridSize / 4.2;
            Triangle << QPoint(0, -1 * dsize) << QPoint(0.866 * dsize, 0.5 * dsize) << QPoint(-0.866 * dsize, 0.5 * dsize);

            for (int j = 0, y = BOARD_TOP; j < Child.Size; j++, y += GridSize) {
                for (int i = 0, x = BOARD_LEFT; i < Child.Size; i++, x += GridSize) {
                    int k = Child.GetPoint(i, j);
                    int color = Child.State.Table[k];
                    int label = Child.Status[k].Label;

                    // Area //
                    if ((View & VIEW_SCORE) && label != BOTH_AREA) {
                        painter.setPen(Qt::NoPen);
                        if (label == BLACK_AREA || label == WHITE_DEAD) {
                            if (theme) painter.setBrush(QBrush(Child.Status[k].Color));
                            else painter.setBrush(QBrush(QColor(0, 0, 0), Qt::BDiagPattern));
                        }
                        else {
                            if (theme) painter.setBrush(QBrush(Child.Status[k].Color));
                            else painter.setBrush(QBrush(QColor(255, 255, 255), Qt::Dense5Pattern));
                        }
                        int half = int (Radius);
                        int size = half * 2 + 1;
                        if (theme) painter.drawRect(x - Radius, y - Radius, GridSize, GridSize);
                        else painter.drawRect(x - half, y - half, size, size);

                    }

                    if (color == BLACK || color == WHITE) {
                        int half = GridSize / 2;
                        int size = half * 2 + 1;

                        if (color == BLACK) {
                            painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
                            painter.setBrush(QColor(0, 0, 0));
                            painter.drawEllipse(QPointF(x + 0.5, y + 0.5), Radius, Radius);
                        }
                        else {
                            painter.setPen(QPen(QColor(36, 36, 36), 1.01, Qt::SolidLine));
                            painter.setBrush(QColor(240, 240, 240));
                            painter.drawEllipse(QPointF(x + 0.5, y + 0.5), Radius, Radius);
                        }
                    }

                    // Mark //
                    if ((View & VIEW_SCORE) && label != BOTH_AREA) {
                        if (label == BLACK_DEAD || label == WHITE_DEAD) {
                            QPolygonF Poly = Mark.translated(x + 0.55, y + 1);
                            painter.setPen(QPen(color == BLACK ? Qt::white : Qt::black, 1.1, Qt::SolidLine));
                            painter.drawLine(Poly[0], Poly[3]);
                            painter.drawLine(Poly[1], Poly[2]);
                        }
                    }
                }
            }

            // Label //
            int total = Child.Record.Node[Child.Record.Index].Prop.size();
            for (int i = 0; i < total; i++) {
                Board::GoProp Prop = Child.Record.Node[Child.Record.Index].Prop[i];
                int color = Child.GetColor(Prop.Col, Prop.Row);
                int x = BOARD_LEFT + Prop.Col * GridSize;
                int y = BOARD_TOP + Prop.Row * GridSize;

                if (Prop.Label == Board::TOKEN_PLAY || Prop.Label == Board::TOKEN_CIRCLE) {
                    QColor color2 = (color == BLACK ? QColor(240, 240, 240) : Qt::black);
                    int Half = double (GridSize) / 12;
                    painter.setPen(QPen(color2, 1.04, Qt::SolidLine));
                    painter.setBrush(color2);
                    painter.drawEllipse(QPointF(x + 0.5, y + 0.5), Half, Half);
                }
                else if (Prop.Label == Board::TOKEN_TRIANGLE) {
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(QBrush(color == BLACK ? Qt::white : Qt::black));
                    painter.drawPolygon(Triangle.translated(x + 0.65, y + 1.4));
                }
                else if (Prop.Label == Board::TOKEN_MARK) {
                    QPolygonF Poly = Mark.translated(x + 0.55, y + 1);
                    painter.setPen(QPen(color == BLACK ? Qt::white : Qt::black, 1.2, Qt::SolidLine));
                    painter.drawLine(Poly[0], Poly[3]);
                    painter.drawLine(Poly[1], Poly[2]);
                }
                else if (Prop.Label == Board::TOKEN_SQUARE) {
                    int Half = GridSize / 6;
                    int size = Half * 2;
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen(color == BLACK ? QColor(240, 240, 240) : Qt::black, 1, Qt::SolidLine));
                    painter.setRenderHint(QPainter::Antialiasing, false);
                    painter.drawRect(x - Half , y - Half, size, size);
                    painter.setRenderHint(QPainter::Antialiasing);
                }
                else if (Prop.Label == Board::TOKEN_LABEL || Prop.Label == Board::TOKEN_NUMBER) {
                    if (Font >= 6) {
                        if (color == EMPTY) painter.setBackgroundMode(Qt::OpaqueMode);
                        painter.setPen(QPen(color == BLACK ? Qt::white : Qt::black, 1, Qt::SolidLine));
                        painter.setFont(QFont("Arial", Font));
                        painter.drawText(x - GridSize + 1, y - GridSize + 1, Grid, Grid, Qt::AlignCenter,
                            Prop.Label == Board::TOKEN_LABEL ? QChar(Prop.Value) : QString::number(Prop.Value));
                        if (color == EMPTY) painter.setBackgroundMode(Qt::TransparentMode);
                    }
                }
            }

            // Cursor //
            if (Cursor[0].x() >=0 && Cursor[0].y() >= 0) {
                QColor color = Child.Record2.Index < 0 ? QColor(0, 224, 0) : Qt::red;
                painter.setPen(QPen(color, 2, Qt::SolidLine));
                painter.setBrush(color);
                painter.drawEllipse(BOARD_LEFT + Cursor[0].x() * GridSize - 2, BOARD_TOP + Cursor[0].y() * GridSize - 2, 5, 5);
            }
        }
    }

}
