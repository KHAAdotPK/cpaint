#include <windows.h>

class MouseTrackEvents
{
    // private: by default
    // class properties (also known as data members or instance variables)
    bool m_bMouseTracking;

    // This isn't a default constructor. If t would've not defined here then compiler would've defined then it would've been called the default constructor
    // The default constructor will default-initialize the class properties or class data members  
    public: MouseTrackEvents() : m_bMouseTracking(false)
    {

    }

    // private by default
    // Class methods (also known as member functions)
    public: 
        void OnMouseMove(HWND);
        void Reset(HWND);
};