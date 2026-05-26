// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2019 Radxa Limited
 * Copyright (c) 2022 Edgeble AI Technologies Pvt. Ltd.
 *
 * Author:
 * - Jagan Teki <jagan@amarulasolutions.com>
 * - Stephen Chen <stephen@radxa.com>
 */
#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/regulator/consumer.h>
#include <linux/module.h>
#include <linux/of.h>

#include <drm/display/drm_dsc.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

#include <video/mipi_display.h>

/** CONFIG **/
#define _AMOLED_REFRESH_RATE 60
#define _AMOLED_HDISPLAY 1080
#define _AMOLED_HFP 72
#define _AMOLED_HSYNC 112
#define _AMOLED_HBP 184
#define _AMOLED_VDISPLAY 1240
#define _AMOLED_VFP 3
#define _AMOLED_VSYNC 10
#define _AMOLED_VBP 33
#define _AMOLED_BACKLIGHT_NAME "pibrick-backlight"


/** START DRIVER **/
struct visionox_vtdr6110 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
	struct regulator_bulk_data *supplies;
};

static const struct regulator_bulk_data visionox_vtdr6110_supplies[] = {
	{ .supply = "vddio" },
	{ .supply = "vci" },
	{ .supply = "vdd" },
};

static inline struct visionox_vtdr6110 *to_visionox_vtdr6110(struct drm_panel *panel)
{
	return container_of(panel, struct visionox_vtdr6110, panel);
}

static void visionox_vtdr6110_reset(struct visionox_vtdr6110 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
}

