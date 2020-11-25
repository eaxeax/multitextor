#include "logger.h"
#include "WndManager.h"

#include <codecvt>


static const pos_t SPLIT_WIDTH { 1 };


bool WndManager::Init()
{
    bool rc = m_console.Init();
    if (!rc)
        return false;

    m_console.GetScreenSize(m_sizex, m_sizey);

    LOG(INFO) << __func__ << " x=" << m_sizex << " y=" << m_sizey;
    CalcView();

    m_textBuff.SetSize(m_sizex, m_sizey);

    return true;
}


bool WndManager::Deinit()
{
    LOG(INFO) << __func__;

    if (m_view[2].wnd)
        m_view[2].wnd.reset();

    m_wndList.clear();

    bool rc = true;
    if (0 != m_textBuff.GetSize())
    {
        rc = m_console.SetTextAttr(DEFAULT_COLOR)
          && m_console.ClrScr();

        m_textBuff.SetSize();
    }

    m_console.Deinit();
    return rc;
}

bool WndManager::CalcView()
{
    LOG(DEBUG) << "CalcView t=" << static_cast<int>(m_splitType);

    //for dialogs
    m_view[0].left  = 0;
    m_view[0].top   = m_topLines;
    m_view[0].sizex = m_sizex;
    m_view[0].sizey = m_sizey - m_topLines - m_bottomLines;

    //for work windows
    if (m_splitType == split_t::split_h)
    {
        //horizontal
        m_view[1].left  = 0;
        m_view[1].top   = m_topLines;
        m_view[1].sizex = m_sizex;
        m_view[1].sizey = m_splitY;

        m_view[2].left  = 0;
        m_view[2].top   = m_topLines + m_splitY + 1;
        m_view[2].sizex = m_sizex;
        m_view[2].sizey = m_sizey - m_topLines - m_bottomLines - m_splitY - 1;
    }
    else if (m_splitType == split_t::split_v)
    {
        //vertical
        m_view[1].left  = 0;
        m_view[1].top   = m_topLines;
        m_view[1].sizex = m_splitX;
        m_view[1].sizey = m_sizey - m_topLines - m_bottomLines;

        m_view[2].left  = m_splitX + SPLIT_WIDTH;
        m_view[2].top   = m_topLines;
        m_view[2].sizex = m_sizex - m_splitX - SPLIT_WIDTH;
        m_view[2].sizey = m_sizey - m_topLines - m_bottomLines;
    }

    return true;
}

bool WndManager::Refresh()
{
    LOG(DEBUG) << "  M::Refresh";

    if (!m_sizex || !m_sizey)
        return false;

    m_disablePaint = 1;
    m_textBuff.Fill(0);

    bool rc;
    if (!m_pLogo)
    {
        rc = SetTextAttr(ColorScreen)
          && Cls();
    }
    else
    {
        //with logo
        rc = FillRect(0, 0, m_sizex, m_sizey, m_pLogo->fillChar, m_pLogo->fillColor);

        pos_t x = m_pLogo->x;
        pos_t y = m_pLogo->y;

        if (x < 0)
            x = (m_sizex - (pos_t)m_pLogo->logoStr.front().size()) / 2;
        if (y < 0)
            y = (m_sizey - (pos_t)m_pLogo->logoStr.size()) / 2;

        SetTextAttr(m_pLogo->logoColor);

        for(const auto& str : m_pLogo->logoStr)
        {
            GotoXY(x, y++);
            WriteStr(str);
        }
    }

//???    rc = g_pApplication->Repaint();
    
    if (!m_wndList.empty())
    {
        if (m_wndList.size() > 1 && m_wndList[0]->GetWndType() == "DLG")
            m_wndList[1]->Refresh();

        if (m_view[2].wnd)
        {
            if (m_splitType == split_t::split_h)
                FillRect(m_view[2].left, m_view[2].top - 1, m_view[2].sizex, 1,
                    ' '/*ACS_HLINE*/, ColorViewSplitter);
            else if (m_splitType == split_t::split_v)
                FillRect(m_view[2].left - SPLIT_WIDTH, m_view[2].top, SPLIT_WIDTH, m_view[2].sizey,
                    ' '/*ACS_VLINE*/, ColorViewSplitter);

            m_view[2].wnd->Refresh();
        }

        rc = m_wndList[0]->Refresh();
    }

    m_disablePaint = 0;
    rc = ShowBuff();
    return rc;
}

bool WndManager::Cls()
{
    LOG(DEBUG) << "  M::Cls";

    HideCursor();
    bool rc = CallConsole(ClrScr());
    return rc;
}


