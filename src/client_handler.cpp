#include "client_handler.h"

#include <iostream>

#include "../lib/json.h"

#include "util/message.h"
#include "layout/client.h"

using json = nlohmann::json;

void ClientHandler::Run() {
    std::shared_ptr<Slate> wm = Slate::getInstance();
    zmq::message_t reply;
    while (true) {
        wm->getClientPipe().recv(&reply);
        std::string msg(static_cast<char*>(reply.data()), reply.size()+1);
        msg[reply.size()] = '\0';
        json jmsg = json::parse(msg);
        int event = jmsg["Event"];

        switch(event) {
            case Message::FromClient::Kill_FOCUSED_CLIENT:
                XKillClient(wm->getDisplay(), wm->getState().focused_client);
                break;
            case Message::FromClient::HIDE_FOCUSED_CLIENT:
                XUnmapWindow(wm->getDisplay(), Client::clientID(wm->getDisplay(), wm->getState().focused_client));
                break;
            case Message::FromClient::UNHIDE_FOCUSED_CLIENT:
                XMapWindow(wm->getDisplay(), Client::clientID(wm->getDisplay(), wm->getState().focused_client));
                break;
            case Message::FromClient::SWITCH_WORKSPACE:
                wm->switchToWorkspace(jmsg["Workspace"]);
                break;
            case Message::FromClient::RESTYLE_ROOT: {
                auto window = jmsg["Root"];
                int styleType = window["styleType"];
                Tile* tile = Tile::restyleTile(window["id"],
                                               {window["xmin"],window["xmax"]},
                                               {window["ymin"],window["ymax"]},
                                               static_cast<StyleType>(styleType),
                                               window["style"],
                                               true);
                tile->drawTile(wm->getDisplay());
                break;
            }
            case Message::FromClient::RESTYLE_CHILDREN: {
                auto primary = jmsg["Primary"];
                int styleType = primary["styleType"];
                Tile* tile = Tile::restyleTile(primary["id"],
                                               {primary["xmin"],primary["xmax"]},
                                               {primary["ymin"],primary["ymax"]},
                                               static_cast<StyleType>(styleType),
                                               primary["style"],
                                               false);
                tile->drawTile(wm->getDisplay());


                auto secondary = jmsg["Secondary"];
                styleType = secondary["styleType"];
                tile = Tile::restyleTile(secondary["id"],
                                         {secondary["xmin"],secondary["xmax"]},
                                         {secondary["ymin"],secondary["ymax"]},
                                         static_cast<StyleType>(styleType),
                                         secondary["style"],
                                         false);
                tile->drawTile(wm->getDisplay());
                break;
            }
            default:
                break;
        }
    }
}


