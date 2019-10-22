//*******************************************************************
//
// Future Camp Project
//
// Copyright(C) 2019 Sergey Denisov.
//
// Written by Sergey Denisov aka LittleBuster(DenisovS21@gmail.com)
// Github:  https://github.com/LittleBuster
//          https://github.com/futcamp
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public Licence 3
// as published by the Free Software Foundation; either version 3
// of the Licence, or(at your option) any later version.
//
//*******************************************************************

#include <Arduino.h>
#include <functional>
#include <ESP8266WiFi.h>

#include "webserver.h"
#include "configs.h"


WebServer::WebServer(const std::shared_ptr<ESP8266WebServer> &server,
                     const std::shared_ptr<IHdk> &hdk):
                     server_(std::move(server)),
                     hdk_(std::move(hdk))
{
}

void WebServer::start()
{
    server_->on("/", std::bind(&WebServer::indexHandler, this));
    server_->on("/therm", std::bind(&WebServer::thermHandler, this));
    server_->onNotFound(std::bind(&WebServer::notFoundHandler, this));
    server_->begin();
}

void WebServer::process()
{
    server_->handleClient();
}

/*
 * Private functions
 */

String WebServer::getDevId()
{
    String id = "";

    for (auto c : WiFi.macAddress())
        if (c != ':')
            id += c;
    id.toLowerCase();

    return id;
}

void WebServer::indexHandler()
{
    String response;

    response = "{\"module\":\"therm\",\"id\":\"" + getDevId() +
               "\",\"board\":\"" + CFG_BOARD_NAME + "\"}";

    server_->send(WEB_SERVER_RESPONSE_OK, "application/json", response);
}

void WebServer::thermHandler()
{
    String  response;

    if (server_->arg("status") == "true")
    {
        hdk_->pinHigh(CFG_STATUS_LED_PIN);
    }
    if (server_->arg("status") == "false")
    {
        hdk_->pinLow(CFG_STATUS_LED_PIN);
    }
    if (server_->arg("heater") == "true")
    {
        hdk_->pinHigh(CFG_HEATER_LED_PIN);
        hdk_->switchPinMode(CFG_HEATER_PIN, HDK_PIN_MODE_OUT);
    }
    if (server_->arg("heater") == "false")
    {
        hdk_->pinLow(CFG_HEATER_LED_PIN);
        hdk_->switchPinMode(CFG_HEATER_PIN, HDK_PIN_MODE_IN);
    }

    server_->send(WEB_SERVER_RESPONSE_OK, "application/json", "{\"result\":true}");
}

void WebServer::notFoundHandler()
{
    String response;

    response = "{\"module\":\"meteo\",\"error\":\"Not found\"}";

    server_->send(WEB_SERVER_RESPONSE_OK, "application/json", response);
}
