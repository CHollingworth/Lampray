//
// Created by charles on 27/09/23.
//
#include "lampControl.h"

Lamp::Core::lampString Lamp::Core::lampControl::getFormattedTimeAndDate() {
    std::time_t unixTimestamp = std::time(nullptr);
    std::tm timeInfo;
    localtime_r(&unixTimestamp, &timeInfo);
    std::ostringstream oss;
    oss << std::setfill('0');
    oss << std::setw(2) << timeInfo.tm_mday << '/' << std::setw(2) << (timeInfo.tm_mon + 1) << '/'
        << (timeInfo.tm_year + 1900) << ' ';
    oss << std::setw(2) << timeInfo.tm_hour << ':' << std::setw(2) << timeInfo.tm_min;
    return oss.str();
}



Lamp::Core::lampControl::lampGameSettingsDisplayHelper::lampGameSettingsDisplayHelper(
        std::string displayString, std::string stringTarget, std::string toolTip, std::string keyPath)
        : displayString(displayString), stringTarget(stringTarget), toolTip(toolTip), keyPath(keyPath) {


}
