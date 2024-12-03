#include "trans_util.h"
#include <cmath>
#include <iostream>

const double a = 6378137.0;              // WGS-84椭球体的长半轴（米）
const double inv_f = 298.257223563;      // WGS-84椭球体的扁率倒数
const double f = 1 / inv_f;
const double b = a * (1 - f);            // 短半轴
const double e_sq = f * (2 - f);         // 第一偏心率的平方

double ee = 0.00669342162296594323;  // 偏心率平方

bool out_of_china(double lon, double lat) {
    return !(lon > 73.66 && lon < 135.05 && lat > 3.86 && lat < 53.55);
}

double _transformlat(double lng, double lat) {
    double ret;
    ret = -100.0 + 2.0 * lng + 3.0 * lat + 0.2 * lat * lat + \
          0.1 * lng * lat + 0.2 * sqrt(fabs(lng));
    ret += (20.0 * sin(6.0 * lng * M_PI) + 20.0 *
                                           sin(2.0 * lng * M_PI)) * 2.0 / 3.0;
    ret += (20.0 * sin(lat * M_PI) + 40.0 *
                                     sin(lat / 3.0 * M_PI)) * 2.0 / 3.0;
    ret += (160.0 * sin(lat / 12.0 * M_PI) + 320 *
                                             sin(lat * M_PI / 30.0)) * 2.0 / 3.0;
    return ret;
}

double _transformlng(double lng, double lat) {
    double ret;
    ret = 300.0 + lng + 2.0 * lat + 0.1 * lng * lng + \
          0.1 * lng * lat + 0.1 * sqrt(fabs(lng));
    ret += (20.0 * sin(6.0 * lng * M_PI) + 20.0 *
                                              sin(2.0 * lng * M_PI)) * 2.0 / 3.0;
    ret += (20.0 * sin(lng * M_PI) + 40.0 *
                                        sin(lng / 3.0 * M_PI)) * 2.0 / 3.0;
    ret += (150.0 * sin(lng / 12.0 * M_PI) + 300.0 *
                                                sin(lng / 30.0 * M_PI)) * 2.0 / 3.0;
    return ret;
}

std::array<double, 2> wgs84_to_gcj02(double lng, double lat) {
    if (out_of_china(lng, lat)) {
        return std::array<double, 2>{lng, lat};
    }
    double dlat = _transformlat(lng - 105.0, lat - 35.0);
    double dlng = _transformlng(lng - 105.0, lat - 35.0);
    double radlat = lat / 180.0 * M_PI;
    double magic = sin(radlat);
    magic = 1 - ee * magic * magic;
    double sqrtmagic = sqrt(magic);
    dlat = (dlat * 180.0) / ((a * (1 - ee)) / (magic * sqrtmagic) * M_PI);
    dlng = (dlng * 180.0) / (a / sqrtmagic * cos(radlat) * M_PI);
    double mglat = lat + dlat;
    double mglng = lng + dlng;
    return std::array<double, 2>{mglng, mglat};
}

std::array<double, 2> gcj02_to_wgs84(double lng, double lat) {
    if (out_of_china(lng, lat)) {
        return std::array<double, 2>{lng, lat};
    }
    double dlat = _transformlat(lng - 105.0, lat - 35.0);
    double dlng = _transformlng(lng - 105.0, lat - 35.0);
    double radlat = lat / 180.0 * M_PI;
    double magic = sin(radlat);
    magic = 1 - ee * magic * magic;
    double sqrtmagic = sqrt(magic);
    dlat = (dlat * 180.0) / ((a * (1 - ee)) / (magic * sqrtmagic) * M_PI);
    dlng = (dlng * 180.0) / (a / sqrtmagic * cos(radlat) * M_PI);
    double mglat = lat + dlat;
    double mglng = lng + dlng;
    return std::array<double, 2>{lng * 2 - mglng, lat * 2 - mglat};
}

// 函数：将经纬度高度(LLA)转换为ECEF
void llaToEcef(double lat, double lon, double alt, double &x, double &y, double &z) {
    double phi = lat * M_PI / 180.0;         // 纬度转弧度
    double lambda = lon * M_PI / 180.0;      // 经度转弧度
    double s_phi = sin(phi);
    double N = a / sqrt(1.0 - e_sq * s_phi * s_phi);

    x = (N + alt) * cos(phi) * cos(lambda);
    y = (N + alt) * cos(phi) * sin(lambda);
    z = ((1 - e_sq) * N + alt) * s_phi;
}

void ecefToENU(double lat_ref, double lon_ref, double delta_X, double delta_Y, double delta_Z,
               double &E, double &N, double &U) {
    double lat_rad = lat_ref * M_PI / 180.0;
    double lon_rad = lon_ref * M_PI / 180.0;

    E = -sin(lon_rad) * delta_X + cos(lon_rad) * delta_Y;
    N = -sin(lat_rad) * cos(lon_rad) * delta_X - sin(lat_rad) * sin(lon_rad) * delta_Y + cos(lat_rad) * delta_Z;
    U = cos(lat_rad) * cos(lon_rad) * delta_X + cos(lat_rad) * sin(lon_rad) * delta_Y + sin(lat_rad) * delta_Z;
}

TransUtil::TransUtil(double ref_lon, double ref_lat, double ref_alt) : ref_lon(ref_lon), ref_lat(ref_lat),
                                                                       ref_alt(ref_alt) {
    llaToEcef(ref_lat, ref_lon, ref_alt, x_ref, y_ref, z_ref);
    std::cout << "(x,y,z)=" << x_ref << "," << y_ref << "," << z_ref << std::endl;
    double ref_E, ref_N, ref_U;
    ecefToENU(ref_lat, ref_lon, 0, 0, 0, ref_E, ref_N, ref_U);
    std::cout << "(E,N,U)=" << ref_E << "," << ref_N << "," << ref_U << std::endl;
}

std::array<double, 3> TransUtil::transToENU(double lon, double lat, double alt) const {
    double x_tgt, y_tgt, z_tgt;
    llaToEcef(lat, lon, alt, x_tgt, y_tgt, z_tgt);

    double ref_E, ref_N, ref_U;
    ecefToENU(ref_lat, ref_lon, 0, 0, 0, ref_E, ref_N, ref_U);

    double delta_X = x_tgt - x_ref;
    double delta_Y = y_tgt - y_ref;
    double delta_Z = z_tgt - z_ref;

    // 转换得到的局部平面坐标
    double E, N, U;

    ecefToENU(ref_lat, ref_lon, delta_X, delta_Y, delta_Z, E, N, U);
    return std::array<double, 3>{E - ref_E, N - ref_N, U - ref_U};
}

