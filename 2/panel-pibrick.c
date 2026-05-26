/*
 * Copyright (c) 2026 amarullz.com
 *
 * Author:
 * - Ahmad Amarullah <amarullz@gmail.com>
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
#define _AMOLED_REFRESH_RATE 60.768
#define _AMOLED_HDISPLAY 1080
#define _AMOLED_HFP 40
#define _AMOLED_HSYNC 8
#define _AMOLED_HBP 40
#define _AMOLED_VDISPLAY 1240
#define _AMOLED_VFP 10
#define _AMOLED_VSYNC 8
#define _AMOLED_VBP 10
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
	
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx,0xFD,0x00);
    mipi_dsi_dcs_write_seq_multi(&dsi_ctx,0xF0,0xA5,0x0F,0xF0);
    mipi_dsi_dcs_write_seq_multi(&dsi_ctx,0xFD,0x00);
    mipi_dsi_dcs_write_seq_multi(&dsi_ctx,0xE3,0xF3,0x00,0x00,0x00,0x00,0xEC,0x5E,0x10,0x32,0x54,0x76,0x98,0xBA,0xDC,0xFE,0x08,0x10,0x18,0x20,0x28,0x30,0x38,0x40,0x3F,0x20,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    mipi_dsi_dcs_write_seq_multi(&dsi_ctx,0xFD,0x40);
    mipi_dsi_dcs_write_seq_multi(&dsi_ctx,0xE3,0x00,0x00,0x00,0x00,0xEC,0x5E,0x00,0x03,0x06,0x09,0x0B,0x0D,0x0F,0x11);
    mipi_dsi_dcs_write_seq_multi(&dsi_ctx,0xFD,0xC0);
    mipi_dsi_dcs_write_seq_multi(&dsi_ctx,0xF5,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    mipi_dsi_dcs_write_seq_multi(&dsi_ctx,0x51,0x07,0xFF);

	// mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0x11);
	// mipi_dsi_dcs_write_seq_multi(&dsi_ctx,  0x29);

	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 250);

	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 50);

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
	/*.clock = (_AMOLED_HDISPLAY + _AMOLED_HFP + _AMOLED_HSYNC + _AMOLED_HBP) * (_AMOLED_VDISPLAY + _AMOLED_VFP + _AMOLED_VSYNC + _AMOLED_VBP) * _AMOLED_REFRESH_RATE / 1000,*/
	.clock = 90000,
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