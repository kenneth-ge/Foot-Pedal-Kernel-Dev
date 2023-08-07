#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xf8cdd757, "module_layout" },
	{ 0x16ad0eab, "hid_unregister_driver" },
	{ 0x813c9a92, "__hid_register_driver" },
	{ 0x70f592a5, "hid_hw_start" },
	{ 0x1cba9e84, "_dev_err" },
	{ 0x6176c6ee, "hid_open_report" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xbdc3f5dc, "_dev_info" },
	{ 0x27e1a049, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("hid:b0003g*v00003553p0000B001");
MODULE_ALIAS("hid:b0003g*v00001A86p0000E026");

MODULE_INFO(srcversion, "657D7ADA29B19888758351B");
MODULE_INFO(rhelversion, "8.8");
