#include <iostream>
#include "workspace.h"
#include "client.h"

int Workspace::default_count = 1;

Workspace::Workspace() {
    name = std::to_string(default_count);
    default_count++;
    focused_client = -1;
}

Workspace::Workspace(std::string name) {
    this->name = name;
    focused_client = -1;
}

void Workspace::addClient(Display* d, Window w) {
    Window top = Client::clientID(d, w);
    std::cout << "Added " << top << std::endl;
    clients.insert(top);
}

void Workspace::switchTo(Slate *wm, std::string targetName) {
    if (wm->state.workspaceID == targetName) {
        return;
    }
    hideWorkspace(wm);
    showWorkspace(wm, targetName);
}

void Workspace::hideWorkspace(Slate *wm) {
    Workspace *curr = wm->state.workspaces[wm->state.workspaceID];
    std::unordered_set<Window> clients = curr->clients;
    for (Window client: clients) {
        XUnmapWindow(wm->display, client);
    }
}

void Workspace::showWorkspace(Slate *wm, std::string targetName) {
    if (wm->state.workspaces.count(targetName) == 0) {
        wm->state.workspaces[targetName] = new Workspace(targetName);
    }
    Workspace *target = wm->state.workspaces[targetName];
    std::unordered_set<Window> clients = target->clients;
    for (Window client: clients) {
        XMapWindow(wm->display, client);
    }
    wm->state.workspaceID = targetName;
}