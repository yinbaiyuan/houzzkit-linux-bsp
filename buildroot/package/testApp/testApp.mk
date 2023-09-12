################################################################################
# 
# testApp
#
################################################################################

TESTAPP_VERSION = 1.0
TESTAPP_SITE = ${TOPDIR}/../app/testApp
TESTAPP_SITE_METHOD = local

TESTAPP_LICENSE = Apache V2.0
TESTAPP_LICENSE_FILES = NOTICE

define TESTAPP_CONFIGURE_CMDS
    cd $(@D); $(TARGET_MAKE_ENV) $(HOST_DIR)/bin/qmake
endef

define TESTAPP_BUILD_CMDS
    $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)
endef

define TESTAPP_INSTALL_TARGET_CMDS
        mkdir -p $(TARGET_DIR)/usr/share/applications $(TARGET_DIR)/usr/share/icon
        $(INSTALL) -D -m 0644 $(@D)/icon_testApp.png $(TARGET_DIR)/usr/share/icon/
        $(INSTALL) -D -m 0755 $(@D)/testApp $(TARGET_DIR)/usr/bin/
        $(INSTALL) -D -m 0755 $(@D)/testApp.desktop $(TARGET_DIR)/usr/share/applications/
	$(INSTALL) -D -m 0644 $(@D)/config.ini $(TARGET_DIR)/etc/
endef

$(eval $(generic-package))
