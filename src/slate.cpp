#include "slate.h"

#include <iostream>
#include <thread>

#include "util/message.h"
#include "util/util.h"

std::shared_ptr<Slate> Slate::instance = std::shared_ptr<Slate>();

Slate::Slate() :
    ctx(1), toclient(ctx, ZMQ_PUSH), fromclient(ctx, ZMQ_PULL) {

    state.openWindows = topWindows();

    // Magic 1.25 till we get a DPI fix
    state.workspaces["0"] = new Workspace(unsigned(int(GetSystemMetrics(SM_CXSCREEN)*1.25)), unsigned(int(GetSystemMetrics(SM_CYSCREEN)*1.25)), "0");
    state.focused_client = root;

    toclient.bind("ipc:///tmp/slateevents");
    fromclient.bind("ipc:///tmp/slateclient");

    Message::InitClientSocket(&toclient);
}

Slate::~Slate() {
}

std::shared_ptr<Slate> Slate::getInstance() {
    if (!instance)
        instance = std::shared_ptr<Slate>(new Slate);
    return instance;
}

void Slate::EventEntry() {
    /*std::unordered_set<Window> openWindows = topWindows();
    for (Window window: openWindows) {
        ShowWindow(window, SW_MAXIMIZE);
        state.workspaces["0"]->addClient(window);
    }
    state.workspaces["0"]->getRoot()->drawTile();*/

    std::thread KeypressThread(KeypressWatcher);
    std::thread WinEventThread(WinEventWatcher);

    KeypressThread.join();
    WinEventThread.join();
}

void Slate::KeypressWatcher() {
    MSG msg;
    keyhook = SetWindowsHookEx(WH_KEYBOARD_LL, KeypressCallback, GetModuleHandle(NULL), 0);
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(keyhook);
    return;
}

void Slate::WinEventWatcher() {
    MSG msg;
    winhook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_FOCUS, NULL, WinEventCallback, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNTHREAD);
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWinEvent(winhook);
    return;
}

LRESULT CALLBACK Slate::KeypressCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    json jmsg;
    if (wParam == WM_KEYDOWN) {
        PKBDLLHOOKSTRUCT p = PKBDLLHOOKSTRUCT(lParam);

        state.alt = false;
        switch (p->vkCode) {
            case 162:
                state.ctl = true;
            case 160:
                state.shift = true;
            case VK_LWIN:
                state.meta = true;
            case VK_SPACE:
                state.space = true;
            case VK_RETURN:
                state.enter = true;
            default:
                char c = char(tolower(p->vkCode));
                if (isalnum(c)) {
                    state.keymask.insert(p->vkCode);
                }
        }

        Message::PopulateMessage(&jmsg, state);
        jmsg["Delta"] = char(tolower(p->vkCode));
        jmsg["Event"] = Message::ToClient::KEY_PRESS;
        Message::SendToClient(&jmsg);
    }
    else if (wParam == WM_KEYUP) {
        PKBDLLHOOKSTRUCT p = PKBDLLHOOKSTRUCT(lParam);

        state.alt = false;
        switch (p->vkCode) {
            case 162:
                state.ctl = false;
            case 160:
                state.shift = false;
            case VK_LWIN:
                state.meta = false;
            case VK_SPACE:
                state.space = false;
            case VK_RETURN:
                state.enter = false;
            default:
                char c = char(tolower(p->vkCode));
                if (isalnum(c)) {
                    state.keymask.erase(c);
                }
        }

        Message::PopulateMessage(&jmsg, state);
        jmsg["Event"] = Message::ToClient::KEY_RELEASE;
        Message::SendToClient(&jmsg);

    }

    return CallNextHookEx(keyhook, nCode, wParam, lParam);
}

