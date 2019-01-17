sysconfdir = /etc
unitdir = $(sysconfdir)/systemd/system

UNITS = \
	pipower-up.service \
	pipower-down.service

#	powerloss-cancel.service \
#	powerloss.service \
#	powerloss-shutdown.service \
#	powerloss-shutdown.timer

all:

install:
	install -m 644 $(UNITS) $(unitdir)

activate: install
	systemctl daemon-reload
	systemctl enable --now pipower-up pipower-down
