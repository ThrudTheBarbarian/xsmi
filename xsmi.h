#ifndef __xsmi_device_file_header__
#define __xsmi_device_file_header__
#include <linux/compiler.h> /* __must_check */

__must_check int register_device(void); /* 0 if Ok*/
void unregister_device(void);

#endif /* ! __xsmi_device_file_header__*/