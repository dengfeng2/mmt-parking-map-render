#ifndef TRANS_UTIL_H
#define TRANS_UTIL_H

#include <array>

std::array<double, 2> wgs84_to_gcj02(double lon, double lat);

std::array<double, 2> gcj02_to_wgs84(double lon, double lat);

class TransUtil {

  public:
    TransUtil(double ref_lon, double ref_lat, double ref_alt);

    [[nodiscard]] std::array<double, 3> transToENU(double lon, double lat, double alt) const;
  private:
    double ref_lon, ref_lat, ref_alt;
    double x_ref, y_ref, z_ref;
};

#endif //TRANS_UTIL_H