bool WndManager::SetTextAttr(color_t color)
{
    m_color = color;
    bool rc = CallConsole(SetTextAttr(color));
    return rc;
}

bool WndManager::GotoXY(short x, short y)
{
    m_cursorx = x;
    m_cursory = y;
    bool rc = CallConsole(GotoXY(x, y));
    return rc;
}

bool WndManager::ShowInputCursor(cursor_t cursor, pos_t x, pos_t y)
{
    if (cursor == cursor_t::CURSOR_HIDE || m_wndList.empty())
    {
        x = 0;
        y = 0;
        cursor = cursor_t::CURSOR_HIDE;
    }
    else if (x < 0 || y < 0)
    {
        if (!m_activeView)
        {
            x = m_wndList[0]->m_cursorx;
            y = m_wndList[0]->m_cursory;
            m_wndList[0]->ClientToScreen(x, y);
        }
        else
        {
            x = m_view[2].wnd->m_cursorx;
            y = m_view[2].wnd->m_cursory;
            m_view[2].wnd->ClientToScreen(x, y);
        }
    }

    bool rc = true;
    if (m_cursorx != x || m_cursory != y || m_cursor != cursor)
    {
        rc = m_console.GotoXY(x, y)
          && m_console.SetCursor(cursor)
          && m_console.Flush();

        m_cursorx = x;
        m_cursory = y;
        m_cursor  = cursor;
    }

    return rc;
}

bool WndManager::HideCursor()
{
    int rc = 0;
    if (m_cursor != cursor_t::CURSOR_HIDE)
    {
        rc = m_console.SetCursor(cursor_t::CURSOR_HIDE);
        m_cursor = cursor_t::CURSOR_HIDE;
    }
    return rc;
}

bool WndManager::FillRect(pos_t left, pos_t top, pos_t sizex, pos_t sizey, input_t c, color_t color)
{
    LOG(DEBUG) << "  M::FillRect l=" << left << " t=" << top << " x=" << sizex << " y=" << sizey << " c=" << c << " color=" <<  color;

    HideCursor();
    cell_t cl = MAKE_CELL(0, color, c);
    for (pos_t y = 0; y < sizey; ++y)
    {
        for (pos_t x = 0; x < sizex; ++x)
            m_textBuff.SetSell((size_t)left + x, (size_t)top + y, cl);
    }

    bool rc = CallConsole(WriteBlock(left, top, left + sizex - 1, top + sizey - 1, m_textBuff));//???
    return rc;
}

bool WndManager::ShowBuff()
{
    if (0 == m_textBuff.GetSize())
        return false;

    bool rc = WriteBlock(0, 0, m_sizex - 1, m_sizey - 1, m_textBuff);
    return rc;
}

bool WndManager::ShowBuff(pos_t left, pos_t top, pos_t sizex, pos_t sizey)
{
    if (0 == m_textBuff.GetSize())
        return false;

    bool rc = false;
    if (left < 0 || top < 0 || left + sizex > m_sizex || top + sizey > m_sizey)
        LOG(ERROR) << "  M::ShowBuff l=" << left << " t=" << top << " sx=" << sizex << " sy=" << sizey;
    else
        rc = WriteBlock(left, top, left + sizex - 1, top + sizey - 1, m_textBuff);
    return rc;
}

bool WndManager::WriteBlock(pos_t left, pos_t top, pos_t right, pos_t bottom, const ScreenBuffer& block)
{
    HideCursor();
    bool rc = CallConsole(WriteBlock(left, top, right, bottom, block));
    return rc;
}

bool WndManager::WriteStr(const std::string& str)
{
    HideCursor();

    //??? utf8->utf16
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    std::u16string wstr = converter.from_bytes(str);

    size_t l = wstr.size();

    bool rc;
    if (m_cursory != m_sizey - 1 || m_cursorx + (pos_t)l <= m_sizex - 1)
        rc = CallConsole(WriteStr(wstr));
    else
    {
        char16_t PrevC = wstr[l - 2];
        char16_t LastC = wstr[l - 1];
        rc = CallConsole(WriteStr(wstr.substr(0, l - 1)));
        rc = CallConsole(WriteLastChar(PrevC, LastC));
    }

    for(const auto& c : wstr)
        m_textBuff.SetSell(m_cursorx++, m_cursory, MAKE_CELL(0, m_color, c));

    return rc;
}

bool WndManager::CheckInput(const std::chrono::milliseconds& waitTime)
{
    while (1)
    {
        m_console.InputPending(waitTime);
        input_t key = m_console.GetInput();
        LOG_IF(key, INFO) << "  " << ConsoleInput::CastKeyCode(key);
        if (key == K_SPACE)
            break;
    }
    return true;
}

