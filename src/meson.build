batteryreport_sources = [
  'main.c',
  'batteryreport-application.c',
  'batteryreport-window.c',
]

batteryreport_deps = [
  dependency('gtk4'),
  dependency('libadwaita-1', version: '>= 1.4'),
  dependency('upower-glib'),
]

batteryreport_sources += gnome.compile_resources('batteryreport-resources',
  'batteryreport.gresource.xml',
  c_name: 'batteryreport'
)

executable('batteryreport', batteryreport_sources,
  dependencies: batteryreport_deps,
       install: true,
)
