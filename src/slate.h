#ifndef SLATEWM_SLATE_H
#define SLATEWM_SLATE_H

#include <windows.h>

#include <memory>
#include <unordered_set>
#include <unordered_map>

#include "zmq.hpp"

#include "layout/workspace.h"

#define ALT 64
#define CTL 37
#define META 133
#define SHIFT 50
#define SPACE 65
#define ENTER 36

typedef HWND Window;

struct slate_state_t{
    bool meta = false;
    bool shift = false;
    bool ctl = false;
    bool alt = false;
    bool space = false;
    bool enter = false;
    std::unordered_set<unsigned long> keymask;
    std::unordered_map<std::string, Workspace*> workspaces;
    std::string workspaceID = "0";
    HWND focused_client= nullptr;
};

class Slate {
private:
    slate_state_t state;

    zmq::context_t ctx;
    zmq::socket_t toclient;
    zmq::socket_t fromclient;

    Window root;

    static std::shared_ptr<Slate> instance;

    Slate();
    void EventLoop();

    void hideWorkspace();
    void showWorkspace(std::string targetName);

public:
    ~Slate();

    const slate_state_t& getState() const;
    zmq::socket_t& getClientPipe();

    static std::shared_ptr<Slate> getInstance();

    static void EventLoopWrapper();

    void switchToWorkspace(std::string targetName);

};


#endif
