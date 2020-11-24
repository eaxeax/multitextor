#pragma once

#include "Console.h"
#include "Wnd.h"

#include <array>


enum class split_t
{
    no_split = 0,
    split_h  = 1,
    split_v  = 2
};

//////////////////////////////////////////////////////////////////////////////
struct Logo
{
    color_t     fillColor;
    color_t     logoColor;
    char16_t    fillChar;
    pos_t       x;  //left up corner
    pos_t       y;         
    std::list<std::string>  logoStr;
};

struct View
{
    pos_t   left;
    pos_t   top;
    pos_t   sisex;
    pos_t   sizey;
    std::shared_ptr<Wnd> wnd;
};

//////////////////////////////////////////////////////////////////////////////
class WndManager final
{
protected:
    std::array<View, 3> m_View{};
    std::list<Wnd*>     m_TopWnd;   //windows list sorted in Z order with them activity

    std::vector<cell_t> m_TextBuff; //current buffer color/symbol/changing
    const Logo*         m_pLogo{nullptr};

    color_t             m_Color     {};
    pos_t               m_cursorx   {};
    pos_t               m_cursory   {};
    cursor_t            m_cursor    {cursor_t::CURSOR_OFF};
    int                 m_fNotPaint {0};
    bool                m_fInvalidate {true}; //first paint
    bool                m_fInvTitle {true};

public:
    std::shared_ptr<Console>    m_Console;

    #define CallConsole(p) ((m_fNotPaint) ? 0 : m_Console-> p)

    //view management
    pos_t               m_nSplitX{};      //15 min
    pos_t               m_nSplitY{};      //3  min
    split_t             m_nSplitType{};   //0-no_view 1-horiz 2-vert
    int                 m_nActiveView{};

    pos_t               m_sizex{};        //screen size
    pos_t               m_sizey{};        //
    pos_t               m_TopLine{};      //number of line used on top
    pos_t               m_BottomLine{};   //number of line used on bottom

public:
    WndManager() = default;
    ~WndManager() = default;

    bool    Init();
    bool    Deinit();
    bool    Resize(pos_t sizex, pos_t sizey);

    bool    CheckInput(const std::chrono::milliseconds& waitTime);
    bool    PutMacro(input_t cmd);
    bool    ProcInput(input_t code); //event that not treated will pass to active window
    bool    ShowInputCursor(cursor_t nCursor, pos_t x = -1, pos_t y = -1);
    bool    HideCursor();
    bool    Beep() {return m_Console->Beep();}

    bool    Cls();
    bool    Refresh();
    bool    CheckRefresh();
    void    StopPaint()  {++m_fNotPaint;}
    void    BeginPaint() {--m_fNotPaint;}
    bool    Flush();
    void    SetLogo(const Logo* pLogo) {m_pLogo = pLogo;}
    bool    WriteConsoleTitle(bool set = true);

    bool    IsVisible(Wnd* pWnd);
    bool    AddWnd(Wnd* wnd);
    bool    AddLastWnd(Wnd* wnd);
    bool    DelWnd(Wnd* wnd);
    bool    SetTopWnd(Wnd* pWnd, int view = -1);
    bool    SetTopWnd(int pos, int view = -1);
    bool    GetWndCount();
    Wnd*    GetWnd(int pos = 0, int view = -1);

    bool    Show(Wnd* wnd, bool refresh = 1, int view = 0);
    bool    Hide(Wnd* wnd, bool refresh = 1);
    bool    Move(Wnd* wnd, bool refresh = 1);

    bool    SetView(pos_t x = 40, pos_t y = 11, split_t type = split_t::no_split);
    bool    ChangeViewMode(split_t fType = split_t::no_split);
    bool    CalcView();
    bool    CloneView(Wnd* wnd = NULL);
    bool    SetActiveView(int pos = -1);
    int     GetActiveView() {return m_nActiveView;}
    bool    TrackView(const std::string& msg);
    View*   GetView(Wnd* wnd);

    void    Invalidate() {m_fInvalidate = 1;}

    ////////////////////////////////////////////////////////////////////////////

    bool    GotoXY(pos_t x, pos_t y);
    bool    SetTextAttr(color_t color);
  
    bool    WriteStr(const std::string& str);
    bool    WriteChar(char c = ' ');
    bool    WriteWStr(const std::u16string& str);
    bool    WriteColorWStr(std::u16string& str, const std::vector<color_t>& color);
    bool    WriteWChar(char16_t c = ' ');

    bool    Scroll(pos_t left, pos_t top, pos_t right, pos_t bottom, pos_t n, scroll_t mode);
    bool    FillRect(pos_t left, pos_t top, pos_t sizex, pos_t sizey, input_t c, color_t color);
    bool    ColorRect(pos_t left, pos_t top, pos_t sizex, pos_t sizey, color_t color);
    bool    InvColorRect(pos_t left, pos_t top, pos_t sizex, pos_t sizey);
    bool    WriteColor(pos_t x, pos_t y, std::vector<color_t>& color);

    bool    ShowBuff();
    bool    ShowBuff(pos_t left, pos_t top, pos_t sizex, pos_t sizey);

    bool    GetBlock(pos_t left, pos_t top, pos_t right, pos_t bottom, std::vector<cell_t>& block);
    bool    PutBlock(pos_t left, pos_t top, pos_t right, pos_t bottom, const std::vector<cell_t>& block);

protected:
    bool    WriteBlock(pos_t left, pos_t top, pos_t right, pos_t bottom, const std::vector<cell_t>& block);
};