static int visionox_vtdr6110_on(struct visionox_vtdr6110 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = dsi };

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0x35, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0x53, 0x20);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0x51, 0x03, 0xFF);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0x6F, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xF0, 0xAA, 0x10);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB0, 0x01, 0x36, 0x87, 0x00, 0x87);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB1, 0x01, 0x2D, 0x00, 0x0E, 0x00, 0x0E, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB2, 0x01, 0x2D, 0x00, 0x0E, 0x00, 0x0E, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB6, 0x40);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB7, 0x84);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xBD, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC1, 0x00, 0x05, 0x00, 0x05);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC3, 0x11, 0x55);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC5, 0x10);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xCF, 0x0B, 0x0E, 0x0E);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xD0, 0x80, 0x13, 0x50, 0x14, 0x14, 0x00, 0x29, 0x2D, 0x19, 0x00, 0x00, 0x00, 0x2D, 0x19, 0x33, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC0, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xF0, 0xAA, 0x12);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB1, 0x33);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xb5, 0xA7, 0xA7);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xb6, 0x87, 0x87);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB7, 0x30, 0x30);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB8, 0x20, 0x20);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB9, 0x38, 0x37, 0x37);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xBA, 0x38, 0x1B);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xD2, 0x0F, 0x00, 0x00, 0x02, 0x04, 0x02, 0x04, 0x06, 0x00, 0x00, 0x02, 0x02, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC4, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xCF, 0x03, 0x28, 0x00, 0x11);

	//mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xF0, 0xAA, 0x11);
	//mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC2, 0x10, 0x00, 0x60, 0x02, 0x01, 0x11, 0x22, 0x33, 0x43, 0x53);
	//mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC3, 0x00, 0x00, 0x00, 0x2A, 0x00, 0xAA, 0x01, 0xFF, 0x02, 0xFF, 0x03, 0xFF);
	//mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC4, 0x06, 0x20, 0x05, 0xF6, 0x05, 0x5A, 0x03, 0x87, 0x01, 0xE8, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x0A, 0x80, 0x40, 0x80, 0x40);

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xF0, 0xAA, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC2, 0x10, 0x00, 0x60, 0x02, 0x01, 0x11, 0x21, 0x31, 0x41, 0x51);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC3, 0x00, 0x00, 0x00, 0x2A, 0x00, 0xAA, 0x01, 0xFF, 0x02, 0xFF, 0x03, 0xFF);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC4, 0x04, 0xD8, 0x04, 0x4C, 0x03, 0xF8, 0x01, 0x8C, 0x01, 0x20, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x0A, 0x80, 0x20, 0x80, 0x20);

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xF0, 0xAA, 0x13);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xBF, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB0, 0x00, 0x00, 0x00, 0x46, 0x00, 0x93, 0x00, 0xA6, 0x00, 0xB7, 0x00, 0xD6, 0x00, 0xF0, 0x01, 0x07);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB1, 0x01, 0x18, 0x01, 0x29, 0x01, 0x38, 0x01, 0x46, 0x01, 0x53, 0x01, 0x6B, 0x01, 0x80, 0x01, 0x94);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB2, 0x01, 0xA6, 0x01, 0xB6, 0x01, 0xD6, 0x01, 0xF3, 0x02, 0x0E, 0x02, 0x42, 0x02, 0x73, 0x02, 0xA7);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB3, 0x02, 0xD8);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB4, 0x00, 0x00, 0x00, 0x45, 0x00, 0x91, 0x00, 0x99, 0x00, 0xA5, 0x00, 0xBF, 0x00, 0xD5, 0x00, 0xEB);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB5, 0x00, 0xFB, 0x01, 0x0B, 0x01, 0x1B, 0x01, 0x28, 0x01, 0x34, 0x01, 0x4A, 0x01, 0x61, 0x01, 0x74);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB6, 0x01, 0x85, 0x01, 0x93, 0x01, 0xB2, 0x01, 0xCD, 0x01, 0xE5, 0x02, 0x16, 0x02, 0x42, 0x02, 0x70);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB7, 0x02, 0x9B);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB8, 0x00, 0x00, 0x00, 0x54, 0x00, 0xAD, 0x00, 0xC2, 0x00, 0xD4, 0x00, 0xF7, 0x01, 0x10, 0x01, 0x25);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB9, 0x01, 0x35, 0x01, 0x47, 0x01, 0x58, 0x01, 0x69, 0x01, 0x77, 0x01, 0x91, 0x01, 0xA9, 0x01, 0xBF);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xBA, 0x01, 0xD3, 0x01, 0xE4, 0x02, 0x06, 0x02, 0x26, 0x02, 0x42, 0x02, 0x7A, 0x02, 0xAE, 0x02, 0xE5);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xBB, 0x03, 0x19);

	//mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC0, 0x10, 0x11, 0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x43, 0x44, 0x55, 0x55);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xD2, 0x14, 0x92, 0x24);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC3, 0x00, 0x23, 0x61, 0x45, 0x21, 0x43, 0x65);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xD4, 0x00, 0xFF);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xD5, 0x00, 0x0C, 0x00, 0x1C, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xD6, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xF0, 0xAA, 0x14);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB2, 0x03, 0x33);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB4, 0x04, 0x11, 0x11, 0x07);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB5, 0x00, 0x6D, 0x6D, 0x06, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB9, 0x00, 0x00, 0x08, 0x32);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xBC, 0x10, 0x00, 0x00, 0x06, 0x11, 0x30, 0x88);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xBE, 0x10, 0x10, 0x00, 0x08, 0x22, 0x30, 0x86);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xF0, 0xAA, 0x15);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xb1, 0x33, 0x41);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB2, 0x63);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB3, 0x84, 0x23, 0x30);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB4, 0x83, 0x04);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB5, 0x83, 0x04);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB6, 0x83, 0x00, 0x04);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB7, 0x83, 0x00, 0x04);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xBC, 0x72);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xBD, 0x48);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xBE, 0x11, 0x00, 0x00, 0x22, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xBF, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC0, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC1, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xC3, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xF0, 0xAA, 0x16);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB0, 0x25, 0x21, 0x20, 0x0B);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB1, 0x0A, 0x04, 0x13, 0x12);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB2, 0x25, 0x25, 0x25, 0x25);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB3, 0x25, 0x25, 0x25, 0x25);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB4, 0x25, 0x25, 0x25, 0x25);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB5, 0x25, 0x25, 0x25, 0x25);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB6, 0x25, 0x25, 0x25, 0x25);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB7, 0x25, 0x25, 0x25, 0x25);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB8, 0x12, 0x13, 0x00, 0x0A);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB9, 0x0B, 0x20, 0x21, 0x25);

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xf0, 0xaa, 0x17);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xb0, 0x02, 0x30, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xdc, 0xc0, 0x0d, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xb1, 0x0c, 0xe0, 0xff, 0x1f, 0xfc, 0x00, 0x20, 0x20, 0x00, 0x20, 0x20, 0x00, 0x20, 0x20, 0x00, 0x20);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xB2, 0x00, 0x00, 0x04, 0x00, 0x80, 0xC0, 0x80, 0x00, 0x14, 0x14, 0x00, 0x14, 0x14, 0x00, 0x14, 0x14);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xb3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0xc0, 0x40, 0x0d, 0x40, 0xa0, 0xc0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xb4, 0x67, 0x67, 0x67, 0x67, 0x67, 0x67, 0x3f);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xb5, 0x23, 0x61, 0x45, 0x12, 0x53, 0x64);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0xe5, 0x11);


	// mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0x11);
	// mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0x29);

	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 60);

	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 10);

	return dsi_ctx.accum_err;
}

static void visionox_vtdr6110_off(struct visionox_vtdr6110 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = dsi };

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 10);

	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 60);
}

