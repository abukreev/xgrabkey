#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <ctype.h>

#include <stack>
#include <list>

Display* display;
Window rootWindow;
Window focusWindow;

XKeyEvent createKeyEvent(Display *display, Window &win,
                         Window &winRoot, bool press,
                         int keycode, int modifiers) {
   XKeyEvent event;

   event.display     = display;
   event.window      = win;
   event.root        = winRoot;
   event.subwindow   = None;
   event.time        = CurrentTime;
   event.x           = 1;
   event.y           = 1;
   event.x_root      = 1;
   event.y_root      = 1;
   event.same_screen = True;
   event.keycode     = XKeysymToKeycode(display, keycode);
   event.state       = modifiers;

   if(press)
      event.type = KeyPress;
   else
      event.type = KeyRelease;

   return event;
}

std::list<XKeyEvent> createKeyClickEvents(Display *display, Window &win,
                                          Window &winRoot, int keycode) {

    std::list<XKeyEvent> events;
    events.push_back(createKeyEvent(display, win, winRoot, false, keycode, 0));
    events.push_back(createKeyEvent(display, win, winRoot, true,  keycode, 0));
    return events;
}

std::stack<int> pressed_keys;

char replacement[] = { XK_A, XK_B, XK_C, XK_D, XK_E, XK_F, XK_G, XK_H, XK_I, XK_J, XK_K, XK_L, XK_M, XK_N, XK_O, XK_P, XK_Q, XK_R, XK_S, XK_T, XK_U, XK_V, XK_W, XK_X,  XK_Y, XK_Z };

void key_pressed(int keycode) {

    if (XK_Break == keycode) {
        std::list<XKeyEvent> events;

        while (!pressed_keys.empty()) {
            int keycode = pressed_keys.top();
            pressed_keys.pop();
            if (XK_a <= keycode && keycode <= XK_Z) {
                int new_code = replacement[keycode - XK_A];
                std::list<XKeyEvent> pressReleaseEvents = createKeyClickEvents(display, rootWindow, focusWindow, new_code);
                events.insert(events.end(), pressReleaseEvents.begin(), pressReleaseEvents.end());
            }
        }

        if (!events.empty()) {
        }

        pressed_keys = std::stack<int>();
    } else {
        pressed_keys.push(keycode); 
    }
}

int main ()
{
    display = XOpenDisplay(NULL);
    rootWindow = DefaultRootWindow(display);

    char buf[17];
    KeySym ks;
    XComposeStatus comp;
    int len;
    int revert;
    XTextProperty text_prop_return;

    XGetInputFocus (display, &focusWindow, &revert);
    XSelectInput(display,
                 focusWindow,
                 KeyPressMask | KeyReleaseMask | FocusChangeMask);

    while (1) {
        XEvent ev;
        XNextEvent(display, &ev);
        switch (ev.type) {
        case FocusOut:
            printf ("Focus changed!\n");
            XGetWMName(display, focusWindow, &text_prop_return);
            printf ("Old focus is %d (\"%s\")\n",
                    (int)focusWindow,
                    text_prop_return.value);
            if (focusWindow != rootWindow) {
                XSelectInput(display, focusWindow, 0);
            }
            XGetInputFocus (display, &focusWindow, &revert);
            XGetWMName(display, focusWindow, &text_prop_return);
            printf ("New focus is %d (\"%s\")\n",
                    (int)focusWindow,
                    text_prop_return.value);
            if (focusWindow == PointerRoot) {
                focusWindow = rootWindow;
            }
            XSelectInput(display,
                         focusWindow,
                         KeyPressMask|KeyReleaseMask|FocusChangeMask);
            break;

        case KeyRelease:
            printf ("Got key!\n");
            len = XLookupString(&ev.xkey, buf, 16, &ks, &comp);
            if (len > 0 && isprint(buf[0])) {
                buf[len]=0;
                printf("String is: %s\n", buf);
                for (int i = 0; i < len; ++i) {
                    key_pressed(buf[i]);
                }
            } else {
                printf ("Key is: %d\n", (int) ks);
                key_pressed(ks);
            }
            break;
        }
    }
}
