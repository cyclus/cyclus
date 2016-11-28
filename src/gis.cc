#include "gis.h"

namespace cyclus {

GIS::GIS(){
    latitude = 0.0;
    longitude = 0.0;
}

GIS::GIS(float lat, float lon){
    string temp_lat = to_string(lat);
    if (temp_lat.length() != 8){
        throw std::invalid_argument("Please format the latitude properly. Proper format is shown in the documentation");
    }
    string temp_lon = to_string(lon);
    if (temp_lon.length() != 9){
        throw std::invalid_argument("Please format the longitude properly. Proper format is shown in the documentation");
    }
    latitude = lat;
    longitude = lon;
}

GIS::~GIS(){
}

float GIS::get_latitude_decimal() const{
    string temp_lat = to_string(latitude);
    float lat = std::stof(temp_lat.substr(0,2)) + std::stof(temp_lat.substr(2,2))/60 + std::stof(temp_lat.substr(4,4))/3600;
    temp_lat = to_string(lat);
    lat = std::stof(temp_lat.substr(0,8));
    return lat;
}

float GIS::get_latitude_degrees() const{
    return latitude;
}

float GIS::get_longitude_decimal() const{
    string temp_lon = to_string(longitude);
    float lon = std::stof(temp_lon.substr(0,2)) + std::stof(temp_lon.substr(2,2))/60 + std::stof(temp_lon.substr(4,4))/3600;
    temp_lon = to_string(lon);
    lon = std::stof(temp_lon.substr(0,8));
    return lon;
}

float GIS::get_longitude_degrees() const{
    return longitude;
}

void GIS::set_latitude_decimal(float lat){
}

void GIS::set_latitude_degrees(float lat){
    latitude = lat;
}

void GIS::set_longitude_decimal(float lon){
}

void GIS::set_longitude_degrees(float lon){
    longitude = lon;
}

double GIS::get_distance(GIS target) const{
}

GIS[] GIS::nearby(double range) const{
}

GIS[] GIS::nearby(const GIS *reference, double range) const{
}

string GIS::toString() const{
}

double GIS::sort(double &list[]){
}

}
