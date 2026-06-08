project('retro-book-gardens', 'cpp',
  default_options: ['warning_level=1', 'buildtype=debug'])

sdl3 = dependency('sdl3')

link_args = []
if host_machine.system() == 'darwin'
  link_args += ['-Wl,-undefined,dynamic_lookup']
endif

demos = [
  'bitmapmaze',
  'bitmapmaze2',
  'polymaze',
  'heightmap',
  'heightmap2',
  'lightmap',
  'lightmap2',
  'wiremaze',
  'raytrace',
  'floor',
  'wallspoly',
  'walls',
  'raycast',
  'raycastlight',
  'gardens',
]

foreach demo : demos
  executable(demo, 'src' / demo + '.cpp',
    dependencies: sdl3,
    link_args: link_args)
endforeach
