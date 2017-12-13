#ifndef _BOARD_H_
#define _BOARD_H_

#include <QtCore>
#include <QColor>
#include <QtWidgets>

#define MAX_SIZE 19
#define MIN_SIZE 2

#define BOARD_MAX (MAX_SIZE + 1) * (MAX_SIZE + 2) + 1
#define MOVE_MAX  MAX_SIZE * MAX_SIZE + 2
#define BOARD_MOVE_MAX MAX_SIZE * MAX_SIZE * 2

// Simple List
inline void SLIST_CLEAR(int *ls) { ls[0] = 0; }
inline void SLIST_PUSH(int *ls, int i) { ls[++ls[0]] = i; }
enum Color
{
    EMPTY, BLACK, WHITE, OUT
};

enum
{
    BOTH_AREA, BLACK_AREA, WHITE_AREA,
    BLACK_DEAD, WHITE_DEAD
};

class Board
{
public:

    enum Color
    {
        EMPTY, BLACK, WHITE, OUT
    };

    enum
    {
        BOTH_AREA, BLACK_AREA, WHITE_AREA,
        BLACK_DEAD, WHITE_DEAD
    };
    enum
    {
        PASS, RESIGN
    };

    enum
    {
        TOKEN_NONE, TOKEN_BLACK, TOKEN_WHITE, TOKEN_ADD, TOKEN_ADD_BLACK, TOKEN_ADD_WHITE, TOKEN_PLAY,
        TOKEN_TAKE, TOKEN_PASS, TOKEN_LABEL, TOKEN_NUMBER, TOKEN_MARK, TOKEN_TRIANGLE, TOKEN_CIRCLE,
        TOKEN_SQUARE, TOKEN_PLAY_BLACK, TOKEN_PLAY_WHITE, TOKEN_COMMENT, TOKEN_TIME, TOKEN_EVENT,
        TOKEN_PLACE, TOKEN_DATE, TOKEN_RESULT, TOKEN_SIZE, TOKEN_KOMI, TOKEN_HANDICAP
    };

    inline static int OtherColor(int color)
    {
        return color ^ 3;
    }

    struct State
    {
        int Table[BOARD_MAX];
        int Ko, Turn, Pass;
    };

    struct Status
    {
        QColor Color;
        int Move;
        int Label;
    };

    struct GoProp
    {
        int Label, Value;
        int Col, Row;
		int Move;
    };

    struct GoNode // Record
    {
        std::vector<GoProp> Prop;
        QString Text;
        int Ko, Turn, Pass;
    };

    struct Tree
    {
        std::vector<GoNode> Node;
        int Index;
    };

    Board();

    void Clear();
    void ClearMark();
    void Reset(int x = MAX_SIZE, int y = MAX_SIZE);
    int GetColor(int x, int y);
    int GetPoint(int x, int y);
    int CheckBound(int x, int y);
    int Remove(int z, int color, GoNode &node);
    int GetProp(QString &str, int i, int j);
    GoProp MakeProp(int label, int value, int x, int y, int index = 0);
    void Cut();
    int Play(int x, int y, int color, int mode = 0);

    int Width, Height, Size;
    int Path;

    int Mark[BOARD_MAX];
    int Move[MOVE_MAX];
    int Block[MOVE_MAX];

    int Side[8];
    int Star[18];
    int Handicap[20];

    int BLACK_SCORE;
    int WHITE_SCORE;

    double FINAL_SCORE;
    double Komi;

    int DATA_WIDTH, DATA_HEIGHT;
    int DATA_START, DATA_END;

    Status Status[BOARD_MAX];
    Tree Record, Record2;
    State State;

    QString BOARD_EVENT, BOARD_DATE;
    QString PLAYER_BLACK, PLAYER_WHITE;
    QString BLACK_LEVEL, WHITE_LEVEL;
    QString BOARD_HANDICAP, BOARD_KOMI;
    QString BOARD_RESULT, BOARD_FILE;
    QString BOARD_SCORE;

};





enum
{
    BOARD_FILE = 1,
    BOARD_PLAY
};

class BoardWidget : public QWidget
{
public:

    BoardWidget(QWidget *parent = NULL);

    void ShowTable(int init = 0);
    Board Child;
    enum
    {
        VIEW_LABEL = 1,
        VIEW_SCORE = 2,
        VIEW_MARK = 4,
        PLAY_PAUSE = 32
    };

    int Mode, View;
    int Side;

    QString Title;
    QTextEdit *TextEdit;
    QTableWidget *Table;

protected:

    void paintEvent(QPaintEvent *event);

    QPoint Cursor[2];

    int GridSize;

    int BOARD_TOP, BOARD_LEFT;
    int BOARD_DOWN, BOARD_RIGHT;
    int TABLE_TOP, TABLE_LEFT;
    int TABLE_DOWN, TABLE_RIGHT;
    int TABLE_WIDTH;

};

#endif
