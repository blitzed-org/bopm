AUTOMAKE_OPTIONS = foreign

SUBDIRS = src

data_DATA = bopm.conf.sample

EXTRA_DIST = ChangeLog contrib INSTALL README bopm.conf.sample

install-data-local:
	@if test -f $(sysconfdir)/bopm.conf ; then \
		echo "$@ will not overwrite existing $(sysconfdir)/bopm.conf"; \
	else \
		$(mkinstalldirs) $(sysconfdir); \
		echo "$(INSTALL_DATA) bopm.conf.sample $(sysconfdir)/bopm.conf"; \
		$(INSTALL_DATA) bopm.conf.sample $(sysconfdir)/bopm.conf; \
	fi
	$(mkinstalldirs) $(localstatedir)

uninstall-local:
	@if test -f $(sysconfdir)/bopm.conf ; then \
		echo "rm -f $(sysconfdir)/bopm.conf"; \
		$(RM) -f $(sysconfdir)/bopm.conf; \
	fi
