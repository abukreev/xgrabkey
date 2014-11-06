#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;

int main()
{
    Display*    dpy     = XOpenDisplay(0);
    Window      root    = DefaultRootWindow(dpy);
    XEvent      ev;

//    unsigned int    modifiers       = ControlMask | ShiftMask;
    unsigned int    modifiers       = AnyModifier;
//    int             keycode         = XKeysymToKeycode(dpy,XK_Y);
    int             keycode         = AnyKey;
    Window          grab_window     = root;
    Bool            owner_events    = False;
    int             pointer_mode    = GrabModeAsync;
    int             keyboard_mode   = GrabModeAsync;
    int             counter         = 1;


    XGrabKey(dpy, keycode, modifiers, grab_window, owner_events, pointer_mode,
             keyboard_mode);

    XSelectInput(dpy, root, KeyPressMask);
    while(true)
    {
        cout << "counter: " << counter++ << endl;
        XPeekEvent(dpy, &ev);
        switch(ev.type)
        {
            case KeyPress:
		cout << "KeyPress: keycode = " << ev.xkey.keycode << " state = " << ev.xkey.state << endl;
            default:
                break;
        }
    }

    XUngrabKey(dpy,keycode,modifiers,grab_window);
    XCloseDisplay(dpy);
    return 0;
}