static int visionox_vtdr6110_prepare(struct drm_panel *panel)
{
	struct visionox_vtdr6110 *ctx = to_visionox_vtdr6110(panel);
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(visionox_vtdr6110_supplies),
				    ctx->supplies);
	if (ret < 0)
		return ret;

	visionox_vtdr6110_reset(ctx);

	ret = visionox_vtdr6110_on(ctx);
	if (ret < 0) {
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(visionox_vtdr6110_supplies),
				       ctx->supplies);
		return ret;
	}

	return 0;
}

static int visionox_vtdr6110_unprepare(struct drm_panel *panel)
{
	struct visionox_vtdr6110 *ctx = to_visionox_vtdr6110(panel);

	visionox_vtdr6110_off(ctx);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	regulator_bulk_disable(ARRAY_SIZE(visionox_vtdr6110_supplies),
			       ctx->supplies);

	return 0;
}

static const struct drm_display_mode visionox_vtdr6110_mode = {
	.clock = (_AMOLED_HDISPLAY + _AMOLED_HFP + _AMOLED_HSYNC + _AMOLED_HBP) * (_AMOLED_VDISPLAY + _AMOLED_VFP + _AMOLED_VSYNC + _AMOLED_VBP) * _AMOLED_REFRESH_RATE / 1000,
	.hdisplay = _AMOLED_HDISPLAY,
	.hsync_start = _AMOLED_HDISPLAY + _AMOLED_HFP,
	.hsync_end = _AMOLED_HDISPLAY + _AMOLED_HFP + _AMOLED_HSYNC,
	.htotal = _AMOLED_HDISPLAY + _AMOLED_HFP + _AMOLED_HSYNC + _AMOLED_HBP,
	.vdisplay = _AMOLED_VDISPLAY,
	.vsync_start = _AMOLED_VDISPLAY + _AMOLED_VFP,
	.vsync_end = _AMOLED_VDISPLAY + _AMOLED_VFP + _AMOLED_VSYNC,
	.vtotal = _AMOLED_VDISPLAY + _AMOLED_VFP + _AMOLED_VSYNC + _AMOLED_VBP,
	.width_mm = 65,
	.height_mm = 75,
};

static int visionox_vtdr6110_get_modes(struct drm_panel *panel,
				       struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &visionox_vtdr6110_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);
	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs visionox_vtdr6110_panel_funcs = {
	.prepare = visionox_vtdr6110_prepare,
	.unprepare = visionox_vtdr6110_unprepare,
	.get_modes = visionox_vtdr6110_get_modes,
};

static int visionox_vtdr6110_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness = backlight_get_brightness(bl);

	return mipi_dsi_dcs_set_display_brightness_large(dsi, brightness);
}

static const struct backlight_ops visionox_vtdr6110_bl_ops = {
	.update_status = visionox_vtdr6110_bl_update_status,
};

static struct backlight_device *
visionox_vtdr6110_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 1023,
		.max_brightness = 1023,
	};

	return devm_backlight_device_register(dev, _AMOLED_BACKLIGHT_NAME /*dev_name(dev)*/, dev, dsi,
					      &visionox_vtdr6110_bl_ops, &props);
}

static int visionox_vtdr6110_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct visionox_vtdr6110 *ctx;
	int ret;

	// New Linux Kernel Allocation
	// ctx = devm_drm_panel_alloc(dev, struct visionox_vtdr6110, panel,
	// 			   &visionox_vtdr6110_panel_funcs,
	// 			   DRM_MODE_CONNECTOR_DSI);
	// if (IS_ERR(ctx))
	// 	return PTR_ERR(ctx);

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ret = devm_regulator_bulk_get_const(&dsi->dev,
					    ARRAY_SIZE(visionox_vtdr6110_supplies),
					    visionox_vtdr6110_supplies,
					    &ctx->supplies);
	if (ret < 0)
		return ret;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	drm_panel_init(&ctx->panel, &dsi->dev, &visionox_vtdr6110_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;
	ctx->panel.prepare_prev_first = true;

	ctx->panel.backlight = visionox_vtdr6110_create_backlight(dsi);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "Failed to create backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		drm_panel_remove(&ctx->panel);
		return ret;
	}

	return 0;
}

static void visionox_vtdr6110_remove(struct mipi_dsi_device *dsi)
{
	struct visionox_vtdr6110 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id visionox_vtdr6110_of_match[] = {
	{ .compatible = "pibrick,amoled" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, visionox_vtdr6110_of_match);

static struct mipi_dsi_driver visionox_vtdr6110_driver = {
	.probe = visionox_vtdr6110_probe,
	.remove = visionox_vtdr6110_remove,
	.driver = {
		.name = "panel-pibrick",
		.of_match_table = visionox_vtdr6110_of_match,
	},
};
module_mipi_dsi_driver(visionox_vtdr6110_driver);

MODULE_AUTHOR("me@amarullz.com");
MODULE_DESCRIPTION("piBrick Amoled and XGA Panel Driver");
MODULE_LICENSE("GPL");