void CALLBACK Slate::WinEventCallback(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                               LONG idObject, LONG idChild,
                               DWORD dwEventThread, DWORD dwmsEventTime) {
    if (idObject != OBJID_WINDOW && event != EVENT_OBJECT_FOCUS) {
        return;
    }
    json jmsg;

    std::unordered_set<HWND> currWindows;
    HWND window;
    switch (event) {
        case EVENT_OBJECT_CREATE:
            currWindows = topWindows();
            if (currWindows.size() > state.openWindows.size() ||
                currWindows != state.openWindows) {
                for (HWND p: currWindows) {
                    if (state.openWindows.count(p) == 0) {
                        window = p;
                        break;
                    }
                }
                Workspace *w = state.workspaces[state.workspaceID];
                if (w) {
                    w->addClient(window);
                    w->getRoot()->drawTile();
                }
                Message::PopulateMessage(&jmsg, state);
                state.openWindows = currWindows;
            }
            break;

        case EVENT_OBJECT_DESTROY:
            currWindows = topWindows();
            if (currWindows.size() < state.openWindows.size() || currWindows != state.openWindows) {
                for (HWND p: state.openWindows) {
                    if (currWindows.count(p) == 0) {
                        window = p;
                        break;
                    }
                }
                Workspace *w = state.workspaces[Workspace::clientLUT[window]];
                if (w) {
                    w->removeClient(window);
                    w->getRoot()->drawTile();
                }
                state.openWindows = currWindows;
            }
            break;
        case EVENT_OBJECT_FOCUS:
            while (GetParent(hwnd) != NULL) {
                hwnd = GetParent(hwnd);
            }
            state.focused_client = hwnd;
            state.workspaces[state.workspaceID]->setFocusedClient(hwnd);
            break;
        default:
            break;
    }
}

BOOL CALLBACK add_to_set(HWND hwnd, LPARAM lparam) {
    std::unordered_set<HWND> *set = (std::unordered_set<HWND>*)(lparam);

    //if (IsWindowVisible(hwnd)) {
        if (GetParent(hwnd) == NULL) {
            bool bNoOwner = (GetWindow(hwnd, GW_OWNER) == NULL);
            long lExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

            if ((((lExStyle & WS_EX_TOOLWINDOW) == false) && bNoOwner) ||
                ((lExStyle & WS_EX_APPWINDOW) && !bNoOwner)) {

                char sWindowText[256];
                int lReturn = GetWindowText(hwnd, sWindowText, 256);
                if (lReturn) {
                    set->insert(hwnd);
                }
            }
        }
    //}
    return true;
}

std::unordered_set<HWND> Slate::topWindows() {
    std::unordered_set<HWND> windows;
    EnumWindows(add_to_set, (LPARAM)&windows);
    return windows;
}

void Slate::EventEntryWrapper() {
    getInstance()->EventEntry();
}

zmq::socket_t& Slate::getClientPipe() {
    return fromclient;
}

const slate_state_t& Slate::getState() const {
    return state;
}

void Slate::switchToWorkspace(std::string targetName) {
    if (state.workspaceID == targetName) {
        return;
    }
    hideWorkspace();
    showWorkspace(targetName);
}

void Slate::hideWorkspace() {
    Workspace *curr = state.workspaces[state.workspaceID];
    std::unordered_set<HWND> clients = curr->clients;
    for (Window client: clients) {
        ShowWindow(client, SW_HIDE);
    }
}

void Slate::showWorkspace(std::string targetName) {
    if (state.workspaces.count(targetName) == 0) {

        // Magic 1.25 till we get a DPI fix
        state.workspaces[targetName] = new Workspace(unsigned(int(GetSystemMetrics(SM_CXSCREEN)*1.25)), unsigned(int(GetSystemMetrics(SM_CYSCREEN)*1.25)), "0");
    }
    Workspace *target = state.workspaces[targetName];
    std::unordered_set<HWND> clients = target->clients;
    for (Window client: clients) {
        ShowWindow(client, SW_SHOW);
    }
    state.workspaceID = targetName;
}
