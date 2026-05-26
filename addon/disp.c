#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

int main() {
    int fd = open("/dev/dri/card1", O_RDWR);
    drmModeRes *res = drmModeGetResources(fd);

    drmModeConnector *conn = NULL;
    for (int i = 0; i < res->count_connectors; i++) {
        conn = drmModeGetConnector(fd, res->connectors[i]);
        if (conn->connection == DRM_MODE_CONNECTED)
            break;
        drmModeFreeConnector(conn);
    }

    drmModeEncoder *enc = drmModeGetEncoder(fd, conn->encoder_id);
    drmModeModeInfo mode = conn->modes[0];

    printf("Turning display OFF\n");
    drmModeSetCrtc(fd, enc->crtc_id, 0, 0, 0, NULL, 0, NULL);

    sleep(3);

    printf("Turning display ON\n");
    drmModeSetCrtc(fd, enc->crtc_id, 0, 0, 0,
                   &conn->connector_id, 1, &mode);

    drmModeFreeEncoder(enc);
    drmModeFreeConnector(conn);
    drmModeFreeResources(res);
    close(fd);
}