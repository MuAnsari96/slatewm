#ifndef SLATEWM_WORKSPACE_H
#define SLATEWM_WORKSPACE_H
#include <X11/Xlib.h>
#include <unordered_set>
#include <string>
#include "../slate.h"

class Slate;

class Workspace {
public:
    std::string name;
    unsigned int focused_client;
    std::unordered_set<Window> clients;

    Workspace();
    Workspace(std::string name);
    void addClient(Display* d, Window w);

    static int default_count;
    static void switchTo(Slate* wm, std::string targetName);

private:
    static void hideWorkspace(Slate* wm);
    static void showWorkspace(Slate* wm, std::string targetName);

};


#endif //SLATEWM_WORKSPACE_H
