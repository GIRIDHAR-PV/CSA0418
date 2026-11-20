/*
Video-storage design comparison (illustrative model, not a real benchmark).
Compare: (A) HDD-only storage, (B) HDD archive + SSD hot-video cache.
*/
#include <stdio.h>

int main(void) {
    const double video_gb = 1.0;      /* assumed average video size */
    const int video_count = 1000;     /* example catalog size */
    const int replicas = 2;           /* one primary + one replica */
    const double hdd_rate_mb_s = 150.0;
    const double ssd_rate_mb_s = 1000.0;
    const double cache_hit_rate = 0.80; /* assumed hot-cache hit rate */
    const double startup_seek_hdd_s = 0.080;
    const double startup_seek_ssd_s = 0.005;
    const double start_chunk_mb = 8.0;  /* first chunk needed for playback */

    double raw_storage_tb = video_gb * video_count / 1000.0;
    double replicated_tb = raw_storage_tb * replicas;
    double hdd_start_s = startup_seek_hdd_s + start_chunk_mb / hdd_rate_mb_s;
    double ssd_start_s = startup_seek_ssd_s + start_chunk_mb / ssd_rate_mb_s;
    double cache_design_avg_s = cache_hit_rate * ssd_start_s
                             + (1.0 - cache_hit_rate) * hdd_start_s;

    printf("Assumptions: %d videos, %.2f GB average, %d replicas\n",
           video_count, video_gb, replicas);
    printf("Catalog raw storage: %.2f TB; with replicas: %.2f TB\n",
           raw_storage_tb, replicated_tb);
    printf("HDD-only estimated first-chunk start: %.3f s\n", hdd_start_s);
    printf("SSD-cache hit estimated first-chunk start: %.3f s\n", ssd_start_s);
    printf("Hybrid average estimated start (%.0f%% hit rate): %.3f s\n",
           cache_hit_rate * 100.0, cache_design_avg_s);
    printf("Sequential full-video transfer (1 GB): HDD %.2f s; SSD %.2f s\n",
           video_gb * 1000.0 / hdd_rate_mb_s,
           video_gb * 1000.0 / ssd_rate_mb_s);
    return 0;
}
