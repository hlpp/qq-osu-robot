﻿/*
天气预报功能
author: hulang
*/
#ifndef WEATHERFORECAST_H
#define WEATHERFORECAST_H

#include <string>
#include "robot/utils/stringutil.h"
#include "robot/utils/http/httpget.h"  
#include "libs/jsoncpp/json.h"
#include "robot/functions/function.hpp"

using namespace std;
using namespace QQRobot;

namespace QQRobot
{
    class WeatherForecast : public Function
    {
        //API DOC: https://www.seniverse.com/doc
        const string apikey = "YK6GIIA0EB";

    public:
        WeatherForecast() {}

        handle_message_code handleMessage(Message &fromMsg, Message &toMsg)
        {
            string text = fromMsg.getContent();
            size_t locIndex = text.find(" ");
            if (locIndex == string::npos)
                return handle_message_code::syntax_error;
            string location = text.substr(locIndex + 1);

            string resStr = queryDailyString(location);
            if (resStr.length() > 0)
            {
                toMsg.setContent(resStr);
                robot->sender->sendMessage(toMsg);
                return handle_message_code::block;
            }
            return handle_message_code::ignore;
        }

    private:
        string queryDailyString(string slocation)
        {
            Json::Value result;
            int ret = get.setUrl("https://api.seniverse.com/v3/weather/daily.json")
                .addParam("location", slocation)
                .addParam("language", "en")//zh-Hans
                .addParam("unit", "c")
                .addParam("start", 0).addParam("days", 5)
                .addParam("key", apikey)
                .performForJSON(result);

            result = result["results"][0];
            Json::Value location = result["location"];
            Json::Value daily = result["daily"];

            string resultString = location["name"].asString() + " " + to_string(daily.size()) + "天天气预报：\n";
            char *dayNames[] = { "今天", "明天", "后天" };
            for (int i = 0, j = daily.size(); i < j; i++)
            {
                    Json::Value aday = daily[i];
                    char str[100];
                    sprintf_s(str, "%s 白天%s，晚间%s，气温最低%s最高%s℃，%s风（风速%skm/h，风速%s级）\n",//单位km/h（当unit=c时）
                        dayNames[i],
                        aday["text_day"].asCString(),
                        aday["text_night"].asCString(),
                        aday["low"].asCString(),
                        aday["high"].asCString(),
                        aday["wind_direction"].asCString(),
                        aday["wind_speed"].asCString(),
                        aday["wind_scale"].asCString());
                    resultString += str;
            }
            
            return resultString;
        }

        private:
            HttpGet get = HttpGet();
    };
}

#endif