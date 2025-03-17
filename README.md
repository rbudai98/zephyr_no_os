# zephyr_no_os
Interfacing solution between Zephyr-RTOS and No-Os driver stack


### NOTES:

## No-OS changes:
* Branch: https://github.com/analogdevicesinc/no-OS/tree/zephyr_api
* Needed `zephyr` directory in the root of the repository, including `module.yml`
* `zephyr/CMakeLists.txt`: including in zephyr build system the neccessary files:

	* Utils:
		* ${NOOS_DIR}/util/no_os_alloc.c
    	* ${NOOS_DIR}/util/no_os_mutex.c
    	* ${NOOS_DIR}/util/no_os_util.c

	* Peripherals generic api:
		* ${NOOS_DIR}/drivers/api/no_os_i2c.c
    	* ${NOOS_DIR}/drivers/api/no_os_spi.c
    	* ${NOOS_DIR}/drivers/api/no_os_gpio.c

	* Zephyr api implementation:
		* ${NOOS_DIR}/drivers/platform/zephyr/zephyr_i2c.c
    	* ${NOOS_DIR}/drivers/platform/zephyr/zephyr_spi.c
    	* ${NOOS_DIR}/drivers/platform/zephyr/zephyr_gpio.c
    	* ${NOOS_DIR}/drivers/platform/zephyr/zephyr_delay.c
	
	* Any additional desired driver files

* Changes and files includes in this CMakeFiles might have to be moved in `hal_adi`

## Zephyr changes:
* Adding no-os with neccessary changes as module in west build system:
```console
	diff --git a/submanifests/optional.yaml b/submanifests/optional.yaml
	index b3d961d2670..9c5f00590fe 100644
	--- a/submanifests/optional.yaml
	+++ b/submanifests/optional.yaml
	@@ -71,4 +71,9 @@ manifest:
		remote: upstream
		groups:
			- optional
	+    - name: no-os
	+      url: https://github.com/analogdevicesinc/no-OS.git
	+      revision: zephyr_i2c
	+      path: modules/lib/no-os
	+      
	# zephyr-keep-sorted-stop
```
* Clone no-os files: `west update`

## Example projects:

### Example_adxl355
```console
.
├── CMakeLists.txt
├── dts
│   └── bindings
│       └── adxl355_no_os_test.yaml
├── overlay
│   └── adxl355.overlay
├── prj.conf
├── sample.yaml
└── src
    └── main.c
```
### Example_gpio
```console
.
├── CMakeLists.txt
├── overlay
│   └── gpio.overlay
├── prj.conf
├── sample.yaml
└── src
    └── main.c
```
### Example_max31875
```console
.
├── CMakeLists.txt
├── dts
│   └── bindings
│       └── max31875_no_os_test.yaml
├── overlay
│   └── max31875.overlay
├── prj.conf
├── sample.yaml
└── src
    └── main.c
```