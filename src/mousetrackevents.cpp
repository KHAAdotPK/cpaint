#include "mousetrackevents.hh"

// OnMouseMove method definition
void MouseTrackEvents::OnMouseMove(HWND hwnd)
{
    // ... code to track mouse movement ...
    if (!m_bMouseTracking)
    {
        // Enable mouse tracking.
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = hwnd;
        tme.dwFlags = /*TME_HOVER |*/TME_LEAVE;
        //tme.dwHoverTime = /*HOVER_DEFAULT*/ DEFAULT_LEAVE;
        TrackMouseEvent(&tme);
        m_bMouseTracking = true;
    }
}

// Reset method definition
void MouseTrackEvents::Reset(HWND hwnd)
{
    // ... code to reset mouse tracking ...
    m_bMouseTracking = false;
}

