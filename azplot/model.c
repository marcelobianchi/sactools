#include "model.h"

#include "iasp91.h"

float time2distance(float target) {
    int i, ii;

    if (target < 0.0)
        return -999.0;

    if (target > (m_ts[m_len-1] - m_tp[m_len-1]))
        return -999.0;

    for(i=0, ii=1; i < (m_len-1); i++, ii++) {
        float isp = m_ts[i] - m_tp[i];
        float iisp = m_ts[ii] - m_tp[ii];


        if (target <= isp || target > iisp) continue;

        // fprintf(stderr, "%d %f %f %f %f\n", i, m_dist[i], m_tp[i], m_ts[i], isp);
        // fprintf(stderr, "%d %f %f %f %f\n", ii, m_dist[ii], m_tp[ii], m_ts[ii], iisp);

        break;
    }

    float isp = m_ts[i] - m_tp[i];
    float iisp = m_ts[ii] - m_tp[ii];

    return (target-isp)*(m_dist[ii]-m_dist[i])/(iisp - isp) + m_dist[i];
}

int model_locate(float slon, float slat, float distance, float azimuth, float *elon, float *elat) {
    /*
     * Computations relies on https://github.com/geographiclib/geographiclib-c
     */
    double esaz;
    double edlat, edlon;

    struct geod_geodesic g;

    // WGS84
    geod_init(&g, 6378137, 1/298.257223563);

    // Compute
    geod_direct(&g, (double)slat, (double)slon, (double)azimuth, (double)distance * 1000.0, &edlat, &edlon, &esaz);

    *elon = (float)edlon;
    *elat = (float)edlat;

    return 0;
}
